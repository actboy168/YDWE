// Edit By TigerCN
define {
    Limit = 0x100000
    ToMemory = GetHandleId
    <new hash> = InitHashtable()
    <new cache>(file) = InitGameCache(file)
    <new clock> = CreateTimer()
    <new form> = CreateGroup()
    <new troop> = CreateForce()
    <new area> = CreateRegion()
    <new contact> = CreateTrigger()
    <new frame> = DialogCreate()
    <new rank> = CreateLeaderboard()
    <new mult> = CreateMultiboard()
    <new ask> = CreateQuest()
    <new tag> = CreateTextTag()
    <new camera> = CreateCameraSetup()
    <new shut>(t) = CreateTimerDialog(t)

    <new weather>(where,effectID) = AddWeatherEffect(where,effectID)

    <new own>() = GetLocalPlayer()
    <new own>(id) = Player(id)

    <new loc>() = Location(0,0)
    <new loc>(x,y) = Location(x,y)

    <new rct>() = Rect(0,0,0,0)
    <new rct>(x1,y1,x2,y2) = Rect(x1,y1,x2,y2)

    <new obji>(itemid) = CreateItem(itemid,0,0)
    <new obji>(itemid,x,y) = CreateItem(itemid,x,y)

    <new object> = GetTriggerUnit()
    <new object>(p,id) = CreateUnit(p,id,0,0,0)
    <new object>(p,id,x,y) = CreateUnit(p,id,x,y,0)
    <new object>(p,id,x,y,f) = CreateUnit(p,id,x,y,f)

    <new sequel>(modelName) = AddSpecialEffect(modelName,0,0)
    <new sequel>(modelName,x,y) = AddSpecialEffect(modelName,x,y)

    <new objd>(objectid,x,y) = CreateDestructable(objectid,x,y,1,0)
    <new objd>(objectid,x,y,z) = CreateDestructableZ(objectid,x,y,z,1,0)
    <new objd>(objectid,x,y,facescale,variation) = CreateDestructable(objectid,x,y,facescale,variation)
    <new objd>(objectid,x,y,z,facescale,variation) = CreateDestructableZ(objectid,x,y,z,facescale,variation)

    <new trail>(trackableModelPath,x,y,facing) = CreateTrackable(trackableModelPath,x,y,facing)

    <new voice>(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting) = CreateSound(fileName,looping,is3D,stopwhenoutofrange,fadeInRate,fadeOutRate,eaxSetting)

    <new ubersplat>(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime) = CreateUbersplat(x,y,name,red,green,blue,alpha,forcePaused,noBirthTime)

    <new fulm>(codeName,checkVisibility,x1,y1,x2,y2) = AddLightning(codeName,checkVisibility,x1,y1,x2,y2)
    <new fulm>(codeName,checkVisibility,x1,y1,z1,x2,y2,z2) = AddLightningEx(codeName,checkVisibility,x1,y1,z1,x2,y2,z2)

    <new image>(file,sizeX,sizeY,sizez,posX,posY,posZ,originX,originY,originZ,imageType) = CreateImage(file,sizeX,sizeY,sizeZ,posX,posY,posz,originX,originY,originZ,imageType)
    
    <new act>(t,a) = TriggerAddAction(t, a)
    <new term>(t,a) = TriggerAddCondition(t,a)

    <current contact> = GetTriggeringTrigger()
    <current clock> = GetExpiredTimer()
    <current trail> = GetTriggeringTrackable()
    <current button> = GetClickedButton()

    <this x> = GetUnitX(GetTriggerUnit())
    <this y> = GetUnitY(GetTriggerUnit())
    <that object> = GetSpellTargetUnit()
    <this loc>(u) = GetUnitLoc(u)
    <that loc> = GetSpellTargetLoc()

    Sleep(a) = TriggerSleepAction(a)

    addobj(u,s) = UnitAddAbility(u,s)
    removeobj(u,s) = UnitRemoveAbility(u,s)

    towords = GetUnitFacing

    <del obj> = RemoveUnit
    <del form> = DestroyGroup
    <del loc> = RemoveLocation
    <del clock> = DestroyTimer
    <del shut> = DestroyTimerDialog
    <del sequel> = DestroyEffect
    <del tag> = DestroyTextTag
    <del fact> = DestroyTrigger
    <del term> = DestroyCondition
    <del obji> = RemoveItem
    <del objd> = RemoveDestructable

    //begin(t, lasttime, circle, name) = TimerStart(t, lasttime, circle, function name)
    endl(t) = { PauseTimer(t); ReleaseTimer(t) }
    
    register(trig,whichEvent,cond) = {
        local integer index
        set index = 0
        loop
            call TriggerRegisterPlayerUnitEvent(trig, Player(index), whichEvent, null)
            set index = index + 1
            exitwhen index == bj_MAX_PLAYER_SLOTS
        endloop
        TriggerAddCondition(trig,Condition(function cond))
    }

    free(a) = a = null
    free(a,b) = { a=null; b=null }
    free(a,b,c) = { a=null; b=null; c=null }
    free(a,b,c,d) = { a=null; b=null; c=null; d=null }         
    free(a,b,c,d,e) = { a=null; b=null; c=null; d=null; e = null }
    free(a,b,c,d,e,f) = { a=null; b=null; c=null; d=null; e = null; f = null }
    free(a,b,c,d,e,f,g) = { a=null; b=null; c=null; d=null; e = null; f = null; g = null }

    cos(d,r) = d * Cos(r * bj_DEGTORAD)
    sin(d,r) = d * Sin(r * bj_DEGTORAD)

    printf = BJDebugMsg
    closet = DisableTrigger
    opent = EnableTrigger
    
    /*getobjx = GetUnitX
    getobjy = GetUnitY
    getobjz = GetUnitFlyHeight*/
    setobjcoor(u,x,y,z) = { SetUnitX(u,x); SetUnitY(u,y); SetUnitFlyHeight(u,z,0) }
    
    <new here>(u,id)=CreateUnit(GetOwningPlayer(u),id,GetUnitX(u),GetUnitY(u),GetUnitFacing(u))

    getf1(u1,u2) = 57.296*Atan2(GetUnitY(u2)-GetUnitY(u1), GetUnitX(u2)-GetUnitX(u1))
    getf2(u,l) = 57.296*Atan2(GetLocationY(l)-GetUnitY(u), GetLocationX(l)-GetUnitX(u))
    getf3(l1,l2) = 57.296*Atan2(GetLocationY(l2)-GetLocationY(l1), GetLocationX(l2)-GetLocationX(l1))
    getl1(u1,u2) = SquareRoot((GetUnitX(u2)-GetUnitX(u1))*(GetUnitX(u2)-GetUnitX(u1))+(GetUnitY(u2)-GetUnitY(u1))*(GetUnitY(u2)-GetUnitY(u1)))
    getl2(u,l) = SquareRoot((GetLocationX(l)-GetUnitX(u))*(GetLocationX(l)-GetUnitX(u))+(GetLocationY(l)-GetUnitY(u))*(GetLocationY(l)-GetUnitY(u)))
    getl3(l1,l2) = SquareRoot((GetLocationX(l2)-GetLocationX(l1))*(GetLocationX(l2)-GetLocationX(l1))+(GetLocationY(l2)-GetLocationY(l1))*(GetLocationY(l2)-GetLocationY(l1)))
    
    pass = GetSpellAbilityId()
    path(x,y) = IsTerrainPathable(x, y, PATHING_TYPE_WALKABILITY) == false

    addform = GroupAddUnit
    removeform = GroupRemoveUnit
    head = FirstOfGroup
    perk(whichGroup, x, y, radius, filter) = GroupEnumUnitsInRange(whichGroup, x, y, radius, filter)
    perk(whichGroup, r, filter) = GroupEnumUnitsInRect(whichGroup, r, filter)

    autoinstall = { FogEnable(false); FogMaskEnable(false); BJDebugMsg("|cff00ff00Author: |r"+"|cff1e90ffWarft_TigerCN|r") }
    root = Player(0)
    
    floatlist(a,b) = {float a,float b}
    floatlist(a,b,c) = {float a,float b,float c}
    floatlist(a,b,c,d) = {float a,float b,float c,float d}
    intlist(a,b) = {int a,int b}
    intlist(a,b,c) = {int a,int b,int c}
    intlist(a,b,c,d) = {int a,int b,int c,int d}
    codelist(a,b) = {code a, code b}
    
    OpenDebugModeForCheat = {Cheat("greedisgood 10000");Cheat("pointbreak");}
    
}

library Union
    
    function GetHandleId takes handle h returns integer
        return h
        return 0
    endfunction

    #define NULL = Condition(function TRUTH)

    constant function TRUTH takes nothing returns boolean
        return true
    endfunction

    function Enroll takes playerunitevent whichEvent, conditionfunc cond, code action returns nothing
        local trigger trig = CreateTrigger()
        local integer index = 0
        loop
            call TriggerRegisterPlayerUnitEvent(trig, Player(index), whichEvent, null)
            set index = index + 1
            exitwhen index == bj_MAX_PLAYER_SLOTS
        endloop
        call TriggerAddCondition(trig, cond)
        call TriggerAddAction(trig, action)
        set trig = null
    endfunction

endlibrary