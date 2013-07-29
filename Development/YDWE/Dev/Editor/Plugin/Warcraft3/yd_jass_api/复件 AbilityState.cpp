#include <aero/function/fp_call.hpp>
#include <ydwe/hook/iat.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/hook/inline.h>
#include <ydwe/util/console.h>

namespace ydwe { namespace warcraft3 { namespace japi {

	enum ABILITY_STATE
	{
		ABILITY_STATE_COOLDOWN  = 0x01,
	};

	enum ABILITY_DATA_TYPE
	{
		ABILITY_DATA_TARGS = 0,
		ABILITY_DATA_CAST,
		ABILITY_DATA_DUR,
		ABILITY_DATA_HERODUR,
		ABILITY_DATA_COST,
		ABILITY_DATA_COOL,
		ABILITY_DATA_AREA,
		ABILITY_DATA_RNG,
		ABILITY_DATA_DATA_A,
		ABILITY_DATA_DATA_B,
		ABILITY_DATA_DATA_C,
		ABILITY_DATA_DATA_D,
		ABILITY_DATA_DATA_E,
		ABILITY_DATA_DATA_F,
		ABILITY_DATA_DATA_G,
		ABILITY_DATA_DATA_H,
		ABILITY_DATA_DATA_I,
		ABILITY_DATA_UNITID,
		ABILITY_DATA_18,
		ABILITY_DATA_19,
		ABILITY_DATA_BUFFID_PTR,
		ABILITY_DATA_21,
		ABILITY_DATA_22,
		ABILITY_DATA_23,
		ABILITY_DATA_EFCTID_PTR,
		ABILITY_DATA_25,
	};

	struct ability_table
	{
		uint32_t unk00;
		uint32_t unk01;
		uint32_t unk02;
		uint32_t unk03;
		uint32_t unk04;
		uint32_t unk05;
		uint32_t unk06;
		uint32_t unk07;
		uint32_t unk08;
		uint32_t unk09;
		uint32_t unk0A;
		uint32_t unk0B;
		uint32_t unk0C;
		uint32_t unk0D;
		uint32_t unk0E;
		uint32_t unk0F;
	};

	struct ability_data_table
	{
		uint32_t data[26];
	};

	struct ability_data
	{
		uint32_t unk00;
		uint32_t unk01;
		uint32_t unk02;
		uint32_t unk03;
		uint32_t unk04;
		uint32_t unk05;
		uint32_t unk06;
		uint32_t unk07;
		uint32_t unk08;
		uint32_t unk09;
		uint32_t unk0A;
		uint32_t unk0B;                 // 0x2C
		uint32_t unk0C;
		uint32_t unk0D;
		uint32_t unk0E;
		uint32_t unk0F;
		uint32_t unk10;
		uint32_t unk11;
		uint32_t unk12;
		uint32_t unk13;
		uint32_t            max_level_; // 0x50
		ability_data_table* tabel_;     // 0x54
	};

	struct object_id_64
	{
		uint32_t a;
		uint32_t b;

		object_id_64(uint32_t a_, uint32_t b_)
			: a(a_)
			, b(b_)
		{ }

		bool is_valid() const
		{
			return (a & b) != (uint32_t)(-1);
		}
	};

	uint32_t GetAbilityObjectById64(object_id_64 const& id)
	{
		ability_table* table_ptr = *(ability_table**)((uintptr_t)::GetModuleHandleW(L"Game.dll") + 0x00ACE5E0);

		if (id.a >> 31)
		{
			if ((id.a & 0x7FFFFFFF) < table_ptr->unk0F)
			{
				if (*(uint32_t *)(table_ptr->unk0B + 8 * id.a) == -2)
				{
					uint32_t v4 = *(uint32_t *)(table_ptr->unk0B + 8 * id.a + 4);
					if (v4 && (!*(uint32_t*)(v4 + 0x20)) && (*(uint32_t*)(v4 + 0x18) == id.b))
					{
						return *(uintptr_t*)(v4 + 0x54);
					}
				}
			}
		}
		else
		{
			if (id.a < table_ptr->unk07)
			{
				if (*(uint32_t *)(table_ptr->unk03 + 8 * id.a) == -2)
				{
					uint32_t v5 = *(uint32_t *)(table_ptr->unk03 + 8 * id.a + 4);
					if (v5 && (!*(uint32_t*)(v5 + 0x20)) && (*(uint32_t*)(v5 + 0x18) == id.b))
					{
						return *(uintptr_t*)(v5 + 0x54);
					}
				}
			}
		}
		return 0;
	}

	uint32_t GetAbilityCooldown(uintptr_t ability_ptr)
	{
		if (ability_ptr)
		{
			uintptr_t genttimer_ptr = *(uintptr_t*)(ability_ptr + 0xDC);
			if (genttimer_ptr)
			{
				float current  = jass::from_real(*(uint32_t*)(*(uint32_t*)(genttimer_ptr + 12) + 64));
				float end_time = jass::from_real(*(uint32_t*)(genttimer_ptr + 4));
				return jass::to_real(end_time - current);
			}
		}

		return jass::to_real(0.f);
	}

