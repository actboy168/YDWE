
library YDWERPGBilling
    globals
        private gamecache array m_table
    endglobals
    private function ToLetter takes integer i returns string
        return SubString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", i, i+1)
    endfunction
    private function GetTable takes integer playerid returns gamecache
        if m_table[playerid] == null then
            set m_table[playerid] = InitGameCache("11billing@"+ToLetter(playerid))
        endif
        return m_table[playerid]
    endfunction
    function YDWERPGBillingGetStatus takes player p, string key returns integer
        return GetStoredInteger(GetTable(GetPlayerId(p)), "状态", key)
    endfunction
    function YDWERPGBillingGetItem takes player p, string key returns integer
        return GetStoredInteger(GetTable(GetPlayerId(p)), "道具", key)
    endfunction
    function YDWERPGBillingHasStatus takes player p, string key returns boolean
        return HaveStoredInteger(GetTable(GetPlayerId(p)), "状态", key)
    endfunction
    function YDWERPGBillingHasItem takes player p, string key returns boolean
        return HaveStoredInteger(GetTable(GetPlayerId(p)), "道具", key)
    endfunction
endlibrary

