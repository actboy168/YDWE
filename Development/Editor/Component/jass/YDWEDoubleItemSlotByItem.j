#ifndef YDWEDoubleItemSlotByItemIncluded
#define YDWEDoubleItemSlotByItemIncluded

#include "YDWEBase.j"

//===========================================================================
//多重物品栏 
//===========================================================================   
library YDWEDoubleItemSlotByItem requires YDWEBase 
function YDWEIsItemSwitch takes unit u returns boolean    
    return YDWEGetBooleanByString(I2S(YDWEH2I(u)), "YDWEItemAbilitySystemTriggerClose")
endfunction 

function YDWEItemSwitch takes unit Ua,unit Ub returns nothing
    local item array Ia
    local item array Ib
    local integer i = 0        
    call YDWESaveBooleanByString(I2S(YDWEH2I(Ua)), "YDWEItemAbilitySystemTriggerClose",true)
    call SetUnitOwner(Ub,GetOwningPlayer(Ua),false)
    loop               
        exitwhen i >5              
        if   UnitItemInSlot(Ua, i) != null   then 
            set Ia[i]= UnitItemInSlot(Ua, i) 
            call UnitRemoveItemFromSlot(Ua,i)           
        endif
        if   UnitItemInSlot(Ub, i) != null   then 
            set Ib[i]= UnitItemInSlot(Ub, i)            
            call UnitRemoveItemFromSlot(Ub,i)  
        endif  
        set i = i+1              
     endloop
     set i = 0
     loop         
        exitwhen i >5 
        if Ia[i] == null then                  
           call UnitAddItem(Ub,CreateItem('stpg',GetUnitX(Ub),GetUnitY(Ub)))              
          else 
           call UnitAddItem(Ub,Ia[i])
        endif 
        set i = i+1 
     endloop  
 //给A的物品    
     set i = 0
     loop             
        exitwhen i >5
        if Ib[i] == null then                  
           call UnitAddItem(Ua,CreateItem('stpg',GetUnitX(Ua),GetUnitY(Ua)))             
          else 
           call UnitAddItem(Ua,Ib[i])
        endif  
        set i = i+1
     endloop  
     set i = 0
     loop           
        exitwhen i >5
        if GetItemTypeId(UnitItemInSlot(Ua,i))=='stpg' then              
          call RemoveItem(UnitItemInSlot(Ua,i))           
        endif
        if GetItemTypeId(UnitItemInSlot(Ub,i))=='stpg' then             
          call RemoveItem(UnitItemInSlot(Ub,i))
        endif
        set i = i+1
     endloop 
    set i = 0 
    loop              
        exitwhen i >5 
        set Ia[i]=null
        set Ib[i]=null
        set i = i+1
    endloop  
    call SetUnitOwner(Ub,Player(PLAYER_NEUTRAL_PASSIVE),false)
    call YDWESaveBooleanByString(I2S(YDWEH2I(Ua)), "YDWEItemAbilitySystemTriggerClose",false)
endfunction                    

function YDWEDoubleItemSlotByAbility_CON takes nothing returns boolean 
    local unit u = null    
    local trigger trg = GetTriggeringTrigger()
    local unit hero = GetTriggerUnit()
    local integer ab = YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"Ability")     
    local integer ItemSlotMax=YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotMax")
    local integer i=0 
    if GetSpellAbilityId() !=ab then 
        set u=null
        set trg=null
        set hero=null
        return false
    endif          
    if ItemSlotMax == 1  then       
        set u=null
        set trg=null
        set hero=null
        return false
    endif
    set i=YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum")    
    set i=i+1         
    if i>ItemSlotMax then 
       set i=1                
    endif          
    call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum",i)
    set u=YDWEGetUnitByString(I2S(YDWEH2I(trg)),I2S(i))
    call YDWEItemSwitch(hero,u)
    call SetUnitAbilityLevel(hero,ab,i) 
    if i==1 then
        call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(ItemSlotMax),u)
      else  
        call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(i-1),u)
    endif                                    
    set u=null
    set trg=null
    set hero=null
    return false     
endfunction 

