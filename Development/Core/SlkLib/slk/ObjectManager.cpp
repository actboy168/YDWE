
#include <slk/ObjectManager.hpp>
#include <base/util/buffer.h>
#include <slk/reader/ObjReader.hpp>
#include <slk/reader/TxtReader.hpp>
#include <slk/reader/IniReader.hpp>
#include <slk/reader/SlkReader.hpp>
#include <slk/reader/MetaReader.hpp>
#include <slk/writer/ObjWriter.hpp>
#include <slk/converter/ConvertTable.hpp>
#include <base/file/stream.h>

namespace slk
{
	namespace detail
	{
		inline const char* FileList(META_SLK_TYPE::ENUM type)
		{
			static const char* list[] = {
				"Units\\AbilityMetaData.slk",
				"Units\\AbilityBuffMetaData.slk",
				"Units\\UnitMetaData.slk",
				"Units\\UnitMetaData.slk",
				"Units\\UpgradeMetaData.slk",
				"Doodads\\DoodadMetaData.slk",
				"Units\\DestructableMetaData.slk",
			};

			return list[type];
		}

		inline const char* FileList(OBJ_TYPE::ENUM type)
		{
			static const char* list[] = {
				"war3map.w3a",
				"war3map.w3h",
				"war3map.w3u",
				"war3map.w3t",
				"war3map.w3q",
				"war3map.w3d",
				"war3map.w3b",
			};

			return list[type];
		}

		inline const char* FileList(TXT_TYPE::ENUM type)
		{
			static const char* list[] = {
				"Units\\CampaignUnitStrings.txt",
				"Units\\HumanUnitStrings.txt",
				"Units\\NeutralUnitStrings.txt",
				"Units\\NightElfUnitStrings.txt",
				"Units\\OrcUnitStrings.txt",
				"Units\\UndeadUnitStrings.txt",
				"Units\\CampaignUnitFunc.txt",
				"Units\\HumanUnitFunc.txt",
				"Units\\NeutralUnitFunc.txt",
				"Units\\NightElfUnitFunc.txt",
				"Units\\OrcUnitFunc.txt",
				"Units\\UndeadUnitFunc.txt",

				"Units\\CampaignAbilityStrings.txt",
				"Units\\CommonAbilityStrings.txt",
				"Units\\HumanAbilityStrings.txt",
				"Units\\NeutralAbilityStrings.txt",
				"Units\\NightElfAbilityStrings.txt",
				"Units\\OrcAbilityStrings.txt",
				"Units\\UndeadAbilityStrings.txt",
				"Units\\ItemAbilityStrings.txt",
				"Units\\CampaignAbilityFunc.txt",
				"Units\\CommonAbilityFunc.txt",
				"Units\\HumanAbilityFunc.txt",
				"Units\\NeutralAbilityFunc.txt",
				"Units\\NightElfAbilityFunc.txt",
				"Units\\OrcAbilityFunc.txt",
				"Units\\UndeadAbilityFunc.txt",
				"Units\\ItemAbilityFunc.txt",

				"Units\\CampaignUpgradeStrings.txt",
				"Units\\HumanUpgradeStrings.txt",
				"Units\\NightElfUpgradeStrings.txt",
				"Units\\OrcUpgradeStrings.txt",
				"Units\\UndeadUpgradeStrings.txt",
				"Units\\NeutralUpgradeStrings.txt",
				"Units\\CampaignUpgradeFunc.txt",
				"Units\\HumanUpgradeFunc.txt",
				"Units\\NightElfUpgradeFunc.txt",
				"Units\\OrcUpgradeFunc.txt",
				"Units\\UndeadUpgradeFunc.txt",
				"Units\\NeutralUpgradeFunc.txt",

				"Units\\ItemStrings.txt",
				"Units\\ItemFunc.txt",

				"UI\\MiscData.txt",
				"Units\\MiscData.txt",
				"Units\\MiscGame.txt",
				"war3mapmisc.txt",

			};

			return list[type];
		}

		inline const char* FileList(SLK_TYPE::ENUM type)
		{
			static const char* list[] = {
				"Doodads\\Doodads.slk",
				"Units\\DestructableData.slk",
				"Units\\UpgradeData.slk",
				"Units\\AbilityData.slk",
				"Units\\AbilityBuffData.slk",
				"Units\\UnitUI.slk",
				"Units\\UnitData.slk",
				"Units\\UnitBalance.slk",
				"Units\\UnitAbilities.slk",
				"Units\\UnitWeapons.slk",
				"Units\\ItemData.slk",
			};

			return list[type];
		}
	}

