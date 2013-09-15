#include <boost/assign.hpp>
#include <string>
#include <map>

namespace locvar
{
	std::map<std::string, std::string> trigger_data_ui = boost::assign::map_list_of
		("GetSpellTargetUnit", "unit")
		("GetTriggerUnit",     "unit")
		;
}