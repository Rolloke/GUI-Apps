#ifndef WORKERTHREADCONNECTOR_H
#define WORKERTHREADCONNECTOR_H

#include <QThread>
#include <QVariant>

#ifdef USE_BOOST
#include <boost/function.hpp>
#else
#include <functional>
#endif

class Worker : public QObject
{
    Q_OBJECT

public:
    static constexpr char command_id[] = "commandid";
    static constexpr char command[]    = "command";
    static constexpr char result[]     = "result";
    static constexpr char action[]     = "action";
    static constexpr char flags[]      = "flags";
    static constexpr char work[]       = "work";
    static constexpr char repository[] = "repository";

    Worker();
    friend class WorkerThreadConnector;
    bool isBusy() { return mIsBusy; }


Q_SIGNALS:
    void sendMessage(QVariant);

public Q_SLOTS:
    void operate(QVariant);

private:
    std::function< QVariant (QVariant)> mWorkerFunction;
    volatile bool mIsBusy;
};


class WorkerThreadConnector : public QObject
{
    Q_OBJECT
public:
    WorkerThreadConnector(QObject*aParent);
    virtual ~WorkerThreadConnector();

#ifdef USE_BOOST
    void setWorkerFunction(const boost::function< QVariant (const QVariant&) >& aFunc);
    void setMessageFunction(const boost::function< void (QVariant) >& aFunc);
#else
    void setWorkerFunction(const std::function< QVariant (const QVariant&) >& aFunc);
    void setMessageFunction(const std::function< void (QVariant) >& aFunc);
#endif
    bool isBusy();
    void setOnceBusy();
    void setAppendToBatch(bool append);
    const QString& getCurrentCmdName();
    QString        getBatchToolTip();
    bool hasBatchList();
    void clearBatchList();

public Q_SLOTS:
    void doWork(const QVariant&);
    void receiveMessage(QVariant);

Q_SIGNALS:
    void operate(QVariant);

private:
    bool    appendToBatch();
    bool    mOnceBusy;
    Worker* mWorker;
    QThread mWorkerThread;
    QString mCurrentCmdName;
#ifdef USE_BOOST
    boost::function< void (const QVariant&) > mMessageFunction;
#else
    std::function< void (const QVariant&) > mMessageFunction;
#endif
    bool    mAppendToBatch;
    QList<QVariant> mBatch;
};

#endif // WORKERTHREADCONNECTOR_H

