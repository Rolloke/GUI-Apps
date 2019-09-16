#ifndef WORKERTHREADCONNECTOR_H
#define WORKERTHREADCONNECTOR_H

#include <QObject>
#include <QThread>
#include <boost/function.hpp>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    friend class WorkerThreadConnector;

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

    void setWorkerFunction(const boost::function< void (int) >& aFunc);

public Q_SLOTS:
    void doWork(int );

Q_SIGNALS:
    void operate(int );

private:
    Worker* mWorker;
    QThread mWorkerThread;
};

#endif // WORKERTHREADCONNECTOR_H

