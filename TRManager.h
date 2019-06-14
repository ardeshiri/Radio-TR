#ifndef TRMANAGER_H
#define TRMANAGER_H
#include <iostream>
#include <queue>
#include "SerialTR.h"
#include "Error.h"
#include <mutex>
#include <thread>
#include <vector>
#include <iomanip>
#include <math.h>

#include <bitset>

enum class radioWMode{zero=0,one,two,three, unknown};
enum class radioUARTParityBit{_8N1, _8O1, _8E1,noChange};
enum class radioUARTBaudRate{_1200bps,_2400bps,_4800bps,_9600bps,_19200bps,_38400bps,_57600bps,_115200bps,noChange};
enum class radioDataRate{_1k2,_2k4,_4k8,_9k6,_19k2,_38k4,_50k,_70k,noChange};
enum class radioTransmissionMode{Transparent, Fixed,noChange};
enum class radioIODeriveMode{PP,OC,noChange};
enum class radioWirelessWakeupTime{_250ms,_500ms,_750ms,_1000ms,_1250ms,_1500ms,_1750ms,_2000ms,noChange};
enum class radioFEC{FEC_ON,FEC_OFF,noChange};
enum class radioTransmissionPower{_30dBm,_27dBm,_24dBm,_21dBm,noChange};

struct address
{
    unsigned char ADDH;
    unsigned char ADDL;
    address():ADDH{0},ADDL{0}{}
    address(unsigned char a, unsigned char b):ADDH(a),ADDL{b}
    {
    }
};

struct setting
{
    unsigned char HEAD;
    address addr;
    unsigned char SPED;
    unsigned char CHAN;
    unsigned char OPTN;

    setting():HEAD{0},addr{},SPED{0},CHAN{0},OPTN{0}{}

    operator bool()
    {
        unsigned char bl{0};
        bl = HEAD | addr.ADDH | addr.ADDL | SPED | CHAN | OPTN;
        return (bool)bl;
    }

    void printBitRep()
    {
        std::bitset<8> h{HEAD};
        std::bitset<8> ah{addr.ADDH};
        std::bitset<8> al{addr.ADDL};
        std::bitset<8> s{SPED};
        std::bitset<8> c{CHAN};
        std::bitset<8> o{OPTN};
        std::cout<<"HEAD>> "<<h.to_string()<<std::endl;
        std::cout<<"ADDH>> "<<ah.to_string()<<std::endl;
        std::cout<<"ADDL>> "<<al.to_string()<<std::endl;
        std::cout<<"SPED>> "<<s.to_string()<<std::endl;
        std::cout<<"CHAN>> "<<c.to_string()<<std::endl;
        std::cout<<"OPTN>> "<<o.to_string()<<std::endl;
    }
};

struct targetH
{
    address targetAddr;
    unsigned char targetChannel;
    targetH():targetAddr{},targetChannel{}{}
    targetH(unsigned char a, unsigned char b, unsigned char c):targetAddr{a,b},targetChannel{c}
    {}
};


class TRManager
{
    public:
        explicit TRManager(std::string str="");
        virtual ~TRManager();
        int connect(std::string str="");
        void writeStr(std::string str);
        void writeStr(std::vector<unsigned char>);
        void readDevice();
        void init();
        std::string readBuffer(const char ch);
        std::vector<unsigned char> readSettingBuffer();
        bool isEmpty(const char ch);
        void react(std::string str);
        bool getAUX();
        int getBuffNum();
        radioWMode getMode();
        void setMode(radioWMode rm);
        void printSetting(setting);
        void getSettingFromDevice();
        void getVersionFromDevice();
        void resetDevice();
        void setDeviceSetting(setting);

        void transmit(targetH th, std::string str,radioWMode rm);

        void setAddr(address adr);
        void setChannel(unsigned char commChannel);
        void setTransmissionPower(radioTransmissionPower rtp=radioTransmissionPower::_30dBm);
        void setTransmissionMode(radioTransmissionMode rtm=radioTransmissionMode::Fixed);
        void setDateRate(radioDataRate rdr=radioDataRate::_1k2);
        void setUARTBaudRate(radioUARTBaudRate rubr=radioUARTBaudRate::_9600bps);
        void setUARTParityBit(radioUARTParityBit rupb=radioUARTParityBit::_8N1);
        void setIODeriveMode(radioIODeriveMode riodm=radioIODeriveMode::PP);
        void setWirelessWakeupTime(radioWirelessWakeupTime rwwt=radioWirelessWakeupTime::_250ms);
        void setFEC(radioFEC rfec=radioFEC::FEC_ON);
        void setSettingsPermanently(bool permanent=true);
        void set_parameters(address addr,char commChannel,bool permanent=false,radioTransmissionPower rtp=radioTransmissionPower::_30dBm,
                            radioTransmissionMode rtm=radioTransmissionMode::Fixed , radioDataRate rdr=radioDataRate::_1k2, radioUARTBaudRate rubr=radioUARTBaudRate::_9600bps,
                            radioUARTParityBit rupb=radioUARTParityBit::_8N1,radioIODeriveMode riodm=radioIODeriveMode::PP,
                            radioWirelessWakeupTime rwwt=radioWirelessWakeupTime::_250ms,radioFEC rfec=radioFEC::FEC_ON);
        setting getCurrentSetting();
        std::vector<unsigned char> getCurrentModel();


    protected:

    private:
        void sttngSet(std::vector<unsigned char>);
        void vrsnSet(std::vector<unsigned char>);
        SerialTR tr;
        int fd;
        bool AUX;
        radioWMode MD;
        int Buff;
        bool deviceReady;
        std::queue<std::string> databuffer;
        std::queue<std::string> msgbuffer;
        std::queue<std::vector<unsigned char>> sttngbuffer;
        std::queue<std::string> sendBuffer;
        std::mutex mu{};
        setting currentSetting;
        std::vector<unsigned char> model;
        bool active;
};

#endif // TRMANAGER_H
