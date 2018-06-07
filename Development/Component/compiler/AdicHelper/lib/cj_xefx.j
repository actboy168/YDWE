//Edit By TigerCN
library xefx initializer init requires xebasic
//**************************************************
// xefx 0.6
// --------
//  Recommended: ARGB (adds ARGBrecolor method)
//  For your movable fx needs
//
//**************************************************

//==================================================
    globals
        private constant integer MAX_INSTANCES = 8190 //change accordingly.
        private constant real    RECYCLE_DELAY = 4.0
        //recycling, in order to show the effect correctly, must wait some time before
        //removing the unit.
        private timer    recycler
        private timer    NOW
    endglobals

   private struct recyclebin extends array
   
       unit u
       real schedule
       static recyclebin end=0
       static recyclebin begin=0

       static method Recycle takes nothing returns nothing
            call RemoveUnit(.begin.u) //this unit is private, systems shouldn't mess with it.
            set .begin.u=null
            set .begin=recyclebin(integer(.begin)+1)
            if(.begin==.end) then
                set .begin=0
                set .end=0
            else
                call TimerStart(recycler, .begin.schedule-TimerGetElapsed(NOW), false, function recyclebin.Recycle)
            endif
       endmethod
       
   endstruct

   private function init takes nothing returns nothing
       set recycler=CreateTimer()
       set NOW=CreateTimer()
       call TimerStart(NOW,43200,true,null)
   endfunction

   struct xefx[MAX_INSTANCES]
   
       public integer tag=0
       private unit   dummy
       private effect fx=null
       private real   zang=0.0
       private integer r=255
       private integer g=255
       private integer b=255
       private integer a=255
       private integer abil=0

       static method create takes real x, real y, real facing returns xefx
           local xefx this=xefx.allocate()
           set this.dummy= CreateUnit(Player(15), XE_DUMMY_UNITID, x,y, facing*bj_RADTODEG)
           call UnitAddAbility(this.dummy,XE_HEIGHT_ENABLER)
           call UnitAddAbility(this.dummy,'Aloc')
           call UnitRemoveAbility(this.dummy,XE_HEIGHT_ENABLER)
           call SetUnitX(this.dummy,x)
           call SetUnitY(this.dummy,y)
           return this
       endmethod

       method operator owner takes nothing returns player
           return GetOwningPlayer(this.dummy)
       endmethod

       method operator owner= takes player p returns nothing
           call SetUnitOwner(this.dummy,p,false)
       endmethod

       method operator teamcolor= takes playercolor c returns nothing
           call SetUnitColor(this.dummy,c)
       endmethod

       method operator scale= takes real value returns nothing
           call SetUnitScale(this.dummy,value,value,value)
       endmethod

       //! textmacro XEFX_colorstuff takes colorname, colorvar
       method operator $colorname$ takes nothing returns integer
           return this.$colorvar$
       endmethod
       
       method operator $colorname$= takes integer value returns nothing
           set this.$colorvar$=value
           call SetUnitVertexColor(this.dummy,this.r,this.g,this.b,this.a)
       endmethod
       //! endtextmacro
       //! runtextmacro XEFX_colorstuff("red","r")
       //! runtextmacro XEFX_colorstuff("green","g")
       //! runtextmacro XEFX_colorstuff("blue","b")
       //! runtextmacro XEFX_colorstuff("alpha","a")

       method recolor takes integer r, integer g , integer b, integer a returns nothing
           set this.r=r
           set this.g=g
           set this.b=b
           set this.a=a
           call SetUnitVertexColor(this.dummy,this.r,this.g,this.b,this.a)
       endmethod

       implement optional ARGBrecolor

       method operator abilityid takes nothing returns integer
           return this.abil
       endmethod

       method operator abilityid= takes integer a returns nothing
           if(this.abil!=0) then
               call UnitRemoveAbility(this.dummy,this.abil)
           endif
           if(a!=0) then
               call UnitAddAbility(this.dummy,a)
           endif
           set this.abil=a
       endmethod

       method flash takes string fx returns nothing
           call DestroyEffect(AddSpecialEffectTarget(fx,this.dummy,"origin"))
       endmethod

       method operator xyangle takes nothing returns real
           return GetUnitFacing(this.dummy)*bj_DEGTORAD
       endmethod
       
       method operator xyangle= takes real value returns nothing
           call SetUnitFacing(this.dummy,value*bj_RADTODEG)
       endmethod

       method operator zangle takes nothing returns real
           return this.zang
       endmethod

       method operator zangle= takes real value returns nothing
        local integer i=R2I(value*bj_RADTODEG+90.5)
           set this.zang=value
           if(i>=180) then
               set i=179
           elseif(i<0) then
               set i=0
           endif
           call SetUnitAnimationByIndex(this.dummy, i  )
       endmethod
       
       method operator x takes nothing returns real
           return GetUnitX(this.dummy)
       endmethod
       method operator y takes nothing returns real
           return GetUnitY(this.dummy)
       endmethod
       method operator z takes nothing returns real
           return GetUnitFlyHeight(this.dummy)
       endmethod

       method operator z= takes real value returns nothing
           call SetUnitFlyHeight(this.dummy,value,0)
       endmethod

       method operator x= takes real value returns nothing
           call SetUnitX(this.dummy,value)
       endmethod

       method operator y= takes real value returns nothing
           call SetUnitY(this.dummy,value)
       endmethod

       method operator fxpath= takes string newpath returns nothing
           if (this.fx!=null) then
               call DestroyEffect(this.fx)
           endif
           if (newpath=="") then
               set this.fx=null
           else
               set this.fx=AddSpecialEffectTarget(newpath,this.dummy,"origin")
           endif
       endmethod
 
       private method onDestroy takes nothing returns nothing
           if(this.abil!=0) then
               call UnitRemoveAbility(this.dummy,this.abil)
           endif
           if(this.fx!=null) then
               call DestroyEffect(this.fx)
               set this.fx=null
           endif
           if (recyclebin.end==MAX_INSTANCES) then
               call TimerStart(recycler,0,false,function recyclebin.Recycle)
               call ExplodeUnitBJ(this.dummy)
           else
               set recyclebin.end.u=this.dummy
               set recyclebin.end.schedule=TimerGetElapsed(NOW)+RECYCLE_DELAY
               set recyclebin.end= recyclebin( integer(recyclebin.end)+1)
               if( recyclebin.end==1) then
                   call TimerStart(recycler, RECYCLE_DELAY, false, function recyclebin.Recycle)
               endif
               call SetUnitOwner(this.dummy,Player(15),false)
           endif
           set this.dummy=null
       endmethod

   endstruct
   
endlibrary