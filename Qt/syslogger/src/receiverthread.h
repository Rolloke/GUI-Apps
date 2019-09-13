
#ifndef RECEIVERTHREAD_H
#define RECEIVERTHREAD_H

#include "asyncqueue.h"

#include <QThread>
#include <QByteArray>
#include <QUdpSocket>
#include <QMutex>

class ReceiverThread : public QThread
{
    Q_OBJECT

    QMutex          mMutex;
    QUdpSocket*     mUDPSocket;
    bool            mStopFlag;
    int             mPort;
	
    void run() ;
	
public:
    AsyncQueue<QByteArray>      mDatagramQueue;

    void stop();
    void changeUDPPort(int aPort);

    ReceiverThread(QObject* aParent);
    virtual ~ReceiverThread();

};


#endif
