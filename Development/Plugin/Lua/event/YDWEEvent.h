#pragma once

#include <luabind/luabind.hpp>

namespace NYDWE
{

typedef std::function<void(luabind::object&)> TEventData;
typedef std::function<int(TEventData)> TEvent;

enum EVENT_ID
{
	EVENT_WE_START = 0,
	EVENT_WE_EXIT,
	EVENT_PRE_SAVE_MAP,
	EVENT_SAVE_MAP,
	EVENT_TEST_MAP,
	EVENT_INIT_MENU,
	EVENT_MSS_LOAD,
	EVENT_WINDOW_MESSAGE,
	EVENT_DIALOG_MESSAGE,
	EVENT_NEW_OBJECT_ID,
	EVENT_MAXIMUM,
};

extern TEvent event_array[EVENT_MAXIMUM];

void SetupEvent();

}
