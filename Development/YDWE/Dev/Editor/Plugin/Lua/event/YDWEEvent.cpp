#include <utility>
#include "YDWEEvent.h"

namespace NYDWE
{

CYDWEEventData::CYDWEEventData()
	: eventDataStore_() {}

CYDWEEventData::TEventDataStore &CYDWEEventData::getDataStore()
{
	return eventDataStore_;
}

boost::optional<CYDWEEventData::TEventDataItem &> CYDWEEventData::getEventDataItem(const std::string &key)
{
	boost::optional<CYDWEEventData::TEventDataItem &> result;
	TEventDataStore::iterator itr = eventDataStore_.find(key);

	if (itr != eventDataStore_.end())
		result = itr->second;

	return std::move(result);
}

void CYDWEEventData::setEventDataItem(const std::string &key, const CYDWEEventData::TEventDataItem &data)
{
	eventDataStore_[key] = data;
}

TYDWEEvent event_array[EVENT_MAXIMUM];

}
