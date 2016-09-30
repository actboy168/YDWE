#pragma once

#include <base/config.h>
#include <cstdint>
#include <vector>

namespace base { namespace warcraft3 { namespace jass {

	enum hook_type
	{
		HOOK_MEMORY_TABLE         = 0x00000001,
		HOOK_MEMORY_REGISTER      = 0x00000002,
		HOOK_CODE_REGISTER        = 0x00000004,
		HOOK_ONCE_MEMORY_REGISTER = 0x00000008,
		HOOK_AS_JAPI              = 0x00000100,
	};

	_BASE_API bool     table_hook        (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     table_unhook      (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     register_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     register_unhook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     async_hook        (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     async_unhook      (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     async_once_hook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     async_once_unhook (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     japi_hook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     japi_unhook       (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_BASE_API bool     async_add         (uintptr_t func, const char* name, const char* param);
	_BASE_API bool     japi_add          (uintptr_t func, const char* name, const char* param);
	_BASE_API uint32_t hook              (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag);
	_BASE_API uint32_t unhook            (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag);
}}}
