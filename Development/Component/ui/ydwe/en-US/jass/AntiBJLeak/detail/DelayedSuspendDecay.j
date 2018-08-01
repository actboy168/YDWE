#if 0
//--------------------------------------------//
//         本文件为自动生成，请勿编辑         //
//             thanks to 最萌小汐             //
//--------------------------------------------//
#endif
#ifdef USE_BJ_ANTI_LEAK
#ifndef YDWEDelayedSuspendDecayNullIncluded
#define YDWEDelayedSuspendDecayNullIncluded

#include "AntiBJLeak/detail/DelayedSuspendDecayFleshEnum.j"
#include "AntiBJLeak/detail/DelayedSuspendDecayBoneEnum.j"
#include "AntiBJLeak/detail/DelayedSuspendDecayStopAnimEnum.j"

library YDWEDelayedSuspendDecayNull requires YDWEDelayedSuspendDecayFleshEnumNull, YDWEDelayedSuspendDecayBoneEnumNull, YDWEDelayedSuspendDecayStopAnimEnumNull

function YDWEDelayedSuspendDecayNull takes nothing returns nothing
    local group boneGroup
    local group fleshGroup

    // Switch the global unit groups over to local variables and recreate
    // the global versions, so that this function YDWEcan handle overlapping
    // calls.
    set boneGroup = bj_suspendDecayBoneGroup
    set fleshGroup = bj_suspendDecayFleshGroup
    set bj_suspendDecayBoneGroup = CreateGroup()
    set bj_suspendDecayFleshGroup = CreateGroup()

    call ForGroup(fleshGroup, function YDWEDelayedSuspendDecayStopAnimEnumNull)
    call ForGroup(boneGroup, function YDWEDelayedSuspendDecayStopAnimEnumNull)

    call TriggerSleepAction(bj_CORPSE_MAX_DEATH_TIME)
    call ForGroup(fleshGroup, function YDWEDelayedSuspendDecayFleshEnumNull)
    call ForGroup(boneGroup, function YDWEDelayedSuspendDecayBoneEnumNull)

    call TriggerSleepAction(0.05)
    call ForGroup(fleshGroup, function YDWEDelayedSuspendDecayStopAnimEnumNull)

    call DestroyGroup(boneGroup)
    call DestroyGroup(fleshGroup)
    set boneGroup = null
    set fleshGroup = null
endfunction

endlibrary

#endif
#endif
