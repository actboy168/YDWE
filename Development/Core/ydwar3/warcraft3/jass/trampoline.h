#pragma once

#include <warcraft3/config.h>
#include <warcraft3/jass/opcode.h>

namespace warcraft3::jass {

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
}

