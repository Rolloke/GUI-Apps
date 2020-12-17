
#include "receiverthread.h"

#include <QMutexLocker>


ReceiverThread::ReceiverThread(QObject* aParent)
: QThread(aParent)
, mDatagramQueue(2000)
, mUDPSocket(0)
, mTCPServer(0)
, mChildSocket(0)
, mStopFlag(false)
, mSocketInitialized(false)
, mProtocolType(ProtocolTypes::UDP)
, mPort(58514)
, mAddress("224.0.0.7")
{
}


ReceiverThread::~ReceiverThread()
{
}


void ReceiverThread::stop()
{
    mStopFlag = true;
}


void ReceiverThread::changeProtocol(ProtocolTypes::Type aType)
{
    QMutexLocker fGuard(&mMutex);

    mProtocolType = aType;
    mSocketInitialized = false;
}

void ReceiverThread::changePort(int aPort)
{
    QMutexLocker fGuard(&mMutex);
    mPort = aPort;
    mSocketInitialized = false;
}


void ReceiverThread::changeAddress(const QHostAddress& aAddress)
{
    QMutexLocker fGuard(&mMutex);
    mAddress = aAddress;
    mSocketInitialized = false;
}


QHostAddress ReceiverThread::getAddress()
{
    QHostAddress fCurrentAddress;
    
    QMutexLocker fGuard(&mMutex);
    fCurrentAddress = mAddress;

    return fCurrentAddress;
}

void ReceiverThread::run()
{      
    mUDPSocket = new QUdpSocket(this);
    mTCPServer = new QTcpServer(this);


    while( !mStopFlag )
    {
        {
            QMutexLocker fGuard(&mMutex);
            if(!mSocketInitialized)
            {
                if(mProtocolType != ProtocolTypes::TCP )
                {
                    if (mChildSocket)
                    {
                        mChildSocket->close();
                        delete mChildSocket;
                        mChildSocket = 0;
                    }
                    mTCPServer->close();
                    mUDPSocket->close();
                    mUDPSocket->bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ReuseAddressHint);
                    if (mProtocolType == ProtocolTypes::Multicast)
                    {
                        mUDPSocket->joinMulticastGroup(QHostAddress("224.0.0.7"));
                    }
                    mSocketInitialized = true;
                }
                else
                {
                    if (mChildSocket)
                    {
                        mChildSocket->close();
                        delete mChildSocket;
                        mChildSocket = 0;
                    }
                    mTCPServer->close();
                    mUDPSocket->close();
                    mTCPServer->listen(QHostAddress::AnyIPv4, mPort);
                    mSocketInitialized = true;
                }
            }
        }
        if (mSocketInitialized)
        {

            if (mProtocolType != ProtocolTypes::TCP)
            {
                mUDPSocket->waitForReadyRead(20);
                while (mUDPSocket->hasPendingDatagrams())
                {
                    QByteArray fDatagram;
                    qint64 fSize = mUDPSocket->pendingDatagramSize();

                    fDatagram.resize(fSize);
                    mUDPSocket->readDatagram(fDatagram.data(), fSize);
                    mDatagramQueue.push(fDatagram, 50);
                }
            }
            else
            {
                if (!mChildSocket)
                {
                    mTCPServer->waitForNewConnection(20);
                    while (mTCPServer->hasPendingConnections())
                    {
                        mChildSocket = mTCPServer->nextPendingConnection();
                    }
                }
                else
                {
                    if (mChildSocket->state() == QAbstractSocket::ConnectedState)
                    {
                        mChildSocket->waitForReadyRead(20);
                        qint64 fSize = mChildSocket->bytesAvailable();
                        if (fSize > 0)
                        {
                            QByteArray fDatagram;
                            fDatagram.resize(fSize);
                            mChildSocket->read(fDatagram.data(), fSize);
                            mDatagramQueue.push(fDatagram, 50);
                        }
                    }
                    else
                    {
                        mChildSocket->close();
                        delete mChildSocket;
                        mChildSocket = 0;
                    }
                }

            }
        }
        yieldCurrentThread();
    }
}