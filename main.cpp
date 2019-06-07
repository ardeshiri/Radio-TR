#include <iostream>
#include <thread>
#include <future>
#include <iomanip>
#include "TRManager.h"
#include "SerialTR.h"
using namespace std;

int main()
try
{

    TRManager trm{};
    trm.connect("/dev/ttyUSB0");

   // thread {&TRManager::writeStr,ref(trm)}.detach();
    thread {&TRManager::readDevice,&trm}.detach();

    trm.init();


    while(1)
    {
        string str{};
        cin>>str;
        trm.writeStr(str);
        if(str == "endm")
            while(!trm.isEmpty('m'))
                cout<<trm.readBuffer('m');

        if(str == "ends")
            while(!trm.isEmpty('s'))
                {
                    //for(auto o:trm.readSettingBuffer())
                        for(auto o:trm.readSettingBuffer())
                           {
                               cout<<setbase(16)<<(short)o<<" - ";
                           }
                        //if(trm.readSettingBuffer()[2] == 0xc0)
                          //  cout<<"A";
                        //if(trm.readSettingBuffer()[2] == 0xc3)
                          //  cout<<"B";
                }

         if(str == "endv")
                {
                        for(auto o:trm.getCurrentModel())
                           {
                               cout<<setbase(16)<<(short)o<<" - ";
                           }
                }


        if(str == "endd")
            while(!trm.isEmpty('d'))
                cout<<trm.readBuffer('d');


        if(str == "m3")
        {
            trm.setMode(radioWMode::three);
        }
        if(str == "m2")
        {
            trm.setMode(radioWMode::two);
        }
        if(str == "m1")
        {
            trm.setMode(radioWMode::one);
        }
        if(str == "m0")
        {
            trm.setMode(radioWMode::zero);
        }

        if(str == "stst")
        {
            trm.setAddr(address{0x00,0xFF});
            trm.setChannel(0xff);
            trm.setTransmissionPower(radioTransmissionPower::_27dBm);
            trm.setTransmissionMode(radioTransmissionMode::Fixed);
            trm.setDateRate(radioDataRate::_4k8);
            trm.setUARTBaudRate(radioUARTBaudRate::_2400bps);

            trm.setUARTParityBit(radioUARTParityBit::_8E1);

            trm.setIODeriveMode(radioIODeriveMode::PP);
            trm.setWirelessWakeupTime(radioWirelessWakeupTime::_1000ms);
            trm.setFEC(radioFEC::FEC_OFF);
            trm.saveSettings(false);
        }
        if(str == "prnt")
        {
            trm.printSetting(trm.getCurrentSetting());
        }
                if(str == "vw")
        {
            trm.getCurrentSetting().printBitRep();
        }

        if(str == "get")
        {
            cout<<trm.getAUX();
            //cout<<trm.getMode();
            cout<<trm.getBuffNum();
        }

        if(str == "tst")
        {
            trm.getVersionFromDevice();
        }


        if(str == "tst2")
        {
            trm.transmit("abcdefgh12345",radioWMode::zero);
        }
      //  cout<<trm.isEmpty('m');
    }



    return 0;
}
catch(Error& e)
{
    e.what();
}
