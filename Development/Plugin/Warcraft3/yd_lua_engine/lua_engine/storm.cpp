#include "storm.h"
#include <base/hook/fp_call.h>

namespace warcraft3 {

	storm_dll::storm_dll()
		: mod_(::LoadLibraryW(L"Storm.dll"))
		, valid_(load())
	{ }

	storm_dll::~storm_dll()
	{
		if (mod_)
		{
			::FreeLibrary(mod_);
			mod_ = NULL;
		}
	}

	bool storm_dll::load()
	{
		if (!mod_) return false;
		fn_sfile_load_file_   = get_proc(281); if (!(fn_sfile_load_file_))   return false; 
		fn_sfile_unload_file_ = get_proc(280); if (!(fn_sfile_unload_file_)) return false; 
		fn_sfile_exists_      = get_proc(288); if (!(fn_sfile_exists_))      return false; 

		fn_sfile_open_file_        = get_proc(267); if (!(fn_sfile_open_file_))        return false;
		fn_sfile_close_file_       = get_proc(253); if (!(fn_sfile_close_file_))       return false;
		fn_sfile_get_file_archive_ = get_proc(264); if (!(fn_sfile_get_file_archive_)) return false;
		fn_sfile_get_archive_name_ = get_proc(275); if (!(fn_sfile_get_archive_name_)) return false;
		return true;
	}

	bool storm_dll::valid() const
	{
		return valid_;
	}

	bool storm_dll::load_file(const char* file_name, const void** buffer_ptr, size_t* size_ptr)
	{
		return load_file(NULL, file_name, buffer_ptr, size_ptr, 0, 1, NULL);
	}

	bool storm_dll::load_file(HANDLE archive_handle, const char* file_name, const void** buffer_ptr, size_t* size_ptr, size_t reserve_size, uint32_t search_scope, OVERLAPPED* overlapped_ptr)
	{
		if (!valid()) return false;
		return !!base::std_call<BOOL>(fn_sfile_load_file_, archive_handle, file_name, buffer_ptr, size_ptr, reserve_size, search_scope, overlapped_ptr);
	}

	bool storm_dll::unload_file(const void* buffer)
	{
		if (!valid()) return false;
		return !!base::std_call<BOOL>(fn_sfile_unload_file_, buffer);
	}

	bool storm_dll::has_file(const char* file_name)
	{
		if (!valid()) return false;
		return !!base::std_call<BOOL>(fn_sfile_exists_, file_name);
	}

	uintptr_t storm_dll::get_proc(uint32_t ord)
	{
		return (uintptr_t)::GetProcAddress(mod_, (const char*)ord);
	}

	bool storm_dll::get_mpq_name(char* buf, size_t len)
	{
		HANDLE filehandle = 0;
		HANDLE mpqhandle = 0;
		if (!base::std_call<BOOL>(fn_sfile_open_file_, "war3map.j", &filehandle)) {
			if (!base::std_call<BOOL>(fn_sfile_open_file_, "script\\war3map.j", &filehandle)) {
				return false;
			}
		}
		bool ok = false;
		if (base::std_call<BOOL>(fn_sfile_get_file_archive_, filehandle, &mpqhandle)) {
			if (base::std_call<BOOL>(fn_sfile_get_archive_name_, mpqhandle, buf, len)) {
				ok = true;
			}
		}
        base::std_call<BOOL>(fn_sfile_close_file_, filehandle);
		return ok;
	}
}
