#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)			  	  		
#include <base/filesystem.h>
#include <base/hook/fp_call.h>	
#include <base/util/unicode.h>
#include <Windows.h>

namespace NLuaAPI { namespace NSTORM {

	const int gOrdinalStormSFileOpenArchive = 266;
	const int gOrdinalStormSFileCloseArchive = 252;
	const int gOrdinalStormSFileOpenFileEx = 268;
	const int gOrdinalStormSFileCloseFile = 253;
	const int gOrdinalStormSFileGetFileSize = 265;
	const int gOrdinalStormSFileSetFilePointer = 271;
	const int gOrdinalStormSFileReadFile = 269;
	const int gOrdinalStormSFileLoadFile = 279;
	const int gOrdinalStormSFileUnloadFile = 280;
	const int gOrdinalStormSFileExists = 288;
	const int gOrdinalStormSStrHash = 590;

	void *pgStormSFileOpenArchive;
	void *pgStormSFileCloseArchive;
	void *pgStormSFileOpenFileEx;
	void *pgStormSFileCloseFile;
	void *pgStormSFileGetFileSize;
	void *pgStormSFileSetFilePointer;
	void *pgStormSFileReadFile;
	void *pgStormSFileLoadFile;
	void *pgStormSFileUnloadFile;
	void *pgStormSFileExists;
	void *pgStormSStrHash;

#define INITIALIZE_STORM_FUNCTION_POINTER(name) pgStorm##name = GetProcAddress(stormDll, reinterpret_cast<LPCSTR>(gOrdinalStorm##name##));

	static void InitStormDllFunctions()
	{
		HMODULE stormDll = GetModuleHandleW(L"storm.dll");
		if (stormDll)
		{
			INITIALIZE_STORM_FUNCTION_POINTER(SFileOpenArchive)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileCloseArchive)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileOpenFileEx)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileCloseFile)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileGetFileSize)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileSetFilePointer)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileReadFile)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileLoadFile)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileUnloadFile)
			INITIALIZE_STORM_FUNCTION_POINTER(SFileExists)
			INITIALIZE_STORM_FUNCTION_POINTER(SStrHash)
		}
	}

#undef INITIALIZE_STORM_FUNCTION_POINTER



	static void *LuaMpqNativeOpenArchive(const fs::path &mpqPath, uint32_t priority)
	{
		if (!pgStormSFileOpenArchive)
			return 0;

		HANDLE mpqHandle;

		if (!base::std_call<BOOL>(pgStormSFileOpenArchive, mpqPath.string().c_str(), priority, 0, &mpqHandle))
		{
			return 0;
		}

		 return mpqHandle;
	}

	static bool LuaMpqNativeCloseArchive(void *mpqHandle)
	{
		if (!pgStormSFileCloseArchive)
			return false;

		return !!base::std_call<BOOL>(pgStormSFileCloseArchive, mpqHandle);
	}

	static bool LuaMpqNativeHasFile(const std::string &pathInMpq)
	{
		if (!pgStormSFileExists)
			return false;

		return !!base::std_call<BOOL>(pgStormSFileExists, pathInMpq.c_str());
	}

	static bool LuaMpqNativeExtractFile(const fs::path &filePath, const std::wstring &wpathinmpq)
	{
		if (!pgStormSFileLoadFile || !pgStormSFileUnloadFile)
			return false;

		std::string pathinmpq = base::w2a(wpathinmpq, base::conv_method::replace | '?');
		uint8_t* buf;
		uint32_t buf_size;
		if (!base::std_call<BOOL>(pgStormSFileLoadFile, pathinmpq.c_str(), &buf, &buf_size, 0, NULL))
		{
			return false;
		}

		FILE *out = fopen(filePath.string().c_str(), "wb");
		if (!out)
		{
			base::std_call<BOOL>(pgStormSFileUnloadFile, buf);
			return false;
		}

		fwrite(buf, 1, buf_size, out);
		fclose(out);
		base::std_call<BOOL>(pgStormSFileUnloadFile, buf);
		return true;
	}

	static void LuaMpqNativeLoadFile(lua_State *pState, const std::wstring &wpathinmpq)
	{
		if (!pgStormSFileLoadFile || !pgStormSFileUnloadFile)
		{
			lua_pushboolean(pState, false);
			return ;
		}

		std::string pathinmpq = base::w2a(wpathinmpq, base::conv_method::replace | '?');
		char* fileContentBuffer;
		uint32_t size;
		if (!base::std_call<BOOL>(pgStormSFileLoadFile, pathinmpq.c_str(), &fileContentBuffer, &size, 0, NULL))
		{
			lua_pushboolean(pState, false);
			return;
		}

		lua_pushboolean(pState, true);
		lua_pushlstring(pState, fileContentBuffer, size);
		base::std_call<BOOL>(pgStormSFileUnloadFile, fileContentBuffer);
	}

	uint32_t LuaStormStringHash(const char* str)
	{
		return base::std_call<uint32_t>(pgStormSStrHash, str);
	}
}}

int luaopen_ar_storm(lua_State *pState)
{
	NLuaAPI::NSTORM::InitStormDllFunctions();

	using namespace luabind;
	module(pState, "ar")
	[
		namespace_("storm")
		[
			def("open_archive",         &NLuaAPI::NSTORM::LuaMpqNativeOpenArchive),
			def("close_archive",        &NLuaAPI::NSTORM::LuaMpqNativeCloseArchive),
			def("extract_file",         &NLuaAPI::NSTORM::LuaMpqNativeExtractFile),
			def("has_file",             &NLuaAPI::NSTORM::LuaMpqNativeHasFile),
			def("load_file",            &NLuaAPI::NSTORM::LuaMpqNativeLoadFile),
			def("string_hash",          &NLuaAPI::NSTORM::LuaStormStringHash)
		]
	];

	return 0;
}
