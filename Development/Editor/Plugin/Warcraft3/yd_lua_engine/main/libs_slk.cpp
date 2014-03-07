#include "../lua/helper.h"
#include <base/util/do_once.h>
#include <base/util/string_ref.h>
#include <base/warcraft3/event.h>
#include <base/lua/make_range.h>
#include <slk/ObjectManager.hpp>
#include "../misc/slk_interface_storm.h"

namespace base { namespace warcraft3 { namespace lua_engine {

	class slk_manager
	{
	public:
		slk_manager()
			: storm_()
			, mgr_(storm_)
		{ }

		slk::SlkTable& load(slk::ROBJECT_TYPE::ENUM type)
		{
			return mgr_.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(type);
		}

		std::string const& convert_string(std::string const& str)
		{
			return mgr_.convert_string(str);
		}

	private:
		slk_interface_storm storm_;
		slk::ObjectManager  mgr_;

	public:
		static slk_manager& instance()
		{
			if (!s_ptr_)
			{
				s_ptr_.reset(new slk_manager());
				DO_ONCE_NOTHREADSAFE()
				{
					register_game_reset_event([](uintptr_t){ s_ptr_.reset(); });
				}
			}

			return *s_ptr_;
		}

	private:
		static std::unique_ptr<slk_manager> s_ptr_;
	};

	std::unique_ptr<slk_manager> slk_manager::s_ptr_;

	int jass_slk_object_index(lua::state* ls);
	int jass_slk_object_pairs(lua::state* ls);
	int jass_slk_create_proxy_table(lua::state* ls, lua::state::cfunction index_func, lua::state::cfunction pairs_func, uintptr_t upvalue);
}}

namespace lua
{
	template <>
	int convert_to_lua(state* ls, const std::string& v)
	{
		ls->pushstring(v.c_str());
		return 1;
	}

	template <>
	int convert_to_lua(state* ls, const slk::SlkValue& v)
	{
		ls->pushstring(v.to_string().c_str());
		return 1;
	}

	template <>
	int convert_to_lua(state* ls, const slk::object_id& v)
	{
		ls->pushstring(v.to_string().c_str());
		return 1;
	}

	template <>
	int convert_to_lua(state* ls, const slk::SlkSingle& v)
	{
		warcraft3::lua_engine::jass_slk_create_proxy_table(
			  ls
			, warcraft3::lua_engine::jass_slk_object_index
			, warcraft3::lua_engine::jass_slk_object_pairs
			, (uintptr_t)&(v)
			);
		return 1;
	}
}

namespace warcraft3 { namespace lua_engine {
	
	int jass_slk_table_newindex(lua::state* /*ls*/)
	{
		return 0;
	}

	int jass_slk_create_proxy_table(lua::state* ls, lua::state::cfunction index_func, lua::state::cfunction pairs_func, uintptr_t upvalue)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushunsigned(upvalue);
				ls->pushcclosure(index_func, 1);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure(jass_slk_table_newindex, 0);
				ls->rawset(-3);

				ls->pushstring("__pairs");
				ls->pushunsigned(upvalue);
				ls->pushcclosure(pairs_func, 1);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);

			ls->pushstring("factory");
			ls->pushunsigned(upvalue);
			ls->pushcclosure(pairs_func, 1);
			ls->rawset(-3);
		}
		return 1;
	}

	int jass_slk_object_pairs(lua::state* ls)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)ls->tounsigned(lua_upvalueindex(1));
		return lua::make_range(ls, *object_ptr);
	}

	int jass_slk_object_index(lua::state* ls)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)ls->tounsigned(lua_upvalueindex(1));
		const char* key = ls->tostring(2);
		auto it = object_ptr->find(key);
		if (it == object_ptr->end())
		{
			ls->pushnil();
			return 1;
		}

		return lua::convert_to_lua(ls, it->second);
	}

	int jass_slk_table_pairs(lua::state* ls)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)ls->tounsigned(lua_upvalueindex(1));
		slk::SlkTable& table = slk_manager::instance().load(type);
		return lua::make_range(ls, table);
	}

	int jass_slk_table_index(lua::state* ls)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)ls->tounsigned(lua_upvalueindex(1));
		slk::object_id id;

		switch (ls->type(2))
		{
		case LUA_TSTRING:	
			id = slk::object_id(std::string_view(ls->tostring(2)));
			break;
		case LUA_TNUMBER:	
			id = slk::object_id(ls->tounsigned(2));
			break;
		default:
			ls->pushnil();
			return 1;
		}

		slk::SlkTable& table = slk_manager::instance().load(type);
		auto it = table.find(id);
		if (it == table.end())
		{
			ls->pushnil();
			return 1;
		}

		return lua::convert_to_lua(ls, it->second);
	}

	int slk_create_table(lua::state* ls, const char* name, slk::ROBJECT_TYPE::ENUM type)
	{
		ls->pushstring(name);
		jass_slk_create_proxy_table(ls, jass_slk_table_index, jass_slk_table_pairs, type);
		ls->rawset(-3);
		return 0;
	}

	int jass_slk(lua::state* ls)
	{
		ls->newtable();
		{
			slk_create_table(ls, "ability", slk::ROBJECT_TYPE::ABILITY);
			slk_create_table(ls, "buff", slk::ROBJECT_TYPE::BUFF);
			slk_create_table(ls, "unit", slk::ROBJECT_TYPE::UNIT);
			slk_create_table(ls, "item", slk::ROBJECT_TYPE::ITEM);
			slk_create_table(ls, "upgrade", slk::ROBJECT_TYPE::UPGRADE);
			slk_create_table(ls, "doodad", slk::ROBJECT_TYPE::DOODAD);
			slk_create_table(ls, "destructable", slk::ROBJECT_TYPE::DESTRUCTABLE);
		}
		return 1;
	}
}}}
