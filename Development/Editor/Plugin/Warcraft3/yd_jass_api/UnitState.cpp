#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/jass.h>
#include <cassert>

namespace base { namespace warcraft3 { namespace japi {

enum UNIT_STATE
{
	UNIT_STATE_DAMAGE_DICE  = 0x10,
	UNIT_STATE_DAMAGE_SIDE  = 0x11,
	UNIT_STATE_DAMAGE_BASE  = 0x12,
	UNIT_STATE_DAMAGE_BONUS = 0x13,
	UNIT_STATE_DAMAGE_MIN   = 0x14,
	UNIT_STATE_DAMAGE_MAX   = 0x15,
	UNIT_STATE_DAMAGE_RANGE = 0x16,
	UNIT_STATE_ARMOR        = 0x20,
};

struct unit_property {
	uint32_t unk_00[34];
	int32_t  damage_dice;   // 0x88
	uint32_t unk_8c[2];
	int32_t  damage_side;   // 0x94
	uint32_t unk_98[2];
	int32_t  damage_base;   // 0xA0
	uint32_t unk_a4[2];
	int32_t  damage_bonus;  // 0xAC
	uint32_t unk_a5[106];
	uint32_t damage_ranage; // 0x258
};

uintptr_t RealGetUnitState = 0;
uint32_t _cdecl FakeGetUnitState(uint32_t unit_handle, uint32_t state_type)
{
	switch (state_type)
	{
	case UNIT_STATE_DAMAGE_DICE:
	case UNIT_STATE_DAMAGE_SIDE:
	case UNIT_STATE_DAMAGE_BASE:
	case UNIT_STATE_DAMAGE_BONUS:
	case UNIT_STATE_DAMAGE_MIN:
	case UNIT_STATE_DAMAGE_MAX:
	case UNIT_STATE_DAMAGE_RANGE:
	case UNIT_STATE_ARMOR:
		break;
	default:
		return c_call<uint32_t>(RealGetUnitState, unit_handle, state_type);
	}

	uintptr_t unit_object = handle_to_object(unit_handle);
	if (!unit_object)
	{
		return 0;
	}
	assert('+w3u' == get_object_type(unit_object));

	if (state_type == UNIT_STATE_ARMOR)
	{
		return *(uint32_t*)(unit_object + 0xE0);
	}

	war3_searcher&s = get_war3_searcher();
	unit_property* ptr = (unit_property*)*(uintptr_t*)(unit_object + 0x1E4 + (s.get_version() > version_124c ? 4: 0));
	if (!ptr)
	{
		return 0;
	}

	int32_t retval = 0;
	switch (state_type)
	{
	case UNIT_STATE_DAMAGE_DICE:
		retval = ptr->damage_dice;
		break;
	case UNIT_STATE_DAMAGE_SIDE:
		retval = ptr->damage_side;
		break;
	case UNIT_STATE_DAMAGE_BASE:
		retval = ptr->damage_base;
		break;
	case UNIT_STATE_DAMAGE_BONUS:
		retval = ptr->damage_bonus;
		break;
	case UNIT_STATE_DAMAGE_MIN:
		retval = ptr->damage_base + ptr->damage_bonus + ptr->damage_dice;
		break;
	case UNIT_STATE_DAMAGE_MAX:
		retval = ptr->damage_base + ptr->damage_bonus + ptr->damage_dice * ptr->damage_side;
		break;
	case UNIT_STATE_DAMAGE_RANGE:
		return ptr->damage_ranage;
	default:
		retval = 0;
		break;
	}

	return jass::to_real((float)retval);
}

uintptr_t RealSetUnitState = 0;
void _cdecl FakeSetUnitState(uint32_t unit_handle, uint32_t state_type, uint32_t* value_ptr)
{
	switch (state_type)
	{
	case UNIT_STATE_DAMAGE_DICE:
	case UNIT_STATE_DAMAGE_SIDE:
	case UNIT_STATE_DAMAGE_BASE:
	case UNIT_STATE_DAMAGE_BONUS:
	case UNIT_STATE_DAMAGE_MIN:
	case UNIT_STATE_DAMAGE_MAX:
	case UNIT_STATE_DAMAGE_RANGE:
	case UNIT_STATE_ARMOR:
		break;
	default:
		c_call<void>(RealSetUnitState, unit_handle, state_type, value_ptr);
		return;
	}

	uintptr_t unit_object = handle_to_object(unit_handle);
	if (!unit_object)
	{
		return ;
	}
	assert('+w3u' == get_object_type(unit_object));

	if (state_type == UNIT_STATE_ARMOR)
	{
		*(uint32_t*)(unit_object + 0xE0) = *value_ptr;
		return ;
	}

	war3_searcher&s = get_war3_searcher();
	unit_property* ptr = (unit_property*)*(uintptr_t*)(unit_object + 0x1E4 + (s.get_version() > version_124c ? 4: 0));
	if (!ptr)
	{
		return ;
	}

	switch (state_type)
	{
	case UNIT_STATE_DAMAGE_DICE:
		ptr->damage_dice = (uint32_t)jass::from_real(*value_ptr);
		return ;
	case UNIT_STATE_DAMAGE_SIDE:
		ptr->damage_side = (uint32_t)jass::from_real(*value_ptr);
		return ;
	case UNIT_STATE_DAMAGE_BASE:
		ptr->damage_base = (uint32_t)jass::from_real(*value_ptr);
		return ;
	case UNIT_STATE_DAMAGE_BONUS:
		ptr->damage_bonus = (uint32_t)jass::from_real(*value_ptr);
		return ;
	case UNIT_STATE_DAMAGE_RANGE:
		ptr->damage_ranage = *value_ptr;
		return ;
	case UNIT_STATE_DAMAGE_MIN:
	case UNIT_STATE_DAMAGE_MAX:
	default:
		return ;
	}
}

uint32_t _cdecl EXGetUnitObject(uint32_t unit_handle)
{
	return handle_to_object(unit_handle);
}

void InitializeUnitState()
{
	jass::japi_hook("GetUnitState", &RealGetUnitState, (uintptr_t)FakeGetUnitState);
	jass::japi_hook("SetUnitState", &RealSetUnitState, (uintptr_t)FakeSetUnitState);
	//jass::async_add((uintptr_t)EXGetUnitObject, "EXGetUnitObject", "(Hunit;)I");
}

}}}
