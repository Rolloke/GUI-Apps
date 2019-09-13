#include "serialport.h"

#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions


using namespace std;

SerialPort::SerialPort():
  mBaud(0)
, mFile(-1)
{

}

SerialPort::~SerialPort()
{
    close();
}

void SerialPort::setPortName(QString aPort)
{
    mPort = aPort;
}

void SerialPort::setBaudRate(quint32 aBaud)
{
    mBaud = aBaud;
}

void SerialPort::setDataBits(char aBits)
{
    mBit = aBits;
}

void SerialPort::setParity(char aParity)
{
    mParity = aParity;
}

void SerialPort::setStopBits(char aStopBits)
{
    mStopbits = aStopBits;
}

bool SerialPort::isOpen()
{
    return mFile != -1;
}

bool SerialPort::open(int mode)
{
    int fFlag = O_NONBLOCK | O_NDELAY;
    if ((mode & QIODevice::ReadWrite) == QIODevice::ReadOnly)       fFlag |= O_RDONLY;
    else if ((mode & QIODevice::ReadWrite) == QIODevice::WriteOnly) fFlag |= O_WRONLY;
    else if ((mode & QIODevice::ReadWrite) == QIODevice::ReadWrite) fFlag |= O_RDWR;

    mFile = ::open(mPort.toStdString().c_str(),  fFlag);
    if (mFile != -1)
    {
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        // Error Handling
        if ( tcgetattr ( mFile, &tty ) != 0 )
        {
            //cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
        }

        speed_t fSpeed = 0;
        switch (mBaud)
        {
        case 1200:   fSpeed = B1200; break;
        case 1800:   fSpeed = B1800; break;
        case 2400:   fSpeed = B2400; break;
        case 4800:   fSpeed = B4800; break;
        case 9600:   fSpeed = B9600; break;
        case 19200:  fSpeed = B19200; break;
        case 38400:  fSpeed = B38400; break;
        case 57600:  fSpeed = B57600; break;
        case 115200: fSpeed = B115200; break;
        default:     fSpeed = B9600; break;
        }

        // Set Baud Rate
        cfsetospeed (&tty, fSpeed);
        cfsetispeed (&tty, fSpeed);

        tty.c_cflag     &=  ~PARENB;        // Make 8n1
        tty.c_cflag     &=  ~CSTOPB;
        tty.c_cflag     &=  ~CSIZE;
        tty.c_cflag     |=  CS8;
        tty.c_cflag     &=  ~CRTSCTS;       // no flow control
        tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
        tty.c_oflag     =   0;                  // no remapping, no delays
        tty.c_cc[VMIN]      =   0;                  // read doesn't block
        tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

        tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
        tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
        tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
        tty.c_oflag     &=  ~OPOST;              // make raw

        // Flush Port, then applies attributes
        tcflush( mFile, TCIFLUSH );

        if ( tcsetattr ( mFile, TCSANOW, &tty ) != 0)
        {
        //cout << "Error " << errno << " from tcsetattr" << endl;
        }

        return true;
    }
    return false;
}

void SerialPort::close()
{
    if (mFile != -1)
    {
        ::close(mFile);
        mFile = -1;
    }
}

bool SerialPort::isSequential() const
{
    return true;
}

QByteArray SerialPort::read(int aLength)
{
    mArray.resize(aLength);

    int fReadLength = ::read(mFile, mArray.data(), mArray.size());
    mArray.resize(fReadLength);

    return mArray;
}

ssize_t SerialPort::write(const QString& aBuffer)
{
    return ::write(mFile, aBuffer.toStdString().c_str(), aBuffer.length());
}

QString  SerialPort::errorString()
{
    // TODO
    return "";
}


