#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <ydwe/lua/luabind.h>
#pragma warning(pop)
#include <boost/filesystem.hpp>
#include <boost/scope_exit.hpp>
#include <aero/function/fp_call.hpp>
#include <Windows.h>

namespace fs = boost::filesystem;

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
		}
	}

#undef INITIALIZE_STORM_FUNCTION_POINTER



	static void *LuaMpqNativeOpenArchive(const fs::path &mpqPath, uint32_t priority)
	{
		if (!pgStormSFileOpenArchive)
			return 0;

		HANDLE mpqHandle;

		if (!aero::std_call<BOOL>(pgStormSFileOpenArchive, mpqPath.string().c_str(), priority, 0, &mpqHandle))
		{
			return 0;
		}

		 return mpqHandle;
	}

	static bool LuaMpqNativeCloseArchive(void *mpqHandle)
	{
		if (!pgStormSFileCloseArchive)
			return false;

		return !!aero::std_call<BOOL>(pgStormSFileCloseArchive, mpqHandle);
	}

	static bool LuaMpqNativeHasFile(const std::string &pathInMpq)
	{
		if (!pgStormSFileExists)
			return false;

		return !!aero::std_call<BOOL>(pgStormSFileExists, pathInMpq.c_str());
	}

	static bool LuaMpqNativeExtractFile(const fs::path &filePath, const std::string &pathInMpq)
	{
		if (!pgStormSFileLoadFile || !pgStormSFileUnloadFile)
			return false;

		boost::uint8_t *fileContentBuffer;
		boost::uint32_t size;
		BOOL ret = FALSE;

		BOOST_SCOPE_EXIT( (&ret) (&fileContentBuffer) )
		{
			if (ret)
			{
				aero::std_call<BOOL>(pgStormSFileUnloadFile, fileContentBuffer);
			}
		} BOOST_SCOPE_EXIT_END;

		ret = aero::std_call<BOOL>(pgStormSFileLoadFile, pathInMpq.c_str(), &fileContentBuffer, &size, 0, NULL);

		if (ret)
		{
			FILE *out = NULL;

			BOOST_SCOPE_EXIT( (&out) )
			{
				if (out)
				{
					fclose(out);
				}
			} BOOST_SCOPE_EXIT_END;

			out = fopen(filePath.string().c_str(), "wb");
			if (out)
			{
				fwrite(fileContentBuffer, 1, size, out);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	static void LuaMpqNativeLoadFile(lua_State *pState, const std::string &pathInMpq)
	{
		if (!pgStormSFileLoadFile || !pgStormSFileUnloadFile)
		{
			luabind::object(pState, false).push(pState);
			return ;
		}

		char* fileContentBuffer;
		uint32_t size;
		BOOL ret = FALSE;

		ret = aero::std_call<BOOL>(pgStormSFileLoadFile, pathInMpq.c_str(), &fileContentBuffer, &size, 0, NULL);

		BOOST_SCOPE_EXIT( (&ret) (&fileContentBuffer) )
		{
			if (ret)
			{
				aero::std_call<BOOL>(pgStormSFileUnloadFile, fileContentBuffer);
			}
		} BOOST_SCOPE_EXIT_END;

		if (ret)
		{
			luabind::object(pState, true).push(pState);
			luabind::object(pState, std::string(fileContentBuffer, size)).push(pState);
			return ;
		}
		else
		{
			luabind::object(pState, false).push(pState);
		}
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
			def("load_file",            &NLuaAPI::NSTORM::LuaMpqNativeLoadFile)
		]
	];

	return 0;
}
