#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>		   
#include <base/warcraft3/jass/opcode.h>

namespace base { namespace warcraft3 {

	jass_vm_t* get_jass_vm(int index)
	{
		uintptr_t ptr = get_war3_searcher().get_instance(5);
		uintptr_t vm = *(uintptr_t*)(*(uintptr_t*)(ptr + 0x90) + 0x04 * index);
		return vm ? (jass_vm_t*)(vm - ((get_war3_searcher().get_version() > version_120e) ? 0 : 4)) : nullptr;
	}

	jass_vm_t* get_jass_thread()
	{
		uintptr_t ptr = get_war3_searcher().get_instance(5);
		uint32_t index = *(uintptr_t*)(ptr + 0x14);
		if (index == 0) {
			return 0;
		}
		return *(jass_vm_t**)(*(uintptr_t*)(ptr + 0x0C) + 4 * index - 4);
	}

	hashtable::native_func_table* get_native_function_hashtable()
	{
		return (hashtable::native_func_table*)(get_war3_searcher().get_instance(5)+0x18);
	}
}}
