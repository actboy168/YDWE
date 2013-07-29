// Edit By TigerCN
define {

    //printf(sign,""str) = printf(sign,NewStrId(str))

}

library StringArray

    globals
        integer STRING_INDEX_NONE       = -1
        string  STRING_INVALID_SEGMENT  = ""
    endglobals
    
        struct StringSegments
        
            string s
            string delim
            integer sLen
            integer dLen
            integer pos
            integer count
        
            method calculateCount takes nothing returns nothing
                local integer i = pos
                local integer prev = pos
                local integer count = 0
                if (dLen == 0 or sLen == 0) then
                    return
                endif
                loop
                    if (i >= sLen) then
                        if (i - prev > 0) then
                            set count = count + 1
                        endif
                        exitwhen true
                    endif
                    if (SubString(s, i, i + dLen) == delim ) then
                        if (i - prev > 0) then
                            set count = count + 1
                            set i = i + dLen
                            set prev = i
                        endif
                        set i = i + dLen
                        set prev = i
                    else 
                        set i = i + 1 
                    endif
                endloop
            endmethod

            static method create takes string s, string delim returns thistype
                local thistype t = thistype.allocate()
                set t.s = s
                set t.pos = 0
                set t.sLen = StringLength(s)
                set t.delim = delim
                set t.dLen = StringLength(delim)
                call t.calculateCount()
                return t
            endmethod
        
            method countSegments takes nothing returns integer 
                return count
            endmethod

            method hasMoreSegments takes nothing returns boolean
                return count > 0
            endmethod

            method nextSegment takes nothing returns string
                local integer i = pos
                local string str
                if (count <= 0) then
                    return STRING_INVALID_SEGMENT
                endif
                loop
                    loop
                        set str = SubString(s, i, i + dLen)
                        if (str == delim or str == "" or str == null) then
                            exitwhen true
                        endif
                        set i = i + 1
                    endloop
                    if (i - pos > 0) then
                        set str = SubString(s, pos, i)
                        set pos = i
                        set count = count - 1
                        exitwhen true
                    endif
                    set pos = i + dLen
                    set i = pos
                endloop
                return str
            endmethod
            
            method nextSegmentEx takes string line returns string
                set delim = line
                set dLen = StringLength(line)
                call calculateCount()
                return nextSegment()
            endmethod
            
        endstruct
        

endlibrary

library StrHash

    globals
        string array Trend_String
    endglobals
    
        function IntToStr takes string h returns integer
	        return h
            return 0
        endfunction
	    
        function NewStrId takes string h returns integer
	        local integer EnumId
            local integer ConvertedTimer = IntToStr(h)
            local integer StartId = ConvertedTimer - (ConvertedTimer / 8192) * 8192
	        if (StartId < 0) then 
                set StartId = StartId + 8192
            endif
	        set EnumId = StartId
            loop
                if (Trend_String[EnumId] == null) then
		    	    set Trend_String[EnumId] = h
		    	    return EnumId
		        elseif (EnumId < 8191) then
	    		    set EnumId = EnumId + 1
		        else 
		    	    set EnumId = 0
                endif
                exitwhen EnumId == StartId
            endloop
            return -1
        endfunction
        
        function ClearStrId takes integer id returns nothing
	        set Trend_String[id] = null
        endfunction

endlibrary

