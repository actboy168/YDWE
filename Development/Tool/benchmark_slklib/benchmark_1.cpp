
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <base/exception/exception.h>
#include <slk/ObjectManager.hpp>
#include <slk/utility/sequence.h>
#include <boost/algorithm/string.hpp>
#include "InterfaceStormLib.h"

void Item(slk::SlkTable& dota_slk)
{
	std::ofstream os("Item.txt");
	os << "char* item_info[] = {" << std::endl;

	foreach(auto& it, dota_slk)
	{
		slk::object_id const& id = it.first;
		slk::SlkSingle const& obj = it.second;
		auto name = obj.find("name");
		auto goldcost = obj.find("goldcost");
		if (name != obj.end() && goldcost != obj.end())
		{
			os << "  \"" << id.to_string()
				<< "\", \"" << goldcost->second.to_string()
				<< "\", \"" << name->second.to_string()
				<< "\", " << std::endl;
		}
	}

	os << "};" << std::endl;
	os.close();
}

template <typename ResultT, typename SequenceT>
inline ResultT& SplitHeroId(ResultT& Result, SequenceT& Input)
{
	typedef typename SequenceT::value_type CharT;
	slk::split(Result, Input, slk::basic_char_equal<CharT>(static_cast<CharT>(',')));

	typedef typename ResultT::iterator ForwardIteratorT;
	ForwardIteratorT InBegin = Result.begin();
	ForwardIteratorT InEnd = Result.end();

	for (ForwardIteratorT It = InBegin; It != InEnd; ++It)
	{
		boost::algorithm::trim(*It);
	}
	std::remove_if(Result.begin(), Result.end(), [](typename ResultT::reference& It){ return It.size() != 4;});

	return Result;
}

void HouseAddHero2(slk::SlkTable const& unit_slk, std::map<std::string, slk::SlkSingle>& heromap, slk::SlkSingle const& house)
{
	auto sellunits = house.find("sellunits");
	if (sellunits != house.end())
	{
		std::string unitslist = sellunits->second.to_string();

		std::vector<std::string> what;
		SplitHeroId(what, unitslist);

		for (auto It = what.begin(); It != what.end(); ++It)
		{
			if (('A' <= (*It)[0]) && ((*It)[0] <= 'Z'))
			{
				auto it = unit_slk.find(*It);
				if (it != unit_slk.end())
				{
					slk::SlkSingle const& unit = it->second;
					auto it = unit.find("name");
					if (it != unit.end())
					{
						std::string const& unitname = it->second.to_string();
						heromap[unitname] = unit;
						heromap[unitname][std::string("typeid")] = *It;
					}
				}
			}
		}
	}
}

void Hero(slk::SlkTable const& dota_slk)
{
	std::map<std::string, slk::SlkSingle> heromap;
	foreach(auto& it, dota_slk)
	{
		auto& unit = it.second;
		if (unit.find("sellunits") != unit.end())
		{
			HouseAddHero2(dota_slk, heromap, unit);
		}
	}

	std::ofstream os("Hero.txt");
	os << "char* hero_info[] = {" << std::endl;
	foreach(auto& it, dota_slk)
	{
		slk::object_id const& id = it.first;
		slk::SlkSingle const& obj = it.second;

		if (id.is_hero())
		{
			{
				auto name = obj.find("name");
				if (name != obj.end())
				{
					auto hero = heromap.find(name->second.to_string());
					if (hero != heromap.end())
					{
						slk::object_id newHeroId(id);
						slk::object_id oldHeroId(hero->second["typeid"].to_string());

						os << "  \"" << newHeroId.to_string()
							<< "\", \"" << oldHeroId.to_string()
							<< "\", \"" << name->second.to_string()
							<< "\", " << std::endl;
						continue;
					}
				}
			}
			{
				auto name = obj.find("tip");
				if (name != obj.end())
				{
					auto hero = heromap.find(name->second.to_string());
					if (hero != heromap.end())
					{
						slk::object_id newHeroId(id);
						slk::object_id oldHeroId(hero->second["typeid"].to_string());

						os << "  \"" << newHeroId.to_string()
							<< "\", \"" << oldHeroId.to_string()
							<< "\", \"" << name->second.to_string()
							<< "\", // by tip " << std::endl;
					}
				}
			}
		}
	}

	os << "};" << std::endl;
	os.close();
}

void benchmark_1()
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

		slk::SlkTable ItemTable;
		mgr.load(slk::ROBJECT_TYPE::ITEM, ItemTable);
		Item(ItemTable);

		slk::SlkTable UnitTable;
		mgr.load(slk::ROBJECT_TYPE::UNIT, UnitTable);
		Hero(UnitTable);
	}
	catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}
}
