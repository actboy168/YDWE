
#include <base/lua/make_range.h>
#include <slk/ObjectManager.hpp>
#include <slk/InterfaceStorm.hpp>

namespace base { namespace warcraft3 { namespace lua_engine {
	static int slk_object_index(lua_State* L);
	static int slk_object_pairs(lua_State* L);
	static int slk_create_proxy_table(lua_State* L, lua_CFunction index_func, lua_CFunction pairs_func, uintptr_t upvalue);
}}

namespace lua
{
	template <>
	int convert_to_lua(lua_State* L, const std::string& v)
	{
		lua_pushstring(L, v.c_str());
		return 1;
	}

	template <>
	int convert_to_lua(lua_State* L, const slk::SlkValue& v)
	{
		lua_pushstring(L, v.to_string().c_str());
		return 1;
	}

	template <>
	int convert_to_lua(lua_State* L, const slk::object_id& v)
	{
		lua_pushstring(L, v.to_string().c_str());
		return 1;
	}

	template <>
	int convert_to_lua(lua_State* L, const slk::SlkSingle& v)
	{
		warcraft3::lua_engine::slk_create_proxy_table(
			  L
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

		std::string_view convert_string(std::string_view const& str)
		{
			return mgr_.convert_string(str);
		}

		static int destroy(lua_State* L)
		{
			static_cast<slk_manager*>(lua_touserdata(L, 1))->~slk_manager();
			return 0;
		}

		static int create(lua_State* L, slk::InterfaceStorm& storm)
		{
			slk_manager* mgr = (slk_manager*)lua_newuserdata(L, sizeof(slk_manager));
			lua_newtable(L);
			lua_pushcclosure(L, slk_manager::destroy, 0);
			lua_setfield(L, -2, "__gc");
			lua_setmetatable(L, -2);
			new (mgr)slk_manager(storm);
			lua_setfield(L, LUA_REGISTRYINDEX, "_JASS_SLK_MGR");
			return 0;
		}

		static slk_manager* get(lua_State* L)
		{
			lua_getfield(L, LUA_REGISTRYINDEX, "_JASS_SLK_MGR");
			slk_manager* mgr = (slk_manager*)lua_touserdata(L, -1);
			lua_pop(L, 1);
			return mgr;
		}

	private:
		slk::ObjectManager  mgr_;
	};

	static int slk_table_newindex(lua_State* /*L*/)
	{
		return 0;
	}

	static int slk_create_proxy_table(lua_State* L, lua_CFunction index_func, lua_CFunction pairs_func, uintptr_t upvalue)
	{
		lua_newtable(L);
		{
			lua_newtable(L);
			{
				lua_pushstring(L, "__index");
				lua_pushinteger(L, upvalue);
				lua_pushcclosure(L, index_func, 1);
				lua_rawset(L, -3);

				lua_pushstring(L, "__newindex");
				lua_pushcclosure(L, slk_table_newindex, 0);
				lua_rawset(L, -3);

				lua_pushstring(L, "__pairs");
				lua_pushinteger(L, upvalue);
				lua_pushcclosure(L, pairs_func, 1);
				lua_rawset(L, -3);
			}
			lua_setmetatable(L, -2);

			lua_pushstring(L, "factory");
			lua_pushinteger(L, upvalue);
			lua_pushcclosure(L, pairs_func, 1);
			lua_rawset(L, -3);
		}
		return 1;
	}

	static int slk_object_pairs(lua_State* L)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
		return lua::make_range(L, *object_ptr);
	}

	static int slk_object_index(lua_State* L)
	{
		slk::SlkSingle* object_ptr = (slk::SlkSingle*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
		const char* key = lua_tostring(L, 2);
		auto it = object_ptr->find(key);
		if (it == object_ptr->end())
		{
			lua_pushnil(L);
			return 1;
		}

		return lua::convert_to_lua(L, it->second);
	}

	static int slk_table_pairs(lua_State* L)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)lua_tointeger(L, lua_upvalueindex(1));
		slk::SlkTable& table = slk_manager::get(L)->load(type);
		return lua::make_range(L, table);
	}

