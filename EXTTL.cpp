#include "EXTTL.h"

EXTTL::EXTTL(std::string str):TR{str}
{
    //ctor
}

EXTTL::~EXTTL()
{
    //dtor
}

void EXTTL::connect(std::string str)
{
    TR.connect(str);
}


void EXTTL::listen()
{
    std::thread {&TRManager::readDevice,&TR}.detach();
}
