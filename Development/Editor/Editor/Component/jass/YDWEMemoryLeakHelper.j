#ifndef YDWEMemoryLeakHelperIncluded
#define YDWEMemoryLeakHelperIncluded

#include "YDWEBase.j"



//library ValueIndexing initializer Init requires YDWEBase
library MemoryLeakHelper initializer Init requires YDWEBase
    globals
        private constant real HASH_DECAY_TIME = 500.
        private constant integer CLEAR_HASH_COUNT = 8190
        private integer HashNumber = 0
        private integer array HashData
        private integer array HashHash
        private integer array HashPlace
        private real array CreationTime
        
        private integer TempHashNumber = 0
        private integer array TempHashHash
        private integer array TempHashPlace
        private integer array TempHashData
        private real array TempCreationTime
        
        private integer LastHashedValue = 0
        private integer LastIndex = 0
        private real GameTime = 0.
        private timer GameTimeTimer = CreateTimer()
        private constant real GAMETIME_TIMER_INTERVAL = 30.
        private constant integer key = 0        
        
        private constant integer MAX_INSTANCES=8100 //400000
        
        // The system fires when you do something that creates a leak.
        // The data that cause leak are saved in a variable then.
        // And every CLEAN_UP_INTERVAL seconds those data are destroyed.
        // This shouldn't be too high, or too low.
        private constant real CLEAN_UP_INTERVAL = 10
        // If this is set to true, the system will work more slowly (but you wont notice)
        // and count, how much memory this system was able to save.
        // This value is display by the function DisplayLeaks() then.
        // WARNING: This sucks a lot of performance. I would ONLY use it when you want
        // to test, if this is useful for your map. Later set it to false.
        private constant boolean DISPLAY_SAVED_MEMORY = true
        // The Data are only cleaned up, when that many handles were caught
        private constant integer MIN_LEAK_NUMBER = 1750
        // How often are data passed to the destroyer?
        // Leaks stay for a random time between CLEAN_UP_INTERVAL and CLEAN_UP_INTERVAL+PASS_INTERVAL
        // in the game
        private constant real PASS_INTERVAL = 2.5
        // Memory leaks occur pretty frequently. When a leak is caught it is saved in
        // an array. But the array can't have more than MAX_LEAK_INSTANCES instances, so
        // if more than MAX_LEAK_INSTANCES memory leaks occur during a destroy interval,
        // the system fails.
        private constant integer MAX_LEAK_INSTANCES = 60000
    endglobals
    
    private function GetElapsedGameTime takes nothing returns real
        return GameTime + TimerGetElapsed(GameTimeTimer)
    endfunction
    
    private function UpdateGameTime takes nothing returns nothing
        set GameTime = GameTime + GAMETIME_TIMER_INTERVAL
    endfunction

    private struct Index	
		static method DestroyHashes takes nothing returns nothing
			local real gt = GetElapsedGameTime
			local Index ind
			
			// Well, due to the nature of this system, the looking of the code sucks.
			loop
				exitwhen HashNumber == 0
				if gt - CreationTime[HashNumber] > HASH_DECAY_TIME then
					set ind = HashHash[HashNumber]
					call ind.destroy()
					call YDWEFlushStoredIntegerByInteger(key,HashData[HashNumber])
				else
					set TempHashNumber = TempHashNumber + 1
					set TempHashData[TempHashNumber] = HashData[HashNumber]
					set TempHashHash[TempHashNumber] = HashHash[HashNumber]
					set TempHashPlace[TempHashNumber] = HashPlace[HashNumber]
					set TempCreationTime[TempHashNumber] = CreationTime[HashNumber]
				endif
				set HashData[HashNumber] = 0
				set HashHash[HashNumber] = 0
				set HashPlace[HashNumber] = 0
				set CreationTime[HashNumber] = 0.
				set HashNumber = HashNumber - 1
			endloop
			
			loop
				exitwhen TempHashNumber == 0
				set HashNumber = HashNumber + 1
				set HashData[HashNumber] = TempHashData[TempHashNumber]
				set HashHash[HashNumber] = TempHashHash[TempHashNumber]
				set HashPlace[HashNumber] = TempHashPlace[TempHashNumber]
				set TempHashData[TempHashNumber] = 0
				set TempHashNumber = TempHashNumber - 1
			endloop
			
		endmethod
	
        static method GetHash takes integer value returns integer
            local integer int = YDWEGetIntegerByInteger(key,value)
            
            if int == 0 then
                set int = Index.create()
                call YDWESaveIntegerByInteger(key,value,int)
                
                set HashNumber = HashNumber + 1
                set HashPlace[int] = HashNumber
                set HashData[HashNumber] = value
                set HashHash[HashNumber] = int
                set CreationTime[HashNumber] = GetElapsedGameTime()
                
                if HashNumber >= CLEAR_HASH_COUNT then
                    call DestroyHashes()
                endif
            endif
            
            set LastHashedValue = value
            set LastIndex = int
            
            return int
        endmethod
    endstruct

    private struct GTable[MAX_INSTANCES]
        method reset takes nothing returns nothing
			call YDWEFlushMissionByInteger(0)
            call YDWEFlushMissionByInteger(integer(this))
        endmethod

        private method onDestroy takes nothing returns nothing
            call this.reset()
        endmethod
    endstruct
	
	private struct HandleTable extends GTable
        method operator [] takes handle key returns integer
            return YDWEGetIntegerByInteger(integer(this), YDWEH2I(key)) 
        endmethod

        method operator []= takes handle key, integer value returns nothing
            call YDWESaveIntegerByInteger(integer(this), YDWEH2I(key), value)
        endmethod

        method flush takes handle key returns nothing
            call YDWEFlushStoredIntegerByInteger(integer(this), YDWEH2I(key))
        endmethod

        method exists takes handle key returns boolean
            return YDWEHaveSavedIntegerByInteger(integer(this), YDWEH2I(key))
        endmethod

        static method flush2D takes string firstkey returns nothing
            call HandleTable(- YDWES2I(firstkey)).reset()
        endmethod

        static method operator [] takes string firstkey returns HandleTable
            return HandleTable(- YDWES2I(firstkey))
        endmethod
    endstruct