	static int slk_table_index(lua_State* L)
	{
		slk::ROBJECT_TYPE::ENUM type = (slk::ROBJECT_TYPE::ENUM)lua_tointeger(L, lua_upvalueindex(1));
		slk::object_id id;

		switch (lua_type(L, 2))
		{
		case LUA_TSTRING:	
			id = slk::object_id(std::string_view(lua_tostring(L, 2)));
			break;
		case LUA_TNUMBER:	
			id = slk::object_id((uint32_t)lua_tointeger(L, 2));
			break;
		default:
			lua_pushnil(L);
			return 1;
		}

		slk::SlkTable& table = slk_manager::get(L)->load(type);
		auto it = table.find(id);
		if (it == table.end())
		{
			lua_pushnil(L);
			return 1;
		}

		return lua::convert_to_lua(L, it->second);
	}

	static int slk_create_table(lua_State* L, const char* name, slk::ROBJECT_TYPE::ENUM type)
	{
		lua_pushstring(L, name);
		slk_create_proxy_table(L, slk_table_index, slk_table_pairs, type);
		lua_rawset(L, -3);
		return 0;
	}

	class slk_interface_storm
		: public slk::InterfaceStorm
	{
	public:
		slk_interface_storm(lua_State* L, int idx)
			: slk::InterfaceStorm()
			, L_(L)
			, class_(LUA_REFNIL)
		{
			lua_pushvalue(L_, idx);
			class_ = luaL_ref(L, LUA_REGISTRYINDEX);
		}

		~slk_interface_storm()
		{
			luaL_unref(L_, LUA_REGISTRYINDEX, class_); 
		}

		bool has(std::string const& path)
		{
			lua_rawgeti(L_, LUA_REGISTRYINDEX, class_);
			lua_getfield(L_, -1, "has");
			lua_pushvalue(L_, -2);
			lua_pushlstring(L_, path.c_str(), path.size());
			int err = lua_pcall(L_, 2, 1, 0);
			if (err != LUA_OK)
			{
				lua_error(L_);
				return false;
			}
			int suc = lua_toboolean(L_, -1);
			lua_pop(L_, 1);
			return !!suc;
		}

		std::string load(std::string const& path, error_code& ec)
		{
			lua_rawgeti(L_, LUA_REGISTRYINDEX, class_);
			lua_getfield(L_, -1, "load");
			lua_pushvalue(L_, -2);
			lua_pushlstring(L_, path.c_str(), path.size());
			int err = lua_pcall(L_, 2, 1, 0);
			if (err != LUA_OK)
			{
				lua_error(L_);
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}

			size_t bufsize = 0;
			const char* buf = lua_tolstring(L_, -1, &bufsize);
			if (!buf || !bufsize)
			{
				lua_pop(L_, 1);
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}
			std::string retval(buf, bufsize);
			lua_pop(L_, 1);
			return std::move(retval);
		}

		static slk_interface_storm& instance(lua_State* L, int idx)
		{
			static std::unique_ptr<slk_interface_storm> storm;
			storm.reset(new slk_interface_storm(L, idx));
			return *storm;
		}

	private:
		lua_State* L_;
		int class_;
	};

	int jass_slk(lua_State* L)
	{
		slk_manager::create(L, slk_interface_storm::instance(L, 1));

		lua_newtable(L);
		{
			slk_create_table(L, "ability", slk::ROBJECT_TYPE::ABILITY);
			slk_create_table(L, "buff", slk::ROBJECT_TYPE::BUFF);
			slk_create_table(L, "unit", slk::ROBJECT_TYPE::UNIT);
			slk_create_table(L, "item", slk::ROBJECT_TYPE::ITEM);
			slk_create_table(L, "upgrade", slk::ROBJECT_TYPE::UPGRADE);
			slk_create_table(L, "doodad", slk::ROBJECT_TYPE::DOODAD);
			slk_create_table(L, "destructable", slk::ROBJECT_TYPE::DESTRUCTABLE);
			slk_create_table(L, "misc", slk::ROBJECT_TYPE::MISC);
		}
		return 1;
	}
}}}

int luaopen_mapanalyzer(lua_State* L)
{
	static luaL_Reg func[] = {
		{ "new", base::warcraft3::lua_engine::jass_slk },
		{ NULL, NULL }
	};
	luaL_newlibtable(L, func);
	luaL_setfuncs(L, func, 0);
	lua_setglobal(L, "mapanalyzer");
	return 0;
}
