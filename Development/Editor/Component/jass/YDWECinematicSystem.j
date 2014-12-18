#ifndef YDWECinematicSystemIncluded
#define YDWECinematicSystemIncluded

#include "YDWEBase.j"

library YDWEScriptSystem requires YDWEBase

// ****************************************************************************
// **
// **  CALIBRATION FUNCTIONS
// **
// ****************************************************************************
globals
//ScriptSystem
    private unit array              YDWE_cinS_actor
    private camerasetup array       YDWE_cinS_camera
    private real array              YDWE_cinS_act_moveX
    private real array              YDWE_cinS_act_moveY
    private real array              YDWE_cinS_act_moveZ
    private real array              YDWE_cinS_act_moveVX
    private real array              YDWE_cinS_act_moveVY
    private real array              YDWE_cinS_act_moveVZ
    private real array              YDWE_cinS_act_moveAZ
    private real array              YDWE_cinS_act_moveTime
    private real array              YDWE_cinS_act_turnFi
    private real array              YDWE_cinS_act_turnVFi
    private real array              YDWE_cinS_act_turnTime
    private real array              YDWE_cinS_act_colourR
    private real array              YDWE_cinS_act_colourG
    private real array              YDWE_cinS_act_colourB
    private real array              YDWE_cinS_act_colourA
    private real array              YDWE_cinS_act_colourVR
    private real array              YDWE_cinS_act_colourVG
    private real array              YDWE_cinS_act_colourVB
    private real array              YDWE_cinS_act_colourVA
    private real array              YDWE_cinS_act_colourTime
    private real array              YDWE_cinS_act_sizeS
    private real array              YDWE_cinS_act_sizeVS
    private real array              YDWE_cinS_act_sizeTime
    private real array              YDWE_cinS_cam_startval
    private real array              YDWE_cinS_cam_startvel
    private real array              YDWE_cinS_cam_accprim
    private real array              YDWE_cinS_cam_accsec
    private real array              YDWE_cinS_cam_time
    private real array              YDWE_cinS_cam_endtime
    private group array             YDWE_cinS_act_groups
    private unit                    YDWE_cinS_cam_OvRxyzUnitPrim = null
    private real array              YDWE_cinS_cam_OvRoffset
    private real                    YDWE_cinS_cam_OvRxyzTime    = 0
    private real                    YDWE_cinS_cam_OvRxyzEndtime = 0
    private real                    YDWE_cinS_cam_OvRangTime    = 0
    private real                    YDWE_cinS_cam_OvRangEndtime = 0
    private unit                    YDWE_cinS_cam_OvRxyzUnitSec = null
    private unit                    YDWE_cinS_cam_OvRangUnitPrim = null
    private unit                    YDWE_cinS_cam_OvRangUnitSec = null
    private boolean                 YDWE_cinS_cam_OvRxyzSmooth  = false
    private boolean                 YDWE_cinS_cam_OvRangSmooth  = false
    private unit                    YDWE_cinS_cam_fadeUnit      = null
    private location                YDWE_cinS_tempPoint         = null
    private boolean                 YDWE_cinS_cam_ignoreTerrain = false
    private boolean                 YDWE_cinS_displaySubtitles  = true
    private integer                 YDWE_cinS_FadeFilterUnit  = 'e000'
    private real                    YDWE_cinS_ScriptSystemPeriod = 0.02
    private real                    YDWE_cinS_ParticlePeriod = 0.02
    private real                    YDWE_cinS_CameraPeriod = 0.02
endglobals

function YDWEScriptSystemPeriod takes real timeout returns nothing
    set YDWE_cinS_ScriptSystemPeriod=timeout
endfunction

function YDWEParticlePeriod takes real timeout returns nothing
    set YDWE_cinS_ParticlePeriod=timeout
endfunction

function YDWECameraPeriod takes real timeout returns nothing
    set YDWE_cinS_CameraPeriod=timeout
endfunction

function YDWEFadeFilterUnit takes integer uid returns nothing
    set YDWE_cinS_FadeFilterUnit=uid
endfunction

//===========================================================================
//ϵͳ-ScriptSystem
//===========================================================================

function YDWECinSInitTempPoint takes location point returns nothing
    set YDWE_cinS_tempPoint=point
    //return
endfunction

function YDWECinSActor takes integer i returns unit
    return YDWE_cinS_actor[i]
endfunction

// ****************************************************************************
// **
// **  GENERAL AND MISCELANEOUS FUNCTIONS
// **
// ****************************************************************************

//function GC takes nothing returns gamecache
//    if YDWE_cinS_GC == null then
//        call FlushGameCache(InitGameCache( "CinemaCache" ))
//        set YDWE_cinS_GC = InitGameCache( "CinemaCache" )
//    endif
//    return YDWE_cinS_GC
//endfunction

function YDWECameraSetupAssign takes camerasetup cs, integer actID returns nothing
    set YDWE_cinS_camera[actID] = cs
endfunction

function YDWEActorCreate takes player p, integer actID, integer unitType returns nothing
    local unit u = CreateUnit(p, unitType, 0.0, 0.0, 0.0)
    call SetUnitUserData(u, actID)
    call YDWEFlyEnable(u)
    set YDWE_cinS_actor[actID] = u
    set YDWE_cinS_act_colourR[actID] = 100.0
    set YDWE_cinS_act_colourG[actID] = 100.0
    set YDWE_cinS_act_colourB[actID] = 100.0
    set YDWE_cinS_act_colourA[actID] = 100.0
    set u = null
endfunction

// ****************************************************************************
// **
// **  CAMERA SYSTEM
// **
// ****************************************************************************

function YDWECinematicCameraEnable takes nothing returns nothing
    local trigger t = YDWEGetTriggerByString( "Globals", "CamTrig")
    call EnableTrigger(t)
    set t = null
endfunction
function YDWECinematicCameraDisable takes nothing returns nothing
    local trigger t = YDWEGetTriggerByString( "Globals", "CamTrig")
    call DisableTrigger(t)
    set t = null
endfunction

function YDWESetCameraZ takes real z returns nothing
    //thanks to ToadCop for this function
    set z = GetCameraField(CAMERA_FIELD_ZOFFSET)+z-GetCameraTargetPositionZ()
    call SetCameraField(CAMERA_FIELD_ZOFFSET,z,- 0.01)
    call SetCameraField(CAMERA_FIELD_ZOFFSET,z,0.01)
endfunction

