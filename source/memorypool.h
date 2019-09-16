#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <map>
#include <memory>
#include <QMutex>
#include <QSemaphore>



class MemoryPool
{
    struct Pool
    {
        Pool(int aSizeBytes=0);
        std::shared_ptr<std::uint8_t> mPointer;
        bool mFlag;
    };

    typedef std::map<void*, Pool> PoolMap;

public:
    MemoryPool();
    void initialize(int aBuffers, int aSizeBytes);

    void*  getBuffer();
    void   releaseBuffer(void *);
    bool   isInitialized();
    const QMutex& getMutex();
    int    getBufferSize() const;
    int    getFreePoolItems();
    int    getPoolSize() const;
private:

    PoolMap                    mBuffers;
    int                        mBufferSize;
    QMutex                     mMutex;
};



#endif // MEMORYPOOL_H
