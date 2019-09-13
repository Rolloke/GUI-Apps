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

Q_SIGNALS:
    void sendMessage(int, QVariant);

public Q_SLOTS:
    void operate(int aInt);

private:
    boost::function< void (int) > mWorkerFunction;
};


class WorkerThreadConnector : public QObject
{
    Q_OBJECT
public:
    WorkerThreadConnector(QObject*aParent);
    virtual ~WorkerThreadConnector();

    void sendMessage(int, QVariant);
    void setWorkerFunction(const boost::function< void (int) >& aFunc);
    void setMessageFunction(const boost::function< void (int, QVariant) >& aFunc);

public Q_SLOTS:
    void doWork(int );
    void receiveMessage(int, QVariant);

Q_SIGNALS:
    void operate(int );

private:
    Worker* mWorker;
    QThread mWorkerThread;
    boost::function< void (int, QVariant) > mMessageFunction;
};

#endif // WORKERTHREADCONNECTOR_H

