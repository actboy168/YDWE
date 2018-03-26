#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEIssueHauntOrderAtLocBJNullIncluded
#define YDWEIssueHauntOrderAtLocBJNullIncluded


library YDWEIssueHauntOrderAtLocBJNull

globals
#ifndef YDWE_NNULLTEMPUNIT_DEFVAR
#define YDWE_NNULLTEMPUNIT_DEFVAR
    unit yd_NullTempUnit
#endif
endglobals

function YDWEIssueHauntOrderAtLocBJNull takes unit whichPeon, location loc returns boolean
    local group g = null
    local unit goldMine = null

    // Search for a gold mine within a 1-cell radius of the specified location.
    set g = CreateGroup()
    call GroupEnumUnitsInRangeOfLoc(g, loc, 2*bj_CELLWIDTH, filterIssueHauntOrderAtLocBJ)
    set goldMine = FirstOfGroup(g)
    call DestroyGroup(g)
    set g = null

    // If no mine was found, abort the request.
    if (goldMine == null) then
        return false
    endif
    set yd_NullTempUnit = goldMine
    set goldMine = null
    // Issue the Haunt Gold Mine order.
    return IssueTargetOrderById(whichPeon, 'ugol', yd_NullTempUnit)
endfunction

endlibrary

#endif
#endif
