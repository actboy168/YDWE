#include <aero/function/fp_call.hpp>
#include <ydwe/hook/iat.h>
#include <ydwe/warcraft3/native_function.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/detail/memory_search.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/hook/inline.h>
#include <array>

_BASE_BEGIN
namespace warcraft3 { namespace japi {

	uint32_t  __cdecl EXGetUnitObject(uint32_t unit_handle);

	template <class T, size_t kSize = 256>
	class circular_queue : public std::array<T, kSize>
	{
		typedef std::array<T, kSize> mybase;
	public:
		circular_queue()
			: std::array<T, kSize>()
			, cur_(0)
		{ }

		size_t push(const T& val)
		{
			size_t retval = cur_;
			mybase::operator [](cur_) = val;
			cur_++;
			if (cur_ == mybase::size())
			{
				cur_ = 0;
			}

			return 0x100 + retval;
		}

		T at(size_t n) const
		{
			if (n >= 0x100 && n < 0x100 + mybase::size())
			{
				return mybase::operator [](n - 0x100);
			}

			return T();
		}

	private:
		size_t cur_;
	};

	enum ABILITY_STATE_TYPE
	{
		ABILITY_STATE_COOLDOWN  = 0x01,
	};

	enum ABILITY_DATA_TYPE
	{
		ABILITY_DATA_TARGS = 100, // integer
		ABILITY_DATA_CAST,        // real
		ABILITY_DATA_DUR,         // real
		ABILITY_DATA_HERODUR,     // real
		ABILITY_DATA_COST,        // integer
		ABILITY_DATA_COOL,        // real
		ABILITY_DATA_AREA,        // real
		ABILITY_DATA_RNG,         // real
		ABILITY_DATA_DATA_A,      // real
		ABILITY_DATA_DATA_B,      // real
		ABILITY_DATA_DATA_C,      // real
		ABILITY_DATA_DATA_D,      // real
		ABILITY_DATA_DATA_E,      // real
		ABILITY_DATA_DATA_F,      // real
		ABILITY_DATA_DATA_G,      // real
		ABILITY_DATA_DATA_H,      // real
		ABILITY_DATA_DATA_I,      // real
		ABILITY_DATA_UNITID,      // integer
		ABILITY_DATA_18,
		ABILITY_DATA_19,
		ABILITY_DATA_BUFFID_PTR,
		ABILITY_DATA_21,
		ABILITY_DATA_22,
		ABILITY_DATA_23,
		ABILITY_DATA_EFCTID_PTR,
		ABILITY_DATA_25,

		ABILITY_DATA_HOTKET = 200,      // integer
		ABILITY_DATA_UNHOTKET,          // integer
		ABILITY_DATA_RESEARCH_HOTKEY,   // integer
		ABILITY_DATA_NAME,             	// string
		ABILITY_DATA_ART,				// string
		ABILITY_DATA_TARGET_ART,		// string
		ABILITY_DATA_CASTER_ART,		// string
		ABILITY_DATA_EFFECT_ART,		// string
		ABILITY_DATA_AREAEFFECT_ART,	// string
		ABILITY_DATA_MISSILE_ART,		// string
		ABILITY_DATA_SPECIAL_ART,		// string
		ABILITY_DATA_LIGHTNING_EFFECT,	// string
		ABILITY_DATA_BUFF_TIP,			// string
		ABILITY_DATA_BUFF_UBERTIP,		// string
		ABILITY_DATA_RESEARCH_TIP,		// string
		ABILITY_DATA_TIP,				// string
		ABILITY_DATA_UNTIP,				// string
		ABILITY_DATA_RESEARCH_UBERTIP,  // string
		ABILITY_DATA_UBERTIP,			// string
		ABILITY_DATA_UNUBERTIP,			// string
	};

	struct mapping_ability
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

	template <class T>
	struct ability_ui_elem
	{
		uint32_t	unk_;
		uint32_t 	max_;
		T*      	items_;

		T* get(uint32_t level) const
		{
			if (level == 0 || level > max_)
			{
				return nullptr;
			}

			return &(items_[level-1]);
		}
	};

	struct ability_ui
	{
		uintptr_t		           vf_table_;			//0x00
		uint8_t		               unk_04_[0x78];	    //0x04
		ability_ui_elem<uint32_t>  hotkey_[3];          //0x7C
		ability_ui_elem<char*>     array_[17];          //0xA0
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

		uint32_t    unk08;
		uint32_t    unk09;
		ability_ui* ui_;
		uint32_t    available_;

		uint32_t base_id_;
		uint32_t object_id_;
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

	circular_queue<uintptr_t> ability_pool;

	mapping_ability* search_mapping_ability()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = 0;
		if (s.get_version() > version_121b)
		{
			ptr = s.search_string("e:\\Drive1\\temp\\buildwar3x\\Engine\\Source\\Tempest/tempest_thread.h");
		}
		else
		{
			ptr = s.search_string("..\\Tempest/tempest_thread.h");
		}