function YDWECameraMainLoop takes nothing returns nothing
    local real array s
    local location stupid = YDWE_cinS_tempPoint
    local integer i = 0
    local real ovrFactor

    // camera motion
    loop
        exitwhen i == 18
        set i = i + 1
        set YDWE_cinS_cam_time[i] = YDWE_cinS_cam_time[i] + YDWE_cinS_CameraPeriod

        set s[i] = YDWE_cinS_cam_startval[i] + ( YDWE_cinS_cam_startvel[i] * YDWE_cinS_cam_time[i] ) + ( YDWE_cinS_cam_accprim[i] * YDWE_cinS_cam_time[i] * YDWE_cinS_cam_time[i] / 2 )

        if i > 3 and i < 7 then
            loop
            if s[i] > 360 then
                set s[i] = s[i] - 360
            elseif s[i] < 0 then
                set s[i] = s[i] + 360
            else
                exitwhen true
            endif
            endloop
        endif

        if YDWE_cinS_cam_time[i] + YDWE_cinS_CameraPeriod >= YDWE_cinS_cam_endtime[i] then
            if YDWE_cinS_cam_accsec[i] != 0 then
                set YDWE_cinS_cam_startvel[i] = YDWE_cinS_cam_startvel[i] + ( YDWE_cinS_cam_accprim[i] * YDWE_cinS_cam_time[i] )
                set YDWE_cinS_cam_accprim[i] = YDWE_cinS_cam_accsec[i]
                set YDWE_cinS_cam_startval[i] = s[i]
                set YDWE_cinS_cam_time[i] = 0.0
                set YDWE_cinS_cam_accsec[i] = 0.0
            else
                if i>9 then
                set YDWE_cinS_cam_startvel[i] = 0.0
                set YDWE_cinS_cam_startval[i] = s[i]
                set YDWE_cinS_cam_time[i] = 0.0
                endif
            endif
        endif
    endloop

    // camera lock-to-unit override
    if YDWE_cinS_cam_OvRxyzUnitPrim!=null or YDWE_cinS_cam_OvRxyzUnitSec!=null then
        set YDWE_cinS_cam_OvRxyzTime = YDWE_cinS_cam_OvRxyzTime + YDWE_cinS_CameraPeriod
        if YDWE_cinS_cam_OvRxyzTime > YDWE_cinS_cam_OvRxyzEndtime then
            set YDWE_cinS_cam_OvRxyzTime = YDWE_cinS_cam_OvRxyzEndtime
        endif

        set ovrFactor = YDWE_cinS_cam_OvRxyzTime/YDWE_cinS_cam_OvRxyzEndtime
        if YDWE_cinS_cam_OvRxyzSmooth then
            set ovrFactor = (1.0 - Cos(ovrFactor*bj_PI)) / 2
        endif

        call MoveLocation(stupid, GetUnitX(YDWE_cinS_cam_OvRxyzUnitPrim),GetUnitY(YDWE_cinS_cam_OvRxyzUnitPrim))
        set s[19] = GetLocationZ(stupid)
        call MoveLocation(stupid, GetUnitX(YDWE_cinS_cam_OvRxyzUnitSec),GetUnitY(YDWE_cinS_cam_OvRxyzUnitSec))
        set s[20] = GetLocationZ(stupid)

        if YDWE_cinS_cam_OvRxyzUnitSec==null then //going from null to unit
            set s[1] = s[1]*(1-ovrFactor) + (GetUnitX(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[1])*ovrFactor
            set s[2] = s[2]*(1-ovrFactor) + (GetUnitY(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[2])*ovrFactor
            set s[3] = s[3]*(1-ovrFactor) + (s[19]+GetUnitFlyHeight(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[3])*ovrFactor
        elseif YDWE_cinS_cam_OvRxyzUnitPrim==null then //going from unit to null
            set s[1] = s[1]*ovrFactor + (GetUnitX(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[7])*(1.0-ovrFactor)
            set s[2] = s[2]*ovrFactor + (GetUnitY(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[8])*(1.0-ovrFactor)
            set s[3] = s[3]*ovrFactor + (s[20]+GetUnitFlyHeight(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[9])*(1.0-ovrFactor)
        else //going from unit to unit
            set s[1] = (GetUnitX(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[7])*(1-ovrFactor) + (GetUnitX(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[1])*ovrFactor
            set s[2] = (GetUnitY(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[8])*(1-ovrFactor) + (GetUnitY(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[2])*ovrFactor
            set s[3] = (s[20]+GetUnitFlyHeight(YDWE_cinS_cam_OvRxyzUnitSec)+YDWE_cinS_cam_OvRoffset[9])*(1-ovrFactor) + (s[19]+GetUnitFlyHeight(YDWE_cinS_cam_OvRxyzUnitPrim)+YDWE_cinS_cam_OvRoffset[3])*ovrFactor
        endif
        if YDWE_cinS_cam_OvRxyzTime == YDWE_cinS_cam_OvRxyzEndtime then
            if YDWE_cinS_cam_OvRxyzUnitPrim==null then
                set YDWE_cinS_cam_OvRxyzUnitSec = null
            endif
        endif
    endif

    // camera face-to-unit override
    if YDWE_cinS_cam_OvRangUnitPrim!=null or YDWE_cinS_cam_OvRangUnitSec!=null then
        set YDWE_cinS_cam_OvRangTime = YDWE_cinS_cam_OvRangTime + YDWE_cinS_CameraPeriod
        if YDWE_cinS_cam_OvRangTime > YDWE_cinS_cam_OvRangEndtime then
            set YDWE_cinS_cam_OvRangTime = YDWE_cinS_cam_OvRangEndtime
        endif

        set ovrFactor = YDWE_cinS_cam_OvRangTime/YDWE_cinS_cam_OvRangEndtime
        if YDWE_cinS_cam_OvRangSmooth then
            set ovrFactor = (1.0 - Cos(ovrFactor*bj_PI)) / 2
        endif

        call MoveLocation(stupid, GetUnitX(YDWE_cinS_cam_OvRangUnitPrim),GetUnitY(YDWE_cinS_cam_OvRangUnitPrim))
        set s[19] = GetLocationZ(stupid)
        call MoveLocation(stupid, GetUnitX(YDWE_cinS_cam_OvRangUnitSec),GetUnitY(YDWE_cinS_cam_OvRangUnitSec))
        set s[20] = GetLocationZ(stupid)

        if YDWE_cinS_cam_OvRangUnitPrim!=null then
            set s[21]=(GetUnitX(YDWE_cinS_cam_OvRangUnitPrim)+YDWE_cinS_cam_OvRoffset[4])-s[1]
            set s[22]=(GetUnitY(YDWE_cinS_cam_OvRangUnitPrim)+YDWE_cinS_cam_OvRoffset[5])-s[2]
            set s[23]=(s[19]+GetUnitFlyHeight(YDWE_cinS_cam_OvRangUnitPrim)+YDWE_cinS_cam_OvRoffset[6])-s[3]
            set s[24]=Atan2(s[22], s[21]) * bj_RADTODEG
            set s[25]=Atan2(s[23], SquareRoot(s[21]*s[21]+s[22]*s[22])) * bj_RADTODEG
        else
            set s[24]=s[4]
            set s[25]=s[5]
        endif

        if YDWE_cinS_cam_OvRangUnitSec!=null then
            set s[21]=(GetUnitX(YDWE_cinS_cam_OvRangUnitSec)+YDWE_cinS_cam_OvRoffset[10])-s[1]
            set s[22]=(GetUnitY(YDWE_cinS_cam_OvRangUnitSec)+YDWE_cinS_cam_OvRoffset[11])-s[2]
            set s[23]=(s[20]+GetUnitFlyHeight(YDWE_cinS_cam_OvRangUnitSec)+YDWE_cinS_cam_OvRoffset[12])-s[3]
            set s[26]=Atan2(s[22], s[21]) * bj_RADTODEG
            set s[27]=Atan2(s[23], SquareRoot(s[21]*s[21]+s[22]*s[22])) * bj_RADTODEG
        else
            set s[26]=s[4]
            set s[27]=s[5]
        endif

        if (s[26] - s[24]) > 180 then
            set s[24] = s[24] + 360
        elseif (s[26] - s[24]) < -180 then
            set s[24] = s[24] - 360
        endif
        set s[4] = ovrFactor * s[24] + (1-ovrFactor) * s[26]

        if (s[27] - s[25]) > 180 then
            set s[25] = s[25] + 360
        elseif (s[27] - s[25]) < -180 then
            set s[25] = s[25] - 360
        endif
        set s[5] = ovrFactor * s[25] + (1-ovrFactor) * s[27]

        if YDWE_cinS_cam_OvRangTime == YDWE_cinS_cam_OvRangEndtime then
            if YDWE_cinS_cam_OvRangUnitPrim==null then
                set YDWE_cinS_cam_OvRangUnitSec = null
            endif
        endif
    endif

    if s[9]<100.0 then
        set s[9]=100.0
    endif

    // camera fade filter unit
    if s[18]>0 then
        set s[23]=Sin(s[5]*bj_DEGTORAD)
        set s[20]=Cos(s[5]*bj_DEGTORAD)
        set s[21]=Cos(s[4]*bj_DEGTORAD)*s[20]
        set s[22]=Sin(s[4]*bj_DEGTORAD)*s[20]

        set s[19]=(s[9]-128.0)
        set s[20]=Tan(s[7]*bj_DEGTORAD/2) *40

        set s[21]=s[1]-s[19]*s[21]
        set s[22]=s[2]-s[19]*s[22]
        set s[23]=s[3]-s[19]*s[23]

        call MoveLocation(stupid, s[21], s[22])
        set s[23]=s[23]-GetLocationZ(stupid)
        call SetUnitX(YDWE_cinS_cam_fadeUnit, s[21])
        call SetUnitY(YDWE_cinS_cam_fadeUnit, s[22])
        call SetUnitFlyHeight(YDWE_cinS_cam_fadeUnit, s[23], 0.0)
        call SetUnitScale(YDWE_cinS_cam_fadeUnit, s[20],s[20],s[20])
    endif

    call PanCameraToTimed( s[1], s[2], 0.0 )
    call MoveLocation(stupid, s[1], s[2])
    if YDWE_cinS_cam_ignoreTerrain then
        call YDWESetCameraZ(s[3])
    else
        call SetCameraField( CAMERA_FIELD_ZOFFSET, s[3]-GetLocationZ(stupid), 0.00 )
    endif
    call SetCameraField( CAMERA_FIELD_ROTATION, s[4], 0.00 )
    call SetCameraField( CAMERA_FIELD_ANGLE_OF_ATTACK, s[5], 0.00 )
    call SetCameraField( CAMERA_FIELD_ROLL, s[6], 0.00 )
    call SetCameraField( CAMERA_FIELD_FIELD_OF_VIEW, s[7], 0.00 )
    call SetCameraField( CAMERA_FIELD_FARZ, s[8], 0.00 )
    call SetCameraField( CAMERA_FIELD_TARGET_DISTANCE, s[9], 0.00 )

    call SetUnitVertexColor(YDWE_cinS_cam_fadeUnit, R2I(s[15]*256), R2I(s[16]*256), R2I(s[17]*256), R2I(s[18]*256))
    call SetTerrainFogEx(0, s[10], s[11], 0.0, s[12], s[13], s[14])

    set stupid = null
endfunction


function YDWECameraStatChangeLin takes integer stat, real endvalue, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )

    set YDWE_cinS_cam_startval[stat] = startvalue
    set YDWE_cinS_cam_startvel[stat] = (endvalue - startvalue)/time
    set YDWE_cinS_cam_accprim[stat] = 0.0
    set YDWE_cinS_cam_accsec[stat] = 0.0
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time
endfunction

function YDWECameraStatChangeLinRad takes integer stat, real endvalue, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )
    local real distance = endvalue - startvalue

    local real velocity1 = distance/time
    local real velocity2 = -( RSignBJ(distance) * (360 - RAbsBJ(distance)) )/time

    set YDWE_cinS_cam_startval[stat] = startvalue
    if RAbsBJ(velocity1) < RAbsBJ(velocity2) then
        set YDWE_cinS_cam_startvel[stat] = velocity1
    else
        set YDWE_cinS_cam_startvel[stat] = velocity2
    endif
    set YDWE_cinS_cam_accprim[stat] = 0.0
    set YDWE_cinS_cam_accsec[stat] = 0.0
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time
endfunction

function YDWECameraStatChange takes integer stat, real endvalue, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )
    local real startvelocity = YDWE_cinS_cam_startvel[stat] + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] )
    local real distance = endvalue - startvalue

    local real accel = 2 * ( distance - startvelocity * time ) / ( time * time )

    set YDWE_cinS_cam_startval[stat] = startvalue
    set YDWE_cinS_cam_startvel[stat] = startvelocity
    set YDWE_cinS_cam_accprim[stat] = accel
    set YDWE_cinS_cam_accsec[stat] = 0.0
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time
endfunction

function YDWECameraStatChangeRad takes integer stat, real endvalue, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )
    local real startvelocity = YDWE_cinS_cam_startvel[stat] + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] )
    local real distance = endvalue - startvalue

    local real accel1 = 2 * ( distance - startvelocity * time ) / ( time * time )
    local real accel2 = 2 * ( -( RSignBJ(distance) * (360 - RAbsBJ(distance)) ) - startvelocity * time ) / ( time * time )

    set YDWE_cinS_cam_startval[stat] = startvalue
    set YDWE_cinS_cam_startvel[stat] = startvelocity
    if RAbsBJ(accel1) < RAbsBJ(accel2) then
        set YDWE_cinS_cam_accprim[stat] = accel1
    else
        set YDWE_cinS_cam_accprim[stat] = accel2
    endif
    set YDWE_cinS_cam_accsec[stat] = 0.0
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time
endfunction

function YDWECameraStatChangeAdv takes integer stat, real endvalue, real endvelocity, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )
    local real startvelocity = YDWE_cinS_cam_startvel[stat] + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] )
    local real distance = endvalue - startvalue

    local real accel1 = 4 * ( distance ) / ( time * time ) - ( endvelocity + 3 * startvelocity ) / time
    local real accel2 = ( 3 * endvelocity + startvelocity ) / time - 4 * ( distance ) / ( time * time )

    set YDWE_cinS_cam_startval[stat] = startvalue
    set YDWE_cinS_cam_startvel[stat] = startvelocity
    set YDWE_cinS_cam_accprim[stat] = accel1
    set YDWE_cinS_cam_accsec[stat] = accel2
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time/2
endfunction

