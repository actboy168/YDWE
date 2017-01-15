
//===========================================================================
//Return bug
//===========================================================================
globals
//ȫ�ֻ��� 
#ifndef YDWE_HASH_DEFVAR
#define YDWE_HASH_DEFVAR
	gamecache YDGC = null  
#endif  
endglobals

//===========================================================================
//Return bug
//===========================================================================
function YDWEH2I takes handle h returns integer
    return h
    return 0
endfunction

function YDWEI2U takes integer i returns unit
    return i
    return null
endfunction

function YDWEI2It takes integer i returns item
    return i
    return null
endfunction

function YDWEI2P takes integer i returns player
    return i
    return null
endfunction

function YDWEI2Tm takes integer i returns timer
    return i
    return null
endfunction

function YDWEI2T takes integer i returns trigger
    return i
    return null
endfunction

function YDWEI2Pt takes integer i returns location
    return i
    return null
endfunction

function YDWEI2G takes integer i returns group
    return i
    return null
endfunction

function YDWEI2M takes integer i returns multiboard
    return i
    return null
endfunction

function YDWEI2Mt takes integer i returns multiboarditem
    return i
    return null
endfunction

function YDWEI2Tt takes integer i returns texttag
    return i
    return null
endfunction

function YDWEI2Ln takes integer i returns lightning
    return i
    return null
endfunction

function YDWEI2Rn takes integer i returns region
    return i
    return null
endfunction

function YDWEI2Rt takes integer i returns rect
    return i
    return null
endfunction

function YDWEI2Lb takes integer i returns leaderboard
    return i
    return null
endfunction

function YDWEI2E takes integer i returns effect
    return i
    return null
endfunction

function YDWEI2Da takes integer i returns destructable
    return i
    return null
endfunction

function YDWEI2TC takes integer i returns triggercondition
    return i
    return null
endfunction

function YDWEI2TA takes integer i returns triggeraction
    return i
    return null
endfunction

function YDWEI2Et takes integer i returns event
    return i
    return null
endfunction

function YDWEI2Fc takes integer i returns force
    return i
    return null
endfunction

function YDWEI2BE takes integer i returns boolexpr
    return i
    return null
endfunction

function YDWEI2Sd takes integer i returns sound
    return i
    return null
endfunction

function YDWEI2TD takes integer i returns timerdialog
    return i
    return null
endfunction

function YDWEI2Ta takes integer i returns trackable
    return i
    return null
endfunction

function YDWEI2D takes integer i returns dialog
    return i
    return null
endfunction

function YDWEI2Bt takes integer i returns button
    return i
    return null
endfunction

function YDWEI2Qt takes integer i returns quest
    return i
    return null
endfunction

function YDWEI2QI takes integer i returns questitem
    return i
    return null
endfunction

function GetHandleId takes handle h returns integer
    return h
    return 0
endfunction

//����
function YDWEFlushAllData takes nothing returns nothing
    call FlushGameCache(YDGC)
endfunction

function YDWEFlushMissionByInteger takes integer i returns nothing
    call FlushStoredMission(YDGC,I2S(i))
endfunction

function YDWEFlushMissionByString takes string s returns nothing
    call FlushStoredMission(YDGC,s)
endfunction

function YDWEFlushStoredIntegerByInteger takes integer i,integer j returns nothing
    call FlushStoredInteger(YDGC,I2S(i),I2S(j))
endfunction

function YDWEFlushStoredIntegerByString takes string s1,string s2 returns nothing
    call FlushStoredInteger(YDGC,s1,s2)
endfunction

function YDWEHaveSavedIntegerByInteger takes integer i,integer j returns boolean
    return HaveStoredInteger(YDGC,I2S(i),I2S(j))
endfunction

function YDWEHaveSavedIntegerByString takes string s1,string s2 returns boolean
    return HaveStoredInteger(YDGC,s1,s2)
endfunction

//store and get integer
function YDWESaveIntegerByInteger takes integer pTable,integer pKey,integer i returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),i)
endfunction

function YDWESaveIntegerByString takes string pTable,string pKey,integer i returns nothing
    call StoreInteger(YDGC,pTable,pKey,i)
endfunction

