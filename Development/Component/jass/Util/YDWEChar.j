#ifndef YDWECharIncluded
#define YDWECharIncluded

library YDWEChar
    ///           
	/// 预处理器无法处理0x01~0x1F，所以这里没有0x01~0x0F 
	/// 1.20下0xFF有bug 
	/// 
    globals
		#include "charlist.h"
	endglobals
	
    public function chr takes integer n returns string
        if $20 <= n and n < $FF then
            return SubString(originstr, n-$20, n-$1F)
        endif
        
        return ""
    endfunction
    
	private function ord_hi takes string s returns integer
		local integer a = $60
	
		loop
			exitwhen (a > $DE)
			if (s == SubString(originstr, a, a+1)) then
				return a + $20
			endif
			set a = a + 1
		endloop
	
		return $FF
	endfunction
    
    public function ord takes string s returns integer		
        if s == " " then
			return 32
		endif
		if s == "!" then
			return 33
		endif
		if s == "\"" then
			return 34
		endif
		if s == "#" then
			return 35
		endif
		if s == "$" then
			return 36
		endif
		if s == "%" then
			return 37
		endif
		if s == "&" then
			return 38
		endif
		if s == "'" then
			return 39
		endif
		if s == "(" then
			return 40
		endif
		if s == ")" then
			return 41
		endif
		if s == "*" then
			return 42
		endif
		if s == "+" then
			return 43
		endif
		if s == "," then
			return 44
		endif
		if s == "-" then
			return 45
		endif
		if s == "." then
			return 46
		endif
		if s == "/" then
			return 47
		endif
		if s == "0" then
			return 48
		endif
		if s == "1" then
			return 49
		endif
		if s == "2" then
			return 50
		endif
		if s == "3" then
			return 51
		endif
		if s == "4" then
			return 52
		endif
		if s == "5" then
			return 53
		endif
		if s == "6" then
			return 54
		endif
		if s == "7" then
			return 55
		endif
		if s == "8" then
			return 56
		endif
		if s == "9" then
			return 57
		endif
		if s == ":" then
			return 58
		endif
		if s == ";" then
			return 59
		endif
		if s == "<" then
			return 60
		endif
		if s == "=" then
			return 61
		endif
		if s == ">" then
			return 62
		endif
		if s == "?" then
			return 63
		endif
		if s == "@" then
			return 64
		endif
		if s == "A" then
			return 65
		endif
		if s == "B" then
			return 66
		endif
		if s == "C" then
			return 67
		endif
		if s == "D" then
			return 68
		endif
		if s == "E" then
			return 69
		endif
		if s == "F" then
			return 70
		endif
		if s == "G" then
			return 71
		endif
		if s == "H" then
			return 72
		endif
		if s == "I" then
			return 73
		endif
		if s == "J" then
			return 74
		endif
		if s == "K" then
			return 75
		endif
		if s == "L" then
			return 76
		endif
		if s == "M" then
			return 77
		endif
		if s == "N" then
			return 78
		endif
		if s == "O" then
			return 79
		endif
		if s == "P" then
			return 80
		endif
		if s == "Q" then
			return 81
		endif
		if s == "R" then
			return 82
		endif
		if s == "S" then
			return 83
		endif
		if s == "T" then
			return 84
		endif
		if s == "U" then
			return 85
		endif
		if s == "V" then
			return 86
		endif
		if s == "W" then
			return 87
		endif
		if s == "X" then
			return 88
		endif
		if s == "Y" then
			return 89
		endif
		if s == "Z" then
			return 90
		endif
		if s == "[" then
			return 91
		endif
		if s == "\\" then
			return 92
		endif
		if s == "]" then
			return 93
		endif
		if s == "^" then
			return 94
		endif
		if s == "_" then
			return 95
		endif
		if s == "`" then
			return 96
		endif
		if s == "a" then
			return 97
		endif
		if s == "b" then
			return 98
		endif
		if s == "c" then
			return 99
		endif
		if s == "d" then
			return 100
		endif
		if s == "e" then
			return 101
		endif
		if s == "f" then
			return 102
		endif
		if s == "g" then
			return 103
		endif
		if s == "h" then
			return 104
		endif
		if s == "i" then
			return 105
		endif
		if s == "j" then
			return 106
		endif
		if s == "k" then
			return 107
		endif
		if s == "l" then
			return 108
		endif
		if s == "m" then
			return 109
		endif
		if s == "n" then
			return 110
		endif
		if s == "o" then
			return 111
		endif
		if s == "p" then
			return 112
		endif
		if s == "q" then
			return 113
		endif
		if s == "r" then
			return 114
		endif
		if s == "s" then
			return 115
		endif
		if s == "t" then
			return 116
		endif
		if s == "u" then
			return 117
		endif
		if s == "v" then
			return 118
		endif
		if s == "w" then
			return 119
		endif
		if s == "x" then
			return 120
		endif
		if s == "y" then
			return 121
		endif
		if s == "z" then
			return 122
		endif
		if s == "{" then
			return 123
		endif
		if s == "|" then
			return 124
		endif
		if s == "}" then
			return 125
		endif
		if s == "~" then
			return 126
		endif
		if s == "" then
			return 127
		endif
		if s == "�" then
			return 128
		endif
        return ord_hi(s)
    endfunction
endlibrary

#endif
