#ifndef YDWEAddAIOrderIncluded
#define YDWEAddAIOrderIncluded

#include "YDWEBase.j"

//===========================================================================
//AI-自动释放技能 
//===========================================================================
library YDWEAddAIOrder requires YDWEBase 
private function AIOrderRun takes nothing returns boolean           
    local unit u = GetTriggerUnit()
    local integer count = YDWEGetIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Count")
    local integer order = 0
    local integer pro = 0
    local integer index = 0
    local integer i = 0
    local integer a = 0
    set a = GetRandomInt(0,100*count)
    loop
        set pro = YDWEGetIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Probability"+I2S(i))
        if 100*i <= a and a < 100*i+pro then  
            set order = YDWEGetIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder"+I2S(i))
            set index = YDWEGetIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Index"+I2S(i))
        endif
        exitwhen i >= count
        set i = i + 1
    endloop
    // 1：敌目标 2：点目标 3：无目标 4：自己
    if index == 1 then
        call IssueTargetOrderById(u,order,GetEventTargetUnit())
    elseif index == 2 then
        call IssuePointOrderById(u,order,GetUnitX(GetEventTargetUnit()),GetUnitY(GetEventTargetUnit()))
    elseif index == 3 then
        call IssueImmediateOrderById(u, order)
    elseif index == 4 then
        call IssueTargetOrderById(u,order,u)
    endif
    set u = null    
    return false
endfunction

function YDWEAddAIOrder takes unit u,integer N,integer index,string orderA,string orderB,string orderC,integer probability returns nothing
    local integer i = YDWEGetIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Count")
    local trigger trg
    local string order=null
    if i==0 then
        set trg = CreateTrigger()
        if N==1 then
            call TriggerRegisterUnitEvent(trg, u, EVENT_UNIT_TARGET_IN_RANGE) //用发动攻击为事件
          else
            call TriggerRegisterUnitEvent(trg, u, EVENT_UNIT_ATTACKED  ) //用被攻击为事件 
        endif      
        call YDWESaveIntegerByString(I2S(YDWEH2I(u)), "MercenaryTrigger",YDWEH2I(trg))
        call YDWESaveTriggerConditionByString(I2S(YDWEH2I(trg)), "TriggerCondition",TriggerAddCondition(trg, Condition(function AIOrderRun))) //好吧，这个提升效率是非常明显的
    endif
    if index == 1 then
        set order=orderA
      elseif index == 2 then
        set order=orderB
      elseif index == 3 then
        set order=orderC
      elseif index == 4 then
        set order=orderA
    endif                                         
    call YDWESaveIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder"+I2S(i), OrderId(order)) //使用OrderId要比OrderString好得多
    call YDWESaveIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Probability"+I2S(i), probability)
    call YDWESaveIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Index"+I2S(i), index)
    call YDWESaveIntegerByString(I2S(YDWEH2I(u)), "MercenaryAIOrder_Count", i+1)
    set trg = null            
endfunction 

//这个是清空函数
function YDWEFlushAIOrder takes unit u returns nothing
    local trigger trg=YDWEGetTriggerByString(I2S(YDWEH2I(u)), "MercenaryTrigger")
    call TriggerRemoveCondition(trg,YDWEGetTriggerConditionByString(I2S(YDWEH2I(trg)), "TriggerCondition"))
    call DestroyTrigger(trg)
    call YDWEFlushMissionByString(I2S(YDWEH2I(trg)))
    call YDWEFlushMissionByString(I2S(YDWEH2I(u)))
    set trg=null
endfunction
endlibrary 

#endif /// YDWEAddAIOrderIncluded