	template <class _Reader, class _Table>
	void TableRead(_Table& table, base::buffer&& buf, bool create_if_not_exists)
	{
		base::buffer_reader reader(buf);
		_Reader::Read(reader, table, create_if_not_exists);
	}

	ObjectManager::ObjectManager(InterfaceStorm& that)
		: storm_(that)
		, converter_(storm_)
	{ }

	base::buffer ObjectManager::load_file(const char* szFilename)
	{
		InterfaceStorm::error_code ec = 0;
		base::buffer buf(storm_.load(szFilename, ec));
		if (ec != 0 || buf.begin() == buf.end())
		{
			throw base::exception("Not found file %s.", szFilename);
		}
		return std::move(buf);
	}

	bool ObjectManager::save_file(const char* szFilename, base::buffer&& buf)
	{
		base::file::write_stream(szFilename).write(std::forward<base::buffer>(buf));
		return false;
	}
	
	template <> SLKLIB_API
	bool ObjectManager::load(OBJ_TYPE::ENUM type, ObjTable<ObjectWithoutOptinal>& table)
	{
		try
		{
			switch (type)
			{
			case OBJ_TYPE::UNIT:
			case OBJ_TYPE::ITEM:
			case OBJ_TYPE::DESTRUCTABLE:
			case OBJ_TYPE::BUFF:
				TableRead<ObjReader>(table, load_file(detail::FileList(type)), true);
				break;
			default:
				throw base::exception("Unknown object type %d.", type);
			}
		}
		catch (base::exception const&)
		{
			return false;
		}

		return true;
	}

	template <> SLKLIB_API
	bool ObjectManager::load(OBJ_TYPE::ENUM type, ObjTable<ObjectWithOptinal>& table)
	{
		try
		{
			switch (type)
			{
			case OBJ_TYPE::DOODAD:
			case OBJ_TYPE::ABILITY:
			case OBJ_TYPE::UPGRADE:
				TableRead<ObjReader>(table, load_file(detail::FileList(type)), true);
				break;
			default:
				throw base::exception("Unknown object type %d.", type);
			}
		}
		catch (base::exception const&)
		{
			return false;
		}

		return true;
	}

	template <> SLKLIB_API
	bool ObjectManager::load(SLK_TYPE::ENUM type, SlkTable& table)
	{
		try
		{
			switch (type)
			{
			case SLK_TYPE::DOODAD_DATA :
			case SLK_TYPE::DESTRUCTABLE_DATA:
			case SLK_TYPE::UPGRADE_DATA:
			case SLK_TYPE::ABILITY_DATA:
			case SLK_TYPE::BUFF_DATA:
			case SLK_TYPE::UNIT_UI:
			case SLK_TYPE::UNIT_DATA:
			case SLK_TYPE::UNIT_BALANCE:
			case SLK_TYPE::UNIT_ABILITIES:
			case SLK_TYPE::UNIT_WEAPONS:
			case SLK_TYPE::ITEM_DATA:
				TableRead<SlkReader>(table, load_file(detail::FileList(type)), true);
				break;
			default:
				throw base::exception("Unknown slk type %d.", type);
			}
		}
		catch (base::exception const&)
		{
			return false;
		}

		return true;
	}

