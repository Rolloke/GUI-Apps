#ifndef WORKERTHREADCONNECTOR_H
#define WORKERTHREADCONNECTOR_H

#include <QThread>
#include <QVariant>

#include <boost/function.hpp>

class Worker : public QObject
{
    Q_OBJECT

public:
    static constexpr char command[] = "command";
    static constexpr char result[]  = "result";
    static constexpr char action[]  = "action";
    static constexpr char flags[]   = "flags";
    static constexpr char work[]    = "work";

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

    void setWorkerFunction(const boost::function< QVariant (const QVariant&) >& aFunc);
    void setMessageFunction(const boost::function< void (QVariant) >& aFunc);
    bool isBusy();
    void setOnceBusy();
    void setAppendToBatch(bool append);
    const QString& getCurrentCmdName();
    QString        getBatchToolTip();

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
    boost::function< void (const QVariant&) > mMessageFunction;
    bool    mAppendToBatch;
    QList<QVariant> mBatch;
};

#endif // WORKERTHREADCONNECTOR_H

