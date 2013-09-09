#include "../lua/helper.h"
#include <ydwe/util/do_once.h>
#include <ydwe/util/string_ref.h>
#include <ydwe/warcraft3/event.h>
#include <slk/ObjectManager.hpp>
#include "../misc/slk_interface_storm.h"

_BASE_BEGIN
namespace warcraft3 { namespace lua_engine {
	
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
					register_game_reset_event([]{ s_ptr_.reset(); });
				}
			}

			return *s_ptr_;
		}

	private:
		static std::unique_ptr<slk_manager> s_ptr_;
	};

	std::unique_ptr<slk_manager> slk_manager::s_ptr_;

	int jass_slk_create_proxy_table(lua::state* ls, lua::state::cfunction f, uintptr_t upvalue)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushunsigned(upvalue);
				ls->pushcclosure(f, 1);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);
		}
		return 1;
	}

	int jass_slk_object(lua::state* ls)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)ls->tounsigned(lua_upvalueindex(1));
		const char* key = ls->tostring(2);
		auto it = object_ptr->find(key);
		if (it == object_ptr->end())
		{
			ls->pushnil();
			return 1;
		}

		ls->pushstring(it->second.to_string().c_str());
		return 1;
	}

	int jass_slk_table(lua::state* ls)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)ls->tounsigned(lua_upvalueindex(1));
		slk::object_id id;

		switch (ls->type(2))
		{
		case LUA_TSTRING:	
			id = slk::object_id(boost::string_ref(ls->tostring(2)));
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
		
		return jass_slk_create_proxy_table(ls, jass_slk_object, (uintptr_t)&(it->second));
	}

	int slk_create_table(lua::state* ls, const char* name, slk::ROBJECT_TYPE::ENUM type)
	{
		ls->pushstring(name);
		jass_slk_create_proxy_table(ls, jass_slk_table, type);
		ls->rawset(-3);
		return 0;
	}

	int open_slk(lua::state* ls)
	{
		ls->newtable();
		{
			slk_create_table(ls, "ability",      slk::ROBJECT_TYPE::ABILITY);
			slk_create_table(ls, "buff",         slk::ROBJECT_TYPE::BUFF);
			slk_create_table(ls, "unit",         slk::ROBJECT_TYPE::UNIT);
			slk_create_table(ls, "item",         slk::ROBJECT_TYPE::ITEM);
			slk_create_table(ls, "upgrade",      slk::ROBJECT_TYPE::UPGRADE);
			slk_create_table(ls, "doodad",       slk::ROBJECT_TYPE::DOODAD);
			slk_create_table(ls, "destructable", slk::ROBJECT_TYPE::DESTRUCTABLE);
		}
		ls->setglobal("slk");
		return 1;
	}
}}

_BASE_END
