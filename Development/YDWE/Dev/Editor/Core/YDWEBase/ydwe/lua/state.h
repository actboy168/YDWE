#pragma once

#include <lua.hpp>

namespace ydwe { namespace lua {

	struct state
	{
		typedef int (__cdecl * cfunction)(state *);

		inline lua_State* self()
		{
			return (lua_State*)this;
		}

		inline static state* newstate(lua_Alloc f, void *ud)
		{
			return (state*)lua_newstate(f, ud);
		}

		inline void close()
		{
			lua_close(self());
		}

		inline state* newthread()
		{
			return (state*)lua_newthread(self());
		}

		inline cfunction atpanic(cfunction panicf)
		{
			return (cfunction)lua_atpanic(self(), (lua_CFunction)panicf);
		}

		inline const lua_Number* version()
		{
			return lua_version(self());
		}

		inline int absindex(int idx)
		{
			return lua_absindex(self(), idx);
		}

		inline int gettop()
		{
			return lua_gettop(self());
		}

		inline void settop(int idx)
		{
			lua_settop(self(), idx);
		}

		inline void pushvalue(int idx)
		{
			lua_pushvalue(self(), idx);
		}

		inline void remove(int idx)
		{
			lua_remove(self(), idx);
		}

		inline void insert(int idx)
		{
			lua_insert(self(), idx);
		}

		inline void replace(int idx)
		{
			lua_replace(self(), idx);
		}

		inline void copy(int fromidx, int toidx)
		{
			lua_copy(self(), fromidx, toidx);
		}

		inline int checkstack(int sz)
		{
			return lua_checkstack(self(), sz);
		}

		inline void xmove(lua::state *to, int n)
		{
			lua_xmove(self(), to->self(), n);
		}

		inline int isnumber(int idx)
		{
			return lua_isnumber(self(), idx);
		}

		inline int isstring(int idx)
		{
			return lua_isstring(self(), idx);
		}

		inline int iscfunction(int idx)
		{
			return lua_iscfunction(self(), idx);
		}

		inline int isuserdata(int idx)
		{
			return lua_isuserdata(self(), idx);
		}

		inline int type(int idx)
		{
			return lua_type(self(), idx);
		}

		inline const char* type_name(int tp)
		{
			return lua_typename(self(), tp);
		}

		inline lua_Number tonumberx(int idx, int *isnum)
		{
			return lua_tonumberx(self(), idx, isnum);
		}

		inline lua_Integer tointegerx(int idx, int *isnum)
		{
			return lua_tointegerx(self(), idx, isnum);
		}

		inline lua_Unsigned tounsignedx(int idx, int *isnum)
		{
			return lua_tounsignedx(self(), idx, isnum);
		}

		inline lua_Number tonumber(int idx)
		{
			return lua_tonumberx(self(), idx, NULL);
		}

		inline lua_Integer tointeger(int idx)
		{
			return lua_tointegerx(self(), idx, NULL);
		}

		inline lua_Unsigned tounsigned(int idx)
		{
			return lua_tounsignedx(self(), idx, NULL);
		}

		inline int toboolean(int idx)
		{
			return lua_toboolean(self(), idx);
		}

		inline const char* tolstring(int idx, size_t *len)
		{
			return lua_tolstring(self(), idx, len);
		}

		inline size_t rawlen(int idx)
		{
			return lua_rawlen(self(), idx);
		}

		inline cfunction tocfunction(int idx)
		{
			return (cfunction)lua_tocfunction(self(), idx);
		}

		inline void* touserdata(int idx)
		{
			return lua_touserdata(self(), idx);
		}

		inline state* tothread(int idx)
		{
			return (state*)lua_tothread(self(), idx);
		}

		inline const void* topointer(int idx)
		{
			return lua_topointer(self(), idx);
		}

		inline int rawequal(int idx1, int idx2)
		{
			return lua_rawequal(self(), idx1, idx2);
		}