	template <> SLKLIB_API
	bool ObjectManager::load(TXT_TYPE::ENUM type, SlkTable& table)
	{
		try
		{
			switch (type)
			{
			case TXT_TYPE::UNIT_CAMPAIG_STRINGS:
			case TXT_TYPE::UNIT_NEUTRAL_STRINGS:
			case TXT_TYPE::UNIT_NIGHTELF_STRINGS:
			case TXT_TYPE::UNIT_HUMAN_STRINGS:
			case TXT_TYPE::UNIT_ORC_STRINGS:
			case TXT_TYPE::UNIT_UNDEAD_STRINGS:
			case TXT_TYPE::UNIT_CAMPAIG_FUNC:
			case TXT_TYPE::UNIT_HUMAN_FUNC:
			case TXT_TYPE::UNIT_NEUTRAL_FUNC:
			case TXT_TYPE::UNIT_NIGHTELF_FUNC:
			case TXT_TYPE::UNIT_ORC_FUNC:
			case TXT_TYPE::UNIT_UNDEAD_FUNC:
			case TXT_TYPE::UPGRADE_CAMPAIG_STRINGS:
			case TXT_TYPE::UPGRADE_HUMAN_STRINGS:
			case TXT_TYPE::UPGRADE_NEUTRAL_STRINGS:
			case TXT_TYPE::UPGRADE_NIGHTELF_STRINGS:
			case TXT_TYPE::UPGRADE_ORC_STRINGS:
			case TXT_TYPE::UPGRADE_UNDEAD_STRINGS:
			case TXT_TYPE::UPGRADE_CAMPAIG_FUNC:
			case TXT_TYPE::UPGRADE_HUMAN_FUNC:
			case TXT_TYPE::UPGRADE_NIGHTELF_FUNC:
			case TXT_TYPE::UPGRADE_ORC_FUNC:
			case TXT_TYPE::UPGRADE_UNDEAD_FUNC:
			case TXT_TYPE::UPGRADE_NEUTRAL_FUNC:
			case TXT_TYPE::ITEM_STRINGS:
			case TXT_TYPE::ITEM_FUNC:
				TableRead<TxtReader>(table, load_file(detail::FileList(type)), false);
				break;
			case TXT_TYPE::ABILITY_CAMPAIG_STRINGS:
			case TXT_TYPE::ABILITY_COMMON_STRINGS:
			case TXT_TYPE::ABILITY_HUMAN_STRINGS:
			case TXT_TYPE::ABILITY_NEUTRAL_STRINGS:
			case TXT_TYPE::ABILITY_NIGHTELF_STRINGS:
			case TXT_TYPE::ABILITY_ORC_STRINGS:
			case TXT_TYPE::ABILITY_UNDEAD_STRINGS:
			case TXT_TYPE::ABILITY_ITEM_STRINGS:
			case TXT_TYPE::ABILITY_CAMPAIG_FUNC:
			case TXT_TYPE::ABILITY_COMMON_FUNC:
			case TXT_TYPE::ABILITY_HUMAN_FUNC:
			case TXT_TYPE::ABILITY_NEUTRAL_FUNC:
			case TXT_TYPE::ABILITY_NIGHTELF_FUNC:
			case TXT_TYPE::ABILITY_ORC_FUNC:
			case TXT_TYPE::ABILITY_UNDEAD_FUNC:
			case TXT_TYPE::ABILITY_ITEM_FUNC:
			case TXT_TYPE::MISC_UI_DATA:
			case TXT_TYPE::MISC_UNITS_DATA:
			case TXT_TYPE::MISC_UNITS_GAME:
			case TXT_TYPE::MISC_WAR3MAP:
				TableRead<TxtReader>(table, load_file(detail::FileList(type)), true);
				break;
			default:
				throw base::exception("Unknown txt type %d.", type);
			}
		}
		catch (base::exception const&)
		{
			return false;
		}

		return true;
	}
	
	template <> SLKLIB_API
	bool ObjectManager::load(META_SLK_TYPE::ENUM type, MetaTable& table)
	{
		try
		{
			switch (type)
			{
			case META_SLK_TYPE::ABILITY:
			case META_SLK_TYPE::BUFF:
			case META_SLK_TYPE::UNIT:
			case META_SLK_TYPE::ITEM:
			case META_SLK_TYPE::UPGRADE:
			case META_SLK_TYPE::DOODAD:
			case META_SLK_TYPE::DESTRUCTABLE:
				TableRead<MetaReader>(table, load_file(detail::FileList(type)), true);
				break;
			default:
				throw base::exception("Unknown MetaSlk type %d.", type);
			}
		}
		catch (base::exception const&)
		{
			return false;
		}

		return true;
	}

	bool ObjectManager::load_ability_profile(SlkTable& table)
	{
		load(TXT_TYPE::ABILITY_CAMPAIG_STRINGS, table);
		load(TXT_TYPE::ABILITY_COMMON_STRINGS, table);
		load(TXT_TYPE::ABILITY_HUMAN_STRINGS, table);
		load(TXT_TYPE::ABILITY_NEUTRAL_STRINGS, table);
		load(TXT_TYPE::ABILITY_NIGHTELF_STRINGS, table);
		load(TXT_TYPE::ABILITY_ORC_STRINGS, table);
		load(TXT_TYPE::ABILITY_UNDEAD_STRINGS, table);
		load(TXT_TYPE::ABILITY_ITEM_STRINGS, table);
		load(TXT_TYPE::ABILITY_CAMPAIG_FUNC, table);
		load(TXT_TYPE::ABILITY_COMMON_FUNC, table);
		load(TXT_TYPE::ABILITY_HUMAN_FUNC, table);
		load(TXT_TYPE::ABILITY_NEUTRAL_FUNC, table);
		load(TXT_TYPE::ABILITY_NIGHTELF_FUNC, table);
		load(TXT_TYPE::ABILITY_ORC_FUNC, table);
		load(TXT_TYPE::ABILITY_UNDEAD_FUNC, table);
		load(TXT_TYPE::ABILITY_ITEM_FUNC, table);

		return true;
	}