//===========================================================================
// Trigger: MemoryLeakHelper
//
//     //hook GetLocationX CatchLocation
//     //hook GetLocationY CatchLocation  
//      //hook ForGroup FG
// 129k
// 3,02 + 43,32 + 697,86
// 744
//===========================================================================

    
    globals
        private HandleTable IndexData
        private HandleTable IsSaved
        
        private integer CaughtLocationLeaks = 0
        private location array LocationLeakData[MAX_LEAK_INSTANCES]
        private integer LocationDestroyCount = 0
        private location array LocationDestroyData[MAX_LEAK_INSTANCES]
		
        private integer CaughtEffectLeaks = 0
        private effect array EffectLeakData[MAX_LEAK_INSTANCES]
        private integer EffectDestroyCount = 0
        private effect array EffectDestroyData[MAX_LEAK_INSTANCES]
		
        private integer CaughtGroupLeaks = 0
        private group array GroupLeakData[MAX_LEAK_INSTANCES]
        private integer GroupDestroyCount = 0
        private group array GroupDestroyData[MAX_LEAK_INSTANCES]
        
        
        private integer DestroyedLeaks = 0
        private integer CaughtLeaks = 0
        private integer DestroyedLeaksUser = 0
        private handle LastCaught
        private timer PassTimer = CreateTimer()
        private timer CleanTimer = CreateTimer()
        private timer DelayTimer = CreateTimer()
        private boolean IsDestroying = false
        private real SavedMemory = 0.
        private real LastCheckedGroupMemoryUsage = 0.
        private boolean DestroyThreadRunning = false
        private boolean Disabled = false
        
        // These values were found out in a big leak test by gekko.
        private constant real LOCATION_MEMORY_USAGE        = 0.361
        private constant real GROUP_MEMORY_USAGE           = 0.62
        private constant real GROUP_UNIT_MEMORY_USAGE      = 0.040
        private constant real EFFECT_MEMORY_USAGE          = 11.631
        private constant real REMOVED_EFFECT_MEMORY_USAGE  = 0.066
    endglobals
    
    globals
        // 用于判断是否开启内存排泄帮助
        private boolean IsOpenMemoryLeakHelper = false
        private boolean IsDisplayMemoryLeakHelper = false
    endglobals
    
    function YDWEMemoryLeakHelperGetLastCaughtHandle takes nothing returns handle
        return LastCaught
    endfunction
    
    function YDWEMemoryLeakHelperProtectHandle takes handle h returns nothing
        set IsSaved[h] = 1
    endfunction
    
    function YDWEMemoryLeakHelperProtectVariable takes handle h returns nothing
        set IsSaved[h] = 1
    endfunction

    private function EnableMMH takes nothing returns nothing
        set Disabled = false
    endfunction

    function YDWEMemoryLeakHelperDelayMMH takes nothing returns nothing
        set Disabled = true
        call TimerStart(DelayTimer,0.00,false,function EnableMMH)
    endfunction
    
    function YDWEMemoryLeakHelperDisplayLeaks takes nothing returns nothing
        local location loc=null 
        if IsDisplayMemoryLeakHelper == false then
            call CreateNUnitsAtLoc( 1, 'hfoo', Player(15), GetRectCenter(GetPlayableMapRect()), bj_UNIT_FACING )
            call RemoveUnit(bj_lastCreatedUnit)
            set loc = GetRectCenter(GetPlayableMapRect())
            call CreateNUnitsAtLoc( 1, 'hfoo', Player(15), loc, bj_UNIT_FACING )
            call RemoveUnit(bj_lastCreatedUnit)
            call RemoveLocation(loc)   
            set loc=null
            set IsDisplayMemoryLeakHelper=true              
        endif    
        call ClearTextMessages()
        call BJDebugMsg("======= 自动排泄系统 =======")
        call BJDebugMsg("排除的泄漏"+I2S(DestroyedLeaks))
        call BJDebugMsg("用户手动排除的泄漏"+I2S(DestroyedLeaksUser))
        call BJDebugMsg("系统排泄比例: "+R2S(I2R(DestroyedLeaks)/I2R(DestroyedLeaks+DestroyedLeaksUser)*100.)+"%")
        call BJDebugMsg("作者排泄比例"+R2S(I2R(DestroyedLeaksUser)/I2R(DestroyedLeaks+DestroyedLeaksUser)*100.)+"%")
        call BJDebugMsg("下次排除的泄漏"+I2S(MIN_LEAK_NUMBER-CaughtLeaks))
        call BJDebugMsg(" === 待排除的泄漏 === ")
        call BJDebugMsg("   单位组泄漏"+I2S(GroupDestroyCount))
        call BJDebugMsg("   点泄漏"+I2S(LocationDestroyCount))
        call BJDebugMsg("   特效泄漏: "+I2S(EffectDestroyCount))
        call BJDebugMsg(" === 还未排除的泄漏=== ")
        call BJDebugMsg("   单位组泄漏"+I2S(CaughtGroupLeaks))
        call BJDebugMsg("   点泄漏"+I2S(CaughtLocationLeaks))
        call BJDebugMsg("   特效泄漏: "+I2S(CaughtEffectLeaks))
        call BJDebugMsg("下次排泄时间: "+I2S(R2I(TimerGetRemaining(PassTimer)+0.5))+" seconds.")
        call BJDebugMsg("======= 自动排泄系统 =======")
        call BJDebugMsg("排除泄漏"+I2S(DestroyedLeaks))        
        //if DISPLAY_SAVED_MEMORY then
            call BJDebugMsg("自动排泄系统释放的总内存为 "+R2S(SavedMemory)+" kb.")                 
        //endif
        call BJDebugMsg("================================")
    endfunction

    
    private function GroupGetMemoryUsageEnum takes nothing returns nothing
        set LastCheckedGroupMemoryUsage = LastCheckedGroupMemoryUsage + GROUP_UNIT_MEMORY_USAGE
    endfunction
    
    private function GroupGetMemoryUsage takes group g returns real
        set LastCheckedGroupMemoryUsage = 0.
        call ForGroup(g,function GroupGetMemoryUsageEnum)
        return LastCheckedGroupMemoryUsage + GROUP_MEMORY_USAGE
    endfunction
    	
	private function CatchLocation takes location l returns nothing
        set LastCaught = l
        
        if Disabled then
            return
        elseif CaughtLocationLeaks == MAX_LEAK_INSTANCES then
            debug call BJDebugMsg("MemoryLeakHelper: Failed to store leak because of size limitations")
            return
        endif
        
        if IndexData.exists(l) == false then
            //call BJDebugMsg("Caught Location")
            set CaughtLocationLeaks = CaughtLocationLeaks + 1
            set LocationLeakData[CaughtLocationLeaks] = l
            set IndexData[l] = CaughtLocationLeaks
        endif
    endfunction
    
    private function AddToLocationDestroyQueue takes location l returns nothing
        set LocationDestroyCount = LocationDestroyCount + 1
        set LocationDestroyData[LocationDestroyCount] = l
        set IndexData[l] = LocationDestroyCount*-1 // Put his to negative, so we know that this is used in the DestroyQueue now.
    endfunction
    
    private function ReleaseLocation takes location l returns nothing
        local integer index
        if IsDestroying == false and IndexData.exists(l) then
            set index = IndexData[l]
            // If this is true, the index wasn't put to a destroy queue yet.
            if index > 0 then
                set LocationLeakData[index] = LocationLeakData[CaughtLocationLeaks]
                set CaughtLocationLeaks = CaughtLocationLeaks - 1
            else
                set index = index * -1
                set LocationDestroyData[index] = LocationDestroyData[LocationDestroyCount]
                set LocationDestroyCount = LocationDestroyCount - 1
            endif
            call IndexData.flush(l)
            set DestroyedLeaksUser = DestroyedLeaksUser + 1
        endif
    endfunction
	
	private function CatchGroup takes group l returns nothing
        set LastCaught = l
        
        if Disabled then
            return
        elseif CaughtGroupLeaks == MAX_LEAK_INSTANCES then
            debug call BJDebugMsg("MemoryLeakHelper: Failed to store leak because of size limitations")
            return
        endif
        
        if IndexData.exists(l) == false then
            //call BJDebugMsg("Caught Group")
            set CaughtGroupLeaks = CaughtGroupLeaks + 1
            set GroupLeakData[CaughtGroupLeaks] = l
            set IndexData[l] = CaughtGroupLeaks
        endif
    endfunction
    
    private function AddToGroupDestroyQueue takes group l returns nothing
        set GroupDestroyCount = GroupDestroyCount + 1
        set GroupDestroyData[GroupDestroyCount] = l
        set IndexData[l] = GroupDestroyCount*-1 // Put his to negative, so we know that this is used in the DestroyQueue now.
    endfunction
    
    private function ReleaseGroup takes group l returns nothing
        local integer index
        if IsDestroying == false and IndexData.exists(l) then
            set index = IndexData[l]
            // If this is true, the index wasn't put to a destroy queue yet.
            if index > 0 then
                set GroupLeakData[index] = GroupLeakData[CaughtGroupLeaks]
                set CaughtGroupLeaks = CaughtGroupLeaks - 1
            else
                set index = index * -1
                set GroupDestroyData[index] = GroupDestroyData[GroupDestroyCount]
                set GroupDestroyCount = GroupDestroyCount - 1
            endif
            call IndexData.flush(l)
            set DestroyedLeaksUser = DestroyedLeaksUser + 1
        endif
    endfunction
	
	private function CatchEffect takes effect l returns nothing
        set LastCaught = l
        
        if Disabled then
            return
        elseif CaughtEffectLeaks == MAX_LEAK_INSTANCES then
            debug call BJDebugMsg("MemoryLeakHelper: Failed to store leak because of size limitations")
            return
        endif
        
        if IndexData.exists(l) == false then
            //call BJDebugMsg("Caught Effect")
            set CaughtEffectLeaks = CaughtEffectLeaks + 1
            set EffectLeakData[CaughtEffectLeaks] = l
            set IndexData[l] = CaughtEffectLeaks
        endif
    endfunction
    
    private function AddToEffectDestroyQueue takes effect l returns nothing
        set EffectDestroyCount = EffectDestroyCount + 1
        set EffectDestroyData[EffectDestroyCount] = l
        set IndexData[l] = EffectDestroyCount*-1 // Put his to negative, so we know that this is used in the DestroyQueue now.
    endfunction
    
    private function ReleaseEffect takes effect l returns nothing
        local integer index
        if IsDestroying == false and IndexData.exists(l) then
            set index = IndexData[l]
            // If this is true, the index wasn't put to a destroy queue yet.
            if index > 0 then
                set EffectLeakData[index] = EffectLeakData[CaughtEffectLeaks]
                set CaughtEffectLeaks = CaughtEffectLeaks - 1
            else
                set index = index * -1
                set EffectDestroyData[index] = EffectDestroyData[EffectDestroyCount]
                set EffectDestroyCount = EffectDestroyCount - 1
            endif
            call IndexData.flush(l)
            set DestroyedLeaksUser = DestroyedLeaksUser + 1
        endif
    endfunction
	
    private function DestroyMemoryLeaks takes nothing returns nothing
        set IsDestroying = true
        
		set DestroyedLeaks = DestroyedLeaks + GroupDestroyCount
        loop
            exitwhen GroupDestroyCount == 0
            
            if DISPLAY_SAVED_MEMORY then
                set SavedMemory = SavedMemory + GroupGetMemoryUsage(GroupDestroyData[GroupDestroyCount])
            endif
            
            call DestroyGroup(GroupDestroyData[GroupDestroyCount])
            call IndexData.flush(GroupDestroyData[GroupDestroyCount])
            set GroupDestroyCount = GroupDestroyCount - 1
        endloop
		
		set DestroyedLeaks = DestroyedLeaks + LocationDestroyCount
        loop
            exitwhen LocationDestroyCount == 0
            
            if DISPLAY_SAVED_MEMORY then
                set SavedMemory = SavedMemory + LOCATION_MEMORY_USAGE
            endif
            
            call RemoveLocation(LocationDestroyData[LocationDestroyCount])
            call IndexData.flush(LocationDestroyData[LocationDestroyCount])
            set LocationDestroyCount = LocationDestroyCount - 1
        endloop
		
		set DestroyedLeaks = DestroyedLeaks + EffectDestroyCount
        loop
            exitwhen EffectDestroyCount == 0
            
            if DISPLAY_SAVED_MEMORY then
                set SavedMemory = SavedMemory + EFFECT_MEMORY_USAGE
            endif
            
            call DestroyEffect(EffectDestroyData[EffectDestroyCount])
            call IndexData.flush(EffectDestroyData[EffectDestroyCount])
            set EffectDestroyCount = EffectDestroyCount - 1
        endloop
		
        set IsDestroying = false
        set DestroyThreadRunning = false
        //call StartPassTimer.execute() // Strange. This causes bugs sometimes and the function isn't called
        // This is slower, but safe.
        call ExecuteFunc("YDWEMemoryLeakStartPassTimer")
    endfunction
    
    private function StartDestroyThread takes nothing returns nothing
        if DestroyThreadRunning == false then
            set DestroyThreadRunning = true
            call TimerStart(CleanTimer,CLEAN_UP_INTERVAL,false,function DestroyMemoryLeaks)
            call PauseTimer(PassTimer)
        endif
    endfunction
    
   // hook DoNothing StartDestroyThread
    
    // We want that the user doesn't have to protect too many variables, but all the variables that are filled longer
    // than CLEAN_UP_INTERVAL seconds. But what, when the handle thing is put into the destroy stack and the next destroy is
    // in 5 seconds, because the last one was 15 seconds ago? We can simply avoid something like that by using a 2-step-system
    // that goes sure, the handle is only destroyed when it passed the CLEAN_UP_INTERVAL twice.
    // Having two kinds of variables is simply easier and more efficient than having another variable that refers to
    // how many times the handle passed the timer; If it isn't passed/cleared in the Interval then, we can't loop
    // that easily through the data and we'd have to fix gaps later; That would suck a lot of performacne.
    private function PassMemoryLeaks takes nothing returns nothing        
		set CaughtLeaks = CaughtLeaks + CaughtGroupLeaks
        loop
            exitwhen CaughtGroupLeaks < 1
            if IsSaved.exists(GroupLeakData[CaughtGroupLeaks]) == false and GroupLeakData[CaughtGroupLeaks] != null then
                call AddToGroupDestroyQueue(GroupLeakData[CaughtGroupLeaks])
            endif
            set GroupLeakData[CaughtGroupLeaks] = null
            set CaughtGroupLeaks = CaughtGroupLeaks - 1
        endloop
		
		set CaughtLeaks = CaughtLeaks + CaughtLocationLeaks
        loop
            exitwhen CaughtLocationLeaks < 1
            if IsSaved.exists(LocationLeakData[CaughtLocationLeaks]) == false and LocationLeakData[CaughtLocationLeaks] != null then
                call AddToLocationDestroyQueue(LocationLeakData[CaughtLocationLeaks])
            endif
            set LocationLeakData[CaughtLocationLeaks] = null
            set CaughtLocationLeaks = CaughtLocationLeaks - 1
        endloop
		
		set CaughtLeaks = CaughtLeaks + CaughtEffectLeaks
        loop
            exitwhen CaughtEffectLeaks < 1
            if IsSaved.exists(EffectLeakData[CaughtEffectLeaks]) == false and EffectLeakData[CaughtEffectLeaks] != null then
                call AddToEffectDestroyQueue(EffectLeakData[CaughtEffectLeaks])
            endif
            set EffectLeakData[CaughtEffectLeaks] = null
            set CaughtEffectLeaks = CaughtEffectLeaks - 1
        endloop
		
        if CaughtLeaks > MIN_LEAK_NUMBER then
            set CaughtLeaks = 0
            //call BJDebugMsg("Caught Leaks: "+I2S(MIN_LEAK_NUMBER))
            //call BJDebugMsg("Now start Destroy Timer")
            set DestroyThreadRunning = true
            call TimerStart(CleanTimer,CLEAN_UP_INTERVAL,false,function DestroyMemoryLeaks)
            // We have to pause this timer a bit; Otherwise it would break the CLEAN_UP_INTERVAL rule.
            call PauseTimer(PassTimer)
        endif
        
    endfunction
    
    // =================================
    // ============= Usage =============
    // =================================
    
    private function PP takes location source, real dist, real angle returns nothing
        call CatchLocation(source)
    endfunction
    
    private function CU takes integer count, integer unitId, player p, location l, real face returns nothing
        call CatchLocation(l)
    endfunction
    
    private function IPO takes unit k, string order, location l returns nothing
        call CatchLocation(l)
    endfunction
    
    private function SUP takes unit who, location l returns nothing
        call CatchLocation(l)
    endfunction
    
    private function SUF takes unit who, location l, real dur returns nothing
        call CatchLocation(l)
    endfunction
    
    private function GUR takes real radius, location l, boolexpr filter returns nothing
        call CatchLocation(l)
    endfunction
    
    private function CUF takes integer count, integer unitId, player whichPlayer, location loc, location lookAt returns nothing
        call CatchLocation(loc)
        call CatchLocation(lookAt)
    endfunction
    
    hook PolarProjectionBJ PP
    hook CreateNUnitsAtLoc CU
    hook CreateNUnitsAtLocFacingLocBJ CUF
    hook IssuePointOrderLocBJ IPO
    hook SetUnitPositionLoc SUP
    hook SetUnitFacingToFaceLocTimed SUF
    hook GetUnitsInRangeOfLocMatching GUR

    hook RemoveLocation ReleaseLocation

    private function FG takes group g, code callback returns nothing
        call CatchGroup(g)
    endfunction
    
    hook ForGroupBJ FG // :D This should catch all GUI usages for groups.
    hook GroupPickRandomUnit CatchGroup
    hook CountUnitsInGroup CatchGroup
    
    hook DestroyGroup ReleaseGroup
    
    private function ASETU takes string bla, widget d, string blu returns nothing
        // We can not catch THIS effect, but the effect that was created before.
        // So we can destroy all SpecialEffects excpet one.
        call CatchEffect(GetLastCreatedEffectBJ())
    endfunction
    
    private function ASE takes location where, string modelName returns nothing
        call CatchLocation(where)
        call CatchEffect(GetLastCreatedEffectBJ())
    endfunction

    hook AddSpecialEffectLocBJ ASE
    hook AddSpecialEffectTargetUnitBJ ASETU
    hook DestroyEffect ReleaseEffect
    hook DestroyEffectBJ ReleaseEffect
    
    // When I want to make the timer run the PassMemoryLeaks things, I have to use an .execute command which requires an extra func.
    function YDWEMemoryLeakStartPassTimer takes nothing returns nothing
        //call BJDebugMsg("Restarting PassTimer")
        call TimerStart(PassTimer,PASS_INTERVAL,true,function PassMemoryLeaks)
    endfunction
    
	// 开启接口函数
    function YDWEMemoryLeakHelperMapIsOpenMemoryLeakHelper takes boolean IsOpenMemoryLeak returns nothing
        set IsOpenMemoryLeakHelper = IsOpenMemoryLeak
        call PauseTimer(PassTimer)
        if ( IsOpenMemoryLeak ) then
            call YDWEMemoryLeakStartPassTimer()
        endif
    endfunction
    
    function YDWEMemoryLeakHelperProtectLastCaughtHandle takes nothing returns nothing
        call YDWEMemoryLeakHelperProtectHandle(YDWEMemoryLeakHelperGetLastCaughtHandle())
    endfunction
    
    function YDWEMemoryLeakHelperProtectLocation takes location loc returns nothing
        call YDWEMemoryLeakHelperProtectHandle(loc)
    endfunction

    function YDWEMemoryLeakHelperProtectGroup takes group g returns nothing
        call YDWEMemoryLeakHelperProtectHandle(g)
    endfunction
    
    function YDWEMemoryLeakHelperProtectEffect takes effect e returns nothing
        call YDWEMemoryLeakHelperProtectHandle(e)
    endfunction
        
    private function Init takes nothing returns nothing
        set IndexData = HandleTable.create()
        set IsSaved = HandleTable.create()
        
        //call YDWEMemoryLeakStartPassTimer()
        call TimerStart(GameTimeTimer,GAMETIME_TIMER_INTERVAL,true,function UpdateGameTime)
    endfunction
    
endlibrary

#endif /// YDWEMemoryLeakHelperIncluded