function YDWEGetIntegerByInteger takes integer pTable,integer pKey returns integer
    return GetStoredInteger(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetIntegerByString takes string pTable,string pKey returns integer
    return GetStoredInteger(YDGC,pTable,pKey)
endfunction

//store and get real
function YDWESaveRealByInteger takes integer pTable,integer pKey,real r returns nothing
    call StoreReal(YDGC,I2S(pTable),I2S(pKey),r)
endfunction

function YDWESaveRealByString takes string pTable,string pKey,real r returns nothing
    call StoreReal(YDGC,pTable,pKey,r)
endfunction

function YDWEGetRealByInteger takes integer pTable,integer pKey returns real
    return GetStoredReal(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetRealByString takes string pTable,string pKey returns real
    return GetStoredReal(YDGC,pTable,pKey)
endfunction

//store and get string
function YDWESaveStringByInteger takes integer pTable,integer pKey,string s returns nothing
    call StoreString(YDGC,I2S(pTable),I2S(pKey),s)
endfunction

function YDWESaveStringByString takes string pTable,string pKey,string s returns nothing
    call StoreString(YDGC,pTable,pKey,s)
endfunction

function YDWEGetStringByInteger takes integer pTable,integer pKey returns string
    return GetStoredString(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetStringByString takes string pTable,string pKey returns string
    return GetStoredString(YDGC,pTable,pKey)
endfunction

//store and get boolean
function YDWESaveBooleanByInteger takes integer pTable,integer pKey,boolean b returns nothing
    call StoreBoolean(YDGC,I2S(pTable),I2S(pKey),b)
endfunction

function YDWESaveBooleanByString takes string pTable,string pKey,boolean b returns nothing
    call StoreBoolean(YDGC,pTable,pKey,b)
endfunction

function YDWEGetBooleanByInteger takes integer pTable,integer pKey returns boolean
    return GetStoredBoolean(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetBooleanByString takes string pTable,string pKey returns boolean
    return GetStoredBoolean(YDGC,pTable,pKey)
endfunction

//Covert Unit
function YDWESaveUnitByInteger takes integer pTable,integer pKey,unit u returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(u))
endfunction

function YDWESaveUnitByString takes string pTable,string pKey,unit u returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(u))
endfunction

function YDWEGetUnitByInteger takes integer pTable,integer pKey returns unit
    return YDWEI2U(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetUnitByString takes string pTable,string pKey returns unit
    return YDWEI2U(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert UnitID
function YDWESaveUnitIDByInteger takes integer pTable,integer pKey,integer uid returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),uid)
endfunction

function YDWESaveUnitIDByString takes string pTable,string pKey,integer uid returns nothing
    call StoreInteger(YDGC,pTable,pKey,uid)
endfunction

function YDWEGetUnitIDByInteger takes integer pTable,integer pKey returns integer
    return GetStoredInteger(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetUnitIDByString takes string pTable,string pKey returns integer
    return GetStoredInteger(YDGC,pTable,pKey)
endfunction

//Covert AbilityID
function YDWESaveAbilityIDByInteger takes integer pTable,integer pKey,integer abid returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),abid)
endfunction

function YDWESaveAbilityIDByString takes string pTable,string pKey,integer abid returns nothing
    call StoreInteger(YDGC,pTable,pKey,abid)
endfunction

function YDWEGetAbilityIDByInteger takes integer pTable,integer pKey returns integer
    return GetStoredInteger(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetAbilityIDByString takes string pTable,string pKey returns integer
    return GetStoredInteger(YDGC,pTable,pKey)
endfunction

//Covert Player
function YDWESavePlayerByInteger takes integer pTable,integer pKey,player p returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(p))
endfunction

function YDWESavePlayerByString takes string pTable,string pKey,player p returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(p))
endfunction

function YDWEGetPlayerByInteger takes integer pTable,integer pKey returns player
    return YDWEI2P(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetPlayerByString takes string pTable,string pKey returns player
    return YDWEI2P(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert Item
function YDWESaveItemByInteger takes integer pTable,integer pKey,item it returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(it))
endfunction

function YDWESaveItemByString takes string pTable,string pKey,item it returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(it))
endfunction

