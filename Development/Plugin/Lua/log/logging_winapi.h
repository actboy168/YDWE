#pragma once

#include <winerror.h> 

#ifndef WINAPI
#  define WINAPI __stdcall
#endif
#ifndef NTAPI
#  define NTAPI __stdcall
#endif

namespace logging { namespace winapi {
extern "C" {
    typedef int BOOL;
    typedef unsigned long DWORD;
    typedef void* HMODULE;

    typedef char CHAR;
    typedef CHAR *LPSTR;
    typedef const CHAR *LPCSTR;

    typedef wchar_t WCHAR;
    typedef WCHAR *LPWSTR;
    typedef const WCHAR *LPCWSTR;

# ifdef _WIN64
	typedef __int64 (WINAPI *FARPROC)();
	typedef __int64 (WINAPI *NEARPROC)();
	typedef __int64 (WINAPI *PROC)();
# else
    typedef int (WINAPI *FARPROC)();
    typedef int (WINAPI *NEARPROC)();
    typedef int (WINAPI *PROC)();
# endif // _WIN64

	typedef struct _FILETIME {
		DWORD dwLowDateTime;
		DWORD dwHighDateTime;
	} FILETIME, *PFILETIME, *LPFILETIME;

	struct timeval {
		long    tv_sec;         /* seconds */
		long    tv_usec;        /* and microseconds */
	};
}

}}
