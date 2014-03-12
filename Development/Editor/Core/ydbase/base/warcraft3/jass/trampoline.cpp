#include <base/warcraft3/jass/trampoline.h>
#include <base/warcraft3/hashtable.h>

namespace base { 
namespace warcraft3 { namespace jass {

	trampoline::trampoline()
		: mybase()
		, trampoline_(0)
		, code_(0)
	{
		uintptr_t address = 4 + (uintptr_t)&trampoline_;
		uintptr_t offset = address - get_jass_vm()->code_table->table;
		*(uintptr_t*)(address + (offset % 4)) = (uintptr_t)mybase::data();
		code_ = offset / 4;
	}

	trampoline::trampoline(trampoline const& that)
		: mybase(that)
		, trampoline_(0)
		, code_(0)
	{
		uintptr_t address = 4 + (uintptr_t)&trampoline_;
		uintptr_t offset = address - get_jass_vm()->code_table->table;
		*(uintptr_t*)(address + (offset % 4)) = (uintptr_t)mybase::data();
		code_ = offset / 4;
	}

	uintptr_t trampoline::code() const
	{
		return code_;
	}
}}
}
