#ifndef YDWEAbilityStateIncluded
#define YDWEAbilityStateIncluded

library YDWEAbilityState
	globals
		private constant integer ABILITY_STATE_COOLDOWN         = 1

		private constant integer ABILITY_DATA_TARGS             = 100 // integer
		private constant integer ABILITY_DATA_CAST              = 101 // real
		private constant integer ABILITY_DATA_DUR               = 102 // real
		private constant integer ABILITY_DATA_HERODUR           = 103 // real
		private constant integer ABILITY_DATA_COST              = 104 // integer
		private constant integer ABILITY_DATA_COOL              = 105 // real
		private constant integer ABILITY_DATA_AREA              = 106 // real
		private constant integer ABILITY_DATA_RNG               = 107 // real
		private constant integer ABILITY_DATA_DATA_A            = 108 // real
		private constant integer ABILITY_DATA_DATA_B            = 109 // real
		private constant integer ABILITY_DATA_DATA_C            = 110 // real
		private constant integer ABILITY_DATA_DATA_D            = 111 // real
		private constant integer ABILITY_DATA_DATA_E            = 112 // real
		private constant integer ABILITY_DATA_DATA_F            = 113 // real
		private constant integer ABILITY_DATA_DATA_G            = 114 // real
		private constant integer ABILITY_DATA_DATA_H            = 115 // real
		private constant integer ABILITY_DATA_DATA_I            = 116 // real
		private constant integer ABILITY_DATA_UNITID            = 117 // integer

		private constant integer ABILITY_DATA_HOTKET            = 200 // integer
		private constant integer ABILITY_DATA_UNHOTKET          = 201 // integer
		private constant integer ABILITY_DATA_RESEARCH_HOTKEY   = 202 // integer
		private constant integer ABILITY_DATA_NAME              = 203 // string
		private constant integer ABILITY_DATA_ART               = 204 // string
		private constant integer ABILITY_DATA_TARGET_ART        = 205 // string
		private constant integer ABILITY_DATA_CASTER_ART        = 206 // string
		private constant integer ABILITY_DATA_EFFECT_ART        = 207 // string
		private constant integer ABILITY_DATA_AREAEFFECT_ART    = 208 // string
		private constant integer ABILITY_DATA_MISSILE_ART       = 209 // string
		private constant integer ABILITY_DATA_SPECIAL_ART       = 210 // string
		private constant integer ABILITY_DATA_LIGHTNING_EFFECT  = 211 // string
		private constant integer ABILITY_DATA_BUFF_TIP          = 212 // string
		private constant integer ABILITY_DATA_BUFF_UBERTIP      = 213 // string
		private constant integer ABILITY_DATA_RESEARCH_TIP      = 214 // string
		private constant integer ABILITY_DATA_TIP               = 215 // string
		private constant integer ABILITY_DATA_UNTIP             = 216 // string
		private constant integer ABILITY_DATA_RESEARCH_UBERTIP  = 217 // string
		private constant integer ABILITY_DATA_UBERTIP           = 218 // string
		private constant integer ABILITY_DATA_UNUBERTIP         = 219 // string
		private constant integer ABILITY_DATA_UNART             = 220 // string
	endglobals

	native EXGetUnitAbility        takes unit u, integer abilcode returns ability
	native EXGetUnitAbilityByIndex takes unit u, integer index returns ability
	native EXGetAbilityId          takes ability abil returns integer
	native EXGetAbilityState       takes ability abil, integer state_type returns real
	native EXSetAbilityState       takes ability abil, integer state_type, real value returns boolean
	native EXGetAbilityDataReal    takes ability abil, integer level, integer data_type returns real
	native EXSetAbilityDataReal    takes ability abil, integer level, integer data_type, real value returns boolean
	native EXGetAbilityDataInteger takes ability abil, integer level, integer data_type returns integer
	native EXSetAbilityDataInteger takes ability abil, integer level, integer data_type, integer value returns boolean
	native EXGetAbilityDataString  takes ability abil, integer level, integer data_type returns string
	native EXSetAbilityDataString  takes ability abil, integer level, integer data_type, string value returns boolean

	function YDWEGetUnitAbilityState takes unit u, integer abilcode, integer state_type returns real
		return EXGetAbilityState(EXGetUnitAbility(u, abilcode), state_type)
	endfunction

	function YDWEGetUnitAbilityDataInteger takes unit u, integer abilcode, integer level, integer data_type returns integer
		return EXGetAbilityDataInteger(EXGetUnitAbility(u, abilcode), level, data_type)
	endfunction

	function YDWEGetUnitAbilityDataReal takes unit u, integer abilcode, integer level, integer data_type returns real
		return EXGetAbilityDataReal(EXGetUnitAbility(u, abilcode), level, data_type)
	endfunction

	function YDWEGetUnitAbilityDataString takes unit u, integer abilcode, integer level, integer data_type returns string
		return EXGetAbilityDataString(EXGetUnitAbility(u, abilcode), level, data_type)
	endfunction

	function YDWESetUnitAbilityState takes unit u, integer abilcode, integer state_type, real value returns boolean
		return EXSetAbilityState(EXGetUnitAbility(u, abilcode), state_type, value)
	endfunction

	function YDWESetUnitAbilityDataInteger takes unit u, integer abilcode, integer level, integer data_type, integer value returns boolean
		return EXSetAbilityDataInteger(EXGetUnitAbility(u, abilcode), level, data_type, value)
	endfunction

	function YDWESetUnitAbilityDataReal takes unit u, integer abilcode, integer level, integer data_type, real value returns boolean
		return EXSetAbilityDataReal(EXGetUnitAbility(u, abilcode), level, data_type, value)
	endfunction

	function YDWESetUnitAbilityDataString takes unit u, integer abilcode, integer level, integer data_type, string value returns boolean
		return EXSetAbilityDataString(EXGetUnitAbility(u, abilcode), level, data_type, value)
	endfunction

	native EXSetAbilityAEmeDataA takes ability abil, integer unitid returns boolean

	function YDWEUnitTransform takes unit u, integer abilcode, integer targetid returns nothing
		call UnitAddAbility(u, abilcode)
		call EXSetAbilityDataInteger(EXGetUnitAbility(u, abilcode), 1, ABILITY_DATA_UNITID, GetUnitTypeId(u))
		call EXSetAbilityAEmeDataA(EXGetUnitAbility(u, abilcode), GetUnitTypeId(u))
		call UnitRemoveAbility(u, abilcode)
		call UnitAddAbility(u, abilcode)
		call EXSetAbilityAEmeDataA(EXGetUnitAbility(u, abilcode), targetid)
		call UnitRemoveAbility(u, abilcode)
	endfunction

	native EXGetItemDataString takes integer itemcode, integer data_type returns string
	native EXSetItemDataString takes integer itemcode, integer data_type, string value returns boolean

	function YDWEGetItemDataString takes integer itemcode, integer data_type returns string
		return EXGetItemDataString(itemcode, data_type)
	endfunction

	function YDWESetItemDataString takes integer itemcode, integer data_type, string value returns boolean
		return EXSetItemDataString(itemcode, data_type, value)
	endfunction

endlibrary

#endif  /// YDWEAbilityStateIncluded
