#include <base/warcraft3/virtual_mpq.h>
#include <base/hook/fp_call.h>
#include <base/file/stream.h>
#include <base/hook/iat.h>
#include <base/hook/inline.h>
#include <base/util/foreach.h>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <cstdint>
#include <array>

namespace base { namespace warcraft3 { namespace virtual_mpq {

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

		uintptr_t SMemAlloc = 0;
	}

	namespace filesystem
	{
		std::array<std::list<boost::filesystem::path>, 16> mpq_path;

		void* SMemAlloc(size_t amount)
		{
			return base::std_call<void*>(real::SMemAlloc, amount, ".\\SFile.cpp", 4072, 0);
		}

		boost::optional<boost::filesystem::path> find_file(const char* filename)
		{
			for (uint32_t priority = 15; priority != 0; --priority)
			{
				foreach(auto it, mpq_path[priority])
				{
					if (boost::filesystem::exists(it / filename))
					{
						return boost::optional<boost::filesystem::path>(it / filename);
					}
				}
			}

			return boost::optional<boost::filesystem::path>();
		}

		void open_path(const boost::filesystem::path& p, uint32_t priority)
		{
			if (priority > 15) priority = 15;
			mpq_path[priority].push_front(p);
		}

		bool SFileLoadFile(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			try {
				if (!buffer_ptr)
				{
					return false;
				}

				boost::optional<boost::filesystem::path> file_path = find_file(filename);
				if (!file_path)
				{
					return false;
				}

				std::string buf = base::file::read_stream(file_path.get()).read<std::string>();
				void* result = SMemAlloc(buf.size() + reserve_size);
				if (!result)
				{
					return false;
				}

				memcpy(result, buf.data(), buf.size());
				*buffer_ptr = result;
				if (reserve_size) memset((unsigned char*)result + buf.size(), 0, reserve_size);
				if (size_ptr) *size_ptr = buf.size();
				if (overlapped_ptr && overlapped_ptr->hEvent) ::SetEvent(overlapped_ptr->hEvent);

				return true;

			}
			catch (...) {}

			return false;
		}

		bool SFileFileExists(const char* filename)
		{
			try {
				return  !!find_file(filename);
			}
			catch (...) {}

			return false;
		}
	}

	namespace fake
	{
		//bool __stdcall SFileEnableDirectAccess(uint32_t flags)
		//{
		//	return base::std_call<bool>(real::SFileEnableDirectAccess, flags);
		//}
		//
		//bool __stdcall SFileGetFileArchive(HANDLE file_handle, HANDLE* mpq_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileGetFileArchive, file_handle, mpq_handle_ptr);
		//}
		//
		//LCID __stdcall SFileSetLocale(LCID locale)
		//{
		//	return base::std_call<LCID>(real::SFileSetLocale, locale);
		//}
		//
		//bool __stdcall SFileOpenArchive(const char* mpqname, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileOpenArchive, mpqname, priority, flags, mpq_handle_ptr);;
		//}
		//
		//bool __stdcall SFileOpenFile(const char* filename, HANDLE* file_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileOpenFile, filename, file_handle_ptr);;
		//}
		//
		//bool __stdcall SFileOpenFileEx(HANDLE mpq_handle, const char *filename, uint32_t search_scope, HANDLE* file_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileOpenFileEx, mpq_handle, filename, search_scope, file_handle_ptr);;
		//}
		//
		//bool __stdcall SFileCloseFile(HANDLE file_handle)
		//{
		//	return base::std_call<bool>(real::SFileCloseFile, file_handle);
		//}
		//
		//uint32_t __stdcall SFileGetFileSize(HANDLE file_handle, uint32_t* file_size_high_ptr)
		//{
		//	return base::std_call<uint32_t>(real::SFileGetFileSize, file_handle, file_size_high_ptr);
		//}
		//
		//bool __stdcall SFileReadFile(HANDLE file_handle, void* buffer, uint32_t size, uint32_t* size_ptr, uint32_t unk)
		//{
		//	return base::std_call<bool>(real::SFileReadFile, file_handle, buffer, size, size_ptr, unk);
		//}
		//
		//uint32_t __stdcall SFileSetFilePointer(HANDLE file_handle, uint32_t pos, uint32_t* pos_high, uint32_t method)
		//{
		//	return base::std_call<uint32_t>(real::SFileSetFilePointer, file_handle, pos, pos_high, method);
		//}
		//
		//bool __stdcall SFileGetArchiveName(HANDLE mpq_handle, char* name, uint32_t size)
		//{
		//	return base::std_call<bool>(real::SFileGetArchiveName, mpq_handle, name, size);
		//}

