#include "workerthreadconnector.h"

Worker::Worker()
{

}

void Worker::operate(int aInt)
{
    if (mWorkerFunction)
    {
        mWorkerFunction(aInt);
    }
}

WorkerThreadConnector::WorkerThreadConnector(QObject*aParent):
    QObject(aParent)
{
    Worker*fWorker = new Worker;
    fWorker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, SIGNAL(finished()), fWorker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate(int)), fWorker, SLOT(operate(int)));
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


void WorkerThreadConnector::doWork(int aInt)
{
    Q_EMIT operate(aInt);
}

