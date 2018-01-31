#pragma once

#include <base/config.h>
#include <functional>

namespace base { namespace warcraft3 {
	_BASE_API void event_game_reset(std::function<void()> func);
}}
