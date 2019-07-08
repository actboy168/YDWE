#pragma once

#include <warcraft3/config.h>
#include <base/util/signal.h>
#include <cstdint>

namespace warcraft3::jass::nf_register {
	extern base::signal<void, void> event_hook;
	extern base::signal<void, void> event_add;
	bool initialize();
}
