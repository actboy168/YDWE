/*
** $Id: lprefix.h,v 1.2 2014/12/29 16:54:13 roberto Exp $
** Definitions for Lua code that must come before any other header file
** See Copyright Notice in lua.h
*/

#ifndef lprefix_h
#define lprefix_h


/*
** Allows POSIX/XSI stuff
*/
#if !defined(LUA_USE_C89)	/* { */

#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE           600
#elif _XOPEN_SOURCE == 0
#undef _XOPEN_SOURCE  /* use -D_XOPEN_SOURCE=0 to undefine it */
#endif

/*
** Allows manipulation of large files in gcc and some other compilers
*/
#if !defined(LUA_32BITS) && !defined(_FILE_OFFSET_BITS)
#define _LARGEFILE_SOURCE       1
#define _FILE_OFFSET_BITS       64
#endif

#endif				/* } */


/*
** Windows stuff
*/
#if defined(_WIN32) 	/* { */

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS  /* avoid warnings about ISO C functions */
#endif

#ifndef lua_c
#include "build/utf8/utf8_crt.h"
#ifndef lundump_c
#include <Windows.h>
#endif
#define fopen(...) utf8_fopen(__VA_ARGS__)
#define popen(...) utf8_popen(__VA_ARGS__)
#define system(...) utf8_system(__VA_ARGS__)
#define remove(...) utf8_remove(__VA_ARGS__)
#define rename(...) utf8_rename(__VA_ARGS__)
#define getenv(...) utf8_getenv(__VA_ARGS__)
#define LoadLibraryExA(...) utf8_LoadLibraryExA(__VA_ARGS__)
#define GetModuleFileNameA(...) utf8_GetModuleFileNameA(__VA_ARGS__)
#endif

#endif			/* } */

#include <time.h>

__inline unsigned int luafix_makeseed(void *L) {
	const char* seed = getenv("LUA_SEED");
	if (seed) {
		return atoi(seed);
	}
	return (unsigned int)time(NULL);
}

#define luai_makeseed luafix_makeseed

#endif

