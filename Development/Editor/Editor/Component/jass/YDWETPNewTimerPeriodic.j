#ifndef YDWETPNewTimerPeriodicIncluded
#define YDWETPNewTimerPeriodicIncluded

#include "YDWEBase.j"

library YDWETimerPeriodic requires YDWEBase

    globals
        private constant timer TIMER = CreateTimer()
        private constant real  INTER = 0.01
    endglobals

    globals
        private integer array Structs
        private integer Total = 0
        private integer Index = 0
    endglobals

    private struct TimerData
        trigger trg   = null
        real    dur   = 0.
        real    inter = 0.
        boolean perio = false
        boolean pause = false
    endstruct

    private function Spin takes nothing returns nothing
        local TimerData data = 0
        local integer   inst = 0
        loop
            exitwhen inst == Total
            set data = inst:Structs
            set data.dur = data.dur + INTER
            if data.dur == data.inter or data.dur > data.inter then
                set Index = integer(data)
                if not data.pause then
                    call TriggerExecute(data.trg)
                endif
                if data.perio then
                    set data.dur = 0.
                else
                    set data.trg = null
                    call data.destroy()
                    
                    set Total = Total - 1
                    set inst:Structs = Total:Structs
                    set inst = inst - 1
                endif
                set Index = 0
            endif
            set inst = inst + 1
        endloop
        if Total == 0 then
            call PauseTimer(TIMER)
        endif
    endfunction
    
    // 获取计时器：逝去时间、剩余时间、设置的时间    
    function YDWE_TP_GetElapsedTime takes TimerData data returns real
        return data.dur
    endfunction
    
    function YDWE_TP_GetRemainingTime takes TimerData data returns real
        return data.inter - data.dur
    endfunction
    
    function YDWE_TP_GetTimeout takes TimerData data returns real
        return data.inter
    endfunction 
    
    function YDWE_TP_PauseTimerPeriodic takes TimerData data returns nothing
        set data.perio = false
        set data.pause = true
        set data.inter = 0.
    endfunction

    function YDWE_TP_SetTimeInterval takes TimerData data, real interval returns nothing
        if data.perio then
            set data.inter = interval
        endif
    endfunction

    function YDWE_TP_GetExpiredTimerIndex takes nothing returns TimerData
        return Index
    endfunction  

    function YDWE_TP_NewTimerPeriodic takes real timeout, boolean periodic, trigger trig returns TimerData
        local TimerData data = TimerData.create()
        set data.inter = timeout
        set data.perio = periodic
        set data.trg   = trig
        set Total:Structs = data
        set Total = Total + 1 
        if Total - 1 == 0 then
            call TimerStart(TIMER, INTER, true, function Spin)
        endif
        //call BJDebugMsg(I2S(Total))
        return data
    endfunction

endlibrary 



#endif /// YDWETPNewTimerPeriodicIncluded
