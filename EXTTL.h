#ifndef EXTTL_H
#define EXTTL_H
#include "TRManager.h"
#include <thread>

class EXTTL
{
    public:
        explicit EXTTL(std::string str = "");
        virtual ~EXTTL();
        void connect(std::string str);
        void listen();
/*
        void changeSetting
        void send
        void readOne
        void readAll
        void getAUX
        void getMode
        void getBuffNum
        void changeMode
*/
    protected:

    private:
        TRManager TR;

};

#endif // EXTTL_H
