#ifndef YDWETimerSystemIncluded
#define YDWETimerSystemIncluded

#include <YDTrigger/ImportSaveLoadSystem.h>

//===========================================================================
//系统-TimerSystem
//===========================================================================

library YDWETimerSystem initializer Init requires YDTriggerSaveLoadSystem

globals
	private integer CurrentTime
	private integer CurrentIndex
    private integer TaskListHead
    private integer TaskListIdleHead
    private integer TaskListIdleMax
    private integer  array TaskListIdle
    private integer  array TaskListNext
    private integer  array TaskListTime
    private trigger  array TaskListProc  //函数组
    private trigger  fnRemoveUnit        //移除单位函数
    private trigger  fnDestroyTimer      //摧毁计时器
    private trigger  fnRemoveItem        //移除物品
    private trigger  fnDestroyEffect     //删除特效
    private trigger  fnDestroyLightning  //删除删掉特效
    private trigger  fnRunTrigger        //运行触发器
    private timer    Timer                //最小时间计时器  系统计时器  用于一些需要精确计时的功能
    private integer  TimerHandle

	private integer TimerSystem_RunIndex = 0
endglobals

private function NewTaskIndex takes nothing returns integer
	local integer h = TaskListIdleHead
	if TaskListIdleHead < 0 then
		if TaskListIdleMax >= 8000 then
			debug call BJDebugMsg("中心计时器任务队列溢出！")
			return 8100
		else
			set TaskListIdleMax = TaskListIdleMax + 1
			return TaskListIdleMax
		endif
	endif
	set TaskListIdleHead = TaskListIdle[h]
	return h
endfunction

private function DeleteTaskIndex takes integer index returns nothing
	set TaskListIdle[index] = TaskListIdleHead
	set TaskListIdleHead = index
endfunction

//该函数序列处理
private function NewTask takes real time, trigger proc returns integer
	local integer index = NewTaskIndex()
	local integer h = TaskListHead
	local integer t = R2I(100.*time) + CurrentTime
	local integer p

	set TaskListProc[index] = proc
	set TaskListTime[index] = t
	loop
		set p = TaskListNext[h]
		if p < 0 or TaskListTime[p] >= t then
		//	call BJDebugMsg("NewTask:"+I2S(index))
			set TaskListNext[h] = index
			set TaskListNext[index] = p
			return index
		endif
		set h = p
	endloop
	return index
endfunction

function YDWETimerSystemNewTask takes real time, trigger proc returns integer
	return NewTask(time, proc)
endfunction
function YDWETimerSystemGetCurrentTask takes nothing returns integer
	return CurrentIndex
endfunction

//删除单位
private function RemoveUnit_CallBack takes nothing returns nothing
    call RemoveUnit(YDTriggerGet(unit, TimerHandle, CurrentIndex))
    call YDTriggerClear(unit, TimerHandle, CurrentIndex)
endfunction

function YDWETimerRemoveUnit takes real time, unit u returns nothing
    call YDTriggerSet(unit, TimerHandle, NewTask(time, fnRemoveUnit), u)
endfunction

//摧毁计时器
private function DestroyTimer_CallBack takes nothing returns nothing
    call DestroyTimer(YDTriggerGet(timer, TimerHandle, CurrentIndex))
    call YDTriggerClear(timer, TimerHandle, CurrentIndex)
endfunction

function YDWETimerDestroyTimer takes real time, timer t returns nothing
    call YDTriggerSet(timer, TimerHandle, NewTask(time, fnDestroyTimer), t)
endfunction

//删除物品
private function RemoveItem_CallBack takes nothing returns nothing
    call RemoveItem(YDTriggerGet(item, TimerHandle, CurrentIndex))
    call YDTriggerClear(item, TimerHandle, CurrentIndex)
endfunction

function YDWETimerRemoveItem takes real time, item it returns nothing
    call YDTriggerSet(item, TimerHandle, NewTask(time, fnRemoveItem), it)
