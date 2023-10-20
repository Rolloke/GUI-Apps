#include "workerthreadconnector.h"
#include "logger.h"

Worker::Worker(): mIsBusy(false)
{
}

void Worker::operate(QVariant data)
{
    if (mWorkerFunction)
    {
        mIsBusy = true;
        QThread::msleep(1);
        QVariant result = mWorkerFunction(data);
        Q_EMIT sendMessage(result);
        mIsBusy = false;
    }
}

WorkerThreadConnector::WorkerThreadConnector(QObject*aParent):
    QObject(aParent), mOnceBusy(false), mAppendToBatch(false)
{
    Worker*fWorker = new Worker;
    fWorker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, SIGNAL(finished()), fWorker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate(QVariant)), fWorker, SLOT(operate(QVariant)));
    connect(fWorker, SIGNAL(sendMessage(QVariant)), this, SLOT(receiveMessage(QVariant)));

    mWorker = fWorker;
    mWorkerThread.start();

}

WorkerThreadConnector::~WorkerThreadConnector()
{
    mWorkerThread.quit();
    mWorkerThread.wait();
}

void WorkerThreadConnector::setWorkerFunction(const boost::function<QVariant (const QVariant&) >& aFunc)
{
    if (mWorker)
    {
        mWorker->mWorkerFunction = aFunc;
    }
}

void WorkerThreadConnector::doWork(const QVariant& data)
{
    if (data.isValid())
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (data.typeId() == QMetaType::QVariantMap)
#else
        if (data.type() == QVariant::Map)
#endif
        {
            if (isBusy() && appendToBatch())
            {
                TRACEX(Logger::to_browser, "append \"" << data.toMap()[Worker::command].toString() << "\" to background batch");
                mBatch.append(data);
                return;
            }

            mCurrentCmdName = data.toMap()[Worker::command].toString();
            TRACEX(Logger::to_browser, "running \"" << mCurrentCmdName << "\" in background");
            Q_EMIT operate(data);
        }
        else
        {
            TRACEX(Logger::to_browser, "Error, " << __FUNCTION__ << " data type is wrong");
        }
    }
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

QString WorkerThreadConnector::getBatchToolTip()
{
    QString tool_tip = tr("Background commands");
    if (mCurrentCmdName.isEmpty() && mBatch.size() == 0)
    {
    }
    else
    {
        tool_tip += tr("\n:currently running: %1\n").arg(mCurrentCmdName);
        if (mBatch.size())
        {
            tool_tip += "Batch list:\n";
            const auto &batch_list = mBatch;
            for (const auto & batch : batch_list)
            {
                tool_tip += "- ";
                tool_tip += batch.toMap()[Worker::command].toString();
                tool_tip += "\n";
            }
        }
    }
    return tool_tip;
}

void WorkerThreadConnector::clearBatchList()
{
    mBatch.clear();
}

bool WorkerThreadConnector::hasBatchList()
{
    return mBatch.size() > 0;
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
    return mWorker->isBusy() || mBatch.size() > 0;
}


void WorkerThreadConnector::setMessageFunction(const boost::function<void (QVariant)> &aFunc)
{
    mMessageFunction = aFunc;
}

void WorkerThreadConnector::receiveMessage(QVariant aData)
{
    mCurrentCmdName.clear();
    if (mMessageFunction)
    {
        mMessageFunction(aData);
    }
    if (mBatch.size())
    {
        auto& batch_command = mBatch.front();
        mCurrentCmdName = batch_command.toMap()[Worker::command].toString();
        Q_EMIT operate(batch_command);
        mBatch.pop_front();
    }
}
