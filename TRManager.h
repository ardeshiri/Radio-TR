#ifndef TRMANAGER_H
#define TRMANAGER_H
#include <iostream>
#include <queue>
#include "SerialTR.h"
#include "Error.h"
#include <mutex>
#include <thread>

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

    protected:

    private:
        SerialTR tr;
        int fd;
        bool AUX;
        int MD;
        int Buff;
        bool deviceReady;
        std::queue<std::string> databuffer;
        std::queue<std::string> msgbuffer;
        std::queue<std::string> sttngbuffer;
        std::mutex mu{};
};

#endif // TRMANAGER_H
