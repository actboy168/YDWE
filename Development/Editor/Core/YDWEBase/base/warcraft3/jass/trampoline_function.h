#pragma once

#include <base/config.h>
#include <base/util/horrible_cast.h>
#include <cstdint>

_BASE_BEGIN 
namespace warcraft3 { namespace jass {

	_BASE_API uintptr_t trampoline_create(uintptr_t param_,  uintptr_t callback_);

	template <class C>
	inline uintptr_t trampoline_create(C* this_,  uint32_t (C::*callback_)())
	{
		return trampoline_create(reinterpret_cast<uintptr_t>(this_), horrible_cast<uintptr_t>(callback_));
	}

	inline uintptr_t trampoline_create(uint32_t (__fastcall*callback_)(uint32_t), uint32_t param_)
	{
		return trampoline_create(static_cast<uintptr_t>(param_), horrible_cast<uintptr_t>(callback_));
	}
}}

_BASE_END						