endfunction

//删除特效
private function DestroyEffect_CallBack takes nothing returns nothing
    call DestroyEffect(YDTriggerGet(effect, TimerHandle, CurrentIndex))
    call YDTriggerClear(effect, TimerHandle, CurrentIndex)
endfunction

function YDWETimerDestroyEffect takes real time, effect e returns nothing
    call YDTriggerSet(effect, TimerHandle, NewTask(time, fnDestroyEffect), e)
endfunction

//删除闪电特效
private function DestroyLightning_CallBack takes nothing returns nothing
    call DestroyLightning(YDTriggerGet(lightning, TimerHandle, CurrentIndex))
    call YDTriggerClear(lightning, TimerHandle, CurrentIndex)
endfunction

function YDWETimerDestroyLightning takes real time, lightning lt returns nothing
	local integer i = NewTask(time, fnDestroyLightning)
    call YDTriggerSet(lightning, TimerHandle, i, lt)
endfunction

//运行触发器
private function RunTrigger_CallBack takes nothing returns nothing
    call TriggerExecute(YDTriggerGet(trigger, TimerHandle, CurrentIndex))
    call YDTriggerClear(trigger, TimerHandle, CurrentIndex)
endfunction

function YDWETimerRunTrigger takes real time, trigger trg returns nothing
    call YDTriggerSet(trigger, TimerHandle, NewTask(time, fnRunTrigger), trg)
endfunction

//删除漂浮文字
function YDWETimerDestroyTextTag takes real time, texttag tt returns nothing
    local integer N=0
    local integer i=0
    if time <= 0 then
        set time = 0.01
    endif
    call SetTextTagPermanent(tt,false)
    call SetTextTagLifespan(tt,time)
    call SetTextTagFadepoint(tt,time)
endfunction

//中心计时器主函数
private function Main takes nothing returns nothing
	local integer h = TaskListHead
	local integer p
	loop
		set CurrentIndex = TaskListNext[h]
		exitwhen CurrentIndex < 0 or CurrentTime < TaskListTime[CurrentIndex]
		//call BJDebugMsg("Task:"+I2S(CurrentIndex))
		call TriggerEvaluate(TaskListProc[CurrentIndex])
		call DeleteTaskIndex(CurrentIndex)
		set TaskListNext[h] = TaskListNext[CurrentIndex]
	endloop
	set CurrentTime = CurrentTime + 1
endfunction


//初始化函数
private function Init takes nothing returns nothing
    set Timer = CreateTimer()
	set TimerHandle	= YDTriggerAny2I(timer, Timer)
	set CurrentTime      = 0
	set TaskListHead     = 0
	set TaskListNext[0]  = -1
	set TaskListIdleHead = 1
	set TaskListIdleMax  = 1
	set TaskListIdle[1]  = -1
	
	set fnRemoveUnit       = CreateTrigger()
	set fnDestroyTimer     = CreateTrigger()
	set fnRemoveItem       = CreateTrigger()
	set fnDestroyEffect    = CreateTrigger()
	set fnDestroyLightning = CreateTrigger()
	set fnRunTrigger       = CreateTrigger()
	call TriggerAddCondition(fnRemoveUnit,        Condition(function RemoveUnit_CallBack))
	call TriggerAddCondition(fnDestroyTimer,      Condition(function DestroyTimer_CallBack))
	call TriggerAddCondition(fnRemoveItem,        Condition(function RemoveItem_CallBack))
	call TriggerAddCondition(fnDestroyEffect,     Condition(function DestroyEffect_CallBack))
	call TriggerAddCondition(fnDestroyLightning,  Condition(function DestroyLightning_CallBack))
	call TriggerAddCondition(fnRunTrigger,        Condition(function RunTrigger_CallBack))
	
    call TimerStart(Timer, 0.01, true, function Main)
endfunction

