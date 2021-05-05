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
    void operate(int aInt, QVariant);

private:
    std::function< QVariant (int, QVariant) > mWorkerFunction;
};


class WorkerThreadConnector : public QObject
{
    Q_OBJECT
public:
    WorkerThreadConnector(QObject*aParent);
    virtual ~WorkerThreadConnector();

    void sendMessage(int, QVariant);
    void setWorkerFunction(const boost::function< QVariant (int, const QVariant&) >& aFunc);
    void setMessageFunction(const boost::function< void (int, QVariant) >& aFunc);

public Q_SLOTS:
    void doWork(int, const QVariant&);
    void receiveMessage(int, QVariant);

Q_SIGNALS:
    void operate(int, QVariant);

private:
    Worker* mWorker;
    QThread mWorkerThread;
    boost::function< void (int, const QVariant&) > mMessageFunction;
};

#endif // WORKERTHREADCONNECTOR_H

