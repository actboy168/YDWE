#pragma once

#include <Windows.h>
#include <intrin.h>

#pragma intrinsic( _InterlockedExchange8 )
#pragma intrinsic( _ReadWriteBarrier )

#define COMPILER_RW_BARRIER  _ReadWriteBarrier()

namespace base { namespace lock { namespace os {

	inline void sleep0()
	{
		::SwitchToThread();
	}

	inline void sleep1()
	{
		::Sleep(1);
	}
	
	inline void nop()
	{
		__nop();
	}
	
	inline unsigned int getcpu()
	{
		SYSTEM_INFO info;
		::GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}
}}}
