#include <base/warcraft3/player.h>
#include <base/warcraft3/war3_searcher.h>

namespace base { namespace warcraft3 { namespace player {

	static uintptr_t game_war3()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.get_instance(0x0D);
		ptr = *(uintptr_t*)(ptr + 0x10);
		ptr = *(uintptr_t*)(ptr + 0x18);
		return ptr;
	}

	uint32_t local_pid()
	{
		return (uint32_t)(*(uint16_t*)(game_war3() + 0x28));
	}

	uintptr_t from_id(uint32_t pid)
	{
		return *(uintptr_t*)(game_war3() + 0x58 + 0x04 * pid);
	}

	uintptr_t local()
	{
		return from_id(local_pid());
	}

	selection_t* selection(uintptr_t player)
	{
		return (selection_t*)*(uintptr_t*)(player + 0x34);
	}
}}}
