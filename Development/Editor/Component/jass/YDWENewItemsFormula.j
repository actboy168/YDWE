#ifndef YDWENewItemsFormulaIncluded
#define YDWENewItemsFormulaIncluded

#include "YDWEBase.j"


library YDWEStringFormula initializer Init requires YDWEBase

globals
    item bj_lastCombinedItem=null
endglobals

function GetLastCombinedItem takes nothing returns item
    return  bj_lastCombinedItem
endfunction

//===========================================================================
//物品合成
globals
    private trigger array ItemCombineEventQueue
    private integer ItemCombineEventNumber = 0
endglobals

function YDWESyStemItemCombineTriggerAction takes nothing returns nothing
	local integer i = 0
    loop
        exitwhen i >= ItemCombineEventNumber
        if ItemCombineEventQueue[i] != null and TriggerEvaluate(ItemCombineEventQueue[i]) and IsTriggerEnabled(ItemCombineEventQueue[i]) then
            call TriggerExecute(ItemCombineEventQueue[i])
        endif
        set i = i + 1  
    endloop
endfunction

//GetLastMovedItemInItemSlot 
//===========================================================================  
//物品合成事件 
//===========================================================================  
function YDWESyStemItemCombineRegistTrigger takes trigger trg returns nothing
	set ItemCombineEventQueue[ItemCombineEventNumber] = trg
	set ItemCombineEventNumber = ItemCombineEventNumber + 1        
endfunction

    private struct Inventory    
        method reset takes nothing returns nothing
            call YDWEFlushMissionByString("YDWEStringFormula."+I2S(this))
        endmethod
        
        method onDestroy takes nothing returns nothing
            call YDWEFlushMissionByString("YDWEStringFormula."+I2S(this))
        endmethod
    endstruct    
    
    private struct StringTable extends Inventory        
        method operator [] takes string key returns integer
            return YDWEGetIntegerByString("YDWEStringFormula."+I2S(this), key)
        endmethod
        
        method operator []= takes string key, integer value returns nothing
            call YDWESaveIntegerByString("YDWEStringFormula."+I2S(this), key, value)
        endmethod
        
        method flush takes string key returns nothing
            call YDWEFlushStoredIntegerByString("YDWEStringFormula."+I2S(this), key)
        endmethod
        
        method exists takes string key returns boolean
            return YDWEHaveSavedIntegerByString("YDWEStringFormula."+I2S(this), key)
        endmethod        
    endstruct

//***************************************************************
//* StringFormula 2.0 - Fetrix_sai
//* ------------
//*
//***************************************************************

    private keyword ItemIdMatrix
    private keyword FormulaMatrix
    private keyword Sorting

    globals
        private constant integer MAX_INSTANCES  = 8100
        private constant integer SEGMENT_LENGTH = 4
    endglobals
    
    private function Convert takes integer value, integer length returns string
        local string  Char = I2S(value)
        local integer slen = StringLength(Char)
            
        loop
            exitwhen slen >= length
            set Char = "0" + Char
            set slen = slen + 1
        endloop
        
        return Char
    endfunction
   
   
////////////////////////////////////////////////////////////
//////////////////// __ItemId Matrix__ /////////////////////

    private struct ItemIdMatrix extends array [MAX_INSTANCES]
    
        static StringTable Data
        static integer Total = 1
        integer itemId 
        
        string  keyStr
        integer keyLen
        
        static method get takes integer id returns thistype
            return .Data[I2S(id)]
        endmethod
        
        static method exists takes integer id returns boolean
            return .Data.exists(I2S(id))
        endmethod
                 
        method sub takes integer segment returns integer
            local integer start = .keyLen * segment + 1
            return S2I(SubString(.keyStr, start, (start + .keyLen)))
        endmethod
        
        static method assigns takes integer id returns thistype
            local thistype this = 0

            if id == null or id == 0 then
                return 0
            endif
            
            set this = thistype.get(id)
                
            if integer(this) > 0 then
                return this
            endif
            
            set this = thistype[.Total]
            set this.itemId = id
            set this.keyStr = I2S(SEGMENT_LENGTH)
            set this.keyLen = SEGMENT_LENGTH
            set .Data[I2S(id)] = integer(this)
            set .Total = .Total + 1
            return this
        endmethod
        
        method memory takes string key returns nothing
            set .keyStr = .keyStr + key
            //debug call BJDebugMsg("|cff00ff00KEY: |r" + .keyStr)
        endmethod
        
        private static method onInit takes nothing returns nothing
            set .Data = StringTable.create()
        endmethod
        
    endstruct

