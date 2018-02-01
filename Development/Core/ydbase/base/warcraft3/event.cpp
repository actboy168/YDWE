#include <base/warcraft3/event.h>
#include <base/warcraft3/jass/nf_register.h>

namespace base { namespace warcraft3 {
	void event_game_reset(std::function<void()> func)
	{
		jass::nf_register::initialize();
		jass::nf_register::event_add.connect(func);
	}
}}
