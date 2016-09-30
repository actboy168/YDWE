
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/jass.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include <deque>

namespace base { namespace warcraft3 { namespace japi {

uintptr_t searchUnitDamageFunc()
{
	war3_searcher& s = get_war3_searcher();
	uintptr_t str = s.search_string_ptr("EtherealDamageBonusAlly", sizeof("EtherealDamageBonusAlly"));

	for (uintptr_t ptr = s.search_int_in_text(str); ptr; ptr = s.search_int_in_text(str, ptr+1))
	{
		uintptr_t func = s.current_function(ptr);
		if (ptr - func > 1000)
		{
			return s.search_int_in_rdata(func);
		}
	}

	return 0;
}

uintptr_t getUnitDamageFunc()
{
	static uintptr_t s_result = searchUnitDamageFunc();
	return s_result;
}

struct war3_event_damage_data
{
	uint32_t source_unit;
	uint32_t weapon_type;
	uint32_t unk2;
	uint32_t flag;
	uint32_t amount;
	uint32_t damage_type;
	uint32_t unk6;
	uint32_t unk7;
	uint32_t attack_type;
};

struct event_damage_data
{
	bool                    physical;
	bool                    change;
	war3_event_damage_data* data;
	uint32_t                old_amount;
	uint32_t                new_amount;

	event_damage_data(uint32_t is_physical, war3_event_damage_data* ptr)
		: physical(!!is_physical)
		, change(false)
		, data(ptr)
		, old_amount(0)
		, new_amount(0)
	{ }

	bool is_same(uint32_t* damage1, uint32_t* damage2) const
	{
		if (!old_amount) 
		{
			return true;
		}

		if ((*damage1 ^ *damage2) != 0x80000000)
		{
			return false;
		}

		return old_amount == *damage1;
	}

	uint32_t damage_type()
	{
		uint32_t v = data->damage_type;
		uint32_t n = 0;
		for (; v; v >>= 1)
		{
			++n;
		}

		return n-1;
	}
};


std::deque<event_damage_data> g_edd;

uintptr_t RealGetEventDamage = 0;
uint32_t __cdecl FakeGetEventDamage()
{
	if (!g_edd.empty())
	{
		event_damage_data& edd = g_edd.back();
		if (edd.change)
		{
			return g_edd.back().new_amount;
		}
	}

	return fast_call<uint32_t>(RealGetEventDamage);
}

uintptr_t RealUnitDamageDoneFunc = 0;
uint32_t __fastcall FakeUnitDamageDoneFunc(uint32_t _this, uint32_t _edx, uint32_t* damage1, uint32_t* damage2)
{
	if (!g_edd.empty())
	{
		event_damage_data& edd = g_edd.back();
		if (edd.change && edd.is_same(damage1, damage2))
		{
			edd.change = false;
			float d = jass::from_real(edd.new_amount);
			uint32_t new_damage1 = jass::to_real(d);
			uint32_t new_damage2 = jass::to_real(-d);
			return fast_call<uint32_t>(RealUnitDamageDoneFunc, _this, _edx, &new_damage1, &new_damage2);
		}
	}

	return fast_call<uint32_t>(RealUnitDamageDoneFunc, _this, _edx, damage1, damage2);
}

uintptr_t RealUnitDamageFunc = 0;
uint32_t __fastcall FakeUnitDamageFunc(uint32_t _this, uint32_t _edx, uint32_t a2, war3_event_damage_data* ptr, uint32_t is_physical, uint32_t source_unit)
{
	if ((uintptr_t)FakeUnitDamageDoneFunc != *(uintptr_t*)(*(uint32_t*)_this + 296))
	{
		RealUnitDamageDoneFunc = hook::replace_pointer(*(uint32_t*)_this + 296, (uintptr_t)FakeUnitDamageDoneFunc);
	}

	g_edd.push_back(event_damage_data(is_physical, ptr));
	uint32_t retval = fast_call<uint32_t>(RealUnitDamageFunc, _this, _edx, a2, ptr, is_physical, source_unit);
	g_edd.pop_back();
	return retval;
}


enum EVENT_DAMAGE_DATA_TYPE
{
	EVENT_DAMAGE_DATA_VAILD  = 0,
	EVENT_DAMAGE_DATA_PHYSICAL,
	EVENT_DAMAGE_DATA_IS_ATTACK,
	EVENT_DAMAGE_DATA_IS_RANGED,
	EVENT_DAMAGE_DATA_DAMAGE_TYPE,
	EVENT_DAMAGE_DATA_WEAPON_TYPE,
	EVENT_DAMAGE_DATA_ATTACK_TYPE,
};

uint32_t __cdecl EXGetEventDamageData(uint32_t type)
{
	if (g_edd.empty())
	{
		return 0;
	}

	war3_event_damage_data* ptr = g_edd.back().data;

	switch (type)
	{
	case EVENT_DAMAGE_DATA_VAILD:
		return 1;
	case EVENT_DAMAGE_DATA_PHYSICAL:
		return g_edd.back().physical;
	case EVENT_DAMAGE_DATA_IS_ATTACK:
		return !!(ptr->flag & 0x100);
	case EVENT_DAMAGE_DATA_IS_RANGED:
		return !!(ptr->flag & 0x001);
	case EVENT_DAMAGE_DATA_DAMAGE_TYPE:
		//return ptr->damage_type;
		return g_edd.back().damage_type();
	case EVENT_DAMAGE_DATA_WEAPON_TYPE:
		return ptr->weapon_type;
	case EVENT_DAMAGE_DATA_ATTACK_TYPE:
		return ptr->attack_type;
	default:
		break;
	}

	return 0;
}

bool __cdecl EXSetEventDamage(uint32_t value)
{
	if (g_edd.empty())
	{
		return false;
	}

	event_damage_data& edd = g_edd.back();
	edd.change = true;
	edd.old_amount = edd.data->amount;
	edd.new_amount = *(uint32_t*)value;

	return true;
}

void InitializeEventDamageData()
{
	RealUnitDamageFunc = hook::replace_pointer(getUnitDamageFunc(), (uintptr_t)FakeUnitDamageFunc);
	jass::japi_hook("GetEventDamage", &RealGetEventDamage, (uintptr_t)FakeGetEventDamage);
	jass::japi_add((uintptr_t)EXGetEventDamageData, "EXGetEventDamageData", "(I)I");
	jass::japi_add((uintptr_t)EXSetEventDamage,     "EXSetEventDamage",     "(R)B");
}
}}}
