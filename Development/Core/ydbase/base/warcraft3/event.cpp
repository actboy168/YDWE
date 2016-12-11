#include <base/warcraft3/event.h>
#include <base/warcraft3/jass/nf_register.h>

namespace base { namespace warcraft3 {
	void register_game_reset_event(std::function<void(uintptr_t)> func)
	{
		jass::nf_register::initialize();
		jass::nf_register::event_hook.connect(func);
	}
}}