////////////////// __End of ItemId Matrix__ ////////////////
////////////////////////////////////////////////////////////
    

    
////////////////////////////////////////////////////////////
//////////////////// __Formula Matrix__ ////////////////////
    
    globals
        private FormulaMatrix FormulaData = 0
    endglobals
    
    private struct FormulaMatrix extends array [MAX_INSTANCES]
        
        static StringTable Data
        static integer Total = 1
        
        string  formula
        integer segmLen
        
        string model
        string message
        integer chance
        boolean delete
        
        static method exists takes string s returns boolean
            return .Data.exists(s)
        endmethod
        
        method sub takes integer segment returns integer
            local integer start = .segmLen * segment + 1
            return S2I(SubString(.formula, start, (start + .segmLen)))
        endmethod
        
        static method memory takes integer lingth, string formStr, ItemIdMatrix eventually returns thistype
            local thistype this = 0
            
            if FormulaMatrix.exists(formStr) then
                //debug call BJDebugMsg("|cffff0000Registration cannot be repeated !!|r")
                return 0
            endif
            
            set this = thistype[thistype.Total]
            set this.formula    = formStr + Convert(eventually, SEGMENT_LENGTH)
            set this.segmLen    = lingth
            
            set this.model      = null //"Abilities\\Spells\\Items\\AIam\\AIamTarget.mdl"
            set this.message    = null //"|cff00ff00你合成了：|r" 
            set this.chance     = 100
            set this.delete     = false
            set .Data[formStr]  = integer(this) 
            set .Total  = .Total + 1
            
            //debug call BJDebugMsg("|cff00ff00Formula Register: |r" + I2S(integer(this)) + ". " + this.formula)
            return this
        endmethod
        
        private static method onInit takes nothing returns nothing
            set .Data = StringTable.create()
        endmethod
        
    endstruct
    
////////////////// __End of Formula Matrix__ ///////////////
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
//////////////////// __Formula Sorting__ ///////////////////

    private struct Sorting
        ItemIdMatrix array stack[8]
        integer array count[8]
        integer total = 7
        
        method onDestroy takes nothing returns nothing
            local integer i = 0
            loop
                exitwhen i == stack.size
                set .stack[i] = 0
                set .count[i] = 0
                set i = i + 1
            endloop
            set .char = ""
        endmethod
        
        static method create takes integer type1, integer n1, integer type2, integer n2, integer type3, integer n3,/*
     */ integer type4, integer n4, integer type5, integer n5, integer type6, integer n6, integer type7, integer n7 returns thistype
            local thistype this = thistype.allocate()
            
            set this.stack[1] = ItemIdMatrix.assigns(type1)
            set this.stack[2] = ItemIdMatrix.assigns(type2)
            set this.stack[3] = ItemIdMatrix.assigns(type3)
            set this.stack[4] = ItemIdMatrix.assigns(type4)
            set this.stack[5] = ItemIdMatrix.assigns(type5)
            set this.stack[6] = ItemIdMatrix.assigns(type6)
            set this.stack[7] = ItemIdMatrix.assigns(type7)
            
            set this.count[1] = n1
            set this.count[2] = n2
            set this.count[3] = n3
            set this.count[4] = n4
            set this.count[5] = n5
            set this.count[6] = n6
            set this.count[7] = n7
            
            return this
        endmethod
        
        string char
        method convert takes nothing returns string
            local integer i = 1
            local integer j 
            local integer k
              
            set .char = ""
            loop
                exitwhen i > .total
                if .stack[i] == 0 or .count[i] == 0 then
                    set .stack[i] = .stack[.total]
                    set .count[i] = .count[.total]
                    set .total = .total - 1
                    set i = i - 1
                else
                    set j = i + 1
                    loop
                        exitwhen j > .total
                        if .stack[j] == 0 or .count[j] == 0 then
                            set .stack[j] = .stack[.total]
                            set .count[j] = .count[.total]
                            set .total = .total - 1
                            set j = j - 1
                        elseif integer(.stack[i]) > integer(.stack[j]) then
                            set k = .stack[i]
                            set .stack[i] = .stack[j]
                            set .stack[j] = k
                            set k = .count[i]
                            set .count[i] = .count[j]
                            set .count[j] = k
                        elseif integer(.stack[i]) == integer(.stack[j]) then
                            set .count[i] = .count[i] + .count[j]
                            set .stack[j] = 0
                            set j = j - 1
                        endif
                        set j = j + 1
                    endloop
                
                    set j = .count[i]
                    loop
                        exitwhen j == 0
                        set .char = .char + Convert(.stack[i], SEGMENT_LENGTH)
                        set j = j - 1
                    endloop
                endif
                set i = i + 1
            endloop
            
            return .char
        endmethod
        
    endstruct

