#include <warcraft3/event.h>
#include <warcraft3/jass/nf_register.h>

namespace warcraft3 {
	void event_game_reset(std::function<void()> func)
	{
		jass::nf_register::initialize();
		jass::nf_register::event_hook.connect(func);
	}
}
