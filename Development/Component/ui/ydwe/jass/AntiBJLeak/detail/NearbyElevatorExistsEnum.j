#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWENearbyElevatorExistsEnumNullIncluded
#define YDWENearbyElevatorExistsEnumNullIncluded


library YDWENearbyElevatorExistsEnumNull

function YDWENearbyElevatorExistsEnumNull takes nothing returns nothing
    local destructable d     = GetEnumDestructable()
    local integer      dType = GetDestructableTypeId(d)

    if (dType == bj_ELEVATOR_CODE01) or (dType == bj_ELEVATOR_CODE02) then
        set bj_elevatorNeighbor = d
    endif
    set d = null
endfunction

endlibrary

#endif
#endif