function YDWEGetItemByInteger takes integer pTable,integer pKey returns item
    return YDWEI2It(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetItemByString takes string pTable,string pKey returns item
    return YDWEI2It(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert ItemID
function YDWESaveItemIDByInteger takes integer pTable,integer pKey,integer itid returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),itid)
endfunction

function YDWESaveItemIDByString takes string pTable,string pKey,integer itid returns nothing
    call StoreInteger(YDGC,pTable,pKey,itid)
endfunction

function YDWEGetItemIDByInteger takes integer pTable,integer pKey returns integer
    return GetStoredInteger(YDGC,I2S(pTable),I2S(pKey))
endfunction

function YDWEGetItemIDByString takes string pTable,string pKey returns integer
    return GetStoredInteger(YDGC,pTable,pKey)
endfunction

//Covert Timer
function YDWESaveTimerByInteger takes integer pTable,integer pKey,timer t returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(t))
endfunction

function YDWESaveTimerByString takes string pTable,string pKey,timer t returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(t))
endfunction

function YDWEGetTimerByInteger takes integer pTable,integer pKey returns timer
    return YDWEI2Tm(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTimerByString takes string pTable,string pKey returns timer
    return YDWEI2Tm(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert Trigger
function YDWESaveTriggerByInteger takes integer pTable,integer pKey,trigger trg returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(trg))
endfunction

function YDWESaveTriggerByString takes string pTable,string pKey,trigger trg returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(trg))
endfunction

function YDWEGetTriggerByInteger takes integer pTable,integer pKey returns trigger
    return YDWEI2T(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTriggerByString takes string pTable,string pKey returns trigger
    return YDWEI2T(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert Location
function YDWESaveLocationByInteger takes integer pTable,integer pKey,location pt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(pt))
endfunction

function YDWESaveLocationByString takes string pTable,string pKey,location pt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(pt))
endfunction

function YDWEGetLocationByInteger takes integer pTable,integer pKey returns location
    return YDWEI2Pt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetLocationByString takes string pTable,string pKey returns location
    return YDWEI2Pt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert Group
function YDWESaveGroupByInteger takes integer pTable,integer pKey,group g returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(g))
endfunction

function YDWESaveGroupByString takes string pTable,string pKey,group g returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(g))
endfunction

function YDWEGetGroupByInteger takes integer pTable,integer pKey returns group
    return YDWEI2G(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetGroupByString takes string pTable,string pKey returns group
    return YDWEI2G(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert multiboard
function YDWESaveMultiboardByInteger takes integer pTable,integer pKey,multiboard m returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(m))
endfunction

function YDWESaveMultiboardByString takes string pTable,string pKey,multiboard m returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(m))
endfunction

function YDWEGetMultiboardByInteger takes integer pTable,integer pKey returns multiboard
    return YDWEI2M(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetMultiboardByString takes string pTable,string pKey returns multiboard
    return YDWEI2M(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert multiboarditem
function YDWESaveMultiboardItemByInteger takes integer pTable,integer pKey,multiboarditem mt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(mt))
endfunction

function YDWESaveMultiboardItemByString takes string pTable,string pKey,multiboarditem mt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(mt))
endfunction

function YDWEGetMultiboardItemByInteger takes integer pTable,integer pKey returns multiboarditem
    return YDWEI2Mt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetMultiboardItemByString takes string pTable,string pKey returns multiboarditem
    return YDWEI2Mt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert Texttag
function YDWESaveTextTagByInteger takes integer pTable,integer pKey,texttag tt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(tt))
endfunction

function YDWESaveTextTagByString takes string pTable,string pKey,texttag tt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(tt))
endfunction

function YDWEGetTextTagByInteger takes integer pTable,integer pKey returns texttag
    return YDWEI2Tt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTextTagByString takes string pTable,string pKey returns texttag
    return YDWEI2Tt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert lightning
function YDWESaveLightningByInteger takes integer pTable,integer pKey,lightning ln returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(ln))
endfunction

function YDWESaveLightningByString takes string pTable,string pKey,lightning ln returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(ln))
endfunction

