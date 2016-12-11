#pragma once

#include <base/lua/luabind/converter_pvoid.h>
#include <base/lua/luabind/converter_std_wstring.h>
#include <base/lua/luabind/converter_std_string_view.h>
#include <base/lua/luabind/converter_uint32_t.h>	 
#include <base/lua/luabind/converter_fs_path.h>

namespace base { namespace lua {
   template <class T>
   inline T read(lua_State* L, int idx)
   {
	   return luabind::default_converter<T>().from(L, idx);
   }
   template <class T>
   inline void push(lua_State* L, T const& value)
   {
	   return luabind::default_converter<T>().to(L, value);
   }
}}