function YDWECameraStatChangeAdvRad takes integer stat, real endvalue, real endvelocity, real time returns nothing
    local real startvalue = YDWE_cinS_cam_startval[stat] + ( YDWE_cinS_cam_startvel[stat] * YDWE_cinS_cam_time[stat] ) + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] * YDWE_cinS_cam_time[stat] / 2 )
    local real startvelocity = YDWE_cinS_cam_startvel[stat] + ( YDWE_cinS_cam_accprim[stat] * YDWE_cinS_cam_time[stat] )
    local real distance = endvalue - startvalue

    local real accel1 = 4 * ( distance ) / ( time * time ) - ( endvelocity + 3 * startvelocity ) / time
    local real accel2 = ( 3 * endvelocity + startvelocity ) / time - 4 * ( distance ) / ( time * time )
    local real accel3 = 4 * ( -( RSignBJ(distance) * (360 - RAbsBJ(distance)) ) ) / ( time * time ) - ( endvelocity + 3 * startvelocity ) / time
    local real accel4 = ( 3 * endvelocity + startvelocity ) / time - 4 * ( -( RSignBJ(distance) * (360 - RAbsBJ(distance)) ) ) / ( time * time )

    set YDWE_cinS_cam_startval[stat] = startvalue
    set YDWE_cinS_cam_startvel[stat] = startvelocity
    if (RAbsBJ(accel3) + RAbsBJ(accel4)) > (RAbsBJ(accel1) + RAbsBJ(accel2)) then
        set YDWE_cinS_cam_accprim[stat] = accel1
        set YDWE_cinS_cam_accsec[stat] = accel2
    else
        set YDWE_cinS_cam_accprim[stat] = accel3
        set YDWE_cinS_cam_accsec[stat] = accel4
    endif
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = time/2

endfunction


function YDWECameraLockToUnitOverride takes unit u, real xoff, real yoff, real zoff, boolean smooth, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif

    set YDWE_cinS_cam_OvRxyzUnitSec = YDWE_cinS_cam_OvRxyzUnitPrim
    set YDWE_cinS_cam_OvRxyzUnitPrim = u
    set YDWE_cinS_cam_OvRxyzSmooth = smooth

    set YDWE_cinS_cam_OvRoffset[7]=YDWE_cinS_cam_OvRoffset[1]
    set YDWE_cinS_cam_OvRoffset[8]=YDWE_cinS_cam_OvRoffset[2]
    set YDWE_cinS_cam_OvRoffset[9]=YDWE_cinS_cam_OvRoffset[3]

    set YDWE_cinS_cam_OvRoffset[1]=xoff
    set YDWE_cinS_cam_OvRoffset[2]=yoff
    set YDWE_cinS_cam_OvRoffset[3]=zoff

    set YDWE_cinS_cam_OvRxyzTime = 0.0
    set YDWE_cinS_cam_OvRxyzEndtime = duration
endfunction
function YDWECameraLockToUnitReset takes boolean smooth, real duration returns nothing
    call YDWECameraLockToUnitOverride(null, 0.0,0.0,0.0, smooth, duration)
endfunction

function YDWECameraFaceToUnitOverride takes unit u, real xoff, real yoff, real zoff, boolean smooth, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif

    set YDWE_cinS_cam_OvRangUnitSec = YDWE_cinS_cam_OvRangUnitPrim
    set YDWE_cinS_cam_OvRangUnitPrim = u
    set YDWE_cinS_cam_OvRangSmooth = smooth

    set YDWE_cinS_cam_OvRoffset[10]=YDWE_cinS_cam_OvRoffset[4]
    set YDWE_cinS_cam_OvRoffset[11]=YDWE_cinS_cam_OvRoffset[5]
    set YDWE_cinS_cam_OvRoffset[12]=YDWE_cinS_cam_OvRoffset[6]

    set YDWE_cinS_cam_OvRoffset[4]=xoff
    set YDWE_cinS_cam_OvRoffset[5]=yoff
    set YDWE_cinS_cam_OvRoffset[6]=zoff

    set YDWE_cinS_cam_OvRangTime = 0.0
    set YDWE_cinS_cam_OvRangEndtime = duration
endfunction
function YDWECameraFaceToUnitReset takes boolean smooth, real duration returns nothing
    call YDWECameraFaceToUnitOverride(null, 0.0,0.0,0.0, smooth, duration)
endfunction


function YDWEApplyCameraInstantly takes camerasetup whichSetup returns nothing
    local integer i = 0

    set YDWE_cinS_cam_startval[1] = CameraSetupGetDestPositionX( whichSetup )
    set YDWE_cinS_cam_startval[2] = CameraSetupGetDestPositionY( whichSetup )
    call MoveLocation(YDWE_cinS_tempPoint, CameraSetupGetDestPositionX( whichSetup ), CameraSetupGetDestPositionY( whichSetup ))
    set YDWE_cinS_cam_startval[3] = CameraSetupGetField( whichSetup, CAMERA_FIELD_ZOFFSET) + GetLocationZ(YDWE_cinS_tempPoint)
    set YDWE_cinS_cam_startval[4] = CameraSetupGetField( whichSetup, CAMERA_FIELD_ROTATION )
    set YDWE_cinS_cam_startval[5] = CameraSetupGetField( whichSetup, CAMERA_FIELD_ANGLE_OF_ATTACK )
    set YDWE_cinS_cam_startval[6] = CameraSetupGetField( whichSetup, CAMERA_FIELD_ROLL )
    set YDWE_cinS_cam_startval[7] = CameraSetupGetField( whichSetup, CAMERA_FIELD_FIELD_OF_VIEW )
    set YDWE_cinS_cam_startval[8] = CameraSetupGetField( whichSetup, CAMERA_FIELD_FARZ )
    set YDWE_cinS_cam_startval[9] = CameraSetupGetField( whichSetup, CAMERA_FIELD_TARGET_DISTANCE )

    loop
        set i = i + 1
        exitwhen i > 9
        set YDWE_cinS_cam_startvel[i] = 0.0
        set YDWE_cinS_cam_accprim[i] = 0.0
        set YDWE_cinS_cam_accsec[i] = 0.0
        set YDWE_cinS_cam_time[i] = 0.0
        set YDWE_cinS_cam_endtime[i] = 0.0
    endloop
endfunction

function YDWEApplyCameraLinear takes camerasetup whichSetup, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif
    call YDWECameraStatChangeLin(1, CameraSetupGetDestPositionX( whichSetup ), duration)
    call YDWECameraStatChangeLin(2, CameraSetupGetDestPositionY( whichSetup ), duration)
    call MoveLocation(YDWE_cinS_tempPoint, CameraSetupGetDestPositionX( whichSetup ), CameraSetupGetDestPositionY( whichSetup ))
    call YDWECameraStatChangeLin(3, CameraSetupGetField( whichSetup, CAMERA_FIELD_ZOFFSET ) + GetLocationZ(YDWE_cinS_tempPoint), duration)
    call YDWECameraStatChangeLinRad(4, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROTATION ), duration)
    call YDWECameraStatChangeLinRad(5, CameraSetupGetField( whichSetup, CAMERA_FIELD_ANGLE_OF_ATTACK ), duration)
    call YDWECameraStatChangeLinRad(6, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROLL ), duration)
    call YDWECameraStatChangeLin(7, CameraSetupGetField( whichSetup, CAMERA_FIELD_FIELD_OF_VIEW ), duration)
    call YDWECameraStatChangeLin(8, CameraSetupGetField( whichSetup, CAMERA_FIELD_FARZ ), duration)
    call YDWECameraStatChangeLin(9, CameraSetupGetField( whichSetup, CAMERA_FIELD_TARGET_DISTANCE ), duration)
endfunction

function YDWEApplyCameraSmooth takes camerasetup whichSetup, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif
    call YDWECameraStatChange(1, CameraSetupGetDestPositionX( whichSetup ), duration)
    call YDWECameraStatChange(2, CameraSetupGetDestPositionY( whichSetup ), duration)
    call MoveLocation(YDWE_cinS_tempPoint, CameraSetupGetDestPositionX( whichSetup ), CameraSetupGetDestPositionY( whichSetup ))
    call YDWECameraStatChange(3, CameraSetupGetField( whichSetup, CAMERA_FIELD_ZOFFSET ) + GetLocationZ(YDWE_cinS_tempPoint), duration)
    call YDWECameraStatChangeRad(4, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROTATION ), duration)
    call YDWECameraStatChangeRad(5, CameraSetupGetField( whichSetup, CAMERA_FIELD_ANGLE_OF_ATTACK ), duration)
    call YDWECameraStatChangeRad(6, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROLL ), duration)
    call YDWECameraStatChange(7, CameraSetupGetField( whichSetup, CAMERA_FIELD_FIELD_OF_VIEW ), duration)
    call YDWECameraStatChange(8, CameraSetupGetField( whichSetup, CAMERA_FIELD_FARZ ), duration)
    call YDWECameraStatChange(9, CameraSetupGetField( whichSetup, CAMERA_FIELD_TARGET_DISTANCE ), duration)
