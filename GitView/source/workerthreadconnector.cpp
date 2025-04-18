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

#ifdef USE_BOOST
void WorkerThreadConnector::setWorkerFunction(const boost::function<QVariant (const QVariant&) >& aFunc)
#else
void WorkerThreadConnector::setWorkerFunction(const std::function<QVariant (const QVariant&) >& aFunc)
#endif
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
        tool_tip += tr(": none");
    }
    else
    {
        tool_tip += tr("\n:currently running: %1\n").arg(mCurrentCmdName);
        if (mBatch.size())
        {
            tool_tip += "Batch list:\n";
            const auto &batch_list = mBatch;
            for (const auto & batch : std::as_const(batch_list))
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


#ifdef USE_BOOST
void WorkerThreadConnector::setMessageFunction(const boost::function<void (QVariant)> &aFunc)
#else
void WorkerThreadConnector::setMessageFunction(const std::function<void (QVariant)> &aFunc)
#endif
{
    mMessageFunction = aFunc;
}

void WorkerThreadConnector::receiveMessage(QVariant aData)
{
    QVariant batch_command;
    if (mBatch.size())
    {
        batch_command = mBatch.front();
        mCurrentCmdName = batch_command.toMap()[Worker::command].toString();
        mBatch.pop_front();
        mOnceBusy = mBatch.size() == 0;
    }
    else
    {
        mCurrentCmdName.clear();
    }
    if (mMessageFunction)
    {
        mMessageFunction(aData);
    }
    if (batch_command.isValid() && !batch_command.isNull())
    {
        Q_EMIT operate(batch_command);
    }
}
