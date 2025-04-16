#include "memorypool.h"
#include <algorithm>
#include "main.h"

using namespace std;

MemoryPool::Pool::Pool(int aSizeBytes) :mPointer(new uint8_t[aSizeBytes]), mFlag(true)
{

}

MemoryPool::MemoryPool() : mBufferSize(0)
{

}

void MemoryPool::initialize(int aBuffers, int aSizeBytes)
{
    mBuffers.clear();
    for (int i=0; i<aBuffers; ++i)
    {
        Pool fPool(aSizeBytes);
        mBuffers[fPool.mPointer.get()] = fPool;
    }
    mBufferSize = aSizeBytes;
}

void*  MemoryPool::getBuffer()
{
    QMutexLocker fLock(&mMutex);
    for (PoolMap::value_type& fBuffer : mBuffers)
    {
        if (fBuffer.second.mFlag)
        {
            fBuffer.second.mFlag = false;
            return fBuffer.second.mPointer.get();
        }
    }
    return 0;
}

void MemoryPool::releaseBuffer(void* aPB)
{
    QMutexLocker fLock(&mMutex);
    PoolMap::iterator fBuffer = mBuffers.find(aPB);
    if (fBuffer != mBuffers.end())
    {
        fBuffer->second.mFlag = true;
    }
}

bool MemoryPool::isInitialized()
{
    return mBuffers.size() > 0;
}

const QMutex& MemoryPool::getMutex()
{
    return mMutex;
}

int MemoryPool::getBufferSize() const
{
    return mBufferSize;
}

int MemoryPool::getPoolSize() const
{
    return mBuffers.size();
}

int MemoryPool::getFreePoolItems()
{
    int fCount = 0;
    QMutexLocker fLock(&mMutex);
    for (PoolMap::value_type& fBuffer : mBuffers)
    {
        if (fBuffer.second.mFlag) ++fCount;
    }
    return fCount;
//    return count_if(mBuffers.begin(), mBuffers.end(), [] ( MemoryPool::PoolMap::const_iterator fBuffer) { return fBuffer.second.mFlag != false; } );
}