endfunction

function YDWEApplyCameraSmoothStop takes camerasetup whichSetup, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif
    call YDWECameraStatChangeAdv(1, CameraSetupGetDestPositionX( whichSetup ), 0.00, duration)
    call YDWECameraStatChangeAdv(2, CameraSetupGetDestPositionY( whichSetup ), 0.00, duration)
    call MoveLocation(YDWE_cinS_tempPoint, CameraSetupGetDestPositionX( whichSetup ), CameraSetupGetDestPositionY( whichSetup ))
    call YDWECameraStatChangeAdv(3, CameraSetupGetField( whichSetup, CAMERA_FIELD_ZOFFSET ) + GetLocationZ(YDWE_cinS_tempPoint), 0.00, duration)
    call YDWECameraStatChangeAdvRad(4, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROTATION ), 0.00, duration)
    call YDWECameraStatChangeAdvRad(5, CameraSetupGetField( whichSetup, CAMERA_FIELD_ANGLE_OF_ATTACK ), 0.00, duration)
    call YDWECameraStatChangeAdvRad(6, CameraSetupGetField( whichSetup, CAMERA_FIELD_ROLL ), 0.00, duration)
    call YDWECameraStatChangeAdv(7, CameraSetupGetField( whichSetup, CAMERA_FIELD_FIELD_OF_VIEW ), 0.00, duration)
    call YDWECameraStatChangeAdv(8, CameraSetupGetField( whichSetup, CAMERA_FIELD_FARZ ), 0.00, duration)
    call YDWECameraStatChangeAdv(9, CameraSetupGetField( whichSetup, CAMERA_FIELD_TARGET_DISTANCE ), 0.00, duration)
endfunction

function YDWEApplyFog takes real startZ, real endZ, real red, real green, real blue, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif
    call YDWECameraStatChangeLin(10,startZ,duration)
    call YDWECameraStatChangeLin(11,endZ,  duration)
    call YDWECameraStatChangeLin(12,red*0.01,   duration)
    call YDWECameraStatChangeLin(13,green*0.01, duration)
    call YDWECameraStatChangeLin(14,blue*0.01,  duration)
endfunction

function YDWEApplyFade takes real red, real green, real blue, real opacity, real duration returns nothing
    if duration < YDWE_cinS_CameraPeriod then
        set duration = YDWE_cinS_CameraPeriod
    endif
    call YDWECameraStatChangeLin(15,red*0.01,   duration)
    call YDWECameraStatChangeLin(16,green*0.01, duration)
    call YDWECameraStatChangeLin(17,blue*0.01,  duration)
    call YDWECameraStatChangeLin(18,opacity*0.01,  duration)
endfunction


function YDWECameraSystemInit takes nothing returns nothing
    local trigger t = CreateTrigger()
    local camerasetup cs = CreateCameraSetup()
    call TriggerRegisterTimerEvent(t, YDWE_cinS_CameraPeriod, true)
    call TriggerAddAction( t, function YDWECameraMainLoop )
    call YDWESaveTriggerByString( "Globals", "CamTrig", t )
    set t = null
    call CameraSetupSetField(cs, CAMERA_FIELD_TARGET_DISTANCE, GetCameraField(CAMERA_FIELD_TARGET_DISTANCE), 0)
    call CameraSetupSetField(cs, CAMERA_FIELD_FARZ,            GetCameraField(CAMERA_FIELD_FARZ),            0)
    call CameraSetupSetField(cs, CAMERA_FIELD_ZOFFSET,         GetCameraField(CAMERA_FIELD_ZOFFSET),         0)
    call CameraSetupSetField(cs, CAMERA_FIELD_ANGLE_OF_ATTACK, bj_RADTODEG * GetCameraField(CAMERA_FIELD_ANGLE_OF_ATTACK), 0)
    call CameraSetupSetField(cs, CAMERA_FIELD_FIELD_OF_VIEW,   bj_RADTODEG * GetCameraField(CAMERA_FIELD_FIELD_OF_VIEW),   0)
    call CameraSetupSetField(cs, CAMERA_FIELD_ROLL,            bj_RADTODEG * GetCameraField(CAMERA_FIELD_ROLL),            0)
    call CameraSetupSetField(cs, CAMERA_FIELD_ROTATION,        bj_RADTODEG * GetCameraField(CAMERA_FIELD_ROTATION),        0)
    call CameraSetupSetDestPosition(cs, GetCameraTargetPositionX(), GetCameraTargetPositionY(), 0)
    call YDWEApplyCameraInstantly( cs )
    set cs = null
    call YDWEApplyFog( 10000.0, 10000.0, 0.0,0.0,0.0, 0.0 )
    set YDWE_cinS_cam_fadeUnit = CreateUnit(Player(12), YDWE_cinS_FadeFilterUnit, 0,0,0)
    call YDWEFlyEnable(YDWE_cinS_cam_fadeUnit)
    set YDWE_cinS_cam_ignoreTerrain = false
endfunction

// ****************************************************************************
// **
// **  PARTICLE SYSTEM
// **
// ****************************************************************************

function YDWEParticleMove takes nothing returns nothing
    local unit u = GetEnumUnit()
    local integer i = GetUnitUserData(u)
    local real h
    local location stupid = YDWE_cinS_tempPoint

    set YDWE_cinS_act_moveTime[i] = YDWE_cinS_act_moveTime[i] - YDWE_cinS_ParticlePeriod

    set YDWE_cinS_act_moveX[i]=YDWE_cinS_act_moveX[i]+YDWE_cinS_act_moveVX[i]
    set YDWE_cinS_act_moveY[i]=YDWE_cinS_act_moveY[i]+YDWE_cinS_act_moveVY[i]

    if YDWE_cinS_act_moveAZ[i] != 0 then
        set YDWE_cinS_act_moveVZ[i] = YDWE_cinS_act_moveVZ[i] + YDWE_cinS_act_moveAZ[i]
        set YDWE_cinS_act_moveZ[i] = YDWE_cinS_act_moveZ[i] + YDWE_cinS_act_moveVZ[i]
        call MoveLocation(stupid, YDWE_cinS_act_moveX[i],YDWE_cinS_act_moveY[i])
        set h=YDWE_cinS_act_moveZ[i]-GetLocationZ(stupid)
        call SetUnitFlyHeight( u, h, 0 )
        if h <= 0.0 then
            call GroupRemoveUnit(YDWE_cinS_act_groups[0], u)
            set YDWE_cinS_act_moveZ[i] = 0.0
        endif
    elseif YDWE_cinS_act_moveTime[i]<=0.0 then
        call GroupRemoveUnit(YDWE_cinS_act_groups[0], u)
    endif

    call SetUnitX(u,YDWE_cinS_act_moveX[i])
    call SetUnitY(u,YDWE_cinS_act_moveY[i])

    set stupid=null
    set u = null
endfunction

function YDWEParticleTurn takes nothing returns nothing
    local unit u = GetEnumUnit()
    local integer i = GetUnitUserData(u)

    set YDWE_cinS_act_turnTime[i] = YDWE_cinS_act_turnTime[i] - YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_turnFi[i] = YDWE_cinS_act_turnFi[i] + YDWE_cinS_act_turnVFi[i]
    call SetUnitFacing( u, YDWE_cinS_act_turnFi[i] )

    if YDWE_cinS_act_turnTime[i]<=0.0 then
        call GroupRemoveUnit(YDWE_cinS_act_groups[1], u)
    endif
    set u = null
endfunction

function YDWEParticleColour takes nothing returns nothing
    local unit u = GetEnumUnit()
    local integer i = GetUnitUserData(u)

    set YDWE_cinS_act_colourTime[i] = YDWE_cinS_act_colourTime[i] - YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_colourR[i] = YDWE_cinS_act_colourR[i] + YDWE_cinS_act_colourVR[i]
    set YDWE_cinS_act_colourG[i] = YDWE_cinS_act_colourG[i] + YDWE_cinS_act_colourVG[i]
    set YDWE_cinS_act_colourB[i] = YDWE_cinS_act_colourB[i] + YDWE_cinS_act_colourVB[i]
    set YDWE_cinS_act_colourA[i] = YDWE_cinS_act_colourA[i] + YDWE_cinS_act_colourVA[i]
    call SetUnitVertexColor(u, PercentTo255(YDWE_cinS_act_colourR[i]), PercentTo255(YDWE_cinS_act_colourG[i]), PercentTo255(YDWE_cinS_act_colourB[i]), PercentTo255(YDWE_cinS_act_colourA[i]))

    if YDWE_cinS_act_colourTime[i]<=0.0 then
        call GroupRemoveUnit(YDWE_cinS_act_groups[2], u)
    endif
    set u = null
endfunction

function YDWEParticleSize takes nothing returns nothing
    local unit u = GetEnumUnit()
    local integer i = GetUnitUserData(u)

    set YDWE_cinS_act_sizeTime[i] = YDWE_cinS_act_sizeTime[i] - YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_sizeS[i] = YDWE_cinS_act_sizeS[i] + YDWE_cinS_act_sizeVS[i]
    call SetUnitScale(u,YDWE_cinS_act_sizeS[i],YDWE_cinS_act_sizeS[i],YDWE_cinS_act_sizeS[i])

    if YDWE_cinS_act_sizeTime[i]<=0.0 then
        call GroupRemoveUnit(YDWE_cinS_act_groups[3], u)
    endif
    set u = null
endfunction

