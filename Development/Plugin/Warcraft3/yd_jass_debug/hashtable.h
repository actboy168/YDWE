#pragma once

#include <functional>

namespace ht {
	void hashtableEachHandle(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> cb);
	uint32_t getHandlePos(uint32_t handle);
	void initialize();
}
