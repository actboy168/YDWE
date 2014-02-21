#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <cstring>

namespace base { namespace warcraft3 {

	uintptr_t get_jass_virtual_machine()
	{
		war3_searcher& s = get_war3_searcher();
		return *(uintptr_t*)(*(uintptr_t*)(s.get_instance(5)+0x90)+0x04) - ((s.get_version() > version_120e) ? 0: 4);
	}

	uintptr_t get_current_jass_virtual_machine()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t vm = s.get_instance(5);
		vm = *(uintptr_t*)(*(uintptr_t*)(vm+0x0C) + 4 * *(uintptr_t*)(vm+0x14) - 4);
		return vm - ((s.get_version() > version_120e) ? 0: 4);
	}
	
	hashtable::native_func_table* get_native_function_hashtable()
	{
		return (hashtable::native_func_table*)(get_war3_searcher().get_instance(5)+0x18);
	}

	hashtable::variable_table* get_variable_hashtable()
	{
		return (hashtable::variable_table*)(*(uintptr_t*)(get_jass_virtual_machine()+0x285C));
	}

	hashtable::reverse_table* get_string_hashtable()
	{
		return (hashtable::reverse_table*)(*(uintptr_t*)(*(uintptr_t*)(get_jass_virtual_machine()+0x2858)+0x08));
	}

	hashtable::string_fasttable* get_string_fasttable()
	{
		return (hashtable::string_fasttable*)(*(uintptr_t*)(get_jass_virtual_machine()+0x2874));
	}

	uintptr_t get_code_table()
	{
		return *(uintptr_t*)(*(uintptr_t*)(get_jass_virtual_machine() + 0x2888) + 0x08);
	}
}}
