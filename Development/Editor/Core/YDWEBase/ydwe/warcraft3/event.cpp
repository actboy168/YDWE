#include <ydwe/warcraft3/event.h>
#include <ydwe/warcraft3/jass/nf_register.h>

_BASE_BEGIN 
namespace warcraft3 {
	void register_game_reset_event(std::function<void(void)> func)
	{
		jass::nf_register::initialize();
		jass::nf_register::event_hook.connect(func);
	}
}
_BASE_END