function YDWEDoubleItemSlotByAbility takes unit hero,integer ab,integer uid,integer MaxItemSlot returns nothing 
    local integer i = YDWEGetIntegerByString(I2S(YDWEH2I(hero)),"MulItemSlotByAbility")
    local trigger trg = null   
    local unit u=null     
    if i==0 then
        set trg = CreateTrigger()
        call TriggerRegisterUnitEvent(trg, hero, EVENT_UNIT_SPELL_EFFECT) 
        call TriggerAddCondition(trg, Condition(function YDWEDoubleItemSlotByAbility_CON))
        call YDWESaveTriggerByString(I2S(YDWEH2I(hero)),"MulItemSlotByAbility",trg)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"Ability",ab)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotMax",MaxItemSlot)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum",1)
        if MaxItemSlot>1 then
          loop
            exitwhen MaxItemSlot==1                
                set u=CreateUnit(Player(PLAYER_NEUTRAL_PASSIVE),uid,0,0,0)    
                call YDWESaveBooleanByString(I2S(YDWEH2I(u)), "YDWEItemAbilitySystemTriggerClose",true)        
                call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(MaxItemSlot),u)  
                set MaxItemSlot=MaxItemSlot-1     
                set u=null        
          endloop
        endif      
    endif        
    set trg=null
endfunction 

function YDWEDoubleItemSlotByItem_CON takes nothing returns boolean 
    local unit u = null    
    local trigger trg = GetTriggeringTrigger()
    local unit hero = GetTriggerUnit()
    local integer itid = YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"ItemTypeId")     
    local integer ItemSlotMax=YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotMax")
    local integer i=0 
    if GetItemTypeId(GetManipulatedItem()) !=itid then 
        set u=null
        set trg=null
        set hero=null
        return false
    endif          
    if ItemSlotMax == 1  then       
        set u=null
        set trg=null
        set hero=null
        return false
    endif
    set i=YDWEGetIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum")    
    set i=i+1         
    if i>ItemSlotMax then 
       set i=1                
    endif          
    call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum",i)
    set u=YDWEGetUnitByString(I2S(YDWEH2I(trg)),I2S(i))
    call YDWEItemSwitch(hero,u)
    call SetItemCharges(UnitItemInSlot(hero,5),i)   
    if i==1 then
        call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(ItemSlotMax),u)
      else  
        call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(i-1),u)
    endif                                    
    set u=null
    set trg=null
    set hero=null
    return false     
endfunction 

function YDWEDoubleItemSlotByItem takes unit hero,integer ItemTypeId,integer uid,integer MaxItemSlot returns nothing 
    local integer i = YDWEGetIntegerByString(I2S(YDWEH2I(hero)),"MulItemSlotByItem") 
    local trigger trg = null    
    local unit u=null    
    local item it=null    
    if i==0 then
        set trg = CreateTrigger()
        call TriggerRegisterUnitEvent(trg, hero, EVENT_UNIT_USE_ITEM) 
        call TriggerAddCondition(trg, Condition(function YDWEDoubleItemSlotByItem_CON))
        call YDWESaveTriggerByString(I2S(YDWEH2I(hero)),"MulItemSlotByItem",trg)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemTypeId",ItemTypeId)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotMax",MaxItemSlot)
        call YDWESaveIntegerByString(I2S(YDWEH2I(trg)),"ItemSlotNum",1)                  
        if MaxItemSlot>1 then
            loop
                exitwhen MaxItemSlot==1   
                set u=CreateUnit(Player(PLAYER_NEUTRAL_PASSIVE),uid,0,0,0)    
                call YDWESaveBooleanByString(I2S(YDWEH2I(u)), "YDWEItemAbilitySystemTriggerClose",true)        
                call YDWESaveUnitByString(I2S(YDWEH2I(trg)),I2S(MaxItemSlot),u) 
                set it=CreateItem(ItemTypeId, 0, 0)                 
                call UnitAddItem(u,it)
                call UnitDropItemSlot(u,it,5)                    
                set MaxItemSlot=MaxItemSlot-1      
                set u=null   
                set it=null    
            endloop
        endif      
    endif      
    set trg=null
endfunction 
endlibrary                           

#endif /// YDWEDoubleItemSlotByItemIncluded
