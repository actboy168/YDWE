library ARGB initializer init
//******************************************************************************
//*
//* ARGB 1.2
//* ====
//*  For your color needs.
//*  
//*  An ARGB object is a by-value struct, this means that assigning copies the
//* contents of the struct and that you don't have to use .destroy(), the
//* downside is that you cannot assign its members (can't do set c.r= 123 )
//*
//*  This library should have plenty of uses, for example, if your spell involves
//* some unit recoloring you can allow users to input the color in the config
//* section as 0xAARRGGBB and you can then use this to decode that stuff.
//*
//* You can also easily merge two colors and make fading effects using ARGB.mix
//*
//* There's ARGB.fromPlayer which gets an ARGB object containing the player's
//* color. Then you can use the previous utilities on it.
//*
//* The .str() instance method can recolor a string, and the recolorUnit method
//* will apply the ARGB on a unit
//*
//* For other uses, you can use the .red, .green, .blue and .alpha members to get
//* an ARGB object's color value (from 0 to 255).
//*
//* structs that have a recolor method that takes red,green,blue and alpha as 0.255
//* integers can implement the ARGBrecolor module to gain an ability to quickly
//* recolor using an ARGB object.
//*
//********************************************************************************

//=================================================================================
globals
    private string array i2cc
endglobals

//this double naming stuff is beginning to make me insane, if only TriggerEvaluate() wasn't so slow...
struct ARGB extends array
    static method create takes integer a, integer r, integer g, integer b returns ARGB
        return ARGB(b + g*0x100 + r*0x10000 + a*0x1000000)
    endmethod
    
    // not really part of the exported stuff, I may remove it in the future, so please don't call this textmacro
    //! textmacro ARGB_PLAYER_COLOR_2_ARGB
        if(pc==PLAYER_COLOR_RED) then
            return 0xFFFF0303
        elseif(pc==PLAYER_COLOR_BLUE) then
            return 0xFF0042FF
        elseif(pc==PLAYER_COLOR_CYAN) then
            return 0xFF1CE6B9
        elseif(pc==PLAYER_COLOR_PURPLE) then
            return 0xFF540081
        elseif(pc==PLAYER_COLOR_YELLOW) then
            return 0xFFFFFC01
        elseif(pc==PLAYER_COLOR_ORANGE) then
            return 0xFFFE8A0E
        elseif(pc==PLAYER_COLOR_GREEN) then
            return 0xFF20C000
        elseif(pc==PLAYER_COLOR_PINK) then
            return 0xFFE55BB0
        elseif(pc==PLAYER_COLOR_LIGHT_GRAY) then
            return 0xFF959697
        elseif(pc==PLAYER_COLOR_LIGHT_BLUE) then
            return 0xFF7EBFF1
        elseif(pc==PLAYER_COLOR_AQUA) then
            return 0xFF106246
        elseif(pc==PLAYER_COLOR_BROWN) then
            return 0xFF4E2A04
        endif
        
      return 0xFF111111
    //! endtextmacro
    static method fromPlayerColor takes playercolor pc returns ARGB
        //! runtextmacro ARGB_PLAYER_COLOR_2_ARGB()
    endmethod

    static method fromPlayer takes player p returns ARGB
     local playercolor pc=GetPlayerColor(p)
        //! runtextmacro ARGB_PLAYER_COLOR_2_ARGB()
    endmethod

    method operator alpha takes nothing returns integer
        if( integer(this) <0) then
            return 0x80+(-(-integer(this)+0x80000000))/0x1000000
        else
            return (integer(this))/0x1000000
        endif
    endmethod
    method operator alpha= takes integer na returns ARGB
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       return ARGB(b + g*0x100 + r*0x10000 + na*0x1000000) 
    endmethod




    method operator red takes nothing returns integer
     local integer c=integer(this)*0x100
        if(c<0) then
            return 0x80+(-(-c+0x80000000))/0x1000000
        else
            return c/0x1000000
        endif
    endmethod
    method operator red= takes integer nr returns ARGB
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       return ARGB(b + g*0x100 + nr*0x10000 + a*0x1000000) 
    endmethod

    method operator green takes nothing returns integer
     local integer c=integer(this)*0x10000
        if(c<0) then
            return 0x80+(-(-c+0x80000000))/0x1000000
        else
            return c/0x1000000
        endif
    endmethod
    method operator green= takes integer ng returns ARGB
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       return ARGB(b + ng*0x100 + r*0x10000 + a*0x1000000) 
    endmethod

    //=======================================================
    //
    //
    method operator blue takes nothing returns integer
     local integer c=integer(this)*0x1000000
        if(c<0) then
            return 0x80+(-(-c+0x80000000))/0x1000000
        else
            return c/0x1000000
        endif
    endmethod
    method operator blue= takes integer nb returns ARGB
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       return ARGB(nb + g*0x100 + r*0x10000 + a*0x1000000) 
    endmethod

    //====================================================================
    // Mixes two colors, s would be a number 0<=s<=1 that determines
    // the weight given to color c2.
    //
    //  mix(c1,c2,0)   = c1
    //  mix(c1,c2,1)   = c2
    //  mix(c1,c2,0.5) = Mixing the colors c1 and c2 in equal proportions.
    //
    static method mix takes ARGB c1, ARGB c2, real s returns ARGB
      //widest function ever
      return ARGB( R2I(c2.blue*s+c1.blue*(1-s)+0.5) + R2I(c2.green*s+c1.green*(1-s)+0.5)*0x100 + R2I(c2.red*s+c1.red*(1-s)+0.5)*0x10000 + R2I(c2.alpha*s+c1.alpha*(1-s)+0.5)*0x1000000)
    endmethod

    method str takes string s returns string
       return "|c"+i2cc[.alpha]+i2cc[.red]+i2cc[.green]+i2cc[.blue]+s+"|r"
    endmethod

    method recolorUnit takes unit u returns nothing
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       call SetUnitVertexColor(u,r,g,b,a)
    endmethod

