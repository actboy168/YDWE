#pragma once

#include <warcraft3/config.h>
#include <base/util/signal.h>
#include <cstdint>

namespace base { namespace warcraft3 { namespace jass { namespace nf_register {
	extern signal<void, void> event_hook;
	extern signal<void, void>      event_add;
	bool initialize();
}}}} 
