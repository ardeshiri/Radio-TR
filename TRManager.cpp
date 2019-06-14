#include "TRManager.h"

TRManager::TRManager(std::string str):tr{str},fd{-1},AUX{0},MD{radioWMode::unknown},Buff{0},deviceReady{0}
    ,databuffer{},msgbuffer{},sttngbuffer{},sendBuffer{},currentSetting{},model{},active{true}
{
    set_parameters({0x00,0x00},0x00);
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
        throw Error{"4"};
    if(str.length()>510)
        throw Error{"long input str"};
    if(fd != -1)
        write(fd,str.c_str(),str.size());
}

void TRManager::writeStr(std::vector<unsigned char> v)
{
    if(fd == -1)
        throw Error("5");
    if(v.size()>510)
        throw Error{"long input size"};
    if(fd != -1)
        //for(auto o:v)
        write(fd,v.data(),v.size());
}

void TRManager::readDevice()
{
    if(fd == -1)
        throw Error("5");
    char tmpcs[100]{};
    unsigned char tmpuscs[100];
    memset(tmpcs,0,sizeof(tmpcs));
    memset(tmpuscs,0,sizeof(tmpuscs));
    int ctr = 0;
    char ch{};

    while(read(fd,&ch,1) && active)
    {
        std::cout<<ch;   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        tmpcs[ctr++] = ch;
        if(strcmp(tmpcs,"#S,") == 0)
        {
            read(fd,tmpuscs,10);
            do
            {
                read(fd,&ch,1);
                tmpcs[ctr++] = ch;
            }while(ch != '\0');

            std::vector<unsigned char> tmpv{};
            std::lock_guard<std::mutex> lg{mu};
            for(int i = 0; i<10; i++)
                {
                    tmpv.push_back(tmpuscs[i]);
                }
            sttngbuffer.push(tmpv);
            memset(tmpcs,0,sizeof(tmpcs));
            ctr = 0;
            sttngSet(tmpv);
            vrsnSet(tmpv);
            continue;
        }

        if(ch == '\0')
            {
            std::string tmps{tmpcs};
            memset(tmpcs,0,sizeof(tmpcs));
            ctr = 0;
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
}


void TRManager::sttngSet(std::vector<unsigned char> v)
{
    if(!v.empty())
    {
        if(v[0] == 0xc0 || v[0] == 0xc2)
        {
            memcpy(&currentSetting,v.data(),6);
        }
    }
}

void TRManager::vrsnSet(std::vector<unsigned char> v)
{
    if(!v.empty())
    {
        if(v[0] == 0xc3)
        {
            model.clear();
            for(int i = 0; i < 4; i++)
            {
                model.push_back(v[i]);
            }
        }
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

    return tmp;
}



std::vector<unsigned char> TRManager::readSettingBuffer()
{
    std::vector<unsigned char> tmpv;
    if(!sttngbuffer.empty())
    {
        tmpv = sttngbuffer.front();
        sttngbuffer.pop();
    }
    return tmpv;
}


void TRManager::init()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    writeStr("SS");
    writeStr("#G,AUX|");
    writeStr("#G,B|");
    writeStr("#R,A|");
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
       //|| str.compare("#MS,B1|\r\n")==0 || str.compare("#MS,B2|\r\n")==0 ||
       //str.compare("#MS,B3|\r\n")==0 || str.compare("#MS,B4|\r\n")==0 || str.compare("#MS,B5|\r\n")==0 ||
       //str.compare("#MS,B6|\r\n")==0 || str.compare("#MS,B7|\r\n")==0 || str.compare("#MS,B8|\r\n")==0 ||
       //str.compare("#MS,B9|\r\n")==0 )
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


void TRManager::getSettingFromDevice()
{
    radioWMode mode = MD;

    std::string tmp{};

    tmp += "#CS,";
    tmp.push_back(0xc1);
    tmp.push_back(0xc1);
    tmp.push_back(0xc1);
    tmp += "|";

    for(int i = 0 ; i < 3; i++)
    {
        if(AUX == true)
            break;
        if(AUX == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds((i+1)*50));
            }
    }
    if(AUX == false)
        return;
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



void TRManager::getVersionFromDevice()
{
    radioWMode mode = MD;

    std::string tmp{};

    tmp += "#CS,";
    tmp.push_back(0xc3);
    tmp.push_back(0xc3);
    tmp.push_back(0xc3);
    tmp += "|";

    for(int i = 0 ; i < 3; i++)
    {
        if(AUX == true)
            break;
        if(AUX == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds((i+1)*50));
            }
    }
    if(AUX == false)
        return;
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


void TRManager::resetDevice()
{
    radioWMode mode = MD;

    std::string tmp{};

    tmp += "#CS,";
    tmp.push_back(0xc4);
    tmp.push_back(0xc4);
    tmp.push_back(0xc4);
    tmp += "|";

    for(int i = 0 ; i < 3; i++)
    {
        if(AUX == true)
            break;
        if(AUX == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds((i+1)*50));
            }
    }
    if(AUX == false)
        return;
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


void TRManager::setDeviceSetting(setting stt)
{
    radioWMode mode = MD;
    std::vector <unsigned char> tmp;
    tmp.push_back('@');
    tmp.push_back(stt.HEAD);
    tmp.push_back(stt.addr.ADDH);
    tmp.push_back(stt.addr.ADDL);
    tmp.push_back(stt.SPED);
    tmp.push_back(stt.CHAN);
    tmp.push_back(stt.OPTN);

    for(int i = 0 ; i < 3; i++)
    {
        if(AUX == true)
            break;
        if(AUX == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds((i+1)*50));
            }
    }
    if(AUX == false)
        return;
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

void TRManager::transmit(targetH th, std::string str, radioWMode rm)
{
    /// address and channel must be added , is AUX measured??
    if(rm == radioWMode::three || rm == radioWMode::two)
        {
            return;
        }
    int len = str.length();
    int ctr = std::ceil(((double)len)/40);

    radioWMode mode = MD;
    if(MD != rm) /// first go to desired mode!! otherwise successive writes causes higher latency!!
    {
        setMode(rm);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for(int i = 0; i < ctr; i++)
    {
    std::string tmp{};
    tmp += "$";
    tmp.push_back(th.targetAddr.ADDH);
    tmp.push_back(th.targetAddr.ADDL);
    tmp.push_back(th.targetChannel);
    tmp += str.substr(i*40,40);
    while(AUX == false)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    writeStr(tmp);
    AUX = false;
    }

    if(mode != rm)
        setMode(mode);
}


void TRManager::setAddr(address addr)
{
    memcpy(&currentSetting.addr,&addr,sizeof(address));
}


void TRManager::setChannel(unsigned char commChannel)
{
    currentSetting.CHAN &= 0b00000000;

    memcpy(&currentSetting.CHAN,&commChannel,sizeof(unsigned char));
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
        currentSetting.SPED |= 0b00000000;
    }
    if(rdr == radioDataRate::_2k4)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000001;
    }
    if(rdr == radioDataRate::_4k8)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000010;
    }
    if(rdr == radioDataRate::_9k6)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000011;
    }
    if(rdr == radioDataRate::_19k2)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000100;
    }
    if(rdr == radioDataRate::_38k4)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000101;
    }
    if(rdr == radioDataRate::_50k)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000110;
    }
    if(rdr == radioDataRate::_70k)
    {
        currentSetting.SPED &= 0b11111000;
        currentSetting.SPED |= 0b00000111;
    }
}