	bool ObjectManager::load_ability(SlkTable& table)
	{
		load(SLK_TYPE::ABILITY_DATA, table);
		SlkTable tmpTable;
		load_ability_profile(tmpTable);
		TableAppendIfExist(table, tmpTable);
		return true;
	}

	bool ObjectManager::load_buff(SlkTable& table)
	{
		load(SLK_TYPE::BUFF_DATA, table);
		SlkTable tmpTable;
		load_ability_profile(tmpTable);
		TableAppendIfExist(table, tmpTable);

		return true;
	}

	bool ObjectManager::load_unit(SlkTable& table)
	{
		load(SLK_TYPE::UNIT_UI, table);
		load(SLK_TYPE::UNIT_DATA, table);
		load(SLK_TYPE::UNIT_BALANCE, table);
		load(SLK_TYPE::UNIT_ABILITIES, table);
		load(SLK_TYPE::UNIT_WEAPONS, table);
		load(TXT_TYPE::UNIT_CAMPAIG_STRINGS, table);
		load(TXT_TYPE::UNIT_HUMAN_STRINGS, table);
		load(TXT_TYPE::UNIT_NEUTRAL_STRINGS, table);
		load(TXT_TYPE::UNIT_NIGHTELF_STRINGS, table);
		load(TXT_TYPE::UNIT_ORC_STRINGS, table);
		load(TXT_TYPE::UNIT_UNDEAD_STRINGS, table);
		load(TXT_TYPE::UNIT_CAMPAIG_FUNC, table);
		load(TXT_TYPE::UNIT_HUMAN_FUNC, table);
		load(TXT_TYPE::UNIT_NEUTRAL_FUNC, table);
		load(TXT_TYPE::UNIT_NIGHTELF_FUNC, table);
		load(TXT_TYPE::UNIT_ORC_FUNC, table);
		load(TXT_TYPE::UNIT_UNDEAD_FUNC, table);

		return true;
	}

	bool ObjectManager::load_item(SlkTable& table)
	{
		load(SLK_TYPE::ITEM_DATA, table);
		load(TXT_TYPE::ITEM_STRINGS, table);
		load(TXT_TYPE::ITEM_FUNC, table);

		return true;
	}

	bool ObjectManager::load_upgrde(SlkTable& table)
	{
		load(SLK_TYPE::UPGRADE_DATA, table);
		load(TXT_TYPE::UPGRADE_CAMPAIG_STRINGS, table);
		load(TXT_TYPE::UPGRADE_NEUTRAL_STRINGS, table);
		load(TXT_TYPE::UPGRADE_NIGHTELF_STRINGS, table);
		load(TXT_TYPE::UPGRADE_HUMAN_STRINGS, table);
		load(TXT_TYPE::UPGRADE_ORC_STRINGS, table);
		load(TXT_TYPE::UPGRADE_UNDEAD_STRINGS, table);
		load(TXT_TYPE::UPGRADE_CAMPAIG_FUNC, table);
		load(TXT_TYPE::UPGRADE_HUMAN_FUNC, table);
		load(TXT_TYPE::UPGRADE_NEUTRAL_FUNC, table);
		load(TXT_TYPE::UPGRADE_NIGHTELF_FUNC, table);
		load(TXT_TYPE::UPGRADE_ORC_FUNC, table);
		load(TXT_TYPE::UPGRADE_UNDEAD_FUNC, table);

		return true;
	}

	bool ObjectManager::load_doodad(SlkTable& table)
	{
		load(SLK_TYPE::DOODAD_DATA, table);
		
		return true;
	}

	bool ObjectManager::load_destructable(SlkTable& table)
	{
		load(SLK_TYPE::DESTRUCTABLE_DATA, table);

		return true;
	}

