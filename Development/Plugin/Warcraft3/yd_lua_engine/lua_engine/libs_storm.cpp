#include <lua.hpp>

namespace base { namespace warcraft3 { namespace lua_engine { namespace storm {

	int open(lua_State* L)
	{
		static const char script[] = "\
local mt = {}                         \
function mt.load(filename)            \
	local f = io.open(filename, 'rb') \
	if f then                         \
		local buf = f:read('*a')      \
		f:close()                     \
		return buf                    \
	end                               \
	return nil                        \
end                                   \
function mt.save(filename, buf)       \
	local f = io.open(filename, 'wb') \
	if f then                         \
		f:write(buf)                  \
		f:close()                     \
		return true                   \
	end                               \
	return false                      \
end                                   \
return mt                             \
";
		if (LUA_OK != luaL_loadstring(L, script))
		{
			return lua_error(L);
		}
		lua_call(L, 0, 1);
		return 1;
	}
}}}}
