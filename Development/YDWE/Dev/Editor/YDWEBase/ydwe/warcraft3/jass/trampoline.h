#pragma once

#include <ydwe/config.h>
#include <ydwe/warcraft3/jass/opcode.h>

namespace ydwe { namespace warcraft3 { namespace jass {

	struct YDWE_BASE_API trampoline : public opcode_writer<16>
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
}}}