library SchemeExport requires StrHash
    function DecToBin takes integer dec returns string
        local string bin=""
        local string array bits
        local integer i=0
        loop
        exitwhen ( dec < 2 )
            if ( ModuloInteger(dec, 2) == 1 ) then
                set bits[i]="1"
            else
                set bits[i]="0"
            endif
            set dec=dec / 2
            set i=i + 1
        endloop
        set bits[i]=I2S(ModuloInteger(dec, 2))
        loop
        exitwhen ( i < 0 )
            set bin=bin + bits[i]
            set i=i - 1
        endloop
        return bin
    endfunction  

    function DecToOct takes integer dec returns string
        local integer i=0
        local integer j=0
        local integer oct=0
        if ( dec <= 7 ) then
            set oct=dec
        else
            loop
            exitwhen ( dec <= 7 )
                set j=ModuloInteger(dec, 8)
                set dec=dec / 8
                set oct=R2I(j * Pow(10, i) + oct)
                set i=i + 1
            endloop
        endif
        set oct=R2I(oct + dec * Pow(10, i))
        return I2S(oct)
    endfunction  

    function DecToHex takes integer dec returns string
        local string array b
        local string sec=""
        local integer i=0
        local integer j
        local integer array int
        local StringSegments limb=StringSegments.create("#0 #1 #2 #3 #4 #5 #6 #7 #8 #9 #A #B #C #D #E #F" , " ")
        loop
        exitwhen ( not ( limb.hasMoreSegments() ) )
            set b[i]=SubString(limb.nextSegment(), 1, 2)
            set i=i + 1
        endloop
        set i=0
        set j=dec
        loop
        exitwhen ( j <= 0 )
            set int[i]=ModuloInteger(j, 16)
            set j=j / 16
            set i=i + 1
        endloop
        loop
        exitwhen ( i <= 0 )
            set sec=sec + b[int[i - 1]]
            set i=i - 1
        endloop
        call limb.destroy()
        return sec
    endfunction 

    function ASCIIToChar takes integer i returns string
        if ( i == 0 ) then
            return null
        elseif ( i >= 8 and i <= 10 ) then
            return SubString("\b\t\n", i - 8, i - 7)
        elseif ( i >= 12 and i <= 13 ) then
            return SubString("\f\r", i - 12, i - 11)
        elseif ( i >= 32 and i <= 127 ) then
            return SubString("!\"#$%%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", i - 32, i - 31)
        endif
        return ""
    endfunction  

    function InvertChar takes string inv returns string
        local string new=""
        local string array pol
        local integer i
        set i=StringLength(inv) - 1
        loop
        exitwhen i < 0
            set pol[i]=SubString(inv, i, i + 1)
            set new=new + pol[i]
        set i=i - 1
        endloop
        return new
    endfunction

    function SeverChar takes string h,string mark returns integer
        local integer len=StringLength(h)
        local integer int
        local string result
        set int=0
        loop
        exitwhen int > len - 1
            set result=SubString(h, int, int + 1)
            exitwhen ( result == mark )
        set int=int + 1
        endloop
        return int
    endfunction  

    //public:
        function Print takes string char,real col returns nothing
            local string com
            local string pat
            local string rev
            local string ally
            local string key="dbohxcefst"
            local string sty
            local string dig
            local string acc
            local string line=""
            local integer l=SeverChar(char , "%") //    >>> String Parameters Details Lists <<< // // com(common):  // pat(pattern): // rev(revers): // ally: // key: include keywords. // sty(style): // dig(digital): // acc(accuracy): // line: // //=========================================================================================================// //=========================================================================================================
            local integer i
            local integer j
            local integer k
            set com=SubString(char, 0, l)
            set ally=SubString(char, l + 1, StringLength(char))
            set l=SeverChar(ally , "|")
            set pat=SubString(ally, 0, l)
            set rev=SubString(ally, l + 1, StringLength(ally)) //=========================================================================================================//
            if ( rev == "n" ) then
                set rev="\n"
            elseif ( rev == "r" ) then
                set rev="\r"
            elseif ( rev == "b" ) then
                set rev="\b"
            elseif ( rev == "f" ) then
                set rev="\f"
            elseif ( rev == "t" ) then
                set rev="\t"
            elseif ( rev == "q" ) then
                set rev=" \n\n\n\n\n"
            endif //=========================================================================================================//
            set i=0
            loop
            exitwhen i > StringLength(pat) - 1
                set ally=SubString(pat, i, i + 1)
                set j=0
                loop
                exitwhen j > StringLength(key) - 1
                    set sty=SubString(key, j, j + 1)
                    exitwhen ( ally == sty )
                set j=j + 1
                endloop
            set i=i + 1
            endloop
            set sty=ally //=========================================================================================================//
            set l=SeverChar(pat , ".")
            set dig=SubString(pat, 0, l)
            if ( l >= StringLength(pat) ) then
                set acc=""
            else
                set acc=SubString(pat, l + 1, StringLength(pat) - 1)
            endif //=========================================================================================================//
            if ( sty == "d" ) then
                set j=StringLength(I2S(R2I(col)))
            elseif ( sty == "f" ) then
                set j=StringLength(R2S(col))
                set l=SeverChar(R2S(col) , ".")
                if ( S2I(acc) > j - l - 1 ) then
                    set i=0
                    loop
                    exitwhen i > ( S2I(acc) - j + l )
                        set line=line + "0"
                    set i=i + 1
                    endloop
                    call BJDebugMsg(com + R2S(col) + line + rev)
                else
                    set ally=SubString(R2S(col), 0, S2I(acc) + l + 1)
                    set i=S2I(SubString(R2S(col), S2I(acc) + l + 1, S2I(acc) + l + 2))
                    if ( i >= 5 ) then
                        set col=S2R(ally) + Pow(0.1, - ( S2I(acc) - j + l ))
                        set ally=SubString(R2S(col), 0, S2I(acc) + l + 1)
                    endif
                    call BJDebugMsg(com + ally + rev)
                endif
            endif
            if ( S2I(dig) - j > 0 ) then
                set i=0
                loop
                exitwhen i > ( S2I(dig) - j )
                    set com=com + " "
                set i=i + 1
                endloop
            endif //=========================================================================================================//
            if ( sty == "d" ) then
                call BJDebugMsg(com + I2S(R2I(col)) + rev)
            elseif ( sty == "b" ) then
                call BJDebugMsg(com + DecToBin(R2I(col)) + rev)
            elseif ( sty == "o" ) then
                call BJDebugMsg(com + DecToOct(R2I(col)) + rev)
            elseif ( sty == "h" ) then
                call BJDebugMsg(com + DecToHex(R2I(col)) + rev)
            elseif ( sty == "x" ) then
                call BJDebugMsg(com + "0x" + DecToHex(R2I(col)) + rev)
            elseif ( sty == "s" ) then
                call BJDebugMsg(com + ASCIIToChar(R2I(col)) + rev)
            elseif ( sty == "l" ) then
                call BJDebugMsg(com + Trend_String[R2I(col)])
                call ClearStrId(R2I(col))
            elseif ( sty == "c" ) then
                call BJDebugMsg(com + SubString(Trend_String[R2I(col)], 0, 1))
                call ClearStrId(R2I(col))
            elseif ( sty == "t" ) then //=========================================================================================================//
                call BJDebugMsg(com + InvertChar(Trend_String[R2I(col)]))
                call ClearStrId(R2I(col))
            endif
        endfunction

endlibrary