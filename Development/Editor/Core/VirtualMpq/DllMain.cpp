#include <windows.h>
#define __STORMLIB_SELF__
#include <StormLib.h>
#include <aero/function/fp_call.hpp>
#include <base/hook/iat.h>
#include <base/hook/inline.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <cstdint>

namespace wrap
{
	namespace real
	{
		uintptr_t SFileCloseFile = 0;
		uintptr_t SFileEnableDirectAccess = 0;
		uintptr_t SFileGetFileArchive = 0;
		uintptr_t SFileGetFileSize = 0;
		uintptr_t SFileOpenArchive = 0;
		uintptr_t SFileOpenFile = 0;
		uintptr_t SFileOpenFileEx = 0;
		uintptr_t SFileReadFile = 0;
		uintptr_t SFileSetFilePointer = 0;
		uintptr_t SFileSetLocale = 0;
		uintptr_t SFileGetArchiveName = 0;
		uintptr_t SFileLoadFile = 0;
		uintptr_t SFileUnloadFile = 0;
		uintptr_t SFileLoadFileEx = 0;
		uintptr_t SFileFileExists = 0;
		uintptr_t SFileFileExistsEx = 0;
		uintptr_t SFileReadFileEx = 0;
		uintptr_t SFileOpenFileAsArchive = 0;
		uintptr_t SFileOpenPathAsArchive = 0;
	}

	namespace fake
	{
		bool __stdcall SFileEnableDirectAccess(uint32_t flags)
		{
			return aero::std_call<bool>(real::SFileEnableDirectAccess, flags);
		}

		bool __stdcall SFileGetFileArchive(HANDLE file_handle, HANDLE* mpq_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileGetFileArchive, file_handle, mpq_handle_ptr);
		}

		LCID __stdcall SFileSetLocale(LCID locale)
		{
			return aero::std_call<LCID>(real::SFileSetLocale, locale);
		}

		bool __stdcall SFileOpenArchive(const char* mpqname, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileOpenArchive, mpqname, priority, flags, mpq_handle_ptr);
		}

		bool __stdcall SFileOpenFile(const char* filename, HANDLE* file_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileOpenFile, filename, file_handle_ptr);
		}

		bool __stdcall SFileOpenFileEx(HANDLE mpq_handle, const char *filename, uint32_t search_scope, HANDLE* file_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileOpenFileEx, mpq_handle, filename, search_scope, file_handle_ptr);
		}

		bool __stdcall SFileCloseFile(HANDLE file_handle)
		{
			return aero::std_call<bool>(real::SFileCloseFile, file_handle);
		}

		uint32_t __stdcall SFileGetFileSize(HANDLE file_handle, uint32_t* file_size_high_ptr)
		{
			return aero::std_call<uint32_t>(real::SFileGetFileArchive, file_handle, file_size_high_ptr);
		}

		bool __stdcall SFileReadFile(HANDLE file_handle, void* buffer, uint32_t size, uint32_t* size_ptr, uint32_t unk)
		{
			return aero::std_call<bool>(real::SFileReadFile, file_handle, buffer, size, size_ptr, unk);
		}

		uint32_t __stdcall SFileSetFilePointer(HANDLE file_handle, uint32_t pos, uint32_t* pos_high, uint32_t method)
		{
			return aero::std_call<uint32_t>(real::SFileSetFilePointer, file_handle, pos, pos_high, method);
		}

		bool __stdcall SFileGetArchiveName(HANDLE mpq_handle, char* name, uint32_t size)
		{
			return aero::std_call<bool>(real::SFileGetArchiveName, mpq_handle, name, size);
		}

		bool __stdcall SFileLoadFile(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			return aero::std_call<bool>(real::SFileLoadFile, filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
		}

		bool __stdcall SFileUnloadFile(void* buffer)
		{
			return aero::std_call<bool>(real::SFileUnloadFile, buffer);
		}

		bool __stdcall SFileLoadFileEx(HANDLE mpq_handle, const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, uint32_t search_scope, OVERLAPPED* overlapped_ptr)
		{
			return aero::std_call<bool>(real::SFileLoadFileEx, mpq_handle, filename, buffer_ptr, size_ptr, reserve_size, search_scope, overlapped_ptr);
		}

		bool __stdcall SFileFileExists(const char* filename)
		{
			return aero::std_call<bool>(real::SFileFileExists, filename);
		}

		bool __stdcall SFileFileExistsEx(HANDLE mpq_handle, const char* filename, uint32_t flags)
		{
			return aero::std_call<bool>(real::SFileFileExistsEx, mpq_handle, filename, flags);
		}

		bool __stdcall SFileReadFileEx(HANDLE file_handle, void* buffer, uint32_t size, uint32_t* size_ptr, uint32_t unk, OVERLAPPED* overlapped_ptr, uint32_t unk2)
		{
			return aero::std_call<bool>(real::SFileReadFileEx, file_handle, buffer, size, size_ptr, unk, overlapped_ptr, unk2);
		}

		bool __stdcall SFileOpenFileAsArchive(HANDLE handle, const char* filename, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileOpenFileAsArchive, handle, filename, priority, flags, mpq_handle_ptr);
		}

		bool __stdcall SFileOpenPathAsArchive(HANDLE handle, const char* pathname, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		{
			return aero::std_call<bool>(real::SFileOpenPathAsArchive, handle, pathname, priority, flags, mpq_handle_ptr);
		}
	}
}