function YDWEParticleMain takes nothing returns nothing
    call GroupAddUnit(YDWE_cinS_act_groups[0], null)
    call ForGroup(YDWE_cinS_act_groups[0], function YDWEParticleMove )
    call GroupAddUnit(YDWE_cinS_act_groups[1], null)
    call ForGroup(YDWE_cinS_act_groups[1], function YDWEParticleTurn )
    call GroupAddUnit(YDWE_cinS_act_groups[2], null)
    call ForGroup(YDWE_cinS_act_groups[2], function YDWEParticleColour )
    call GroupAddUnit(YDWE_cinS_act_groups[3], null)
    call ForGroup(YDWE_cinS_act_groups[3], function YDWEParticleSize )
endfunction


function YDWEUnitMove takes unit u, real destX, real destY, real duration, real g returns nothing
    local real x = GetUnitX( u )
    local real y = GetUnitY( u )
    local integer i = GetUnitUserData( u )
    local location stupid = YDWE_cinS_tempPoint
    local real z

    if duration < YDWE_cinS_ParticlePeriod then
        set duration = YDWE_cinS_ParticlePeriod
    endif

    if g != 0 then
        if YDWE_cinS_act_moveZ[i] == 0.0 then
            call MoveLocation(stupid, x,y)
            set z = GetLocationZ(stupid)
            set YDWE_cinS_act_moveZ[i] = z
        else
            set z = YDWE_cinS_act_moveZ[i]
        endif

        call MoveLocation(stupid, destX, destY)
        set z = GetLocationZ(stupid)-z

        set YDWE_cinS_act_moveVZ[i] = ((g*duration/2)+(z/duration))*YDWE_cinS_ParticlePeriod
        set YDWE_cinS_act_moveAZ[i] = (-g*YDWE_cinS_ParticlePeriod*YDWE_cinS_ParticlePeriod)
    else
        set YDWE_cinS_act_moveAZ[i] = 0.0
    endif

    set YDWE_cinS_act_moveX[i] = x
    set YDWE_cinS_act_moveY[i] = y
    set YDWE_cinS_act_moveVX[i] = (destX - x)/duration*YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_moveVY[i] = (destY - y)/duration*YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_moveTime[i] = duration

    call GroupAddUnit( YDWE_cinS_act_groups[0], u )
    set stupid = null
endfunction

function YDWEUnitTurn takes unit u, real destAng, real duration, integer clockwise returns nothing
    // input angle between 0.00 and 360.00 please

    local real angle = GetUnitFacing(u)
    local integer i = GetUnitUserData(u)
    local real turnspeed

    if duration < YDWE_cinS_ParticlePeriod then
        set duration = YDWE_cinS_ParticlePeriod
    endif

    if clockwise > 0 and angle > destAng then
        set destAng = destAng + 360.0
    elseif clockwise < 0 and angle < destAng then
        set destAng = destAng - 360.0
    elseif clockwise == 0 then
        if (angle - destAng) > 180 then
            set destAng = destAng + 360
        elseif (angle - destAng) < -180 then
            set destAng = destAng - 360
        endif
    endif

    set turnspeed = (destAng - angle)/duration*YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_turnFi[i] = angle
    set YDWE_cinS_act_turnVFi[i] = turnspeed
    set YDWE_cinS_act_turnTime[i] = duration

    call GroupAddUnit( YDWE_cinS_act_groups[1], u )
endfunction

function YDWEUnitColour takes unit u, real red, real green, real blue, real opacity, real duration returns nothing
    local integer i = GetUnitUserData(u)

    if duration < YDWE_cinS_ParticlePeriod then
        set duration = YDWE_cinS_ParticlePeriod
    endif

    set red = red - YDWE_cinS_act_colourR[i]
    set YDWE_cinS_act_colourVR[i] = red / duration * YDWE_cinS_ParticlePeriod
    set green = green - YDWE_cinS_act_colourG[i]
    set YDWE_cinS_act_colourVG[i] = green / duration * YDWE_cinS_ParticlePeriod
    set blue = blue - YDWE_cinS_act_colourB[i]
    set YDWE_cinS_act_colourVB[i] = blue / duration * YDWE_cinS_ParticlePeriod
    set opacity = opacity - YDWE_cinS_act_colourA[i]
    set YDWE_cinS_act_colourVA[i] = opacity / duration * YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_colourTime[i] = duration

    call GroupAddUnit( YDWE_cinS_act_groups[2], u )
endfunction

function YDWEUnitSize takes unit u, real size, real duration returns nothing
    local integer i = GetUnitUserData(u)

    if duration < YDWE_cinS_ParticlePeriod then
        set duration = YDWE_cinS_ParticlePeriod
    endif

    set size = size - YDWE_cinS_act_sizeS[i]
    set YDWE_cinS_act_sizeVS[i] = size / duration * YDWE_cinS_ParticlePeriod
    set YDWE_cinS_act_sizeTime[i] = duration

    call GroupAddUnit( YDWE_cinS_act_groups[3], u )
endfunction


function YDWEParticleInit takes nothing returns nothing
    local trigger t = CreateTrigger()
    set YDWE_cinS_act_groups[0]=CreateGroup()
    set YDWE_cinS_act_groups[1]=CreateGroup()
    set YDWE_cinS_act_groups[2]=CreateGroup()
    set YDWE_cinS_act_groups[3]=CreateGroup()
    call TriggerRegisterTimerEvent(t, YDWE_cinS_ParticlePeriod, true)
    call TriggerAddAction( t, function YDWEParticleMain )
    set t = null
endfunction


// ****************************************************************************
// **
// **  SUBTITLE ENGINE
// **
// ****************************************************************************

constant function DefaultCharWidth takes nothing returns real
    return 1.56
endfunction

function YDWEDisplaySubtitle takes string line1 returns nothing
    local string char
    local integer i
    local real charwidth
    local real totalwidth

    set i = 0
    set totalwidth = 0

    loop
        set char = SubString(line1, i, i+1)
        exitwhen char == ""
        if StringCase(char, false)==char then
            set charwidth = YDWEGetRealByString( "subtitles", "low"+char)
        else
            set charwidth = YDWEGetRealByString( "subtitles", "upp"+char)
        endif
        if charwidth == 0.0 then
            set charwidth = DefaultCharWidth()
        endif
        set totalwidth=totalwidth+charwidth
        set i = i + 1
    endloop

    set i = R2I((50.0-totalwidth/2)/YDWEGetRealByString( "subtitles", "low "))
    set char = ""
    loop
        exitwhen i <= 3
        set char=char+" "
        set i = i-1
    endloop
    set line1=char+line1

//    call DisplayTimedTextToPlayer(GetLocalPlayer(), 0.25,0.0, 1000.0, line1)
    call DisplayTimedTextToPlayer( GetLocalPlayer(), 0.25, 0.0, 1000.0, "\n\n\n\n\n\n" + line1 + "\n\n\n\n\n\n\n\n\n\n\n\n")

    //set g=null
endfunction

function YDWEClearSubtitles takes nothing returns nothing
    call ClearTextMessages()
endfunction


function YDWESubtitlesInit takes nothing returns nothing

    call YDWESaveRealByString("subtitles", "uppa", 2.06)
    call YDWESaveRealByString("subtitles", "uppb", 1.62)
    call YDWESaveRealByString("subtitles", "uppc", 1.83)
    call YDWESaveRealByString("subtitles", "uppd", 1.93)
    call YDWESaveRealByString("subtitles", "uppe", 1.53)
    call YDWESaveRealByString("subtitles", "uppf", 1.22)
    call YDWESaveRealByString("subtitles", "uppg", 2.04)
    call YDWESaveRealByString("subtitles", "upph", 2.00)
    call YDWESaveRealByString("subtitles", "uppi", 0.77)
    call YDWESaveRealByString("subtitles", "uppj", 0.80)
    call YDWESaveRealByString("subtitles", "uppk", 1.83)
    call YDWESaveRealByString("subtitles", "uppl", 1.48)
    call YDWESaveRealByString("subtitles", "uppm", 2.64)
    call YDWESaveRealByString("subtitles", "uppn", 2.00)
    call YDWESaveRealByString("subtitles", "uppo", 2.25)
    call YDWESaveRealByString("subtitles", "uppp", 1.56)
    call YDWESaveRealByString("subtitles", "uppq", 2.37)
    call YDWESaveRealByString("subtitles", "uppr", 1.79)
    call YDWESaveRealByString("subtitles", "upps", 1.56)
    call YDWESaveRealByString("subtitles", "uppt", 1.51)
    call YDWESaveRealByString("subtitles", "uppu", 1.87)
    call YDWESaveRealByString("subtitles", "uppv", 1.83)
    call YDWESaveRealByString("subtitles", "uppw", 2.82)
    call YDWESaveRealByString("subtitles", "uppx", 2.04)
    call YDWESaveRealByString("subtitles", "uppy", 1.79)
    call YDWESaveRealByString("subtitles", "uppz", 1.79)

    call YDWESaveRealByString("subtitles", "lowa", 1.51)
    call YDWESaveRealByString("subtitles", "lowb", 1.59)
    call YDWESaveRealByString("subtitles", "lowc", 1.47)
    call YDWESaveRealByString("subtitles", "lowd", 1.62)
    call YDWESaveRealByString("subtitles", "lowe", 1.59)
    call YDWESaveRealByString("subtitles", "lowf", 0.98)
    call YDWESaveRealByString("subtitles", "lowg", 1.76)
    call YDWESaveRealByString("subtitles", "lowh", 1.59)
    call YDWESaveRealByString("subtitles", "lowi", 0.65)
    call YDWESaveRealByString("subtitles", "lowj", 0.77)
    call YDWESaveRealByString("subtitles", "lowk", 1.47)
    call YDWESaveRealByString("subtitles", "lowl", 0.68)
    call YDWESaveRealByString("subtitles", "lowm", 2.27)
    call YDWESaveRealByString("subtitles", "lown", 1.56)
    call YDWESaveRealByString("subtitles", "lowo", 1.66)
    call YDWESaveRealByString("subtitles", "lowp", 1.62)
    call YDWESaveRealByString("subtitles", "lowq", 1.62)
    call YDWESaveRealByString("subtitles", "lowr", 0.98)
    call YDWESaveRealByString("subtitles", "lows", 1.26)
    call YDWESaveRealByString("subtitles", "lowt", 1.00)
    call YDWESaveRealByString("subtitles", "lowu", 1.59)
    call YDWESaveRealByString("subtitles", "lowv", 1.59)
    call YDWESaveRealByString("subtitles", "loww", 2.29)
    call YDWESaveRealByString("subtitles", "lowx", 1.59)
    call YDWESaveRealByString("subtitles", "lowy", 1.59)
    call YDWESaveRealByString("subtitles", "lowz", 1.47)

    call YDWESaveRealByString("subtitles", "low.", 0.51)
    call YDWESaveRealByString("subtitles", "low,", 0.55)
    call YDWESaveRealByString("subtitles", "low ", 0.80)
    call YDWESaveRealByString("subtitles", "low-", 1.04)

    //set g=null
