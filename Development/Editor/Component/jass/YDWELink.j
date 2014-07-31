#ifndef YDWELinkIncluded
#define YDWELinkIncluded

library Link 

    /*在两个单位间创造闪电链接并对他们同时造成持续伤害，当两个单位间的距离足够接近的时闪电消失。
    类似魔兽世界里某位Boss的技能，可以用在RPG图制作。*/

    //API: function LinkSetUp takes unit u1, unit u2, real dam, real dis, string lightn returns nothing

    globals 
        private integer dindex = -1
        private constant real period = 0.0312500
        private timer t = CreateTimer()
    endglobals
    
    native UnitAlive takes unit u returns boolean

    function disstance takes real x1,real y1,real x2,real y2 returns real
        return SquareRoot((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
    endfunction
    
    function aangle takes real x, real y, real x1, real y1 returns real
        //return bj_RADTODEG * Atan2(GetLocationY(locB) - GetLocationY(locA), GetLocationX(locB) - GetLocationX(locA))
        return bj_RADTODEG * Atan2(y1 - y, x1 - x)
    endfunction

    private struct Data
        real x1
        real y1
        real z1
        
        real x2
        real y2
        real z2
        
        real damage
        
        real dis
        
        unit u1
        unit u2

        string l
        
        lightning l1
        //lightning l2
        
        method destroy takes nothing returns nothing
            if dindex == -1 then
                call PauseTimer(t)
            endif
            call this.deallocate()
        endmethod
    endstruct
    
    globals 
        private Data array data
    endglobals
    
    private function Loop takes nothing returns nothing
        local integer i = 0
        local Data this 
        loop
            exitwhen i > dindex
            set this = data[i]
            set this.x1 = GetUnitX(this.u1)
            set this.y1 = GetUnitY(this.u1)
            set this.z1 = GetUnitZ(this.u1)
            set this.x2 = GetUnitX(this.u2)
            set this.y2 = GetUnitY(this.u2)
            set this.z2 = GetUnitZ(this.u2)
            if disstance(this.x1,this.y1,this.x2,this.y2) > this.dis and UnitAlive(this.u1) and UnitAlive(this.u2) then
                call MoveLightningEx(this.l,true,this.x1,this.y1,this.z1,this.x2,this.y2,this.z2)
                call UnitDamageTarget(this.u1, this.u2, this.damage * period, true, false, ATTACK_TYPE_CHAOS, DAMAGE_TYPE_DEMOLITION,null)
                call UnitDamageTarget(this.u2, this.u1, this.damage * period, true, false, ATTACK_TYPE_CHAOS, DAMAGE_TYPE_DEMOLITION,null)
            else
                set this.u1 = null
                set this.u2 = null
                call DestroyLightning(this.l1)
                set this.l1 = null
                set data[i] = data[dindex]
                set i = i - 1
                set dindex = dindex - 1
                call this.destroy()
            endif
            set i = i + 1
        endloop
    endfunction
    
    function LinkSetUp takes unit u1, unit u2, real dam, real dis, string lightn returns nothing
        local Data this = Data.create()
        set this.u1 = u1
        set this.u2 = u2
        set this.x1 = GetUnitX(this.u1)
        set this.y1 = GetUnitY(this.u1)
        set this.z1 = GetUnitZ(this.u1)
        set this.x2 = GetUnitX(this.u2)
        set this.y2 = GetUnitY(this.u2)
        set this.z2 = GetUnitZ(this.u2)
        set this.dis = dis
        set this.damage = dam
        set this.l = lightn
        set this.l1 = AddLightningEx(this.l,true,this.x1,this.y1,this.z1,this.x2,this.y2,this.z2)
        set dindex = dindex + 1
        set data[dindex] = this
        if dindex == 0 then
            call TimerStart(t, period, true, function Loop)
        endif
    endfunction
    
    
endlibrary

#endif /// YDWELinkIncluded