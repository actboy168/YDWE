#include <ydwe/warcraft3/jass/trampoline_function.h>
#include <ydwe/warcraft3/detail/nf_register.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/jass/trampoline.h>
#include <ydwe/warcraft3/native_function.h>
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

		std::map<uint64_t, trampoline> trampoline_mapping;
		uint32_t                       jump_function_id = 0;
		bool                           initialized = false;

		void trampoline_initialize()
		{
			if (initialized)
			{
				return ;
			}

			initialized = true;
			jump_function_id = get_string_hashtable()->get("SetUnitAbilityLevel")->index_;
			table_hook("SetUnitAbilityLevel", (uintptr_t*)&detail::RealSetUnitAbilityLevel, (uintptr_t)detail::FakeSetUnitAbilityLevel);
			nf_register::initialize();
			nf_register::event_hook.connect([&]()
			{
				initialized = false;
				trampoline_mapping.clear();
				jump_function_id = 0;
			});
		}
	}

	uintptr_t trampoline_create(uintptr_t param_,  uintptr_t callback_)
	{
		detail::trampoline_initialize();
		const static uint8_t TEMP_REGISTER = 0xD0;

		uint64_t key = ((uint64_t)param_ << 32) | callback_;
		auto it = detail::trampoline_mapping.find(key);
		if (it != detail::trampoline_mapping.end())
		{
			return it->second.code();
		}

		trampoline& op = detail::trampoline_mapping[key];

		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, 'YDWE');
		op.push(TEMP_REGISTER);
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, param_);
		op.push(TEMP_REGISTER);
		op.mov(TEMP_REGISTER, jass::OPCODE_VARIABLE_INTEGER, callback_);
		op.push(TEMP_REGISTER);
		op.call_native(detail::jump_function_id);
		op.set_ret(jass::RETURN_REGISTER);
		op.ret();

		return op.code();
	}

	void trampoline_clear()
	{

	}
}}

_BASE_END
