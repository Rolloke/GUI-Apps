
#include "LPTPort.h"
#include <sys/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ppdev.h>
#include <errno.h>
#include <assert.h>
#include <linux/parport.h>

LPTPort::LPTPort(void) : mPortBaseAddress(0)
, mPortType(invalid)
, mFile(-1)
{
    if (ieee1284_find_ports(&mPortList, 0) != E1284_OK)
    {
        mPortList.portc = 0;
        perror("Error no parallel ports: ");
    }

   OpenDriver(mPortBaseAddress);
}

LPTPort::~LPTPort(void)
{
    ieee1284_free_ports(&mPortList);
    closeDevice();
}

void  LPTPort::closeDevice()
{
    switch (mPortType)
    {
    case stepper:
        close(mFile);
        break;
    case parport:
        ioctl( mFile, PPRELEASE, NULL );
        close( mFile );
        break;
    case ioport:
        break;
    default:
        break;
    }
    mFile = -1;
    mPortType = invalid;
}

/*!
  \brief sets the port address
  @param aAdr new address value (unsigned short)
  returns true, if the port address has changed and the port was opened succesfully
  */
bool LPTPort::setPortBaseAddress(unsigned short aAdr)
{
    if (mPortBaseAddress != aAdr  )
    {
        mPortBaseAddress = aAdr;
        return OpenDriver(aAdr);
    }
    else
    {
        return false;
    }
}

bool LPTPort::setPortPath(const char *aPath)
{
    closeDevice();
    int fOpenFlag = -1;
    if (access(aPath, W_OK) == 0)
    {
        fOpenFlag = O_RDWR;
    }
    if (access(aPath, R_OK) == 0)
    {
        fOpenFlag = O_RDONLY;
    }
    if (fOpenFlag != -1)
    {
        mFile = open(aPath, fOpenFlag);
    }
    if (mFile > 0 )
    {
        int fResult = ioctl( mFile, PPCLAIM, NULL );
        if (fResult == -1)
        {
            perror("could not apply PPCLAIM: ");
        }
        else
        {
            for (int i=0; i< mPortList.portc; ++i)
            {
                if (strcmp(mPortList.portv[i]->filename, aPath) == 0)
                {
                    mPortBaseAddress = i;
                    break;
                }
            }
            mPortType = parport;
        }
    }
    else
    {
        perror("could not open parallel port");
        mPortType = invalid;
    }
    return mPortType != invalid;
}

int LPTPort::getPorts() const
{
    return mPortList.portc;
}

const char* LPTPort::getPortPath(int aNr) const
{
    if (aNr < mPortList.portc && mPortList.portv)
    {
        return mPortList.portv[aNr]->filename;
    }
    return "";
}

const char* LPTPort::getPortName(int aNr) const
{
    if (aNr < mPortList.portc && mPortList.portv)
    {
        return mPortList.portv[aNr]->name;
    }
    return "";
}

int LPTPort::getPortAddress(int aNr) const
{
    if (aNr < mPortList.portc && mPortList.portv)
    {
        return mPortList.portv[aNr]->base_addr;
    }
    return 0;
}

LPTPort::ePortType LPTPort::getPortType() const
{
    return mPortType;
}

unsigned short LPTPort::getPortBaseAddress() const
{
    if (mPortType == parport)
    {
        return getPortAddress(mPortBaseAddress);
    }
    return mPortBaseAddress;
}

/*! @brief: retrieves the value oft the LPT port
 *  @param aPort port number (ePort)
 *  @return value of LPT port */
Bits LPTPort::getBits(ePort aPort) const
{
    Bits fb;
    fb.bits = ReadUchar(aPort);
    return fb;
}

/*! @brief: sets the value oft the LPT port
 *  @param aPort port number (ePort)
 *  @param aB value          (Bits)
 *  @return zero at success */
void LPTPort::setBits(ePort aPort, Bits aB) const
{
    WriteUchar(aPort, aB.bits);
}

bool LPTPort::hasExtendedIoctl()
{
    return hasIoctl() && LPT_STEPPER_IOC_MAXNR >= 24;
}

/*! @brief: Opens the LPT port Driver or requests ioperm
 *  @param aAdr address of the port (unsigned short)
 *  @return success, if driver or permission to port access*/
bool  LPTPort::OpenDriver(unsigned short aAdr)
{
    closeDevice();
    mFile = open("/dev/stepper1", O_RDWR);
    if (mFile != -1)
    {
        USHORT fAdr = ioctl(mFile, LPT_STEPPER_IOCQ_BASE_ADDRESS, 0);
        if (fAdr != aAdr)
        {
            ioctl(mFile, LPT_STEPPER_IOCT_BASE_ADDRESS, aAdr);
        }
        mPortType = stepper;
    }
    else
    {
        int result = ioperm(aAdr, 5, 1);
        printf("ioperm(%x) : %d\n", aAdr, result);
        if (result < 0)
        {
            perror("could not open port");
        }
        fflush(stdout);
        mPortType = (result == 0) ? ioport : invalid;
    }
    return  mPortType != invalid || mFile != -1;

}

