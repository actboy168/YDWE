#pragma once

#include <base/config.h>
#include <base/util/signal.h>
#include <cstdint>

_BASE_BEGIN 
namespace warcraft3 { namespace jass { namespace nf_register {
	extern util::signal<void, void> event_hook;
	extern util::signal<void, void> event_add;
	bool initialize();
}}}
_BASE_END 
