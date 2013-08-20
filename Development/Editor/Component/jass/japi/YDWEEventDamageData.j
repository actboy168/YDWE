#ifndef YDWEEventDamageDataIncluded 
#define YDWEEventDamageDataIncluded

library YDWEEventDamageData
	globals        
    	private constant integer EVENT_DAMAGE_DATA_VAILD       = 0
    	private constant integer EVENT_DAMAGE_DATA_IS_PHYSICAL = 1
    	private constant integer EVENT_DAMAGE_DATA_IS_ATTACK   = 2
    	private constant integer EVENT_DAMAGE_DATA_IS_RANGED   = 3
    	private constant integer EVENT_DAMAGE_DATA_DAMAGE_TYPE = 4
    	private constant integer EVENT_DAMAGE_DATA_WEAPON_TYPE = 5
    	private constant integer EVENT_DAMAGE_DATA_ATTACK_TYPE = 6
	endglobals

	native EXGetEventDamageData takes integer edd_type returns integer
	native EXSetEventDamage takes real amount returns boolean
	
	function YDWEIsEventPhysicalDamage takes nothing returns boolean
		return 0 != EXGetEventDamageData(EVENT_DAMAGE_DATA_IS_PHYSICAL)
	endfunction

	function YDWEIsEventAttackDamage takes nothing returns boolean
		return 0 != EXGetEventDamageData(EVENT_DAMAGE_DATA_IS_ATTACK)
	endfunction
	
	function YDWEIsEventRangedDamage takes nothing  returns boolean
		return 0 != EXGetEventDamageData(EVENT_DAMAGE_DATA_IS_RANGED)
	endfunction
	
	function YDWEIsEventDamageType takes damagetype damageType returns boolean
		return damageType == ConvertDamageType(EXGetEventDamageData(EVENT_DAMAGE_DATA_DAMAGE_TYPE))
	endfunction

	function YDWEIsEventWeaponType takes weapontype weaponType returns boolean
		return weaponType == ConvertWeaponType(EXGetEventDamageData(EVENT_DAMAGE_DATA_WEAPON_TYPE))
	endfunction
	
	function YDWEIsEventAttackType takes attacktype attackType returns boolean
		return attackType == ConvertAttackType(EXGetEventDamageData(EVENT_DAMAGE_DATA_ATTACK_TYPE))
	endfunction

	
	function YDWESetEventDamage takes real amount returns boolean
		return EXSetEventDamage(amount)
	endfunction
	
	
endlibrary

#endif  /// YDWEEventDamageDataIncluded
