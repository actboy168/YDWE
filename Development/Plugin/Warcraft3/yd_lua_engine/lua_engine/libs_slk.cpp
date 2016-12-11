#include "storm.h"
#include <base/lua/make_range.h>
#include <slk/ObjectManager.hpp>
#include <slk/InterfaceStorm.hpp>

namespace base { namespace warcraft3 { namespace lua_engine { namespace slk {
	static int slk_object_index(lua_State* L);
	static int slk_object_pairs(lua_State* L);
	static int slk_create_proxy_table(lua_State* L, lua_CFunction index_func, lua_CFunction pairs_func, uintptr_t upvalue);
}}}

namespace lua
{
	template <>
	int convert_to_lua(lua_State* L, const std::string& v)
	{
		lua_pushlstring(L, v.data(), v.size());
		return 1;
	}

	template <>
	int convert_to_lua(lua_State* L, const std::string_view& v)
	{
		lua_pushlstring(L, v.data(), v.size());
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
		warcraft3::lua_engine::slk::slk_create_proxy_table(
			  L
			  , warcraft3::lua_engine::slk::slk_object_index
			  , warcraft3::lua_engine::slk::slk_object_pairs
			  , (uintptr_t)&(v)
			);
		return 1;
	}
}

namespace warcraft3 { namespace lua_engine { namespace slk {

	class slk_manager
	{
	public:
		slk_manager(::slk::InterfaceStorm& storm)
			: mgr_(storm)
		{ }

		::slk::SlkTable& load(::slk::ROBJECT_TYPE::ENUM type)
		{
			return mgr_.load_singleton<::slk::ROBJECT_TYPE::ENUM, ::slk::SlkTable>(type);
		}

		std::string_view convert_string(const std::string_view& str)
		{
			return mgr_.convert_string(str);
		}

		static int destroy(lua_State* L)
		{
			static_cast<slk_manager*>(lua_touserdata(L, 1))->~slk_manager();
			return 0;
		}

		static int create(lua_State* L, ::slk::InterfaceStorm& storm)
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
		::slk::ObjectManager  mgr_;
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
		::slk::SlkSingle* object_ptr = (::slk::SlkSingle*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
		return lua::make_range(L, *object_ptr);
	}

	static int slk_object_index(lua_State* L)
	{
		::slk::SlkSingle* object_ptr = (::slk::SlkSingle*)(uintptr_t)lua_tointeger(L, lua_upvalueindex(1));
		const char* key = lua_tostring(L, 2);
		auto it = object_ptr->find(key);
		if (it == object_ptr->end())
		{
			lua_pushnil(L);
			return 1;
		}
		return lua::convert_to_lua(L, slk_manager::get(L)->convert_string(it->second.to_string()));
	}

	static int slk_table_pairs(lua_State* L)
	{
		::slk::ROBJECT_TYPE::ENUM type = (::slk::ROBJECT_TYPE::ENUM)lua_tointeger(L, lua_upvalueindex(1));
		::slk::SlkTable& table = slk_manager::get(L)->load(type);
		return lua::make_range(L, table);
	}

	static int slk_table_index(lua_State* L)
	{
		::slk::ROBJECT_TYPE::ENUM type = (::slk::ROBJECT_TYPE::ENUM)lua_tointeger(L, lua_upvalueindex(1));
		::slk::object_id id;

		switch (lua_type(L, 2))
		{
		case LUA_TSTRING:	
			id = ::slk::object_id(std::string_view(lua_tostring(L, 2)));
			break;
		case LUA_TNUMBER:	
			id = ::slk::object_id((uint32_t)lua_tointeger(L, 2));
			break;
		default:
			lua_pushnil(L);
			return 1;
		}

		::slk::SlkTable& table = slk_manager::get(L)->load(type);
		auto it = table.find(id);
		if (it == table.end())
		{
			lua_pushnil(L);
			return 1;
		}

		return lua::convert_to_lua(L, it->second);
	}

	static int slk_create_table(lua_State* L, const char* name, ::slk::ROBJECT_TYPE::ENUM type)
	{
		lua_pushstring(L, name);
		slk_create_proxy_table(L, slk_table_index, slk_table_pairs, type);
		lua_rawset(L, -3);
		return 0;
	}

	class slk_interface_storm
		: public ::slk::InterfaceStorm
	{
	public:
		slk_interface_storm()
			: s_()
		{ }

		bool has(std::string const& path)
		{
			return s_.has_file(path.c_str());
		}

		std::string load(std::string const& path, error_code& ec)
		{
			const void* buf_data = nullptr;
			size_t      buf_size = 0;

			if (!s_.load_file(path.c_str(), &buf_data, &buf_size))
			{
				ec = ERROR_FILE_NOT_FOUND;
				return std::move(std::string());
			}
			std::string result((const char*)buf_data, ((const char*)buf_data) + buf_size);
			s_.unload_file(buf_data);
			return std::move(result);
		}

		static slk_interface_storm& instance()
		{
			static slk_interface_storm storm;
			return storm;
		}

	private:
		storm_dll s_;
	};

	int open(lua_State* L)
	{
		slk_manager::create(L, slk_interface_storm::instance());

		lua_newtable(L);
		{
			slk_create_table(L, "ability", ::slk::ROBJECT_TYPE::ABILITY);
			slk_create_table(L, "buff", ::slk::ROBJECT_TYPE::BUFF);
			slk_create_table(L, "unit", ::slk::ROBJECT_TYPE::UNIT);
			slk_create_table(L, "item", ::slk::ROBJECT_TYPE::ITEM);
			slk_create_table(L, "upgrade", ::slk::ROBJECT_TYPE::UPGRADE);
			slk_create_table(L, "doodad", ::slk::ROBJECT_TYPE::DOODAD);
			slk_create_table(L, "destructable", ::slk::ROBJECT_TYPE::DESTRUCTABLE);
			slk_create_table(L, "misc", ::slk::ROBJECT_TYPE::MISC);
		}
		return 1;
	}
}}}}
