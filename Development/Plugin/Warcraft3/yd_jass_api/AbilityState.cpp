#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/war3_searcher.h>	
#include <base/warcraft3/hashtable.h>
#include <base/hook/inline.h>
#include <array>
#include <string>
#include "StringPool.h"

namespace base { namespace warcraft3 { namespace japi {

	string_pool_t string_pool;

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
		ABILITY_DATA_UNART,	            // string	
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
		uint8_t		               unk_04_[0x34];	    //0x04	   
		char*		               unart;	            //0x38	
		char*                      research_art;        //0x3C
		uint8_t		               unk_3C_[0x3C];	    //0x40
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

	circular_queue<uintptr_t> ability_pool;

	uintptr_t find_abilityid_64(const objectid_64& id)
	{
		uintptr_t ptr = find_objectid_64(id);
		if (*(uintptr_t*)(ptr + 0x20))
		{
			return 0;
		}
		return *(uintptr_t*)(ptr + 0x54);
	}

	struct CAgentTimer
	{
	public:
		jass::jreal_t get_timeout()
		{
			if (is_valid())
			{
				uintptr_t rf = running_info();
				if (rf_is_valid(rf))
				{
					return jass::to_real(jass::from_real(timeout(rf)) - jass::from_real(current_time(rf)));
				}
			}
			return jass::to_real(0.f);
		}

		bool set_timeout(float value)
		{
			if (is_valid())
			{
				if (value < 0.f) value = 0.f;

				uintptr_t rf = running_info();
				if (!rf)
				{
					start();
					rf = running_info();
				}

				if (rf_is_valid(rf))
				{
					jass::jreal_t cooldown = jass::to_real(value);
					this_call<void>(*(uintptr_t*)(*(uintptr_t*)(this) + 0x08), this, *(uintptr_t*)((uintptr_t)this + 0x08), &cooldown, 0xD01BEu, 0, 0);

					return true;
				}
			}
			return false;
		}

	private:
		bool is_valid()
		{
			static uintptr_t s_vft = 0;

			if (!this) return false;

			if (s_vft)
			{
				if (::IsBadReadPtr(reinterpret_cast<const void*>(this), sizeof(uintptr_t)))
				{
					return false;
				}
				return (s_vft == *(uintptr_t*)this);
			}

			const char* name = get_class_name((uintptr_t)this);
			if (name && std::string(name) == ".?AVCAgentTimer@@")
			{
				s_vft = *(uintptr_t*)this;
				return true;
			}
			return false;
		}

		bool rf_is_valid(uintptr_t rf)
		{ 
			return rf && (rf + 0x04) && (rf + 0x0C);
		}

		uintptr_t running_info()
		{
			return *(uintptr_t*)((uintptr_t)(this) + 0x0C);
		}

		uintptr_t current_time(uintptr_t rf) 
		{  
			return *(uintptr_t*)(*(uintptr_t*)(rf + 0x0C) + 0x40);
		}

		uintptr_t& timeout(uintptr_t rf) 
		{  
			return *(uintptr_t*)(rf + 0x04);
		}

		void start()
		{
			uintptr_t ability = (uintptr_t)(this) - 0xD0;
			jass::jreal_t cooldown = jass::to_real(0.f);
			this_call<void>(*(uintptr_t*)(*(uintptr_t*)ability + 0x2EC), ability, &cooldown, *(uintptr_t*)(ability + 0x50));
			if (jass::from_real(cooldown) > 0.f)
			{
				this_call<void>(*(uintptr_t*)(*(uintptr_t*)ability + 0x3A4), ability, &cooldown);
			}
		}
	};

	uint32_t GetAbilityCooldown(uintptr_t ability_ptr)
	{
		if (ability_ptr)
		{
			return reinterpret_cast<CAgentTimer*>(ability_ptr + 0xD0)->get_timeout();
		}

		return jass::to_real(0.f);
	}

	bool     SetAbilityCooldown(uintptr_t ability_ptr, float value)
	{	
		if (ability_ptr)
		{
			return reinterpret_cast<CAgentTimer*>(ability_ptr + 0xD0)->set_timeout(value);
		}

		return false;
	}

	uintptr_t GetUnitAbilityById(uint32_t unit_handle, uint32_t abil_code)
	{
		uintptr_t unit_ptr = handle_to_object(unit_handle);

		if (unit_ptr)
		{
			objectid_64* abil_id = (objectid_64*)(unit_ptr + 0x1D8 + (get_war3_searcher().get_version() > version_124c ? 4: 0));

			for (; abil_id->is_valid();)
			{
				uint32_t ability_ptr = find_abilityid_64(*abil_id);

				if (!ability_ptr)
				{
					return 0;
				}

				if (abil_code == *(uintptr_t*)(ability_ptr + 0x34))
				{
					return ability_ptr;
				}

				abil_id = (objectid_64*)(ability_ptr + 0x24);
			}
		}

		return 0;
	}

