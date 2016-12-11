#ifndef YDWETimerPatternIncluded
#define YDWETimerPatternIncluded

#include "YDWEBase.j"

library_once YDWETimerPattern initializer Init requires YDWEBase

//***************************************************
//* ∑ - Matrix 万能模板函数
//*--------------------
//* 作者：Warft_TigerCN  代码优化：Fetrix_sai
//***************************************************

    #define TIMER_PATTERN_RADIUS 120.0

    private keyword Thread

    globals
        private boolexpr Bexpr = null
        private rect Area = null
        private Thread tmp_data
        private location yd_loc = Location(0.0, 0.0)
    endglobals

    private struct YDVector3
        real x
        real y
        real z
    endstruct

    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //                                       Timer Pattern Union                                              //
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    private function SingleMagic takes unit sour, unit targ, real x, real y, real h, integer uid, integer aid, integer lv, integer order returns nothing
        local unit dummy = CreateUnit(GetOwningPlayer(sour), uid, x, y, GetUnitFacing(sour))
        call UnitApplyTimedLife(dummy, 'BHwe', 1.0)
        call UnitAddAbility(dummy, aid)
        call SetUnitAbilityLevel(dummy, aid, lv)
        call SetUnitFlyHeight(dummy, h, 0.0)
        call IssueTargetOrderById(dummy, order, targ)
        //debug call BJDebugMsg("Target order")
        set dummy = null
    endfunction

    private function GetUnitZ takes unit u returns real
        call MoveLocation(yd_loc, GetUnitX(u), GetUnitY(u))
        return GetUnitFlyHeight(u) + GetLocationZ(yd_loc)
    endfunction

    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //                                            Filter Funcs                                                //
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    private function EnemyFilter takes unit u, unit caster returns boolean
        return IsUnitType(u, UNIT_TYPE_MAGIC_IMMUNE) == false and IsUnitType(u, UNIT_TYPE_RESISTANT) == false /*
        */ and IsUnitType(u, UNIT_TYPE_SLEEPING) == false     and GetUnitState(u, UNIT_STATE_LIFE) > 0.405    /*
        */ and IsUnitType(u, UNIT_TYPE_STRUCTURE) == false    and IsUnitIllusion(u) == false                  /*
        */ and IsUnitHidden(u) == false                       and IsUnitEnemy(u, GetOwningPlayer(caster))     /*
        */ and IsUnitVisible(u, GetOwningPlayer(caster))
    endfunction

    private function TreeFilter takes nothing returns boolean
        local integer id = GetDestructableTypeId(GetFilterDestructable())
        return id == 'LTlt' or id == 'ATtr' or id == 'BTtw' or id == 'KTtw' /*
          */or id == 'YTft' or id == 'JTct' or id == 'YTst' or id == 'YTct' /*
          */or id == 'YTwt' or id == 'JTtw' or id == 'DTsh' or id == 'FTtw' /*
          */or id == 'CTtr' or id == 'ITtw' or id == 'NTtw' or id == 'OTtw' /*
          */or id == 'ZTtw' or id == 'WTst' or id == 'GTsh' or id == 'VTlt' /*
          */or id == 'WTtw' or id == 'ATtc' or id == 'BTtc' or id == 'CTtc' /*
          */or id == 'ITtc' or id == 'NTtc' or id == 'ZTtc'
    endfunction

    private function DamageFilter takes nothing returns boolean
        local unit   u = GetFilterUnit()
        local Thread d = tmp_data
        if not(IsUnitInGroup(u, d.g)) and d.switch != 0 and EnemyFilter(u, d.caster) then
            call UnitDamageTarget(d.caster, u, d.amount, true, true, bj_lastSetAttackType, bj_lastSetDamageType, bj_lastSetWeaponType)
            call DestroyEffect(AddSpecialEffectTarget(d.dsfx, u, d.part))
            if d.skills > '0000' and d.skills != null and d.order > 0 and d.order != null then
                call SingleMagic(d.caster, u, d.pos.x, d.pos.y, GetUnitFlyHeight(d.obj), d.unitid, d.skills, d.level, d.order)
            endif
            if not(d.recycle) then
                //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r A one-time.")
                set d.switch = 0
            endif
            set d.target = u
            //call GroupAddUnit(d.g, u)
            set u = null
            return true
        endif
        set u = null
        return false
    endfunction

    private function TreeKill takes nothing returns nothing
        local destructable d = GetEnumDestructable()
        if GetWidgetLife(d) > 0.405 then
            call KillDestructable(d)
        endif
        set d = null
    endfunction

    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    //                                         Major Structure Code                                           //
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    private struct Thread
        YDVector3 des //destination
        YDVector3 pos //position
        YDVector3 vel //velocity
        unit caster
        unit source
        unit target
        unit obj
        real ac
        real bc
        real dist
        real step
        real amount
        integer switch
        integer follow
        integer unitid
        integer skills
        integer order
        integer level
        string dsfx
        string gsfx
        string wsfx
        string part
        boolean recycle
        boolean killdest
        boolean volume
        group g
        timer t

        static method operator [] takes handle h returns thistype
            return YDWEGetIntegerByString("YDWETimerPattern.", I2S(YDWEH2I(h)))
        endmethod

        static method operator []= takes handle h, thistype value returns nothing
            call YDWESaveIntegerByString("YDWETimerPattern.", I2S(YDWEH2I(h)), value)
        endmethod

        static method flush takes handle h returns nothing
            call YDWEFlushStoredIntegerByString("YDWETimerPattern.", I2S(YDWEH2I(h)))
        endmethod

        method operator x= takes real value returns nothing
            set .pos.x = value
            call SetUnitX(.obj, value)
        endmethod

        method operator y= takes real value returns nothing
            set .pos.y = value
            call SetUnitY(.obj, value)
        endmethod

        method operator z= takes real value returns nothing
            set .pos.z = value
            call MoveLocation(yd_loc, .pos.x, .pos.y)
            call SetUnitFlyHeight(.obj, value - GetLocationZ(yd_loc), 0)
        endmethod

        method onDestroy takes nothing returns nothing
            //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r Knockback stopped!")
            call Thread.flush(.obj)
            call Thread.flush(.t)
            call GroupClear(.g)
            call DestroyGroup(.g)
            call PauseTimer(.t)
            call DestroyTimer(.t)
            call .des.destroy()
            call .pos.destroy()
            call .vel.destroy()
            set .caster = null
            set .target = null
            set .obj = null
            set .g = null
            set .t = null
            set .amount = 0
            set .skills = 0
            set .order = 0
            set .dsfx = ""
            set .gsfx = ""
            set .wsfx = ""
            set .part = ""
        endmethod
    endstruct

    private struct Parabola extends Thread

        static method move takes nothing returns nothing
            local thistype this = Thread[GetExpiredTimer()]
            //local real vx = .des.x - .pos.x
            //local real vy = .des.y - .pos.y
            //local real vz = .des.z - .pos.z
            //if vx * vx + vy * vy + vz * vz > 900.0 then
                set .x = GetUnitX(.obj) + .vel.x //.pos.x + .vel.x
                set .y = GetUnitY(.obj) + .vel.y //.pos.y + .vel.y
                set .z = GetUnitZ(.obj) + .ac * .step * 2 + .ac * .dist + .bc //.pos.z + .ac * .step * 2 + .ac * .dist + .bc
                set .step = .step + .dist
                //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r high = ." + R2S(GetLocationZ(yd_loc)))
                if YDWECoordinateX(.pos.x) != .pos.x or YDWECoordinateY(.pos.y) != .pos.y or .pos.z <= GetLocationZ(yd_loc) then
                    set .switch = 0
                endif
                if .amount > 0.0 then
                    //call this.damage(.caster, .pos.x + .vel.x, .pos.y + .vel.y, GetUnitZ(.obj), false, false)
                    set tmp_data = integer(this)
                    call GroupEnumUnitsInRange(.g, .pos.x + .vel.x, .pos.y + .vel.y, TIMER_PATTERN_RADIUS, function DamageFilter)
                    //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r Area damage.")
                endif
            //else
                //set .switch = 0
            //endif

            if .switch == 0 then
                call SetUnitFlyHeight(.obj, GetUnitDefaultFlyHeight(.obj), 200.0)
                call SetUnitTimeScale(.obj, 1)
                //YDWETriggerEvent
                call YDWESyStemAbilityCastingOverTriggerAction(.obj, 7)
                call this.destroy()
            endif
        endmethod

        static method create takes unit source, unit object, real angle, real distance, real time, real interval, real high, real damage, string attach, string deff returns thistype
            local thistype this = thistype.allocate()
            local real vx = 0.0
            local real vy = 0.0
            local real vz = 0.0
            set .des = YDVector3.create()
            set .pos = YDVector3.create()
            set .vel = YDVector3.create()
            set .pos.x = GetUnitX(object)
            set .pos.y = GetUnitY(object)
            set .pos.z = GetUnitZ(object)
            set .des.x = .pos.x + distance * Cos(angle)
            set .des.y = .pos.y + distance * Sin(angle)
            call MoveLocation(yd_loc, .des.x, .des.y)
            set .des.z = GetLocationZ(yd_loc)
            if .pos.z > .des.z then
                set high = high + .pos.z
            else
                set high = high + .des.z
            endif
            set .ac = (2 * (.pos.z + .des.z) - 4 * high) / (distance * distance)
            set .bc = (.des.z - .pos.z - .ac * distance * distance) / distance
            set .dist = distance * interval / time
            set .ac = .ac * .dist
            set .bc = .bc * .dist
            set .vel.x = .dist * Cos(angle)
            set .vel.y = .dist * Sin(angle)
            set .step = 0.0
            set .caster = source
            set .obj = object
            set .amount = damage
            set .dsfx = deff
            set .part = attach
            set .switch = 1
            set .recycle = true
            set .t = CreateTimer()
            set .g = CreateGroup()
            call UnitAddAbility(.obj, 'Amrf')
            call UnitRemoveAbility(.obj, 'Amrf')
            call TimerStart(.t, interval, true, function thistype.move)
            call GroupAddUnit(.g, object)
            set Thread[object] = integer(this)
            set Thread[.t] = integer(this)
            return this
        endmethod

    endstruct

    // uniform speed
    private struct Linear extends Thread

        static method move takes nothing returns nothing
            local thistype this = Thread[GetExpiredTimer()]
            if .step > .dist then
                set .x = GetUnitX(.obj) + .vel.x //.pos.x + .vel.x
                set .y = GetUnitY(.obj) + .vel.y //.pos.y + .vel.y
                //set .pos.z = GetUnitZ(.obj)
                set .step = .step - .dist
                //call this.damage(.caster, .pos.x, .pos.y, .pos.z, true, true)
                set tmp_data = integer(this)
                call GroupEnumUnitsInRange(.g, .pos.x + .vel.x, .pos.y + .vel.y, TIMER_PATTERN_RADIUS, function DamageFilter)
                if YDWECoordinateX(.pos.x) != .pos.x or YDWECoordinateY(.pos.y) != .pos.y then
                    set .switch = 0
                endif
            else
                set .switch = 0
            endif

            if .switch == 0 then
                // YDWETriggerEvent
                if .target != null then
                    //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r  |cffff0000" + GetUnitName(.target) + "|r was hit!!!")
                    //call YDWESaveUnitByString(I2S(YDWEH2I(.caster)), "MoonPriestessArrow", .target)
                    set bj_lastAbilityTargetUnit = .target
                    call YDWESyStemAbilityCastingOverTriggerAction(.caster, 8)
                else
                    call YDWESyStemAbilityCastingOverTriggerAction(.caster, 9)
                endif
                //call KillUnit(.obj)
                call RemoveUnit(.obj)
                call this.destroy()
            endif
        endmethod

        static method create takes unit source, unit object, real angle, real distance, real time, real interval, integer uid, integer aid, integer lv, integer orderid, string attach, string sfx returns thistype
            local thistype this = thistype.allocate()
            set .des = YDVector3.create()
            set .pos = YDVector3.create()
            set .vel = YDVector3.create()
            set .step = distance
            set .dist = distance * interval / time
            set .vel.x = .dist * Cos(angle)
            set .vel.y = .dist * Sin(angle)
            set .pos.x = GetUnitX(object)
            set .pos.y = GetUnitY(object)
            set .caster = source
            set .obj = object
            set .unitid = uid
            set .skills = aid
            set .level = lv
            set .order = orderid
            set .part = attach
            set .gsfx = sfx
            set .switch = 1
            set .recycle = false
            set .t = CreateTimer()
            set .g = CreateGroup()
            call TimerStart(.t, interval, true, function thistype.move)
            set Thread[.t] = integer(this)
            return this
        endmethod

    endstruct

    // Uniform deceleration
    private struct Deceleration extends Thread

        static method move takes nothing returns nothing
            local thistype this = Thread[GetExpiredTimer()]
            local real xp = GetUnitX(.obj) + .dist * .vel.x
            local real yp = GetUnitY(.obj) + .dist * .vel.y
            if .volume == false then
                //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |rPathable without terrain.")
                if IsTerrainPathable(xp, yp, PATHING_TYPE_WALKABILITY) then
                    set .switch = 0
                else
                    set .x = xp
                    set .y = yp
                endif
            else
                set .x = xp
                set .y = yp
            endif
            if .follow == 0 then
                if GetUnitFlyHeight(.obj) < 5. then
                    if IsTerrainPathable(.pos.x, .pos.y, PATHING_TYPE_FLOATABILITY) then
                        call DestroyEffect(AddSpecialEffect(.gsfx, .pos.x, .pos.y))
                    else
                        call DestroyEffect(AddSpecialEffect(.wsfx, .pos.x, .pos.y))
                    endif
                endif
            endif
            set .follow = .follow + 1
            if .follow == 2 then
                set .follow = 0
            endif
            if .killdest then
                call MoveRectTo(Area, .pos.x, .pos.y)
                call EnumDestructablesInRect(Area, Bexpr, function TreeKill)
            endif
            if .amount > 0.0 then
                //call this.damage(.caster, .pos.x, .pos.y, 0.0, false, .recycle)
                set tmp_data = integer(this)
                call GroupEnumUnitsInRange(.g, .pos.x, .pos.y, TIMER_PATTERN_RADIUS, function DamageFilter)
            endif
            set .dist = .dist - .step
            if .dist <= 0.0 or YDWECoordinateX(.pos.x) != .pos.x or YDWECoordinateY(.pos.y) != .pos.y then
                set .switch = 0
            endif

            if .switch == 0 then
                call SetUnitFlyHeight(.obj, GetUnitDefaultFlyHeight(.obj), 200.0)
                call SetUnitTimeScale(.obj, 1)
                // YDWETriggerEvent
                call YDWESyStemAbilityCastingOverTriggerAction(.obj, 6)
                call this.destroy()
            endif
        endmethod

        static method create takes unit source, unit object, real angle, real distance, real time, real interval, real damage, boolean killtrees, boolean cycle, boolean path, string part, string geff, string weff returns thistype
            local thistype this = thistype.allocate() //thistype(Thread[object])
            local real vx = 0.0
            local real vy = 0.0
            local real l  = 0.0
            set .des = YDVector3.create()
            set .pos = YDVector3.create()
            set .vel = YDVector3.create()
            set .vel.x = Cos(angle)
            set .vel.y = Sin(angle)
            set .dist  = 2 * distance * interval / time
            set .step  = .dist * interval / time
            set .pos.x = GetUnitX(object)
            set .pos.y = GetUnitY(object)
            set .caster = source
            set .obj = object
            set .amount = damage
            set .killdest = killtrees
            set .recycle = cycle
            set .volume = path
            set .gsfx = geff
            set .wsfx = weff
            set .switch = 1
            set .follow = 0
            set .g = CreateGroup()
            set .t = CreateTimer()
            call TimerStart(.t, interval, true, function thistype.move)
            set Thread[.t] = integer(this)
            return this
        endmethod
    endstruct


    // Jump Attack PUI
    function YDWETimerPatternJumpAttack takes unit u, real face, real dis, real lasttime, real timeout, real high, real damage, string part, string dsfx returns nothing
        if u == null then
            //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r No object!")
            return
        endif
        call Parabola.create(u, u, Deg2Rad(face), RMaxBJ(dis, 0), RMaxBJ(lasttime, 0), RMaxBJ(timeout, 0), high, damage, part, dsfx)
    endfunction

    // Moon Priestess Arrow PUI
    function YDWETimerPatternMoonPriestessArrow takes unit u, real face, real dis, real lasttime, real timeout, integer lv, integer aid, integer uid, string order, string part, string dsfx returns nothing
        local unit sour = null
        if u == null then
            //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r No object!")
            return
        endif
        set sour = YDWEGetUnitByString(I2S(YDWEH2I(u)), "MoonPriestessArrow")
        if sour == null then
            set sour = u
        endif
        call Linear.create(sour, u, Deg2Rad(face), RMaxBJ(dis, 0), RMaxBJ(lasttime, 0), RMaxBJ(timeout, 0), uid, aid, IMaxBJ(lv, 1), OrderId(order), part, dsfx)
        //call YDWEFlushMissionByString(I2S(YDWEH2I(u)))
        set sour = null
    endfunction

    // Rush Slide PUI
    function YDWETimerPatternRushSlide takes unit u, real face, real dis, real lasttime, real timeout, real damage, boolean killtrees, boolean cycle, boolean path, string part, string gsfx, string wsfx returns nothing
        if u == null then
            //debug call BJDebugMsg("|cff00ff00[YDWE] Timer Pattern : |r No object!")
            return
        endif
        call Deceleration.create(u, u, Deg2Rad(face), RMaxBJ(dis, 0), RMaxBJ(lasttime, 0), RMaxBJ(timeout, 0), damage, killtrees, cycle, path, part, gsfx, wsfx)
    endfunction

    private function Init takes nothing returns nothing
        set Area  = Rect(-TIMER_PATTERN_RADIUS, -TIMER_PATTERN_RADIUS, TIMER_PATTERN_RADIUS, TIMER_PATTERN_RADIUS)
        set Bexpr = Filter(function TreeFilter)
    endfunction

	#undef TIMER_PATTERN_RADIUS
endlibrary

#endif /// YDWETimerPatternIncluded