/*! @brief: retrieves value of port adress
 *  @param aPort port adress (USHORT)
 *  @param aNr port adress offset (USHORT)
 *  @return value of port address */
unsigned char  LPTPort::ReadUchar(USHORT aNr) const
{

    switch (mPortType)
    {
    case ioport:
        return inb(mPortBaseAddress+aNr);
    case stepper:
        if(mFile != -1)
        {
            return ioctl(mFile, LPT_STEPPER_IOCQ_VALUE, aNr);
        }
        break;
    case parport:
        if(mFile != -1)
        {
            int fResult = 0;
            unsigned char fbyte=0;
            switch (aNr)
            {
            case data:
                fResult = ioctl( mFile, PPRDATA  , &fbyte);
                break;
            case status:
                fResult = ioctl( mFile, PPRSTATUS, &fbyte);
                break;
            case control:
                fResult = ioctl( mFile, PPRCONTROL, &fbyte);
                break;
            }
            if (fResult < 0)
            {
                perror("error reading parport: ");
            }
            return fbyte;
        } break;
    default:
        break;
    }
    return 0;
}

/*! @brief: sets value of port adress
 *  @param aPort port adress (USHORT)
 *  @param aNr port adress offset (USHORT) */
void  LPTPort::WriteUchar(USHORT aNr, unsigned char aData) const
{
    printf("write %x -> port %x:%d\n", aData, mPortBaseAddress, aNr);
    switch (mPortType)
    {
        case ioport:
        {
            outb(aData, mPortBaseAddress+aNr);
        } break;
        case stepper:
        {
            unsigned long arg = aNr;
            arg <<= 16;
            arg |= aData;
            ioctl(mFile, LPT_STEPPER_IOCT_VALUE, arg);
        } break;
        case parport:
        {
            int fResult = 0;
            switch (aNr)
            {
            case data:
                fResult = ioctl( mFile, PPWDATA  , &aData);
                break;
            case control:
                fResult = ioctl( mFile, PPWCONTROL, &aData);
                break;
            default:
                fResult = EINVAL;
                break;
            }
            if (fResult < 0)
            {
                perror("error reading parport: ");
            }
        }break;
        default:break;
    }
}

int  LPTPort::Ioctl(long aCmd, unsigned long arg)
{
    if(mPortType == stepper)
    {
        return ioctl(mFile, aCmd, arg);
    }
    return 0;
}

bool  LPTPort::hasIoctl() const
{
    return mPortType == stepper;
}

/*! @brief: sets the stepper motor type
 *  @param desired direction of motor  (eDirection)
 *  @param motor type                  (eMotors) */
void LPTPort::setMotor(stepper::eDirection aDir, stepper::eMotors aMotor)
{
    unsigned long fDirMotor[2] = { aDir, aMotor };
    Ioctl(LPT_STEPPER_IOCS_MOTORS, (unsigned long)&fDirMotor[0]);
}

/*! @brief: sets the stepper motor bits for the desired output pin
 *  @param desired direction of motor  (eDirection)
 *  @param bit0 of output pin (1-12)   (long)
 *  @param bit1 of output pin (1-12)   (long)
 *  @param bit2 of output pin (1-12)   (long)
 *  @param bit3 of output pin (1-12)   (long)
 *  @hint a negative pin value inverts the polarity */
void LPTPort::setMotorBits(stepper::eDirection aDirection, long aBit0, long aBit1, long aBit2, long aBit3)
{
    long fDirMotorBits[5] = { aDirection, aBit0, aBit1, aBit2, aBit3 };
    Ioctl(LPT_STEPPER_IOCS_MOTOR_BITS, (unsigned long)&fDirMotorBits[0]);
}

/*! @brief: sets the predefined pen position
 *  @param aPenPos desired pen position                     (ePenPos)
 *  @param aVerticalPosition z position of pen position     (int) */
void LPTPort::setPenPos(stepper::ePenPos aPenPos, long aVerticalPosition)
{
    long fPenPosition[2] = { aPenPos, aVerticalPosition };
    Ioctl(LPT_STEPPER_IOCS_PEN_POS, (unsigned long)&fPenPosition[0]);
}

/*! @brief: enables or disables all motors
 *  @param aEnable false: disable, true: enable motors  (bool)
 *  @return zero at success */
void LPTPort::enableMotors(bool aEnable)
{
    Ioctl(LPT_STEPPER_IOCT_ENABLE_MOTORS, aEnable);
}

/*! @brief: sets the status bit for the desired input pin
 *  @param desired state identifier    (eStates)
 *  @param bit of status pin (3-7)     (long)
 *  @hint a negative pin value inverts the polarity */
