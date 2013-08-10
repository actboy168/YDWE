#pragma once

#include <ydwe/config.h>
#include <ydwe/warcraft3/jass/opcode.h>

_BASE_BEGIN 
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
_BASE_END
