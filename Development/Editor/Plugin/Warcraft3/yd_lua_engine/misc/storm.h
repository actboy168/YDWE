#pragma once

#include <Windows.h>
#include <cstdint>
#include <base/config.h>

_BASE_BEGIN
namespace warcraft3 {

	class storm
	{
	public:
		storm();
		~storm();
		bool load();
		bool valid() const;
		bool load_file(const char* file_name, const void** buffer_ptr, size_t* size_ptr);
		bool load_file(HANDLE archive_handle, const char* file_name, const void** buffer_ptr, size_t* size_ptr, size_t reserve_size, uint32_t search_scope, OVERLAPPED* overlapped_ptr);
		bool unload_file(const void* buffer);
		bool has_file(const char* file_name);

	private:
		uintptr_t get_proc(uint32_t ord);

	private:
		HMODULE   mod_;
		bool      valid_;
		uintptr_t fn_sfile_load_file_;
		uintptr_t fn_sfile_unload_file_;
		uintptr_t fn_sfile_exists_;
	};
}

_BASE_END
