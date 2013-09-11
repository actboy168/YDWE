#pragma once

#include <base/config.h>
#include <functional>

_BASE_BEGIN 
namespace warcraft3 {
	_BASE_API void register_game_reset_event(std::function<void(void)> func);
}
_BASE_END
