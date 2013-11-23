
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <slk/ObjectManager.hpp>
#include <slk/table/SlkTable.hpp>
#include <slk/table/IniTable.hpp>
#include <slk/reader/SlkReader.hpp>
#include <slk/reader/TxtReader.hpp>
#include <slk/reader/IniReader.hpp>
#include "InterfaceStormLib.h"

namespace NLua { namespace TableWrap {

	template <class TTable>
	struct Iterator
	{
		Iterator(TTable &table)
			: cur_(table.begin())
			, end_(table.end())
		{ }

		bool valid() const
		{
			return !(cur_ == end_);
		}

		typename TTable::iterator cur_;
		typename TTable::iterator end_;
	};

	template <class TTable>
	Iterator<TTable> begin(TTable &table)
	{
		return Iterator<TTable>(table);
	}
} 

namespace SlkObjectWrap {

	void next(lua_State* pState, TableWrap::Iterator<slk::SlkSingle>& it)
	{
		luabind::object(pState, it.cur_->first).push(pState);
		luabind::object(pState, it.cur_->second.to_string()).push(pState);
		++it.cur_;
	}

	void get(lua_State* pState, slk::SlkSingle const &table, std::string const &key)
	{
		auto const& It = table.find(key);
		if (It == table.end())
		{
			lua_pushnil(pState);
		}
		else
		{
			luabind::object(pState, It->second.to_string()).push(pState);
		}
	}

	void set(slk::SlkSingle &table, std::string const &key, std::string const &val)
	{
		table[key] = slk::SlkValue(val);
	}

} 

namespace SlkTableWrap {

	void next(lua_State* pState, TableWrap::Iterator<slk::SlkTable>& it)
	{
		luabind::object(pState, it.cur_->first.to_string()).push(pState);
		luabind::object(pState, it.cur_->second).push(pState);
		++it.cur_;
	}

	void get(lua_State* pState, slk::SlkTable const &table, std::string const &key)
	{
		auto const& It = table.find(key);
		if (It == table.end())
		{
			lua_pushnil(pState);
		}
		else
		{
			luabind::object(pState, It->second).push(pState);
		}
	}

	void set(slk::SlkTable &table, std::string const &key, slk::SlkSingle const &val)
	{
		table[key] = val;
	}

}

namespace IniObjectWrap {

	void next(lua_State* pState, TableWrap::Iterator<slk::IniSingle>& it)
	{
		luabind::object(pState, it.cur_->first).push(pState);
		luabind::object(pState, it.cur_->second).push(pState);
		++it.cur_;
	}

	void get(lua_State* pState, slk::IniSingle const &table, std::string const &key)
	{
		auto const& It = table.find(key);
		if (It == table.end())
		{
			lua_pushnil(pState);
		}
		else
		{
			luabind::object(pState, It->second).push(pState);
		}
	}

	void set(slk::IniSingle &table, std::string const &key, std::string const &val)
	{
		table[key] = val;
	}
}

namespace IniTableWrap {

	void next(lua_State* pState, TableWrap::Iterator<slk::IniTable>& it)
	{
		luabind::object(pState, it.cur_->first).push(pState);
		luabind::object(pState, it.cur_->second).push(pState);
		++it.cur_;
	}

	void get(lua_State* pState, slk::IniTable const &table, std::string const &key)
	{
		auto const& It = table.find(key);
		if (It == table.end())
		{
			lua_pushnil(pState);
		}
		else
		{
			luabind::object(pState, It->second).push(pState);
		}
	}

	void set(slk::IniTable &table, std::string const &key, slk::IniSingle const &val)
	{
		table[key] = val;
	}
}

struct InterfaceStormWrap : slk::InterfaceStorm, luabind::wrap_base
{
	bool has(std::string const& path) 
	{
		return luabind::call_member<bool>(this, "has", path);
	}

	std::string load(std::string const& path, error_code& ec) 
	{
		std::string ret = luabind::call_member<std::string>(this, "load", path);

		if (ret.empty())
		{
			ec = ERROR_FILE_NOT_FOUND;
		}

		return std::move(ret);
	}
};
} // namespace NLua

