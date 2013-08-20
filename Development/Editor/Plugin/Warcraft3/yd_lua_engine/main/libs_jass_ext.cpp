#include "../lua/helper.h"
#include <ydwe/util/console.h>

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {

	int jass_enable_console(lua_State* /*L*/)
	{
		util::console::enable();
		util::console::disable_close_button();

		return 0;
	}

	int open_jass_ext(lua::state* ls)
	{
		ls->newtable();
		{
			ls->pushstring("EnableConsole");
			ls->pushcclosure((lua::state::cfunction)jass_enable_console, 0);
			ls->rawset(-3);
		}
		ls->setglobal("jass_ext");
		return 1;
	}
}}

_BASE_END