endfunction


// ****************************************************************************
// **
// **  SOUND ENGINE
// **
// ****************************************************************************

function YDWEAllocateSound takes string path, integer id, integer volume, real pitch returns nothing

endfunction


// ****************************************************************************
// **
// **  SCRIPT SYSTEM
// **
// ****************************************************************************

function YDWEPlayScene takes integer scene returns nothing
    call YDWESaveIntegerByString( "Interpreter", "Scene", scene )
    call YDWESaveRealByString( "Interpreter", "SceneTime", 0.00 )
    call YDWESaveIntegerByString( "Interpreter", "PendingAction", YDWEGetIntegerByString( "Scene"+I2S(scene), "FirstSpot" ) )
endfunction

function YDWEPlaySceneFromTime takes integer scene, real time returns nothing
    local string s
    local integer previous = YDWEGetIntegerByString( "Scene"+I2S(scene), "LastSpot" )

    if time < 0.00 then
        debug call BJDebugMsg( "User error: negative action time" )
        return
    endif

    loop
        set s = "S"+I2S(scene)+"A"+I2S(previous)
        exitwhen time > YDWEGetRealByString( s, "Time" )
        set previous = YDWEGetIntegerByString( s, "PrevAction" )
    endloop

    call YDWESaveIntegerByString( "Interpreter", "Scene", scene )
    call YDWESaveRealByString( "Interpreter", "SceneTime", time )
    call YDWESaveIntegerByString( "Interpreter", "PendingAction", YDWEGetIntegerByString( s, "NextAction" ) )
endfunction

function YDWEScriptInterpreter takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local real time = YDWEGetRealByString( "Interpreter", "SceneTime" ) + YDWE_cinS_ScriptSystemPeriod
    local string s

    if scene == 0 then
        return
    endif

    loop
        set s = "S"+I2S(scene)+"A"+I2S(action)
        exitwhen time < YDWEGetRealByString( s, "Time" )

        if action == 0 then
            return
        endif

        call ExecuteFunc( YDWEGetStringByString( s, "YDWEActionType" ) )
        if scene != YDWEGetIntegerByString( "Interpreter", "Scene" ) then
            return
        endif

        set action = YDWEGetIntegerByString( s, "NextAction" )
        call YDWESaveIntegerByString( "Interpreter", "PendingAction", action )
    endloop

    call YDWESaveRealByString( "Interpreter", "SceneTime", time )

endfunction


function YDWEWriteToScene takes integer scene returns nothing
    call YDWESaveIntegerByString( "Scripter", "Scene", scene )
endfunction

function YDWEGetActionSpot takes integer scene, real time returns integer
    //local gamecache g = GC()
    local string s
    local integer previous = YDWEGetIntegerByString( "Scene"+I2S(scene), "LastSpot" )
    local integer length = YDWEGetIntegerByString( "Scene"+I2S(scene), "Length" ) + 1
    local integer next = 0

    if time < 0.00 then
        debug call BJDebugMsg( "User error: negative action time" )
        return -1
    endif
    call YDWESaveIntegerByString( "Scene"+I2S(scene), "Length", length )

    loop
        if previous == 0 then
            call YDWESaveIntegerByString( "Scene"+I2S(scene), "FirstSpot", length )
        endif
        set s = "S"+I2S(scene)+"A"+I2S(previous)
        exitwhen time >= YDWEGetRealByString( s, "Time" )
        set previous = YDWEGetIntegerByString( s, "PrevAction" )
    endloop

    if previous == YDWEGetIntegerByString( "Scene"+I2S(scene), "LastSpot" ) then
        call YDWESaveIntegerByString( "Scene"+I2S(scene), "LastSpot", length )
        call YDWESaveIntegerByString( s, "NextAction", length )
    else
        set next = YDWEGetIntegerByString( s, "NextAction" )
        call YDWESaveIntegerByString( s, "NextAction", length )
        set s = "S"+I2S(scene)+"A"+I2S(next)
        call YDWESaveIntegerByString( s, "PrevAction", length )
    endif

    set s = "S"+I2S(scene)+"A"+I2S(length)
    call YDWESaveIntegerByString( s, "PrevAction", previous )
    call YDWESaveIntegerByString( s, "NextAction", next )

    return length
endfunction


// next scene
function YDWENextScene takes nothing returns nothing
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    call YDWEPlayScene(scene+1)
endfunction

function YDWEScriptNextScene takes real time returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWENextScene" )
    call YDWESaveRealByString( s, "Time", time )
    //set g=null
endfunction

// external function
function YDWEScriptExternalFunc takes real time, string funcName returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", funcName )
    call YDWESaveRealByString( s, "Time", time )
    //set g=null
endfunction

// special effect
function YDWESpecialEffectDestroy takes nothing returns nothing
    local timer t = GetExpiredTimer()
    local effect eff = YDWEGetEffectByString( I2S(YDWEH2I(t)), "effect" )
    call DestroyEffect( eff )
    call YDWEFlushMissionByString(I2S(YDWEH2I(t)))
    call DestroyTimer( t )
    set t = null
    set eff = null
endfunction
function YDWESpecialEffectDestroyOverTime takes effect eff, real duration returns nothing
    local timer t = CreateTimer()
    call YDWESaveEffectByString( I2S(YDWEH2I(t)), "effect", eff )
    call TimerStart(t, duration, false, function YDWESpecialEffectDestroy)
    set t = null
endfunction

function YDWESpecialEffectPoint takes nothing returns nothing
    //local gamecache g = GC()
    local effect eff = null
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local real x = YDWEGetRealByString( s, "real1" )
    local real y = YDWEGetRealByString( s, "real2" )
    local real z = YDWEGetRealByString( s, "real3" )
    local real duration = YDWEGetRealByString( s, "real4" )
    local string path = YDWEGetStringByString( s, "str1" )

    local destructable d = CreateDestructableZ( 'OTip', x, y, z, 0.00, 1, 0 )
    set eff = AddSpecialEffect(path, x, y)
    call RemoveDestructable( d )

    call YDWESpecialEffectDestroyOverTime(eff, duration)

    set d = null
    set eff = null
    //set g=null
endfunction

function YDWEScriptPointEffect takes real time, real x, real y, real z, string path, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWESpecialEffectPoint" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", x )
    call YDWESaveRealByString( s, "real2", y )
    call YDWESaveRealByString( s, "real3", z )
    call YDWESaveRealByString( s, "real4", duration )
    call YDWESaveStringByString( s, "str1", path )
    //set g=null
endfunction

function YDWESpecialEffectActor takes nothing returns nothing
    //local gamecache g = GC()
    local effect eff = null
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real duration = YDWEGetRealByString( s, "real1" )
    local string path = YDWEGetStringByString( s, "str1" )
    local string attPoint = YDWEGetStringByString( s, "str2" )

    set eff = AddSpecialEffectTarget(path, YDWE_cinS_actor[actID], attPoint)

    call YDWESpecialEffectDestroyOverTime(eff, duration)
    set eff = null
    //set g=null
endfunction

function YDWEScriptActEffect takes real time, integer actor, string path, string attachpoint, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWESpecialEffectActor" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "real1", duration )
    call YDWESaveStringByString( s, "str1", path )
    call YDWESaveStringByString( s, "str2", attachpoint )
    //set g=null
endfunction

// play sound
function YDWEPlaySoundPath takes nothing returns nothing
    //local gamecache g = GC()
    local sound snd = null
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer volume = YDWEGetIntegerByString( s, "int1" )
    local real pitch = YDWEGetRealByString( s, "real1" )
    local string path = YDWEGetStringByString( s, "str1" )

    set snd = CreateSound(path, false, false, false, 10, 10, "")
    call SetSoundPitch(snd, pitch)
    call SetSoundVolume(snd, volume)
    call StartSound(snd)
    call KillSoundWhenDone(snd)
    set snd = null
    //set g=null
endfunction

function YDWEPreloadSoundPath takes nothing returns nothing
    //local gamecache g = GC()
    local timer t = GetExpiredTimer()
    local string path = YDWEGetStringByString( I2S(YDWEH2I(t)), "path" )
    local sound snd = CreateSound(path, false, false, false, 10, 10, "")
    call SetSoundVolume(snd, 0)
    call StartSound(snd)
    call KillSoundWhenDone(snd)
    set snd = null
    call YDWEFlushMissionByString( I2S(YDWEH2I(t)) )
    call DestroyTimer(t)
    set t = null
    //set g=null
endfunction

function YDWEScriptPlaySound takes real time, string path, real pitch, integer volume returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local timer t
    if not(YDWEGetBooleanByString("preloading", path)) then
        set t = CreateTimer()
        call YDWESaveStringByString( I2S(YDWEH2I(t)), "path", path )
        call TimerStart(t, 1.00, false, function YDWEPreloadSoundPath)
        call YDWESaveBooleanByString("preloading", path, true)
        set t = null
    endif

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEPlaySoundPath" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", pitch )
    call YDWESaveIntegerByString( s, "int1", volume )
    call YDWESaveStringByString( s, "str1", path )
    //set g=null
