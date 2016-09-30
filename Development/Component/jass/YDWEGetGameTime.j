#ifndef YDWEGetGameTimeIncluded
#define YDWEGetGameTimeIncluded

library YDWEGetGameTime initializer YDWEGetGameTimeInit

globals
    private timer t
endglobals

function YDWEGetGameTime takes nothing returns real
	return TimerGetElapsed(t)
endfunction

function YDWEGetGameCurrentTime takes nothing returns integer
	return R2I(TimerGetElapsed(t))
endfunction

function YDWEGetGameTimeInit takes nothing returns nothing
	set t = CreateTimer()
	call TimerStart(t, 1000000.0, false, null)
endfunction

endlibrary

#endif
