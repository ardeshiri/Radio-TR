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
                    string str = trm.readBuffer('s');
                    for(short o:str)
                        cout<<setbase(16)<<(short)o<<" <> ";
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


        if(str == "get")
        {
            cout<<trm.getAUX();
            //cout<<trm.getMode();
            cout<<trm.getBuffNum();
        }

        if(str == "tst")
        {
            trm.getSetting();
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