endfunction

// ability level
function YDWEActorAbilityLevel takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local integer abilityID = YDWEGetIntegerByString( s, "int1" )
    local integer level = YDWEGetIntegerByString( s, "int2" )
    if level>0 then
        call UnitRemoveAbility( YDWE_cinS_actor[actID], abilityID )
        call UnitAddAbility( YDWE_cinS_actor[actID], abilityID )
        call SetUnitAbilityLevel( YDWE_cinS_actor[actID], abilityID, level )
      else
        call UnitRemoveAbility( YDWE_cinS_actor[actID], abilityID )
    endif
    //set g=null
endfunction

function YDWEScriptActAbilityLvl takes real time, integer actor, integer abilityID, integer level returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorAbilityLevel" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveIntegerByString( s, "int1", abilityID )
    call YDWESaveIntegerByString( s, "int2", level )
    //set g=null
endfunction

// play animation
function YDWEActorPlayAnimation takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local integer animationID = YDWEGetIntegerByString( s, "int1" )
    local real animationBlend = YDWEGetRealByString( s, "real1" )

    call SetUnitBlendTime(YDWE_cinS_actor[actID], animationBlend)
    call SetUnitAnimationByIndex( YDWE_cinS_actor[actID], animationID )
    //set g=null
endfunction

function YDWEScriptActAnimPlay takes real time, integer actor, integer animationID, real animationBlend returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorPlayAnimation" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveIntegerByString( s, "int1", animationID )
    call YDWESaveRealByString( s, "real1", animationBlend )
    //set g=null
endfunction

function YDWEActorResetAnimation takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    call ResetUnitAnimation( YDWE_cinS_actor[actID] )
    //set g=null
endfunction

function YDWEScriptActAnimReset takes real time, integer actor returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorResetAnimation" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    //set g=null
endfunction

// animation speed
function YDWEActorSpeedAnimation takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real animationSpeed = YDWEGetRealByString( s, "real1" )

    call SetUnitTimeScale( YDWE_cinS_actor[actID], animationSpeed )
    //set g=null
endfunction

function YDWEScriptActAnimSpeed takes real time, integer actor, real speed returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorSpeedAnimation" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", speed )
    //set g=null
endfunction

// slide unit
function YDWEActorSlide takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real destX = YDWEGetRealByString( s, "real1" )
    local real destY = YDWEGetRealByString( s, "real2" )
    local real duration = YDWEGetRealByString( s, "real3" )
    local real gr = YDWEGetRealByString( s, "real4" )

    call YDWEUnitMove( YDWE_cinS_actor[actID], destX, destY, duration, gr )
    //set g=null
endfunction

function YDWEScriptActSlide takes real time, integer actor, real x, real y, real duration, real gr returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorSlide" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", x )
    call YDWESaveRealByString( s, "real2", y )
    call YDWESaveRealByString( s, "real3", duration )
    call YDWESaveRealByString( s, "real4", gr )
    //set g=null
endfunction

// walk unit
function YDWEActorWalkProperties takes integer unitID, integer walkAnimID, real walkAnimDuration, real walkSpeed returns nothing
    local real speed = walkSpeed / walkAnimDuration
    local string s = "ActorStats "+I2S(unitID)

    call YDWESaveRealByString( s, "speed", speed )
    call YDWESaveIntegerByString( s, "animation", walkAnimID )
endfunction

function YDWEActorWalk takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real destX = YDWEGetRealByString( s, "real1" )
    local real destY = YDWEGetRealByString( s, "real2" )
    local real duration = YDWEGetRealByString( s, "real3" )
    local real animationBlend = YDWEGetRealByString( s, "real4" )

    call YDWEUnitMove( YDWE_cinS_actor[actID], destX, destY, duration, 0 )

    set s = "ActorStats "+I2S(GetUnitTypeId(YDWE_cinS_actor[actID]))
    set destX=destX-GetUnitX(YDWE_cinS_actor[actID])
    set destY=destY-GetUnitY(YDWE_cinS_actor[actID])

    call YDWEUnitTurn( YDWE_cinS_actor[actID], bj_RADTODEG * Atan2(destY , destX), animationBlend, 0 )

    call SetUnitBlendTime(YDWE_cinS_actor[actID], animationBlend)
    call SetUnitTimeScale( YDWE_cinS_actor[actID], (SquareRoot(destX*destX+destY*destY) / (duration+0.01) / (YDWEGetRealByString( s, "speed"))+0.01) )
    call SetUnitAnimationByIndex( YDWE_cinS_actor[actID], YDWEGetIntegerByString( s, "animation") )
    //set g=null
endfunction

function YDWEScriptActWalk takes real time, integer actor, real x, real y, real duration, real animationBlend returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorWalk" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", x )
    call YDWESaveRealByString( s, "real2", y )
    call YDWESaveRealByString( s, "real3", duration )
    call YDWESaveRealByString( s, "real4", animationBlend )
    //set g=null
endfunction

// turn unit
function YDWEActorTurn takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real destAng = YDWEGetRealByString( s, "real1" )
    local real duration = YDWEGetRealByString( s, "real2" )
    local integer clockwise = YDWEGetIntegerByString( s, "int1" )

    call YDWEUnitTurn( YDWE_cinS_actor[actID], destAng, duration, clockwise )
    //set g=null
endfunction

function YDWEScriptActTurn takes real time, integer actor, real destAng, real duration, integer clockwise returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorTurn" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", destAng )
    call YDWESaveRealByString( s, "real2", duration )
    call YDWESaveIntegerByString( s, "int1", clockwise )
    //set g=null
endfunction

function YDWEActorFace takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local integer targetID = YDWEGetIntegerByString( s, "int1" )
    local integer headORchest = YDWEGetIntegerByString( s, "int2" )
    local real x = YDWEGetRealByString( s, "real1" )
    local real y = YDWEGetRealByString( s, "real2" )
    local real z = YDWEGetRealByString( s, "real3" )
    local real animationBlend = YDWEGetRealByString( s, "real4" )

    if headORchest == 0 then
        set s = "Bone_Head"
    else
        set s = "Bone_Chest"
    endif

    call SetUnitBlendTime(YDWE_cinS_actor[actID], animationBlend)
    call SetUnitLookAt( YDWE_cinS_actor[actID], s, YDWE_cinS_actor[targetID], x,y,z )
    //set g=null
endfunction

function YDWEScriptActFace takes real time, integer actor, integer destActor, integer headORchest, real xoff, real yoff, real zoff, real animationBlend returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorFace" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveIntegerByString( s, "int1", destActor )
    call YDWESaveIntegerByString( s, "int2", headORchest )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", xoff )
    call YDWESaveRealByString( s, "real2", yoff )
    call YDWESaveRealByString( s, "real3", zoff )
    call YDWESaveRealByString( s, "real4", animationBlend )
    //set g=null
endfunction

function YDWEActorFaceReset takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real animationBlend = YDWEGetRealByString( s, "real1" )

    call SetUnitBlendTime(YDWE_cinS_actor[actID], animationBlend)
    call ResetUnitLookAt( YDWE_cinS_actor[actID] )
    //set g=null
endfunction

function YDWEScriptActFaceReset takes real time, integer actor, real animationBlend returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorFaceReset" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", animationBlend )
    //set g=null
endfunction

// vertex colouring
function YDWEActorColour takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real red = YDWEGetRealByString( s, "real1" )
    local real green = YDWEGetRealByString( s, "real2" )
    local real blue = YDWEGetRealByString( s, "real3" )
    local real opacity = YDWEGetRealByString( s, "real4" )
    local real duration = YDWEGetRealByString( s, "real5" )

    call YDWEUnitColour( YDWE_cinS_actor[actID], red, green, blue, opacity, duration )
    //set g=null
endfunction

function YDWEScriptActColour takes real time, integer actor, real red, real green, real blue, real opacity, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorColour" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )

    call YDWESaveRealByString( s, "real1", red )
    call YDWESaveRealByString( s, "real2", green )
    call YDWESaveRealByString( s, "real3", blue )
    call YDWESaveRealByString( s, "real4", opacity )
    call YDWESaveRealByString( s, "real5", duration )
    //set g=null
endfunction

// size unit
function YDWEActorSize takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real size = YDWEGetRealByString( s, "real1" )
    local real duration = YDWEGetRealByString( s, "real2" )

    call YDWEUnitSize( YDWE_cinS_actor[actID], size, duration )
    //set g=null
endfunction

function YDWEScriptActSize takes real time, integer actor, real size, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorSize" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", size )
    call YDWESaveRealByString( s, "real2", duration )
    //set g=null
endfunction

// camera scripting
function YDWECameraAction takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer camID = YDWEGetIntegerByString( s, "actor" )
    local integer changeType = YDWEGetIntegerByString( s, "int1" )
    local real duration = YDWEGetRealByString( s, "real1" )

    if changeType == 1 then
        call YDWEApplyCameraInstantly( YDWE_cinS_camera[camID] )
    elseif changeType == 2 then
        call YDWEApplyCameraLinear( YDWE_cinS_camera[camID], duration )
    elseif changeType == 3 then
        call YDWEApplyCameraSmooth( YDWE_cinS_camera[camID], duration )
    elseif changeType == 4 then
        call YDWEApplyCameraSmoothStop( YDWE_cinS_camera[camID], duration )
    endif
    //set g=null
endfunction

function YDWEScriptCamera takes real time, integer camera, real duration, integer changeType returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWECameraAction" )

    call YDWESaveIntegerByString( s, "actor", camera )
    call YDWESaveIntegerByString( s, "int1", changeType )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    //set g=null
endfunction


function YDWECameraStatChangeInst takes integer stat, real endvalue returns nothing
    set YDWE_cinS_cam_startval[stat] = endvalue
    set YDWE_cinS_cam_startvel[stat] = 0.0
    set YDWE_cinS_cam_accprim[stat] = 0.0
    set YDWE_cinS_cam_accsec[stat] = 0.0
    set YDWE_cinS_cam_time[stat] = 0.0
    set YDWE_cinS_cam_endtime[stat] = 0.0
