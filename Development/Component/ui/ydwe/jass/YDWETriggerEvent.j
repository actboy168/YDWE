#ifndef YDWETriggerEventIncluded
#define YDWETriggerEventIncluded

//===========================================================================  
//===========================================================================  
//自定义事件 
//===========================================================================
//===========================================================================   

library YDWETriggerEvent 

globals
#ifndef YDWE_DamageEventTrigger
#define YDWE_DamageEventTrigger
    trigger yd_DamageEventTrigger = null
#endif
    private constant integer DAMAGE_EVENT_SWAP_TIMEOUT = 600  // 每隔这个时间(秒), yd_DamageEventTrigger 会被移入销毁队列
    private constant boolean DAMAGE_EVENT_SWAP_ENABLE = true  // 若为 false 则不启用销毁机制
    private trigger yd_DamageEventTriggerToDestory = null

    private trigger array DamageEventQueue
    private integer DamageEventNumber = 0
	
    item bj_lastMovedItemInItemSlot = null
	
    private trigger MoveItemEventTrigger = null
    private trigger array MoveItemEventQueue
    private integer MoveItemEventNumber = 0
endglobals
	
//===========================================================================  
//任意单位伤害事件 
//===========================================================================
function YDWEAnyUnitDamagedTriggerAction takes nothing returns nothing
    local integer i = 0
    
    loop
        exitwhen i >= DamageEventNumber
        if DamageEventQueue[i] != null and IsTriggerEnabled(DamageEventQueue[i]) and TriggerEvaluate(DamageEventQueue[i]) then
            call TriggerExecute(DamageEventQueue[i])
        endif
        set i = i + 1  
    endloop    
endfunction

function YDWEAnyUnitDamagedFilter takes nothing returns boolean     
    if GetUnitAbilityLevel(GetFilterUnit(), 'Aloc') <= 0 then 
        call TriggerRegisterUnitEvent(yd_DamageEventTrigger, GetFilterUnit(), EVENT_UNIT_DAMAGED)
    endif
    return false
endfunction

function YDWEAnyUnitDamagedEnumUnit takes nothing returns nothing
    local group g = CreateGroup()
    local integer i = 0
    loop
        call GroupEnumUnitsOfPlayer(g, Player(i), Condition(function YDWEAnyUnitDamagedFilter))
        set i = i + 1
        exitwhen i >= bj_MAX_PLAYER_SLOTS
    endloop
    call DestroyGroup(g)
    set g = null
endfunction

function YDWEAnyUnitDamagedRegistTriggerUnitEnter takes nothing returns nothing
    local trigger t = CreateTrigger()
    local region  r = CreateRegion()
    local rect world = GetWorldBounds()
    call RegionAddRect(r, world)
    call TriggerRegisterEnterRegion(t, r, Condition(function YDWEAnyUnitDamagedFilter))
    call RemoveRect(world)
    set t = null
    set r = null
    set world = null
endfunction

// 将 yd_DamageEventTrigger 移入销毁队列, 从而排泄触发器事件
function YDWESyStemAnyUnitDamagedSwap takes nothing returns nothing
    local boolean isEnabled = IsTriggerEnabled(yd_DamageEventTrigger)

    call DisableTrigger(yd_DamageEventTrigger)
    if yd_DamageEventTriggerToDestory != null then
        call DestroyTrigger(yd_DamageEventTriggerToDestory)
    endif

    set yd_DamageEventTriggerToDestory = yd_DamageEventTrigger
    set yd_DamageEventTrigger = CreateTrigger()
    if not isEnabled then
        call DisableTrigger(yd_DamageEventTrigger)
    endif

    call TriggerAddAction(yd_DamageEventTrigger, function YDWEAnyUnitDamagedTriggerAction) 
    call YDWEAnyUnitDamagedEnumUnit()
endfunction

function YDWESyStemAnyUnitDamagedRegistTrigger takes trigger trg returns nothing
    if trg == null then
        return
    endif
        
    if DamageEventNumber == 0 then
        set yd_DamageEventTrigger = CreateTrigger()
        call TriggerAddAction(yd_DamageEventTrigger, function YDWEAnyUnitDamagedTriggerAction) 
        call YDWEAnyUnitDamagedEnumUnit()
        call YDWEAnyUnitDamagedRegistTriggerUnitEnter()
        if DAMAGE_EVENT_SWAP_ENABLE then
            // 每隔 DAMAGE_EVENT_SWAP_TIMEOUT 秒, 将正在使用的 yd_DamageEventTrigger 移入销毁队列
            call TimerStart(CreateTimer(), DAMAGE_EVENT_SWAP_TIMEOUT, true, function YDWESyStemAnyUnitDamagedSwap)
        endif
    endif   
    
    set DamageEventQueue[DamageEventNumber] = trg
    set DamageEventNumber = DamageEventNumber + 1
endfunction

//===========================================================================  
//移动物品事件 
//===========================================================================  
function YDWESyStemItemUnmovableTriggerAction takes nothing returns nothing
    local integer i = 0
    
    if GetIssuedOrderId() >= 852002 and GetIssuedOrderId() <= 852007 then 
		set bj_lastMovedItemInItemSlot = GetOrderTargetItem() 
    	loop
        	exitwhen i >= MoveItemEventNumber
        	if MoveItemEventQueue[i] != null and IsTriggerEnabled(MoveItemEventQueue[i]) and TriggerEvaluate(MoveItemEventQueue[i]) then
        	    call TriggerExecute(MoveItemEventQueue[i])
        	endif
        	set i = i + 1  
    	endloop  
	endif	
endfunction

function YDWESyStemItemUnmovableRegistTrigger takes trigger trg returns nothing
    if trg == null then
        return
    endif
        
    if MoveItemEventNumber == 0 then
        set MoveItemEventTrigger = CreateTrigger()
        call TriggerAddAction(MoveItemEventTrigger, function YDWESyStemItemUnmovableTriggerAction) 
        call TriggerRegisterAnyUnitEventBJ(MoveItemEventTrigger, EVENT_PLAYER_UNIT_ISSUED_TARGET_ORDER)
    endif   
    
    set MoveItemEventQueue[MoveItemEventNumber] = trg
    set MoveItemEventNumber = MoveItemEventNumber + 1
endfunction

function GetLastMovedItemInItemSlot takes nothing returns item
    return  bj_lastMovedItemInItemSlot
endfunction

endlibrary 

#endif /// YDWETriggerEventIncluded
