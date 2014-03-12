#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <cstring>

namespace base { namespace warcraft3 {

	jass_vm_t* get_jass_vm()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t vm = *(uintptr_t*)(*(uintptr_t*)(s.get_instance(5) + 0x90) + 0x04);
		return vm ? (jass_vm_t*)(vm - ((s.get_version() > version_120e) ? 0 : 4)) : nullptr;
	}

	jass_vm_t* get_current_jass_vm()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.get_instance(5);
		uintptr_t vm = *(uintptr_t*)(*(uintptr_t*)(ptr + 0x0C) + 4 * *(uintptr_t*)(ptr + 0x14) - 4);
		return vm ? (jass_vm_t*)(vm - ((s.get_version() > version_120e) ? 0 : 4)) : nullptr;
	}
	
	hashtable::native_func_table* get_native_function_hashtable()
	{
		return (hashtable::native_func_table*)(get_war3_searcher().get_instance(5)+0x18);
	}
}}
