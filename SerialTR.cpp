#include "SerialTR.h"

SerialTR::SerialTR(std::string prt):port{prt},fd{-1},SerialPortSettings{}
{
    //ctor
}

SerialTR::~SerialTR()
{
    close(fd);
}

int SerialTR::connect(std::string str)
{
    if(!str.empty())
    {
        port.assign(str);
    }
    if(port.empty())
    {
        throw Error("1");
    }
    fd = open(port.c_str(),O_RDWR|O_NOCTTY);
    if(fd == -1)
    {
        throw Error("2");
    }
    memset(&SerialPortSettings,0,sizeof(struct termios));
    cfsetispeed(&SerialPortSettings,B9600);
    cfsetispeed(&SerialPortSettings,B9600);
    SerialPortSettings.c_cflag &= ~PARENB;
    SerialPortSettings.c_cflag &= ~CSTOPB;
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |=  CS8;
    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    SerialPortSettings.c_oflag &= ~OPOST;
    SerialPortSettings.c_cc[VMIN] = 10;
    SerialPortSettings.c_cc[VTIME] = 0;
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0)
        throw Error{"3"};
    tcflush(fd, TCIFLUSH);
    return fd;
}
