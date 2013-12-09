#pragma once

#include <base/config.h>
#include <functional>

namespace base { namespace warcraft3 {
	_BASE_API void register_game_reset_event(std::function<void(uintptr_t)> func);
}}
