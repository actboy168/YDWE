#include <ydwe/warcraft3/jass/trampoline_function.h>
#include <ydwe/warcraft3/jass/trampoline.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/util/singleton.h>
#include <aero/function/fp_call.hpp>
#include <map>

_BASE_BEGIN 
namespace warcraft3 { namespace jass {

	namespace detail {
		uintptr_t RealSetUnitAbilityLevel = 0;
	
		uint32_t __cdecl FakeSetUnitAbilityLevel(uint32_t unit_handle, uint32_t abilcode, uint32_t level)
		{
			if (unit_handle == 'YDWE')
			{
				return ((uint32_t(__fastcall*)(uint32_t))level)(abilcode);
			}
	
			return aero::c_call<uint32_t>(RealSetUnitAbilityLevel, unit_handle, abilcode, level);
		}

		struct trampoline_function : public std::map<uint64_t, trampoline>
		{
			trampoline_function()
				: JUMP_FUNC_ID(get_string_hashtable()->get("SetUnitAbilityLevel")->index_)
			{
				native_function::table_hook("SetUnitAbilityLevel", (uintptr_t*)&detail::RealSetUnitAbilityLevel, (uintptr_t)detail::FakeSetUnitAbilityLevel);
			}

			uint32_t JUMP_FUNC_ID;
		};

		typedef util::singleton_nonthreadsafe<trampoline_function> trampoline_function_s;
	}

	uintptr_t trampoline_create(uintptr_t param_,  uintptr_t callback_)
	{
		const static uint8_t TEMP_REGISTER = 0xD0;
		detail::trampoline_function& tf = detail::trampoline_function_s::instance();

		uint64_t key = ((uint64_t)param_ << 32) | callback_;
		auto it = tf.find(key);
		if (it != tf.end())
		{
			return it->second.code();
		}

		trampoline& op = tf[key];

		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, 'YDWE');
		op.push(TEMP_REGISTER);
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, param_);
		op.push(TEMP_REGISTER);
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, callback_);
		op.push(TEMP_REGISTER);
		op.call_native(tf.JUMP_FUNC_ID);
		op.set_ret(jass::RETURN_REGISTER);
		op.ret();

		return op.code();
	}
}}

_BASE_END
