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

	function YDWEDisplayChat takes player p, integer chat_recipient, string message returns nothing
		call EXDisplayChat(p, chat_recipient, message)
	endfunction
endlibrary

#endif  /// YDWEYDWEJapiOtherIncluded
