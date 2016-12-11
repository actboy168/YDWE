

globals
  string array udg_YDWE_PreloadSL_List 
  string udg_YDWE_PreloadSL_CurrentDir = ""
endglobals 

function YDWE_PreloadSL_OpenDir takes string dir returns nothing 
  local integer index
  
  set index = 0
  loop
    exitwhen index >= 16
    set udg_YDWE_PreloadSL_List[16+index] = GetPlayerName(Player(index))
    call SetPlayerName(Player(index), udg_YDWE_PreloadSL_List[index])
    set index = index + 1
  endloop  
  call Preloader(YDWE_PRELOADSL_LIST_FILE(dir))  
  set index = 0
  loop
    exitwhen index >= 16
    set udg_YDWE_PreloadSL_List[index] = GetPlayerName(Player(index)) 
    call SetPlayerName(Player(index), udg_YDWE_PreloadSL_List[16+index])
    set index = index + 1
  endloop
endfunction 

function YDWE_PreloadSL_CloseDir takes string dir returns nothing
  local integer index
  
  call PreloadGenClear()
  call PreloadGenStart()

  set index = 0
  loop
    exitwhen index >= 16
    call Preload("\" )
    call SetPlayerName(Player("+I2S(index)+"),\""+udg_YDWE_PreloadSL_List[index]+"\")
    //")
    set index = index + 1
  endloop         
  call Preload("\" )
endfunction    

function YDWE_PreloadSL_ListFile takes nothing returns nothing
  //")
  call PreloadGenEnd(YDWE_PRELOADSL_LIST_FILE(dir))
endfunction
  
function YDWE_PreloadSL_SetFile takes player p, string dir, string value, integer n returns nothing
  if GetLocalPlayer() == p then
    if udg_YDWE_PreloadSL_CurrentDir == "" then
      call YDWE_PreloadSL_OpenDir(dir)
      set udg_YDWE_PreloadSL_CurrentDir = dir
    elseif udg_YDWE_PreloadSL_CurrentDir != dir then 
      call YDWE_PreloadSL_CloseDir(udg_YDWE_PreloadSL_CurrentDir)
      call YDWE_PreloadSL_OpenDir(dir) 
      set udg_YDWE_PreloadSL_CurrentDir = dir
    endif 
    
    if 0 < n and n < 16 then
      set udg_YDWE_PreloadSL_List[n-1] = value
      call YDWE_PreloadSL_CloseDir(udg_YDWE_PreloadSL_CurrentDir)
    endif  
  endif
endfunction 
    
function YDWE_PreloadSL_GetFile takes player p, string dir, integer n returns string
  if GetLocalPlayer() == p then
    if udg_YDWE_PreloadSL_CurrentDir == "" then 
      call YDWE_PreloadSL_OpenDir(dir)
      set udg_YDWE_PreloadSL_CurrentDir = dir
    elseif udg_YDWE_PreloadSL_CurrentDir != dir then 
      call YDWE_PreloadSL_CloseDir(udg_YDWE_PreloadSL_CurrentDir)
      call YDWE_PreloadSL_OpenDir(dir) 
      set udg_YDWE_PreloadSL_CurrentDir = dir
    endif 
    
    if 0 < n and n < 16 then 
      return udg_YDWE_PreloadSL_List[n-1] 
    else
      return "<null>"
    endif 
  else
    return "NotBelongToYou"
  endif
endfunction
    
