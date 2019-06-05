#include "TRManager.h"

TRManager::TRManager(std::string str):tr{str},fd{-1},AUX{0},MD{radioWMode::unknown},Buff{0},deviceReady{0}
    ,databuffer{},msgbuffer{},sttngbuffer{},currentSetting{}
{
}

TRManager::~TRManager()
{
}

int TRManager::connect(std::string str)
{
    fd = tr.connect(str);
    return fd;
}

void TRManager::writeStr(std::string str)
{
    if(fd == -1)
        throw Error("4");
    if(fd != -1)
        write(fd,str.c_str(),str.size());
}

void TRManager::readDevice()
{
    if(fd == -1)
        throw Error("5");
    char tmp[600]{};
    int ctr = 0;

    char ch{};

    while(read(fd,&ch,1))
    {
        //std::cout<<ch;  /////////////////////////////***************************
        //tmp.push_back(ch);
        tmp[ctr++] = ch;

        if(strcmp(tmp,"#S,") == 0)
        {
            read(fd,&tmp[3],14);
            std::vector<char> tmpv{};
            std::lock_guard<std::mutex> lg{mu};
            for(int i = 0; i<17; i++)
                tmpv.push_back(tmp[i]);
            sttngbuffer.push(tmpv);
            return;
        }

        if(ch == '\0')
        break;
    }

    std::string tmps{tmp};
        {
            if(tmps.find("#D") != std::string::npos)
            {
                std::lock_guard<std::mutex> lg{mu};
                databuffer.push(tmps);
            }
            if(tmps.find("#MS") != std::string::npos)
            {
                std::lock_guard<std::mutex> lg{mu};
                msgbuffer.push(tmps);
            }
            //if(tmp.find("#S") != std::string::npos)
            //{
             //   std::lock_guard<std::mutex> lg{mu};
             //   sttngbuffer.push(tmp);
            //}
            react(tmps.c_str());
            tmps.clear();
        }
}

std::string TRManager::readBuffer(const char ch)
{
    std::string tmp{};
    std::lock_guard<std::mutex> lg{mu};

    if(ch == 'd' && !databuffer.empty())
    {
        tmp = databuffer.front();
        databuffer.pop();
    }
    if(ch == 'm' && !msgbuffer.empty())
    {
        tmp = msgbuffer.front();
        msgbuffer.pop();
    }
    if(ch == 's' && !sttngbuffer.empty())
    {
        tmp = sttngbuffer.front();
        sttngbuffer.pop();
    }
    return tmp;
}



void TRManager::init()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    writeStr("SS");
    writeStr("#G,AUX|");
    writeStr("#G,B|");
    writeStr("#G,MD");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    deviceReady = true;
}


bool TRManager::isEmpty(const char ch)
{
    if(ch == 'd')
        return databuffer.empty();
    if(ch == 'm')
        return msgbuffer.empty();
    if(ch == 's')
        return sttngbuffer.empty();
    return false;
}


void TRManager::react(std::string str)
{
    if(str.compare("#MS,ND|\r\n")==0)
        writeStr("#R,A|");

    if(str.compare("#MS,NS|\r\n")==0)
        writeStr("#R,A|");

    if(str.compare("#MS,BF|\r\n")==0)
        writeStr("#R,A|");

    if(str.compare("#MS,AH|\r\n")==0)
        AUX = true;
    if(str.compare("#MS,AL|\r\n")==0)
        AUX = false;
    if(str.compare("#MS,AF|\r\n")==0)
        AUX = false;
    if(str.compare("#MS,AR|\r\n")==0)
        AUX = true;

    if(str.compare("#MS,B0|\r\n")==0)
        Buff = 0;
    if(str.compare("#MS,B1|\r\n")==0)
        Buff = 1;
    if(str.compare("#MS,B2|\r\n")==0)
        Buff = 2;
    if(str.compare("#MS,B3|\r\n")==0)
        Buff = 3;
    if(str.compare("#MS,B4|\r\n")==0)
        Buff = 4;
    if(str.compare("#MS,B5|\r\n")==0)
        Buff = 5;
    if(str.compare("#MS,B6|\r\n")==0)
        Buff = 6;
    if(str.compare("#MS,B7|\r\n")==0)
        Buff = 7;
    if(str.compare("#MS,B8|\r\n")==0)
        Buff = 8;
    if(str.compare("#MS,B9|\r\n")==0)
        Buff = 9;

    if(str.compare("#MS,M0|\r\n")==0)
        MD = radioWMode::zero;
    if(str.compare("#MS,M1|\r\n")==0)
        MD = radioWMode::one;
    if(str.compare("#MS,M2|\r\n")==0)
        MD = radioWMode::two;
    if(str.compare("#MS,M3|\r\n")==0)
        MD = radioWMode::three;


}


