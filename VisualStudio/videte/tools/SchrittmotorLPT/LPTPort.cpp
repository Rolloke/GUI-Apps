#include "StdAfx.h"
#include "LPTPort.h"

LPTPort::LPTPort(void)
{
}

LPTPort::~LPTPort(void)
{
}


BOOL LPTPort::setPortBaseAddress(unsigned short aAdr)
{
    mPortBaseAddress = aAdr;
    return IsInpOutDriverOpen();
}


Bits LPTPort::getBits(ePort aPort)
{
    Bits fb;
    fb.bits = DlPortReadPortUchar(mPortBaseAddress+aPort);
    return fb;
}

void LPTPort::setBits(ePort aPort, Bits aB)
{
    DlPortWritePortUchar(mPortBaseAddress+aPort, aB.bits);
}

