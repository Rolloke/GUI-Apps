#ifndef SERIALPORT_H
#define SERIALPORT_H


#include <QIODevice>


class SerialPort
{
public:
    explicit SerialPort();

    virtual ~SerialPort();

    bool open(int);
    void close();
    bool isSequential() const;
    bool isOpen();

    void setPortName(QString aPort);
    void setBaudRate(quint32 aBaud);
    void setDataBits(char aBits);
    void setParity(char aParity);
    void setStopBits(char stopBits);

    QByteArray read(int aLength);
    ssize_t  write(const QString& aBuffer);
    QString  errorString();

private:

    QString mPort;
    quint32 mBaud;
    char    mBit;
    char    mParity;
    char    mStopbits;
    QByteArray mArray;
    int mFile;
};

#endif // SERIALPORT_H

