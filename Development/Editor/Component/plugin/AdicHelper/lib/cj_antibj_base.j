/*
    cj_antibj_base v 0.11 (beta)

    Intended to optimize map script by replacing the calls to Blizzard.j functions to their common.j analogs.
    Mainly useful for fast automated compile-time optimization of GUI triggers.
    This script doesn't remove locations or fix any poor code problems.
    For more extreme map optimization you can use Wc3mapoptimizer by Vexorian

    Intended for usage with game version 1.24+

    This file is part of cJass standard library
    © 2009 ADOLF \\ cjass.xgm.ru
*/

#guard cj_antibj_base

define {

    // At first we are to remove such constructs
    <else;endif>             = endif
    <else;DoNothing();endif> = endif
    <else;call DoNothing();endif> = endif

    // These BJ constants, used in map, will be replaced to their values.
    bj_PI                            = 3.14159
    bj_E                             = 2.71828
    bj_CELLWIDTH                     = 128.
    bj_CLIFFHEIGHT                   = 128.
    bj_UNIT_FACING                   = 270.
    bj_RADTODEG                      = 57.2958
    bj_DEGTORAD                      = .0174532
    bj_TEXT_DELAY_QUEST              = 20.
    bj_TEXT_DELAY_QUESTUPDATE        = 20.
    bj_TEXT_DELAY_QUESTDONE          = 20.
    bj_TEXT_DELAY_QUESTFAILED        = 20.
    bj_TEXT_DELAY_QUESTREQUIREMENT   = 20.
    bj_TEXT_DELAY_MISSIONFAILED      = 20.
    bj_TEXT_DELAY_ALWAYSHINT         = 12.
    bj_TEXT_DELAY_HINT               = 12.
    bj_TEXT_DELAY_SECRET             = 10.
    bj_TEXT_DELAY_UNITACQUIRED       = 15.
    bj_TEXT_DELAY_UNITAVAILABLE      = 10.
    bj_TEXT_DELAY_ITEMACQUIRED       = 10.
    bj_TEXT_DELAY_WARNING            = 12.
    bj_QUEUE_DELAY_QUEST             = 5.
    bj_QUEUE_DELAY_HINT              = 5.
    bj_QUEUE_DELAY_SECRET            = 3.
    bj_HANDICAP_EASY                 = 60.
    bj_GAME_STARTED_THRESHOLD        = .01
    bj_WAIT_FOR_COND_MIN_INTERVAL    = .1
    bj_POLLED_WAIT_INTERVAL          = .1
    bj_POLLED_WAIT_SKIP_THRESHOLD    = 2.

    bj_MAX_INVENTORY                 =  6
    bj_MAX_PLAYERS                   = 12
    bj_PLAYER_NEUTRAL_VICTIM         = 13
    bj_PLAYER_NEUTRAL_EXTRA          = 14
    bj_MAX_PLAYER_SLOTS              = 16
    bj_MAX_SKELETONS                 = 25
    bj_MAX_STOCK_ITEM_SLOTS          = 11
    bj_MAX_STOCK_UNIT_SLOTS          = 11
    bj_MAX_ITEM_LEVEL                = 10

    bj_MELEE_STARTING_TOD            = 8.
    bj_MELEE_STARTING_GOLD_V0        = 750
    bj_MELEE_STARTING_GOLD_V1        = 500
    bj_MELEE_STARTING_LUMBER_V0      = 200
    bj_MELEE_STARTING_LUMBER_V1      = 150
    bj_MELEE_STARTING_HERO_TOKENS    = 1
    bj_MELEE_HERO_LIMIT              = 3
    bj_MELEE_HERO_TYPE_LIMIT         = 1
    bj_MELEE_MINE_SEARCH_RADIUS      = 2000
    bj_MELEE_CLEAR_UNITS_RADIUS      = 1500
    bj_MELEE_CRIPPLE_TIMEOUT         = 120.
    bj_MELEE_CRIPPLE_MSG_DURATION    = 20.
    bj_MELEE_MAX_TWINKED_HEROES_V0   = 3
    bj_MELEE_MAX_TWINKED_HEROES_V1   = 1

    bj_CREEP_ITEM_DELAY              = .5

    bj_STOCK_RESTOCK_INITIAL_DELAY   = 120
    bj_STOCK_RESTOCK_INTERVAL        = 30
    bj_STOCK_MAX_ITERATIONS          = 20

    bj_MAX_DEST_IN_REGION_EVENTS     = 64

    bj_CAMERA_MIN_FARZ               = 100
    bj_CAMERA_DEFAULT_DISTANCE       = 1650
    bj_CAMERA_DEFAULT_FARZ           = 5000
    bj_CAMERA_DEFAULT_AOA            = 304
    bj_CAMERA_DEFAULT_FOV            = 70
    bj_CAMERA_DEFAULT_ROLL           = 0
    bj_CAMERA_DEFAULT_ROTATION       = 90

    bj_RESCUE_PING_TIME              = 2.

    bj_NOTHING_SOUND_DURATION        = 5.
    bj_TRANSMISSION_PING_TIME        = 1.
    bj_TRANSMISSION_IND_RED          = 255
    bj_TRANSMISSION_IND_BLUE         = 255
    bj_TRANSMISSION_IND_GREEN        = 255
    bj_TRANSMISSION_IND_ALPHA        = 255
    bj_TRANSMISSION_PORT_HANGTIME    = 1.5

    bj_CINEMODE_INTERFACEFADE        = .5
    bj_CINEMODE_GAMESPEED            = MAP_SPEED_NORMAL // constant from common.j

    bj_CINEMODE_VOLUME_UNITMOVEMENT  = .4
    bj_CINEMODE_VOLUME_UNITSOUNDS    = .0
    bj_CINEMODE_VOLUME_COMBAT        = .4
    bj_CINEMODE_VOLUME_SPELLS        = .4
    bj_CINEMODE_VOLUME_UI            = .0
    bj_CINEMODE_VOLUME_MUSIC         = .55
    bj_CINEMODE_VOLUME_AMBIENTSOUNDS = 1.
    bj_CINEMODE_VOLUME_FIRE          = .6

    bj_SPEECH_VOLUME_UNITMOVEMENT    = .25
    bj_SPEECH_VOLUME_UNITSOUNDS      = .0
    bj_SPEECH_VOLUME_COMBAT          = .25
    bj_SPEECH_VOLUME_SPELLS          = .25
    bj_SPEECH_VOLUME_UI              = .0
    bj_SPEECH_VOLUME_MUSIC           = .55
    bj_SPEECH_VOLUME_AMBIENTSOUNDS   = 1.
    bj_SPEECH_VOLUME_FIRE            = .6

    bj_SMARTPAN_TRESHOLD_PAN         = 500
    bj_SMARTPAN_TRESHOLD_SNAP        = 3500

    bj_MAX_QUEUED_TRIGGERS           = 100
    bj_QUEUED_TRIGGER_TIMEOUT        = 180.

    bj_GAMECACHE_BOOLEAN             = 0
    bj_GAMECACHE_INTEGER             = 1
    bj_GAMECACHE_REAL                = 2
    bj_GAMECACHE_UNIT                = 3
    bj_GAMECACHE_STRING              = 4
	
    bj_HASHTABLE_BOOLEAN             = 0
    bj_HASHTABLE_INTEGER             = 1
    bj_HASHTABLE_REAL                = 2
    bj_HASHTABLE_STRING              = 3
    bj_HASHTABLE_HANDLE              = 4

    bj_ITEM_STATUS_HIDDEN            = 0
    bj_ITEM_STATUS_OWNED             = 1
    bj_ITEM_STATUS_INVULNERABLE      = 2
    bj_ITEM_STATUS_POWERUP           = 3
    bj_ITEM_STATUS_SELLABLE          = 4
    bj_ITEM_STATUS_PAWNABLE          = 5

    bj_ITEMCODE_STATUS_POWERUP       = 0
    bj_ITEMCODE_STATUS_SELLABLE      = 1
    bj_ITEMCODE_STATUS_PAWNABLE      = 2

    bj_MINIMAPPINGSTYLE_SIMPLE       = 0
    bj_MINIMAPPINGSTYLE_FLASHY       = 1
    bj_MINIMAPPINGSTYLE_ATTACK       = 2

    bj_CORPSE_MAX_DEATH_TIME         = 8.

    bj_CORPSETYPE_FLESH              = 0
    bj_CORPSETYPE_BONE               = 1

    bj_ELEVATOR_BLOCKER_CODE         = 'DTep'
    bj_ELEVATOR_CODE01               = 'DTrf'
    bj_ELEVATOR_CODE02               = 'DTrx'

    bj_ELEVATOR_WALL_TYPE_ALL        = 0
    bj_ELEVATOR_WALL_TYPE_EAST       = 1
    bj_ELEVATOR_WALL_TYPE_NORTH      = 2
    bj_ELEVATOR_WALL_TYPE_SOUTH      = 3
    bj_ELEVATOR_WALL_TYPE_WEST       = 4

    // Then we'll replace lots of other "bad" functions
    // because almost all of them are just wrappers

    <call DoNothing()> =
    <DoNothing()> =

    SinBJ (r)  = Sin((r) * bj_DEGTORAD)
    CosBJ (r)  = Cos((r) * bj_DEGTORAD)
    TanBJ (r)  = Tan((r) * bj_DEGTORAD)
    AsinBJ (r) = Asin(r) * bj_RADTODEG
    AcosBJ (r) = Acos(r) * bj_RADTODEG
    AtanBJ (r) = Atan(r) * bj_RADTODEG
    Atan2BJ (x, y) = Atan2(y, x) * bj_RADTODEG

    // Here we just use simpler functions
    <call SinBJ> (r) = Sin(r)
    <call CosBJ> (r) = Cos(r)
    <call TanBJ> (r) = Tan(r)
    <call AsinBJ> (r) = Asin(r)
    <call AcosBJ> (r) = Acos(r)
    <call AtanBJ> (r) = Atan(r)
    <call Atan2BJ> (x, y) = Atan2(x, y)

    <GetRandomDirectionDeg()> = GetRandomReal(0, 360)
    <GetRandomPercentageBJ()> = GetRandomReal(0, 100)

    <call QueuedTriggerClearBJ()>         = { PauseTimer(bj_queuedExecTimeoutTimer) ; bj_queuedExecTotal = 0 }
    <QueuedTriggerClearBJ()>              = { PauseTimer(bj_queuedExecTimeoutTimer) ; bj_queuedExecTotal = 0 }
    <call QueuedTriggerClearInactiveBJ()> = bj_queuedExecTotal = IMinBJ(bj_queuedExecTotal, 1)
    <QueuedTriggerClearInactiveBJ()>      = bj_queuedExecTotal = IMinBJ(bj_queuedExecTotal, 1)
    <IsTriggerQueueEmptyBJ()>             = (bj_queuedExecTotal <= 0)
    IsTriggerQueuedBJ (t)                 = (QueuedTriggerGetIndex(t) != -1)
    <call QueuedTriggerCountBJ()>         =
    <QueuedTriggerCountBJ()>              = bj_queuedExecTotal
    <call IsTriggerQueueEmptyBJ()>        =
    <call IsTriggerQueuedBJ> (t)          =

    <GetForLoopIndexA()>        = bj_forLoopAIndex
    <GetForLoopIndexB()>        = bj_forLoopBIndex
    <call SetForLoopIndexA> (i) = bj_forLoopAIndex = i
    <call SetForLoopIndexB> (i) = bj_forLoopBIndex = i
    <call GetForLoopIndexA()>   =
    <call GetForLoopIndexB()>   =

    StringIdentity (s) = GetLocalizedString(s)

    GetBooleanAnd (a, b)        = (a && b)
    GetBooleanOr  (a, b)        = (a || b)
    <call GetBooleanAnd> (a, b) = Get ## BooleanAnd(a, b)
    <call GetBooleanOr>  (a, b) = Get ## BooleanOr(a, b)

    PercentTo255 (r) = PercentToInt(r, 255)

    <GetTimeOfDay()> = GetFloatGameState(GAME_STATE_TIME_OF_DAY)
    SetTimeOfDay (r) = SetFloatGameState(GAME_STATE_TIME_OF_DAY, r)

    SetTimeOfDayScalePercentBJ (r) = SetTimeOfDayScale((r) * .01)
    <GetTimeOfDayScalePercentBJ()> = GetTimeOfDayScale() * 100.
    <call GetTimeOfDayScalePercentBJ()> =

    <call CameraSetupApplyForPlayer> (b, c, p, r)     = { if GetLocalPlayer() == p { CameraSetupApplyForceDuration(c, b, r) } }
    <call SetCameraFieldForPlayer> (p, c, a, b)       = { if GetLocalPlayer() == p { SetCameraField(c, a, b) } }
    <call SetCameraPositionForPlayer> (p, x, y)       = { if GetLocalPlayer() == p { SetCameraPosition(x, y) } }
    <call PanCameraToForPlayer> (p, x, y)             = { if GetLocalPlayer() == p { PanCameraTo(x, y) } }
    <call PanCameraToTimedForPlayer> (p, x, y, r)     = { if GetLocalPlayer() == p { PanCameraToTimed(x, y, r) } }
    <call SetCinematicCameraForPlayer> (p, s)         = { if GetLocalPlayer() == p { SetCinematicCamera(s) } }
    <call ResetToGameCameraForPlayer> (p, r)          = { if GetLocalPlayer() == p { ResetToGameCamera(r) } }
    <call CameraSetSourceNoiseForPlayer> (p, a, b)    = { if GetLocalPlayer() == p { CameraSetSourceNoise(a, b) } }
    <call CameraSetTargetNoiseForPlayer> (p, a, b)    = { if GetLocalPlayer() == p { CameraSetTargetNoise(a, b) } }
    <call SetCameraTargetControllerNoZForPlayer> (p, u, x, y, b) = { if GetLocalPlayer() == p { SetCameraTargetController(u, x, y, b) } }

    <CameraSetupApplyForPlayer> (b, c, p, r)     = { if GetLocalPlayer() == p { CameraSetupApplyForceDuration(c, b, r) } }
    <SetCameraFieldForPlayer> (p, c, a, b)>      = { if GetLocalPlayer() == p { SetCameraField(c, a, b) } }
    <SetCameraPositionForPlayer> (p, x, y)       = { if GetLocalPlayer() == p { SetCameraPosition(x, y) } }
    <PanCameraToForPlayer> (p, x, y)             = { if GetLocalPlayer() == p { PanCameraTo(x, y) } }
    <PanCameraToTimedForPlayer> (p, x, y, r)     = { if GetLocalPlayer() == p { PanCameraToTimed(x, y, r) } }
    <SetCinematicCameraForPlayer> (p, s)         = { if GetLocalPlayer() == p { SetCinematicCamera(s) } }
    <ResetToGameCameraForPlayer> (p, r)          = { if GetLocalPlayer() == p { ResetToGameCamera(r) } }
    <CameraSetSourceNoiseForPlayer> (p, a, b)    = { if GetLocalPlayer() == p { CameraSetSourceNoise(a, b) } }
    <CameraSetTargetNoiseForPlayer> (p, a, b)    = { if GetLocalPlayer() == p { CameraSetTargetNoise(a, b) } }
    <SetCameraTargetControllerNoZForPlayer> (p, u, x, y, b) = { if GetLocalPlayer() == p { SetCameraTargetController(u, x, y, b) } }

    CameraSetupGetFieldSwap (a, b) = CameraSetupGetField(b, a)

    <GetCurrentCameraBoundsMapRectBJ()> = Rect(GetCameraBoundMinX(), GetCameraBoundMinY(), GetCameraBoundMaxX(), GetCameraBoundMaxY())

    <GetCameraBoundsMapRect()> = bj_mapInitialCameraBounds
    <call GetCameraBoundsMapRect()> =

    <GetPlayableMapRect()> = bj_mapInitialPlayableArea
    <call GetPlayableMapRect()> =

    GetEntireMapRect = GetWorldBounds

    <call SetCameraBoundsToRectForPlayerBJ> (p, r)           = { if GetLocalPlayer() == p { SetCameraBoundsToRect(r) } }
    <call SetCameraQuickPositionForPlayer> (p, x, y)         = { if GetLocalPlayer() == p { SetCameraQuickPosition(x, y) } }
    <call StopCameraForPlayerBJ> (p)                         = { if GetLocalPlayer() == p { StopCamera() } }
    <call AdjustCameraBoundsForPlayerBJ> (i, p, a, b, c, d)  = { if GetLocalPlayer() == p { AdjustCameraBoundsBJ(i, a, b, c, d) } }
    <call SetCameraOrientControllerForPlayerBJ> (p, u, x, y) = { if GetLocalPlayer() == p { SetCameraOrientController(u, x, y) } }

    <SetCameraBoundsToRectForPlayerBJ> (p, r)           = { if GetLocalPlayer() == p { SetCameraBoundsToRect(r) } }
    <SetCameraQuickPositionForPlayer> (p, x, y)         = { if GetLocalPlayer() == p { SetCameraQuickPosition(x, y) } }
    <StopCameraForPlayerBJ> (p)                         = { if GetLocalPlayer() == p { StopCamera() } }
    <AdjustCameraBoundsForPlayerBJ> (i, p, a, b, c, d)  = { if GetLocalPlayer() == p { AdjustCameraBoundsBJ(i, a, b, c, d) } }
    <SetCameraOrientControllerForPlayerBJ> (p, u, x, y) = { if GetLocalPlayer() == p { SetCameraOrientController(u, x, y) } }

    <CameraSetSmoothingFactorBJ> (r) = CameraSetSmoothingFactor(r)
    <CameraResetSmoothingFactorBJ()> = CameraSetSmoothingFactor(0.)

    <call DisplayTextToForce> (f, s)         = { if IsPlayerInForce(GetLocalPlayer(), f) { DisplayTextToPlayer(GetLocalPlayer(), 0., 0., s) } }
    <call DisplayTimedTextToForce> (f, r, s) = { if IsPlayerInForce(GetLocalPlayer(), f) { DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., r, s) } }
    <call ClearTextMessagesBJ> (f)           = { if IsPlayerInForce(GetLocalPlayer(), f) { ClearTextMessages() } }

    <DisplayTextToForce> (f, s)         = { if IsPlayerInForce(GetLocalPlayer(), f) { DisplayTextToPlayer(GetLocalPlayer(), 0., 0., s) } }
    <DisplayTimedTextToForce> (f, r, s) = { if IsPlayerInForce(GetLocalPlayer(), f) { DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., r, s) } }
    <ClearTextMessagesBJ> (f)           = { if IsPlayerInForce(GetLocalPlayer(), f) { ClearTextMessages() } }

    <call SubStringBJ> (str, s, e) = SubString(str, (s)-1, e) 
    SubStringBJ (str, s, e)        = SubString(str, (s)-1, e)

    <call GetHandleIdBJ> (h) =
    GetHandleIdBJ (h) = GetHandleId(h)

    <call StringHashBJ> (s) =
    StringHashBJ (s) = StringHash(s)

    TriggerRegisterPlayerUnitEventSimple (t, p ,e)   = TriggerRegisterPlayerUnitEvent(t, p, e, null)
    TriggerRegisterTimerEventPeriodic (t, r)         = TriggerRegisterTimerEvent(t, r, true)
    TriggerRegisterTimerEventSingle   (t, r)         = TriggerRegisterTimerEvent(t, r, false)
    TriggerRegisterTimerExpireEventBJ (t, tm)        = TriggerRegisterTimerExpireEvent(t, tm)
    TriggerRegisterPlayerEventVictory (t, p)         = TriggerRegisterPlayerEvent(t, p, EVENT_PLAYER_VICTORY)
    TriggerRegisterPlayerEventDefeat  (t, p)         = TriggerRegisterPlayerEvent(t, p, EVENT_PLAYER_DEFEAT)
    TriggerRegisterPlayerEventLeave   (t, p)         = TriggerRegisterPlayerEvent(t, p, EVENT_PLAYER_LEAVE)
    TriggerRegisterPlayerEventAllianceChanged (t, p) = TriggerRegisterPlayerEvent(t, p, EVENT_PLAYER_ALLIANCE_CHANGED)
    TriggerRegisterPlayerEventEndCinematic    (t, p) = TriggerRegisterPlayerEvent(t, p, EVENT_PLAYER_END_CINEMATIC)
    TriggerRegisterGameStateEventTimeOfDay (t, l, r) = TriggerRegisterGameStateEvent(t, GAME_STATE_TIME_OF_DAY, l, r)
    TriggerRegisterEnterRegionSimple (t, r)          = TriggerRegisterEnterRegion(t, r, null)
    TriggerRegisterLeaveRegionSimple (t, r)          = TriggerRegisterLeaveRegion(t, r, null)
    TriggerRegisterDistanceBetweenUnits (t, u, b, r) = TriggerRegisterUnitInRange(t, u, r, b)
    TriggerRegisterUnitInRangeSimple (t, r, u)       = TriggerRegisterUnitInRange(t, u, r, null)
    TriggerRegisterUnitLifeEvent (t, u, l, r)        = TriggerRegisterUnitStateEvent(t, u, UNIT_STATE_LIFE, l, r)
    TriggerRegisterUnitManaEvent (t, u, l, r)        = TriggerRegisterUnitStateEvent(t, u, UNIT_STATE_MANA, l, r)
    TriggerRegisterDialogEventBJ (t, d)              = TriggerRegisterDialogEvent(t, d)
    TriggerRegisterShowSkillEventBJ (t)              = TriggerRegisterGameEvent(t, EVENT_GAME_SHOW_SKILL)
    TriggerRegisterBuildSubmenuEventBJ(t)            = TriggerRegisterGameEvent(t, EVENT_GAME_BUILD_SUBMENU)
    TriggerRegisterGameLoadedEventBJ (t)             = TriggerRegisterGameEvent(t, EVENT_GAME_LOADED)
    TriggerRegisterGameSavedEventBJ (t)              = TriggerRegisterGameEvent(t, EVENT_GAME_SAVE)

    <call AddWeatherEffectSaveLast> (r, i) = bj_lastCreatedWeatherEffect = AddWeatherEffect(r, i)
    <call GetLastCreatedWeatherEffect()>   =
    <GetLastCreatedWeatherEffect()>        = bj_lastCreatedWeatherEffect
    RemoveWeatherEffectBJ (e)              = RemoveWeatherEffect (e)

    TerrainDeformationStopBJ (d, r) = TerrainDeformStop(d, R2I(r * 1000))

    <call GetLastCreatedTerrainDeformation()> =
    <GetLastCreatedTerrainDeformation()>      = bj_lastCreatedTerrainDeformation

    DestroyLightningBJ (l) = DestroyLightning(l)

    GetLightningColorABJ (l) = GetLightningColorA(l)
    GetLightningColorRBJ (l) = GetLightningColorR(l)
    GetLightningColorGBJ (l) = GetLightningColorG(l)
    GetLightningColorBBJ (l) = GetLightningColorB(l)

    SetLightningColorBJ (l, r, g, b, a) = SetLightningColor(l, r, g, b, a)

    <call GetLastCreatedLightningBJ()> =
    <GetLastCreatedLightningBJ()>      = bj_lastCreatedLightning

    GetAbilityEffectBJ (a, e, i) = GetAbilityEffectById(a, e, i)
    GetAbilitySoundBJ (a, t)     = GetAbilitySoundById(a, t)

    SetWaterBaseColorBJ (r, g, b, a) = SetWaterBaseColor(PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    <call CreateFogModifierRectSimple> (p, f, r, b)         = bj_lastCreatedFogModifier = CreateFogModifierRect(p, f, r, true, b)
    <call CreateFogModifierRadiusLocSimple> (p, f, l, r, b) = bj_lastCreatedFogModifier = CreateFogModifierRadiusLoc(p, f, l, r, true, b)

    <call GetLastCreatedFogModifier()> =
    <GetLastCreatedFogModifier()>      = bj_lastCreatedFogModifier

    <FogEnableOn()>  = FogEnable(true)
    <FogEnableOff()> = FogEnable(false)

    <FogMaskEnableOn()>  = FogMaskEnable(true)
    <FogMaskEnableOff()> = FogMaskEnable(false)

    UseTimeOfDayBJ (b) = SuspendTimeOfDay(!b)

    SetTerrainFogExBJ (i, zs, ze, d, r, g, b) = SetTerrainFogEx(i, zs, ze, d, (r)*.01, (g)*.01, (b)*.01)

    ResetTerrainFogBJ = ResetTerrainFog

    SetDoodadAnimationRectBJ (s, i, r)     = SetDoodadAnimationRect(r, i, s, false)
    AddUnitAnimationPropertiesBJ (b, s, u) = AddUnitAnimationProperties(u, s, b)

    ShowImageBJ (b, i) = ShowImage(i, b)

    SetImageColorBJ (i, r, g, b, a) = SetImageColor(i, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    <call GetLastCreatedImage()> =
    <GetLastCreatedImage()>      = bj_lastCreatedImage

    ShowUbersplatBJ (b, u) = ShowUbersplat(u, b)

    <call GetLastCreatedUbersplat()> =
    <GetLastCreatedUbersplat()>      = bj_lastCreatedUbersplat

    StopSoundBJ (s, b)                = StopSound(s, false, b)
    SetSoundVolumeBJ (s, r)           = SetSoundVolume(s, PercentToInt(r, 127))
    SetSoundOffsetBJ (r, s)           = SetSoundPlayPosition(s, R2I((r)*1000))
    SetSoundDistanceCutoffBJ (s, r)   = SetSoundDistanceCutoff(s, r)
    SetSoundPitchBJ (s, r)            = SetSoundPitch(s, r)
    AttachSoundToUnitBJ (s, u)        = AttachSoundToUnit(s, u)
    SetSoundConeAnglesBJ (s, i, o, r) = SetSoundConeAngles(s, i, o, PercentToInt(o, 127))

    KillSoundWhenDoneBJ = KillSoundWhenDone

    <call PlayMusicExBJ> (s, a, b)> = { bj_lastPlayedMusic = s ; PlayMusicEx(bj_lastPlayedMusic, R2I((a)*1000), R2I((b)*1000)) }

    SetMusicOffsetBJ (r)         = SetMusicPlayPosition(R2I((r)*1000))
    PlayThematicMusicBJ          = PlayThematicMusic
    PlayThematicMusicExBJ (s, r) = PlayThematicMusicEx(s, R2I((r)*1000))
    SetThematicMusicOffsetBJ (r) = SetThematicMusicPlayPosition(R2I((r)*1000))
    EndThematicMusicBJ           = EndThematicMusic
    StopMusicBJ                  = StopMusic
    ResumeMusicBJ                = ResumeMusic
    SetMusicVolumeBJ (r)         = SetMusicVolume(PercentToInt(r, 127))

    <call GetSoundFileDurationBJ> (s) =
    GetSoundFileDurationBJ (s)        = I2R(GetSoundFileDuration(s))*.001

    <call GetLastPlayedSound()> =
    <GetLastPlayedSound()>      = bj_lastPlayedSound
    <call GetLastPlayedMusic()> =
    <GetLastPlayedMusic()>      = bj_lastPlayedMusic

    VolumeGroupSetVolumeBJ (v, r) = VolumeGroupSetVolume(v, (r)*.01)

    VolumeGroupResetImmediateBJ = VolumeGroupReset

    WaitForSoundBJ (s, r) = TriggerWaitForSound(s, r)

    SetMapMusicIndexedBJ (s, i) = SetMapMusic(s, false, i)
    SetMapMusicRandomBJ (s)     = SetMapMusic(s, true, 0)
    ClearMapMusicBJ             = ClearMapMusic

    StartSoundForPlayerBJ (p, s)              = { if GetLocalPlayer() == p { StartSound(s) } }
    VolumeGroupSetVolumeForPlayerBJ (p, v, r) = { if GetLocalPlayer() == p { VolumeGroupSetVolume(v, r) } }

    <call EnableDawnDusk> (b) = bj_useDawnDuskSounds = b

    <call IsDawnDuskEnabled()> = 
    <IsDawnDuskEnabled()>      = bj_useDawnDuskSounds

    <call AddSpecialEffectLocBJ> (l, s)           = bj_lastCreatedEffect = AddSpecialEffectLoc(s, l)
    <call AddSpecialEffectTargetUnitBJ> (s, w, a) = bj_lastCreatedEffect = AddSpecialEffectTarget(s, w, a)

    DestroyEffectBJ = DestroyEffect

    <call GetLastCreatedEffectBJ()> =
    <GetLastCreatedEffectBJ()>      = bj_lastCreatedEffect

    GetItemLifeBJ = GetWidgetLife
    SetItemLifeBJ = SetWidgetLife

    AddHeroXPSwapped (i, u, b) = AddHeroXP(u, i, b)

    DecUnitAbilityLevelSwapped (i, u)    = DecUnitAbilityLevel(u, i)
    IncUnitAbilityLevelSwapped (i, u)    = IncUnitAbilityLevel(u, i)
    SetUnitAbilityLevelSwapped (i, u, l) = SetUnitAbilityLevel(u, i, l)
    GetUnitAbilityLevelSwapped (i, u)    = GetUnitAbilityLevel(u, i)

    <call UnitHasBuffBJ> (u, b) =
    UnitHasBuffBJ (u, b)        = GetUnitAbilityLevel(u, b) > 0

    UnitRemoveBuffBJ (b, u)   = UnitRemoveAbility(u, b)
    UnitAddItemSwapped (i, u) = UnitAddItem(u, i)

    <call UnitRemoveItemSwapped> (i, u)         = { bj_lastRemovedItem = i ; UnitRemoveItem(u, bj_lastRemovedItem) }
    <call UnitRemoveItemFromSlotSwapped> (i, u) = bj_lastRemovedItem = UnitRemoveItemFromSlot(u, i-1)

    <call GetLastCreatedItem()> =
    <GetLastCreatedItem()>      = bj_lastCreatedItem
    <call GetLastRemovedItem()> =
    <GetLastRemovedItem()>      = bj_lastRemovedItem

    GetLearnedSkillBJ = GetLearnedSkill

    SuspendHeroXPBJ (b, u) = SuspendHeroXP(u, !b)

    SetPlayerHandicapXPBJ (p, r)     = SetPlayerHandicapXP(p, (r)*.01)
    <call GetPlayerHandicapXPBJ> (p) = GetPlayerHandicapXP(p)
    GetPlayerHandicapXPBJ (p)        = GetPlayerHandicapXP(p)*100

    SetPlayerHandicapBJ (p, r)     = SetPlayerHandicap(p, (r)*.01)
    <call GetPlayerHandicapBJ> (p) = GetPlayerHandicap(p)
    GetPlayerHandicapBJ (p)        = GetPlayerHandicap(p)*100

    UnitDropItemPointBJ          = UnitDropItemPoint
    UnitDropItemSlotBJ (u, i, s) = UnitDropItemSlot(u, i, s-1)
    UnitDropItemTargetBJ         = UnitDropItemTarget
    UnitUseItemDestructable      = UnitUseItemTarget
    UnitItemInSlotBJ (u, i)      = UnitItemInSlot(u, i-1)

    <call UnitHasItemOfTypeBJ> (u, i) = GetInventoryIndexOfItemTypeBJ(u, i)
    UnitHasItemOfTypeBJ (u, i)        = GetInventoryIndexOfItemTypeBJ(u, i) > 0

    UnitInventorySizeBJ   = UnitInventorySize
    SetItemInvulnerableBJ = SetItemInvulnerable
    SetItemDropOnDeathBJ  = SetItemDropOnDeath
    SetItemDroppableBJ    = SetItemDroppable
    SetItemPlayerBJ       = SetItemPlayer

    SetItemVisibleBJ (b, i) = SetItemVisible(i, b)

    <call IsItemHiddenBJ> (i) = IsItemVisible(i)
    IsItemHiddenBJ (i)        = !IsItemVisible(i)

    ChooseRandomItemBJ = ChooseRandomItem

    ChooseRandomItemExBJ (i, t) = ChooseRandomItemEx(t, i)

    ChooseRandomNPBuildingBJ = ChooseRandomNPBuilding
    ChooseRandomCreepBJ      = ChooseRandomCreep

    EnumItemsInRectBJ (r, c)     = EnumItemsInRect(r, null, c)
    RandomItemInRectSimpleBJ (r) = RandomItemInRectBJ(r, null)

    String2UnitIdBJ    = UnitId
    GetIssuedOrderIdBJ = GetIssuedOrderId

    GetKillingUnitBJ = GetKillingUnit

    <call GetLastCreatedUnit()> =
    <GetLastCreatedUnit()>      = bj_lastCreatedUnit

    CreateNUnitsAtLocFacingLocBJ (i, u, p, l, lx) = CreateNUnitsAtLoc(i, u, p, l, AngleBetweenPoints(l, lx))

    <GetLastCreatedGroupEnum()> = GroupAddUnit(bj_groupLastCreatedDest, GetEnumUnit())

    UnitSuspendDecayBJ (b, u) = UnitSuspendDecay(u, b)
    GetUnitStateSwap (a, b)   = GetUnitState (b, a)

    GetUnitLifePercent (u) = GetUnitStatePercent(u, UNIT_STATE_LIFE, UNIT_STATE_MAX_LIFE)
    GetUnitManaPercent (u) = GetUnitStatePercent(u, UNIT_STATE_MANA, UNIT_STATE_MAX_MANA)

    <call SelectUnitSingle> (u) = { ClearSelection() ; SelectUnit(u, true) }

    <call SelectGroupBJEnum()> = SelectUnit(GetEnumUnit(), true)

    <call SelectGroupBJ> (g) = { ClearSelection() ; ForGroup(g, function SelectGroupBJEnum) }

    SelectUnitAdd (u)    = SelectUnit(u, true)
    SelectUnitRemove (u) = SelectUnit(u, false)

    <call ClearSelectionForPlayer> (p)      = { if GetLocalPlayer() == p { ClearSelection() } }
    <call SelectUnitAddForPlayer> (u, p)    = { if GetLocalPlayer() == p { SelectUnit(u, true) } }
    <call SelectUnitRemoveForPlayer> (u, p) = { if GetLocalPlayer() == p { SelectUnit(u, false) } }

    SetUnitLifeBJ (u, r) = SetUnitState(u, UNIT_STATE_LIFE, RMaxBJ(0, r))
    SetUnitManaBJ (u, r) = SetUnitState(u, UNIT_STATE_MANA, RMaxBJ(0, r))

    <call IsUnitDeadBJ> (u)  = GetUnitState(u, UNIT_STATE_LIFE)
    IsUnitDeadBJ (u)         = GetUnitState(u, UNIT_STATE_LIFE) <= 0.
    <call IsUnitAliveBJ> (u) = GetUnitState(u, UNIT_STATE_LIFE)
    IsUnitAliveBJ (u)        = GetUnitState(u, UNIT_STATE_LIFE) > 0.

    <call IsUnitGroupEmptyBJEnum()> = bj_isUnitGroupEmptyResult = false

    <call IsUnitGroupInRectBJEnum()> = { if !RectContainsUnit(bj_isUnitGroupInRectRect, GetEnumUnit()) { bj_isUnitGroupInRectResult = false } }

    IsUnitHiddenBJ   = IsUnitHidden
    ShowUnitHide (u) = ShowUnit(u, false)

    <call IssueHauntOrderAtLocBJFilter()> = GetUnitTypeId(GetFilterUnit())
    <IssueHauntOrderAtLocBJFilter()>      = GetUnitTypeId(GetFilterUnit()) == 'ngol'

    GroupTrainOrderByIdBJ   = GroupImmediateOrderById
    IssueUpgradeOrderByIdBJ = IssueImmediateOrderById

    GetAttackedUnitBJ = GetTriggerUnit

    SetUnitFlyHeightBJ = SetUnitFlyHeight
    SetUnitTurnSpeedBJ = SetUnitTurnSpeed

    <call GetUnitPropWindowBJ> (u) = GetUnitPropWindow(u)
    GetUnitPropWindowBJ (u)        = (GetUnitPropWindow(u) * bj_RADTODEG)

    GetUnitDefaultPropWindowBJ = GetUnitDefaultPropWindow
    SetUnitBlendTimeBJ         = SetUnitBlendTime
    SetUnitAcquireRangeBJ      = SetUnitAcquireRange
    UnitSetCanSleepBJ          = UnitSetCanSleep
    UnitCanSleepBJ             = UnitCanSleep
    UnitWakeUpBJ               = UnitWakeUp
    UnitIsSleepingBJ           = UnitIsSleeping

    <call WakePlayerUnitsEnum()> = UnitWakeUp(GetEnumUnit())

    UnitGenerateAlarms (u, b) = UnitIgnoreAlarm(u, !b)

    DoesUnitGenerateAlarms (u) = !UnitIgnoreAlarmToggled(u)

    <call PauseAllUnitsBJEnum()> = PauseUnit(GetEnumUnit(), bj_pauseAllUnitsFlag)

    PauseUnitBJ (b, u) = PauseUnit(u, b)
    IsUnitPausedBJ     = IsUnitPaused

    UnitPauseTimedLifeBJ (b, u) = UnitPauseTimedLife(u, b)

    UnitApplyTimedLifeBJ (r, i, u) = UnitApplyTimedLife(u, i, r)

    UnitShareVisionBJ (b, u, p) = UnitShareVision(u, p, b)

    UnitRemoveAbilityBJ (i, u) = UnitRemoveAbility(u, i)
    UnitAddAbilityBJ (i, u)    = UnitAddAbility(u, i)

    UnitRemoveTypeBJ (a, b) = UnitRemoveType(b, a)
    UnitAddTypeBJ (a, b)    = UnitAddType(b, a)

    UnitMakeAbilityPermanentBJ (b, i, u) = UnitMakeAbilityPermanent(u, b, i)

    SetUnitExplodedBJ = SetUnitExploded

    GetTransportUnitBJ = GetTransportUnit
    GetLoadedUnitBJ    = GetLoadedUnit

    IsUnitInTransportBJ = IsUnitInTransport
    IsUnitLoadedBJ      = IsUnitLoaded
    IsUnitIllusionBJ    = IsUnitIllusion

    <call GetLastReplacedUnitBJ()> =
    <GetLastReplacedUnitBJ()>      = bj_lastReplacedUnit

    AddItemToStockBJ (a, b, c, d) = AddItemToStock(b, a, c, d)
    AddUnitToStockBJ (a, b, c, d) = AddUnitToStock(b, a, c, d)

    RemoveItemFromStockBJ (i, u) = RemoveItemFromStock(u, i)
    RemoveUnitFromStockBJ (a, b) = RemoveUnitFromStock(b, a)

    SetUnitUseFoodBJ (b, u) = SetUnitUseFood(u, b)

    UnitDamageTargetBJ (u, t, r, at, dt) = UnitDamageTarget(u, t, r, true, false, at, dt, WEAPON_TYPE_WHOKNOWS)

    <call GetLastCreatedDestructable()> =
    <GetLastCreatedDestructable()>      = bj_lastCreatedDestructable

    ShowDestructableBJ (b, d) = ShowDestructable(d, b)

    SetDestructableInvulnerableBJ = SetDestructableInvulnerable
    IsDestructableInvulnerableBJ  = IsDestructableInvulnerable

    EnumDestructablesInRectAll (r, c) = EnumDestructablesInRect(r, null, c)

    <call IsDestructableDeadBJ> (d) = GetDestructableLife(d)
    IsDestructableDeadBJ (d)        = GetDestructableLife(d) <= 0.

    <call IsDestructableAliveBJ> (d) = GetDestructableLife(d)
    IsDestructableAliveBJ (d)        = GetDestructableLife(d) > 0.

    RandomDestructableInRectSimpleBJ (r) = RandomDestructableInRectBJ(r, null)

    SetDestructableMaxLifeBJ = SetDestructableMaxLife

    <call FindElevatorWallBlockerEnum()> = bj_elevatorWallBlocker = GetEnumDestructable()

    WaygateActivateBJ (b, u) = WaygateActivate(u, b)

    WaygateIsActiveBJ = WaygateIsActive

    UnitSetUsesAltIconBJ (b, u) = UnitSetUsesAltIcon(u, b)

    <call ForceUIKeyBJ> (p, s) = { if GetLocalPlayer() == p { ForceUIKey(s) } }
    <call ForceUICancelBJ> (p) = { if GetLocalPlayer() == p { ForceUICancel() } }

    GroupAddUnitSimple (u, g)    = GroupAddUnit(g, u)
    GroupRemoveUnitSimple (u, g) = GroupRemoveUnit(g, u)

    <call GroupAddGroupEnum()>    = GroupAddUnit(bj_groupAddGroupDest, GetEnumUnit())
    <call GroupRemoveGroupEnum()> = GroupRemoveUnit(bj_groupRemoveGroupDest, GetEnumUnit())

    ForceAddPlayerSimple (p, f)    = ForceAddPlayer(f, p)
    ForceRemovePlayerSimple (p, f) = ForceRemovePlayer(f, p)

    GetUnitsInRectAll (r) = GetUnitsInRectMatching(r, null)

    <call GetUnitsInRectOfPlayerFilter()> = GetOwningPlayer(GetFilterUnit())
    <GetUnitsInRectOfPlayerFilter()>      = GetOwningPlayer(GetFilterUnit()) == bj_groupEnumOwningPlayer

    GetUnitsInRangeOfLocAll (r, l) = GetUnitsInRangeOfLocMatching(r, l, null)

    <call GetUnitsOfTypeIdAllFilter()> = GetUnitTypeId(GetFilterUnit())
    <GetUnitsOfTypeIdAllFilter()>      = GetUnitTypeId(GetFilterUnit()) == bj_groupEnumTypeId

    GetUnitsOfPlayerAll (p) = GetUnitsOfPlayerMatching(p, null)

    <call GetUnitsOfPlayerAndTypeIdFilter()> = GetUnitTypeId(GetFilterUnit())
    <GetUnitsOfPlayerAndTypeIdFilter()>      = GetUnitTypeId(GetFilterUnit()) == bj_groupEnumTypeId

    <call GetPlayersAll()> =
    <GetPlayersAll()>      = bj_FORCE_ALL_PLAYERS

    <call CountUnitsInGroupEnum()>   = bj_groupCountUnits ++
    <call CountPlayersInForceEnum()> = bj_forceCountPlayers ++

    ResetUnitAnimation (u) = SetUnitAnimation(u, "stand")

    SetUnitTimeScalePercent (u, r)   = SetUnitTimeScale(u, (r)*.01)
    SetUnitScalePercent (u, x, y, z) = SetUnitScale(u, (x)*.01, (y)*.01, (z)*.01)

    SetUnitVertexColorBJ (u, r, g, b, a)       = SetUnitVertexColor(u, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))
    UnitAddIndicatorBJ (u, r, g, b, a)         = AddIndicator(u, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))
    DestructableAddIndicatorBJ (d, r, g, b, a) = AddIndicator(d, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))
    ItemAddIndicatorBJ (i, r, g, b, a)         = AddIndicator(d, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    QueueUnitAnimationBJ         = QueueUnitAnimation
    SetDestructableAnimationBJ   = SetDestructableAnimation
    QueueDestructableAnimationBJ = QueueDestructableAnimation

    SetDestAnimationSpeedPercent (d, r) = SetDestructableAnimationSpeed(d, (r)*.01)

    DialogDisplayBJ (b, d, p) = DialogDisplay(p, d, b)

    DialogSetMessageBJ = DialogSetMessage
    DialogClearBJ      = DialogClear
    GetClickedButtonBJ = GetClickedButton
    GetClickedDialogBJ = GetClickedDialog

    <call DialogAddButtonBJ> (d, s)              = bj_lastCreatedButton = DialogAddButton(d, s, 0)
    <call DialogAddButtonWithHotkeyBJ> (d, s, i) = bj_lastCreatedButton = DialogAddButton(d, s, i)
    DialogAddButtonBJ (d, s)                     = bj_lastCreatedButton = DialogAddButton(d, s, 0)
    DialogAddButtonWithHotkeyBJ (d, s, i)        = bj_lastCreatedButton = DialogAddButton(d, s, i)

    <call GetLastCreatedButtonBJ()> =
    <GetLastCreatedButtonBJ()>      = bj_lastCreatedButton

    SetPlayerAllianceStateVisionBJ (s, d, b)      = SetPlayerAlliance(s, d, ALLIANCE_SHARED_VISION, b)
    SetPlayerAllianceStateControlBJ (s, d, b)     = SetPlayerAlliance(s, d, ALLIANCE_SHARED_CONTROL, b)
    SetPlayerAllianceStateFullControlBJ (s, d, b) = SetPlayerAlliance(s, d, ALLIANCE_SHARED_ADVANCED_CONTROL, b)

    <MakeUnitsPassiveForPlayerEnum()> = SetUnitOwner(GetEnumUnit(), Player(bj_PLAYER_NEUTRAL_VICTIM), false)

    EndGameBJ = EndGame

    <call CustomDefeatRestartBJ()> = { PauseGame(false) ; RestartGame(true) }
    <call CustomDefeatLoadBJ ()>   = { PauseGame(false) ; DisplayLoadDialog() }

    SetPlayerOnScoreScreenBJ (b, p) = SetPlayerOnScoreScreen(p, b)

    DestroyQuestBJ = DestroyQuest

    QuestSetTitleBJ           = QuestSetTitle
    QuestSetDescriptionBJ     = QuestSetDescription
    QuestSetCompletedBJ       = QuestSetCompleted
    QuestSetFailedBJ          = QuestSetFailed
    QuestSetDiscoveredBJ      = QuestSetDiscovered
    QuestItemSetDescriptionBJ = QuestItemSetDescription
    QuestItemSetCompletedBJ   = QuestItemSetCompleted

    QuestSetEnabledBJ (b, q) = QuestSetEnabled(q, b)

    <call GetLastCreatedQuestBJ()>     =
    <GetLastCreatedQuestBJ()>          = bj_lastCreatedQuest
    <call GetLastCreatedQuestItemBJ()> =
    <GetLastCreatedQuestItemBJ()>      = bj_lastCreatedQuestItem

    DestroyDefeatConditionBJ        = DestroyDefeatCondition
    DefeatConditionSetDescriptionBJ = DefeatConditionSetDescription

    <call GetLastCreatedDefeatConditionBJ()> =
    <GetLastCreatedDefeatConditionBJ()>      = bj_lastCreatedDefeatCondition

    FlashQuestDialogButtonBJ = FlashQuestDialogButton

    DestroyTimerBJ = DestroyTimer

    <call GetLastCreatedTimerBJ()> = 
    <GetLastCreatedTimerBJ()>      = bj_lastStartedTimer

    DestroyTimerDialogBJ  = DestroyTimerDialog
    TimerDialogSetTitleBJ = TimerDialogSetTitle

    TimerDialogSetTitleColorBJ (t, r, g, b, a) = TimerDialogSetTitleColor(t, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))
    TimerDialogSetTimeColorBJ (t, r, g, b, a)  = TimerDialogSetTimeColor(t, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    TimerDialogSetSpeedBJ = TimerDialogSetSpeed

    <call TimerDialogDisplayForPlayerBJ> (b, t, p) = { if GetLocalPlayer() == p { TimerDialogDisplay(t, b) } }

    TimerDialogDisplayBJ (b, y) = TimerDialogDisplay(t, b)

    <call GetLastCreatedTimerDialogBJ()> =
    <GetLastCreatedTimerDialogBJ()>      = bj_lastCreatedTimerDialog

    LeaderboardSetLabelColorBJ (l, r, g, b, a) = LeaderboardSetLabelColor(l, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))
    LeaderboardSetValueColorBJ (l, r, g, b, a) = LeaderboardSetValueColor(l, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    LeaderboardSetStyleBJ          = LeaderboardSetStyle
    LeaderboardGetItemCountBJ      = LeaderboardGetItemCount
    LeaderboardHasPlayerItemBJ     = LeaderboardHasPlayerItem
    DestroyLeaderboardBJ           = DestroyLeaderboard
    LeaderboardSortItemsByPlayerBJ = LeaderboardSortItemsByPlayer
    LeaderboardSortItemsByLabelBJ  = LeaderboardSortItemsByLabel
    PlayerGetLeaderboardBJ         = PlayerGetLeaderboard

    LeaderboardDisplayBJ (b, l) = LeaderboardDisplay(l, b)

    <call LeaderboardGetPlayerIndexBJ> (p, l) =
    LeaderboardGetPlayerIndexBJ (p, l)        = (LeaderboardGetPlayerIndex(l, p) + 1)

    <call GetLastCreatedLeaderboard()> =
    <GetLastCreatedLeaderboard()>      = bj_lastCreatedLeaderboard

    DestroyMultiboardBJ = DestroyMultiboard

    <call GetLastCreatedMultiboard()> =
    <GetLastCreatedMultiboard()>      = bj_lastCreatedMultiboard

    MultiboardDisplayBJ (b, m)  = MultiboardDisplay(m, b)
    MultiboardMinimizeBJ (b, m) = MultiboardMinimize(m, b)

    MultiboardSetTitleTextColorBJ (m, r, g, b, a) = MultiboardSetTitleTextColor(m, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    MultiboardAllowDisplayBJ (b) = MultiboardSuppressDisplay (!(b))

    <call TextTagSize2Height> (r) = Text ## TagSize2Height (r)
    TextTagSize2Height (r)        = ((r)*.0023)

    <call TextTagSpeed2Velocity> (r) = Text ## TagSpeed2Velocity (r)
    TextTagSpeed2Velocity (r)        = ((r)*.0005546875)

    SetTextTagColorBJ (t, r, g, b, a) = SetTextTagColor(tt, PercentTo255(r), PercentTo255(g), PercentTo255(b), PercentTo255(100.-(a)))

    SetTextTagTextBJ (t, s, r) = SetTextTagText(t, s, TextTagSize2Height(r))

    SetTextTagPosUnitBJ    = SetTextTagPosUnit
    SetTextTagSuspendedBJ  = SetTextTagSuspended
    SetTextTagPermanentBJ  = SetTextTagPermanent
    SetTextTagAgeBJ        = SetTextTagAge
    SetTextTagLifespanBJ   = SetTextTagLifespan
    SetTextTagFadepointBJ  = SetTextTagFadepoint
    DestroyTextTagBJ       = DestroyTextTag

    <call ShowTextTagForceBJ> (b, t, f) = { if IsPlayerInForce(GetLocalPlayer(), f) { SetTextTagVisibility(t, b) } }

    <call GetLastCreatedTextTag()> =
    <GetLastCreatedTextTag()>      = bj_lastCreatedTextTag

    <call PauseGameOn()>  = PauseGame(true)
    <call PauseGameOff()> = PauseGame(false)

    <call SetUserControlForceOn> (f)    = { if IsPlayerInForce(GetLocalPlayer(), f) { EnableUserControl(true) } }
    <call SetUserControlForceOff> (f)   = { if IsPlayerInForce(GetLocalPlayer(), f) { EnableUserControl(false) } }
    <call ShowInterfaceForceOn> (f, r)  = { if IsPlayerInForce(GetLocalPlayer(), f) { ShowInterface(true, r) } }
    <call ShowInterfaceForceOff> (f, r) = { if IsPlayerInForce(GetLocalPlayer(), f) { ShowInterface(false, r) } }

    <call PingMinimapForForce> (f, x, y, r)  = { if IsPlayerInForce(GetLocalPlayer(), f) { PingMinimap(x, y, r) } }
    <call PingMinimapForPlayer> (p, x, y, r) = { if GetLocalPlayer() == p { PingMinimap(x, y, r) } }

    <call EnableWorldFogBoundaryBJ> (b, f) = { if IsPlayerInForce(GetLocalPlayer(), f) { EnableWorldFogBoundary(b) } }
    <call EnableOcclusionBJ> (b, f)        = { if IsPlayerInForce(GetLocalPlayer(), f) { EnableOcclusion(b) } }

    <call CancelCineSceneBJ()> = { StopSoundBJ(bj_cineSceneLastSound, true) ; EndCinematicScene() }

    <call GetLastTransmissionDurationBJ()> =
    <GetLastTransmissionDurationBJ()>      = bj_lastTransmissionDuration

    ForceCinematicSubtitlesBJ = ForceCinematicSubtitles

    CinematicModeBJ (b, f) = CinematicModeExBJ(b, f, bj_CINEMODE_INTERFACEFADE)

    DisplayCineFilterBJ = DisplayCineFilter

    <call FinishCinematicFadeAfterBJ> (r) = { bj_cineFadeFinishTimer = CreateTimer() ; TimerStart(bj_cineFadeFinishTimer, r, false, function FinishCinematicFadeBJ) }

    <call SetRescueUnitColorChangeBJ> (b)     = bj_rescueChangeColorUnit = b
    <call SetRescueBuildingColorChangeBJ> (b) = bj_rescueChangeColorBldg = b

    <call MakeUnitRescuableToForceBJEnum()> = { TryInitRescuableTriggersBJ() ; SetUnitRescuable(bj_makeUnitRescuableUnit, GetEnumPlayer(), bj_makeUnitRescuableFlag) }

    SetPlayerTechResearchedSwap (i, l, p) = SetPlayerTechResearched(p, i, l)
    SetPlayerTechMaxAllowedSwap (i, m, p) = SetPlayerTechMaxAllowed(p, i, m)

    SetPlayerMaxHeroesAllowed (m, p) = SetPlayerTechMaxAllowed(p, 'HERO', m)

    GetPlayerTechCountSimple (i, p)    = GetPlayerTechCount(p, i, true)
    GetPlayerTechMaxAllowedSwap (i, p) = GetPlayerTechMaxAllowed(p, i)

    SetPlayerAbilityAvailableBJ (b, i, p) = SetPlayerAbilityAvailable(p, i, b)

    <call InitGameCacheBJ> (s) = bj_lastCreatedGameCache = InitGameCache(s)

    SaveGameCacheBJ = SaveGameCache

    <call GetLastCreatedGameCacheBJ()> =
    <GetLastCreatedGameCacheBJ()>      = bj_lastCreatedGameCache

    <call InitHashtableBJ()> = bj_lastCreatedHashtable = InitHashtable()

    <call GetLastCreatedHashtableBJ()> =
    <GetLastCreatedHashtableBJ()>      = bj_lastCreatedHashtable

    StoreRealBJ (v, a, b, c)    = StoreReal(c, b, a, v)
    StoreIntegerBJ (v, a, b, c) = StoreInteger(c, b, a, v)
    StoreBooleanBJ (v, a, b, c) = StoreBoolean(c, b, a, v)
    StoreStringBJ (v, a, b, c)  = StoreString(c, b, a, v)
    StoreUnitBJ (v, a, b, c)    = StoreUnit(c, b, a, v)

    SaveRealBJ (v, a, b, h)                   = SaveReal(h, b, a, v)
    SaveIntegerBJ (v, a, b, h)                = SaveInteger(h, b, a, v)
    SaveBooleanBJ (v, a, b, h)                = SaveBoolean(h, b, a, v)
    SaveStringBJ (v, a, b, h)                 = SaveStr(h, b, a, v)
    SavePlayerHandleBJ (v, a, b, h)           = SavePlayerHandle(h, b, a, v)
    SaveWidgetHandleBJ (v, a, b, h)           = SaveWidgetHandle(h, b, a, v)
    SaveDestructableHandleBJ (v, a, b, h)     = SaveDestructableHandle(h, b, a, v)
    SaveItemHandleBJ (v, a, b, h)             = SaveItemHandle(h, b, a, v)
    SaveUnitHandleBJ (v, a, b, h)             = SaveUnitHandle(h, b, a, v)
    SaveAbilityHandleBJ (v, a, b, h)          = SaveAbilityHandle(h, b, a, v)
    SaveTimerHandleBJ (v, a, b, h)            = SaveTimerHandle(h, b, a, v)
    SaveTriggerHandleBJ (v, a, b, h)          = SaveTriggerHandle(h, b, a, v)
    SaveTriggerConditionHandleBJ (v, a, b, h) = SaveTriggerConditionHandle(h, b, a, v)
    SaveTriggerActionHandleBJ (v, a, b, h)    = SaveTriggerActionHandle(h, b, a, v)
    SaveTriggerEventHandleBJ (v, a, b, h)     = SaveTriggerEventHandle(h, b, a, v)
    SaveForceHandleBJ (v, a, b, h)            = SaveForceHandle(h, b, a, v)
    SaveGroupHandleBJ (v, a, b, h)            = SaveGroupHandle(h, b, a, v)
    SaveLocationHandleBJ (v, a, b, h)         = SaveLocationHandle(h, b, a, v)
    SaveRectHandleBJ (v, a, b, h)             = SaveRectHandle(h, b, a, v)
    SaveBooleanExprHandleBJ (v, a, b, h)      = SaveBooleanExprHandle(h, b, a, v)
    SaveSoundHandleBJ (v, a, b, h)            = SaveSoundHandle(h, b, a, v)
    SaveEffectHandleBJ (v, a, b, h)           = SaveEffectHandle(h, b, a, v)
    SaveUnitPoolHandleBJ (v, a, b, h)         = SaveUnitPoolHandle(h, b, a, v)
    SaveItemPoolHandleBJ (v, a, b, h)         = SaveItemPoolHandle(h, b, a, v)
    SaveQuestHandleBJ (v, a, b, h)            = SaveQuestHandle(h, b, a, v)
    SaveQuestItemHandleBJ (v, a, b, h)        = SaveQuestItemHandle(h, b, a, v)
    SaveDefeatConditionHandleBJ (v, a, b, h)  = SaveDefeatConditionHandle(h, b, a, v)
    SaveTimerDialogHandleBJ (v, a, b, h)      = SaveTimerDialogHandle(h, b, a, v)
    SaveLeaderboardHandleBJ (v, a, b, h)      = SaveLeaderboardHandle(h, b, a, v)
    SaveMultiboardHandleBJ (v, a, b, h)       = SaveMultiboardHandle(h, b, a, v)
    SaveMultiboardItemHandleBJ (v, a, b, h)   = SaveMultiboardItemHandle(h, b, a, v)
    SaveTrackableHandleBJ (v, a, b, h)        = SaveTrackableHandle(h, b, a, v)
    SaveDialogHandleBJ (v, a, b, h)           = SaveDialogHandle(h, b, a, v)
    SaveButtonHandleBJ (v, a, b, h)           = SaveButtonHandle(h, b, a, v)
    SaveTextTagHandleBJ (v, a, b, h)          = SaveTextTagHandle(h, b, a, v)
    SaveLightningHandleBJ (v, a, b, h)        = SaveLightningHandle(h, b, a, v)
    SaveImageHandleBJ (v, a, b, h)            = SaveImageHandle(h, b, a, v)
    SaveUbersplatHandleBJ (v, a, b, h)        = SaveUbersplatHandle(h, b, a, v)
    SaveRegionHandleBJ (v, a, b, h)           = SaveRegionHandle(h, b, a, v)
    SaveFogStateHandleBJ (v, a, b, h)         = SaveFogStateHandle(h, b, a, v)
    SaveFogModifierHandleBJ (v, a, b, h)      = SaveFogModifierHandle(h, b, a, v)
    SaveAgentHandleBJ (v, a, b, h)            = SaveAgentHandle(h, b, a, v)
    SaveHashtableHandleBJ (v, a, b, h)        = SaveHashtableHandle(h, b, a, v)

    GetStoredRealBJ (a, b, c)    = GetStoredReal(c, b, a)
    GetStoredIntegerBJ (a, b, c) = GetStoredInteger(c, b, a)
    GetStoredBooleanBJ (a, b, c) = GetStoredBoolean(c, b, a)

    LoadRealBJ (a, b, h)                   = LoadReal(h, b, a)
    LoadIntegerBJ (a, b, h)                = LoadInteger(h, b, a)
    LoadBooleanBJ (a, b, h)                = LoadBoolean(h, b, a)
    LoadPlayerHandleBJ (a, b, h)           = LoadPlayerHandle(h, b, a)
    LoadWidgetHandleBJ (a, b, h)           = LoadWidgetHandle(h, b, a)
    LoadDestructableHandleBJ (a, b, h)     = LoadDestructableHandle(h, b, a)
    LoadItemHandleBJ (a, b, h)             = LoadItemHandle(h, b, a)
    LoadUnitHandleBJ (a, b, h)             = LoadUnitHandle(h, b, a)
    LoadAbilityHandleBJ (a, b, h)          = LoadAbilityHandle(h, b, a)
    LoadTimerHandleBJ (a, b, h)            = LoadTimerHandle(h, b, a)
    LoadTriggerHandleBJ (a, b, h)          = LoadTriggerHandle(h, b, a)
    LoadTriggerConditionHandleBJ (a, b, h) = LoadTriggerConditionHandle(h, b, a)
    LoadTriggerActionHandleBJ (a, b, h)    = LoadTriggerActionHandle(h, b, a)
    LoadTriggerEventHandleBJ (a, b, h)     = LoadTriggerEventHandle(h, b, a)
    LoadForceHandleBJ (a, b, h)            = LoadForceHandle(h, b, a)
    LoadGroupHandleBJ (a, b, h)            = LoadGroupHandle(h, b, a)
    LoadLocationHandleBJ (a, b, h)         = LoadLocationHandle(h, b, a)
    LoadRectHandleBJ (a, b, h)             = LoadRectHandle(h, b, a)
    LoadBooleanExprHandleBJ (a, b, h)      = LoadBooleanExprHandle(h, b, a)
    LoadSoundHandleBJ (a, b, h)            = LoadSoundHandle(h, b, a)
    LoadEffectHandleBJ (a, b, h)           = LoadEffectHandle(h, b, a)
    LoadUnitPoolHandleBJ (a, b, h)         = LoadUnitPoolHandle(h, b, a)
    LoadItemPoolHandleBJ (a, b, h)         = LoadItemPoolHandle(h, b, a)
    LoadQuestHandleBJ (a, b, h)            = LoadQuestHandle(h, b, a)
    LoadQuestItemHandleBJ (a, b, h)        = LoadQuestItemHandle(h, b, a)
    LoadDefeatConditionHandleBJ (a, b, h)  = LoadDefeatConditionHandle(h, b, a)
    LoadTimerDialogHandleBJ (a, b, h)      = LoadTimerDialogHandle(h, b, a)
    LoadLeaderboardHandleBJ (a, b, h)      = LoadLeaderboardHandle(h, b, a)
    LoadMultiboardHandleBJ (a, b, h)       = LoadMultiboardHandle(h, b, a)
    LoadMultiboardItemHandleBJ (a, b, h)   = LoadMultiboardItemHandle(h, b, a)
    LoadTrackableHandleBJ (a, b, h)        = LoadTrackableHandle(h, b, a)
    LoadDialogHandleBJ (a, b, h)           = LoadDialogHandle(h, b, a)
    LoadButtonHandleBJ (a, b, h)           = LoadButtonHandle(h, b, a)
    LoadTextTagHandleBJ (a, b, h)          = LoadTextTagHandle(h, b, a)
    LoadLightningHandleBJ (a, b, h)        = LoadLightningHandle(h, b, a)
    LoadImageHandleBJ (a, b, h)            = LoadImageHandle(h, b, a)
    LoadUbersplatHandleBJ (a, b, h)        = LoadUbersplatHandle(h, b, a)
    LoadRegionHandleBJ (a, b, h)           = LoadRegionHandle(h, b, a)
    LoadFogStateHandleBJ (a, b, h)         = LoadFogStateHandle(h, b, a)
    LoadFogModifierHandleBJ (a, b, h)      = LoadFogModifierHandle(h, b, a)
    LoadHashtableHandleBJ (a, b, h)        = LoadHashtableHandle(h, b, a)

    <call GetLastRestoredUnitBJ()> = GetLastRestoredUnitBJ
    <GetLastRestoredUnitBJ()>      = bj_lastLoadedUnit

    FlushGameCacheBJ = FlushGameCache

    FlushStoredMissionBJ (x, c) = FlushStoredMission(c, x)

    FlushParentHashtableBJ = FlushParentHashtable

    FlushChildHashtableBJ (x, h) = FlushChildHashtable(h, x)

    ShowCustomCampaignButton (b, i) = SetCustomCampaignButtonVisible((i)-1, b)

    IsCustomCampaignButtonVisibile (i) =  GetCustomCampaignButtonVisible((i)-1)

    LoadGameBJ = LoadGame

    <call SaveAndChangeLevelBJ> (s, l, b) = { SaveGame(s) ; ChangeLevel(l, b) }
    <call SaveAndLoadGameBJ> (s, l, b)    = { SaveGame(s) ; LoadGame(l, b) }

    RenameSaveDirectoryBJ = RenameSaveDirectory
    RemoveSaveDirectoryBJ = RemoveSaveDirectory
    CopySaveGameBJ        = CopySaveGame

    GetPlayerStartLocationX (p)   = GetStartLocationX(GetPlayerStartLocation(p))
    GetPlayerStartLocationY (p)   = GetStartLocationY(GetPlayerStartLocation(p))
    GetPlayerStartLocationLoc (p) = GetStartLocationLoc(GetPlayerStartLocation(p))

    <call IsPlayerSlotState> (p, x) =
    IsPlayerSlotState (p, x)        = GetPlayerSlotState(p) == x

    SetPlayerFlagBJ (x, b, p) = SetPlayerState(p, x, IntegerTertiaryOp(b, 1, 0))

    SetPlayerTaxRateBJ (i, x, a, b) = SetPlayerTaxRate(a, b, x, i)
    GetPlayerTaxRateBJ (x, a, b)    = GetPlayerTaxRate(a, b, x)

    <call IsPlayerFlagSetBJ> (x, p) =
    IsPlayerFlagSetBJ (x, p)        = GetPlayerState(p, x) == 1

    AddResourceAmountBJ (i, u) = AddResourceAmount(u, i)

    <call GetConvertedPlayerId> (p) = Get ## ConvertedPlayerId (p)
    <call ConvertedPlayer> (i)      = Converted ## Player (i)
    GetConvertedPlayerId (p)        = (GetPlayerId(p)+1)
    ConvertedPlayer (i)             = (Player(i-1))

    <call BlightGoldMineForPlayer> (u, p) = bj_lastHauntedGoldMine = BlightGoldMineForPlayerBJ(u, p)

    <call GetLastHauntedGoldMine()> =
    <GetLastHauntedGoldMine()>      = bj_lastHauntedGoldMine

    <SetPlayerColorBJEnum()> = SetUnitColor(GetEnumUnit(), bj_setPlayerTargetColor)

    <LockGameSpeedBJ()>   = SetMapFlag(MAP_LOCK_SPEED, true)
    <UnlockGameSpeedBJ()> = SetMapFlag(MAP_LOCK_SPEED, false)

    IssueTargetOrderBJ           = IssueTargetOrder
    IssuePointOrderLocBJ         = IssuePointOrderLoc
    IssueTargetDestructableOrder = IssueTargetOrder
    IssueTargetItemOrder         = IssueTargetOrder
    IssueImmediateOrderBJ        = IssueImmediateOrder

    GroupTargetOrderBJ           = GroupTargetOrder
    GroupPointOrderLocBJ         = GroupPointOrderLoc
    GroupImmediateOrderBJ        = GroupImmediateOrder
    GroupTargetDestructableOrder = GroupTargetOrder
    GroupTargetItemOrder         = GroupTargetOrder

    GetDyingDestructable = GetTriggerDestructable

    SetUnitRallyPoint (u, l)        = IssuePointOrderLocBJ(u, "setrally", l)
    SetUnitRallyUnit (u, t)         = IssueTargetOrder(u, "setrally", t)
    SetUnitRallyDestructable (u, d) = IssueTargetOrder(u, "setrally", d)

    <call SaveDyingWidget()> = bj_lastDyingWidget = GetTriggerWidget()

    SetBlightRectBJ (b, p, r)         = SetBlightRect(p, r, b)
    SetBlightRadiusLocBJ (b, p, r, l) = SetBlightLoc(p, l, r, b)

    GetAbilityName = GetObjectName

    <MeleeStartingVisibility()> = SetFloatGameState(GAME_STATE_TIME_OF_DAY, bj_MELEE_STARTING_TOD)

    <MeleeTrainedUnitIsHeroBJFilter()> = IsUnitType(GetFilterUnit(), UNIT_TYPE_HERO)

    <MeleeGrantItemsToTrainedHero()>            = MeleeGrantItemsToHero(GetTrainedUnit())
    <MeleeGrantItemsToHiredHero()>              = MeleeGrantItemsToHero(GetSoldUnit())
    <MeleeTriggerActionConstructCancel()>       = MeleeCheckLostUnit(GetCancelledStructure())
    <MeleeTriggerActionUnitConstructionStart()> = MeleeCheckAddedUnit(GetConstructingStructure())
    <RemovePurchasedItem()>                     = RemoveItemFromStock(GetSellingUnit(), GetItemTypeId(GetSoldItem()))

    LockGuardPosition (u) = SetUnitCreepGuard(u, true)

    MeleeGetCrippledRevealedMessage (p) = GetLocalizedString("CRIPPLE_REVEALING_PREFIX")+GetPlayerName(p)+GetLocalizedString("CRIPPLE_REVEALING_POSTFIX")

    <call MeleeTriggerActionAllianceChange()> = { MeleeCheckForLosersAndVictors() ; MeleeCheckForCrippledPlayers() }

    <MeleeInitPlayerSlots()> = TeamInitPlayerSlots(bj_MAX_PLAYERS)
    <FFAInitPlayerSlots()>   = TeamInitPlayerSlots(bj_MAX_PLAYERS)

    <call SetDNCSoundsDawn()>     = { if bj_useDawnDuskSounds { StartSound(bj_dawnSound) } }
    <call bj_useDawnDuskSounds()> = { if bj_useDawnDuskSounds { StartSound(bj_duskSound) } }

    <call RandomDistReset()> = bj_randDistCount = 0

}