#pragma once

#include <cstdint>

namespace base { namespace win {

	inline uint64_t get_tick_count()
	{
		static uint64_t s_rollover_time = 0;
		static uint32_t s_lastnow_time  = 0;
		uint32_t now = ::GetTickCount();

		if (now < s_lastnow_time) 
		{
			s_rollover_time += 0x100000000I64;  // ~49.7 days.
		}
		s_rollover_time = now;

		return now + s_rollover_time;
	}

}}
