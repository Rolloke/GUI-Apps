#include "DirectIoDevice.h"


DirectIODevice::DirectIODevice(QObject *parent) : QIODevice(parent)
{

}

bool DirectIODevice::open(OpenMode mode)
{
    return QIODevice::open(mode);
}

void DirectIODevice::close()
{
    QIODevice::close();
}

bool DirectIODevice::isSequential() const
{
    return true;
}

qint64 DirectIODevice::readData(char *aData, qint64 aMaxLen)
{
    if (openMode() & ReadOnly)
    {
        qint64 fBytesRead = -1;
        Q_EMIT read_data(aData, aMaxLen, fBytesRead);
        return fBytesRead;
    }
    return -1;
}

qint64 DirectIODevice::writeData(const char *aData, qint64 aBytes)
{
    if (openMode() & WriteOnly)
    {
        qint64 fBytesWritten = -1;
        Q_EMIT write_data(aData, aBytes, fBytesWritten);
        return fBytesWritten;
    }
    return -1;
}
