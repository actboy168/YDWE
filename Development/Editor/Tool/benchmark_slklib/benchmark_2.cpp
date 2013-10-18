
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>

#include <base/exception/exception.h>
#include <slk/ObjectManager.hpp>
#include <slk/utility/sequence.h>
#include "InterfaceStormLib.h"

void benchmark_2()
{
	try
	{
		InterfaceStormLib storm;
		slk::ObjectManager mgr(storm);
		if (!storm.open_archive("units.mpq", true))
		{
			throw base::exception("Can't open units.mpq.");
		}
		if (!storm.open_archive("DotA v6.77.w3x", true))
		{
			throw base::exception("Can't open DotA v6.77.w3x.");
		}

		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::ABILITY);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::BUFF);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::DESTRUCTABLE);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::DOODAD);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::ITEM);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::UNIT);
		mgr.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(slk::ROBJECT_TYPE::UPGRADE);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
}
