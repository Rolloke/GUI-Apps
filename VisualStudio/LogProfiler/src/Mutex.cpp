#include "Mutex.h"



Mutex::Mutex()
{
    InitializeCriticalSection(&mCS);
}


Mutex::~Mutex()
{
    DeleteCriticalSection(&mCS);
}

void Mutex::lock()
{
    EnterCriticalSection(&mCS);
}

BOOL Mutex::trylock()
{
    return TryEnterCriticalSection(&mCS);
}

void Mutex::unlock()
{
    LeaveCriticalSection(&mCS);
}

DWORD Mutex::setSpinCount(DWORD aCount)
{
    return SetCriticalSectionSpinCount(&mCS, aCount);
}

AutoMutex::AutoMutex(Mutex* aM) : mMutex(aM)
{
    mMutex->lock();
}
AutoMutex::~AutoMutex()
{
    if (mMutex)
    {
        mMutex->unlock();
    }
}

void AutoMutex::unlock()
{
    if (mMutex)
    {
        mMutex->unlock();
        mMutex = 0;
    }
}
