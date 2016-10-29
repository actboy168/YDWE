#include <lua.hpp>
#include <stdint.h>
#include <time.h>

#include <base/filesystem.h>
#include <base/util/unicode.h>

#ifdef WIN32
#	pragma warning(disable:4996)
#	include <windows.h>
#	include <direct.h>
#else
#	include <unistd.h>
#	include <sys/types.h> 
#	include <sys/stat.h>
#	include <sys/time.h>
#endif
			
namespace base { namespace warcraft3 { namespace lua_engine { namespace log {

enum loglevel {	 
	trace = 0,
	debug,
	info,
	warning,
	error,
	fatal
};

#ifdef WIN32
/*
* Number of micro-seconds between the beginning of the Windows epoch
* (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
*
* This assumes all Win32 compilers have 64-bit support.
*/
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
#	define DELTA_EPOCH_IN_USEC 11644473600000000Ui64
#else
#	define DELTA_EPOCH_IN_USEC 11644473600000000ULL
#endif

static uint64_t log_filetime_to_unix_epoch(const FILETIME *ft)
{
	uint64_t res = (uint64_t)ft->dwHighDateTime << 32;

	res |= ft->dwLowDateTime;
	res /= 10; /* from 100 nano-sec periods to usec */
	res -= DELTA_EPOCH_IN_USEC; /* from Win epoch to Unix epoch */
	return (res);
}

static int gettimeofday(struct timeval *tv, void *)
{
	FILETIME ft;
	uint64_t tim;

	if (!tv) {
		//errno = EINVAL;
		return (-1);
	}
	GetSystemTimeAsFileTime(&ft);
	tim = log_filetime_to_unix_epoch(&ft);
	tv->tv_sec = (long)(tim / 1000000L);
	tv->tv_usec = (long)(tim % 1000000L);
	return (0);
}
#endif

static struct tm log_datenow(long* usec = 0)
{
	struct tm tm_now;
	struct timeval tv;
	gettimeofday(&tv, 0);
	time_t now = tv.tv_sec;
#ifdef WIN32
	localtime_s(&tm_now, &now);
#else
	localtime_r(&now, &tm_now);
#endif
	if (usec) *usec = tv.tv_usec;
	return tm_now;
}

static const char* log_filename(const char* path)
{
	static char filename[1024] = { 0 };
	static const char fmt[] = "%s\\log-%04d-%02d-%02d-%02d-%02d-%02d.log";
	struct tm t = log_datenow();
	sprintf(filename, fmt, path, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
	return filename;
}

static void	log_print(lua_State* L, const char* msg)
{
	lua_pushstring(L, "__path");
	lua_rawget(L, lua_upvalueindex(1));
	size_t pathlen = 0;
	const wchar_t* path = (const wchar_t*)lua_tolstring(L, -1, &pathlen);
	if (path) {
		FILE* f = _wfopen(path, L"a");
		if (f)
		{
			fputs(msg, f);
			fclose(f);
		}
	}
	lua_pop(L, 1);
}

static const char* log_lvstr[] = {
	"trace",
	"debug",
	"info",
	"warn",
	"error",
	"fatal",
};

static const char *	log_timestr()
{
	static char str[64] = { 0 };
	long usec = 0;
	struct tm t = log_datenow(&usec);
	sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d.%03d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, usec / 1000);
	return str;
}

static int l_log(lua_State* L, int level)
{
	lua_pushstring(L, "__level");
	lua_rawget(L, lua_upvalueindex(1));
	if (level < lua_tointeger(L, -1))
	{
		return 0;
	}
	lua_pop(L, 1);

	luaL_Buffer b;
	luaL_buffinit(L, &b);
	luaL_addstring(&b, log_timestr());
	luaL_addstring(&b, " [");
	luaL_addstring(&b, log_lvstr[level]);
	luaL_addstring(&b, "]: ");

	int n = lua_gettop(L);
	lua_getglobal(L, "tostring");
	for (int i = 1; i <= n; ++i)
	{
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		size_t l;
		const char * s = lua_tolstring(L, -1, &l);
		if (s == NULL) {
			s = "nil";
		}
		if (i>1) luaL_addchar(&b, '\t');
		luaL_addlstring(&b, s, l);
		lua_pop(L, 1);
	}

	luaL_addchar(&b, '\n');
	luaL_pushresult(&b);
	log_print(L, lua_tostring(L, -1));
	return 1;
}

static int l_logdebug(lua_State* L) { return l_log(L, debug); }
static int l_loginfo(lua_State* L) { return l_log(L, info); }
static int l_logwarning(lua_State* L) { return l_log(L, warning); }
static int l_logerror(lua_State* L) { return l_log(L, error); }
static int l_logfatal(lua_State* L) { return l_log(L, fatal); }
static int l_logtrace(lua_State* L) { return l_log(L, trace); }

static int l__index(lua_State* L)
{
	try {
		const char* key = lua_tostring(L, 2);
		if (strcmp(key, "path") == 0)
		{
			lua_pushstring(L, "__path");
			lua_rawget(L, lua_upvalueindex(1));
			size_t len = 0;
			const char* str = lua_tolstring(L, -1, &len);
			std::string utf8_str = base::w2u(std::wstring_view((const wchar_t*)str, len / sizeof(wchar_t)));
			lua_pushlstring(L, utf8_str.c_str(), utf8_str.size());
			return 1;
		}
		else if (strcmp(key, "level") == 0)
		{
			lua_pushstring(L, "__level");
			lua_rawget(L, lua_upvalueindex(1));
			int level = lua_tointeger(L, 1);
			if (level >= 0 && level < sizeof(log_lvstr))
			{
				lua_pushstring(L, log_lvstr[level]);
				return 1;
			}
			return 0;
		}
	}
	catch (...) {}
	return 0;
}

static int l__newindex(lua_State* L)
{
	try {
		const char* key = lua_tostring(L, 2);
		if (strcmp(key, "path") == 0)
		{
			size_t pathlen = 0;
			const char* path = lua_tolstring(L, 3, &pathlen);
			std::wstring utf16_str = base::u2w(std::string_view(path, pathlen));
			fs::create_directories(fs::path(utf16_str).remove_filename());
			lua_pushstring(L, "__path");
			lua_pushlstring(L, (const char*)utf16_str.c_str(), (utf16_str.size() + 1) * sizeof(wchar_t));
			lua_rawset(L, lua_upvalueindex(1));
			return 0;
		}
		else if (strcmp(key, "level") == 0)
		{
			const char* level = lua_tostring(L, 3);
			for (int i = 0; i < sizeof(log_lvstr); ++i)
			{
				if (strcmp(log_lvstr[i], level) == 0)
				{
					lua_pushstring(L, "__level");
					lua_pushinteger(L, i);
					lua_rawset(L, lua_upvalueindex(1));
					return 0;
				}
			}
			return 0;
		}
	}
	catch (...) {}
	return 0;
}

int open(lua_State* L)
{
	static luaL_Reg func[] = {
		{ "debug", l_logdebug },
		{ "info", l_loginfo },
		{ "warn", l_logwarning },
		{ "error", l_logerror },
		{ "fatal", l_logfatal },
		{ "trace", l_logtrace },
		{ NULL, NULL },
	};
	lua_newtable(L);
	lua_pushvalue(L, -1);
	luaL_setfuncs(L, func, 1);

	static luaL_Reg mt[] = {
		{ "__index", l__index },
		{ "__newindex", l__newindex },
		{ NULL, NULL },
	};
	lua_newtable(L);
	lua_pushvalue(L, -2);
	luaL_setfuncs(L, mt, 1);
	lua_setmetatable(L, -2);

	const char* path = log_filename("log");
	lua_pushstring(L, path);
	lua_setfield(L, -2, "path");

	return 1;
}

}}}}
