#pragma once

#if defined(_DEBUG)

namespace
{
	/// See <http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx> 
	/// and <http://blogs.msdn.com/b/stevejs/archive/2005/12/19/505815.aspx> for
	/// more information on the code below.

	const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD  dwType;     // Must be 0x1000.
		LPCSTR szName;     // Pointer to name (in user addr space).
		DWORD  dwThreadID; // Thread ID (-1=caller thread).
		DWORD  dwFlags;    // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void setThreadName(DWORD dwThreadID, const char* threadName)
	{
		if (IsDebuggerPresent())
		{
			THREADNAME_INFO info;
			info.dwType     = 0x1000;
			info.szName     = threadName;
			info.dwThreadID = dwThreadID;
			info.dwFlags    = 0;

			__try
			{
				RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
			}
			__except (EXCEPTION_CONTINUE_EXECUTION)
			{
			}
		}
	}

	const char* getCurrentThreadName(void)
	{
		const char* pszName = NULL;
		__asm
		{
			mov eax, fs:[0x18]    //    Locate the caller's TIB
			mov eax, [eax+0x14]   //    Read the pvArbitary field in the TIB
			mov [pszName], eax    //    pszName = pTIB->pvArbitary
		}
		return pszName ? pszName : "Win32 Thread";
	}
}

#endif

#include <Windows.h>
#include <process.h>
#include <string>
#include <sstream>

class Runnable
{
public:	
	Runnable() {};
	virtual ~Runnable() {};
	virtual void run() = 0;
};

template <class C>
class RunnableAdapter: public Runnable
{
public:
	typedef void (C::*Callback)();
	RunnableAdapter(C* object, Callback method)
		: _pObject(object), _method(method)
	{ }
	RunnableAdapter(const RunnableAdapter& ra)
		: _pObject(ra._pObject), _method(ra._method) 
	{ }
	~RunnableAdapter() { }
	RunnableAdapter& operator = (const RunnableAdapter& ra)
	{
		_pObject = ra._pObject;
		_method  = ra._method;
		return *this;
	}
	void run()
	{
		(_pObject->*_method)();
	}

	RunnableAdapter()
		: _pObject(nullptr), _method(nullptr)
	{ }
private:
	C*       _pObject;
	Callback _method;
};

class Thread
{
public:	
	typedef unsigned (__stdcall* Callback)(void*);
	struct CallbackData
	{
		CallbackData(): callback(0), pData(0)
		{
		}

		Callback  callback;
		void*     pData; 
	};

public:
	Thread()
		:_thread(NULL), _threadId(0), _pRunnableTarget(nullptr)
#if defined(_DEBUG)
		, _id(uniqueId()), _name(makeName())
#endif
	{ }

	~Thread() { }
	
	bool join()
	{
		if (!_thread) return true;

		switch (::WaitForSingleObject(_thread, INFINITE))
		{
		case WAIT_OBJECT_0:
			cleanup();
			return true;
		default:
			// cannot join thread?
			return false;
		}
	}

	bool join(long milliseconds)
	{
		if (!_thread) return true;
	
		switch (::WaitForSingleObject(_thread, milliseconds))
		{
		case WAIT_TIMEOUT:
			return false;
		case WAIT_OBJECT_0:
			cleanup();
			return true;
		default:
			// cannot join thread?
			return false;
		}
	}
	
	void start(Callback target, void* pData = nullptr)
	{
		if (isRunning()) return ;
	
		cleanup();
		_callbackTarget.callback = target;
		_callbackTarget.pData    = pData;

		unsigned threadId;
		_thread = (HANDLE) _beginthreadex(NULL, 0, callableEntry, this, 0, &threadId);
		_threadId = static_cast<DWORD>(threadId);
	}

	void start(Runnable& target)
	{
		if (isRunning()) return ;

		cleanup();
		_pRunnableTarget = &target;

		unsigned threadId;
		_thread = (HANDLE) _beginthreadex(NULL, 0, runnableEntry, this, 0, &threadId);
		_threadId = static_cast<DWORD>(threadId);
	}

	static void sleep(long milliseconds)
	{
		::Sleep(DWORD(milliseconds));
	}

	void terminate(long code = -1)
	{
		::TerminateThread(_thread, DWORD(code));
	}

	static void yield()
	{
		Thread::sleep(10);
	}

	bool isRunning() const
	{
		if (_thread)
		{
			DWORD ec = 0;
			return ::GetExitCodeThread(_thread, &ec) && ec == STILL_ACTIVE;
		}
		return false;
	}

	void cleanup()
	{
		if (!_thread) return;
		if (::CloseHandle(_thread)) _thread = NULL;
	}

	void setName(const std::string& name)
	{
#if defined(_DEBUG)
		_name = name;
#endif
	}

#if defined(_DEBUG)
	inline const std::string getName() const
	{
		return _name;
	}
#endif

protected:
	DWORD	     _threadId;
	HANDLE	     _thread;
	CallbackData _callbackTarget;
	Runnable*    _pRunnableTarget;

#if defined(_DEBUG)
	int          _id;
	std::string  _name;	
#endif

#if defined(_DEBUG)
	std::string makeName()
	{
		std::ostringstream name;
		name << '#' << _id;
		return name.str();
	}

	int uniqueId()
	{
		static volatile unsigned count = 0;

		return InterlockedIncrement(&count);
	}
#endif

private:	
	static unsigned __stdcall callableEntry(void* pThread)
	{
#if defined(_DEBUG)
		setThreadName(::GetCurrentThreadId(), reinterpret_cast<Thread*>(pThread)->getName().c_str());
#endif
		try
		{
			Thread* pTI = reinterpret_cast<Thread*>(pThread);
			pTI->_callbackTarget.callback(pTI->_callbackTarget.pData);
		}
		catch (...) { }
		reinterpret_cast<Thread*>(pThread)->cleanup();

		return 0;
	}

	static unsigned __stdcall runnableEntry(void* pThread)
	{
#if defined(_DEBUG)
		setThreadName(::GetCurrentThreadId(), reinterpret_cast<Thread*>(pThread)->getName().c_str());
#endif
		try
		{
			reinterpret_cast<Thread*>(pThread)->_pRunnableTarget->run();
		}
		catch (...) { }
		reinterpret_cast<Thread*>(pThread)->cleanup();

		return 0;
	}
};