////////////////// __End of Formula Sorting__ //////////////
////////////////////////////////////////////////////////////
    
    

////////////////////////////////////////////////////////////
/////////////////////// __Craft Item__ /////////////////////
    
    private function Adaptation takes nothing returns boolean
        return true
    endfunction
        
    private function verify takes nothing returns boolean
        return ItemIdMatrix.exists(GetItemTypeId(GetManipulatedItem()))
    endfunction
        
    private function GetCharges takes item it returns integer 
        local integer charges = GetItemCharges(it)
        if charges == 0 then
            set charges = 1
        endif
        return charges
    endfunction
        
    private function CraftItem takes nothing returns nothing
        local FormulaMatrix forx    = 0
        local ItemIdMatrix  itmx    = ItemIdMatrix.get(GetItemTypeId(GetManipulatedItem()))
        local integer       size    = StringLength(itmx.keyStr) / itmx.keyLen
        local integer       inst    = 0
        local integer       numb    = 0
        local integer       sum     = 0
        
        local integer       tmpid   = 0   
        local integer       i       = 0
        local integer       j       = 0
        local boolean       b       = false  
        local item          it      = null
        
        local item          article = GetManipulatedItem()
        local unit          caster  = GetTriggerUnit()
        
        local item    array items 
        local integer array typeid
        local integer array amount    
        local integer array surplus 

        //* Check Slots
        set i = 0
        loop
            exitwhen i == 6
            set it = UnitItemInSlot(caster, i)
                
            if it != null then
                set sum:items  = it
                set sum:typeid = GetItemTypeId(it)
                set sum:amount = GetCharges(it)
                if it == article then
                    set b = true
                endif
                set sum = sum + 1
            endif
                
            set i = i + 1
            set it = null
        endloop
            
        if b == false then
            set sum:items  = article
            set sum:typeid = GetItemTypeId(article)
            set sum:amount = 1
            set sum = sum + 1
        endif

        //* Check Formulas
        loop
            exitwhen inst == size
            set forx = FormulaMatrix(itmx.sub(inst))
            set numb = StringLength(forx.formula) / forx.segmLen - 1
            
            set i = 0
            loop
                exitwhen i == sum
                set i:surplus = i:amount 
                set i = i + 1
            endloop
                
            //! Check Formula
            set i = 0
            loop
                set tmpid = ItemIdMatrix(forx.sub(i)).itemId
                exitwhen i == numb
                
                set j = 0
                loop
                    exitwhen j == sum
                    
                    if j:typeid == tmpid and j:surplus > 0 then
                        set j:surplus = j:surplus - 1
                        exitwhen true 
                    endif
                        
                    set j = j + 1
                endloop
                
                exitwhen j == sum
                set i = i + 1
            endloop

            //! Merge item
            if i == numb then
                if GetRandomInt(0, 99) < forx.chance then
                    //! effect and text
                    if forx.model != null then
                        call DestroyEffect(AddSpecialEffectTarget(forx.model, caster, "origin"))
                    endif
                    
                    if forx.message != null then
                        call DisplayTimedTextToPlayer(GetTriggerPlayer(), 0, 0, 30, forx.message)
                    endif
                
                    //! remove item
                    set i = 0
                    loop
                        exitwhen i == sum
                        if i:surplus > 0 and GetItemCharges(i:items) > 0 then
                            call SetItemCharges(i:items, i:surplus)
                        elseif i:surplus == 0 then
                            call RemoveItem(i:items)
                        endif

                        set i = i + 1
                    endloop
                    
                    //! add item
                    set it = CreateItem(tmpid, GetUnitX(caster), GetUnitY(caster))
            
                    if GetItemCharges(it) > 0 then
                        call SetItemCharges(it, 1)
                
                        set i = 0
                        loop
                            exitwhen i == sum
                            if i:typeid == tmpid then
                                call RemoveItem(i:items)
                                call SetItemCharges(it, i:surplus + 1)
                            endif
                            set i = i + 1
                        endloop
                    endif
            
                    call UnitAddItem(caster, it)
                    set bj_lastCombinedItem = it    
                    call YDWESyStemItemCombineTriggerAction()
                elseif forx.delete then
                    //! remove item
                    set i = 0
                    loop
                        exitwhen i == sum
                        if i:surplus > 0 and GetItemCharges(i:items) > 0 then
                            call SetItemCharges(i:items, i:surplus)
                        elseif i:surplus == 0 then
                            call RemoveItem(i:items)
                        endif

                        set i = i + 1
                    endloop
                endif
                    
                set it = null
                exitwhen true
            endif
                
            set inst = inst + 1
        endloop

        if b == false /*and GetItemType(article) == ITEM_TYPE_POWERUP*/ then
            call RemoveItem(article)
        endif
        
        set i = 0
        loop
            exitwhen i == sum
            set i:items = null
            set i = i + 1
        endloop
            
        set article = null
        set caster  = null
        set it = null
    endfunction

    private function Init takes nothing returns nothing
        local filterfunc f = Filter(function Adaptation)
        local trigger    t = CreateTrigger()
        local integer    i = 0
        
        loop
            exitwhen i > 15
            call TriggerRegisterPlayerUnitEvent(t, Player(i), EVENT_PLAYER_UNIT_PICKUP_ITEM, f)
            set i = i + 1
        endloop
        
        call TriggerAddCondition(t, Condition(function verify))
        call TriggerAddAction(t, function CraftItem)
        call DestroyFilter(f)
        
        set f = null
        set t = null   
    endfunction
    
