#ifndef SERIALTR_H
#define SERIALTR_H

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "Error.h"
class SerialTR
{
    public:
        explicit SerialTR(std::string prt="");
        virtual ~SerialTR();
        int connect(std::string str="");
    protected:

    private:
        std::string port;
        int fd;
        struct termios SerialPortSettings;
};

#endif // SERIALTR_H
