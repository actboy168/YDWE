library_once TimerUtils
//*********************************************************************
//* TimerUtils (Purple flavor for 1.23b or later)
//* ----------
//*
//*  To implement it , create a custom text trigger called TimerUtils
//* and paste the contents of this script there.
//*
//*  To copy from a map to another, copy the trigger holding this
//* library to your map.
//*
//* (requires vJass)   More scripts: http://www.wc3c.net/
//*
//* For your timer needs:
//*  * Attaching
//*  * Recycling (with double-free protection)
//*
//* set t=NewTimer()      : Get a timer (alternative to CreateTimer)
//* ReleaseTimer(t)       : Relese a timer (alt to DestroyTimer)
//* SetTimerData(t,2)     : Attach value 2 to timer
//* GetTimerData(t)       : Get the timer's value.
//*                         You can assume a timer's value is 0
//*                         after NewTimer.
//*
//* Purple Flavor: Slower than the red flavor by a multiplication and a
//*             division, and as such faster than the blue flavor. Has
//*             a theoretical limit of timers, which is HASH_SIZE, but
//*             you should keep your timer count below 3/4 of that to
//*             insure good performance of the NewTimer function.
//*
//* Credits:  * Hash algorithm by Cohadar (used in an early version
//*             of his ABCT timer system)
//*
//*           * TimerUtils "interface" by Vexorian.
//*
//*           * This library by Iron_Doors.
//*
//********************************************************************

//================================================================

    globals
        private constant integer MAX_HANDLE_ID_COUNT = 408000
        // values lower than 8191: very fast, but very unsafe.
        // values bigger than 8191: not that fast, the bigger the number is the slower the function gets
        // Most maps don't really need a value bigger than 50000 here, but if you are unsure, leave it
        // as the rather inflated value of 408000
        private integer array data[MAX_HANDLE_ID_COUNT]
        private constant integer MIN_HANDLE_ID=0x100000
        //use a totally random number here, the more improbable someone uses it, the better.
        private constant integer HELD=0x28829022
        private timer array tT
        private integer tN = 0
    endglobals
    
    function GetHandleId takes handle h returns integer
        return h
        return 0
    endfunction

    //It is dependent on jasshelper's recent inlining optimization in order to perform correctly.
    function SetTimerData takes timer t, integer value returns nothing
        debug if(GetHandleId(t)-MIN_HANDLE_ID>=MAX_HANDLE_ID_COUNT) then
        debug     call BJDebugMsg("SetTimerData: Handle id too big, increase the max handle id count or use gamecache instead")
        debug endif
        set data[GetHandleId(t)-MIN_HANDLE_ID]=value
    endfunction

    function GetTimerData takes timer t returns integer
        debug if(GetHandleId(t)-MIN_HANDLE_ID>=MAX_HANDLE_ID_COUNT) then
        debug     call BJDebugMsg("GetTimerData: Handle id too big, increase the max handle id count or use gamecache instead")
        debug endif
        return data[GetHandleId(t)-MIN_HANDLE_ID]
    endfunction

    function NewTimer takes nothing returns timer
        if (tN==0) then
            set tT[0]=CreateTimer()
        else
            set tN=tN-1
        endif
        call SetTimerData(tT[tN],0)
     return tT[tN]
    endfunction

    function ReleaseTimer takes timer t returns nothing
        if(t==null) then
            debug call BJDebugMsg("Warning: attempt to release a null timer")
            return
        endif
        if (tN==8191) then
            debug call BJDebugMsg("Warning: Timer stack is full, destroying timer!!")
            //stack is full, the map already has much more troubles than the chance of bug
            call DestroyTimer(t)
        else
            call PauseTimer(t)
            if(GetTimerData(t)==HELD) then
                debug call BJDebugMsg("Warning: ReleaseTimer: Double free!")
                return
            endif
            call SetTimerData(t,HELD)
            set tT[tN]=t
            set tN=tN+1
        endif    
    endfunction

endlibrary