	bool     SetAbilityCooldown(uintptr_t ability_ptr, float value)
	{	
		if (ability_ptr)
		{
			uintptr_t genttimer_ptr = *(uintptr_t*)(ability_ptr + 0xDC);
			if (genttimer_ptr)
			{
				float current = jass::from_real(*(uint32_t*)(*(uint32_t*)(genttimer_ptr + 12) + 64));
				*(uint32_t*)(genttimer_ptr + 4) =  jass::to_real(current + value);
				return true;
			}
		}

		return false;
	}

	uint32_t  __cdecl EXGetUnitObject(uint32_t unit_handle);

	uintptr_t __cdecl EXGetUnitAbilityObject(uint32_t unit_handle, uint32_t abil_code)
	{
		uintptr_t unit_ptr = EXGetUnitObject(unit_handle);

		if (unit_ptr)
		{
			object_id_64* abil_id = (object_id_64*)(unit_ptr + 0x1DC);

			for (; abil_id->is_valid();)
			{
				uint32_t ability_ptr = GetAbilityObjectById64(*abil_id);

				if (!ability_ptr)
				{
					return 0;
				}

				if (abil_code == *(uintptr_t*)(ability_ptr + 0x34))
				{
					return ability_ptr;
				}

				abil_id = (object_id_64*)(ability_ptr + 0x24);
			}
		}

		return 0;
	}

	ability_data* GetUnitAbilityDataObject(uint32_t unit_handle, uint32_t abil_code)
	{
		uintptr_t ability_ptr = EXGetUnitAbilityObject(unit_handle, abil_code);

		if (ability_ptr)
		{
			return *(ability_data**)(ability_ptr + 0x54);
		}

		return nullptr;
	}

	ability_data_table* GetUnitAbilityDataTableObject(uint32_t unit_handle, uint32_t abil_code, uint32_t level)
	{
		ability_data* ptr = GetUnitAbilityDataObject(unit_handle, abil_code);

		if (ptr && ptr->unk0B && (ptr->max_level_ > level))
		{
			return &(ptr->tabel_[level]);
		}

		return nullptr;
	}

	uintptr_t __cdecl EXGetUnitAbilityId(uint32_t unit_handle, uint32_t index)
	{
		uintptr_t unit_ptr = EXGetUnitObject(unit_handle);

		if (unit_ptr)
		{
			object_id_64* abil_id = (object_id_64*)(unit_ptr + 0x1DC);

			for (; abil_id->is_valid(); --index)
			{
				uint32_t ability_ptr = GetAbilityObjectById64(*abil_id);

				if (!ability_ptr)
				{
					return 0;
				}

				if (index == 0)
				{
					return *(uintptr_t*)(ability_ptr + 0x34);
				}

				abil_id = (object_id_64*)(ability_ptr + 0x24);
			}
		}

		return 0;
	}

	uint32_t  __cdecl EXGetUnitAbilityState(uint32_t unit_handle, uint32_t abil_code, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_STATE_COOLDOWN:
			return GetAbilityCooldown(EXGetUnitAbilityObject(unit_handle, abil_code));
		default:
			break;
		}

		return jass::to_real(0.f);
	}

	bool      __cdecl EXSetUnitAbilityState(uint32_t unit_handle, uint32_t abil_code, uint32_t state_type, uint32_t* value_ptr)
	{
		switch (state_type)
		{
		case ABILITY_STATE_COOLDOWN:
			return SetAbilityCooldown(EXGetUnitAbilityObject(unit_handle, abil_code), jass::from_real(*value_ptr));
		default:
			break;
		}

		return false;
	}
	
	uint32_t  __cdecl EXGetUnitAbilityDataR(uint32_t unit_handle, uint32_t abil_code, uint32_t level, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_COOL:
		case ABILITY_DATA_AREA:
		case ABILITY_DATA_RNG:
		case ABILITY_DATA_DATA_A:
		case ABILITY_DATA_DATA_B:
		case ABILITY_DATA_DATA_C:
		case ABILITY_DATA_DATA_D:
		case ABILITY_DATA_DATA_E:
		case ABILITY_DATA_DATA_F:
		case ABILITY_DATA_DATA_G:
		case ABILITY_DATA_DATA_H:
		case ABILITY_DATA_DATA_I:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetUnitAbilityDataTableObject(unit_handle, abil_code, level);
				if (ptr)
				{
					return ptr->data[state_type];
				}
			}
			break;
		default:
			break;
		}

