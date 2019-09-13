#pragma once

typedef unsigned short USHORT;
#include "/home/rolf/Projekte/own_drivers/LptPortStepperDriver/lpt_stepper.h"
#include <ieee1284.h>

struct DataBit
{
    unsigned char bit1              : 1; /// pin 2
    unsigned char bit2              : 1; /// pin 3
    unsigned char bit3              : 1; /// pin 4
    unsigned char bit4              : 1; /// pin 5
    unsigned char bit5              : 1; /// pin 6
    unsigned char bit6              : 1; /// pin 7
    unsigned char bit7              : 1; /// pin 8
    unsigned char bit8              : 1; /// pin 9
};

struct StatusBit
{
    unsigned char notused1          : 1; 
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
    unsigned char error             : 1; /// pin 15
    unsigned char select            : 1; /// pin 13
    unsigned char paper_end         : 1; /// pin 12
    unsigned char acknowledge       : 1; /// pin 10
    unsigned char busy              : 1; /// pin 11 (inv)
};

struct ControlBit
{
    unsigned char data_strobe       : 1; /// pin 1  (inv)
    unsigned char line_feed         : 1; /// pin 14 (inv)
    unsigned char init_printer      : 1; /// pin 16
    unsigned char select_input      : 1; /// pin 17 (inv)
    unsigned char enable_interrupt  : 1;
    unsigned char notused1          : 1;
    unsigned char notused2          : 1;
    unsigned char notused3          : 1;
};


union Bits
{
    unsigned char   bits; 
    DataBit         d;
    ControlBit      c;
    StatusBit       s;
};

class LPTPort
{
public:
    enum ePort {data, status, control};
    enum ePortType {invalid, stepper, ioport, parport };
    LPTPort();
    ~LPTPort();
    // getter / setter
    unsigned short getPortBaseAddress() const;
    bool setPortBaseAddress(unsigned short aAdr);
    bool setPortPath(const char* aPath);
    int  getPorts() const;
    const char* getPortPath(int aNr) const;
    const char* getPortName(int aNr) const;
    int         getPortAddress(int aNr) const;
    ePortType   getPortType() const;
    Bits getBits(ePort aPort) const;
    void setBits(ePort aPort, Bits aB) const;
    bool hasExtendedIoctl();
    int  Ioctl(long aCmd, unsigned long arg);
    void setMotor(stepper::eDirection aDir, stepper::eMotors aMotor);
    void setMotorBits(stepper::eDirection aDir, long aBit0, long aBit1, long aBit2, long aBit3);
    void setPenPos(stepper::ePenPos aPenPos, long aVerticalPosition);
    void setStatusBit(stepper::eStates aState, long aBit);
    void setPosition(stepper::eDirection aDir, long aPosition);
    void setEnableMotorsBit(long aBit);
    void setAsynchron(long aAsync);
    void setDelay(stepper::eDirection aDir, long aDelay_us);
    void setStartDelay(stepper::eDirection aDir, long aDelay_us);
    void setRampSteps(stepper::eDirection aDir, long aDelay_us);

    void enableMotors(bool aEnable);
    void moveTo(long aX, long aY, bool aRelative=false);
    void lineTo(long aX, long aY, bool aRelative=false);
    void lineTo3D(long aX, long aY, long aZ);
    void moveToPenPos(stepper::ePenPos aPenPos);

    long getPosition(stepper::eDirection aDir);

private:
    bool  OpenDriver(unsigned short aAdr);
    unsigned char  ReadUchar(USHORT aNr) const;
    void  WriteUchar(USHORT aNr, unsigned char aData) const;
    bool  hasIoctl() const;
    void  closeDevice();

    unsigned short mPortBaseAddress;
    ePortType mPortType;
    int  mFile ;
    parport_list mPortList;
};
