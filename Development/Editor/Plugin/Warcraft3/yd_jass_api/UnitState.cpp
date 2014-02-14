#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace japi {
	enum UNIT_STATE
	{
	    // original
	    UNIT_STATE_ATTACK1_DAMAGE_DICE          = 0x10,
	    UNIT_STATE_ATTACK1_DAMAGE_SIDE          = 0x11,
	    UNIT_STATE_ATTACK1_DAMAGE_BASE          = 0x12,
	    UNIT_STATE_ATTACK1_DAMAGE_BONUS         = 0x13,
	    UNIT_STATE_ATTACK1_DAMAGE_MIN           = 0x14,
	    UNIT_STATE_ATTACK1_DAMAGE_MAX           = 0x15,
	    UNIT_STATE_ATTACK1_RANGE                = 0x16,
	    UNIT_STATE_ARMOR                        = 0x20,
	    // attack 1 attribute adds
	    UNIT_STATE_ATTACK1_DAMAGE_LOSS_FACTOR   = 0x21,
	    UNIT_STATE_ATTACK1_WEAPON_SOUND         = 0x22,
	    UNIT_STATE_ATTACK1_ATTACK_TYPE          = 0x23,
	    UNIT_STATE_ATTACK1_MAX_TARGETS          = 0x24,
	    UNIT_STATE_ATTACK1_INTERVAL             = 0x25,
	    UNIT_STATE_ATTACK1_INITIAL_DELAY        = 0x26,
	    UNIT_STATE_ATTACK1_BACK_SWING           = 0x28,
	    UNIT_STATE_ATTACK1_RANGE_BUFFER         = 0x27,
	    UNIT_STATE_ATTACK1_TARGET_TYPES         = 0x29,
	    UNIT_STATE_ATTACK1_SPILL_DIST           = 0x56,
	    UNIT_STATE_ATTACK1_SPILL_RADIUS         = 0x57,
		UNIT_STATE_ATTACK1_WEAPON_TYPE			= 0x58,
	    // attack 2 attributes (sorted in a sequencial order based on memory address)
	    UNIT_STATE_ATTACK2_DAMAGE_DICE          = 0x30,
	    UNIT_STATE_ATTACK2_DAMAGE_SIDE          = 0x31,
	    UNIT_STATE_ATTACK2_DAMAGE_BASE          = 0x32,
	    UNIT_STATE_ATTACK2_DAMAGE_BONUS         = 0x33,
	    UNIT_STATE_ATTACK2_DAMAGE_LOSS_FACTOR   = 0x34,
	    UNIT_STATE_ATTACK2_WEAPON_SOUND         = 0x35,
	    UNIT_STATE_ATTACK2_ATTACK_TYPE          = 0x36,
	    UNIT_STATE_ATTACK2_MAX_TARGETS          = 0x37,
	    UNIT_STATE_ATTACK2_INTERVAL             = 0x38,
	    UNIT_STATE_ATTACK2_INITIAL_DELAY        = 0x39,
	    UNIT_STATE_ATTACK2_RANGE                = 0x40,
	    UNIT_STATE_ATTACK2_RANGE_BUFFER         = 0x41,
	    UNIT_STATE_ATTACK2_DAMAGE_MIN           = 0x42,
	    UNIT_STATE_ATTACK2_DAMAGE_MAX           = 0x43,
	    UNIT_STATE_ATTACK2_BACK_SWING           = 0x44,
	    UNIT_STATE_ATTACK2_TARGET_TYPES         = 0x45,
	    UNIT_STATE_ATTACK2_SPILL_DIST           = 0x46,
	    UNIT_STATE_ATTACK2_SPILL_RADIUS         = 0x47,
		UNIT_STATE_ATTACK2_WEAPON_TYPE			= 0x59,
	    // general attributes
	    UNIT_STATE_ARMOR_TYPE                   = 0x50,
	    UNIT_STATE_RATE_OF_FIRE                 = 0x51, // global attack rate of unit, work on both attacks
	    UNIT_STATE_ACQUISITION_RANGE            = 0x52, // how far the unit will automatically look for targets
	    UNIT_STATE_LIFE_REGEN                   = 0x53,
	    UNIT_STATE_MANA_REGEN                   = 0x54,
	    UNIT_STATE_MAX_LIFE                     = 0x1,
	    UNIT_STATE_MAX_MANA                     = 0x3,
	    UNIT_STATE_MIN_RANGE                    = 0x55,
	    // ...starts from 0x5A
	};

	struct unit_property {
	    uint32_t unk_00[34];
	    int32_t  attack1_damage_dice;           // 0x88 (integer)
	    int32_t  attack2_damage_dice;           // +4
	    uint32_t unk_8c[1];
	    int32_t  attack1_damage_side;           // 0x94 (integer)
	    int32_t  attack2_damage_side;           // +4
	    uint32_t unk_9c[1];
	    int32_t  attack1_damage_base;           // 0xA0 (integer)
	    int32_t  attack2_damage_base;           // +4
	    uint32_t unk_a8[1];
	    int32_t  attack1_damage_bonus;          // 0xAC (integer)
	    int32_t  attack2_damage_bonus;          // +4
	    uint32_t unk_b4[2];
	    uint32_t attack1_damage_loss_factor;    // 0xBC (real)
	    uint32_t unk_c0[3];
	    uint32_t attack2_damage_loss_factor;    // +16(0xF)
	    uint32_t unk_d0[3];
	    int32_t  attack1_weapTp;                // 0xDC (integer)
	    int32_t  attack2_weapTp;                // +4
	    uint32_t unk_e4;
	    int32_t  attack1_weapon_sound;          // 0xE8 (real)
	    int32_t  attack2_weapon_sound;          // +4
	    uint32_t unk_f0[1];
	    int32_t  attack1_atkType;               // 0xF4 (integer)
	    int32_t  attack2_atkType;               // +4
	    uint32_t unk_fc[1];
	    int32_t  attack1_target_amount;         // 0x100 (integer)
	    int32_t  attack2_target_amount;         // +4
	    uint32_t unk_108[2];
	    uint32_t attack1_spill_dist;            // 0x110 (real)
	    uint32_t unk_114[3];
	    uint32_t attack2_spill_dist;            // +16(0xF)
	    uint32_t unk_124[4];
	    uint32_t attack1_spill_radius;          // 0x134
	    uint32_t unk_138[3];
	    uint32_t attack2_spill_radius;          // +16(0xF)
	    uint32_t unk_148[4];
	    uint32_t attack1_interval;              // 0x158 (real)
	    uint32_t unk_15c[1];
	    uint32_t attack2_interval;              // +8
	    uint32_t unk_164[2];
	    uint32_t attack1_dmgpt;                 // 0x16C (real)
	    uint32_t unk_170[3];
	    uint32_t attack2_dmgpt;                 // +16(0xF)
	    uint32_t unk_180[4];
	    uint32_t attack1_back_swing;            // 0x190 (real)
	    uint32_t unk_194[3];
	    uint32_t attack2_back_swing;            // +16(0xF)
	    uint32_t unk_1a4[3];
	    uint32_t rate_of_fire;                  // 0x1B0 (real)
	    uint32_t unk_1b4[25];
	    int32_t  attack1_targs;                 // 0x218
	    int32_t  attack2_targs;                 // +4
	    uint32_t unk_220[9];
	    uint32_t acquisition_range;             // 0x244 (real)
	    uint32_t unk_248[1];
	    uint32_t minimum_range;                 // 0x24c (real)
	    uint32_t unk_250[2];
	    uint32_t attack1_range;                 // 0x258 (real)
	    uint32_t unk_25c[1];
	    uint32_t attack2_range;                 // +8
	    uint32_t unk_264[2];
	    uint32_t attack1_range_buffer;          // 0x26C (real)
	    uint32_t unk_270[1];
	    uint32_t attack2_range_buffer;          // +8
	};

	uintptr_t getGameAddress()
	{
		war3_searcher&s = get_war3_searcher();
		uint32_t version = s.get_version();
		uintptr_t address = s.base();
		switch (version)
		{
		case version_120:
		case version_120b:
		case version_120c:
		case version_120d:
		case version_120e:
			address += 0x87C744;
			break;
		case version_121:
		case version_121b:
			address += 0x87D7BC;
			break;
		case version_122:
			address += 0xAA4178;
			break;
		case version_123:
			address += 0xABCFC8;
			break;
		case version_124:
		case version_124b:
		case version_124c:
		case version_124d:
		case version_124e:
			address += 0xACE5E0;
			break;
		case version_125:
		case version_125b:
		case version_126:
			address += 0xAB7788;
			break;
		default:
			address = 0;
		}
		return address;
	}

	uintptr_t getNodeAddress(int32_t nodeIndex, uint32_t offest)
	{
		uintptr_t gameAddress = getGameAddress();
		if (gameAddress == 0)
			return 0;
		uintptr_t gameInstanceAddress = *(uint32_t*)(gameAddress);
		uintptr_t gameMemoryAddress = *(uint32_t*)(gameInstanceAddress + 0xC);
		if (gameMemoryAddress == 0xFFFFFFFF || gameMemoryAddress == 0)
			return 0;
		uintptr_t nodeBaseAddress = *(uint32_t*)(gameMemoryAddress + nodeIndex * 8 + 4);
		return nodeBaseAddress + offest;
	}

	uintptr_t RealGetUnitState = 0;
	uint32_t _cdecl FakeGetUnitState(uint32_t unit_handle, uint32_t state_type)
	{
		switch (state_type)
		{
		case UNIT_STATE_ATTACK1_DAMAGE_DICE:
		case UNIT_STATE_ATTACK1_DAMAGE_SIDE:
		case UNIT_STATE_ATTACK1_DAMAGE_BASE:
		case UNIT_STATE_ATTACK1_DAMAGE_BONUS:
		case UNIT_STATE_ATTACK1_DAMAGE_LOSS_FACTOR:
		case UNIT_STATE_ATTACK1_WEAPON_SOUND:
		case UNIT_STATE_ATTACK1_ATTACK_TYPE:
		case UNIT_STATE_ATTACK1_MAX_TARGETS:
		case UNIT_STATE_ATTACK1_INTERVAL:
		case UNIT_STATE_ATTACK1_INITIAL_DELAY:
		case UNIT_STATE_ATTACK1_RANGE:
		case UNIT_STATE_ATTACK1_RANGE_BUFFER:
		case UNIT_STATE_ATTACK1_DAMAGE_MIN:
		case UNIT_STATE_ATTACK1_DAMAGE_MAX:
		case UNIT_STATE_ATTACK1_BACK_SWING:
		case UNIT_STATE_ATTACK1_TARGET_TYPES:
		case UNIT_STATE_ATTACK1_SPILL_DIST:
		case UNIT_STATE_ATTACK1_SPILL_RADIUS:
		case UNIT_STATE_ATTACK1_WEAPON_TYPE:

		case UNIT_STATE_ATTACK2_DAMAGE_DICE:
		case UNIT_STATE_ATTACK2_DAMAGE_SIDE:
		case UNIT_STATE_ATTACK2_DAMAGE_BASE:
		case UNIT_STATE_ATTACK2_DAMAGE_BONUS:
		case UNIT_STATE_ATTACK2_DAMAGE_LOSS_FACTOR:
		case UNIT_STATE_ATTACK2_WEAPON_SOUND:
		case UNIT_STATE_ATTACK2_ATTACK_TYPE:
		case UNIT_STATE_ATTACK2_MAX_TARGETS:
		case UNIT_STATE_ATTACK2_INTERVAL:
		case UNIT_STATE_ATTACK2_INITIAL_DELAY:
		case UNIT_STATE_ATTACK2_RANGE:
		case UNIT_STATE_ATTACK2_RANGE_BUFFER:
		case UNIT_STATE_ATTACK2_DAMAGE_MIN:
		case UNIT_STATE_ATTACK2_DAMAGE_MAX:
		case UNIT_STATE_ATTACK2_BACK_SWING:
		case UNIT_STATE_ATTACK2_TARGET_TYPES:
		case UNIT_STATE_ATTACK2_SPILL_DIST:
		case UNIT_STATE_ATTACK2_SPILL_RADIUS:
		case UNIT_STATE_ATTACK2_WEAPON_TYPE:

		case UNIT_STATE_ARMOR:
		case UNIT_STATE_ARMOR_TYPE:
		case UNIT_STATE_RATE_OF_FIRE:
		case UNIT_STATE_ACQUISITION_RANGE:
		case UNIT_STATE_LIFE_REGEN:
		case UNIT_STATE_MANA_REGEN:
		case UNIT_STATE_MIN_RANGE:
			break;
		default:
			return base::c_call<uint32_t>(RealGetUnitState, unit_handle, state_type);
		}

		war3_searcher&s = get_war3_searcher();
		uintptr_t unit_object = s.unit_handle_to_object(unit_handle);
		if (!unit_object)
		{
			return 0;
		}

		if (state_type == UNIT_STATE_LIFE_REGEN)
		{
			return *(uint32_t*)(unit_object + 0xB0);
		}

		if (state_type == UNIT_STATE_MANA_REGEN)
		{
			return *(uint32_t*)(unit_object + 0xD4);
		}

		if (state_type == UNIT_STATE_ARMOR)
		{
			return *(uint32_t*)(unit_object + 0xE0);
		}

		if (state_type == UNIT_STATE_ARMOR_TYPE)
		{
		    int32_t retval = *(uint32_t*)(unit_object + 0xE4);
		    return jass::to_real((float)retval);
		}

		unit_property* ptr = (unit_property*)*(uintptr_t*)(unit_object + 0x1E4 + (s.get_version() > version_124c ? 4: 0));
		if (!ptr)
		{
			return 0;
		}

		int32_t retval = 0;
		switch (state_type)
		{
		// General Attributes
		case UNIT_STATE_RATE_OF_FIRE:
			return ptr->rate_of_fire;
		case UNIT_STATE_ACQUISITION_RANGE:
			return ptr->acquisition_range;
		case UNIT_STATE_MIN_RANGE:
			return ptr->minimum_range;
		// Attack - 1
		case UNIT_STATE_ATTACK1_DAMAGE_DICE:
			retval = ptr->attack1_damage_dice;
			break;
		case UNIT_STATE_ATTACK1_DAMAGE_SIDE:
			retval = ptr->attack1_damage_side;
			break;
		case UNIT_STATE_ATTACK1_DAMAGE_BASE:
			retval = ptr->attack1_damage_base;
			break;
		case UNIT_STATE_ATTACK1_DAMAGE_BONUS:
			retval = ptr->attack1_damage_bonus;
			break;
		case UNIT_STATE_ATTACK1_DAMAGE_LOSS_FACTOR:
			return ptr->attack1_damage_loss_factor;
		case UNIT_STATE_ATTACK1_WEAPON_SOUND:
			retval = ptr->attack1_weapon_sound;
			break;
		case UNIT_STATE_ATTACK1_ATTACK_TYPE:
			retval = ptr->attack1_atkType;
			break;
		case UNIT_STATE_ATTACK1_MAX_TARGETS:
			retval = ptr->attack1_target_amount;
			break;
		case UNIT_STATE_ATTACK1_INTERVAL:
			return ptr->attack1_interval;
		case UNIT_STATE_ATTACK1_INITIAL_DELAY:
			return ptr->attack1_dmgpt;
		case UNIT_STATE_ATTACK1_RANGE:
			return ptr->attack1_range;
		case UNIT_STATE_ATTACK1_RANGE_BUFFER:
			return ptr->attack1_range_buffer;
		case UNIT_STATE_ATTACK1_DAMAGE_MIN:
			retval = ptr->attack1_damage_base + ptr->attack1_damage_bonus + ptr->attack1_damage_dice;
			break;
		case UNIT_STATE_ATTACK1_DAMAGE_MAX:
			retval = ptr->attack1_damage_base + ptr->attack1_damage_bonus + ptr->attack1_damage_dice * ptr->attack1_damage_side;
			break;
		case UNIT_STATE_ATTACK1_BACK_SWING:
			return ptr->attack1_back_swing;
		case UNIT_STATE_ATTACK1_TARGET_TYPES:
			retval = ptr->attack1_targs;
		case UNIT_STATE_ATTACK1_SPILL_DIST:
			return ptr->attack1_spill_dist;
		case UNIT_STATE_ATTACK1_SPILL_RADIUS:
			return ptr->attack1_spill_radius;
		case UNIT_STATE_ATTACK1_WEAPON_TYPE:
			retval = ptr->attack1_weapTp;
			break;
		// Attack - 2
		case UNIT_STATE_ATTACK2_DAMAGE_DICE:
			retval = ptr->attack2_damage_dice;
			break;
		case UNIT_STATE_ATTACK2_DAMAGE_SIDE:
			retval = ptr->attack2_damage_side;
			break;
		case UNIT_STATE_ATTACK2_DAMAGE_BASE:
			retval = ptr->attack2_damage_base;
			break;
		case UNIT_STATE_ATTACK2_DAMAGE_BONUS:
			retval = ptr->attack2_damage_bonus;
			break;
		case UNIT_STATE_ATTACK2_DAMAGE_LOSS_FACTOR:
			return ptr->attack2_damage_loss_factor;
		case UNIT_STATE_ATTACK2_WEAPON_SOUND:
			retval = ptr->attack2_weapon_sound;
			break;
		case UNIT_STATE_ATTACK2_ATTACK_TYPE:
			retval = ptr->attack2_atkType;
			break;
		case UNIT_STATE_ATTACK2_MAX_TARGETS:
			retval = ptr->attack2_target_amount;
			break;
		case UNIT_STATE_ATTACK2_INTERVAL:
			return ptr->attack2_interval;
		case UNIT_STATE_ATTACK2_INITIAL_DELAY:
			return ptr->attack2_dmgpt;
		case UNIT_STATE_ATTACK2_RANGE:
			return ptr->attack2_range;
		case UNIT_STATE_ATTACK2_RANGE_BUFFER:
			return ptr->attack2_range_buffer;
		case UNIT_STATE_ATTACK2_DAMAGE_MIN:
			retval = ptr->attack2_damage_base + ptr->attack2_damage_bonus + ptr->attack2_damage_dice;
			break;
		case UNIT_STATE_ATTACK2_DAMAGE_MAX:
			retval = ptr->attack2_damage_base + ptr->attack2_damage_bonus + ptr->attack2_damage_dice * ptr->attack2_damage_side;
			break;
		case UNIT_STATE_ATTACK2_BACK_SWING:
			return ptr->attack2_back_swing;
		case UNIT_STATE_ATTACK2_TARGET_TYPES:
			retval = ptr->attack2_targs;
			break;
		case UNIT_STATE_ATTACK2_SPILL_DIST:
			return ptr->attack2_spill_dist;
		case UNIT_STATE_ATTACK2_SPILL_RADIUS:
			return ptr->attack2_spill_radius;
		case UNIT_STATE_ATTACK2_WEAPON_TYPE:
			retval = ptr->attack2_weapTp;
			break;
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
		case UNIT_STATE_ATTACK1_ATTACK_TYPE:
		case UNIT_STATE_ATTACK1_DAMAGE_DICE:
		case UNIT_STATE_ATTACK1_DAMAGE_SIDE:
		case UNIT_STATE_ATTACK1_DAMAGE_BASE:
		case UNIT_STATE_ATTACK1_DAMAGE_BONUS:
		case UNIT_STATE_ATTACK1_DAMAGE_LOSS_FACTOR:
		case UNIT_STATE_ATTACK1_WEAPON_SOUND:
		case UNIT_STATE_ATTACK1_MAX_TARGETS:
		case UNIT_STATE_ATTACK1_INTERVAL:
		case UNIT_STATE_ATTACK1_INITIAL_DELAY:
		case UNIT_STATE_ATTACK1_RANGE:
		case UNIT_STATE_ATTACK1_RANGE_BUFFER:
		case UNIT_STATE_ATTACK1_DAMAGE_MIN:
		case UNIT_STATE_ATTACK1_DAMAGE_MAX:
		case UNIT_STATE_ATTACK1_BACK_SWING:
		case UNIT_STATE_ATTACK1_TARGET_TYPES:
		case UNIT_STATE_ATTACK1_SPILL_DIST:
		case UNIT_STATE_ATTACK1_SPILL_RADIUS:
		case UNIT_STATE_ATTACK1_WEAPON_TYPE:

		case UNIT_STATE_ATTACK2_DAMAGE_DICE:
		case UNIT_STATE_ATTACK2_DAMAGE_SIDE:
		case UNIT_STATE_ATTACK2_DAMAGE_BASE:
		case UNIT_STATE_ATTACK2_DAMAGE_BONUS:
		case UNIT_STATE_ATTACK2_DAMAGE_LOSS_FACTOR:
		case UNIT_STATE_ATTACK2_WEAPON_SOUND:
		case UNIT_STATE_ATTACK2_ATTACK_TYPE:
		case UNIT_STATE_ATTACK2_MAX_TARGETS:
		case UNIT_STATE_ATTACK2_INTERVAL:
		case UNIT_STATE_ATTACK2_INITIAL_DELAY:
		case UNIT_STATE_ATTACK2_RANGE:
		case UNIT_STATE_ATTACK2_RANGE_BUFFER:
		case UNIT_STATE_ATTACK2_DAMAGE_MIN:
		case UNIT_STATE_ATTACK2_DAMAGE_MAX:
		case UNIT_STATE_ATTACK2_BACK_SWING:
		case UNIT_STATE_ATTACK2_TARGET_TYPES:
		case UNIT_STATE_ATTACK2_SPILL_DIST:
		case UNIT_STATE_ATTACK2_SPILL_RADIUS:
		case UNIT_STATE_ATTACK2_WEAPON_TYPE:

		case UNIT_STATE_ARMOR:
		case UNIT_STATE_ARMOR_TYPE:
		case UNIT_STATE_RATE_OF_FIRE:
		case UNIT_STATE_ACQUISITION_RANGE:
		case UNIT_STATE_LIFE_REGEN:
		case UNIT_STATE_MANA_REGEN:
		case UNIT_STATE_MIN_RANGE:
		
		case UNIT_STATE_MAX_LIFE:
		case UNIT_STATE_MAX_MANA:
			break;
		default:
			base::c_call<void>(RealSetUnitState, unit_handle, state_type, value_ptr);
			return;
		}

		war3_searcher&s = get_war3_searcher();
		uintptr_t unit_object = s.unit_handle_to_object(unit_handle);
		if (!unit_object)
		{
			return ;
		}

		if (state_type == UNIT_STATE_LIFE_REGEN)
		{
			*(uint32_t*)(unit_object + 0xB0) = *value_ptr;
			return ;
		}

		if (state_type == UNIT_STATE_MANA_REGEN)
		{
			*(uint32_t*)(unit_object + 0xD4) = *value_ptr;
			return ;
		}

		if (state_type == UNIT_STATE_ARMOR)
		{
			*(uint32_t*)(unit_object + 0xE0) = *value_ptr;
			return ;
		}

		if (state_type == UNIT_STATE_ARMOR_TYPE)
		{
			*(uint32_t*)(unit_object + 0xE4) = (uint32_t)jass::from_real(*value_ptr);
			return ;
		}

		if (state_type == UNIT_STATE_MAX_LIFE || state_type == UNIT_STATE_MAX_MANA)
		{
			int32_t node_offest = (state_type == UNIT_STATE_MAX_LIFE) ? 0x8 : 0x28;
			int32_t nodeIndex = *(uint32_t*)(unit_object + 0x98 + node_offest);
			if (nodeIndex < 0)
				return;
			uintptr_t stateMaxAddr = getNodeAddress(nodeIndex ,0x84);
			//uintptr_t stateMaxAddr = getNodeAddress(nodeIndex ,0x78);	// current state
			*(uint32_t*)(stateMaxAddr) = *value_ptr;
			return;
		}

		unit_property* ptr = (unit_property*)*(uintptr_t*)(unit_object + 0x1E4 + (s.get_version() > version_124c ? 4: 0));
		if (!ptr)
		{
			return ;
		}

		switch (state_type)
		{
		// General Attributes
		case UNIT_STATE_RATE_OF_FIRE:
			ptr->rate_of_fire = *value_ptr;
			return ;
		case UNIT_STATE_ACQUISITION_RANGE:
			ptr->acquisition_range = *value_ptr;
			return ;
		case UNIT_STATE_MIN_RANGE:
			ptr->minimum_range = *value_ptr;
			return ;
		// Attack - 1
		case UNIT_STATE_ATTACK1_DAMAGE_DICE:
			ptr->attack1_damage_dice = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_DAMAGE_SIDE:
			ptr->attack1_damage_side = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_DAMAGE_BASE:
			ptr->attack1_damage_base = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_DAMAGE_BONUS:
			ptr->attack1_damage_bonus = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_DAMAGE_LOSS_FACTOR:
			ptr->attack1_damage_loss_factor = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_WEAPON_SOUND:
			ptr->attack1_weapon_sound = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_ATTACK_TYPE:
			ptr->attack1_atkType = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_MAX_TARGETS:
			ptr->attack1_target_amount = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_INTERVAL:
			ptr->attack1_interval = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_INITIAL_DELAY:
			ptr->attack1_dmgpt = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_RANGE:
			ptr->attack1_range = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_RANGE_BUFFER:
			ptr->attack1_range_buffer = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_BACK_SWING:
			ptr->attack1_back_swing = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_TARGET_TYPES:
			ptr->attack1_targs = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK1_SPILL_DIST:
			ptr->attack1_spill_dist = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_SPILL_RADIUS:
			ptr->attack1_spill_radius = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK1_WEAPON_TYPE:
			ptr->attack1_weapTp = (uint32_t)jass::from_real(*value_ptr);
			return;
		// Attack - 2
		case UNIT_STATE_ATTACK2_DAMAGE_DICE:
			ptr->attack2_damage_dice = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_DAMAGE_SIDE:
			ptr->attack2_damage_side = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_DAMAGE_BASE:
			ptr->attack2_damage_base = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_DAMAGE_BONUS:
			ptr->attack2_damage_bonus = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_DAMAGE_LOSS_FACTOR:
			ptr->attack2_damage_loss_factor = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_WEAPON_SOUND:
			ptr->attack2_weapon_sound = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_ATTACK_TYPE:
			ptr->attack2_atkType = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_MAX_TARGETS:
			ptr->attack2_target_amount = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_INTERVAL:
			ptr->attack2_interval = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_INITIAL_DELAY:
			ptr->attack2_dmgpt = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_RANGE:
			ptr->attack2_range = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_RANGE_BUFFER:
			ptr->attack2_range_buffer = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_BACK_SWING:
			ptr->attack2_back_swing = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_TARGET_TYPES:
			ptr->attack2_targs = (uint32_t)jass::from_real(*value_ptr);
			return ;
		case UNIT_STATE_ATTACK2_SPILL_DIST:
			ptr->attack2_spill_dist = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_SPILL_RADIUS:
			ptr->attack2_spill_radius = *value_ptr;
			return ;
		case UNIT_STATE_ATTACK2_WEAPON_TYPE:
			ptr->attack2_weapTp = (uint32_t)jass::from_real(*value_ptr);
			return;
		case UNIT_STATE_ATTACK1_DAMAGE_MIN:
		case UNIT_STATE_ATTACK1_DAMAGE_MAX:
		case UNIT_STATE_ATTACK2_DAMAGE_MIN:
		case UNIT_STATE_ATTACK2_DAMAGE_MAX:
		default:
			return ;
		}
	}

	uint32_t _cdecl EXGetUnitObject(uint32_t unit_handle)
	{
		return get_war3_searcher().unit_handle_to_object(unit_handle);
	}

	uint32_t __cdecl EXGetUnitId(uint32_t unit_handle)
	{
		war3_searcher&s = get_war3_searcher();
		uintptr_t unit_object = s.unit_handle_to_object(unit_handle);
		if (!unit_object)
		{
			return 0;
		}
		uint32_t retval = *(uint32_t*)(unit_object + 0x30);
		return retval;
	}

	void __cdecl EXSetUnitId(uint32_t unit_handle, uint32_t value)
	{
		war3_searcher&s = get_war3_searcher();
		uintptr_t unit_object = s.unit_handle_to_object(unit_handle);
		if (!unit_object)
		{
			return ;
		}
		*(uint32_t*)(unit_object + 0x30) = value;
	}

	void InitializeUnitState()
	{
		jass::japi_hook("GetUnitState", &RealGetUnitState, (uintptr_t)FakeGetUnitState);
		jass::japi_hook("SetUnitState", &RealSetUnitState, (uintptr_t)FakeSetUnitState);
		//jass::async_add((uintptr_t)EXGetUnitObject, "EXGetUnitObject", "(Hunit;)I");

		jass::japi_add((uintptr_t)EXGetUnitId, "EXGetUnitId", "(Hunit;)I");
		jass::japi_add((uintptr_t)EXSetUnitId, "EXSetUnitId", "(Hunit;I)V");
	}

}}}
