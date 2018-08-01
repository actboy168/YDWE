#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWECreatePermanentCorpseLocBJNullIncluded
#define YDWECreatePermanentCorpseLocBJNullIncluded

library YDWECreatePermanentCorpseLocBJNull

function YDWECreatePermanentCorpseLocBJNull takes integer style, integer unitid, player whichPlayer, location loc, real facing returns unit
    set bj_lastCreatedUnit = CreateCorpse(whichPlayer, unitid, GetLocationX(loc), GetLocationY(loc), facing)
    call SetUnitBlendTime(bj_lastCreatedUnit, 0)
    call UnitSuspendDecay(bj_lastCreatedUnit, true)
    if style == bj_CORPSETYPE_FLESH then
        call SetUnitAnimation(bj_lastCreatedUnit, "decay flesh")
    else
        call SetUnitAnimation(bj_lastCreatedUnit, "decay bone")
    endif
    call SetUnitTimeScale(bj_lastCreatedUnit, 0.0001)
    return bj_lastCreatedUnit
endfunction

endlibrary

#endif
#endif