bool TRManager::getAUX()
{
    writeStr("#G,AUX|");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return AUX;
}

int TRManager::getBuffNum()
{
    writeStr("#G,B|");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return Buff;
}

radioWMode TRManager::getMode()
{
    writeStr("#G,MD|");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return MD;
}

void TRManager::setMode(radioWMode rm)
{
    if(rm == radioWMode::zero)
    {
        writeStr("#CM,M0|");
    }
    if(rm == radioWMode::one)
    {
        writeStr("#CM,M1|");
    }
    if(rm == radioWMode::two)
    {
        writeStr("#CM,M2|");
    }
    if(rm == radioWMode::three)
    {
        writeStr("#CM,M3|");
    }
    MD = rm;
}


void TRManager::getSetting()
{
    radioWMode mode = MD;

    std::string tmp{};

    tmp += "#CS,";
    tmp.push_back(0xc1);
    tmp.push_back(0xc1);
    tmp.push_back(0xc1);
    tmp += "|";

    if(MD != radioWMode::three)
    {
        setMode(radioWMode::three);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    writeStr(tmp);

    if(mode != radioWMode::three)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            setMode(mode);
        }
}


void TRManager::transmit(std::string str, radioWMode rm)
{

    /// address and channel must be added , is AUX measured??
    if(rm == radioWMode::three || rm == radioWMode::two)
        return;

    std::string tmp{};
    tmp += "#T,";
    tmp.push_back(0xFF);
    tmp.push_back(0xFF);
    tmp += str;
    tmp += "|";

    radioWMode mode = MD;
    if(MD != rm) /// first go to desired mode!! otherwise causes high latency!!
    {
        setMode(rm);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    writeStr(tmp);
    if(mode != rm)
        setMode(mode);
}


void TRManager::setAddr(address addr)
{
    currentSetting.addr.ADDH = addr.ADDH;
    currentSetting.addr.ADDL = addr.ADDL;
}


void TRManager::setChannel(char commChannel)
{
    currentSetting.CHAN = commChannel;
    currentSetting.CHAN &= 0b00011111;
}


void TRManager::setTransmissionPower(radioTransmissionPower rtp)
{
    if(rtp == radioTransmissionPower::_30dBm)
    {
        currentSetting.OPTN &= 0b11111100;
    }
    if(rtp == radioTransmissionPower::_27dBm)
    {
        currentSetting.OPTN &= 0b11111100;
        currentSetting.OPTN |= 0b00000001;
    }
    if(rtp == radioTransmissionPower::_24dBm)
    {
        currentSetting.OPTN &= 0b11111100;
        currentSetting.OPTN |= 0b00000010;
    }
    if(rtp == radioTransmissionPower::_21dBm)
    {
        currentSetting.OPTN &= 0b11111100;
        currentSetting.OPTN |= 0b00000011;
    }
}

void TRManager::setTransmissionMode(radioTransmissionMode rtm)
{
    if(rtm == radioTransmissionMode::Fixed)
    {
        currentSetting.OPTN &= 0b01111111;
        currentSetting.OPTN |= 0b10000000;
    }
    if(rtm == radioTransmissionMode::Transparent)
    {
        currentSetting.OPTN &= 0b01111111;
        currentSetting.OPTN |= 0b00000000;
    }
}

void TRManager::setDateRate(radioDataRate rdr)
{
    if(rdr == radioDataRate::_1k2)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111000;
    }
    if(rdr == radioDataRate::_2k4)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111001;
    }
    if(rdr == radioDataRate::_4k8)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111010;
    }
    if(rdr == radioDataRate::_9k6)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111011;
    }
    if(rdr == radioDataRate::_19k2)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111100;
    }
    if(rdr == radioDataRate::_38k4)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111101;
    }
    if(rdr == radioDataRate::_50k)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111110;
    }
    if(rdr == radioDataRate::_70k)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b11111111;
    }
}

