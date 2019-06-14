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
    trm.connect("/dev/ttyUSB1");

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
            trm.getSettingFromDevice();
            trm.setAddr(address{0x00,0x04});
            trm.setChannel(0x01);
            trm.setDateRate(radioDataRate::_70k);
            trm.setIODeriveMode(radioIODeriveMode::PP);
            trm.setUARTBaudRate(radioUARTBaudRate::_9600bps);

            trm.setSettingsPermanently(false);
            trm.setDeviceSetting(trm.getCurrentSetting());
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
            trm.getSettingFromDevice();
        }


        if(str == "tst2")
        {
            targetH th{0x00,0x03,0x01};
            trm.transmit(th,"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",radioWMode::zero);
        }
      //  cout<<trm.isEmpty('m');
    }



    return 0;
}
catch(Error& e)
{
    e.what();
}