		inline int compare(int idx1, int idx2, int op)
		{
			return lua_compare(self(), idx1, idx2, op);
		}

		inline void pushnil()
		{
			lua_pushnil(self());
		}

		inline void pushnumber(lua_Number n)
		{
			lua_pushnumber(self(), n);
		}

		inline void pushinteger(lua_Integer n)
		{
			lua_pushinteger(self(), n);
		}

		inline void pushunsigned(lua_Unsigned n)
		{
			lua_pushunsigned(self(), n);
		}

		inline const char* pushlstring(const char *s, size_t l)
		{
			return lua_pushlstring(self(), s, l);
		}

		inline const char* pushstring(const char *s)
		{
			return lua_pushstring(self(), s);
		}

		inline void pushcclosure(cfunction fn, int n)
		{
			lua_pushcclosure(self(), (lua_CFunction)fn, n);
		}

		inline void pushboolean(int b)
		{
			lua_pushboolean(self(), b);
		}

		inline void pushlightuserdata(void *p)
		{
			lua_pushlightuserdata(self(), p);
		}

		inline int pushthread()
		{
			return lua_pushthread(self());
		}

		inline void getglobal(const char *var)
		{
			lua_getglobal(self(), var);
		}

		inline void gettable(int idx)
		{
			lua_gettable(self(), idx);
		}

		inline void getfield(int idx, const char *k)
		{
			lua_getfield(self(), idx, k);
		}

		inline void rawget(int idx)
		{
			lua_rawget(self(), idx);
		}

		inline void rawgeti(int idx, int n)
		{
			lua_rawgeti(self(), idx, n);
		}

		inline void rawgetp(int idx, const void *p)
		{
			lua_rawgetp(self(), idx, p);
		}

		inline void createtable(int narr, int nrec)
		{
			lua_createtable(self(), narr, nrec);
		}

		inline void* newuserdata(size_t sz)
		{
			return lua_newuserdata(self(), sz);
		}

		inline int getmetatable(int objindex)
		{
			return lua_getmetatable(self(), objindex);
		}

		inline void getuservalue(int idx)
		{
			lua_getuservalue(self(), idx);
		}

		inline void setglobal(const char *var)
		{
			lua_setglobal(self(), var);
		}

		inline void settable(int idx)
		{
			lua_settable(self(), idx);
		}

		inline void setfield(int idx, const char *k)
		{
			lua_setfield(self(), idx, k);
		}

		inline void rawset(int idx)
		{
			lua_rawset(self(), idx);
		}

		inline void rawseti(int idx, int n)
		{
			lua_rawseti(self(), idx, n);
		}

		inline void rawsetp(int idx, const void *p)
		{
			lua_rawsetp(self(), idx, p);
		}

		inline int setmetatable(int objindex)
		{
			return lua_setmetatable(self(), objindex);
		}

		inline void setuservalue(int idx)
		{
			lua_setuservalue(self(), idx);
		}

		inline void newtable()
		{
			createtable(0, 0);
		}

		inline bool isnil(int idx)
		{
			return type(idx) == LUA_TNIL;
		}

		inline bool isboolean(int idx)
		{
			return type(idx) == LUA_TBOOLEAN;
		}

		inline bool istable(int idx)
		{
			return type(idx) == LUA_TTABLE;
		}

		inline bool isfunction(int idx)
		{
			return type(idx) == LUA_TFUNCTION;
		}

		inline void pop(int idx)
		{
			settop(-idx-1);
		}

		inline const char* tostring(int idx)
		{
			return tolstring(idx, NULL);
		}

		inline int pcallk(int nargs, int nresults, int errfunc, int ctx, cfunction k)
		{
			return lua_pcallk(self(), nargs, nresults, errfunc, ctx, (lua_CFunction)k);
		}

		inline int pcall(int nargs, int nresults, int errfunc)
		{
			return pcallk(nargs, nresults, errfunc, 0, NULL);
		}
	};
}}