void TRManager::setUARTBaudRate(radioUARTBaudRate rubr)
{
    if(rubr == radioUARTBaudRate::_115200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11111111;
    }
    if(rubr == radioUARTBaudRate::_57600bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11110111;
    }
    if(rubr == radioUARTBaudRate::_38400bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11101111;
    }
    if(rubr == radioUARTBaudRate::_19200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11100111;
    }
    if(rubr == radioUARTBaudRate::_9600bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11011111;
    }
    if(rubr == radioUARTBaudRate::_4800bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11010111;
    }
    if(rubr == radioUARTBaudRate::_2400bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11001111;
    }
    if(rubr == radioUARTBaudRate::_1200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b11000111;
    }
}

void TRManager::setUARTParityBit(radioUARTParityBit rupb)
{
    if(rupb == radioUARTParityBit::_8N1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b00111111;
    }
    if(rupb == radioUARTParityBit::_8O1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b01111111;
    }
    if(rupb == radioUARTParityBit::_8E1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b10111111;
    }
}

void TRManager::setIODeriveMode(radioIODeriveMode riodm)
{
    if(riodm == radioIODeriveMode::PP)
    {
        currentSetting.OPTN &= 0b10111111;
        currentSetting.OPTN |= 0b01000000;
    }
    if(riodm == radioIODeriveMode::OC)
    {
        currentSetting.OPTN &= 0b10111111;
        currentSetting.OPTN |= 0b00000000;
    }
}

void TRManager::setWirelessWakeupTime(radioWirelessWakeupTime rwwt)
{
    if(rwwt == radioWirelessWakeupTime::_250ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00000000;
    }

    if(rwwt == radioWirelessWakeupTime::_500ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00001000;
    }
    if(rwwt == radioWirelessWakeupTime::_750ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00010000;
    }
    if(rwwt == radioWirelessWakeupTime::_1000ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00011000;
    }
    if(rwwt == radioWirelessWakeupTime::_1250ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00100000;
    }
    if(rwwt == radioWirelessWakeupTime::_1500ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00101000;
    }
    if(rwwt == radioWirelessWakeupTime::_1750ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00110000;
    }
    if(rwwt == radioWirelessWakeupTime::_1750ms)
    {
        currentSetting.OPTN &= 0b11000111;
        currentSetting.OPTN |= 0b00111000;
    }
}

void TRManager::setFEC(radioFEC rfec)
{
    if(rfec == radioFEC::FEC_ON)
    {
        currentSetting.OPTN &= 0b11111011;
        currentSetting.OPTN |= 0b0000100;
    }
    if(rfec == radioFEC::FEC_OFF)
    {
        currentSetting.OPTN &= 0b11111011;
        currentSetting.OPTN |= 0b00000000;
    }
}

void TRManager::saveSettings(bool permanent)
{
    if(permanent == true)
        currentSetting.HEAD = 0xC0;
    if(permanent == false)
        currentSetting.HEAD = 0xC2;
}

void TRManager::set_parameters(address addr,char commChannel,bool permanent,radioTransmissionPower rtp,
                radioTransmissionMode rtm, radioDataRate rdr, radioUARTBaudRate rubr,
                    radioUARTParityBit rupb,radioIODeriveMode riodm,
                    radioWirelessWakeupTime rwwt,radioFEC rfec)
{
    setAddr(addr);
    setChannel(commChannel);
    saveSettings(permanent);
    setTransmissionPower(rtp);
    setTransmissionMode(rtm);
    setDateRate(rdr);
    setUARTBaudRate(rubr);
    setUARTParityBit(rupb);
    setIODeriveMode(riodm);
    setWirelessWakeupTime(rwwt);
    setFEC(rfec);
}


setting TRManager::getCurrentSetting()
{
    return currentSetting;
}





