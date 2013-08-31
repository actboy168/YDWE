#pragma once

#include "Util.hpp"
#include "Buffer.hpp"
#include "SlkTable.hpp"
#include "MetaTable.hpp"
#include "Converter.hpp"
#include <memory>

namespace slk
{
	enum ROBJECT_TYPE
	{
		ROBJECT_ABILITY = 0,
		ROBJECT_BUFF,
		ROBJECT_UNIT,
		ROBJECT_ITEM,
		ROBJECT_UPGRADE,
		ROBJECT_DOODAD,
		ROBJECT_DESTRUCTABLE,
	};

	enum WOBJECT_TYPE
	{
		WOBJECT_ABILITY = 0,
		WOBJECT_BUFF,
		WOBJECT_UNIT,
		WOBJECT_ITEM,
		WOBJECT_UPGRADE,
		WOBJECT_DOODAD,
		WOBJECT_DESTRUCTABLE,
	};

	namespace META_SLK_TYPE
	{
		enum ENUM
		{
			ABILITY = 0,
			BUFF,
			UNIT,
			ITEM,
			UPGRADE,
			DOODAD,
			DESTRUCTABLE,
		};
	}

	namespace SLK_TYPE
	{
		enum ENUM
		{
			DOODAD_DATA = 0,
			DESTRUCTABLE_DATA,
			UPGRADE_DATA,
			ABILITY_DATA,
			BUFF_DATA,
			UNIT_UI,
			UNIT_DATA,
			UNIT_BALANCE,
			UNIT_ABILITIES,
			UNIT_WEAPONS,
			ITEM_DATA,
		};
	}

	namespace TXT_TYPE
	{
		enum ENUM
		{
			UNIT_CAMPAIG_STRINGS = 0,
			UNIT_NEUTRAL_STRINGS,
			UNIT_NIGHTELF_STRINGS,
			UNIT_HUMAN_STRINGS,
			UNIT_ORC_STRINGS,
			UNIT_UNDEAD_STRINGS,
			UNIT_CAMPAIG_FUNC,
			UNIT_HUMAN_FUNC,
			UNIT_NEUTRAL_FUNC,
			UNIT_NIGHTELF_FUNC,
			UNIT_ORC_FUNC,
			UNIT_UNDEAD_FUNC,
			ABILITY_CAMPAIG_STRINGS,
			ABILITY_COMMON_STRINGS,
			ABILITY_HUMAN_STRINGS,
			ABILITY_NEUTRAL_STRINGS,
			ABILITY_NIGHTELF_STRINGS,
			ABILITY_ORC_STRINGS,
			ABILITY_UNDEAD_STRINGS,
			ABILITY_ITEM_STRINGS,
			ABILITY_CAMPAIG_FUNC,
			ABILITY_COMMON_FUNC,
			ABILITY_HUMAN_FUNC,
			ABILITY_NEUTRAL_FUNC,
			ABILITY_NIGHTELF_FUNC,
			ABILITY_ORC_FUNC,
			ABILITY_UNDEAD_FUNC,
			ABILITY_ITEM_FUNC,
			UPGRADE_CAMPAIG_STRINGS,
			UPGRADE_HUMAN_STRINGS,
			UPGRADE_NEUTRAL_STRINGS,
			UPGRADE_NIGHTELF_STRINGS,
			UPGRADE_ORC_STRINGS,
			UPGRADE_UNDEAD_STRINGS,
			UPGRADE_CAMPAIG_FUNC,
			UPGRADE_HUMAN_FUNC,
			UPGRADE_NIGHTELF_FUNC,
			UPGRADE_ORC_FUNC,
			UPGRADE_UNDEAD_FUNC,
			UPGRADE_NEUTRAL_FUNC,
			ITEM_STRINGS,
			ITEM_FUNC,
		};
	}

	namespace OBJ_TYPE
	{
		enum ENUM
		{
			ABILITY = 0,
			BUFF,
			UNIT,
			ITEM,
			UPGRADE,
			DOODAD,
			DESTRUCTABLE,
		};
	}

	class SLKLIB_API ObjectManager : public noncopyable
	{
	public:
		ObjectManager(InterfaceStorm& that);
		buffer load_file(const char* szFilename);
		bool save_file(const char* szFilename, buffer&& buf);
		bool load_ability_profile(SlkTable& table);
		bool load_ability(SlkTable& table);
		bool load_buff(SlkTable& table);
		bool load_unit(SlkTable& table);
		bool load_item(SlkTable& table);
		bool load_upgrde(SlkTable& table);
		bool load_doodad(SlkTable& table);
		bool load_destructable(SlkTable& table);
		bool load_base(ROBJECT_TYPE type, SlkTable& table);
		std::string const& convert_string(std::string const& str);
		Converter& get_converter();

		template <class Table>             bool   load(const char* filename, Table& table);
		template <class Enum, class Table> bool   load(Enum type, Table& table);
		template <class Enum, class Table> bool   save(Enum type, Table const& table);

		template <class Enum, class Table> Table  load_safe(Enum type)
		{
			Table tmp;
			if (!load<Enum, Table>(type, tmp))
			{
				return std::move(Table());
			}
			return std::move(tmp);
		}

		template <class Enum, class Table> Table& load_singleton(Enum type)
		{
			auto const& It = get_table_map<Enum, Table>().find(type);
			if (It == get_table_map<Enum, Table>().end())
			{
				Table& table = get_table_map<Enum, Table>()[type];
				try {
					load<Enum, Table>(type, table);
				}
				catch (...) {
					table.clear();
					throw;
				}
				return table;
			}
			else
			{
				return It->second;
			}
		}

		template <class Enum, class Table> bool save_singleton(Enum type)
		{
			auto const& It = get_table_map<Enum, Table>().find(type);
			if (It == get_table_map<Enum, Table>().end())
			{
				return false;
			}

			return save<Enum, Table>(type, It->second);
		}

	private:
		template <class Enum, class Table> struct TableMap : public HashTable<Enum, Table>::Type { };
		template <class Enum, class Table> TableMap<Enum, Table>& get_table_map();

		InterfaceStorm&                              storm_;
		TableMap<ROBJECT_TYPE, SlkTable>             robject_map_;
		TableMap<WOBJECT_TYPE, SlkTable>             wobject_map_;
		TableMap<META_SLK_TYPE::ENUM, MetaTable>     meta_map_;
		Converter                                    converter_;
	};
}
