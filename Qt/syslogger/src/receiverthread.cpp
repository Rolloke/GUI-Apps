
#include "receiverthread.h"

#include <QMutexLocker>

ReceiverThread::ReceiverThread(QObject* aParent)
: QThread(aParent)
, mUDPSocket(0)
, mStopFlag(false)
, mPort(58514)
, mDatagramQueue(2000)
{
}


ReceiverThread::~ReceiverThread()
{
}


void ReceiverThread::stop()
{
    mStopFlag = true;
}


void ReceiverThread::changeUDPPort(int aPort)
{
    QMutexLocker fGuard(&mMutex);
    mPort = aPort;
}

void ReceiverThread::run()
{      
    mUDPSocket = new QUdpSocket(this);

    while( !mStopFlag )
    {
        {
            QMutexLocker fGuard(&mMutex);
            if(mPort > 0)
            {
                mUDPSocket->close();
                mUDPSocket->bind(QHostAddress::Any, mPort, QUdpSocket::DefaultForPlatform);
                mPort = -1;
            }
        }
                    
        mUDPSocket->waitForReadyRead(20);
        while (mUDPSocket->hasPendingDatagrams()) 
        {
            QByteArray fDatagram;
            qint64 fSize = mUDPSocket->pendingDatagramSize(); 

            fDatagram.resize(fSize);
            mUDPSocket->readDatagram(fDatagram.data(), fSize );       
            mDatagramQueue.push(fDatagram, 50);
        }
        
        yieldCurrentThread();
    }
}
