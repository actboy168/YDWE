#pragma once

#include <base/config.h>
#include <base/warcraft3/jass/opcode.h>

namespace base { 
namespace warcraft3 { namespace jass {

	struct trampoline : public opcode_writer<16>
	{
		typedef opcode_writer<16> mybase;

	public:
		trampoline();
		trampoline(trampoline const&);
		uintptr_t code() const;

	private:
		uint64_t                trampoline_;
		uintptr_t               code_;
	};
}}
}
