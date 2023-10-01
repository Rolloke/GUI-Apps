#ifndef WORKERTHREADCONNECTOR_H
#define WORKERTHREADCONNECTOR_H

#include <QThread>
#include <QVariant>

#include <boost/function.hpp>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    friend class WorkerThreadConnector;
    bool isBusy() { return mIsBusy; }


Q_SIGNALS:
    void sendMessage(int, QVariant);

public Q_SLOTS:
    void operate(int aInt, QVariant);

private:
    std::function< QVariant (int, QVariant) > mWorkerFunction;
    volatile bool mIsBusy;
};


class WorkerThreadConnector : public QObject
{
    Q_OBJECT
public:
    static constexpr char command[] = "command";
    static constexpr char result[]  = "result";
    static constexpr char action[]  = "action";
    static constexpr char flags[]   = "flags";

    WorkerThreadConnector(QObject*aParent);
    virtual ~WorkerThreadConnector();

    void sendMessage(int, QVariant);
    void setWorkerFunction(const boost::function< QVariant (int, const QVariant&) >& aFunc);
    void setMessageFunction(const boost::function< void (int, QVariant) >& aFunc);
    bool isBusy();
    void setOnceBusy();
    void setAppendToBatch(bool append);
    const QString& getCurrentCmdName();
    QString        getBatchToolTip();

public Q_SLOTS:
    void doWork(int, const QVariant&);
    void receiveMessage(int, QVariant);

Q_SIGNALS:
    void operate(int, QVariant);

private:
    bool    appendToBatch();
    bool    mOnceBusy;
    Worker* mWorker;
    QThread mWorkerThread;
    QString mCurrentCmdName;
    boost::function< void (int, const QVariant&) > mMessageFunction;
    bool    mAppendToBatch;
    QList<QPair<int,QVariant>> mBatch;
};

#endif // WORKERTHREADCONNECTOR_H

