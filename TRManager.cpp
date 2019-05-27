#include "TRManager.h"

TRManager::TRManager(std::string str):tr{str},fd{-1},AUX{0},MD{0},Buff{0},deviceReady{0}
                                        ,databuffer{},msgbuffer{},sttngbuffer{}
{
}

TRManager::~TRManager()
{
}

int TRManager::connect(std::string str)
{
    fd = tr.connect(str);
    return fd;
}

void TRManager::writeStr(std::string str)
{
    if(fd == -1)
        throw Error("4");
    if(fd != -1)
        write(fd,str.c_str(),str.size());
}

void TRManager::readDevice()
{
    if(fd == -1)
        throw Error("5");
    char ch{};
    std::string tmp{};
    while(read(fd,&ch,1))
        {
            tmp.push_back(ch);
            if(ch == '\0')
            {
                if(tmp.find("#D") != std::string::npos)
                {
                    std::lock_guard<std::mutex> lg{mu};
                    databuffer.push(tmp);
                }
                if(tmp.find("#MS") != std::string::npos)
                {
                    std::lock_guard<std::mutex> lg{mu};
                    msgbuffer.push(tmp);
                }
                if(tmp.find("#S") != std::string::npos)
                {
                    std::lock_guard<std::mutex> lg{mu};
                    sttngbuffer.push(tmp);
                }
                react(tmp.c_str());
                tmp.clear();
            }
        }
}

std::string TRManager::readBuffer(const char ch)
{
    std::string tmp{};
    std::lock_guard<std::mutex> lg{mu};

    if(ch == 'd' && !databuffer.empty())
    {
        tmp = databuffer.front();
        databuffer.pop();
    }
    if(ch == 'm' && !msgbuffer.empty())
    {
        tmp = msgbuffer.front();
        msgbuffer.pop();
    }
    if(ch == 's' && !sttngbuffer.empty())
    {
        tmp = sttngbuffer.front();
        sttngbuffer.pop();
    }
    return tmp;
}



void TRManager::init()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    writeStr("SS");
    writeStr("#G,AUX|");
    writeStr("#G,B|");
    deviceReady = true;
}


bool TRManager::isEmpty(const char ch)
{
    if(ch == 'd')
        return databuffer.empty();
    if(ch == 'm')
        return msgbuffer.empty();
    if(ch == 's')
        return sttngbuffer.empty();
    return false;
}


void TRManager::react(std::string str)
{
    if(str.compare("#MS,ND|\r\n")==0)
        writeStr("#R,O|");
}
