
#ifndef ASYNCQUEUE_H
#define ASYNCQUEUE_H

#include <QQueue>
#include <QMutex>

#include <stdint.h>
#include <limits>

template<class T> class AsyncQueue
{
private:
    QQueue<T>       mQueue;
    QMutex          mMutex;
    int32_t    mMaxSize;

public:
    AsyncQueue(uint32_t aMax = std::numeric_limits<int32_t>::max() )
    : mMaxSize(aMax)
    {
    }

    ~AsyncQueue()
    {
        clean();
    }

    uint32_t count()
    {
        mMutex.lock();
        uint32_t fCount = mQueue.count();
        mMutex.unlock();
        return fCount;
    }

    bool isFull()
    {
        mMutex.lock();
        int32_t fCount = mQueue.count();
        mMutex.unlock();
        return fCount > mMaxSize;
    }

    bool isEmpty()
    {
        mMutex.lock();
        bool fEmpty = mQueue.isEmpty();
        mMutex.unlock();
        return fEmpty;
    }

    void clean()
    {
        mMutex.lock();
        mQueue.clear(); 
        mMutex.unlock();
    }

    bool push(const T& aElement, int aTimeOut)
    {
        bool fResult = mMutex.tryLock(aTimeOut);
        if(fResult)
        {
            if(mQueue.count() <= mMaxSize)
            {
                mQueue.enqueue(aElement);
            }
            else
            {
                fResult = false;
            }
            mMutex.unlock();
        }
        return fResult;
    }

    bool pull(T& aElement, int aTimeOut)
    {
        bool fResult = mMutex.tryLock(aTimeOut);
        if(fResult)
        {
            if(!mQueue.isEmpty())
            {
                aElement = mQueue.dequeue();
            }
            else
            {
                fResult = false;
            }
            mMutex.unlock();
        }
        return fResult;
    }

};





#endif 

