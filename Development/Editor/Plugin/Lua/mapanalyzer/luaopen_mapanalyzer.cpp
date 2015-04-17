
#include <base/lua/make_range.h>
#include <slk/ObjectManager.hpp>
#include <slk/InterfaceStorm.hpp>

namespace base { namespace warcraft3 { namespace lua_engine {
	static int slk_object_index(lua::state* ls);
	static int slk_object_pairs(lua::state* ls);
	static int slk_create_proxy_table(lua::state* ls, lua::cfunction index_func, lua::cfunction pairs_func, uintptr_t upvalue);
}}

namespace lua {
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
		warcraft3::lua_engine::slk_create_proxy_table(
			ls
			, warcraft3::lua_engine::slk_object_index
			, warcraft3::lua_engine::slk_object_pairs
			, (uintptr_t)&(v)
			);
		return 1;
	}
}

namespace warcraft3 { namespace lua_engine {

	class slk_manager
	{
	public:
		slk_manager(slk::InterfaceStorm& storm)
			: mgr_(storm)
		{ }

		slk::SlkTable& load(slk::ROBJECT_TYPE::ENUM type)
		{
			return mgr_.load_singleton<slk::ROBJECT_TYPE::ENUM, slk::SlkTable>(type);
		}

		std::string const& convert_string(std::string const& str)
		{
			return mgr_.convert_string(str);
		}

		static int destroy(lua::state* ls)
		{
			static_cast<slk_manager*>(ls->touserdata(1))->~slk_manager();
			return 0;
		}

		static int create(lua::state* ls, slk::InterfaceStorm& storm)
		{
			slk_manager* mgr = (slk_manager*)ls->newuserdata(sizeof(slk_manager));
			ls->newtable();
			ls->pushcclosure(slk_manager::destroy, 0);
			ls->setfield(-2, "__gc");
			ls->setmetatable(-2);
			new (mgr)slk_manager(storm);
			ls->setfield(LUA_REGISTRYINDEX, "_JASS_SLK_MGR");
			return 0;
		}

		static slk_manager* get(lua::state* ls)
		{
			ls->getfield(LUA_REGISTRYINDEX, "_JASS_SLK_MGR");
			slk_manager* mgr = (slk_manager*)ls->touserdata(-1);
			ls->pop(1);
			return mgr;
		}

	private:
		slk::ObjectManager  mgr_;
	};

	static int slk_table_newindex(lua::state* /*ls*/)
	{
		return 0;
	}

	static int slk_create_proxy_table(lua::state* ls, lua::cfunction index_func, lua::cfunction pairs_func, uintptr_t upvalue)
	{
		ls->newtable();
		{
			ls->newtable();
			{
				ls->pushstring("__index");
				ls->pushinteger(upvalue);
				ls->pushcclosure(index_func, 1);
				ls->rawset(-3);

				ls->pushstring("__newindex");
				ls->pushcclosure(slk_table_newindex, 0);
				ls->rawset(-3);

				ls->pushstring("__pairs");
				ls->pushinteger(upvalue);
				ls->pushcclosure(pairs_func, 1);
				ls->rawset(-3);
			}
			ls->setmetatable(-2);

			ls->pushstring("factory");
			ls->pushinteger(upvalue);
			ls->pushcclosure(pairs_func, 1);
			ls->rawset(-3);
		}
		return 1;
	}

