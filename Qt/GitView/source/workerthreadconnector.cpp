#include "workerthreadconnector.h"

Worker::Worker()
{
}

void Worker::operate(int aWorkID)
{
    if (mWorkerFunction)
    {
        Q_EMIT sendMessage(aWorkID, QVariant(true));
        QThread::msleep(1);
        mWorkerFunction(aWorkID);
        Q_EMIT sendMessage(aWorkID, QVariant(false));
    }
}

WorkerThreadConnector::WorkerThreadConnector(QObject*aParent):
    QObject(aParent)
{
    Worker*fWorker = new Worker;
    fWorker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, SIGNAL(finished()), fWorker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate(int)), fWorker, SLOT(operate(int)));
    connect(fWorker, SIGNAL(sendMessage(int,QVariant)), this, SLOT(receiveMessage(int,QVariant)));

    mWorker = fWorker;
    mWorkerThread.start();

}

WorkerThreadConnector::~WorkerThreadConnector()
{
    mWorkerThread.quit();
    mWorkerThread.wait();
}


void WorkerThreadConnector::setWorkerFunction(const boost::function< void (int) >& aFunc)
{
    if (mWorker)
    {
        mWorker->mWorkerFunction = aFunc;
    }
}

void WorkerThreadConnector::doWork(int aWorkID)
{
    Q_EMIT operate(aWorkID);
}


void WorkerThreadConnector::sendMessage(int aMsg, QVariant aData)
{
    Q_EMIT mWorker->sendMessage(aMsg, aData);
}

void WorkerThreadConnector::setMessageFunction(const boost::function< void (int, QVariant) >& aFunc)
{
    mMessageFunction = aFunc;
}


void WorkerThreadConnector::receiveMessage(int aMsg, QVariant aData)
{
    if (mMessageFunction)
    {
        mMessageFunction(aMsg, aData);
    }
}
