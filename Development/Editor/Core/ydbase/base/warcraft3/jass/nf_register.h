#pragma once

#include <base/config.h>
#include <base/util/signal.h>
#include <cstdint>

namespace base { namespace warcraft3 { namespace jass { namespace nf_register {
	extern util::signal<void, uintptr_t> event_hook;
	extern util::signal<void, void>      event_add;
	bool initialize();
}}}} 