		ptr += 0x04;
		ptr = detail::next_opcode(ptr, 0xE8, 5);
		ptr += 0x05;
		ptr = detail::next_opcode(ptr, 0xE8, 5);
		ptr = detail::convert_function(ptr);
		ptr = detail::next_opcode(ptr, 0x89, 6);
		ptr = *(uintptr_t*)(ptr + 0x02);
		return (mapping_ability*)*(uintptr_t*)(ptr);		
	}

	uint32_t GetAbilityObjectById64(object_id_64 const& id)
	{
		static mapping_ability* table_ptr = search_mapping_ability();

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

	uintptr_t GetUnitAbilityById(uint32_t unit_handle, uint32_t abil_code)
	{
		uintptr_t unit_ptr = EXGetUnitObject(unit_handle);

		if (unit_ptr)
		{
			object_id_64* abil_id = (object_id_64*)(unit_ptr + 0x1D8 + (get_war3_searcher().get_version() > version_124c ? 4: 0));

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

	uintptr_t GetUnitAbilityByIndex(uint32_t unit_handle, uint32_t index)
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
					return ability_ptr;
				}

				abil_id = (object_id_64*)(ability_ptr + 0x24);
			}
		}

		return 0;
	}

	ability_data_table* GetAbilityDataTable(uint32_t ability_ptr, uint32_t level)
	{
		if (ability_ptr)
		{
			ability_data* ptr = *(ability_data**)(ability_ptr + 0x54);

			if (ptr && ptr->available_ && ptr->tabel_ && (ptr->max_level_ >= level) && (level > 0))
			{
				return &(ptr->tabel_[level-1]);
			}
		}

		return nullptr;
	}

	ability_ui* GetAbilityUITable(uint32_t ability_ptr)
	{
		if (ability_ptr)
		{
			ability_data* ptr = *(ability_data**)(ability_ptr + 0x54);

			if (ptr)
			{
				return ptr->ui_;
			}
		}

		return nullptr;
	}

	uint32_t  __cdecl EXGetUnitAbilityById(uint32_t unit_handle, uint32_t abil_code)
	{
		uintptr_t ability_ptr = GetUnitAbilityById(unit_handle, abil_code);

		if (ability_ptr)
		{
			return ability_pool.push(ability_ptr);
		}
		return 0;
	}

	uint32_t  __cdecl EXGetUnitAbilityByIndex(uint32_t unit_handle, uint32_t index)
	{
		uintptr_t ability_ptr = GetUnitAbilityByIndex(unit_handle, index);
		if (ability_ptr)
		{
			return ability_pool.push(ability_ptr);
		}
		return 0;
	}

	uint32_t  __cdecl EXGetAbilityId(uint32_t ability_handle)
	{
		uintptr_t ability_ptr = ability_pool.at(ability_handle);
		if (ability_ptr)
		{
			return *(uintptr_t*)(ability_ptr + 0x34);
		}
		return 0;
	}

	uint32_t  __cdecl EXGetAbilityState(uint32_t ability_handle, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_STATE_COOLDOWN:
			return GetAbilityCooldown(ability_pool.at(ability_handle));
		default:
			break;
		}

