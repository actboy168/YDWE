#pragma once

#include <map>
#include <string>
#include <memory>
#include <base/hook/assembler/writer.h>
#include <base/util/noncopyable.h>
#include <base/util/horrible_cast.h>
#include <warcraft3/jass.h>
#include <warcraft3/jass/func_value.h>
#include <warcraft3/jass/hook.h>
#include <warcraft3/war3_searcher.h>
#include "jassbind.h"
#include "callback.h"
#include "libs_runtime.h"
#include "common.h"

namespace warcraft3::lua_engine {

	template <int(extFunc)(lua_State*, void*)>
	struct lua_to_nativefunction : base::noncopyable
	{
		lua_to_nativefunction(lua_State* L, int luaobj, const jass::func_value* funcdef, void* userdata)
			: L(get_mainthread(L))
			, funcdef(funcdef)
			, luaobj(luaobj)
			, funcentry((uintptr_t)code.data())
			, userdata(userdata)
		{
			using namespace base::hook::assembler;
			code.push(esi);
			code.mov(esi, esp);
			code.add(operand(esi), 8);
			code.push(esi);
			code.mov(ecx, (uintptr_t)this);
			code.call(base::horrible_cast<uintptr_t>(&lua_to_nativefunction::lua_function), funcentry + code.size());
			code.pop(esi);
			code.ret();
			if (!code.executable()) {
				throw std::bad_alloc();
			}
		}

		uintptr_t lua_function(const uintptr_t* paramlist)
		{
			size_t param_size = funcdef->get_param().size();

			for (size_t i = 0; i < param_size; ++i)
			{
				if (funcdef->get_param()[i] == jass::TYPE_REAL)
				{
					jassbind::push_real_precise(L, paramlist[i] ? *(uintptr_t*)paramlist[i] : 0);
				}
				else
				{
					jass_push(L, funcdef->get_param()[i], paramlist[i]);
				}
			}
			int n = extFunc(L, userdata);
			return safe_call_ref(L, luaobj, param_size + n, funcdef->get_return());
		}

    	void* operator new(size_t i)
    	{
        	return _aligned_malloc(i, 32);
    	}

    	void operator delete(void* p)
    	{
        	_aligned_free(p);
    	}

		struct __declspec(align(32)) {
			::base::hook::assembler::writer<32> code;
#pragma warning(suppress:4201 4324)
		};
		lua_State*              L;
		int                     luaobj;
		const jass::func_value* funcdef;
		uintptr_t               funcentry;
		void*                   userdata;
	};
}