	bool ObjectManager::load_misc(SlkTable& table)
	{
		load(TXT_TYPE::MISC_UI_DATA, table);
		load(TXT_TYPE::MISC_UNITS_DATA, table);
		load(TXT_TYPE::MISC_UNITS_GAME, table);
		load(TXT_TYPE::MISC_WAR3MAP, table);

		return true;
	}
	
	bool ObjectManager::load_base(ROBJECT_TYPE::ENUM type, SlkTable& table)
	{
		switch (type)
		{
		case ROBJECT_TYPE::UNIT:
			load_unit(table);
			break;
		case ROBJECT_TYPE::ITEM:
			load_item(table);
			break;
		case ROBJECT_TYPE::DESTRUCTABLE:
			load_destructable(table);
			break;
		case ROBJECT_TYPE::BUFF:
			load_buff(table);
			break;
		case ROBJECT_TYPE::DOODAD:
			load_doodad(table);
			break;
		case ROBJECT_TYPE::ABILITY:
			load_ability(table);
			break;
		case ROBJECT_TYPE::UPGRADE:
			load_upgrde(table);
			break;
		default:
			assert(false);
			return false;
		}

		return true;
	}

	namespace detail
	{
		template <OBJECT_PARSER_OPTION Option>
		bool ObjectManagerLoadObjectTable(ObjectManager& mgr, ROBJECT_TYPE::ENUM type, SlkTable& table)
		{
			try
			{
				ObjTable<Option> objTable;

				if (mgr.load(static_cast<OBJ_TYPE::ENUM>(type), objTable))
				{
					SlkTable baseTable;
					if (!mgr.load_base(type, baseTable))
						return false;

					MetaTable const& metaTable = mgr.load_singleton<META_SLK_TYPE::ENUM, MetaTable>(static_cast<META_SLK_TYPE::ENUM>(type));

					TableAppend(table, baseTable);
					ObjTableToSlkTable(table, objTable, baseTable, metaTable, mgr.get_converter());
				}
				else
				{
					if (!mgr.load_base(type, table))
						return false;
				}
			}
			catch (base::exception const&)
			{
				return false;
			}
			return true;
		}

		template <OBJECT_PARSER_OPTION Option>
		bool ObjectManagerLoadObjectTable(ObjectManager& mgr, WOBJECT_TYPE::ENUM type, SlkTable& table)
		{
			try
			{
				ObjTable<Option> objTable;

				if (!mgr.load(static_cast<OBJ_TYPE::ENUM>(type), objTable))
				{
					return false;
				}

				MetaTable const& metaTable = mgr.load_singleton<META_SLK_TYPE::ENUM, MetaTable>(static_cast<META_SLK_TYPE::ENUM>(type));
				ObjTableToSlkTable(table, objTable, metaTable, mgr.get_converter());

			}
			catch (base::exception const&)
			{
				return false;
			}
			return true;
		}

		template <OBJECT_PARSER_OPTION Option>
		bool ObjectManagerSaveObjectTable(ObjectManager& mgr, WOBJECT_TYPE::ENUM type, SlkTable const& table)
		{
			try
			{
				ObjTable<Option> objTable;
				MetaTable const& metaTable = mgr.load_singleton<META_SLK_TYPE::ENUM, MetaTable>(static_cast<META_SLK_TYPE::ENUM>(type));
				SlkTableToObjTable(objTable, table, metaTable);
				return mgr.save(static_cast<OBJ_TYPE::ENUM>(type), objTable);
			}
			catch (base::exception const&)
			{
				return false;
			}
			return true;
		}
	}

	template <> SLKLIB_API 
	bool ObjectManager::load(const char* filename, IniTable& table)
	{
		try {
			base::buffer b = load_file(filename);
			base::buffer_reader reader(b);
			IniReader::Read(reader, table);		
		}
		catch (base::exception const&) {
			return false;
		}
		return true;
	}

	template <> SLKLIB_API 
	bool ObjectManager::load(const char* filename, SlkTable& table)
	{
		try {
			TableRead<SlkReader>(table, load_file(filename), true);	
		}
		catch (base::exception const&) {
			return false;
		}
		return true;
	}

