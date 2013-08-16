
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>

#include <ydwe/exception/exception.h>
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
			throw ydwe::exception("Can't open units.mpq.");
		}
		if (!storm.open_archive("DotA v6.77.w3x", true))
		{
			throw ydwe::exception("Can't open DotA v6.77.w3x.");
		}

		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_ABILITY);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_BUFF);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_DESTRUCTABLE);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_DOODAD);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_ITEM);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_UNIT);
		mgr.load_singleton<slk::ROBJECT_TYPE, slk::SlkTable>(slk::ROBJECT_UPGRADE);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
}
