
#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include "asyncqueue.h"

#include <QThread>
#include <QByteArray>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>


struct ProtocolTypes
{
    enum Type
    {
        UDP,
        Multicast,
        TCP
    };
};

class ReceiverThread : public QThread
{
    Q_OBJECT

    QMutex                mMutex;
    QUdpSocket*            mUDPSocket;
    QTcpServer*         mTCPServer;
    QTcpSocket*            mChildSocket;

    bool                mStopFlag;
    bool                mSocketInitialized;
    ProtocolTypes::Type    mProtocolType;
    int                    mPort;
    QHostAddress        mAddress;
    
    void run() Q_DECL_OVERRIDE;
    
public:
    AsyncQueue<QByteArray>      mDatagramQueue;

    void stop();
    void changeProtocol(ProtocolTypes::Type);
    void changePort(int aPort);
    void changeAddress(const QHostAddress&);

    QHostAddress getAddress();

    ReceiverThread(QObject* aParent);
    virtual ~ReceiverThread();

};


#endif
