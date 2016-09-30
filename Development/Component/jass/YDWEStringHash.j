#ifndef YDWEStringHashIncluded
#define YDWEStringHashIncluded
 
#if WARCRAFT_VERSION >= 124
library YDWEStringHash
endlibrary
#else
#include "Util/YDWEChar.j"
#include "Util/YDWEBitwise.j"
library YDWEStringHash requires YDWEChar, YDWEBitwise
	private function SH_StringChar takes string s, integer n returns integer
		local string ss = SubString(s, n, n+1)
		
		if (ss == "/") then
			set ss = "\\"
		endif
	
		return YDWEChar_ord(ss)
	endfunction
	
	private function SH_StringLong takes string s, integer n returns integer
		return ((SH_StringChar(s, n+3)*256+SH_StringChar(s, n+2))*256+SH_StringChar(s, n+1))*256+SH_StringChar(s, n)
	endfunction

	#define T1(A, B, C) YDNL \
        set A = YDWEBitwise_XOR(YDWEBitwise_RShift(C, 13), A - B - C) YDNL \
        set B = YDWEBitwise_XOR(YDWEBitwise_LShift(A,  8), B - C - A) YDNL \
        set C = YDWEBitwise_XOR(YDWEBitwise_RShift(B, 13), C - A - B) YDNL \
        set A = YDWEBitwise_XOR(YDWEBitwise_RShift(C, 12), A - B - C) YDNL \
        set B = YDWEBitwise_XOR(YDWEBitwise_LShift(A, 16), B - C - A) YDNL \
        set C = YDWEBitwise_XOR(YDWEBitwise_RShift(B,  5), C - A - B) YDNL \
        set A = YDWEBitwise_XOR(YDWEBitwise_RShift(C,  3), A - B - C) YDNL \
        set B = YDWEBitwise_XOR(YDWEBitwise_LShift(A, 10), B - C - A) YDNL \
        set C = YDWEBitwise_XOR(YDWEBitwise_RShift(B, 15), C - A - B)
	
	private function StringHashEx takes string s, integer len returns integer
		local integer n = 0
		local integer sizt_t = len
        local integer A = $9E3779B9
        local integer B = $9E3779B9
        local integer C = 0
	
		loop
			exitwhen len < 12
			set A = A + SH_StringLong(s, n)
			set B = B + SH_StringLong(s, n+4)
			set C = C + SH_StringLong(s, n+8)
            T1(A, B, C)
			set n = n + 12
			set len = len - 12
		endloop
	
        set C = C + sizt_t
	
		if (len == 11) then
            set C = C + SH_StringChar(s, n+10)*$01000000
			set len = len - 1
		endif
		if (len == 10) then
            set C = C + SH_StringChar(s, n+9)*$00010000
			set len = len - 1
		endif
		if (len == 9) then
            set C = C + SH_StringChar(s, n+8)*$00000100
			set len = len - 1
		endif
		if (len == 8) then
            set B = B + SH_StringChar(s, n+7)*$01000000
			set len = len - 1
		endif
		if (len == 7) then
            set B = B + SH_StringChar(s, n+6)*$00010000
			set len = len - 1
		endif 
		if (len == 6) then
            set B = B + SH_StringChar(s, n+5)*$00000100
			set len = len - 1
		endif
		if (len == 5) then
            set B = B + SH_StringChar(s, n+4)
			set len = len - 1
		endif
		if (len == 4) then
            set A = A + SH_StringChar(s, n+3)*$01000000
			set len = len - 1
		endif
		if (len == 3) then
            set A = A + SH_StringChar(s, n+2)*$00010000
			set len = len - 1
		endif
		if (len == 2) then
            set A = A + SH_StringChar(s, n+1)*$00000100
			set len = len - 1
		endif
		if (len == 1) then
            set A = A + SH_StringChar(s, n)
			set len = len - 1
		endif
	
        T1(A, B, C)
	#undef T1
		return C
	endfunction
	
	function StringHash takes string s returns integer
		return StringHashEx(StringCase(s, true), StringLength(s))
	endfunction
endlibrary
#endif

#endif