//循环类仍用独立计时器
function YDWETimerSystemGetRunIndex takes nothing returns integer
    return TimerSystem_RunIndex
endfunction

#define INDEX_TRIGGER  $D0001
#define INDEX_RUNINDEX $D0002
#define INDEX_TIMES    $D0003

private function RunPeriodicTriggerFunction takes nothing returns nothing
    local integer tid = YDTriggerAny2I(timer, GetExpiredTimer())
    local trigger trg = YDTriggerGet(trigger, tid, INDEX_TRIGGER)
	call YDTriggerSetByString(integer, I2S(YDTriggerAny2I(trigger, trg)), "RunIndex", YDTriggerGet(integer, tid, INDEX_RUNINDEX))
    if TriggerEvaluate(trg) then
        call TriggerExecute(trg)
    endif
    set trg = null
endfunction

private function RunPeriodicTriggerFunctionByTimes takes nothing returns nothing
    local integer tid = YDTriggerAny2I(timer, GetExpiredTimer())
    local trigger trg = YDTriggerGet(trigger, tid, INDEX_TRIGGER)
    local integer times = YDTriggerGet(integer, tid, INDEX_TIMES)
	call YDTriggerSetByString(integer, I2S(YDTriggerAny2I(trigger, trg)), "RunIndex", YDTriggerGet(integer, tid, INDEX_RUNINDEX))
    if TriggerEvaluate(trg) then
        call TriggerExecute(trg)
    endif
    set times = times - 1
    if times > 0 then
		call YDTriggerSet(integer, tid, INDEX_TIMES, times)
      else
        call DestroyTimer(GetExpiredTimer())
        call YDTriggerClearTable(tid)
    endif
    set trg = null
endfunction

function YDWETimerRunPeriodicTrigger takes real timeout, trigger trg, boolean b, integer times, integer data returns nothing
    local timer t
    local integer tid
    local integer index = 0
    if timeout < 0 then
        return
      else
        set t = CreateTimer()
		set tid = YDTriggerAny2I(timer, t)
    endif
    set TimerSystem_RunIndex = TimerSystem_RunIndex + 1
	call YDTriggerSet(trigger, tid, INDEX_TRIGGER, trg)
	call YDTriggerSet(integer, tid, INDEX_RUNINDEX, TimerSystem_RunIndex)
	set index = YDTriggerGet(integer, YDTriggerAny2I(trigger, trg), 'YDTS'+data)
    set index = index + 1
	call YDTriggerSet(integer, YDTriggerAny2I(trigger, trg), 'YDTS'+data, index)
	call YDTriggerSet(timer, YDTriggerAny2I(trigger, trg), ('YDTS'+data)*index, t)
	
    if b == false then
		call YDTriggerSet(integer, tid, INDEX_TIMES, times)
        call TimerStart(t, timeout, true, function RunPeriodicTriggerFunctionByTimes)
      else
        call TimerStart(t, timeout, true, function RunPeriodicTriggerFunction)
    endif
    set t = null
endfunction

function YDWETimerRunPeriodicTriggerOver takes trigger trg, integer data returns nothing
	local integer trgid = YDTriggerAny2I(trigger, trg)
    local integer index = YDTriggerGet(integer, trgid, 'YDTS'+data)
    local timer t
    loop
        exitwhen index <= 0
        set t = YDTriggerGet(timer, trgid, ('YDTS'+data)*index)
        call DestroyTimer(t)
        call YDTriggerClearTable(YDTriggerAny2I(timer, t))
		call YDTriggerClear(timer, trgid, ('YDTS'+data)*index)
        set index = index - 1
    endloop
	
    call YDTriggerClear(integer, trgid, 'YDTS'+data)
    set t = null
endfunction

#undef INDEX_TRIGGER
#undef INDEX_RUNINDEX
#undef INDEX_TIMES

endlibrary


#endif /// YDWETimerSystemIncluded
