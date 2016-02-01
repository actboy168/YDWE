/*
 * Events definition
 */
#ifndef YDWE_EVENT_H_INCLUDED
#define YDWE_EVENT_H_INCLUDED

#include <cstddef>
#include <string>
#include <vector>	
#include <unordered_map>
#include <base/util/signal.h>
#include <boost/optional.hpp>
#include <boost/variant.hpp>   

namespace NYDWE
{

/// Event data
class CYDWEEventData
{
public:
	typedef boost::variant<void *, bool, int, unsigned int, float, double, std::string, std::wstring> TEventDataItem;
	typedef std::unordered_map<std::string, TEventDataItem> TEventDataStore;

public:
	CYDWEEventData();

public:
	boost::optional<TEventDataItem &> getEventDataItem(const std::string &key);
	void setEventDataItem(const std::string &key, const TEventDataItem &data);

	TEventDataStore &getDataStore();

	template <typename T>
	T *getEventData()
	{
		TEventDataStore::iterator itr = eventDataStore_.find(key);

		if (itr == eventDataStore_.end())
			return NULL;

		return boost::get<T>(&itr->second);
	}
	
	template <typename T>
	void setEventData(const std::string &key, const T &data)
	{
		eventDataStore_[key] = data;
	}

private:
	TEventDataStore eventDataStore_;
};

typedef base::signal<int, CYDWEEventData&> TYDWEEvent;

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

extern TYDWEEvent event_array[EVENT_MAXIMUM];

namespace NGameHook 
{
	void SetupEvent();
}

}

template <typename Result>
bool results_is_failed(Result r)
{
	return (std::find_if(r.begin(), r.end(), [](Result::value_type const& e)->bool { return e < 0; }) != r.end());
}

#endif // YDWE_EVENT_H_INCLUDED
