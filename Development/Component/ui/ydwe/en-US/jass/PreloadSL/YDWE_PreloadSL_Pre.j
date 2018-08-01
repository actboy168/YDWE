function YDWE_PreloadSL_SaveInteger takes integer key, integer value returns nothing
  if value >= 0 then
    call Preload("\" )
    call SetPlayerTechMaxAllowed(Player(14),"+I2S(key)+",1)
    call SetPlayerTechMaxAllowed(Player(15),"+I2S(key)+","+I2S(value)+")
    //")
  else
    call Preload("\" )
    call SetPlayerTechMaxAllowed(Player(14),"+I2S(key)+",2)
    call SetPlayerTechMaxAllowed(Player(15),"+I2S(key)+","+I2S(-value)+")
    //")
  endif
endfunction

function YDWE_PreloadSL_LoadInteger takes integer key returns integer
  local integer var_type = GetPlayerTechMaxAllowed(Player(14), key)
    
  if var_type == 1 then
    return GetPlayerTechMaxAllowed(Player(15), key)
  elseif var_type == 2 then
    return -GetPlayerTechMaxAllowed(Player(15), key)
  else
    return 0
  endif
endfunction

function YDWE_PreloadSL_LoadPreFile takes player p, integer n, string strFilePath returns nothing
  local integer index = 0
  local integer beg   = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
  
  call Preloader(strFilePath)
  
  loop
    exitwhen index > n
    set CodeHI[beg+index] = YDWE_PreloadSL_LoadInteger(index)
    set CodeLO[beg+index] = YDWE_PreloadSL_LoadInteger($200+index)
    set index = index + 1
  endloop
endfunction

function YDWE_PreloadSL_SavePreFile takes player p, integer n, string strFilePath returns nothing
  local integer index = 0
  local integer beg   = YDWE_PRELOADSL_CODE_INDEX_BEG(p)
  
  call PreloadGenClear()
  call PreloadGenStart()

  loop
    exitwhen index > n
    call YDWE_PreloadSL_SaveInteger(index, CodeHI[beg+index])
    call YDWE_PreloadSL_SaveInteger($200+index, CodeLO[beg+index])
    set index = index + 1
  endloop
          
  call Preload("\" )
endfunction    
function YDWE_PreloadSL_DataFile takes nothing returns nothing
  //")
  call PreloadGenEnd(strFilePath)
endfunction

