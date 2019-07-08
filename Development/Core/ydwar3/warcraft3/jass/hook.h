#pragma once

#include <warcraft3/config.h>
#include <cstdint>
#include <vector>

namespace warcraft3::jass {

	enum hook_type
	{
		HOOK_MEMORY_TABLE         = 0x00000001,
		HOOK_MEMORY_REGISTER      = 0x00000002,
		HOOK_CODE_REGISTER        = 0x00000004,
		HOOK_ONCE_MEMORY_REGISTER = 0x00000008,
	};

	_WAR3_API bool     table_hook        (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     table_unhook      (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     register_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     register_unhook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     async_hook        (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     async_unhook      (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     async_once_hook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     japi_hook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     japi_unhook       (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     japi_once_hook    (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	_WAR3_API bool     japi_add          (uintptr_t func, const char* name, const char* param);
	_WAR3_API bool     japi_table_add    (uintptr_t func, const char* name, const char* param);
	_WAR3_API uint32_t hook              (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag);
	_WAR3_API uint32_t unhook            (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc, uint32_t flag);
}
