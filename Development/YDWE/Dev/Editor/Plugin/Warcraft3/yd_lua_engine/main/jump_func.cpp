#include "../main/jump_func.h"
#include "../lua/callback.h"
#include <ydwe/lua/state.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/util/singleton.h>
#include <aero/function/fp_call.hpp>

namespace ydwe { namespace warcraft3 { namespace lua_engine {

	class native_jump_func
	{
	public:
		native_jump_func()
			: FuncId(get_string_hashtable()->get("IsPlayerRacePrefSet")->index_)
		{
			native_function::table_hook("IsPlayerRacePrefSet", (uintptr_t*)&RealIsPlayerRacePrefSet, (uintptr_t)FakeIsPlayerRacePrefSet);
		}

		uint32_t FuncId;

	private:
		static uintptr_t RealIsPlayerRacePrefSet;
		static uint32_t __cdecl FakeIsPlayerRacePrefSet(uint32_t player, uint32_t pref)
		{
			if (player == 'YDWE')
			{
				uint32_t retval = lua::callback(pref).call();
				return (0 != retval) ? 1: 0;
			}
			else if (player == 'W3SH')
			{
				uint32_t retval = ((uint32_t(__cdecl*)())pref)();
				return (0 != retval) ? 1: 0;
			}

			return aero::c_call<uint32_t>(RealIsPlayerRacePrefSet, player, pref);
		}
	};

	uintptr_t native_jump_func::RealIsPlayerRacePrefSet = 0;

	uintptr_t jump_func::create(uint32_t dst, uint32_t tag)
	{
		auto it = mapping_.find(dst);
		if (it != mapping_.end())
		{
			return (uintptr_t)it->second.code();
		}

		jass::trampoline& op = mapping_[dst];

		static uint32_t JUMP_FUNC_ID = util::singleton_nonthreadsafe<native_jump_func>::instance().FuncId;

		// return IsPlayerRacePrefSet('YDWE', dst)
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, tag);
		op.push(TEMP_REGISTER);
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, dst);
		op.push(TEMP_REGISTER);
		op.call_native(JUMP_FUNC_ID);
		op.set_ret(jass::RETURN_REGISTER);
		op.ret();
		
		return op.code();
	}
}}}
