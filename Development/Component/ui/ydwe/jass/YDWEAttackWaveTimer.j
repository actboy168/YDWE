#ifndef YDWEAttackWaveTimerIncluded
#define YDWEAttackWaveTimerIncluded

#include "YDWEBase.j"

//===========================================================================
//Ë¢±øÏµÍ³ 
//===========================================================================
library YDWEAttackWaveTimer requires YDWEBase
	private function AttackWaveProc takes nothing returns nothing
		local timer t = GetExpiredTimer()
		local real x1 = YDWEGetRealByString( "AttackWave"+I2S(YDWEH2I(t)), "x1")
		local real y1 = YDWEGetRealByString( "AttackWave"+I2S(YDWEH2I(t)), "y1")
		local real x2 = YDWEGetRealByString( "AttackWave"+I2S(YDWEH2I(t)), "x2")
		local real y2 = YDWEGetRealByString( "AttackWave"+I2S(YDWEH2I(t)), "y2")
		local real face = YDWEGetRealByString( "AttackWave"+I2S(YDWEH2I(t)), "face")
		local integer N = YDWEGetIntegerByString( "AttackWave"+I2S(YDWEH2I(t)), "N")
		local integer unitid = YDWEGetIntegerByString( "AttackWave"+I2S(YDWEH2I(t)), "unitid")
		local player p = Player(YDWEGetIntegerByString("AttackWave"+I2S(YDWEH2I(t)), "playerid"))     
		local unit tempUnit
		local integer n = 0   
		call GroupClear(bj_lastCreatedGroup)                                                            
		loop              
			exitwhen (n>=N) 
			set tempUnit = CreateUnit(p,unitid,x1,y1,face)  
			call GroupAddUnit(bj_lastCreatedGroup, tempUnit) 
			set bj_lastCreatedUnit=tempUnit      
			call IssuePointOrder(tempUnit, "attack", x2,y2) 
			set n=n+1
			set tempUnit = null
		endloop    
	endfunction

	function YDWEAttackWaveTimer takes player p,integer N,integer unitid,location whichLocation,real face,location TargetLocation,real timeout returns nothing
		local  real x1 = GetLocationX(whichLocation)
		local  real y1 = GetLocationY(whichLocation)  
		local  real x2 = GetLocationX(TargetLocation) 
		local  real y2 = GetLocationY(TargetLocation)   
		local  timer t = CreateTimer()   
		call YDWESaveIntegerByString( "AttackWave"+I2S(unitid)+R2S(x1)+R2S(y1)+R2S(timeout), "Timer", YDWEH2I(t))
		call YDWESaveIntegerByString( "AttackWave"+I2S(YDWEH2I(t)), "unitid", unitid)
		call YDWESaveIntegerByString( "AttackWave"+I2S(YDWEH2I(t)), "playerid", GetPlayerId(p))
		call YDWESaveIntegerByString( "AttackWave"+I2S(YDWEH2I(t)), "N", N)   
		call YDWESaveRealByString( "AttackWave"+I2S(YDWEH2I(t)), "x1", x1)       
		call YDWESaveRealByString( "AttackWave"+I2S(YDWEH2I(t)), "y1", y1)        
		call YDWESaveRealByString( "AttackWave"+I2S(YDWEH2I(t)), "x2", x2)        
		call YDWESaveRealByString( "AttackWave"+I2S(YDWEH2I(t)), "y2", y2)
		call YDWESaveRealByString( "AttackWave"+I2S(YDWEH2I(t)), "face", face)  
		call YDWESaveTimerByString( "AttackWaveLastTimer", "Timer", t)
		call YDWESaveStringByString( "AttackWave"+I2S(YDWEH2I(t)), "Store", "AttackWave"+I2S(unitid)+R2S(x1)+R2S(y1)+R2S(timeout))
		call TimerStart(t,timeout,true,function AttackWaveProc) 
		call RemoveLocation(whichLocation)   
		set t = null
	endfunction

	function YDWERemoveAttackWaveByUnitID takes integer unitid,location whichLocation,real timeout returns nothing
		local integer t = YDWEGetIntegerByString( "AttackWave"+I2S(unitid)+R2S(GetLocationX(whichLocation))+R2S(GetLocationY(whichLocation))+R2S(timeout), "Timer")
		call YDWEFlushMissionByString( "AttackWave"+I2S(unitid)+I2S(YDWEH2I(whichLocation))+R2S(timeout))
		call YDWEFlushMissionByString( "AttackWave"+I2S(t))
		call DestroyTimer(YDWEGetTimerByString("AttackWave"+I2S(unitid)+R2S(GetLocationX(whichLocation))+R2S(GetLocationY(whichLocation))+R2S(timeout), "Timer"))
	endfunction

	function YDWERemoveAttackWaveByTimer takes timer t returns nothing
		call YDWEFlushMissionByString( YDWEGetStringByString("AttackWave"+I2S(YDWEH2I(t)), "Store"))
		call YDWEFlushMissionByString( "AttackWave"+I2S(YDWEH2I(t)))
		call DestroyTimer(t)
	endfunction   

	function YDWERemoveAttackWaveReturnTimer takes nothing returns timer
		return YDWEGetTimerByString("AttackWaveLastTimer","Timer")
	endfunction
endlibrary 

#endif /// YDWEAttackWaveTimerIncluded
