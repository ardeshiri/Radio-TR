#include <iostream>
#include <thread>
#include <future>

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
            cout<<trm.readBuffer('m');
        if(str == "endd")
            cout<<trm.readBuffer('d');
      //  cout<<trm.isEmpty('m');
    }



    return 0;
}
catch(Error& e)
{
    e.what();
}
