#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEBlightGoldMineForPlayerBJNullIncluded
#define YDWEBlightGoldMineForPlayerBJNullIncluded


library YDWEBlightGoldMineForPlayerBJNull

globals
#ifndef YDWE_NNULLTEMPUNIT_DEFVAR
#define YDWE_NNULLTEMPUNIT_DEFVAR
    unit yd_NullTempUnit
#endif
endglobals

function YDWEBlightGoldMineForPlayerBJNull takes unit goldMine, player whichPlayer returns unit
    local real    mineX
    local real    mineY
    local integer mineGold
    local unit    newMine

    // Make sure we're replacing a Gold Mine and not some other type of unit.
    if GetUnitTypeId(goldMine) != 'ngol' then
        return null
    endif

    // Save the Gold Mine's properties and remove it.
    set mineX    = GetUnitX(goldMine)
    set mineY    = GetUnitY(goldMine)
    set mineGold = GetResourceAmount(goldMine)
    call RemoveUnit(goldMine)

    // Create a Haunted Gold Mine to replace the Gold Mine.
    set newMine = CreateBlightedGoldmine(whichPlayer, mineX, mineY, bj_UNIT_FACING)
    call SetResourceAmount(newMine, mineGold)
    set yd_NullTempUnit = newMine
    set newMine = null
    return yd_NullTempUnit
endfunction

endlibrary

#endif
#endif