		bool __stdcall SFileLoadFile(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			if (filesystem::SFileLoadFile(filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr))
			{
				return true;
			}

			return base::std_call<bool>(real::SFileLoadFile, filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
		}

		bool __stdcall SFileUnloadFile(void* buffer)
		{
			return base::std_call<bool>(real::SFileUnloadFile, buffer);
		}

		bool __stdcall SFileLoadFileEx(HANDLE mpq_handle, const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, uint32_t search_scope, OVERLAPPED* overlapped_ptr)
		{
			if (mpq_handle == 0 && filesystem::SFileLoadFile(filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr))
			{
				return true;
			}

			return base::std_call<bool>(real::SFileLoadFileEx, mpq_handle, filename, buffer_ptr, size_ptr, reserve_size, search_scope, overlapped_ptr);
		}

		bool __stdcall SFileFileExists(const char* filename)
		{
			if (filesystem::SFileFileExists(filename))
			{
				return true;
			}

			return base::std_call<bool>(real::SFileFileExists, filename);
		}

		bool __stdcall SFileFileExistsEx(HANDLE mpq_handle, const char* filename, uint32_t flags)
		{
			if (mpq_handle == 0 && filesystem::SFileFileExists(filename))
			{
				return true;
			}

			return base::std_call<bool>(real::SFileFileExistsEx, mpq_handle, filename, flags);
		}

		//bool __stdcall SFileReadFileEx(HANDLE file_handle, void* buffer, uint32_t size, uint32_t* size_ptr, uint32_t unk, OVERLAPPED* overlapped_ptr, uint32_t unk2)
		//{
		//	return base::std_call<bool>(real::SFileReadFileEx, file_handle, buffer, size, size_ptr, unk, overlapped_ptr, unk2);
		//}
		//
		//bool __stdcall SFileOpenFileAsArchive(HANDLE handle, const char* filename, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileOpenFileAsArchive, handle, filename, priority, flags, mpq_handle_ptr);
		//}
		//
		//bool __stdcall SFileOpenPathAsArchive(HANDLE handle, const char* pathname, uint32_t priority, uint32_t flags, HANDLE* mpq_handle_ptr)
		//{
		//	return base::std_call<bool>(real::SFileOpenPathAsArchive, handle, pathname, priority, flags, mpq_handle_ptr);
		//}
	}

	void open_path(const boost::filesystem::path& p, uint32_t priority)
	{
		filesystem::open_path(p, priority);
	}

	void initialize(HMODULE module_handle)
	{
		real::SMemAlloc = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Storm.dll"), (const char*)401);

#define IAT_HOOK(ord, name) real:: ## name = base::hook::iat(module_handle, "Storm.dll", (const char*)(ord), (uintptr_t)fake:: ## name ##)
		//IAT_HOOK(253, SFileCloseFile);
		//IAT_HOOK(263, SFileEnableDirectAccess);
		//IAT_HOOK(264, SFileGetFileArchive);
		//IAT_HOOK(265, SFileGetFileSize);
		//IAT_HOOK(266, SFileOpenArchive);
		//IAT_HOOK(267, SFileOpenFile);
		//IAT_HOOK(268, SFileOpenFileEx);
		//IAT_HOOK(269, SFileReadFile);
		//IAT_HOOK(271, SFileSetFilePointer);
		//IAT_HOOK(272, SFileSetLocale);
		//IAT_HOOK(275, SFileGetArchiveName);
		IAT_HOOK(279, SFileLoadFile);
		IAT_HOOK(280, SFileUnloadFile);
		IAT_HOOK(281, SFileLoadFileEx);
		IAT_HOOK(288, SFileFileExists);
		IAT_HOOK(289, SFileFileExistsEx);
		//IAT_HOOK(291, SFileReadFileEx);
		//IAT_HOOK(293, SFileOpenFileAsArchive);
		//IAT_HOOK(300, SFileOpenPathAsArchive);
#undef 	IAT_HOOK
	}
}}}
