#pragma once

#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sched.h>
#include <unistd.h>

#define COMPILER_RW_BARRIER  __asm__ __volatile__ ("" ::: "memory")

namespace base { namespace lock { namespace os {

	inline void sleep0()
	{
		usleep(0);
	}

	inline void sleep1()
	{
		usleep(1000);
	}
	
	inline void nop()
	{
		asm volatile ("nop;");
	}
	
	inline unsigned int getcpu()
	{
		return get_nprocs();
	}
}}}
