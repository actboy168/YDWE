function YDWE_PreloadSL_GetMask takes player p returns integer
    local integer value            
    local integer beg = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
    local integer hi = CodeHI[beg]
    local integer lo = CodeLO[beg]
    if hi < $8000 then
        set value = lo+hi*$10000
    else
        set hi = hi-$8000
        set value = -(lo+hi*$10000)
    endif
    set Code[beg] = value
    return value - YDWE_PRELOADSL_HEAD_MASK
endfunction

///
/// 注重效率而不是安全的移动，话说安全有意义吗...
/// 另一个移动函数的逆运算
///
function YDWE_PreloadSL_Unmove takes integer beg, integer n, integer key returns nothing 
    local integer value
    local integer keys = n*key
    local integer index = beg + n
    local integer hi
    local integer lo

    set beg = beg + 1
    loop
        exitwhen index < beg

        set value = keys + (index - beg)
        set value = beg + value - value/n*n
        if value < beg then
            set value = value + n
        endif
        set hi = CodeHI[value]
        set CodeHI[value] = CodeHI[index]
        set CodeHI[index] = hi
        
        set value = keys - (index - beg)
        set value = beg + value - value/n*n
        if value < beg then
            set value = value + n
        endif
        set lo = CodeLO[value]
        set CodeLO[value] = CodeLO[index]
        set CodeLO[index] = lo
        
        set keys = keys - key
        
        set index = index - 1
    endloop
endfunction

function YDWE_PreloadSL_GetCode takes integer beg, integer n, integer key, integer mask returns boolean 
    local integer hi
    local integer lo 
    local integer value
    local integer masks = 0
    local integer keys = 0
    local integer index = beg + 1
    local integer end = beg + n
    
    loop
        exitwhen index > end
        set hi = CodeHI[index]
        set lo = CodeLO[index]
        
        // SubKey
        set keys = keys + key
        set hi = hi - keys
        set lo = lo + keys
        
        // SubMask
        set masks = masks + mask
        set value = masks/$10000
        set hi = hi - (value - value/$100*$100)*$100
        set lo = lo - (masks - masks/$100*$100)*$10000
        
        // CheckMask
        if ((hi/$10000) != 0) then
            return false
        endif
        if ((lo/$10000) != 0) then
            return false
        endif
        
        // GetCode
        if hi < $8000 then
            set value = lo+hi*$10000
        else
            set hi = hi-$8000
            set value = -(lo+hi*$10000)
        endif
        
        set Code[index] = value
        set index = index + 1
    endloop
    
    return true
endfunction

function YDWE_PreloadSL_Decode takes player p, integer n returns boolean
    local integer key    = YDWE_PRELOADSL_GET_KEY(p)
    local integer beg    = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
    local integer mask = YDWE_PreloadSL_GetMask(p)
    call YDWE_PreloadSL_Unmove(beg, n, key)    
    return YDWE_PreloadSL_GetCode(beg, n, key, mask)
endfunction

function YDWE_PreloadSL_SetMask takes player p, integer mask returns nothing    
    local integer hi
    local integer lo         
    local integer beg = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
    local integer value = YDWE_PRELOADSL_HEAD_MASK + mask 
    if value >= 0 then
            set hi = value/$10000
            set lo = value-hi*$10000
    else
            set value = -value
            set hi = value/$10000
            set lo = value-hi*$10000
            set hi = hi+$8000
    endif             
    set CodeHI[beg] = hi
    set CodeLO[beg] = lo
endfunction

function YDWE_PreloadSL_SetCode takes integer beg, integer n, integer key, integer mask returns nothing 
    local integer hi
    local integer lo 
    local integer value
    local integer masks = 0
    local integer keys = 0
    local integer index = beg + 1
    local integer end = beg + n
    
    loop
        exitwhen index > end
        set value = Code[index]
        
        // SetCode
        if value >= 0 then
            set hi = value/$10000
            set lo = value-hi*$10000
        else
            set value = -value
            set hi = value/$10000
            set lo = value-hi*$10000
            set hi = hi+$8000
        endif
        // AddKey
        set keys = keys + key
        set hi = hi + keys
        set lo = lo - keys
        
        // AddMask
        set masks = masks + mask
        set value = masks/$10000
        set hi = hi + (value - value/$100*$100)*$100
        set lo = lo + (masks - masks/$100*$100)*$10000

        set CodeHI[index] = hi
        set CodeLO[index] = lo
        set index = index + 1
    endloop
