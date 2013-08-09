#pragma once

#include <map>
#include <ydwe/warcraft3/jass/trampoline.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	class jump_func
	{
		const static uint8_t TEMP_REGISTER = 0xD0;

	public:
		uintptr_t create(uint32_t dst, uint32_t tag);

	private:
		std::map<uint32_t, jass::trampoline> mapping_;
	};
}}

_BASE_END