	static uintptr_t search_get_ability_ui_table()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.search_string("GetAbilityEffectById");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		ptr = next_opcode(ptr, 0xE8, 5);
		return convert_function(ptr);
	}

	static ability_ui* GetAbilityUITableById(uintptr_t abilcode)
	{
		static uintptr_t get_ability_ui_table = search_get_ability_ui_table();
		return this_call<ability_ui*>(get_ability_ui_table, abilcode);
	}

	uintptr_t GetUnitAbilityByIndex(uint32_t unit_handle, uint32_t index)
	{
		uintptr_t unit_ptr = handle_to_object(unit_handle);

		if (unit_ptr)
		{
			objectid_64* abil_id = (objectid_64*)(unit_ptr + 0x1DC);

			for (; abil_id->is_valid(); --index)
			{
				uint32_t ability_ptr = find_abilityid_64(*abil_id);

				if (!ability_ptr)
				{
					return 0;
				}

				if (index == 0)
				{
					return ability_ptr;
				}

				abil_id = (objectid_64*)(ability_ptr + 0x24);
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

	static jass::jstring_t GetAbilityString(ability_ui* ptr, jass::jinteger_t level, jass::jinteger_t type)
	{
		if (type < ABILITY_DATA_NAME || type > ABILITY_DATA_UNART)
		{
			return jass::create_string("");
		}

		if (!ptr)
		{
			return jass::create_string("");
		}

		char** buf = 0;
		switch (type)
		{
		case ABILITY_DATA_UNART:
			buf = &ptr->unart;
			break;
		default:
			buf = ptr->array_[type - ABILITY_DATA_NAME].get(level);
			break;
		}

		if (!buf || !*buf)
		{
			return jass::create_string("");
		}

		return jass::create_string(*buf);
	}

	static jass::jboolean_t SetAbilityString(ability_ui* ptr, jass::jinteger_t level, jass::jinteger_t type, jass::jstring_t value)
	{
		if (type < ABILITY_DATA_NAME || type > ABILITY_DATA_UNART)
		{
			return jass::jfalse;
		}

		if (!ptr)
		{
			return jass::jfalse;
		}

		char** buf = 0;
		switch (type)
		{
		case ABILITY_DATA_UNART:
			buf = &ptr->unart;
			break;
		default:
			buf = ptr->array_[type - ABILITY_DATA_NAME].get(level);
			break;
		}

		if (!buf || !*buf)
		{
			return jass::jfalse;
		}

		string_pool.free((uintptr_t)*buf);
		const char* value_str = jass::from_string(value);
		if (value_str)
		{
			size_t      value_len = strlen(value_str);
			uintptr_t   value_buf = string_pool.malloc(value_len + 1);
			*buf = (char*)value_buf;
			if (value_buf)
			{
				strncpy_s(*buf, value_len + 1, value_str, value_len);
				//this_call<void>(0x6F021FB0, ability_pool.at(ability_handle));
			}
		}
		else
		{
			*buf = 0;
		}
		return jass::jtrue;
	}

	jass::jstring_t  __cdecl EXGetAbilityDataString(jass::jhandle_t ability_handle, jass::jinteger_t level, jass::jinteger_t type)
	{
		return GetAbilityString(GetAbilityUITable(ability_pool.at(ability_handle)), level, type);
	}

	jass::jboolean_t  __cdecl EXSetAbilityDataString(jass::jhandle_t ability_handle, jass::jinteger_t level, jass::jinteger_t type, jass::jstring_t value)
	{
		return SetAbilityString(GetAbilityUITable(ability_pool.at(ability_handle)), level, type, value);
	}

	bool __cdecl EXSetAbilityAEmeDataA(uint32_t ability_handle, uint32_t value)
	{
		ability_data* ability_ptr = (ability_data*)ability_pool.at(ability_handle);
		if (!ability_ptr)
		{
			return false;
		}
		*(uint32_t*)((uintptr_t)ability_ptr + 0x63 * 4) = value;
		return true;
	}

	enum BUFF_DATA_TYPE
	{
		BUFF_DATA_ART = 1,
		BUFF_DATA_TIP,
		BUFF_DATA_UBERTIP,
	};


	static uintptr_t search_buff_table()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t str = s.search_string_ptr("|cff00ff00", sizeof("|cff00ff00"));
		if (s.get_version() <= version_121b)
		{
			str = s.search_int_in_rdata(str);
		}
		uintptr_t ptr = s.search_int_in_text(str);
		uintptr_t prev = ptr;
		for (; ptr; prev = ptr, ptr = s.search_int_in_text(str, prev + 1))
			;
		ptr = prev;
		ptr = s.current_function(ptr);
		if (s.get_version() > version_121b)
		{
			ptr = next_opcode(ptr, 0xB9, 5);
			return *(uintptr_t*)(ptr + 1);
		}
		else
		{
			ptr = next_opcode(ptr, 0xA1, 5);
			return *(uintptr_t*)(ptr + 1) - 0x24;
		}
	}

	static uintptr_t get_buff_table()
	{
		static uintptr_t table = search_buff_table();
		return table;
	}

	uint32_t  __cdecl EXGetBuffDataString(uint32_t code, uint32_t type)
	{
		typedef hashtable::table<> buff_table_t;
		buff_table_t* table = (buff_table_t*)get_buff_table();
		if (!table) {
			return jass::create_string("");
		}
		hashtable::node* ptr = table->get(code);
		if (!ptr) {
			return jass::create_string("");
		}

		char* buf = 0;
		switch (type) {
		case BUFF_DATA_ART:
			buf = (char*)ptr + 0x18;
			break;
		case BUFF_DATA_TIP:
			buf = (char*)ptr + 0x11c;
			break;
		case BUFF_DATA_UBERTIP:
			buf = (char*)ptr + 0x15c;
			break;
		}
		if (!buf) {
			return jass::create_string("");
		}
		return jass::create_string(buf);
	}

	bool  __cdecl EXSetBuffDataString(uint32_t code, uint32_t type, uint32_t value)
	{
		typedef hashtable::table<> buff_table_t;
		buff_table_t* table = (buff_table_t*)get_buff_table();
		if (!table) {
			return false;
		}
		hashtable::node* ptr = table->get(code);
		if (!ptr) {
			return false;
		}

		char* buf = 0;
		size_t len = 0;
		switch (type) {
		case BUFF_DATA_ART:
			buf = (char*)ptr + 0x18;
			len = 0x100;
			break;
		case BUFF_DATA_TIP:
			buf = (char*)ptr + 0x11c;
			len = 0x40;
			break;
		case BUFF_DATA_UBERTIP:
			buf = (char*)ptr + 0x15c;
			len = 0x100;
			break;
		}
		if (!buf || !len) {
			return false;
		}
		const char* value_str = jass::from_string(value);
		size_t      value_len = strlen(value_str);
		if (value_len > len - 1) {
			value_len = len - 1;
		}
		strncpy_s(buf, value_len + 1, value_str, value_len);
		return true;
	}

	jass::jstring_t  __cdecl EXGetAbilityString(jass::jinteger_t abilcode, jass::jinteger_t level, jass::jinteger_t type)
	{
		return GetAbilityString(GetAbilityUITableById(abilcode), level, type);
	}
	
	jass::jboolean_t  __cdecl EXSetAbilityString(jass::jinteger_t abilcode, jass::jinteger_t level, jass::jinteger_t type, jass::jstring_t value)
	{
		return SetAbilityString(GetAbilityUITableById(abilcode), level, type, value);
	}

	void InitializeAbilityState()
	{
		jass::japi_add((uintptr_t)EXGetUnitAbilityById,    "EXGetUnitAbility",        "(Hunit;I)Hability;");
		jass::japi_add((uintptr_t)EXGetUnitAbilityByIndex, "EXGetUnitAbilityByIndex", "(Hunit;I)Hability;");
		jass::japi_add((uintptr_t)EXGetAbilityId,          "EXGetAbilityId",          "(Hability;)I");
		jass::japi_add((uintptr_t)EXGetAbilityState,       "EXGetAbilityState",       "(Hability;I)R");
		jass::japi_add((uintptr_t)EXSetAbilityState,       "EXSetAbilityState",       "(Hability;IR)B");
		jass::japi_add((uintptr_t)EXGetAbilityDataReal,    "EXGetAbilityDataReal",    "(Hability;II)R");
		jass::japi_add((uintptr_t)EXSetAbilityDataReal,    "EXSetAbilityDataReal",    "(Hability;IIR)B");
		jass::japi_add((uintptr_t)EXGetAbilityDataInteger, "EXGetAbilityDataInteger", "(Hability;II)I");
		jass::japi_add((uintptr_t)EXSetAbilityDataInteger, "EXSetAbilityDataInteger", "(Hability;III)B");
		jass::japi_add((uintptr_t)EXGetAbilityDataString,  "EXGetAbilityDataString",  "(Hability;II)S");
		jass::japi_add((uintptr_t)EXSetAbilityDataString,  "EXSetAbilityDataString",  "(Hability;IIS)B");
		jass::japi_add((uintptr_t)EXSetAbilityAEmeDataA,   "EXSetAbilityAEmeDataA",   "(Hability;I)B");
		jass::japi_add((uintptr_t)EXGetBuffDataString,     "EXGetBuffDataString",     "(II)S");
		jass::japi_add((uintptr_t)EXSetBuffDataString,     "EXSetBuffDataString",     "(IIS)B"); 
		jass::japi_add((uintptr_t)EXGetAbilityString,      "EXGetAbilityString",      "(III)S");
		jass::japi_add((uintptr_t)EXSetAbilityString,      "EXSetAbilityString",      "(IIIS)B");	
	}
}}}