function YDWEGetLightningByInteger takes integer pTable,integer pKey returns lightning
   return YDWEI2Ln(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetLightningByString takes string pTable,string pKey returns lightning
    return YDWEI2Ln(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert region
function YDWESaveRegionByInteger takes integer pTable,integer pKey,region rn returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(rn))
endfunction

function YDWESaveRegionByString takes string pTable,string pKey,region rn returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(rn))
endfunction

function YDWEGetRegionByInteger takes integer pTable,integer pKey returns region
    return YDWEI2Rn(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetRegionByString takes string pTable,string pKey returns region
    return YDWEI2Rn(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert rect
function YDWESaveRectByInteger takes integer pTable,integer pKey,rect rt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(rt))
endfunction

function YDWESaveRectByString takes string pTable,string pKey,rect rt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(rt))
endfunction

function YDWEGetRectByInteger takes integer pTable,integer pKey returns rect
    return YDWEI2Rt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetRectByString takes string pTable,string pKey returns rect
    return YDWEI2Rt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert leaderboard
function YDWESaveLeaderboardByInteger takes integer pTable,integer pKey,leaderboard lb returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(lb))
endfunction

function YDWESaveLeaderboardByString takes string pTable,string pKey,leaderboard lb returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(lb))
endfunction

function YDWEGetLeaderboardByInteger takes integer pTable,integer pKey returns leaderboard
    return YDWEI2Lb(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetLeaderboardByString takes string pTable,string pKey returns leaderboard
    return YDWEI2Lb(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert effect
function YDWESaveEffectByInteger takes integer pTable,integer pKey,effect e returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(e))
endfunction

function YDWESaveEffectByString takes string pTable,string pKey,effect e returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(e))
endfunction

function YDWEGetEffectByInteger takes integer pTable,integer pKey returns effect
    return YDWEI2E(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetEffectByString takes string pTable,string pKey returns effect
    return YDWEI2E(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert destructable
function YDWESaveDestructableByInteger takes integer pTable,integer pKey,destructable da returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(da))
endfunction

function YDWESaveDestructableByString takes string pTable,string pKey,destructable da returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(da))
endfunction

function YDWEGetDestructableByInteger takes integer pTable,integer pKey returns destructable
    return YDWEI2Da(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetDestructableByString takes string pTable,string pKey returns destructable
    return YDWEI2Da(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert triggercondition
function YDWESaveTriggerConditionByInteger takes integer pTable,integer pKey,triggercondition tc returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(tc))
endfunction

function YDWESaveTriggerConditionByString takes string pTable,string pKey,triggercondition tc returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(tc))
endfunction

function YDWEGetTriggerConditionByInteger takes integer pTable,integer pKey returns triggercondition
    return YDWEI2TC(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTriggerConditionByString takes string pTable,string pKey returns triggercondition
    return YDWEI2TC(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert triggeraction
function YDWESaveTriggerActionByInteger takes integer pTable,integer pKey,triggeraction ta returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(ta))
endfunction

function YDWESaveTriggerActionByString takes string pTable,string pKey,triggeraction ta returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(ta))
endfunction

function YDWEGetTriggerActionByInteger takes integer pTable,integer pKey returns triggeraction
    return YDWEI2TA(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTriggerActionByString takes string pTable,string pKey returns triggeraction
    return YDWEI2TA(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert event
function YDWESaveTriggerEventByInteger takes integer pTable,integer pKey,event et returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(et))
endfunction

function YDWESaveTriggerEventByString takes string pTable,string pKey,event et returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(et))
endfunction

function YDWEGetTriggerEventByInteger takes integer pTable,integer pKey returns event
    return YDWEI2Et(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTriggerEventByString takes string pTable,string pKey returns event
    return YDWEI2Et(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert force
function YDWESaveForceByInteger takes integer pTable,integer pKey,force fc returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(fc))
endfunction

function YDWESaveForceByString takes string pTable,string pKey,force fc returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(fc))
endfunction

function YDWEGetForceByInteger takes integer pTable,integer pKey returns force
    return YDWEI2Fc(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetForceByString takes string pTable,string pKey returns force
    return YDWEI2Fc(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert boolexpr
function YDWESaveBoolexprByInteger takes integer pTable,integer pKey,boolexpr be returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(be))
endfunction

function YDWESaveBoolexprByString takes string pTable,string pKey,boolexpr be returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(be))
endfunction

