#ifndef YDWEBitwiseIncluded
#define YDWEBitwiseIncluded

library YDWEBitwise initializer onInit
	globals 
		private integer array C2
		private constant string hexstr = "0123456789ABCDEF"
    endglobals
    
    private function Bit takes integer x returns boolean
        return (x - x/2*2) !=0
    endfunction 
    
	private function NOT_P takes integer x returns integer
		local integer z = 0
		local integer i = 0
	
		loop
			exitwhen (i == 31)
			if not Bit(x) then
				set z = z + C2[i]
			endif
			set x = x / 2
			set i = i + 1	
		endloop
	
		return z
	endfunction
	public function NOT takes integer x returns integer
		if x >= 0 then
            return NOT_P(x) + C2[31]
        else
            return -x-1
        endif
	endfunction
    
    private function AND_P takes integer x, integer y returns integer
		local integer z = 0
		local integer i = 0
	
		loop
			exitwhen (i == 31)
			if Bit(x) and Bit(y) then
				set z = z + C2[i]
			endif
			set x = x / 2
			set y = y / 2
			set i = i + 1	
		endloop
	
		return z
    endfunction    
    public function AND takes integer x, integer y returns integer
		if x >= 0 then
            if y >= 0 then
                return AND_P(x, y)
            else
                return AND_P(x, NOT_P(-y-1))
            endif
        else
            if y >= 0 then
                return AND_P(NOT_P(-x-1), y)
            else
                return AND_P(NOT_P(-x-1), NOT_P(-y-1)) + C2[31]
            endif
        endif
    endfunction
    
    private function OR_P takes integer x, integer y returns integer
		local integer z = 0
		local integer i = 0
	
		loop
			exitwhen (i == 31)
			if Bit(x) or Bit(y) then
				set z = z + C2[i]
			endif
			set x = x / 2
			set y = y / 2
			set i = i + 1	
		endloop
	
		return z
    endfunction    
    public function OR takes integer x, integer y returns integer
		if x >= 0 then
            if y >= 0 then
                return OR_P(x, y)
            else
                return OR_P(x, NOT_P(-y-1)) + C2[31]
            endif
        else
            if y >= 0 then
                return OR_P(NOT_P(-x-1), y) + C2[31]
            else
                return OR_P(NOT_P(-x-1), NOT_P(-y-1)) + C2[31]
            endif
        endif
    endfunction

    private function XOR_P takes integer x, integer y returns integer
		local integer z = 0
		local integer i = 0
	
		loop
			exitwhen (i == 31)
			if Bit(x) != Bit(y) then
				set z = z + C2[i]
			endif
			set x = x / 2
			set y = y / 2
			set i = i + 1	
		endloop
	
		return z
    endfunction    
    public function XOR takes integer x, integer y returns integer
		if x >= 0 then
            if y >= 0 then
                return XOR_P(x, y)
            else
                return XOR_P(x, NOT_P(-y-1)) + C2[31]
            endif
        else
            if y >= 0 then
                return XOR_P(NOT_P(-x-1), y) + C2[31]
            else
                return XOR_P(NOT_P(-x-1), NOT_P(-y-1))
            endif
        endif
    endfunction
    
    private function LShift_P takes integer x, integer n returns integer
        return x * C2[n] 
    endfunction
    public function LShift takes integer x, integer n returns integer
        return x * C2[n] 
    endfunction
    
    private function RShift_P takes integer x, integer n returns integer
        return x / C2[n] 
    endfunction
    public function RShift takes integer x, integer n returns integer
		if x >= 0 then
            return RShift_P(x, n)
        else
            return RShift_P(NOT_P(-x-1), n)+C2[31-n]
        endif
    endfunction
    
    private function ToHexChar takes integer x returns string
        return SubString(hexstr, x, x+1)
    endfunction    
    private function ToHexChar7 takes integer x returns string
        local string s = ""
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        set s = ToHexChar(x-x/$10*$10) + s
        set x = x/$10
        return  ToHexChar(x-x/$10*$10) + s
    endfunction
    public function ToHexString takes integer x returns string
		if x >= 0 then
            return ToHexChar(x/$10000000)+ ToHexChar7(x)
        else
            set x = NOT_P(-x-1)
            return ToHexChar($8+x/$10000000)+ ToHexChar7(x)
        endif
    endfunction
/*
    private function UnitTest takes nothing returns nothing
        local boolean b = true
#define UNIT_TEST(conf) if not (conf) then YDNL call BJDebugMsg("[BitwiseTest] 错误：" + #conf) YDNL set b = false YDNL endif
        UNIT_TEST(NOT($10000007) == $EFFFFFF8)
        UNIT_TEST(AND($10000007, $101) == $1)
        UNIT_TEST(OR ($10000007, $101) == $10000107)
        UNIT_TEST(XOR($10000007, $101) == $10000106)
        UNIT_TEST(RShift($10000001, 20) == $100)
        UNIT_TEST(LShift($10000001, 20) == $100000)
        UNIT_TEST(LShift($F0000001, 1) == $E0000002)
        UNIT_TEST(LShift($70000001, 1) == $E0000002)
        UNIT_TEST(NOT($80000001) == $7FFFFFFE)
        UNIT_TEST(AND($80000001, $101) == $1)
        UNIT_TEST(OR ($80000001, $101) == $80000101)
        UNIT_TEST(XOR($80000001, $101) == $80000100)
        UNIT_TEST(RShift($80000001, 20) == $800)
        UNIT_TEST(LShift($80000001, 20) == $100000)
        UNIT_TEST(LShift($80000001, 0) == $80000001)
        UNIT_TEST(ToHexString($34567891) == "34567891")
        UNIT_TEST(ToHexString($ABCDEF02) == "ABCDEF02")
        UNIT_TEST(ToHexString($FADF2312) == "FADF2312")
#undef  UNIT_TEST
        if b then
            call BJDebugMsg("[BitwiseTest] 所有测试通过！")
        endif
    endfunction
    private function UnitTestInit takes nothing returns nothing
        local trigger t = CreateTrigger()
        call TriggerRegisterPlayerEvent(t, Player(0), EVENT_PLAYER_END_CINEMATIC)
        call TriggerAddAction(t, function UnitTest)
        set t = null
    endfunction
*/

	private function onInit takes nothing returns nothing
		local integer i = 1
		set C2[0] = 1
		loop
			exitwhen (i == 32)
			set C2[i] = C2[i-1]*2
			set i = i + 1
		endloop
/*
        call UnitTestInit()
*/
	endfunction    
endlibrary

#endif
