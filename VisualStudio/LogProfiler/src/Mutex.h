#ifndef MUTEX_INCLUDED
#define MUTEX_INCLUDED

#include ".\DockingFeature\Window.h"

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    BOOL trylock();
    void unlock();
    DWORD setSpinCount(DWORD aCount);

private:
    Mutex(const Mutex& ) = delete;
	Mutex& operator (const Mutex&) = delete;
	
    CRITICAL_SECTION mCS;
};

class AutoMutex
{
public:
    AutoMutex(Mutex* aM);
    ~AutoMutex();
    void unlock();
private:
    Mutex* mMutex;
};

#endif // MUTEX_INCLUDED
