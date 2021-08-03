
#include "gpioclass.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


const std::string gGpioPath = "/sys/class/gpio/";
const std::string gGpio     = "gpio";
#define OK 0

CGPIO::CGPIO(int aPort, eDirection aOutput) : mFile(-1)
{
    const std::string fGpioExport   = gGpioPath  + "export";
    int fResult = access(fGpioExport.c_str(), W_OK);
    if (fResult == OK)
    {
        std::stringstream fPortNo;
        fPortNo << aPort;

        std::string fGpioDevicePath = gGpioPath + gGpio + fPortNo.str();
        fResult = access(fGpioDevicePath.c_str(), F_OK);

        if (fResult != OK)
        {
            std::ofstream ofs(fGpioExport.c_str());
            if (ofs.is_open())
            {
                ofs << aPort;
                ofs.close();
                delay(100);
                
                mPort = fPortNo.str();
                direction(aOutput);
                mFile = open((gGpioPath + gGpio + mPort  + "/value").c_str(), aOutput ? O_RDWR : O_RDONLY);
            }
        }

    }
}

void CGPIO::direction(eDirection aOutput)
{
    if (aOutput != None)
    {
        std::string fDeviceDirection = gGpioPath + gGpio + mPort + "/direction";
        std::ofstream ofs(fDeviceDirection.c_str());
        if (ofs.is_open())
        {
            ofs << (aOutput ? "out" : "in");
            ofs.close();
        }
        else
        {
            perror(("cannot set direction of " + fDeviceDirection).c_str());
        }
    }
}

CGPIO::~CGPIO()
{
    if (mFile != -1)
    {
        close(mFile);
    }        
    if (!mPort.empty())
    {
        std::ofstream ofs((gGpioPath + "unexport").c_str());
        if (ofs.is_open())
        {
            ofs << mPort;
            ofs.close();
        }
        else
        {
            perror(("cannot deactivate port " + mPort).c_str());
        }
    }
}

int CGPIO::value()
{
    int fResult = -1;
    if (mFile != -1)
    {
        char fString[64];
        if (read(mFile, fString, sizeof(fString)-1) < 0)
        {
            perror(("cannot read value of port " + mPort).c_str());
        }
        else
        {
            fResult = atoi(fString);
        }
    }
    return fResult;
}

void CGPIO::value(int aValue)
{
    if (mFile != -1)
    {
        if (write(mFile, aValue != 0 ? "1" : "0", 1) < 0)
        {
            perror(("cannot write value to port " + mPort).c_str());
        }
    }
}    

const std::string&  CGPIO::port()
{
    return mPort;
}

int delay(unsigned long millis)
{
  struct timespec ts;
  int err;

  ts.tv_sec = millis / 1000;
  ts.tv_nsec = (millis % 1000) * 1000000L;
  err = nanosleep(&ts, (struct timespec *)NULL);
  return (err);
}

int udelay(unsigned long micros)
{
  struct timespec ts;
  int err;

  ts.tv_sec = micros / 1000000L;
  ts.tv_nsec = (micros % 1000000L) * 1000L;
  err = nanosleep(&ts, (struct timespec *)NULL);
  return (err);
}

