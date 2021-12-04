#include "workerthreadconnector.h"

Worker::Worker(): mIsBusy(false)
{
}

void Worker::operate(int aWorkID, QVariant data)
{
    if (mWorkerFunction)
    {
        mIsBusy = true;
        QThread::msleep(1);
        QVariant result = mWorkerFunction(aWorkID, data);
        Q_EMIT sendMessage(aWorkID, result);
        mIsBusy = false;
    }
}

WorkerThreadConnector::WorkerThreadConnector(QObject*aParent):
    QObject(aParent)
{
    Worker*fWorker = new Worker;
    fWorker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, SIGNAL(finished()), fWorker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate(int, QVariant)), fWorker, SLOT(operate(int, QVariant)));
    connect(fWorker, SIGNAL(sendMessage(int, QVariant)), this, SLOT(receiveMessage(int, QVariant)));

    mWorker = fWorker;
    mWorkerThread.start();

}

WorkerThreadConnector::~WorkerThreadConnector()
{
    mWorkerThread.quit();
    mWorkerThread.wait();
}


void WorkerThreadConnector::setWorkerFunction(const boost::function< QVariant (int, const QVariant&) >& aFunc)
{
    if (mWorker)
    {
        mWorker->mWorkerFunction = aFunc;
    }
}

void WorkerThreadConnector::doWork(int aWorkID, const QVariant& data)
{
    Q_EMIT operate(aWorkID, data);
}

bool WorkerThreadConnector::isBusy()
{
    return mWorker->isBusy();
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