endfunction

///
/// 注重效率而不是安全的移动，话说安全有意义吗...
/// 移动函数，甚至可以不要...
///
function YDWE_PreloadSL_Move takes integer beg, integer n, integer key returns nothing 
    local integer value
    local integer keys = 0
    local integer index = beg + 1
    local integer end = beg + n
    local integer hi
    local integer lo

    set beg = beg + 1
    loop
        exitwhen index > end
        
        set keys = keys + key
        
        set value = keys + (index - beg)
        set value = beg + value - value/n*n
        if value < beg then
            set value = value + n
        endif
        set hi = CodeHI[value]
        set CodeHI[value] = CodeHI[index]
        set CodeHI[index] = hi
        
        set value = keys - (index - beg)
        set value = beg + value - value/n*n
        if value < beg then
            set value = value + n
        endif
        set lo = CodeLO[value]
        set CodeLO[value] = CodeLO[index]
        set CodeLO[index] = lo
        
        set index = index + 1
    endloop
endfunction

function YDWE_PreloadSL_Encode takes player p, integer n, integer mask returns nothing
    local integer key = YDWE_PRELOADSL_GET_KEY(p)
    local integer beg = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
    call YDWE_PreloadSL_SetMask(p, mask)    
    call YDWE_PreloadSL_SetCode(beg, n, key, mask)
    call YDWE_PreloadSL_Move(beg, n, key)
endfunction                         

/// 
/// 同步玩家数据
/// 同一时间不要同步一个以上的玩家数据
///
function YDWE_PreloadSL_Sync takes player p, integer n returns nothing
    local integer index
    local integer beg = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
    local integer end = beg + n                                

    if GetLocalPlayer() == p then
        set index = beg
        loop
            exitwhen index > end
            call YDWESync_Set("H", I2S(index), CodeHI[index])
            call YDWESync_Set("L", I2S(index), CodeLO[index])
            set index = index + 1
        endloop
    endif                             

    call YDWESync_Start()
    if GetLocalPlayer() == p then 
        set index = beg
        loop 
            exitwhen index > end 
            call YDWESync_Send("H", I2S(index))
            call YDWESync_Send("L", I2S(index))
            set index = index + 1
        endloop     
    endif
    call YDWESync_Ready()
    //call TriggerSleepAction(0.2)
    call YDWESync_Join(p)
    set index = beg
    loop
        exitwhen index > end 
        set CodeHI[index] = YDWESync_Get("H", I2S(index))
        set CodeLO[index] = YDWESync_Get("L", I2S(index))
        set index = index + 1
    endloop
endfunction

function YDWE_PreloadSL_Load takes player p, string dir, string file, integer n returns boolean
    call YDWESync_Lock()
    set n = n + 1
    
    if GetLocalPlayer() == p then
        call YDWE_PreloadSL_LoadPreFile(p, n, YDWE_PRELOADSL_DATA_FILE(dir, file))
    endif 
    
    call YDWE_PreloadSL_Sync(p, n)
    set bj_lastLoadPreloadSLResult = YDWE_PreloadSL_Decode(p, n) 
    call YDWESync_Unlock() 
    
    return bj_lastLoadPreloadSLResult
endfunction

function YDWE_PreloadSL_Save takes player p, string dir, string file, integer n returns nothing
    local integer mask = YDWE_PRELOADSL_GET_RND_MASK()    
    set n = n + 1
    if GetLocalPlayer() == p then
        call YDWE_PreloadSL_Encode(p, n, mask)
        call YDWE_PreloadSL_SavePreFile(p, n, YDWE_PRELOADSL_DATA_FILE(dir, file))
    endif
endfunction
 
function YDWE_PreloadSL_Set takes player p, string s, integer n, integer value returns nothing
    set Code[n+YDWE_PRELOADSL_CODE_INDEX_BEG(p)] = value
endfunction

function YDWE_PreloadSL_Get takes player p, string s, integer n returns integer
    return Code[n+YDWE_PRELOADSL_CODE_INDEX_BEG(p)]
endfunction