class stormlib_builder
{
public:
	stormlib_builder(const boost::filesystem::path& target, const boost::filesystem::path& source)
		: handle_(NULL)
		, source_(source)
	{
		SFILE_CREATE_MPQ CreateInfo = {0};
		CreateInfo.cbSize         = sizeof(SFILE_CREATE_MPQ);
		CreateInfo.dwMpqVersion   = MPQ_FORMAT_VERSION_1;
		CreateInfo.dwStreamFlags  = STREAM_PROVIDER_LINEAR | BASE_PROVIDER_FILE;
		CreateInfo.dwAttrFlags    = 0;
		CreateInfo.dwSectorSize   = 0x10000;
		CreateInfo.dwRawChunkSize = 0;
		CreateInfo.dwMaxFileCount = get_file_count(source);

		if (!SFileCreateArchive2(target.string().c_str(), &CreateInfo, &handle_))
		{
			handle_ = NULL;
		}
	}

	~stormlib_builder()
	{
		if (handle_)
		{
			SFileCloseArchive(handle_);
			handle_ = NULL;
		}
	}

	bool add(const boost::filesystem::path& target, const boost::filesystem::path& source)
	{
		if (!handle_)
			return false;

		return SFileAddFileEx(
			  handle_
			, source.string().c_str()
			, target.string().c_str()
			, MPQ_FILE_COMPRESS | MPQ_FILE_REPLACEEXISTING
			, MPQ_COMPRESSION_ZLIB
			, MPQ_COMPRESSION_ZLIB
			);
	}

	static uint32_t get_file_count(const boost::filesystem::path& p)
	{
		uint32_t sum = 0;
		try {
			boost::filesystem::recursive_directory_iterator end_itr;
			for (boost::filesystem::recursive_directory_iterator itr(p); itr != end_itr; ++itr)
			{
				if (!boost::filesystem::is_directory(*itr))
				{
					sum++;
				}
			}
		}
		catch (...) {
		}

		uint32_t n = 1;
		while (sum) { n<<=1; sum >>=1; }
		return n;
	}

	static boost::filesystem::path path_sub(const boost::filesystem::path& lft, const boost::filesystem::path& rht)
	{
		boost::filesystem::path::iterator lft_itr =  lft.begin();
		boost::filesystem::path::iterator lft_end =  lft.end();
		boost::filesystem::path::iterator rht_itr =  rht.begin();
		boost::filesystem::path::iterator rht_end =  rht.end();

		while ((lft_itr != lft_end) && (rht_itr != rht_end))
		{
			if (*lft_itr != *rht_itr)
			{
				break;
			}

			++lft_itr;
			++rht_itr;
		}

		boost::filesystem::path result;
		for (; lft_itr != lft_end; ++lft_itr)
		{
			result /= *lft_itr;
		}
		return std::move(result);
	}

private:
	HANDLE handle_;
	boost::filesystem::path source_;
};

struct we_import_file
{
	uint8_t type;
	char    file[520];
};

struct we_import_info
{
	char            pre_import_path[260];
	char            default_path[260];
	uint32_t        count1;
	uint32_t        count2;
	we_import_file* array;
};

uintptr_t real_we_create_map = 0x0055D720;
bool __fastcall fake_we_create_map(uintptr_t this_, uintptr_t /*edx_*/, const char* name, bool is_tft)
{
	//return aero::this_call<bool>(real_we_create_map, this_, name, is_tft);
	try {
		boost::filesystem::path target(name);
		boost::filesystem::path source = target.parent_path();
		boost::filesystem::path source_mpq(source.wstring().substr(0, source.wstring().size()-4));

		if (is_tft)
		{
			target.replace_extension(L".w3x");
		}
		else
		{
			target.replace_extension(L".w3m");
		}

		aero::this_call<void>(0x005261D0, *((uint32_t*)this_+3734));

		stormlib_builder sb(target, source);
	
		boost::filesystem::recursive_directory_iterator end_itr;
		for (boost::filesystem::recursive_directory_iterator itr(source); itr != end_itr; ++itr)
		{
			try {
				if (!boost::filesystem::is_directory(*itr) && (itr->path() != target))
				{
					sb.add(stormlib_builder::path_sub(itr->path(), source), itr->path());
				}
			}
			catch(...) {
			}
		}

		aero::std_call<uintptr_t>(0x00405240, (source/ L"war3map.j").string().c_str());
		return true;
	}
	catch (...) {
	}
	
	return false;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);

		base::hook::inline_install(&real_we_create_map, (uintptr_t)fake_we_create_map);
		/*
#define IAT_HOOK(ord, name) wrap::real:: ## name = base::hook::iat(::GetModuleHandleW(NULL), "Storm.dll", (const char*)(ord), (uintptr_t)wrap::fake:: ## name ##)
		IAT_HOOK(253, SFileCloseFile);
		IAT_HOOK(263, SFileEnableDirectAccess);
		IAT_HOOK(264, SFileGetFileArchive);
		IAT_HOOK(265, SFileGetFileSize);
		IAT_HOOK(266, SFileOpenArchive);
		IAT_HOOK(267, SFileOpenFile);
		IAT_HOOK(268, SFileOpenFileEx);
		IAT_HOOK(269, SFileReadFile);
		IAT_HOOK(271, SFileSetFilePointer);
		IAT_HOOK(272, SFileSetLocale);
		IAT_HOOK(275, SFileGetArchiveName);
		IAT_HOOK(279, SFileLoadFile);
		IAT_HOOK(280, SFileUnloadFile);
		IAT_HOOK(281, SFileLoadFileEx);
		IAT_HOOK(288, SFileFileExists);
		IAT_HOOK(289, SFileFileExistsEx);
		IAT_HOOK(291, SFileReadFileEx);
		IAT_HOOK(293, SFileOpenFileAsArchive);
		IAT_HOOK(300, SFileOpenPathAsArchive);
#undef 	IAT_HOOK
		*/
	}

	return TRUE;
}