function YDWEGetBoolexprByInteger takes integer pTable,integer pKey returns boolexpr
    return YDWEI2BE(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetBoolexprByString takes string pTable,string pKey returns boolexpr
    return YDWEI2BE(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert sound
function YDWESaveSoundByInteger takes integer pTable,integer pKey,sound sd returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(sd))
endfunction

function YDWESaveSoundByString takes string pTable,string pKey,sound sd returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(sd))
endfunction

function YDWEGetSoundByInteger takes integer pTable,integer pKey returns sound
    return YDWEI2Sd(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetSoundByString takes string pTable,string pKey returns sound
    return YDWEI2Sd(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert timerdialog
function YDWESaveTimerDialogByInteger takes integer pTable,integer pKey,timerdialog td returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(td))
endfunction

function YDWESaveTimerDialogByString takes string pTable,string pKey,timerdialog td returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(td))
endfunction

function YDWEGetTimerDialogByInteger takes integer pTable,integer pKey returns timerdialog
    return YDWEI2TD(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTimerDialogByString takes string pTable,string pKey returns timerdialog
    return YDWEI2TD(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert trackable
function YDWESaveTrackableByInteger takes integer pTable,integer pKey,trackable ta returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(ta))
endfunction

function YDWESaveTrackableByString takes string pTable,string pKey,trackable ta returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(ta))
endfunction

function YDWEGetTrackableByInteger takes integer pTable,integer pKey returns trackable
    return YDWEI2Ta(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetTrackableByString takes string pTable,string pKey returns trackable
    return YDWEI2Ta(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert dialog
function YDWESaveDialogByInteger takes integer pTable,integer pKey,dialog d returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(d))
endfunction

function YDWESaveDialogByString takes string pTable,string pKey,dialog d returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(d))
endfunction

function YDWEGetDialogByInteger takes integer pTable,integer pKey returns dialog
    return YDWEI2D(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetDialogByString takes string pTable,string pKey returns dialog
    return YDWEI2D(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert button
function YDWESaveButtonByInteger takes integer pTable,integer pKey,button bt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(bt))
endfunction

function YDWESaveButtonByString takes string pTable,string pKey,button bt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(bt))
endfunction

function YDWEGetButtonByInteger takes integer pTable,integer pKey returns button
    return YDWEI2Bt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetButtonByString takes string pTable,string pKey returns button
    return YDWEI2Bt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert quest
function YDWESaveQuestByInteger takes integer pTable,integer pKey,quest qt returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(qt))
endfunction

function YDWESaveQuestByString takes string pTable,string pKey,quest qt returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(qt))
endfunction

function YDWEGetQuestByInteger takes integer pTable,integer pKey returns quest
    return YDWEI2Qt(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetQuestByString takes string pTable,string pKey returns quest
    return YDWEI2Qt(GetStoredInteger(YDGC,pTable,pKey))
endfunction

//Covert questitem
function YDWESaveQuestItemByInteger takes integer pTable,integer pKey,questitem qi returns nothing
    call StoreInteger(YDGC,I2S(pTable),I2S(pKey),YDWEH2I(qi))
endfunction

function YDWESaveQuestItemByString takes string pTable,string pKey,questitem qi returns nothing
    call StoreInteger(YDGC,pTable,pKey,YDWEH2I(qi))
endfunction

function YDWEGetQuestItemByInteger takes integer pTable,integer pKey returns questitem
    return YDWEI2QI(GetStoredInteger(YDGC,I2S(pTable),I2S(pKey)))
endfunction

function YDWEGetQuestItemByString takes string pTable,string pKey returns questitem
    return YDWEI2QI(GetStoredInteger(YDGC,pTable,pKey))
endfunction

function YDWES2I takes string s returns integer
    return s
    return 0
endfunction

function GetSpellTargetX takes nothing returns real
    local location point=GetSpellTargetLoc()
    local real x=GetLocationX(point)
    call RemoveLocation(point)
    set point=null
    return  x
endfunction

function GetSpellTargetY takes nothing returns real
    local location point=GetSpellTargetLoc()
    local real y=GetLocationY(point)
    call RemoveLocation(point)
    set point=null
    return  y
endfunction




















