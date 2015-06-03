#ifndef YDWERecordSystemIncluded
#define YDWERecordSystemIncluded

library YDWERecord
    globals
        private gamecache array m_table
    endglobals

    private function ToLetter takes integer i returns string
        return SubString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", i, i+1)
    endfunction
    
    private function NewTable takes integer playerid returns gamecache
        return InitGameCache("11SAV@"+ToLetter(playerid))
    endfunction
 
    private function GetTable takes integer playerid returns gamecache
        if m_table[playerid] == null then
            set m_table[playerid] = NewTable(playerid)
        endif
        return m_table[playerid]
    endfunction
    
    private function SetS takes integer playerid, string key, string value returns nothing
        call StoreString(GetTable(playerid), "", key, value)
    endfunction
 
    // 指定平台界面显示的项
    // @reserve 保留参数，暂时无用
    // @index   显示的位置，值必须是0~7中的一个
    // @value   显示的标题，其值必须用SetI来设置
    public function SetTitle takes integer playerid, integer reserve, integer index, string value returns nothing
        call SetS(playerid, "Title@" + ToLetter(index), value)
    endfunction
    
    // 写入一项
    // @key    索引
    // @value  值
    public function SetI takes integer playerid, string key, integer value returns nothing
        call StoreInteger(GetTable(playerid), "", key, value)
    endfunction

    // 读取一项
    // @key    索引
    // @Return 值
    public function GetI takes integer playerid, string key returns integer
        return GetStoredInteger(GetTable(playerid), "", key)
    endfunction
    
    // 保存当前的所有内容。
    // @Return   成功与否
    public function Save takes integer playerid returns boolean
        return SaveGameCache(GetTable(playerid))
    endfunction
    
    // 清空所有内容
    public function Clear takes integer playerid returns nothing
        call FlushGameCache(GetTable(playerid))
    endfunction
endlibrary

library YDWERecordSystem requires YDWERecord
	function YDWERecordGetI takes player p,string kid returns integer
		return YDWERecord_GetI(GetPlayerId(p),kid)
	endfunction
	
	function YDWERecordSetI takes player p,string kid,integer data returns nothing
		call YDWERecord_SetI(GetPlayerId(p),kid,data)
	endfunction
	
	function YDWERecordSetTitle takes integer id,string kid returns nothing
		local integer i = 0                 
		loop
			exitwhen i > 11                  
			if ((GetPlayerController(Player(i)) == MAP_CONTROL_USER) and (GetPlayerSlotState(Player(i)) == PLAYER_SLOT_STATE_PLAYING)) then
				call YDWERecord_SetTitle(i,0,id,kid)
			endif
			set i = i + 1
		endloop
	endfunction
	
	function YDWERecordSave takes player p returns nothing
		if not IsPlayerObserver(p) then	
			call YDWERecord_Save(GetPlayerId(p))
		endif
	endfunction
	
	function YDWERecordClear takes player p returns nothing
		call YDWERecord_Clear(GetPlayerId(p))
	endfunction
endlibrary

library YDWE11Platform initializer Init
    globals
        private gamecache gc
		private boolean is_vaild = false
    endglobals  

	private function IsLivingPlayer takes player p returns boolean
		return (GetPlayerSlotState(p) == PLAYER_SLOT_STATE_PLAYING) /*
		*/ and (GetPlayerController(p) == MAP_CONTROL_USER)
	endfunction
	
	private function GetLivingfPlayer takes nothing returns player
		local integer i = 0
		loop
			exitwhen IsLivingPlayer(Player(i)) or i >= 11
			set i = i + 1
		endloop
		return Player(i)
	endfunction
	
	private function InitGC takes nothing returns boolean
		local player p = GetLivingfPlayer()
        if GetLocalPlayer() == p then
			call StoreInteger(gc, "Global", "RoomFlag", GetStoredInteger(gc, "Global", "RoomFlag"))
			call StoreInteger(gc, "Global", "Enable", GetStoredInteger(gc, "Global", "Enable"))
            call StoreInteger(gc, "-", "-", 'YDWE')
        endif
        call TriggerSyncStart()
        if GetLocalPlayer() == p then 
            call SyncStoredInteger(gc, "Global", "RoomFlag")
            call SyncStoredInteger(gc, "Global", "Enable")
            call SyncStoredInteger(gc, "-", "-")
        endif
		call StoreInteger(gc, "Global", "RoomFlag", 0)
		call StoreInteger(gc, "Global", "Enable", 0)
        call StoreInteger(gc, "-", "-", 0)
        call TriggerSyncReady()        
        loop
            if 'YDWE' == GetStoredInteger(gc, "-", "-") then
                return true
            endif
            call TriggerSleepAction(0.2)
        endloop
		
		return true
	endfunction
	
	function YDWEPlatformIsInRoom takes nothing returns boolean
		loop
			exitwhen is_vaild
            call TriggerSleepAction(0.2)
		endloop
		return GetStoredInteger(gc, "Global", "RoomFlag") == $40000000
	endfunction
	
	function YDWEPlatformIsInPlatform takes nothing returns boolean
		loop
			exitwhen is_vaild
            call TriggerSleepAction(0.2)
		endloop
		return GetStoredInteger(gc, "Global", "Enable") == 1
	endfunction
		
	private function Init takes nothing returns nothing
		set gc = InitGameCache("@11CONF")
		set is_vaild = false
		set is_vaild = InitGC()
	endfunction
endlibrary

#endif /// YDWERecordSystemIncluded
