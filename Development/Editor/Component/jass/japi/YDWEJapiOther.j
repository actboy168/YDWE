#ifndef YDWEYDWEJapiOtherIncluded 
#define YDWEYDWEJapiOtherIncluded

library YDWEYDWEJapiOther
	globals		
    	private constant integer CHAT_RECIPIENT_ALL       = 0        // [所有人]
    	private constant integer CHAT_RECIPIENT_ALLIES    = 1        // [盟友]
    	private constant integer CHAT_RECIPIENT_OBSERVERS = 2        // [观看者]
    	private constant integer CHAT_RECIPIENT_REFEREES  = 2        // [裁判]
    	private constant integer CHAT_RECIPIENT_PRIVATE   = 3        // [私人的]    	
	endglobals

	native EXDisplayChat     takes player p, integer chat_recipient, string message returns nothing
	native EXGetUnitId       takes unit u returns integer
	native EXSetUnitId       takes unit u, integer id returns nothing
	native EXRevealTrainingProcess takes boolean flag returns nothing
	native EXGetGameBaseAdr  takes nothing returns integer

	function YDWEDisplayChat takes player p, integer chat_recipient, string message returns nothing
		call EXDisplayChat(p, chat_recipient, message)
	endfunction

	function YDWEGetUnitId takes unit u returns integer
		return EXGetUnitId(u)
	endfunction
	
	function YDWESetUnitId takes unit u, integer id returns nothing
		call EXSetUnitId(u, id)
	endfunction
	
	function YDWERevealTrainingProcess takes boolean enable returns nothing
		call EXRevealTrainingProcess(enable)
	endfunction
	
	function YDWEGetGameBaseAdr takes nothing returns integer
		return EXGetGameBaseAdr()
	endfunction
endlibrary

#endif  /// YDWEYDWEJapiOtherIncluded