	static int slk_object_pairs(lua::state* ls)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)ls->tointeger(lua_upvalueindex(1));
		return lua::make_range(ls, *object_ptr);
	}

	static int slk_object_index(lua::state* ls)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)ls->tointeger(lua_upvalueindex(1));
		const char* key = ls->tostring(2);
		auto it = object_ptr->find(key);
		if (it == object_ptr->end())
		{
			ls->pushnil();
			return 1;
		}

		return lua::convert_to_lua(ls, it->second);
	}

	static int slk_table_pairs(lua::state* ls)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)ls->tointeger(lua_upvalueindex(1));
		slk::SlkTable& table = slk_manager::get(ls)->load(type);
		return lua::make_range(ls, table);
	}

	static int slk_table_index(lua::state* ls)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)ls->tointeger(lua_upvalueindex(1));
		slk::object_id id;

		switch (ls->type(2))
		{
		case LUA_TSTRING:
			id = slk::object_id(std::string_view(ls->tostring(2)));
			break;
		case LUA_TNUMBER:
			id = slk::object_id((uint32_t)ls->tointeger(2));
			break;
		default:
			ls->pushnil();
			return 1;
		}

		slk::SlkTable& table = slk_manager::get(ls)->load(type);
		auto it = table.find(id);
		if (it == table.end())
		{
			ls->pushnil();
			return 1;
		}

		return lua::convert_to_lua(ls, it->second);
	}

	static int slk_create_table(lua::state* ls, const char* name, slk::ROBJECT_TYPE::ENUM type)
	{
		ls->pushstring(name);
		slk_create_proxy_table(ls, slk_table_index, slk_table_pairs, type);
		ls->rawset(-3);
		return 0;
	}

	class slk_interface_storm
		: public slk::InterfaceStorm
	{
	public:
		slk_interface_storm(lua::state* ls, int idx)
			: slk::InterfaceStorm()
			, L(ls->self())
			, class_(LUA_REFNIL)
		{
			lua_pushvalue(L, idx);
			class_ = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		~slk_interface_storm()
		{
			luaL_unref(L, LUA_REGISTRYINDEX, class_); 
		}

		bool has(std::string const& path)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, class_);
			lua_getfield(L, -1, "has");
			lua_pushvalue(L, -2);
			lua_pushlstring(L, path.c_str(), path.size());
			int err = lua_pcall(L, 2, 1, 0);
			if (err != LUA_OK)
			{
				lua_error(L);
				return false;
			}
			int suc = lua_toboolean(L, -1);
			lua_pop(L, 1);
			return !!suc;
		}

		std::string load(std::string const& path, error_code& ec)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, class_);
			lua_getfield(L, -1, "load");
			lua_pushvalue(L, -2);
			lua_pushlstring(L, path.c_str(), path.size());
			int err = lua_pcall(L, 2, 1, 0);
			if (err != LUA_OK)
			{
				lua_error(L);
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}

			size_t bufsize = 0;
			const char* buf = lua_tolstring(L, -1, &bufsize);
			if (!buf || !bufsize)
			{
				lua_pop(L, 1);
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}
			std::string retval(buf, bufsize);
			lua_pop(L, 1);
			return std::move(retval);
		}

		static slk_interface_storm& instance(lua::state* ls, int idx)
		{
			static std::unique_ptr<slk_interface_storm> storm;
			storm.reset(new slk_interface_storm(ls, idx));
			return *storm;
		}

	private:
		lua_State* L;
		int class_;
	};

	int jass_slk(lua::state* ls)
	{
		slk_manager::create(ls, slk_interface_storm::instance(ls, 1));

		ls->newtable();
		{
			slk_create_table(ls, "ability", slk::ROBJECT_TYPE::ABILITY);
			slk_create_table(ls, "buff", slk::ROBJECT_TYPE::BUFF);
			slk_create_table(ls, "unit", slk::ROBJECT_TYPE::UNIT);
			slk_create_table(ls, "item", slk::ROBJECT_TYPE::ITEM);
			slk_create_table(ls, "upgrade", slk::ROBJECT_TYPE::UPGRADE);
			slk_create_table(ls, "doodad", slk::ROBJECT_TYPE::DOODAD);
			slk_create_table(ls, "destructable", slk::ROBJECT_TYPE::DESTRUCTABLE);
			slk_create_table(ls, "misc", slk::ROBJECT_TYPE::MISC);
		}
		return 1;
	}
}}}

int luaopen_mapanalyzer(lua_State* L)
{
	static luaL_Reg func[] = {
		{ "new", (lua_CFunction)base::warcraft3::lua_engine::jass_slk },
		{ NULL, NULL }
	};
	luaL_newlibtable(L, func);
	luaL_setfuncs(L, func, 0);
	lua_setglobal(L, "mapanalyzer");
	return 0;
}