///////////////////// __End of Craft Item__ ////////////////
////////////////////////////////////////////////////////////


    
    // call YDWENewItemsFormula('rat6', 6, 'rat9', 5, 'ratc', 4, 'rde1', 3, 'rde2', 2, 'rde3', 1, 'mcou')
    function YDWENewItemsFormula takes integer type1, integer n1, integer type2, integer n2, integer type3, integer n3, /*
    */  integer type4, integer n4, integer type5, integer n5, integer type6, integer n6, integer eventually returns nothing
        local Sorting sort = Sorting.create(type1, n1, type2, n2, type3, n3, type4, n4, type5, n5, type6, n6, 0, 0)
        local ItemIdMatrix itmx = ItemIdMatrix.assigns(eventually)
        local string  f = I2S(SEGMENT_LENGTH) + sort.convert()
        local string  s = ""
        local integer i = 0
        
        set FormulaData = FormulaMatrix.memory(SEGMENT_LENGTH, f, itmx)

        if integer(FormulaData) > 0 then
            set s = Convert(integer(FormulaData), SEGMENT_LENGTH)
            
            set i = 0
            loop
                exitwhen i > sort.total
                call sort.stack[i].memory(s)
                set i = i + 1
            endloop
            
            call itmx.memory(s)
        endif
        
        call sort.destroy()
        set f = ""
        set s = ""
    endfunction

endlibrary

#endif /// YDWENewItemsFormulaIncluded