void TRManager::setUARTBaudRate(radioUARTBaudRate rubr)
{
    if(rubr == radioUARTBaudRate::_115200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00111000;
    }
    if(rubr == radioUARTBaudRate::_57600bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00110000;
    }
    if(rubr == radioUARTBaudRate::_38400bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00101000;
    }
    if(rubr == radioUARTBaudRate::_19200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00100000;
    }
    if(rubr == radioUARTBaudRate::_9600bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00011000;
    }
    if(rubr == radioUARTBaudRate::_4800bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00010000;
    }
    if(rubr == radioUARTBaudRate::_2400bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00001000;
    }
    if(rubr == radioUARTBaudRate::_1200bps)
    {
        currentSetting.SPED &= 0b11000111;
        currentSetting.SPED |= 0b00000000;
    }
}

void TRManager::setUARTParityBit(radioUARTParityBit rupb)
{
    if(rupb == radioUARTParityBit::_8N1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b00000000;
    }
    if(rupb == radioUARTParityBit::_8O1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b01000000;
    }
    if(rupb == radioUARTParityBit::_8E1)
    {
        currentSetting.SPED &= 0b00111111;
        currentSetting.SPED |= 0b10000000;
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
    if(rwwt == radioWirelessWakeupTime::_2000ms)
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

void TRManager::setSettingsPermanently(bool permanent)
{
    memset(&currentSetting.HEAD , 0 , 1);
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
    setSettingsPermanently(permanent);
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

std::vector<unsigned char> TRManager::getCurrentModel()
{
    return model;
}


void TRManager::printSetting(setting stt)
{
    if(!(bool)currentSetting)
        {
            std::cout<<"not set!"<<std::endl;
            return;
        }

    if(stt.HEAD == 0xc0)
        std::cout<<"permanent setting\n";
    if(stt.HEAD == 0xc2)
        std::cout<<"temporary setting\n";

    std::cout<<"ADDH: "<<std::setbase(16)<<(short)stt.addr.ADDH<<std::endl;
    std::cout<<"ADDL: "<<std::setbase(16)<<(short)stt.addr.ADDL<<std::endl;

    if((stt.SPED & 0b00000111) == 0b00000111)
        std::cout<<"Air Date Rate: "<<"70kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000110)
        std::cout<<"Air Date Rate: "<<"50kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000101)
        std::cout<<"Air Date Rate: "<<"38kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000100)
        std::cout<<"Air Date Rate: "<<"19kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000011)
        std::cout<<"Air Date Rate: "<<"9.6kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000010)
        std::cout<<"Air Date Rate: "<<"4.8kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000001)
        std::cout<<"Air Date Rate: "<<"2.4kbps"<<std::endl;
    if((stt.SPED & 0b00000111) == 0b00000000)
        std::cout<<"Air Date Rate: "<<"1.2kbps"<<std::endl;

    if((stt.SPED & 0b00111000) == 0b00111000)
        std::cout<<"UART Baud Rate: "<<"115200bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00110000)
        std::cout<<"UART Baud Rate: "<<"57600bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00101000)
        std::cout<<"UART Baud Rate: "<<"38400bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00100000)
        std::cout<<"UART Baud Rate: "<<"19200bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00011000)
        std::cout<<"UART Baud Rate: "<<"9600bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00010000)
        std::cout<<"UART Baud Rate: "<<"4800bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00001000)
        std::cout<<"UART Baud Rate: "<<"2400bps"<<std::endl;
    if((stt.SPED & 0b00111000) == 0b00000000)
        std::cout<<"UART Baud Rate: "<<"1200bps"<<std::endl;

    if((stt.SPED & 0b11000000) == 0b11000000)
        std::cout<<"UART Parity Bit: "<<"8N1"<<std::endl;
    if((stt.SPED & 0b11000000) == 0b10000000)
        std::cout<<"UART Parity Bit: "<<"8E1"<<std::endl;
    if((stt.SPED & 0b11000000) == 0b01000000)
        std::cout<<"UART Parity Bit: "<<"8O1"<<std::endl;
    if((stt.SPED & 0b11000000) == 0b00000000)
        std::cout<<"UART Parity Bit: "<<"8N1"<<std::endl;

    stt.CHAN = stt.CHAN & 0b00011111;
    std::cout<<"Channel: "<<std::setbase(10)<<410+(unsigned short)(stt.CHAN)<<"MHz"<<std::endl;

    if((stt.OPTN & 0b00000011) == 0b00000000)
        std::cout<<"Transmission Power: "<<"30dBm"<<std::endl;
    if((stt.OPTN & 0b00000011) == 0b00000001)
        std::cout<<"Transmission Power: "<<"27dBm"<<std::endl;
    if((stt.OPTN & 0b00000011) == 0b00000010)
        std::cout<<"Transmission Power: "<<"24dBm"<<std::endl;
    if((stt.OPTN & 0b00000011) == 0b00000011)
        std::cout<<"Transmission Power: "<<"21dBm"<<std::endl;

    if((stt.OPTN & 0b00000100) == 0b00000000)
        std::cout<<"FEC: OFF"<<std::endl;
    if((stt.OPTN & 0b00000100) == 0b00000100)
        std::cout<<"FEC: ON "<<std::endl;

    if((stt.OPTN & 0b00111000) == 0b00000000)
        std::cout<<"Wireless Wakeup time: "<<"250ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00001000)
        std::cout<<"Wireless Wakeup time: "<<"500ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00010000)
        std::cout<<"Wireless Wakeup time: "<<"750ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00011000)
        std::cout<<"Wireless Wakeup time: "<<"1000ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00100000)
        std::cout<<"Wireless Wakeup time: "<<"1250ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00101000)
        std::cout<<"Wireless Wakeup time: "<<"1500ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00110000)
        std::cout<<"Wireless Wakeup time: "<<"1750ms"<<std::endl;
    if((stt.OPTN & 0b00111000) == 0b00111000)
        std::cout<<"Wireless Wakeup time: "<<"2000ms"<<std::endl;

    if((stt.OPTN & 0b01000000) == 0b00000000)
        std::cout<<"IO Derive mode: "<<"Open Collector"<<std::endl;
    if((stt.OPTN & 0b01000000) == 0b01000000)
        std::cout<<"IO Derive mode: "<<"Push Pull"<<std::endl;

    if((stt.OPTN & 0b10000000) == 0b00000000)
        std::cout<<"Fixed Transmission: "<<"Transparent"<<std::endl;
    if((stt.OPTN & 0b10000000) == 0b10000000)
        std::cout<<"Fixed Transmission: "<<"Fixed"<<std::endl;

}