int luaopen_mapanalyzer(lua_State *pState)
{
	using namespace luabind;

	module(pState, "mapanalyzer")
	[
		class_<slk::InterfaceStorm, NLua::InterfaceStormWrap>("interface_storm")
			.def(constructor<>())
			.def("has",        &slk::InterfaceStorm::has)
			.def("load",       &slk::InterfaceStorm::load)
		,

		class_<slk::ROBJECT_TYPE::ENUM>("OBJECT_TYPE")
			.enum_("constants")
			[
				value("UNIT",         slk::ROBJECT_TYPE::UNIT),
				value("ITEM",         slk::ROBJECT_TYPE::ITEM),
				value("DESTRUCTABLE", slk::ROBJECT_TYPE::DESTRUCTABLE),
				value("BUFF",         slk::ROBJECT_TYPE::BUFF),
				value("DOODAD",       slk::ROBJECT_TYPE::DOODAD),
				value("ABILITY",      slk::ROBJECT_TYPE::ABILITY),
				value("UPGRADE",      slk::ROBJECT_TYPE::UPGRADE)
			]
		,

		class_<NLua::TableWrap::Iterator<slk::SlkSingle>>("slk_object_iterator")
			.def(constructor<slk::SlkSingle &>())
			.def("valid",  &NLua::TableWrap::Iterator<slk::SlkSingle>::valid)
			.def("next",   &NLua::SlkObjectWrap::next)
		,

		class_<slk::SlkSingle>("slk_object")
			.def(constructor<>())
			.def("get",   &NLua::SlkObjectWrap::get)
			.def("set",   &NLua::SlkObjectWrap::set)
			.def("begin", &NLua::TableWrap::begin<slk::SlkSingle>)
		,

		class_<NLua::TableWrap::Iterator<slk::SlkTable>>("slk_table_iterator")
			.def(constructor<slk::SlkTable &>())
			.def("valid",  &NLua::TableWrap::Iterator<slk::SlkTable>::valid)
			.def("next",   &NLua::SlkTableWrap::next)
		,

		class_<slk::SlkTable>("slk_table")
			.def(constructor<>())
			.def("get",   &NLua::SlkTableWrap::get)
			.def("set",   &NLua::SlkTableWrap::set)
			.def("begin", &NLua::TableWrap::begin<slk::SlkTable>)
		,

		class_<NLua::TableWrap::Iterator<slk::IniSingle>>("ini_object_iterator")
			.def(constructor<slk::IniSingle &>())
			.def("valid",  &NLua::TableWrap::Iterator<slk::IniSingle>::valid)
			.def("next",   &NLua::IniObjectWrap::next)
		,

		class_<slk::IniSingle>("ini_object")
			.def(constructor<>())
			.def("get",   &NLua::IniObjectWrap::get)
			.def("set",   &NLua::IniObjectWrap::set)
			.def("begin", &NLua::TableWrap::begin<slk::IniSingle>)
		,

		class_<NLua::TableWrap::Iterator<slk::IniTable>>("ini_table_iterator")
			.def(constructor<slk::IniTable &>())
			.def("valid",  &NLua::TableWrap::Iterator<slk::IniTable>::valid)
			.def("next",   &NLua::IniTableWrap::next)
		,

		class_<slk::IniTable>("ini_table")
			.def(constructor<>())
			.def("get",   &NLua::IniTableWrap::get)
			.def("set",   &NLua::IniTableWrap::set)
			.def("begin", &NLua::TableWrap::begin<slk::IniTable>)
		,

		class_<slk::ObjectManager>("manager2")
			.def(constructor<slk::InterfaceStorm&>())
			.def("load",      &slk::ObjectManager::load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>)
			.def("load_file", &slk::ObjectManager::load<slk::IniTable>)
			.def("convert",   &slk::ObjectManager::convert_string)
	];

	return 0;
}
