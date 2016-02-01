#pragma once

#include <cstddef>
#include <string>
#include <vector>	
#include <unordered_map>
#include <base/util/signal.h>
#include <boost/optional.hpp>
#include <boost/variant.hpp>   

namespace NYDWE
{

typedef boost::variant<void *, bool, int, unsigned int, float, double, std::string, std::wstring> TEventDataValue;
typedef std::unordered_map<std::string, TEventDataValue> TEventData;
typedef std::function<int(TEventData&)> TEvent;

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
