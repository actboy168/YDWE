#include <lua.hpp> 
#include <fstream>
#include <filesystem>
#include "crc32.h"
#include "sha1.h"
#include "rolc.h"

#define __STORMLIB_SELF__
#include <StormLib.h>

namespace fs = std::experimental::filesystem;

std::string w2u(std::wstring& wstr)
{
	return std::move(std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr));
}

int file_error(lua_State* L, fs::path const& path)
{
	return luaL_error(L, "%s: %s", w2u(path.wstring()).c_str(), strerror(ENOENT));
}

size_t sfsize(HANDLE file)
{
	DWORD hi = 0;
	DWORD lo = SFileGetFileSize(file, &hi);
	return hi ? 0 : (size_t)lo;
}

class storm
{
public:
	storm(lua_State* L, fs::path const& path)
	{
		if (!SFileOpenArchive(path.c_str(), 0, STREAM_FLAG_READ_ONLY | MPQ_OPEN_FORCE_MPQ_V1, &handle_))
		{
			file_error(L, path);
		}
	}

	~storm() { SFileCloseArchive(handle_); }
	operator HANDLE() { return handle_; }

private:
	HANDLE handle_;
};

bool readall(fs::path const& path, std::string& buf)
{
	std::ifstream f(path.c_str(), std::ios::binary);
	if (!f)
	{
		return false;
	}
	buf = std::string((std::istreambuf_iterator<char>(f)), (std::istreambuf_iterator<char>()));
	f.close();
	return true;
}

bool readall(HANDLE handle, const char* filename, std::string& buf)
{
	HANDLE file = 0;
	if (!SFileOpenFileEx(handle, filename, 0, &file))
	{
		return false;
	}
	size_t len = sfsize(file);
	if (!len)
	{
		return false;
	}
	buf.resize(len);
	DWORD read = 0;
	if (!SFileReadFile(file, (void*)buf.data(), buf.size(), &read, nullptr))
	{
		SFileCloseFile(file);
		return false;
	}
	SFileCloseFile(file);
	return true;
}

int maphash(lua_State* L)
{
	fs::path& path = *(fs::path*)luaL_checkudata(L, 1, "filesystem");
	fs::path& jass = *(fs::path*)luaL_checkudata(L, 2, "filesystem");
	{
		std::string buf;
		if (!readall(path, buf))
		{
			return file_error(L, path);
		}
		lua_pushinteger(L, buf.size());
		lua_pushinteger(L, hash::crc32((const unsigned char*)buf.data(), buf.size()));
	}

	{
		storm map(L, path);
		hash::rolc rolc;
		hash::sha1 sha1;
		std::string buf;
		if (readall(map, "common.j", buf) || readall(map, "scripts\\common.j", buf) || readall(jass / L"common.j", buf))
		{
			rolc.update((const unsigned char*)buf.data(), buf.size());
			sha1.update((const unsigned char*)buf.data(), buf.size());
		}
		if (readall(map, "blizzard.j", buf) || readall(map, "scripts\\blizzard.j", buf) || readall(jass / L"blizzard.j", buf))
		{
			rolc.update((const unsigned char*)buf.data(), buf.size());
			sha1.update((const unsigned char*)buf.data(), buf.size());
		}
		rolc.update(0x03F1379E);
		sha1.update((const unsigned char*)"\x9E\x37\xF1\x03", 4);

		if (readall(map, "war3map.j", buf) || readall(map, "scripts\\war3map.j", buf))
		{
			rolc.update((const unsigned char*)buf.data(), buf.size());
			sha1.update((const unsigned char*)buf.data(), buf.size());
		}

		for (const char* filename : {
			"war3map.w3e",
			"war3map.wpm",
			"war3map.doo",
			"war3map.w3u",
			"war3map.w3b",
			"war3map.w3d",
			"war3map.w3a",
			"war3map.w3q", 
		})
		{
			if (readall(map, filename, buf))
			{
				rolc.update((const unsigned char*)buf.data(), buf.size());
				sha1.update((const unsigned char*)buf.data(), buf.size());
			}
		}

		uint32_t sha1_result[5];
		sha1.final((unsigned char*)sha1_result);

		lua_pushinteger(L, rolc.final());
		lua_pushinteger(L, sha1_result[0]);
		lua_pushinteger(L, sha1_result[1]);
		lua_pushinteger(L, sha1_result[2]);
		lua_pushinteger(L, sha1_result[3]);
		lua_pushinteger(L, sha1_result[4]);
	}
	return 8;
}

extern "C" __declspec(dllexport)
int luaopen_maphash(lua_State* L)
{
	luaL_checkversion(L);
	lua_pushcfunction(L, maphash);
	return 1;
}