void LPTPort::setStatusBit(stepper::eStates aState, long aBit)
{
    long fStatusBits[2] = { aState, aBit };
    Ioctl(LPT_STEPPER_IOCS_STATUS_BIT, (unsigned long)&fStatusBits[0]);
}

/*! @brief: sets the stepper motor position value without stepping
 *  @param desired direction of position       (eDirection)
 *  @param position value for this direction   (long) */
void LPTPort::setPosition(stepper::eDirection aDir, long aPosition)
{
    long fDirPosition[2] = { aDir, aPosition };
    Ioctl(LPT_STEPPER_IOCS_POSITION, (unsigned long)&fDirPosition[0]);
}

/*! @brief: sets the stepper motor delay for according direction
 *  @param desired direction of motor           (eDirection)
 *  @param delay for this motor in microseconds (long) */
void LPTPort::setDelay(stepper::eDirection aDir, long aDelay_us)
{
    long fDelay[2] = { aDir, aDelay_us };
    Ioctl(LPT_STEPPER_IOCS_DELAY, (unsigned long)&fDelay[0]);
}

/*! @brief: sets the stepper motor start delay for according direction
 *  @param desired direction of motor           (eDirection)
 *  @param delay for this motor in microseconds (long) */
void LPTPort::setStartDelay(stepper::eDirection aDir, long aDelay_us)
{
    long fDelay[2] = { aDir, aDelay_us };
    Ioctl(LPT_STEPPER_IOCS_START_DELAY, (unsigned long)&fDelay[0]);
}

/*! @brief: sets the stepper motor ramp steps for according direction
 *  @param desired direction of motor           (eDirection)
 *  @param ramp steps for this motor (long) */
void LPTPort::setRampSteps(stepper::eDirection aDir, long aSteps)
{
    long fDelay[2] = { aDir, aSteps };
    Ioctl(LPT_STEPPER_IOCS_RAMP_STEPS, (unsigned long)&fDelay[0]);
}

/*! @brief: retrieves the stepper position of the motor direction
 *  @param arg desired Direction (eDirection)
 *  @return Position of the motor direction (long)  */
long LPTPort::getPosition(stepper::eDirection aDir)
{
    return Ioctl(LPT_STEPPER_IOCQ_POSITION, (unsigned long)aDir);
}

/*! @brief: sets the bit to enable or disable all motors
 *  @param arg number of bit (1 - 12)  (unsigned long)
 *  @hint a negative pin value inverts the polarity */
void LPTPort::setEnableMotorsBit(long aBit)
{
    Ioctl(LPT_STEPPER_IOCT_ENABLE_MOTORS_BIT, aBit);
}

/*! @brief: set asynchron delegation of motor control functions to a work queue
 *  @param arg 0: disable, 1: enable asynchron delegation (unsigned long)
 *             2: enables asynchron delegation but waits in a semaphore */
void LPTPort::setAsynchron(long aAsync)
{
    Ioctl(LPT_STEPPER_IOCT_SET_ASYNC, aAsync);
}

/*! @brief: calls the function move to with x and y aruments
 *  @param aX new x position (long)
 *  @param aY new y position (long)
 *  @param [aRelative] move relative to current position (bool) */
void LPTPort::moveTo(long aX, long aY, bool aRelative)
{
    long fPosition[2] = { aX, aY };
    Ioctl(aRelative ? LPT_STEPPER_IOCS_MOVE_RELATIVE : LPT_STEPPER_IOCS_MOVE_TO, (unsigned long)&fPosition[0]);
}

/*! @brief: calls the function line to with x and y aruments
 *  @param aX new x position (long)
 *  @param aY new y position (long)
 *  @param [aRelative] draw line relative to current position (bool) */
void LPTPort::lineTo(long aX, long aY, bool aRelative)
{
    long fPosition[2] = { aX, aY };
    Ioctl(aRelative ?  LPT_STEPPER_IOCS_LINE_RELATIVE : LPT_STEPPER_IOCS_LINE_TO, (unsigned long)&fPosition[0]);
}

/*! @brief: calls the function line to 3D with x, y and z aruments
 *  @param aX new x position (long)
 *  @param aY new y position (long)
 *  @param aZ new z position (long) */
void LPTPort::lineTo3D(long aX, long aY, long aZ)
{
    long fPosition[3] = { aX, aY, aZ };
    Ioctl(LPT_STEPPER_IOCS_LINE_TO_3D, (unsigned long)&fPosition[0]);
}

/*! @brief: move to the predefined pen position
 *  @param aPenPos predefined pen position  (ePenPos) */
void LPTPort::moveToPenPos(stepper::ePenPos aPenPos)
{
    Ioctl(LPT_STEPPER_IOCT_MOVE_TO_PEN_POS, aPenPos);
}