		return jass::to_real(0.f);
	}

	uint32_t  __cdecl EXGetUnitAbilityDataI(uint32_t unit_handle, uint32_t abil_code, uint32_t level, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_COOL:
		case ABILITY_DATA_AREA:
		case ABILITY_DATA_RNG:
		case ABILITY_DATA_DATA_A:
		case ABILITY_DATA_DATA_B:
		case ABILITY_DATA_DATA_C:
		case ABILITY_DATA_DATA_D:
		case ABILITY_DATA_DATA_E:
		case ABILITY_DATA_DATA_F:
		case ABILITY_DATA_DATA_G:
		case ABILITY_DATA_DATA_H:
		case ABILITY_DATA_DATA_I:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetUnitAbilityDataTableObject(unit_handle, abil_code, level);
				if (ptr)
				{
					return ptr->data[state_type];
				}
			}
			break;
		default:
			break;
		}

		return 0;
	}

	bool      __cdecl EXSetUnitAbilityDataR(uint32_t unit_handle, uint32_t abil_code, uint32_t level, uint32_t state_type, uint32_t* value_ptr)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_COOL:
		case ABILITY_DATA_AREA:
		case ABILITY_DATA_RNG:
		case ABILITY_DATA_DATA_A:
		case ABILITY_DATA_DATA_B:
		case ABILITY_DATA_DATA_C:
		case ABILITY_DATA_DATA_D:
		case ABILITY_DATA_DATA_E:
		case ABILITY_DATA_DATA_F:
		case ABILITY_DATA_DATA_G:
		case ABILITY_DATA_DATA_H:
		case ABILITY_DATA_DATA_I:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetUnitAbilityDataTableObject(unit_handle, abil_code, level);
				if (ptr)
				{
					ptr->data[state_type] = *value_ptr;
					return true;
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	bool      __cdecl EXSetUnitAbilityDataI(uint32_t unit_handle, uint32_t abil_code, uint32_t level, uint32_t state_type, uint32_t value)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_COOL:
		case ABILITY_DATA_AREA:
		case ABILITY_DATA_RNG:
		case ABILITY_DATA_DATA_A:
		case ABILITY_DATA_DATA_B:
		case ABILITY_DATA_DATA_C:
		case ABILITY_DATA_DATA_D:
		case ABILITY_DATA_DATA_E:
		case ABILITY_DATA_DATA_F:
		case ABILITY_DATA_DATA_G:
		case ABILITY_DATA_DATA_H:
		case ABILITY_DATA_DATA_I:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetUnitAbilityDataTableObject(unit_handle, abil_code, level);
				if (ptr)
				{
					ptr->data[state_type] = value;
					return true;
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	void InitializeAbilityState()
	{
		util::console::enable();

		//native_function::async_add((uintptr_t)EXGetUnitAbilityObject, "EXGetUnitAbilityObject", "(Hunit;I)I");
		//native_function::async_add((uintptr_t)EXGetUnitAbilityId,     "EXGetUnitAbilityId",     "(Hunit;I)I");
		//native_function::async_add((uintptr_t)EXGetUnitAbilityState,  "EXGetUnitAbilityState",  "(Hunit;II)R");
		//native_function::async_add((uintptr_t)EXSetUnitAbilityState,  "EXSetUnitAbilityState",  "(Hunit;IIR)B");
		//native_function::async_add((uintptr_t)EXGetUnitAbilityDataR,  "EXGetUnitAbilityDataR",  "(Hunit;III)R");
		//native_function::async_add((uintptr_t)EXGetUnitAbilityDataI,  "EXGetUnitAbilityDataI",  "(Hunit;III)I");
		//native_function::async_add((uintptr_t)EXSetUnitAbilityDataR,  "EXSetUnitAbilityDataR",  "(Hunit;IIIR)B");
		//native_function::async_add((uintptr_t)EXSetUnitAbilityDataI,  "EXSetUnitAbilityDataI",  "(Hunit;IIII)B");


		native_function::async_add((uintptr_t)EXGetUnitAbilityObject,  "EXGetUnitAbilityObject",  "(Hunit;I)I");
		native_function::async_add((uintptr_t)EXGetUnitAbilityByIndex, "EXGetUnitAbilityByIndex", "(Hunit;I)Hhandle;");
		native_function::async_add((uintptr_t)EXGetUnitAbilityById,    "EXGetUnitAbilityById",    "(Hunit;I)Hhandle;");
		native_function::async_add((uintptr_t)EXGetAbilityId,          "EXGetAbilityId",          "(Hhandle;)I");
		native_function::async_add((uintptr_t)EXGetAbilityState,       "EXGetAbilityState",       "(Hhandle;I)R");
		native_function::async_add((uintptr_t)EXSetAbilityState,       "EXSetAbilityState",       "(Hhandle;IR)B");
		native_function::async_add((uintptr_t)EXGetAbilityDataR,       "EXGetAbilityDataR",       "(Hhandle;II)R");
		native_function::async_add((uintptr_t)EXGetAbilityDataI,       "EXGetAbilityDataI",       "(Hhandle;II)I");
		native_function::async_add((uintptr_t)EXSetAbilityDataR,       "EXSetAbilityDataR",       "(Hhandle;IIR)B");
		native_function::async_add((uintptr_t)EXSetAbilityDataI,       "EXSetAbilityDataI",       "(Hhandle;III)B");
	}
}}}
