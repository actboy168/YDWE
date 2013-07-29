#pragma once

#include <Windows.h>

template <class T> 
class ScopedLock 
{
private:
    T *m_pLock;
public:
    ScopedLock(T &l) 
    {
        m_pLock = &l;
        m_pLock->Lock();
    }
    ~ScopedLock() 
    {
        if (m_pLock != NULL)
        {
            m_pLock->Unlock();
        }
    }
};

class FastLock
{
private:
    CRITICAL_SECTION m_cs; 
public:
    FastLock()
    {
        ::InitializeCriticalSectionAndSpinCount(&m_cs, 2000);
    }
    ~FastLock()
    {
        ::DeleteCriticalSection(&m_cs);
    }
    void Lock()
    {
        ::EnterCriticalSection(&m_cs);
    }
    void Unlock()
    {
        ::LeaveCriticalSection(&m_cs);
    }
    bool TryLock()
    {
        return (FALSE != ::TryEnterCriticalSection(&m_cs));
    }
    bool IsLocked()
    {
        BOOL bRet = ::TryEnterCriticalSection(&m_cs);

        if (bRet)
        {
            Unlock();
            return false;
        }
        else
        {
            return true;
        }
    }
};

#define AUTO_LOCK(l) ScopedLock<FastLock> _tmp_ ## l(l)
