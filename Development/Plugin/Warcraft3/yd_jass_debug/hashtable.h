#pragma once

#include <functional>
#include <warcraft3/jass/opcode.h>

namespace ht {
	void hashtableEachHandle(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> cb);
	std::vector<base::warcraft3::jass::opcode*> getHandlePos(uint32_t handle);
	void initialize();
}