		return jass::to_real(0.f);
	}

	bool      __cdecl EXSetAbilityState(uint32_t ability_handle, uint32_t state_type, uint32_t* value_ptr)
	{
		switch (state_type)
		{
		case ABILITY_STATE_COOLDOWN:
			return SetAbilityCooldown(ability_pool.at(ability_handle), jass::from_real(*value_ptr));
		default:
			break;
		}

		return false;
	}
	
	uint32_t  __cdecl EXGetAbilityDataReal(uint32_t ability_handle, uint32_t level, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
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
			{
				ability_data_table* ptr = GetAbilityDataTable(ability_pool.at(ability_handle), level);
				if (ptr)
				{
					return ptr->data[state_type-ABILITY_DATA_TARGS];
				}
			}
			break;
		default:
			break;
		}

		return jass::to_real(0.f);
	}

	bool      __cdecl EXSetAbilityDataReal(uint32_t ability_handle, uint32_t level, uint32_t state_type, uint32_t* value_ptr)
	{
		switch (state_type)
		{
		case ABILITY_DATA_CAST:
		case ABILITY_DATA_DUR:
		case ABILITY_DATA_HERODUR:
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
			{
				ability_data_table* ptr = GetAbilityDataTable(ability_pool.at(ability_handle), level);
				if (ptr)
				{
					ptr->data[state_type-ABILITY_DATA_TARGS] = *value_ptr;
					return true;
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	uint32_t  __cdecl EXGetAbilityDataInteger(uint32_t ability_handle, uint32_t level, uint32_t state_type)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetAbilityDataTable(ability_pool.at(ability_handle), level);
				if (ptr)
				{
					return ptr->data[state_type-ABILITY_DATA_TARGS];
				}
			}
			break;
		case ABILITY_DATA_HOTKET:
		case ABILITY_DATA_UNHOTKET:
		case ABILITY_DATA_RESEARCH_HOTKEY:
			{
				ability_ui* ptr = GetAbilityUITable(ability_pool.at(ability_handle));
				if (ptr)
				{
					uint32_t* buf = ptr->hotkey_[state_type-ABILITY_DATA_HOTKET].get(level);
					if (buf) 
					{
						return *buf;
					}
				}
			}
			break;
		default:
			break;
		}

		return 0;
	}

	bool      __cdecl EXSetAbilityDataInteger(uint32_t ability_handle, uint32_t level, uint32_t state_type, uint32_t value)
	{
		switch (state_type)
		{
		case ABILITY_DATA_TARGS:
		case ABILITY_DATA_COST:
		case ABILITY_DATA_UNITID:
			{
				ability_data_table* ptr = GetAbilityDataTable(ability_pool.at(ability_handle), level);
				if (ptr)
				{
					ptr->data[state_type-ABILITY_DATA_TARGS] = value;
					return true;
				}
			}
			break;
		case ABILITY_DATA_HOTKET:
		case ABILITY_DATA_UNHOTKET:
		case ABILITY_DATA_RESEARCH_HOTKEY:
			{
				ability_ui* ptr = GetAbilityUITable(ability_pool.at(ability_handle));
				if (ptr)
				{
					uint32_t* buf = ptr->hotkey_[state_type-ABILITY_DATA_HOTKET].get(level);
					if (buf) 
					{
						*buf = value;
						return true;
					}
				}
			}
			break;
		default:
			break;
		}

		return false;
	}

	uint32_t  __cdecl EXGetAbilityDataString(uint32_t ability_handle, uint32_t level, uint32_t state_type)
	{
		if (state_type < ABILITY_DATA_NAME || state_type > ABILITY_DATA_UNUBERTIP)
		{
			return jass::create_string("");
		}

		ability_ui* ptr = GetAbilityUITable(ability_pool.at(ability_handle));

		if (!ptr)
		{
			return jass::create_string("");
		}


		char** buf = ptr->array_[state_type-ABILITY_DATA_NAME].get(level);
		if (!buf || !*buf) 
		{
			return jass::create_string("");
		}

		return jass::create_string(*buf);
	}

	bool      __cdecl EXSetAbilityDataString(uint32_t ability_handle, uint32_t level, uint32_t state_type, uint32_t value)
	{
		if (state_type < ABILITY_DATA_NAME || state_type > ABILITY_DATA_UNUBERTIP)
		{
			return false;
		}

		ability_ui* ptr = GetAbilityUITable(ability_pool.at(ability_handle));

		if (!ptr)
		{
			return false;
		}

		char** buf = ptr->array_[state_type-ABILITY_DATA_NAME].get(level);
		if (!buf || !*buf) 
		{
			return false;
		}
		strncpy(*buf, jass::from_string(value), 1024);
		//aero::this_call<void>(0x6F021FB0, ability_pool.at(ability_handle));
		return true;
	}

	void InitializeAbilityState()
	{
		native_function::japi_add((uintptr_t)EXGetUnitAbilityById,    "EXGetUnitAbility",        "(Hunit;I)Hability;");
		native_function::japi_add((uintptr_t)EXGetUnitAbilityByIndex, "EXGetUnitAbilityByIndex", "(Hunit;I)Hability;");
		native_function::japi_add((uintptr_t)EXGetAbilityId,          "EXGetAbilityId",          "(Hability;)I");
		native_function::japi_add((uintptr_t)EXGetAbilityState,       "EXGetAbilityState",       "(Hability;I)R");
		native_function::japi_add((uintptr_t)EXSetAbilityState,       "EXSetAbilityState",       "(Hability;IR)B");
		native_function::japi_add((uintptr_t)EXGetAbilityDataReal,    "EXGetAbilityDataReal",    "(Hability;II)R");
		native_function::japi_add((uintptr_t)EXSetAbilityDataReal,    "EXSetAbilityDataReal",    "(Hability;IIR)B");
		native_function::japi_add((uintptr_t)EXGetAbilityDataInteger, "EXGetAbilityDataInteger", "(Hability;II)I");
		native_function::japi_add((uintptr_t)EXSetAbilityDataInteger, "EXSetAbilityDataInteger", "(Hability;III)B");
		native_function::japi_add((uintptr_t)EXGetAbilityDataString,  "EXGetAbilityDataString",  "(Hability;II)S");
		native_function::japi_add((uintptr_t)EXSetAbilityDataString,  "EXSetAbilityDataString",  "(Hability;IIS)B");
	}
}}
_BASE_END
