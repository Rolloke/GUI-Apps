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
    QObject(aParent), mOnceBusy(false), mAppendToBatch(false)
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
    if (isBusy() && appendToBatch())
    {
        mBatch.append(QPair<int, QVariant>(aWorkID, data));
        return;
    }

    mCurrentCmdName = data.toString();
    Q_EMIT operate(aWorkID, data);
}

/// brief force next command executed synchroneously
void WorkerThreadConnector::setOnceBusy()
{
    mOnceBusy = true;
}

void WorkerThreadConnector::setAppendToBatch(bool append)
{
    mAppendToBatch = append;
    if (!appendToBatch())
    {
        mBatch.clear();
    }
}

const QString & WorkerThreadConnector::getCurrentCmdName()
{
    return mCurrentCmdName;
}

bool WorkerThreadConnector::appendToBatch()
{
    return mAppendToBatch;
}

bool WorkerThreadConnector::isBusy()
{
    if (mOnceBusy)
    {
        mOnceBusy = false;
        return true;
    }
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
    mCurrentCmdName.clear();
    if (mMessageFunction)
    {
        mMessageFunction(aMsg, aData);
    }
    if (appendToBatch() && mBatch.size())
    {
        auto& element = mBatch.front();
        mCurrentCmdName = element.second.toString();
        Q_EMIT operate(element.first, element.second);
        mBatch.pop_front();
    }
}
