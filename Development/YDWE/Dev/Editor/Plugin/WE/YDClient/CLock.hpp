#pragma once

#include <Windows.h>

template <class T> 
class CScopedLock 
{
private:
    T *m_pLock;
public:
    CScopedLock(T &l) 
    {
        m_pLock = &l;
        m_pLock->Lock();
    }
    ~CScopedLock() 
    {
        if (m_pLock != NULL)
        {
            m_pLock->Unlock();
        }
    }
};

class CFastLock
{
private:
    CRITICAL_SECTION m_cs; 
public:
    CFastLock()
    {
        ::InitializeCriticalSection(&m_cs);
    }
    ~CFastLock()
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

#define AUTO_LOCK(l) CScopedLock<CFastLock> _tmp_ ## l(l)
