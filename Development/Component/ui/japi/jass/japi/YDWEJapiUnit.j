#ifndef YDWEJapiUnitIncluded
#define YDWEJapiUnitIncluded

library YDWEJapiUnit
	native EXSetUnitFacing takes unit u, real angle returns nothing
	native EXPauseUnit takes unit u, boolean flag returns nothing
	native EXSetUnitCollisionType takes boolean enable, unit u, integer t returns nothing
	native EXSetUnitMoveType takes unit u, integer t returns nothing

	function YDWEUnitAddStun takes unit u returns nothing
		call EXPauseUnit(u, true)
	endfunction

	function YDWEUnitRemoveStun takes unit u returns nothing
		call EXPauseUnit(u, false)
	endfunction
endlibrary

#endif  /// YDWEJapiUnitIncluded