	template <> SLKLIB_API 
	bool ObjectManager::load(ROBJECT_TYPE::ENUM type, SlkTable& table)
	{
		switch (type)
		{
		case ROBJECT_TYPE::UNIT:
		case ROBJECT_TYPE::ITEM:
		case ROBJECT_TYPE::DESTRUCTABLE:
		case ROBJECT_TYPE::BUFF:
			return detail::ObjectManagerLoadObjectTable<ObjectWithoutOptinal>(*this, type, table);
		case ROBJECT_TYPE::DOODAD:
		case ROBJECT_TYPE::ABILITY:
		case ROBJECT_TYPE::UPGRADE:
			return detail::ObjectManagerLoadObjectTable<ObjectWithOptinal>(*this, type, table);
		case ROBJECT_TYPE::MISC:
			return load_misc(table);
		default:
			assert(false);
			return false;
		}
	}

	template <>
	bool ObjectManager::load(WOBJECT_TYPE::ENUM type, SlkTable& table)
	{
		switch (type)
		{
		case WOBJECT_TYPE::UNIT:
		case WOBJECT_TYPE::ITEM:
		case WOBJECT_TYPE::DESTRUCTABLE:
		case WOBJECT_TYPE::BUFF:
			return detail::ObjectManagerLoadObjectTable<ObjectWithoutOptinal>(*this, type, table);
		case WOBJECT_TYPE::DOODAD:
		case WOBJECT_TYPE::ABILITY:
		case WOBJECT_TYPE::UPGRADE:
			return detail::ObjectManagerLoadObjectTable<ObjectWithOptinal>(*this, type, table);
		default:
			assert(false);
			return false;
		}
	}

	template <>
	bool ObjectManager::save(OBJ_TYPE::ENUM type, ObjTable<ObjectWithoutOptinal> const& table)
	{
		try
		{
			switch (type)
			{
			case OBJ_TYPE::UNIT:
			case OBJ_TYPE::ITEM:
			case OBJ_TYPE::DESTRUCTABLE:
			case OBJ_TYPE::BUFF:
				return save_file(detail::FileList(type), ObjWriter::Write<base::buffer>(table));
			default:
				assert(false);
				return false;
			}
		}
		catch (base::exception const&)
		{
			return false;
		}
	}

	template <>
	bool ObjectManager::save(OBJ_TYPE::ENUM type, ObjTable<ObjectWithOptinal> const& table)
	{
		try
		{
			switch (type)
			{
			case OBJ_TYPE::DOODAD:
			case OBJ_TYPE::ABILITY:
			case OBJ_TYPE::UPGRADE:
				return save_file(detail::FileList(type), ObjWriter::Write<base::buffer>(table));
			default:
				assert(false);
				return false;
			}
		}
		catch (base::exception const&)
		{
			return false;
		}
	}

	template <>
	bool ObjectManager::save(WOBJECT_TYPE::ENUM type, SlkTable const& table)
	{
		switch (type)
		{
		case WOBJECT_TYPE::UNIT:
		case WOBJECT_TYPE::ITEM:
		case WOBJECT_TYPE::DESTRUCTABLE:
		case WOBJECT_TYPE::BUFF:
			return detail::ObjectManagerSaveObjectTable<ObjectWithoutOptinal>(*this, type, table);
		case WOBJECT_TYPE::DOODAD:
		case WOBJECT_TYPE::ABILITY:
		case WOBJECT_TYPE::UPGRADE:
			return detail::ObjectManagerSaveObjectTable<ObjectWithOptinal>(*this, type, table);
		default:
			assert(false);
			return false;
		}
	}

	template <> SLKLIB_API
	ObjectManager::TableMap<ROBJECT_TYPE::ENUM, SlkTable>&
		ObjectManager::get_table_map<ROBJECT_TYPE::ENUM, SlkTable>() 
	{ 
		return robject_map_; 
	}

	template <> 
	ObjectManager::TableMap<WOBJECT_TYPE::ENUM, SlkTable>&
		ObjectManager::get_table_map<WOBJECT_TYPE::ENUM, SlkTable>() 
	{ 
		return wobject_map_; 
	}

	template <> SLKLIB_API
	ObjectManager::TableMap<META_SLK_TYPE::ENUM, MetaTable>&
		ObjectManager::get_table_map<META_SLK_TYPE::ENUM, MetaTable>() 
	{ 
		return meta_map_; 
	}

	std::string_view ObjectManager::convert_string(std::string_view const& str)
	{
		return converter_.def_val.Convert(converter_.wes.Convert(str));
	}

	Converter& ObjectManager::get_converter()
	{
		return converter_;
	}
}
