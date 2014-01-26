#ifndef YDWELogarithmIncluded
#define YDWELogarithmIncluded

#include "YDWEBase.j"

library YDWELogarithm initializer onInit

globals
    private real array base
endglobals

// Author : zyl910
// Modified by : c kuhn
// [private]用级数计算以e为底的对数
// Returns 0 if x <= 0
private function ln_taylor takes real x returns real
    // ln( (1+x) / (1-x) ) = ln(1+x) - ln(1-x) = 2 * ( x + x^3 / 3 + x^5 / 5 + ... )
    // x = (y-1) / (y+1)
    // (0 < y < +∞, -1 < x < 1)
    local real fRet = 0.
    local real x2 // x*x
    local real fCur
    local real fDiv = 1.
    
    if x > 0 then
        set x = (x-1.) / (x+1.)
        set x2 = x*x
        loop
            set fCur = fRet + x * 2. / fDiv
            exitwhen fRet >= fCur and fRet <= fCur
            set fRet = fCur
            set x = x * x2
            set fDiv = fDiv + 2.
        endloop
    endif
    
    return fRet
endfunction

// Author : c kuhn
// 通过满二叉树特性快速找到大的部分
private function Log2 takes real x returns real
    local real res = 0.
    local real sign =1.
    local real i = 64.
    local real level = 32.
    local real mid
    local real temp
    local real fac = 0.
    local integer count = 6

    if x>0 and (x<1 or x>1) then
        if x<1 then
            set sign = -1.
            set x = 1. / x
        endif
        if x >= 2. then
            set mid = base[7]
            loop
                set temp = x / mid
                exitwhen temp >= 1. and temp < 2.
                if x < mid then
                    set i = i - level
                    set mid = mid / base[count]
                else
                    set i = i + level
                    set mid = mid * base[count]
                endif
                set level = level / 2.
                set count = count - 1
            endloop
            set fac = i
            set x = temp
        endif
        //根号2
        if x > 1.4142135 then
            set x = x / 1.4142135
            set fac = fac + .5
        endif
        set res=sign*(fac+ln_taylor(x)*1.442695)
    endif
    
    return res
endfunction

// 以 10 为底的对数
// Returns 0 if x <= 0
function YDWELogarithmLg takes real x returns real
    return Log2(x) * 0.3010300 // 1/log2(10) = 0.30102999566398119521373889472449
endfunction

// 以 e 为底的对数
// Returns 0 if x <= 0
function YDWELogarithmLn takes real x returns real
    return Log2(x) * 0.6931472 // 1/log2(e) = 0.69314718055994530941723212145818
endfunction

// 以 任意数 为底的对数 
function YDWELogarithmLog takes real a,real x returns real
    if a<=0 or (a<=1 and a>=1) then
        return 0.
    endif
    return Log2(x) / Log2(a)
endfunction

private function onInit takes nothing returns nothing
    set base[1] = 2.
    set base[2] = 4.
    set base[3] = 16.
    set base[4] = 256.
    set base[5] = 65536.
    //过大的数必须用乘法
    set base[6] = 65536. * 65536. //2^32
    set base[7] = base[6] * base[6] //2^64
endfunction

endlibrary

#endif /// YDWELogarithmIncluded
