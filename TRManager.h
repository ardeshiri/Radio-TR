#ifndef TRMANAGER_H
#define TRMANAGER_H
#include <iostream>
#include <queue>
#include "SerialTR.h"
#include "Error.h"
#include <mutex>
#include <thread>
#include <vector>

enum class radioWMode{zero=0,one,two,three, unknown};
enum class radioUARTParityBit{_8N1, _8O1, _8E1};
enum class radioUARTBaudRate{_1200bps,_2400bps,_4800bps,_9600bps,_19200bps,_38400bps,_57600bps,_115200bps};
enum class radioDataRate{_1k2,_2k4,_4k8,_9k6,_19k2,_38k4,_50k,_70k};
enum class radioTransmissionMode{Transparent, Fixed};
enum class radioIODeriveMode{PP,OC};
enum class radioWirelessWakeupTime{_250ms,_500ms,_750ms,_1000ms,_1250ms,_1500ms,_1750ms,_2000ms};
enum class radioFEC{FEC_ON,FEC_OFF};
enum class radioTransmissionPower{_30dBm,_27dBm,_24dBm,_21dBm};

struct address
{
    char ADDH;
    char ADDL;
};

struct setting
{
    char HEAD;
    address addr;
    char SPED;
    char CHAN;
    char OPTN;
};




class TRManager
{
    public:
        explicit TRManager(std::string str="");
        virtual ~TRManager();
        int connect(std::string str="");
        void writeStr(std::string str);
        void readDevice();
        void init();
        std::string readBuffer(const char ch);
        bool isEmpty(const char ch);
        void react(std::string str);
        bool getAUX();
        int getBuffNum();
        radioWMode getMode();
        void setMode(radioWMode rm);

        void transmit(std::string str,radioWMode rm=radioWMode::zero);
        void getSetting();
        void setPSettings();
        void setTSettings();
        //std::string readAllBuffer(const char ch);

        void setAddr(address adr);
        void setChannel(char commChannel);
        void setTransmissionPower(radioTransmissionPower rtp=radioTransmissionPower::_30dBm);
        void setTransmissionMode(radioTransmissionMode rtm=radioTransmissionMode::Fixed);
        void setDateRate(radioDataRate rdr=radioDataRate::_1k2);
        void setUARTBaudRate(radioUARTBaudRate rubr=radioUARTBaudRate::_9600bps);
        void setUARTParityBit(radioUARTParityBit rupb=radioUARTParityBit::_8N1);
        void setIODeriveMode(radioIODeriveMode riodm=radioIODeriveMode::PP);
        void setWirelessWakeupTime(radioWirelessWakeupTime rwwt=radioWirelessWakeupTime::_250ms);
        void setFEC(radioFEC rfec=radioFEC::FEC_ON);
        void saveSettings(bool permanent=true);

        void set_parameters(address addr,char commChannel,bool permanent=true,radioTransmissionPower rtp=radioTransmissionPower::_30dBm,
                            radioTransmissionMode rtm=radioTransmissionMode::Fixed , radioDataRate rdr=radioDataRate::_1k2, radioUARTBaudRate rubr=radioUARTBaudRate::_9600bps,
                            radioUARTParityBit rupb=radioUARTParityBit::_8N1,radioIODeriveMode riodm=radioIODeriveMode::PP,
                            radioWirelessWakeupTime rwwt=radioWirelessWakeupTime::_250ms,radioFEC rfec=radioFEC::FEC_ON);

        setting getCurrentSetting();

    protected:

    private:
        SerialTR tr;
        int fd;
        bool AUX;
        radioWMode MD;
        int Buff;
        bool deviceReady;
        std::queue<std::string> databuffer;
        std::queue<std::string> msgbuffer;
        std::queue<std::vector<char>> sttngbuffer;
        std::mutex mu{};
        setting currentSetting;
};

#endif // TRMANAGER_H
