#pragma once

#if defined WIN32
#	include "logging_winapi.h"
#else
#	include <sys/time.h>
#endif

#include <stdint.h>
#include <time.h>

namespace logging { namespace datetime {
#if defined WIN32
	using winapi::timeval;
	inline uint64_t filetime_to_unix_epoch(const winapi::FILETIME *ft)
	{
		uint64_t res = (uint64_t)ft->dwHighDateTime << 32;
		res |= ft->dwLowDateTime;
		res /= 10; /* from 100 nano-sec periods to usec */
		res -= 11644473600000000Ui64; /* from Win epoch to Unix epoch */
		return (res);
	}
	inline int gettimeofday(struct timeval *tv, void *tz)
	{
		if (!tv) {
			//errno = EINVAL;
			return (-1);
		}
		winapi::FILETIME ft;
		winapi::GetSystemTimeAsFileTime(&ft);
		uint64_t tim = filetime_to_unix_epoch(&ft);
		tv->tv_sec = (long)(tim / 1000000L);
		tv->tv_usec = (long)(tim % 1000000L);
		return (0);
	}
#endif
	inline void localtime(time_t* now, tm* tm_now)
	{
#if defined WIN32
		::localtime_s(tm_now, now);
#else
		::localtime_r(now, tm_now);
#endif
	}

	inline struct tm now(long* usec = 0)
	{
		struct tm tm_now;
		struct timeval tv;
		gettimeofday(&tv, 0);
		time_t now = tv.tv_sec;
		localtime(&now, &tm_now);
		if (usec) *usec = tv.tv_usec;
		return tm_now;
	}
}}
