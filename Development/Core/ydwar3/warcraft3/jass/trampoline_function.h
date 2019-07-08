#pragma once

#include <warcraft3/config.h>
#include <base/util/horrible_cast.h>
#include <warcraft3/jass/opcode.h>
#include <cstdint>

namespace warcraft3::jass {

	_WAR3_API uintptr_t trampoline_create(uintptr_t callback_, uintptr_t param1_, uintptr_t param2_);

	inline size_t trampoline_size()
	{
		return 9 * sizeof(opcode);
	}

	inline uintptr_t trampoline_create(uint32_t (__fastcall*callback_)(uint32_t, uint32_t), uint32_t param1_, uint32_t param2_)
	{
		return trampoline_create(base::horrible_cast<uintptr_t>(callback_), static_cast<uintptr_t>(param1_), static_cast<uintptr_t>(param2_));
	}
}					