endstruct

module ARGBrecolor
    method ARGBrecolor takes ARGB color returns nothing
     local integer a
     local integer r
     local integer g
     local integer b
     local integer col=integer(this)

       if (col<0) then
           set col=-(-col+0x80000000)
           set a=0x80+col/0x1000000
           set col=col-(a-0x80)*0x1000000
       else
           set a=col/0x1000000
           set col=col-a*0x1000000
       endif
       set r=col/0x10000
       set col=col-r*0x10000
       set g=col/0x100
       set b=col-g*0x100
       
       call this.recolor(r, g , b, a)
    endmethod

endmodule

private function init takes nothing returns nothing
 local integer i=0

    // Don't run textmacros you don't own!
    //! textmacro ARGB_CHAR takes int, chr
        set i=0
        loop
            exitwhen i==16
            set i2cc[$int$*16+i]="$chr$"+i2cc[$int$*16+i]
            set i2cc[i*16+$int$]=i2cc[i*16+$int$]+"$chr$"
            set i=i+1
        endloop
    //! endtextmacro
    //! runtextmacro ARGB_CHAR( "0","0")
    //! runtextmacro ARGB_CHAR( "1","1")
    //! runtextmacro ARGB_CHAR( "2","2")
    //! runtextmacro ARGB_CHAR( "3","3")
    //! runtextmacro ARGB_CHAR( "4","4")
    //! runtextmacro ARGB_CHAR( "5","5")
    //! runtextmacro ARGB_CHAR( "6","6")
    //! runtextmacro ARGB_CHAR( "7","7")
    //! runtextmacro ARGB_CHAR( "8","8")
    //! runtextmacro ARGB_CHAR( "9","9")
    //! runtextmacro ARGB_CHAR("10","A")
    //! runtextmacro ARGB_CHAR("11","B")
    //! runtextmacro ARGB_CHAR("12","C")
    //! runtextmacro ARGB_CHAR("13","D")
    //! runtextmacro ARGB_CHAR("14","E")
    //! runtextmacro ARGB_CHAR("15","F")
endfunction

endlibrary