endfunction

function YDWECameraStatAction takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer statID = YDWEGetIntegerByString( s, "actor" )
    local integer changeType = YDWEGetIntegerByString( s, "int1" )
    local real duration = YDWEGetRealByString( s, "real1" )
    local real endValue = YDWEGetRealByString( s, "real2" )
    local real endSpeed = YDWEGetRealByString( s, "real3" )

    if statID==4 or statID==5 or statID==6 then
        if changeType == 1 then
            call YDWECameraStatChangeInst( statID, endValue )
        elseif changeType == 2 then
            call YDWECameraStatChangeLinRad( statID, endValue, duration )
        elseif changeType == 3 then
            call YDWECameraStatChangeRad( statID, endValue, duration )
        elseif changeType == 4 then
            call YDWECameraStatChangeAdvRad( statID, endValue, endSpeed, duration )
        endif
    else
        if changeType == 1 then
            call YDWECameraStatChangeInst( statID, endValue )
        elseif changeType == 2 then
            call YDWECameraStatChangeLin( statID, endValue, duration )
        elseif changeType == 3 then
            call YDWECameraStatChange( statID, endValue, duration )
        elseif changeType == 4 then
            call YDWECameraStatChangeAdv( statID, endValue, endSpeed, duration )
        endif
    endif
    //set g=null
endfunction

function YDWEScriptCameraStat takes real time, integer statID, real duration, real endValue, real endSpeed, integer changeType returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWECameraStatAction" )

    call YDWESaveIntegerByString( s, "actor", statID )
    call YDWESaveIntegerByString( s, "int1", changeType )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    call YDWESaveRealByString( s, "real2", endValue )
    call YDWESaveRealByString( s, "real3", endSpeed )
    //set g=null
endfunction

// camera statID list
// 1 - x coordinate; 2 - y coordinate; 3 - z coordinate;
// 4 - facing angle; 5 - angle of attack; 6 - roll angle;
// 7 - field ov view; 8 - far z; 9 - distance to target

function YDWECameraIgnoreTerrain takes nothing returns nothing
    set YDWE_cinS_cam_ignoreTerrain = true
endfunction
function YDWECameraUnignoreTerrain takes nothing returns nothing
    set YDWE_cinS_cam_ignoreTerrain = false
endfunction

function YDWEScriptCameraIgnoreTerrain takes real time, boolean ignore returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    if ignore then
        call YDWESaveStringByString( s, "YDWEActionType", "YDWECameraIgnoreTerrain" )
    else
        call YDWESaveStringByString( s, "YDWEActionType", "YDWECameraUnignoreTerrain" )
    endif

    //set g=null
endfunction

function YDWEActorCameraLock takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real duration = YDWEGetRealByString( s, "real1" )
    local real xoff = YDWEGetRealByString( s, "real2" )
    local real yoff = YDWEGetRealByString( s, "real3" )
    local real zoff = YDWEGetRealByString( s, "real4" )
    local boolean smooth = YDWEGetIntegerByString( s, "int1" )==1

    call YDWECameraLockToUnitOverride(YDWE_cinS_actor[actID], xoff, yoff, zoff, smooth, duration )
    //set g=null
endfunction

function YDWEScriptActCameraLock takes real time, integer actor, real xoff, real yoff, real zoff, boolean smooth, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorCameraLock" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "real2", xoff )
    call YDWESaveRealByString( s, "real3", yoff )
    call YDWESaveRealByString( s, "real4", zoff )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    if smooth then
        call YDWESaveIntegerByString( s, "int1", 1 )
    else
        call YDWESaveIntegerByString( s, "int1", 0 )
    endif
    //set g=null
endfunction

function YDWEActorCameraLockReset takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local real duration = YDWEGetRealByString( s, "real1" )
    local boolean smooth = YDWEGetIntegerByString( s, "int1" )==1

    call YDWECameraLockToUnitReset( smooth, duration )
    //set g=null
endfunction

function YDWEScriptActCameraLockReset takes real time, boolean smooth, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorCameraLockReset" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    if smooth then
        call YDWESaveIntegerByString( s, "int1", 1 )
    else
        call YDWESaveIntegerByString( s, "int1", 0 )
    endif
    //set g=null
endfunction

function YDWEActorCameraFace takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local integer actID = YDWEGetIntegerByString( s, "actor" )
    local real duration = YDWEGetRealByString( s, "real1" )
    local real xoff = YDWEGetRealByString( s, "real2" )
    local real yoff = YDWEGetRealByString( s, "real3" )
    local real zoff = YDWEGetRealByString( s, "real4" )
    local boolean smooth = YDWEGetIntegerByString( s, "int1" )==1

    call YDWECameraFaceToUnitOverride(YDWE_cinS_actor[actID], xoff, yoff, zoff, smooth, duration )
    //set g=null
endfunction

function YDWEScriptActCameraFace takes real time, integer actor, real xoff, real yoff, real zoff, boolean smooth, real duration returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorCameraFace" )

    call YDWESaveIntegerByString( s, "actor", actor )
    call YDWESaveRealByString( s, "real2", xoff )
    call YDWESaveRealByString( s, "real3", yoff )
    call YDWESaveRealByString( s, "real4", zoff )
    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    if smooth then
        call YDWESaveIntegerByString( s, "int1", 1 )
    else
        call YDWESaveIntegerByString( s, "int1", 0 )
    endif
    //set g=null
endfunction

function YDWEActorCameraFaceReset takes nothing returns nothing
    //local gamecache g = GC()
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local real duration = YDWEGetRealByString( s, "real1" )
    local boolean smooth = YDWEGetIntegerByString( s, "int1" )==1

    call YDWECameraFaceToUnitReset( smooth, duration )
    //set g=null
endfunction

function YDWEScriptActCameraFaceReset takes real time, boolean smooth, real duration returns nothing
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEActorCameraFaceReset" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    if smooth then
        call YDWESaveIntegerByString( s, "int1", 1 )
    else
        call YDWESaveIntegerByString( s, "int1", 0 )
    endif
    //set g=null
endfunction


function YDWEFogChange takes nothing returns nothing
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local real duration = YDWEGetRealByString( s, "real1" )
    local real startZ = YDWEGetRealByString( s, "real2" )
    local real endZ = YDWEGetRealByString( s, "real3" )
    local real red = YDWEGetRealByString( s, "real4" )
    local real green = YDWEGetRealByString( s, "real5" )
    local real blue = YDWEGetRealByString( s, "real6" )

    call YDWEApplyFog(startZ, endZ, red, green, blue, duration)
endfunction

function YDWEScriptFog takes real time, real startZ, real endZ, real red, real green, real blue, real duration returns nothing
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEFogChange" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    call YDWESaveRealByString( s, "real2", startZ )
    call YDWESaveRealByString( s, "real3", endZ )
    call YDWESaveRealByString( s, "real4", red )
    call YDWESaveRealByString( s, "real5", green )
    call YDWESaveRealByString( s, "real6", blue )
    //set g=null
endfunction

function YDWEFadeChange takes nothing returns nothing
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local real duration = YDWEGetRealByString( s, "real1" )
    local real red = YDWEGetRealByString( s, "real2" )
    local real green = YDWEGetRealByString( s, "real3" )
    local real blue = YDWEGetRealByString( s, "real4" )
    local real opacity = YDWEGetRealByString( s, "real5" )

    call YDWEApplyFade(red, green, blue, opacity, duration)
endfunction

function YDWEScriptFade takes real time, real red, real green, real blue, real opacity, real duration returns nothing
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWEFadeChange" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveRealByString( s, "real1", duration )
    call YDWESaveRealByString( s, "real2", red )
    call YDWESaveRealByString( s, "real3", green )
    call YDWESaveRealByString( s, "real4", blue )
    call YDWESaveRealByString( s, "real5", opacity )
    //set g=null
endfunction



function YDWESubtitleDisplay takes nothing returns nothing
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    local string line = YDWEGetStringByString( s, "string1" )
    if YDWE_cinS_displaySubtitles then
        call YDWEDisplaySubtitle(line)
    endif
    //set g=null
endfunction

function YDWEScriptSubtitleLine takes real time, string line returns nothing
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWESubtitleDisplay" )

    call YDWESaveRealByString( s, "Time", time )
    call YDWESaveStringByString( s, "string1", line )
    //set g=null
endfunction

function YDWESubtitleClear takes nothing returns nothing
    local integer scene = YDWEGetIntegerByString( "Interpreter", "Scene" )
    local integer action = YDWEGetIntegerByString( "Interpreter", "PendingAction" )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWEClearSubtitles()
    //set g=null
endfunction

function YDWEScriptSubtitleClear takes real time returns nothing
    local integer scene = YDWEGetIntegerByString( "Scripter", "Scene" )
    local integer action = YDWEGetActionSpot( scene, time )
    local string s = "S"+I2S(scene)+"A"+I2S(action)

    call YDWESaveStringByString( s, "YDWEActionType", "YDWESubtitleClear" )

    call YDWESaveRealByString( s, "Time", time )
    //set g=null
endfunction


function YDWEScriptSystemInit takes nothing returns nothing
    local trigger t = CreateTrigger()
    call TriggerRegisterTimerEvent(t, YDWE_cinS_ScriptSystemPeriod, true)
    call TriggerAddAction( t, function YDWEScriptInterpreter )
    set t = null
endfunction

function YDWEInitCinematicSystem takes nothing returns nothing
    set YDWE_cinS_tempPoint=Location(0.0,0.0)
    call YDWEScriptSystemInit()
    call YDWECameraSystemInit()
    call YDWEParticleInit()
    call YDWESubtitlesInit()
endfunction

endlibrary

#endif /// YDWECinematicSystemIncluded
