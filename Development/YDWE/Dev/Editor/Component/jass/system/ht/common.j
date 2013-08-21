//============================================================================
// Native types. All native functions take extended handle types when
// possible to help prevent passing bad values to native functions
//
type agent			    extends     handle  // all reference counted objects
type event              extends     agent  // a reference to an event registration
type player             extends     agent  // a single player reference
type widget             extends     agent  // an interactive game object with life
type unit               extends     widget  // a single unit reference
type destructable       extends     widget
type item               extends     widget
type ability            extends     agent
type buff               extends     ability
type force              extends     agent
type group              extends     agent
type trigger            extends     agent
type triggercondition   extends     agent
type triggeraction      extends     handle
type timer              extends     agent
type location           extends     agent
type region             extends     agent
type rect               extends     agent
type boolexpr           extends     agent
type sound              extends     agent
type conditionfunc      extends     boolexpr
type filterfunc         extends     boolexpr
type unitpool           extends     handle
type itempool           extends     handle
type race               extends     handle
type alliancetype       extends     handle
type racepreference     extends     handle
type gamestate          extends     handle
type igamestate         extends     gamestate
type fgamestate         extends     gamestate
type playerstate        extends     handle
type playerscore        extends     handle
type playergameresult   extends     handle
type unitstate          extends     handle
type aidifficulty       extends     handle

type eventid            extends     handle
type gameevent          extends     eventid
type playerevent        extends     eventid
type playerunitevent    extends     eventid
type unitevent          extends     eventid
type limitop            extends     eventid
type widgetevent        extends     eventid
type dialogevent        extends     eventid
type unittype           extends     handle

type gamespeed          extends     handle
type gamedifficulty     extends     handle
type gametype           extends     handle
type mapflag            extends     handle
type mapvisibility      extends     handle
type mapsetting         extends     handle
type mapdensity         extends     handle
type mapcontrol         extends     handle
type playerslotstate    extends     handle
type volumegroup        extends     handle
type camerafield        extends     handle
type camerasetup        extends     handle
type playercolor        extends     handle
type placement          extends     handle
type startlocprio       extends     handle
type raritycontrol      extends     handle
type blendmode          extends     handle
type texmapflags        extends     handle
type effect             extends     agent
type effecttype         extends     handle
type weathereffect      extends     handle
type terraindeformation extends     handle
type fogstate           extends     handle
type fogmodifier        extends     agent
type dialog             extends     agent
type button             extends     agent
type quest              extends     agent
type questitem          extends     agent
type defeatcondition    extends     agent
type timerdialog        extends     agent
type leaderboard        extends     agent
type multiboard         extends     agent
type multiboarditem     extends     agent
type trackable          extends     agent
type gamecache          extends     agent
type version            extends     handle
type itemtype           extends     handle
type texttag            extends     handle
type attacktype         extends     handle
type damagetype         extends     handle
type weapontype         extends     handle
type soundtype          extends     handle
type lightning          extends     handle
type pathingtype        extends     handle
type image              extends     handle
type ubersplat          extends     handle
type hashtable          extends     agent

constant native ConvertRace                 takes integer i returns race
constant native ConvertAllianceType         takes integer i returns alliancetype
constant native ConvertRacePref             takes integer i returns racepreference
constant native ConvertIGameState           takes integer i returns igamestate
constant native ConvertFGameState           takes integer i returns fgamestate
constant native ConvertPlayerState          takes integer i returns playerstate
constant native ConvertPlayerScore          takes integer i returns playerscore
constant native ConvertPlayerGameResult     takes integer i returns playergameresult
constant native ConvertUnitState            takes integer i returns unitstate
constant native ConvertAIDifficulty         takes integer i returns aidifficulty
constant native ConvertGameEvent            takes integer i returns gameevent
constant native ConvertPlayerEvent          takes integer i returns playerevent
constant native ConvertPlayerUnitEvent      takes integer i returns playerunitevent
constant native ConvertWidgetEvent          takes integer i returns widgetevent
constant native ConvertDialogEvent          takes integer i returns dialogevent
constant native ConvertUnitEvent            takes integer i returns unitevent
constant native ConvertLimitOp              takes integer i returns limitop
constant native ConvertUnitType             takes integer i returns unittype
constant native ConvertGameSpeed            takes integer i returns gamespeed
constant native ConvertPlacement            takes integer i returns placement
constant native ConvertStartLocPrio         takes integer i returns startlocprio
constant native ConvertGameDifficulty       takes integer i returns gamedifficulty
constant native ConvertGameType             takes integer i returns gametype
constant native ConvertMapFlag              takes integer i returns mapflag
constant native ConvertMapVisibility        takes integer i returns mapvisibility
constant native ConvertMapSetting           takes integer i returns mapsetting
constant native ConvertMapDensity           takes integer i returns mapdensity
constant native ConvertMapControl           takes integer i returns mapcontrol
constant native ConvertPlayerColor          takes integer i returns playercolor
constant native ConvertPlayerSlotState      takes integer i returns playerslotstate
constant native ConvertVolumeGroup          takes integer i returns volumegroup
constant native ConvertCameraField          takes integer i returns camerafield
constant native ConvertBlendMode            takes integer i returns blendmode
constant native ConvertRarityControl        takes integer i returns raritycontrol
constant native ConvertTexMapFlags          takes integer i returns texmapflags
constant native ConvertFogState             takes integer i returns fogstate
constant native ConvertEffectType           takes integer i returns effecttype
constant native ConvertVersion              takes integer i returns version
constant native ConvertItemType             takes integer i returns itemtype
constant native ConvertAttackType           takes integer i returns attacktype
constant native ConvertDamageType           takes integer i returns damagetype
constant native ConvertWeaponType           takes integer i returns weapontype
constant native ConvertSoundType            takes integer i returns soundtype
constant native ConvertPathingType          takes integer i returns pathingtype

constant native OrderId                     takes string  orderIdString     returns integer
constant native OrderId2String              takes integer orderId           returns string
constant native UnitId                      takes string  unitIdString      returns integer
constant native UnitId2String               takes integer unitId            returns string

// Not currently working correctly...
constant native AbilityId                   takes string  abilityIdString   returns integer
constant native AbilityId2String            takes integer abilityId         returns string

// Looks up the "name" field for any object (unit, item, ability)
constant native GetObjectName               takes integer objectId          returns string

globals

//===================================================
// Game Constants    
//===================================================

    // pfff
    constant boolean            FALSE                           = false
    constant boolean            TRUE                            = true
    constant integer            JASS_MAX_ARRAY_SIZE             = 8192

    constant integer            PLAYER_NEUTRAL_PASSIVE          = 15
    constant integer            PLAYER_NEUTRAL_AGGRESSIVE       = 12

    constant playercolor        PLAYER_COLOR_RED                = ConvertPlayerColor(0)
    constant playercolor        PLAYER_COLOR_BLUE               = ConvertPlayerColor(1)
    constant playercolor        PLAYER_COLOR_CYAN               = ConvertPlayerColor(2)
    constant playercolor        PLAYER_COLOR_PURPLE             = ConvertPlayerColor(3)
    constant playercolor        PLAYER_COLOR_YELLOW             = ConvertPlayerColor(4)
    constant playercolor        PLAYER_COLOR_ORANGE             = ConvertPlayerColor(5)
    constant playercolor        PLAYER_COLOR_GREEN              = ConvertPlayerColor(6)
    constant playercolor        PLAYER_COLOR_PINK               = ConvertPlayerColor(7)
    constant playercolor        PLAYER_COLOR_LIGHT_GRAY         = ConvertPlayerColor(8)
    constant playercolor        PLAYER_COLOR_LIGHT_BLUE         = ConvertPlayerColor(9)
    constant playercolor        PLAYER_COLOR_AQUA               = ConvertPlayerColor(10)
    constant playercolor        PLAYER_COLOR_BROWN              = ConvertPlayerColor(11)

    constant race               RACE_HUMAN                      = ConvertRace(1)
    constant race               RACE_ORC                        = ConvertRace(2)
    constant race               RACE_UNDEAD                     = ConvertRace(3)
    constant race               RACE_NIGHTELF                   = ConvertRace(4)
    constant race               RACE_DEMON                      = ConvertRace(5)
    constant race               RACE_OTHER                      = ConvertRace(7)

    constant playergameresult   PLAYER_GAME_RESULT_VICTORY      = ConvertPlayerGameResult(0)
    constant playergameresult   PLAYER_GAME_RESULT_DEFEAT       = ConvertPlayerGameResult(1)
    constant playergameresult   PLAYER_GAME_RESULT_TIE          = ConvertPlayerGameResult(2)
    constant playergameresult   PLAYER_GAME_RESULT_NEUTRAL      = ConvertPlayerGameResult(3)

    constant alliancetype       ALLIANCE_PASSIVE                = ConvertAllianceType(0)
    constant alliancetype       ALLIANCE_HELP_REQUEST           = ConvertAllianceType(1)
    constant alliancetype       ALLIANCE_HELP_RESPONSE          = ConvertAllianceType(2)
    constant alliancetype       ALLIANCE_SHARED_XP              = ConvertAllianceType(3)
    constant alliancetype       ALLIANCE_SHARED_SPELLS          = ConvertAllianceType(4)
    constant alliancetype       ALLIANCE_SHARED_VISION          = ConvertAllianceType(5)
    constant alliancetype       ALLIANCE_SHARED_CONTROL         = ConvertAllianceType(6)
    constant alliancetype       ALLIANCE_SHARED_ADVANCED_CONTROL= ConvertAllianceType(7)
    constant alliancetype       ALLIANCE_RESCUABLE              = ConvertAllianceType(8)
    constant alliancetype       ALLIANCE_SHARED_VISION_FORCED   = ConvertAllianceType(9)

    constant version            VERSION_REIGN_OF_CHAOS          = ConvertVersion(0)
    constant version            VERSION_FROZEN_THRONE           = ConvertVersion(1)

    constant attacktype         ATTACK_TYPE_NORMAL              = ConvertAttackType(0)
    constant attacktype         ATTACK_TYPE_MELEE               = ConvertAttackType(1)
    constant attacktype         ATTACK_TYPE_PIERCE              = ConvertAttackType(2)
    constant attacktype         ATTACK_TYPE_SIEGE               = ConvertAttackType(3)
    constant attacktype         ATTACK_TYPE_MAGIC               = ConvertAttackType(4)
    constant attacktype         ATTACK_TYPE_CHAOS               = ConvertAttackType(5)
    constant attacktype         ATTACK_TYPE_HERO                = ConvertAttackType(6)

    constant damagetype         DAMAGE_TYPE_UNKNOWN             = ConvertDamageType(0)
    constant damagetype         DAMAGE_TYPE_NORMAL              = ConvertDamageType(4)
    constant damagetype         DAMAGE_TYPE_ENHANCED            = ConvertDamageType(5)
    constant damagetype         DAMAGE_TYPE_FIRE                = ConvertDamageType(8)
    constant damagetype         DAMAGE_TYPE_COLD                = ConvertDamageType(9)
    constant damagetype         DAMAGE_TYPE_LIGHTNING           = ConvertDamageType(10)
    constant damagetype         DAMAGE_TYPE_POISON              = ConvertDamageType(11)
    constant damagetype         DAMAGE_TYPE_DISEASE             = ConvertDamageType(12)
    constant damagetype         DAMAGE_TYPE_DIVINE              = ConvertDamageType(13)
    constant damagetype         DAMAGE_TYPE_MAGIC               = ConvertDamageType(14)
    constant damagetype         DAMAGE_TYPE_SONIC               = ConvertDamageType(15)
    constant damagetype         DAMAGE_TYPE_ACID                = ConvertDamageType(16)
    constant damagetype         DAMAGE_TYPE_FORCE               = ConvertDamageType(17)
    constant damagetype         DAMAGE_TYPE_DEATH               = ConvertDamageType(18)
    constant damagetype         DAMAGE_TYPE_MIND                = ConvertDamageType(19)
    constant damagetype         DAMAGE_TYPE_PLANT               = ConvertDamageType(20)
    constant damagetype         DAMAGE_TYPE_DEFENSIVE           = ConvertDamageType(21)
    constant damagetype         DAMAGE_TYPE_DEMOLITION          = ConvertDamageType(22)
    constant damagetype         DAMAGE_TYPE_SLOW_POISON         = ConvertDamageType(23)
    constant damagetype         DAMAGE_TYPE_SPIRIT_LINK         = ConvertDamageType(24)
    constant damagetype         DAMAGE_TYPE_SHADOW_STRIKE       = ConvertDamageType(25)
    constant damagetype         DAMAGE_TYPE_UNIVERSAL           = ConvertDamageType(26)

    constant weapontype         WEAPON_TYPE_WHOKNOWS            = ConvertWeaponType(0)
    constant weapontype         WEAPON_TYPE_METAL_LIGHT_CHOP    = ConvertWeaponType(1)
    constant weapontype         WEAPON_TYPE_METAL_MEDIUM_CHOP   = ConvertWeaponType(2)
    constant weapontype         WEAPON_TYPE_METAL_HEAVY_CHOP    = ConvertWeaponType(3)
    constant weapontype         WEAPON_TYPE_METAL_LIGHT_SLICE   = ConvertWeaponType(4)
    constant weapontype         WEAPON_TYPE_METAL_MEDIUM_SLICE  = ConvertWeaponType(5)
    constant weapontype         WEAPON_TYPE_METAL_HEAVY_SLICE   = ConvertWeaponType(6)
    constant weapontype         WEAPON_TYPE_METAL_MEDIUM_BASH   = ConvertWeaponType(7)
    constant weapontype         WEAPON_TYPE_METAL_HEAVY_BASH    = ConvertWeaponType(8)
    constant weapontype         WEAPON_TYPE_METAL_MEDIUM_STAB   = ConvertWeaponType(9)
    constant weapontype         WEAPON_TYPE_METAL_HEAVY_STAB    = ConvertWeaponType(10)
    constant weapontype         WEAPON_TYPE_WOOD_LIGHT_SLICE    = ConvertWeaponType(11)
    constant weapontype         WEAPON_TYPE_WOOD_MEDIUM_SLICE   = ConvertWeaponType(12)
    constant weapontype         WEAPON_TYPE_WOOD_HEAVY_SLICE    = ConvertWeaponType(13)
    constant weapontype         WEAPON_TYPE_WOOD_LIGHT_BASH     = ConvertWeaponType(14)
    constant weapontype         WEAPON_TYPE_WOOD_MEDIUM_BASH    = ConvertWeaponType(15)
    constant weapontype         WEAPON_TYPE_WOOD_HEAVY_BASH     = ConvertWeaponType(16)
    constant weapontype         WEAPON_TYPE_WOOD_LIGHT_STAB     = ConvertWeaponType(17)
    constant weapontype         WEAPON_TYPE_WOOD_MEDIUM_STAB    = ConvertWeaponType(18)
    constant weapontype         WEAPON_TYPE_CLAW_LIGHT_SLICE    = ConvertWeaponType(19)
    constant weapontype         WEAPON_TYPE_CLAW_MEDIUM_SLICE   = ConvertWeaponType(20)
    constant weapontype         WEAPON_TYPE_CLAW_HEAVY_SLICE    = ConvertWeaponType(21)
    constant weapontype         WEAPON_TYPE_AXE_MEDIUM_CHOP     = ConvertWeaponType(22)
    constant weapontype         WEAPON_TYPE_ROCK_HEAVY_BASH     = ConvertWeaponType(23)

    constant pathingtype        PATHING_TYPE_ANY                = ConvertPathingType(0)
    constant pathingtype        PATHING_TYPE_WALKABILITY        = ConvertPathingType(1)
    constant pathingtype        PATHING_TYPE_FLYABILITY         = ConvertPathingType(2)
    constant pathingtype        PATHING_TYPE_BUILDABILITY       = ConvertPathingType(3)
    constant pathingtype        PATHING_TYPE_PEONHARVESTPATHING = ConvertPathingType(4)
    constant pathingtype        PATHING_TYPE_BLIGHTPATHING      = ConvertPathingType(5)
    constant pathingtype        PATHING_TYPE_FLOATABILITY       = ConvertPathingType(6)
    constant pathingtype        PATHING_TYPE_AMPHIBIOUSPATHING  = ConvertPathingType(7)

//===================================================
// Map Setup Constants    
//===================================================

    constant racepreference     RACE_PREF_HUMAN                     = ConvertRacePref(1)
    constant racepreference     RACE_PREF_ORC                       = ConvertRacePref(2)
    constant racepreference     RACE_PREF_NIGHTELF                  = ConvertRacePref(4)
    constant racepreference     RACE_PREF_UNDEAD                    = ConvertRacePref(8)
    constant racepreference     RACE_PREF_DEMON                     = ConvertRacePref(16)
    constant racepreference     RACE_PREF_RANDOM                    = ConvertRacePref(32)
    constant racepreference     RACE_PREF_USER_SELECTABLE           = ConvertRacePref(64)

    constant mapcontrol         MAP_CONTROL_USER                    = ConvertMapControl(0)
    constant mapcontrol         MAP_CONTROL_COMPUTER                = ConvertMapControl(1)
    constant mapcontrol         MAP_CONTROL_RESCUABLE               = ConvertMapControl(2)
    constant mapcontrol         MAP_CONTROL_NEUTRAL                 = ConvertMapControl(3)
    constant mapcontrol         MAP_CONTROL_CREEP                   = ConvertMapControl(4)
    constant mapcontrol         MAP_CONTROL_NONE                    = ConvertMapControl(5)

    constant gametype           GAME_TYPE_MELEE                     = ConvertGameType(1)
    constant gametype           GAME_TYPE_FFA                       = ConvertGameType(2)
    constant gametype           GAME_TYPE_USE_MAP_SETTINGS          = ConvertGameType(4)
    constant gametype           GAME_TYPE_BLIZ                      = ConvertGameType(8)
    constant gametype           GAME_TYPE_ONE_ON_ONE                = ConvertGameType(16)
    constant gametype           GAME_TYPE_TWO_TEAM_PLAY             = ConvertGameType(32)
    constant gametype           GAME_TYPE_THREE_TEAM_PLAY           = ConvertGameType(64)
    constant gametype           GAME_TYPE_FOUR_TEAM_PLAY            = ConvertGameType(128)

    constant mapflag            MAP_FOG_HIDE_TERRAIN                = ConvertMapFlag(1)
    constant mapflag            MAP_FOG_MAP_EXPLORED                = ConvertMapFlag(2)
    constant mapflag            MAP_FOG_ALWAYS_VISIBLE              = ConvertMapFlag(4)

    constant mapflag            MAP_USE_HANDICAPS                   = ConvertMapFlag(8)
    constant mapflag            MAP_OBSERVERS                       = ConvertMapFlag(16)
    constant mapflag            MAP_OBSERVERS_ON_DEATH              = ConvertMapFlag(32)

    constant mapflag            MAP_FIXED_COLORS                    = ConvertMapFlag(128)
    
    constant mapflag            MAP_LOCK_RESOURCE_TRADING           = ConvertMapFlag(256)
    constant mapflag            MAP_RESOURCE_TRADING_ALLIES_ONLY    = ConvertMapFlag(512)

    constant mapflag            MAP_LOCK_ALLIANCE_CHANGES           = ConvertMapFlag(1024)
    constant mapflag            MAP_ALLIANCE_CHANGES_HIDDEN         = ConvertMapFlag(2048)

    constant mapflag            MAP_CHEATS                          = ConvertMapFlag(4096)
    constant mapflag            MAP_CHEATS_HIDDEN                   = ConvertMapFlag(8192)

    constant mapflag            MAP_LOCK_SPEED                      = ConvertMapFlag(8192*2)
    constant mapflag            MAP_LOCK_RANDOM_SEED                = ConvertMapFlag(8192*4)
    constant mapflag            MAP_SHARED_ADVANCED_CONTROL         = ConvertMapFlag(8192*8)
    constant mapflag            MAP_RANDOM_HERO                     = ConvertMapFlag(8192*16)
    constant mapflag            MAP_RANDOM_RACES                    = ConvertMapFlag(8192*32)
    constant mapflag            MAP_RELOADED                        = ConvertMapFlag(8192*64)

    constant placement          MAP_PLACEMENT_RANDOM                = ConvertPlacement(0)   // random among all slots
    constant placement          MAP_PLACEMENT_FIXED                 = ConvertPlacement(1)   // player 0 in start loc 0...
    constant placement          MAP_PLACEMENT_USE_MAP_SETTINGS      = ConvertPlacement(2)   // whatever was specified by the script
    constant placement          MAP_PLACEMENT_TEAMS_TOGETHER        = ConvertPlacement(3)   // random with allies next to each other    

    constant startlocprio       MAP_LOC_PRIO_LOW                    = ConvertStartLocPrio(0)
    constant startlocprio       MAP_LOC_PRIO_HIGH                   = ConvertStartLocPrio(1)
    constant startlocprio       MAP_LOC_PRIO_NOT                    = ConvertStartLocPrio(2)

    constant mapdensity         MAP_DENSITY_NONE                    = ConvertMapDensity(0)
    constant mapdensity         MAP_DENSITY_LIGHT                   = ConvertMapDensity(1)
    constant mapdensity         MAP_DENSITY_MEDIUM                  = ConvertMapDensity(2)
    constant mapdensity         MAP_DENSITY_HEAVY                   = ConvertMapDensity(3)

    constant gamedifficulty     MAP_DIFFICULTY_EASY                 = ConvertGameDifficulty(0)
    constant gamedifficulty     MAP_DIFFICULTY_NORMAL               = ConvertGameDifficulty(1)
    constant gamedifficulty     MAP_DIFFICULTY_HARD                 = ConvertGameDifficulty(2)
    constant gamedifficulty     MAP_DIFFICULTY_INSANE               = ConvertGameDifficulty(3)

    constant gamespeed          MAP_SPEED_SLOWEST                   = ConvertGameSpeed(0)
    constant gamespeed          MAP_SPEED_SLOW                      = ConvertGameSpeed(1)
    constant gamespeed          MAP_SPEED_NORMAL                    = ConvertGameSpeed(2)
    constant gamespeed          MAP_SPEED_FAST                      = ConvertGameSpeed(3)
    constant gamespeed          MAP_SPEED_FASTEST                   = ConvertGameSpeed(4)

    constant playerslotstate    PLAYER_SLOT_STATE_EMPTY             = ConvertPlayerSlotState(0)
    constant playerslotstate    PLAYER_SLOT_STATE_PLAYING           = ConvertPlayerSlotState(1)
    constant playerslotstate    PLAYER_SLOT_STATE_LEFT              = ConvertPlayerSlotState(2)

//===================================================
// Sound Constants
//===================================================
    constant volumegroup        SOUND_VOLUMEGROUP_UNITMOVEMENT      = ConvertVolumeGroup(0)
    constant volumegroup        SOUND_VOLUMEGROUP_UNITSOUNDS        = ConvertVolumeGroup(1)
    constant volumegroup        SOUND_VOLUMEGROUP_COMBAT            = ConvertVolumeGroup(2)
    constant volumegroup        SOUND_VOLUMEGROUP_SPELLS            = ConvertVolumeGroup(3)
    constant volumegroup        SOUND_VOLUMEGROUP_UI                = ConvertVolumeGroup(4)
    constant volumegroup        SOUND_VOLUMEGROUP_MUSIC             = ConvertVolumeGroup(5)
    constant volumegroup        SOUND_VOLUMEGROUP_AMBIENTSOUNDS     = ConvertVolumeGroup(6)
    constant volumegroup        SOUND_VOLUMEGROUP_FIRE              = ConvertVolumeGroup(7)


//===================================================
// Game, Player, and Unit States
//
// For use with TriggerRegister<X>StateEvent
//
//===================================================

    constant igamestate GAME_STATE_DIVINE_INTERVENTION          = ConvertIGameState(0)
    constant igamestate GAME_STATE_DISCONNECTED                 = ConvertIGameState(1)
    constant fgamestate GAME_STATE_TIME_OF_DAY                  = ConvertFGameState(2)

    constant playerstate PLAYER_STATE_GAME_RESULT               = ConvertPlayerState(0)

    // current resource levels
    //
    constant playerstate PLAYER_STATE_RESOURCE_GOLD             = ConvertPlayerState(1)
    constant playerstate PLAYER_STATE_RESOURCE_LUMBER           = ConvertPlayerState(2)
    constant playerstate PLAYER_STATE_RESOURCE_HERO_TOKENS      = ConvertPlayerState(3)
    constant playerstate PLAYER_STATE_RESOURCE_FOOD_CAP         = ConvertPlayerState(4)
    constant playerstate PLAYER_STATE_RESOURCE_FOOD_USED        = ConvertPlayerState(5)
    constant playerstate PLAYER_STATE_FOOD_CAP_CEILING          = ConvertPlayerState(6)

    constant playerstate PLAYER_STATE_GIVES_BOUNTY              = ConvertPlayerState(7)
    constant playerstate PLAYER_STATE_ALLIED_VICTORY            = ConvertPlayerState(8)
    constant playerstate PLAYER_STATE_PLACED                    = ConvertPlayerState(9)
    constant playerstate PLAYER_STATE_OBSERVER_ON_DEATH         = ConvertPlayerState(10)
    constant playerstate PLAYER_STATE_OBSERVER                  = ConvertPlayerState(11)
    constant playerstate PLAYER_STATE_UNFOLLOWABLE              = ConvertPlayerState(12)

    // taxation rate for each resource
    //
    constant playerstate PLAYER_STATE_GOLD_UPKEEP_RATE          = ConvertPlayerState(13)
    constant playerstate PLAYER_STATE_LUMBER_UPKEEP_RATE        = ConvertPlayerState(14)

    // cumulative resources collected by the player during the mission
    //
    constant playerstate PLAYER_STATE_GOLD_GATHERED             = ConvertPlayerState(15)
    constant playerstate PLAYER_STATE_LUMBER_GATHERED           = ConvertPlayerState(16)

    constant playerstate PLAYER_STATE_NO_CREEP_SLEEP            = ConvertPlayerState(25)

    constant unitstate UNIT_STATE_LIFE                          = ConvertUnitState(0)
    constant unitstate UNIT_STATE_MAX_LIFE                      = ConvertUnitState(1)
    constant unitstate UNIT_STATE_MANA                          = ConvertUnitState(2)
    constant unitstate UNIT_STATE_MAX_MANA                      = ConvertUnitState(3)

    constant aidifficulty AI_DIFFICULTY_NEWBIE                  = ConvertAIDifficulty(0)
    constant aidifficulty AI_DIFFICULTY_NORMAL                  = ConvertAIDifficulty(1)
    constant aidifficulty AI_DIFFICULTY_INSANE                  = ConvertAIDifficulty(2)

    // player score values
    constant playerscore PLAYER_SCORE_UNITS_TRAINED             = ConvertPlayerScore(0)
    constant playerscore PLAYER_SCORE_UNITS_KILLED              = ConvertPlayerScore(1)
    constant playerscore PLAYER_SCORE_STRUCT_BUILT              = ConvertPlayerScore(2)
    constant playerscore PLAYER_SCORE_STRUCT_RAZED              = ConvertPlayerScore(3)
    constant playerscore PLAYER_SCORE_TECH_PERCENT              = ConvertPlayerScore(4)
    constant playerscore PLAYER_SCORE_FOOD_MAXPROD              = ConvertPlayerScore(5)
    constant playerscore PLAYER_SCORE_FOOD_MAXUSED              = ConvertPlayerScore(6)
    constant playerscore PLAYER_SCORE_HEROES_KILLED             = ConvertPlayerScore(7)
    constant playerscore PLAYER_SCORE_ITEMS_GAINED              = ConvertPlayerScore(8)
    constant playerscore PLAYER_SCORE_MERCS_HIRED               = ConvertPlayerScore(9)
    constant playerscore PLAYER_SCORE_GOLD_MINED_TOTAL          = ConvertPlayerScore(10)
    constant playerscore PLAYER_SCORE_GOLD_MINED_UPKEEP         = ConvertPlayerScore(11)
    constant playerscore PLAYER_SCORE_GOLD_LOST_UPKEEP          = ConvertPlayerScore(12)
    constant playerscore PLAYER_SCORE_GOLD_LOST_TAX             = ConvertPlayerScore(13)
    constant playerscore PLAYER_SCORE_GOLD_GIVEN                = ConvertPlayerScore(14)
    constant playerscore PLAYER_SCORE_GOLD_RECEIVED             = ConvertPlayerScore(15)
    constant playerscore PLAYER_SCORE_LUMBER_TOTAL              = ConvertPlayerScore(16)
    constant playerscore PLAYER_SCORE_LUMBER_LOST_UPKEEP        = ConvertPlayerScore(17)
    constant playerscore PLAYER_SCORE_LUMBER_LOST_TAX           = ConvertPlayerScore(18)
    constant playerscore PLAYER_SCORE_LUMBER_GIVEN              = ConvertPlayerScore(19)
    constant playerscore PLAYER_SCORE_LUMBER_RECEIVED           = ConvertPlayerScore(20)
    constant playerscore PLAYER_SCORE_UNIT_TOTAL                = ConvertPlayerScore(21)
    constant playerscore PLAYER_SCORE_HERO_TOTAL                = ConvertPlayerScore(22)
    constant playerscore PLAYER_SCORE_RESOURCE_TOTAL            = ConvertPlayerScore(23)
    constant playerscore PLAYER_SCORE_TOTAL                     = ConvertPlayerScore(24)
        
//===================================================
// Game, Player and Unit Events
//
//  When an event causes a trigger to fire these
//  values allow the action code to determine which
//  event was dispatched and therefore which set of
//  native functions should be used to get information
//  about the event.
//
// Do NOT change the order or value of these constants
// without insuring that the JASS_GAME_EVENTS_WAR3 enum
// is changed to match.
//
//===================================================

    //===================================================
    // For use with TriggerRegisterGameEvent    
    //===================================================    

    constant gameevent EVENT_GAME_VICTORY                       = ConvertGameEvent(0)
    constant gameevent EVENT_GAME_END_LEVEL                     = ConvertGameEvent(1)

    constant gameevent EVENT_GAME_VARIABLE_LIMIT                = ConvertGameEvent(2)
    constant gameevent EVENT_GAME_STATE_LIMIT                   = ConvertGameEvent(3)   

    constant gameevent EVENT_GAME_TIMER_EXPIRED                 = ConvertGameEvent(4)

    constant gameevent EVENT_GAME_ENTER_REGION                  = ConvertGameEvent(5)
    constant gameevent EVENT_GAME_LEAVE_REGION                  = ConvertGameEvent(6)

    constant gameevent EVENT_GAME_TRACKABLE_HIT                 = ConvertGameEvent(7)
    constant gameevent EVENT_GAME_TRACKABLE_TRACK               = ConvertGameEvent(8)

    constant gameevent EVENT_GAME_SHOW_SKILL                    = ConvertGameEvent(9)    
    constant gameevent EVENT_GAME_BUILD_SUBMENU                 = ConvertGameEvent(10)

    //===================================================
    // For use with TriggerRegisterPlayerEvent
    //===================================================
    constant playerevent EVENT_PLAYER_STATE_LIMIT               = ConvertPlayerEvent(11)
    constant playerevent EVENT_PLAYER_ALLIANCE_CHANGED          = ConvertPlayerEvent(12)

    constant playerevent EVENT_PLAYER_DEFEAT                    = ConvertPlayerEvent(13)
    constant playerevent EVENT_PLAYER_VICTORY                   = ConvertPlayerEvent(14)
    constant playerevent EVENT_PLAYER_LEAVE                     = ConvertPlayerEvent(15)
    constant playerevent EVENT_PLAYER_CHAT                      = ConvertPlayerEvent(16)
    constant playerevent EVENT_PLAYER_END_CINEMATIC             = ConvertPlayerEvent(17)

    //===================================================
    // For use with TriggerRegisterPlayerUnitEvent
    //===================================================

    constant playerunitevent EVENT_PLAYER_UNIT_ATTACKED                 = ConvertPlayerUnitEvent(18)
    constant playerunitevent EVENT_PLAYER_UNIT_RESCUED                  = ConvertPlayerUnitEvent(19)

    constant playerunitevent EVENT_PLAYER_UNIT_DEATH                    = ConvertPlayerUnitEvent(20)
    constant playerunitevent EVENT_PLAYER_UNIT_DECAY                    = ConvertPlayerUnitEvent(21)

    constant playerunitevent EVENT_PLAYER_UNIT_DETECTED                 = ConvertPlayerUnitEvent(22)
    constant playerunitevent EVENT_PLAYER_UNIT_HIDDEN                   = ConvertPlayerUnitEvent(23)

    constant playerunitevent EVENT_PLAYER_UNIT_SELECTED                 = ConvertPlayerUnitEvent(24)
    constant playerunitevent EVENT_PLAYER_UNIT_DESELECTED               = ConvertPlayerUnitEvent(25)

    constant playerunitevent EVENT_PLAYER_UNIT_CONSTRUCT_START          = ConvertPlayerUnitEvent(26)
    constant playerunitevent EVENT_PLAYER_UNIT_CONSTRUCT_CANCEL         = ConvertPlayerUnitEvent(27)
    constant playerunitevent EVENT_PLAYER_UNIT_CONSTRUCT_FINISH         = ConvertPlayerUnitEvent(28)

    constant playerunitevent EVENT_PLAYER_UNIT_UPGRADE_START            = ConvertPlayerUnitEvent(29)
    constant playerunitevent EVENT_PLAYER_UNIT_UPGRADE_CANCEL           = ConvertPlayerUnitEvent(30)
    constant playerunitevent EVENT_PLAYER_UNIT_UPGRADE_FINISH           = ConvertPlayerUnitEvent(31)

    constant playerunitevent EVENT_PLAYER_UNIT_TRAIN_START              = ConvertPlayerUnitEvent(32)
    constant playerunitevent EVENT_PLAYER_UNIT_TRAIN_CANCEL             = ConvertPlayerUnitEvent(33)
    constant playerunitevent EVENT_PLAYER_UNIT_TRAIN_FINISH             = ConvertPlayerUnitEvent(34)

    constant playerunitevent EVENT_PLAYER_UNIT_RESEARCH_START           = ConvertPlayerUnitEvent(35)
    constant playerunitevent EVENT_PLAYER_UNIT_RESEARCH_CANCEL          = ConvertPlayerUnitEvent(36)
    constant playerunitevent EVENT_PLAYER_UNIT_RESEARCH_FINISH          = ConvertPlayerUnitEvent(37)
    constant playerunitevent EVENT_PLAYER_UNIT_ISSUED_ORDER             = ConvertPlayerUnitEvent(38)
    constant playerunitevent EVENT_PLAYER_UNIT_ISSUED_POINT_ORDER       = ConvertPlayerUnitEvent(39)
    constant playerunitevent EVENT_PLAYER_UNIT_ISSUED_TARGET_ORDER      = ConvertPlayerUnitEvent(40)
    constant playerunitevent EVENT_PLAYER_UNIT_ISSUED_UNIT_ORDER        = ConvertPlayerUnitEvent(40)    // for compat

    constant playerunitevent EVENT_PLAYER_HERO_LEVEL                    = ConvertPlayerUnitEvent(41)
    constant playerunitevent EVENT_PLAYER_HERO_SKILL                    = ConvertPlayerUnitEvent(42)

    constant playerunitevent EVENT_PLAYER_HERO_REVIVABLE                = ConvertPlayerUnitEvent(43)

    constant playerunitevent EVENT_PLAYER_HERO_REVIVE_START             = ConvertPlayerUnitEvent(44)
    constant playerunitevent EVENT_PLAYER_HERO_REVIVE_CANCEL            = ConvertPlayerUnitEvent(45)
    constant playerunitevent EVENT_PLAYER_HERO_REVIVE_FINISH            = ConvertPlayerUnitEvent(46)
    constant playerunitevent EVENT_PLAYER_UNIT_SUMMON                   = ConvertPlayerUnitEvent(47)
    constant playerunitevent EVENT_PLAYER_UNIT_DROP_ITEM                = ConvertPlayerUnitEvent(48)
    constant playerunitevent EVENT_PLAYER_UNIT_PICKUP_ITEM              = ConvertPlayerUnitEvent(49)
    constant playerunitevent EVENT_PLAYER_UNIT_USE_ITEM                 = ConvertPlayerUnitEvent(50)

    constant playerunitevent EVENT_PLAYER_UNIT_LOADED                   = ConvertPlayerUnitEvent(51)
    
    //===================================================
    // For use with TriggerRegisterUnitEvent
    //===================================================

    constant unitevent EVENT_UNIT_DAMAGED                               = ConvertUnitEvent(52)
    constant unitevent EVENT_UNIT_DEATH                                 = ConvertUnitEvent(53)
    constant unitevent EVENT_UNIT_DECAY                                 = ConvertUnitEvent(54)
    constant unitevent EVENT_UNIT_DETECTED                              = ConvertUnitEvent(55)
    constant unitevent EVENT_UNIT_HIDDEN                                = ConvertUnitEvent(56)
    constant unitevent EVENT_UNIT_SELECTED                              = ConvertUnitEvent(57)
    constant unitevent EVENT_UNIT_DESELECTED                            = ConvertUnitEvent(58)
                                                                        
    constant unitevent EVENT_UNIT_STATE_LIMIT                           = ConvertUnitEvent(59)                                                                        

    // Events which may have a filter for the "other unit"              
    //                                                                  
    constant unitevent EVENT_UNIT_ACQUIRED_TARGET                       = ConvertUnitEvent(60)
    constant unitevent EVENT_UNIT_TARGET_IN_RANGE                       = ConvertUnitEvent(61)
    constant unitevent EVENT_UNIT_ATTACKED                              = ConvertUnitEvent(62)
    constant unitevent EVENT_UNIT_RESCUED                               = ConvertUnitEvent(63)
                                                                        
    constant unitevent EVENT_UNIT_CONSTRUCT_CANCEL                      = ConvertUnitEvent(64)
    constant unitevent EVENT_UNIT_CONSTRUCT_FINISH                      = ConvertUnitEvent(65)
                                                                        
    constant unitevent EVENT_UNIT_UPGRADE_START                         = ConvertUnitEvent(66)
    constant unitevent EVENT_UNIT_UPGRADE_CANCEL                        = ConvertUnitEvent(67)
    constant unitevent EVENT_UNIT_UPGRADE_FINISH                        = ConvertUnitEvent(68)
                                                                        
    // Events which involve the specified unit performing               
    // training of other units                                          
    //                                                                  
    constant unitevent EVENT_UNIT_TRAIN_START                           = ConvertUnitEvent(69)
    constant unitevent EVENT_UNIT_TRAIN_CANCEL                          = ConvertUnitEvent(70)
    constant unitevent EVENT_UNIT_TRAIN_FINISH                          = ConvertUnitEvent(71)
                                                                        
    constant unitevent EVENT_UNIT_RESEARCH_START                        = ConvertUnitEvent(72)
    constant unitevent EVENT_UNIT_RESEARCH_CANCEL                       = ConvertUnitEvent(73)
    constant unitevent EVENT_UNIT_RESEARCH_FINISH                       = ConvertUnitEvent(74)
                                                                        
    constant unitevent EVENT_UNIT_ISSUED_ORDER                          = ConvertUnitEvent(75)
    constant unitevent EVENT_UNIT_ISSUED_POINT_ORDER                    = ConvertUnitEvent(76)
    constant unitevent EVENT_UNIT_ISSUED_TARGET_ORDER                   = ConvertUnitEvent(77)
                                                                       
    constant unitevent EVENT_UNIT_HERO_LEVEL                            = ConvertUnitEvent(78)
    constant unitevent EVENT_UNIT_HERO_SKILL                            = ConvertUnitEvent(79)
                                                                        
    constant unitevent EVENT_UNIT_HERO_REVIVABLE                        = ConvertUnitEvent(80)
    constant unitevent EVENT_UNIT_HERO_REVIVE_START                     = ConvertUnitEvent(81)
    constant unitevent EVENT_UNIT_HERO_REVIVE_CANCEL                    = ConvertUnitEvent(82)
    constant unitevent EVENT_UNIT_HERO_REVIVE_FINISH                    = ConvertUnitEvent(83)
                                                                        
    constant unitevent EVENT_UNIT_SUMMON                                = ConvertUnitEvent(84)
                                                                        
    constant unitevent EVENT_UNIT_DROP_ITEM                             = ConvertUnitEvent(85)
    constant unitevent EVENT_UNIT_PICKUP_ITEM                           = ConvertUnitEvent(86)
    constant unitevent EVENT_UNIT_USE_ITEM                              = ConvertUnitEvent(87)

    constant unitevent EVENT_UNIT_LOADED                                = ConvertUnitEvent(88)

    constant widgetevent EVENT_WIDGET_DEATH                             = ConvertWidgetEvent(89)

    constant dialogevent EVENT_DIALOG_BUTTON_CLICK                      = ConvertDialogEvent(90)
    constant dialogevent EVENT_DIALOG_CLICK                             = ConvertDialogEvent(91)

    //===================================================
    // Frozen Throne Expansion Events
    // Need to be added here to preserve compat
    //===================================================

    //===================================================
    // For use with TriggerRegisterGameEvent    
    //===================================================    

    constant gameevent          EVENT_GAME_LOADED                       = ConvertGameEvent(256)
    constant gameevent          EVENT_GAME_TOURNAMENT_FINISH_SOON       = ConvertGameEvent(257)
    constant gameevent          EVENT_GAME_TOURNAMENT_FINISH_NOW        = ConvertGameEvent(258)
    constant gameevent          EVENT_GAME_SAVE                         = ConvertGameEvent(259)

    //===================================================
    // For use with TriggerRegisterPlayerEvent
    //===================================================

    constant playerevent        EVENT_PLAYER_ARROW_LEFT_DOWN            = ConvertPlayerEvent(261)
    constant playerevent        EVENT_PLAYER_ARROW_LEFT_UP              = ConvertPlayerEvent(262)
    constant playerevent        EVENT_PLAYER_ARROW_RIGHT_DOWN           = ConvertPlayerEvent(263)
    constant playerevent        EVENT_PLAYER_ARROW_RIGHT_UP             = ConvertPlayerEvent(264)
    constant playerevent        EVENT_PLAYER_ARROW_DOWN_DOWN            = ConvertPlayerEvent(265)
    constant playerevent        EVENT_PLAYER_ARROW_DOWN_UP              = ConvertPlayerEvent(266)
    constant playerevent        EVENT_PLAYER_ARROW_UP_DOWN              = ConvertPlayerEvent(267)
    constant playerevent        EVENT_PLAYER_ARROW_UP_UP                = ConvertPlayerEvent(268)

    //===================================================
    // For use with TriggerRegisterPlayerUnitEvent
    //===================================================

    constant playerunitevent    EVENT_PLAYER_UNIT_SELL                  = ConvertPlayerUnitEvent(269)
    constant playerunitevent    EVENT_PLAYER_UNIT_CHANGE_OWNER          = ConvertPlayerUnitEvent(270)
    constant playerunitevent    EVENT_PLAYER_UNIT_SELL_ITEM             = ConvertPlayerUnitEvent(271)
    constant playerunitevent    EVENT_PLAYER_UNIT_SPELL_CHANNEL         = ConvertPlayerUnitEvent(272)
    constant playerunitevent    EVENT_PLAYER_UNIT_SPELL_CAST            = ConvertPlayerUnitEvent(273)
    constant playerunitevent    EVENT_PLAYER_UNIT_SPELL_EFFECT          = ConvertPlayerUnitEvent(274)
    constant playerunitevent    EVENT_PLAYER_UNIT_SPELL_FINISH          = ConvertPlayerUnitEvent(275)
    constant playerunitevent    EVENT_PLAYER_UNIT_SPELL_ENDCAST         = ConvertPlayerUnitEvent(276)
    constant playerunitevent    EVENT_PLAYER_UNIT_PAWN_ITEM             = ConvertPlayerUnitEvent(277)

    //===================================================
    // For use with TriggerRegisterUnitEvent
    //===================================================

    constant unitevent          EVENT_UNIT_SELL                         = ConvertUnitEvent(286)
    constant unitevent          EVENT_UNIT_CHANGE_OWNER                 = ConvertUnitEvent(287)
    constant unitevent          EVENT_UNIT_SELL_ITEM                    = ConvertUnitEvent(288)
    constant unitevent          EVENT_UNIT_SPELL_CHANNEL                = ConvertUnitEvent(289)
    constant unitevent          EVENT_UNIT_SPELL_CAST                   = ConvertUnitEvent(290)
    constant unitevent          EVENT_UNIT_SPELL_EFFECT                 = ConvertUnitEvent(291)
    constant unitevent          EVENT_UNIT_SPELL_FINISH                 = ConvertUnitEvent(292)
    constant unitevent          EVENT_UNIT_SPELL_ENDCAST                = ConvertUnitEvent(293)
    constant unitevent          EVENT_UNIT_PAWN_ITEM                    = ConvertUnitEvent(294)

    //===================================================
    // Limit Event API constants    
    // variable, player state, game state, and unit state events
    // ( do NOT change the order of these... )
    //===================================================
    constant limitop LESS_THAN                              = ConvertLimitOp(0)
    constant limitop LESS_THAN_OR_EQUAL                     = ConvertLimitOp(1)
    constant limitop EQUAL                                  = ConvertLimitOp(2)
    constant limitop GREATER_THAN_OR_EQUAL                  = ConvertLimitOp(3)
    constant limitop GREATER_THAN                           = ConvertLimitOp(4)
    constant limitop NOT_EQUAL                              = ConvertLimitOp(5)

//===================================================
// Unit Type Constants for use with IsUnitType()
//===================================================

    constant unittype UNIT_TYPE_HERO                        = ConvertUnitType(0)
    constant unittype UNIT_TYPE_DEAD                        = ConvertUnitType(1)
    constant unittype UNIT_TYPE_STRUCTURE                   = ConvertUnitType(2)

    constant unittype UNIT_TYPE_FLYING                      = ConvertUnitType(3)
    constant unittype UNIT_TYPE_GROUND                      = ConvertUnitType(4)

    constant unittype UNIT_TYPE_ATTACKS_FLYING              = ConvertUnitType(5)
    constant unittype UNIT_TYPE_ATTACKS_GROUND              = ConvertUnitType(6)

    constant unittype UNIT_TYPE_MELEE_ATTACKER              = ConvertUnitType(7)
    constant unittype UNIT_TYPE_RANGED_ATTACKER             = ConvertUnitType(8)

    constant unittype UNIT_TYPE_GIANT                       = ConvertUnitType(9)
    constant unittype UNIT_TYPE_SUMMONED                    = ConvertUnitType(10)
    constant unittype UNIT_TYPE_STUNNED                     = ConvertUnitType(11)
    constant unittype UNIT_TYPE_PLAGUED                     = ConvertUnitType(12)
    constant unittype UNIT_TYPE_SNARED                      = ConvertUnitType(13)

    constant unittype UNIT_TYPE_UNDEAD                      = ConvertUnitType(14)
    constant unittype UNIT_TYPE_MECHANICAL                  = ConvertUnitType(15)
    constant unittype UNIT_TYPE_PEON                        = ConvertUnitType(16)
    constant unittype UNIT_TYPE_SAPPER                      = ConvertUnitType(17)
    constant unittype UNIT_TYPE_TOWNHALL                    = ConvertUnitType(18)    
    constant unittype UNIT_TYPE_ANCIENT                     = ConvertUnitType(19)
    
    constant unittype UNIT_TYPE_TAUREN                      = ConvertUnitType(20)
    constant unittype UNIT_TYPE_POISONED                    = ConvertUnitType(21)
    constant unittype UNIT_TYPE_POLYMORPHED                 = ConvertUnitType(22)
    constant unittype UNIT_TYPE_SLEEPING                    = ConvertUnitType(23)
    constant unittype UNIT_TYPE_RESISTANT                   = ConvertUnitType(24)
    constant unittype UNIT_TYPE_ETHEREAL                    = ConvertUnitType(25)
    constant unittype UNIT_TYPE_MAGIC_IMMUNE                = ConvertUnitType(26)

//===================================================
// Unit Type Constants for use with ChooseRandomItemEx()
//===================================================

    constant itemtype ITEM_TYPE_PERMANENT                   = ConvertItemType(0)
    constant itemtype ITEM_TYPE_CHARGED                     = ConvertItemType(1)
    constant itemtype ITEM_TYPE_POWERUP                     = ConvertItemType(2)
    constant itemtype ITEM_TYPE_ARTIFACT                    = ConvertItemType(3)
    constant itemtype ITEM_TYPE_PURCHASABLE                 = ConvertItemType(4)
    constant itemtype ITEM_TYPE_CAMPAIGN                    = ConvertItemType(5)
    constant itemtype ITEM_TYPE_MISCELLANEOUS               = ConvertItemType(6)
    constant itemtype ITEM_TYPE_UNKNOWN                     = ConvertItemType(7)
    constant itemtype ITEM_TYPE_ANY                         = ConvertItemType(8)

    // Deprecated, should use ITEM_TYPE_POWERUP
    constant itemtype ITEM_TYPE_TOME                        = ConvertItemType(2)

//===================================================
// Animatable Camera Fields
//===================================================

    constant camerafield CAMERA_FIELD_TARGET_DISTANCE       = ConvertCameraField(0)
    constant camerafield CAMERA_FIELD_FARZ                  = ConvertCameraField(1)
    constant camerafield CAMERA_FIELD_ANGLE_OF_ATTACK       = ConvertCameraField(2)
    constant camerafield CAMERA_FIELD_FIELD_OF_VIEW         = ConvertCameraField(3)
    constant camerafield CAMERA_FIELD_ROLL                  = ConvertCameraField(4)
    constant camerafield CAMERA_FIELD_ROTATION              = ConvertCameraField(5)
    constant camerafield CAMERA_FIELD_ZOFFSET               = ConvertCameraField(6)

    constant blendmode   BLEND_MODE_NONE                    = ConvertBlendMode(0)
    constant blendmode   BLEND_MODE_DONT_CARE               = ConvertBlendMode(0)
    constant blendmode   BLEND_MODE_KEYALPHA                = ConvertBlendMode(1)
    constant blendmode   BLEND_MODE_BLEND                   = ConvertBlendMode(2)
    constant blendmode   BLEND_MODE_ADDITIVE                = ConvertBlendMode(3)
    constant blendmode   BLEND_MODE_MODULATE                = ConvertBlendMode(4)
    constant blendmode   BLEND_MODE_MODULATE_2X             = ConvertBlendMode(5)
    
    constant raritycontrol  RARITY_FREQUENT                 = ConvertRarityControl(0)
    constant raritycontrol  RARITY_RARE                     = ConvertRarityControl(1)

    constant texmapflags    TEXMAP_FLAG_NONE                = ConvertTexMapFlags(0)
    constant texmapflags    TEXMAP_FLAG_WRAP_U              = ConvertTexMapFlags(1)
    constant texmapflags    TEXMAP_FLAG_WRAP_V              = ConvertTexMapFlags(2)
    constant texmapflags    TEXMAP_FLAG_WRAP_UV             = ConvertTexMapFlags(3)

    constant fogstate       FOG_OF_WAR_MASKED               = ConvertFogState(1)
    constant fogstate       FOG_OF_WAR_FOGGED               = ConvertFogState(2)
    constant fogstate       FOG_OF_WAR_VISIBLE              = ConvertFogState(4)

//===================================================
// Camera Margin constants for use with GetCameraMargin
//===================================================

    constant integer        CAMERA_MARGIN_LEFT              = 0
    constant integer        CAMERA_MARGIN_RIGHT             = 1
    constant integer        CAMERA_MARGIN_TOP               = 2
    constant integer        CAMERA_MARGIN_BOTTOM            = 3

//===================================================
// Effect API constants
//===================================================

    constant effecttype     EFFECT_TYPE_EFFECT              = ConvertEffectType(0)
    constant effecttype     EFFECT_TYPE_TARGET              = ConvertEffectType(1)
    constant effecttype     EFFECT_TYPE_CASTER              = ConvertEffectType(2)
    constant effecttype     EFFECT_TYPE_SPECIAL             = ConvertEffectType(3)
    constant effecttype     EFFECT_TYPE_AREA_EFFECT         = ConvertEffectType(4)
    constant effecttype     EFFECT_TYPE_MISSILE             = ConvertEffectType(5)
    constant effecttype     EFFECT_TYPE_LIGHTNING           = ConvertEffectType(6)

    constant soundtype      SOUND_TYPE_EFFECT               = ConvertSoundType(0)
    constant soundtype      SOUND_TYPE_EFFECT_LOOPED        = ConvertSoundType(1)

endglobals

//============================================================================
// MathAPI
native Deg2Rad  takes real degrees returns real
native Rad2Deg  takes real radians returns real

native Sin      takes real radians returns real
native Cos      takes real radians returns real
native Tan      takes real radians returns real

// Expect values between -1 and 1...returns 0 for invalid input
native Asin     takes real y returns real
native Acos     takes real x returns real

native Atan     takes real x returns real

// Returns 0 if x and y are both 0
native Atan2    takes real y, real x returns real

// Returns 0 if x <= 0
native SquareRoot takes real x returns real

// computes x to the y power
// y == 0.0             => 1
// x ==0.0 and y < 0    => 0
//
native Pow      takes real x, real power returns real

//============================================================================
// String Utility API
native I2R  takes integer i returns real
native R2I  takes real r returns integer
native I2S  takes integer i returns string
native R2S  takes real r returns string
native R2SW takes real r, integer width, integer precision returns string
native S2I  takes string s returns integer
native S2R  takes string s returns real
native GetHandleId takes handle h returns integer
native SubString takes string source, integer start, integer end returns string
native StringLength takes string s returns integer
native StringCase takes string source, boolean upper returns string
native StringHash takes string s returns integer

native GetLocalizedString takes string source returns string
native GetLocalizedHotkey takes string source returns integer

//============================================================================
// Map Setup API
//
//  These are native functions for describing the map configuration
//  these funcs should only be used in the "config" function of
//  a map script. The functions should also be called in this order
//  ( i.e. call SetPlayers before SetPlayerColor...
//

native SetMapName           takes string name returns nothing
native SetMapDescription    takes string description returns nothing

native SetTeams             takes integer teamcount returns nothing
native SetPlayers           takes integer playercount returns nothing

native DefineStartLocation      takes integer whichStartLoc, real x, real y returns nothing
native DefineStartLocationLoc   takes integer whichStartLoc, location whichLocation returns nothing
native SetStartLocPrioCount     takes integer whichStartLoc, integer prioSlotCount returns nothing
native SetStartLocPrio          takes integer whichStartLoc, integer prioSlotIndex, integer otherStartLocIndex, startlocprio priority returns nothing
native GetStartLocPrioSlot      takes integer whichStartLoc, integer prioSlotIndex returns integer
native GetStartLocPrio          takes integer whichStartLoc, integer prioSlotIndex returns startlocprio

native SetGameTypeSupported takes gametype whichGameType, boolean value returns nothing
native SetMapFlag           takes mapflag whichMapFlag, boolean value returns nothing
native SetGamePlacement     takes placement whichPlacementType returns nothing
native SetGameSpeed         takes gamespeed whichspeed returns nothing
native SetGameDifficulty    takes gamedifficulty whichdifficulty returns nothing
native SetResourceDensity   takes mapdensity whichdensity returns nothing
native SetCreatureDensity   takes mapdensity whichdensity returns nothing

native GetTeams             takes nothing returns integer
native GetPlayers           takes nothing returns integer

native IsGameTypeSupported  takes gametype whichGameType returns boolean
native GetGameTypeSelected  takes nothing returns gametype
native IsMapFlagSet         takes mapflag whichMapFlag returns boolean

constant native GetGamePlacement     takes nothing returns placement
constant native GetGameSpeed         takes nothing returns gamespeed
constant native GetGameDifficulty    takes nothing returns gamedifficulty
constant native GetResourceDensity   takes nothing returns mapdensity
constant native GetCreatureDensity   takes nothing returns mapdensity
constant native GetStartLocationX    takes integer whichStartLocation returns real
constant native GetStartLocationY    takes integer whichStartLocation returns real
constant native GetStartLocationLoc  takes integer whichStartLocation returns location


native SetPlayerTeam            takes player whichPlayer, integer whichTeam returns nothing
native SetPlayerStartLocation   takes player whichPlayer, integer startLocIndex returns nothing
// forces player to have the specified start loc and marks the start loc as occupied
// which removes it from consideration for subsequently placed players
// ( i.e. you can use this to put people in a fixed loc and then
//   use random placement for any unplaced players etc )
native ForcePlayerStartLocation takes player whichPlayer, integer startLocIndex returns nothing 
native SetPlayerColor           takes player whichPlayer, playercolor color returns nothing
native SetPlayerAlliance        takes player sourcePlayer, player otherPlayer, alliancetype whichAllianceSetting, boolean value returns nothing
native SetPlayerTaxRate         takes player sourcePlayer, player otherPlayer, playerstate whichResource, integer rate returns nothing
native SetPlayerRacePreference  takes player whichPlayer, racepreference whichRacePreference returns nothing
native SetPlayerRaceSelectable  takes player whichPlayer, boolean value returns nothing
native SetPlayerController      takes player whichPlayer, mapcontrol controlType returns nothing
native SetPlayerName            takes player whichPlayer, string name returns nothing

native SetPlayerOnScoreScreen   takes player whichPlayer, boolean flag returns nothing

native GetPlayerTeam            takes player whichPlayer returns integer
native GetPlayerStartLocation   takes player whichPlayer returns integer
native GetPlayerColor           takes player whichPlayer returns playercolor
native GetPlayerSelectable      takes player whichPlayer returns boolean
native GetPlayerController      takes player whichPlayer returns mapcontrol
native GetPlayerSlotState       takes player whichPlayer returns playerslotstate
native GetPlayerTaxRate         takes player sourcePlayer, player otherPlayer, playerstate whichResource returns integer
native IsPlayerRacePrefSet      takes player whichPlayer, racepreference pref returns boolean
native GetPlayerName            takes player whichPlayer returns string

//============================================================================
// Timer API
//
native CreateTimer          takes nothing returns timer
native DestroyTimer         takes timer whichTimer returns nothing
native TimerStart           takes timer whichTimer, real timeout, boolean periodic, code handlerFunc returns nothing
native TimerGetElapsed      takes timer whichTimer returns real
native TimerGetRemaining    takes timer whichTimer returns real
native TimerGetTimeout      takes timer whichTimer returns real
native PauseTimer           takes timer whichTimer returns nothing
native ResumeTimer          takes timer whichTimer returns nothing
native GetExpiredTimer      takes nothing returns timer

//============================================================================
// Group API
//
native CreateGroup                          takes nothing returns group
native DestroyGroup                         takes group whichGroup returns nothing
native GroupAddUnit                         takes group whichGroup, unit whichUnit returns nothing
native GroupRemoveUnit                      takes group whichGroup, unit whichUnit returns nothing
native GroupClear                           takes group whichGroup returns nothing
native GroupEnumUnitsOfType                 takes group whichGroup, string unitname, boolexpr filter returns nothing
native GroupEnumUnitsOfPlayer               takes group whichGroup, player whichPlayer, boolexpr filter returns nothing
native GroupEnumUnitsOfTypeCounted          takes group whichGroup, string unitname, boolexpr filter, integer countLimit returns nothing
native GroupEnumUnitsInRect                 takes group whichGroup, rect r, boolexpr filter returns nothing
native GroupEnumUnitsInRectCounted          takes group whichGroup, rect r, boolexpr filter, integer countLimit returns nothing
native GroupEnumUnitsInRange                takes group whichGroup, real x, real y, real radius, boolexpr filter returns nothing
native GroupEnumUnitsInRangeOfLoc           takes group whichGroup, location whichLocation, real radius, boolexpr filter returns nothing
native GroupEnumUnitsInRangeCounted         takes group whichGroup, real x, real y, real radius, boolexpr filter, integer countLimit returns nothing
native GroupEnumUnitsInRangeOfLocCounted    takes group whichGroup, location whichLocation, real radius, boolexpr filter, integer countLimit returns nothing
native GroupEnumUnitsSelected               takes group whichGroup, player whichPlayer, boolexpr filter returns nothing

native GroupImmediateOrder                  takes group whichGroup, string order returns boolean
native GroupImmediateOrderById              takes group whichGroup, integer order returns boolean
native GroupPointOrder                      takes group whichGroup, string order, real x, real y returns boolean
native GroupPointOrderLoc                   takes group whichGroup, string order, location whichLocation returns boolean
native GroupPointOrderById                  takes group whichGroup, integer order, real x, real y returns boolean
native GroupPointOrderByIdLoc               takes group whichGroup, integer order, location whichLocation returns boolean
native GroupTargetOrder                     takes group whichGroup, string order, widget targetWidget returns boolean
native GroupTargetOrderById                 takes group whichGroup, integer order, widget targetWidget returns boolean

// This will be difficult to support with potentially disjoint, cell-based regions
// as it would involve enumerating all the cells that are covered by a particularregion
// a better implementation would be a trigger that adds relevant units as they enter
// and removes them if they leave...
native ForGroup                 takes group whichGroup, code callback returns nothing
native FirstOfGroup             takes group whichGroup returns unit

//============================================================================
// Force API
//
native CreateForce              takes nothing returns force
native DestroyForce             takes force whichForce returns nothing
native ForceAddPlayer           takes force whichForce, player whichPlayer returns nothing
native ForceRemovePlayer        takes force whichForce, player whichPlayer returns nothing
native ForceClear               takes force whichForce returns nothing
native ForceEnumPlayers         takes force whichForce, boolexpr filter returns nothing
native ForceEnumPlayersCounted  takes force whichForce, boolexpr filter, integer countLimit returns nothing
native ForceEnumAllies          takes force whichForce, player whichPlayer, boolexpr filter returns nothing
native ForceEnumEnemies         takes force whichForce, player whichPlayer, boolexpr filter returns nothing
native ForForce                 takes force whichForce, code callback returns nothing

//============================================================================
// Region and Location API
//
native Rect                     takes real minx, real miny, real maxx, real maxy returns rect
native RectFromLoc              takes location min, location max returns rect
native RemoveRect               takes rect whichRect returns nothing
native SetRect                  takes rect whichRect, real minx, real miny, real maxx, real maxy returns nothing
native SetRectFromLoc           takes rect whichRect, location min, location max returns nothing
native MoveRectTo               takes rect whichRect, real newCenterX, real newCenterY returns nothing
native MoveRectToLoc            takes rect whichRect, location newCenterLoc returns nothing

native GetRectCenterX           takes rect whichRect returns real
native GetRectCenterY           takes rect whichRect returns real
native GetRectMinX              takes rect whichRect returns real
native GetRectMinY              takes rect whichRect returns real
native GetRectMaxX              takes rect whichRect returns real
native GetRectMaxY              takes rect whichRect returns real

native CreateRegion             takes nothing returns region
native RemoveRegion             takes region whichRegion returns nothing

native RegionAddRect            takes region whichRegion, rect r returns nothing
native RegionClearRect          takes region whichRegion, rect r returns nothing

native RegionAddCell           takes region whichRegion, real x, real y returns nothing
native RegionAddCellAtLoc      takes region whichRegion, location whichLocation returns nothing
native RegionClearCell         takes region whichRegion, real x, real y returns nothing
native RegionClearCellAtLoc    takes region whichRegion, location whichLocation returns nothing

native Location                 takes real x, real y returns location
native RemoveLocation           takes location whichLocation returns nothing
native MoveLocation             takes location whichLocation, real newX, real newY returns nothing
native GetLocationX             takes location whichLocation returns real
native GetLocationY             takes location whichLocation returns real

// This function is asynchronous. The values it returns are not guaranteed synchronous between each player.
//  If you attempt to use it in a synchronous manner, it may cause a desync.
native GetLocationZ             takes location whichLocation returns real

native IsUnitInRegion               takes region whichRegion, unit whichUnit returns boolean
native IsPointInRegion              takes region whichRegion, real x, real y returns boolean
native IsLocationInRegion           takes region whichRegion, location whichLocation returns boolean

// Returns full map bounds, including unplayable borders, in world coordinates
native GetWorldBounds           takes nothing returns rect

//============================================================================
// Native trigger interface
//
native CreateTrigger    takes nothing returns trigger
native DestroyTrigger   takes trigger whichTrigger returns nothing
native ResetTrigger     takes trigger whichTrigger returns nothing
native EnableTrigger    takes trigger whichTrigger returns nothing
native DisableTrigger   takes trigger whichTrigger returns nothing
native IsTriggerEnabled takes trigger whichTrigger returns boolean

native TriggerWaitOnSleeps   takes trigger whichTrigger, boolean flag returns nothing
native IsTriggerWaitOnSleeps takes trigger whichTrigger returns boolean

constant native GetFilterUnit       takes nothing returns unit
constant native GetEnumUnit         takes nothing returns unit

constant native GetFilterDestructable   takes nothing returns destructable
constant native GetEnumDestructable     takes nothing returns destructable

constant native GetFilterItem           takes nothing returns item
constant native GetEnumItem             takes nothing returns item

constant native GetFilterPlayer     takes nothing returns player
constant native GetEnumPlayer       takes nothing returns player

constant native GetTriggeringTrigger    takes nothing returns trigger
constant native GetTriggerEventId       takes nothing returns eventid
constant native GetTriggerEvalCount     takes trigger whichTrigger returns integer
constant native GetTriggerExecCount     takes trigger whichTrigger returns integer

native ExecuteFunc          takes string funcName returns nothing

//============================================================================
// Boolean Expr API ( for compositing trigger conditions and unit filter funcs...)
//============================================================================
native And              takes boolexpr operandA, boolexpr operandB returns boolexpr
native Or               takes boolexpr operandA, boolexpr operandB returns boolexpr
native Not              takes boolexpr operand returns boolexpr
native Condition        takes code func returns conditionfunc
native DestroyCondition takes conditionfunc c returns nothing
native Filter           takes code func returns filterfunc
native DestroyFilter    takes filterfunc f returns nothing
native DestroyBoolExpr  takes boolexpr e returns nothing

//============================================================================
// Trigger Game Event API
//============================================================================

native TriggerRegisterVariableEvent takes trigger whichTrigger, string varName, limitop opcode, real limitval returns event

    // EVENT_GAME_VARIABLE_LIMIT
    //constant native string GetTriggeringVariableName takes nothing returns string

// Creates it's own timer and triggers when it expires
native TriggerRegisterTimerEvent takes trigger whichTrigger, real timeout, boolean periodic returns event

// Triggers when the timer you tell it about expires
native TriggerRegisterTimerExpireEvent takes trigger whichTrigger, timer t returns event

native TriggerRegisterGameStateEvent takes trigger whichTrigger, gamestate whichState, limitop opcode, real limitval returns event

native TriggerRegisterDialogEvent       takes trigger whichTrigger, dialog whichDialog returns event
native TriggerRegisterDialogButtonEvent takes trigger whichTrigger, button whichButton returns event

//  EVENT_GAME_STATE_LIMIT
constant native GetEventGameState takes nothing returns gamestate

native TriggerRegisterGameEvent takes trigger whichTrigger, gameevent whichGameEvent returns event
  
// EVENT_GAME_VICTORY
constant native GetWinningPlayer takes nothing returns player


native TriggerRegisterEnterRegion takes trigger whichTrigger, region whichRegion, boolexpr filter returns event

// EVENT_GAME_ENTER_REGION
constant native GetTriggeringRegion takes nothing returns region
constant native GetEnteringUnit takes nothing returns unit

// EVENT_GAME_LEAVE_REGION

native TriggerRegisterLeaveRegion takes trigger whichTrigger, region whichRegion, boolexpr filter returns event
constant native GetLeavingUnit takes nothing returns unit

native TriggerRegisterTrackableHitEvent takes trigger whichTrigger, trackable t returns event
native TriggerRegisterTrackableTrackEvent takes trigger whichTrigger, trackable t returns event

// EVENT_GAME_TRACKABLE_HIT
// EVENT_GAME_TRACKABLE_TRACK
constant native GetTriggeringTrackable takes nothing returns trackable

// EVENT_DIALOG_BUTTON_CLICK
constant native GetClickedButton takes nothing returns button
constant native GetClickedDialog    takes nothing returns dialog

// EVENT_GAME_TOURNAMENT_FINISH_SOON
constant native GetTournamentFinishSoonTimeRemaining takes nothing returns real
constant native GetTournamentFinishNowRule takes nothing returns integer
constant native GetTournamentFinishNowPlayer takes nothing returns player
constant native GetTournamentScore takes player whichPlayer returns integer

// EVENT_GAME_SAVE
constant native GetSaveBasicFilename takes nothing returns string

//============================================================================
// Trigger Player Based Event API
//============================================================================

native TriggerRegisterPlayerEvent takes trigger whichTrigger, player  whichPlayer, playerevent whichPlayerEvent returns event

// EVENT_PLAYER_DEFEAT
// EVENT_PLAYER_VICTORY
constant native GetTriggerPlayer takes nothing returns player

native TriggerRegisterPlayerUnitEvent takes trigger whichTrigger, player whichPlayer, playerunitevent whichPlayerUnitEvent, boolexpr filter returns event

// EVENT_PLAYER_HERO_LEVEL
// EVENT_UNIT_HERO_LEVEL
constant native GetLevelingUnit takes nothing returns unit

// EVENT_PLAYER_HERO_SKILL
// EVENT_UNIT_HERO_SKILL
constant native GetLearningUnit      takes nothing returns unit
constant native GetLearnedSkill      takes nothing returns integer
constant native GetLearnedSkillLevel takes nothing returns integer

// EVENT_PLAYER_HERO_REVIVABLE
constant native GetRevivableUnit takes nothing returns unit

// EVENT_PLAYER_HERO_REVIVE_START
// EVENT_PLAYER_HERO_REVIVE_CANCEL
// EVENT_PLAYER_HERO_REVIVE_FINISH
// EVENT_UNIT_HERO_REVIVE_START
// EVENT_UNIT_HERO_REVIVE_CANCEL
// EVENT_UNIT_HERO_REVIVE_FINISH
constant native GetRevivingUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_ATTACKED
constant native GetAttacker takes nothing returns unit

// EVENT_PLAYER_UNIT_RESCUED
constant native GetRescuer  takes nothing returns unit

// EVENT_PLAYER_UNIT_DEATH
constant native GetDyingUnit takes nothing returns unit
constant native GetKillingUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_DECAY
constant native GetDecayingUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_SELECTED
//constant native GetSelectedUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_CONSTRUCT_START
constant native GetConstructingStructure takes nothing returns unit

// EVENT_PLAYER_UNIT_CONSTRUCT_FINISH
// EVENT_PLAYER_UNIT_CONSTRUCT_CANCEL
constant native GetCancelledStructure takes nothing returns unit
constant native GetConstructedStructure takes nothing returns unit

// EVENT_PLAYER_UNIT_RESEARCH_START
// EVENT_PLAYER_UNIT_RESEARCH_CANCEL
// EVENT_PLAYER_UNIT_RESEARCH_FINISH
constant native GetResearchingUnit takes nothing returns unit
constant native GetResearched takes nothing returns integer

// EVENT_PLAYER_UNIT_TRAIN_START
// EVENT_PLAYER_UNIT_TRAIN_CANCEL
constant native GetTrainedUnitType takes nothing returns integer

// EVENT_PLAYER_UNIT_TRAIN_FINISH
constant native GetTrainedUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_DETECTED
constant native GetDetectedUnit takes nothing returns unit

// EVENT_PLAYER_UNIT_SUMMONED
constant native GetSummoningUnit    takes nothing returns unit
constant native GetSummonedUnit     takes nothing returns unit

// EVENT_PLAYER_UNIT_LOADED
constant native GetTransportUnit    takes nothing returns unit
constant native GetLoadedUnit       takes nothing returns unit

// EVENT_PLAYER_UNIT_SELL
constant native GetSellingUnit      takes nothing returns unit
constant native GetSoldUnit         takes nothing returns unit
constant native GetBuyingUnit       takes nothing returns unit

// EVENT_PLAYER_UNIT_SELL_ITEM
constant native GetSoldItem         takes nothing returns item

// EVENT_PLAYER_UNIT_CHANGE_OWNER
constant native GetChangingUnit             takes nothing returns unit
constant native GetChangingUnitPrevOwner    takes nothing returns player

// EVENT_PLAYER_UNIT_DROP_ITEM
// EVENT_PLAYER_UNIT_PICKUP_ITEM
// EVENT_PLAYER_UNIT_USE_ITEM
constant native GetManipulatingUnit takes nothing returns unit
constant native GetManipulatedItem  takes nothing returns item

// EVENT_PLAYER_UNIT_ISSUED_ORDER
constant native GetOrderedUnit takes nothing returns unit
constant native GetIssuedOrderId takes nothing returns integer

// EVENT_PLAYER_UNIT_ISSUED_POINT_ORDER
constant native GetOrderPointX takes nothing returns real
constant native GetOrderPointY takes nothing returns real
constant native GetOrderPointLoc takes nothing returns location

// EVENT_PLAYER_UNIT_ISSUED_TARGET_ORDER
constant native GetOrderTarget              takes nothing returns widget
constant native GetOrderTargetDestructable  takes nothing returns destructable
constant native GetOrderTargetItem          takes nothing returns item
constant native GetOrderTargetUnit          takes nothing returns unit

// EVENT_UNIT_SPELL_CHANNEL
// EVENT_UNIT_SPELL_CAST
// EVENT_UNIT_SPELL_EFFECT
// EVENT_UNIT_SPELL_FINISH
// EVENT_UNIT_SPELL_ENDCAST
// EVENT_PLAYER_UNIT_SPELL_CHANNEL
// EVENT_PLAYER_UNIT_SPELL_CAST
// EVENT_PLAYER_UNIT_SPELL_EFFECT
// EVENT_PLAYER_UNIT_SPELL_FINISH
// EVENT_PLAYER_UNIT_SPELL_ENDCAST
constant native GetSpellAbilityUnit         takes nothing returns unit
constant native GetSpellAbilityId           takes nothing returns integer
constant native GetSpellAbility             takes nothing returns ability
constant native GetSpellTargetLoc           takes nothing returns location
constant native GetSpellTargetX				takes nothing returns real
constant native GetSpellTargetY				takes nothing returns real
constant native GetSpellTargetDestructable  takes nothing returns destructable
constant native GetSpellTargetItem          takes nothing returns item
constant native GetSpellTargetUnit          takes nothing returns unit

native TriggerRegisterPlayerAllianceChange takes trigger whichTrigger, player whichPlayer, alliancetype whichAlliance returns event
native TriggerRegisterPlayerStateEvent takes trigger whichTrigger, player whichPlayer, playerstate whichState, limitop opcode, real limitval returns event

// EVENT_PLAYER_STATE_LIMIT
constant native GetEventPlayerState takes nothing returns playerstate

native TriggerRegisterPlayerChatEvent takes trigger whichTrigger, player whichPlayer, string chatMessageToDetect, boolean exactMatchOnly returns event

// EVENT_PLAYER_CHAT

// returns the actual string they typed in ( same as what you registered for
// if you required exact match )
constant native GetEventPlayerChatString takes nothing returns string

// returns the string that you registered for
constant native GetEventPlayerChatStringMatched takes nothing returns string

native TriggerRegisterDeathEvent takes trigger whichTrigger, widget whichWidget returns event

//============================================================================
// Trigger Unit Based Event API
//============================================================================

// returns handle to unit which triggered the most recent event when called from
// within a trigger action function...returns null handle when used incorrectly

constant native GetTriggerUnit takes nothing returns unit

native TriggerRegisterUnitStateEvent takes trigger whichTrigger, unit whichUnit, unitstate whichState, limitop opcode, real limitval returns event

// EVENT_UNIT_STATE_LIMIT
constant native GetEventUnitState takes nothing returns unitstate

native TriggerRegisterUnitEvent takes trigger whichTrigger, unit whichUnit, unitevent whichEvent returns event

// EVENT_UNIT_DAMAGED
constant native GetEventDamage takes nothing returns real
constant native GetEventDamageSource takes nothing returns unit

// EVENT_UNIT_DEATH
// EVENT_UNIT_DECAY
// Use the GetDyingUnit and GetDecayingUnit funcs above

// EVENT_UNIT_DETECTED 
constant native GetEventDetectingPlayer takes nothing returns player

native TriggerRegisterFilterUnitEvent takes trigger whichTrigger, unit whichUnit, unitevent whichEvent, boolexpr filter returns event

// EVENT_UNIT_ACQUIRED_TARGET
// EVENT_UNIT_TARGET_IN_RANGE
constant native GetEventTargetUnit takes nothing returns unit

// EVENT_UNIT_ATTACKED
// Use GetAttacker from the Player Unit Event API Below...

// EVENT_UNIT_RESCUEDED
// Use GetRescuer from the Player Unit Event API Below...

// EVENT_UNIT_CONSTRUCT_CANCEL
// EVENT_UNIT_CONSTRUCT_FINISH

// See the Player Unit Construction Event API above for event info funcs

// EVENT_UNIT_TRAIN_START
// EVENT_UNIT_TRAIN_CANCELLED
// EVENT_UNIT_TRAIN_FINISH

// See the Player Unit Training Event API above for event info funcs

// EVENT_UNIT_SELL

// See the Player Unit Sell Event API above for event info funcs

// EVENT_UNIT_DROP_ITEM
// EVENT_UNIT_PICKUP_ITEM
// EVENT_UNIT_USE_ITEM
// See the Player Unit/Item manipulation Event API above for event info funcs

// EVENT_UNIT_ISSUED_ORDER
// EVENT_UNIT_ISSUED_POINT_ORDER
// EVENT_UNIT_ISSUED_TARGET_ORDER

// See the Player Unit Order Event API above for event info funcs

native TriggerRegisterUnitInRange takes trigger whichTrigger, unit whichUnit, real range, boolexpr filter returns event

native TriggerAddCondition    takes trigger whichTrigger, boolexpr condition returns triggercondition
native TriggerRemoveCondition takes trigger whichTrigger, triggercondition whichCondition returns nothing
native TriggerClearConditions takes trigger whichTrigger returns nothing

native TriggerAddAction     takes trigger whichTrigger, code actionFunc returns triggeraction
native TriggerRemoveAction  takes trigger whichTrigger, triggeraction whichAction returns nothing
native TriggerClearActions  takes trigger whichTrigger returns nothing
native TriggerSleepAction   takes real timeout returns nothing
native TriggerWaitForSound  takes sound s, real offset returns nothing
native TriggerEvaluate      takes trigger whichTrigger returns boolean
native TriggerExecute       takes trigger whichTrigger returns nothing
native TriggerExecuteWait   takes trigger whichTrigger returns nothing
native TriggerSyncStart     takes nothing returns nothing
native TriggerSyncReady     takes nothing returns nothing

//============================================================================
// Widget API
native  GetWidgetLife   takes widget whichWidget returns real
native  SetWidgetLife   takes widget whichWidget, real newLife returns nothing
native  GetWidgetX      takes widget whichWidget returns real
native  GetWidgetY      takes widget whichWidget returns real
constant native GetTriggerWidget takes nothing returns widget

//============================================================================
// Destructable Object API
// Facing arguments are specified in degrees
native          CreateDestructable          takes integer objectid, real x, real y, real face, real scale, integer variation returns destructable
native          CreateDestructableZ         takes integer objectid, real x, real y, real z, real face, real scale, integer variation returns destructable
native          CreateDeadDestructable      takes integer objectid, real x, real y, real face, real scale, integer variation returns destructable
native          CreateDeadDestructableZ     takes integer objectid, real x, real y, real z, real face, real scale, integer variation returns destructable
native          RemoveDestructable          takes destructable d returns nothing
native          KillDestructable            takes destructable d returns nothing
native          SetDestructableInvulnerable takes destructable d, boolean flag returns nothing
native          IsDestructableInvulnerable  takes destructable d returns boolean
native          EnumDestructablesInRect     takes rect r, boolexpr filter, code actionFunc returns nothing
native          GetDestructableTypeId       takes destructable d returns integer
native          GetDestructableX            takes destructable d returns real
native          GetDestructableY            takes destructable d returns real
native          SetDestructableLife         takes destructable d, real life returns nothing
native          GetDestructableLife         takes destructable d returns real
native          SetDestructableMaxLife      takes destructable d, real max returns nothing
native          GetDestructableMaxLife      takes destructable d returns real
native          DestructableRestoreLife     takes destructable d, real life, boolean birth returns nothing
native          QueueDestructableAnimation  takes destructable d, string whichAnimation returns nothing
native          SetDestructableAnimation    takes destructable d, string whichAnimation returns nothing
native          SetDestructableAnimationSpeed takes destructable d, real speedFactor returns nothing
native          ShowDestructable            takes destructable d, boolean flag returns nothing
native          GetDestructableOccluderHeight takes destructable d returns real
native          SetDestructableOccluderHeight takes destructable d, real height returns nothing
native          GetDestructableName         takes destructable d returns string
constant native GetTriggerDestructable takes nothing returns destructable

//============================================================================
// Item API
native          CreateItem      takes integer itemid, real x, real y returns item
native          RemoveItem      takes item whichItem returns nothing
native          GetItemPlayer   takes item whichItem returns player
native          GetItemTypeId   takes item i returns integer
native          GetItemX        takes item i returns real
native          GetItemY        takes item i returns real
native          SetItemPosition takes item i, real x, real y returns nothing
native          SetItemDropOnDeath  takes item whichItem, boolean flag returns nothing
native          SetItemDroppable takes item i, boolean flag returns nothing
native          SetItemPawnable takes item i, boolean flag returns nothing
native          SetItemPlayer    takes item whichItem, player whichPlayer, boolean changeColor returns nothing
native          SetItemInvulnerable takes item whichItem, boolean flag returns nothing
native          IsItemInvulnerable  takes item whichItem returns boolean
native          SetItemVisible  takes item whichItem, boolean show returns nothing
native          IsItemVisible   takes item whichItem returns boolean
native          IsItemOwned     takes item whichItem returns boolean
native          IsItemPowerup   takes item whichItem returns boolean
native          IsItemSellable  takes item whichItem returns boolean
native          IsItemPawnable  takes item whichItem returns boolean
native          IsItemIdPowerup takes integer itemId returns boolean
native          IsItemIdSellable takes integer itemId returns boolean
native          IsItemIdPawnable takes integer itemId returns boolean
native          EnumItemsInRect     takes rect r, boolexpr filter, code actionFunc returns nothing
native          GetItemLevel    takes item whichItem returns integer
native          GetItemType     takes item whichItem returns itemtype
native          SetItemDropID   takes item whichItem, integer unitId returns nothing
constant native GetItemName     takes item whichItem returns string
native          GetItemCharges  takes item whichItem returns integer
native          SetItemCharges  takes item whichItem, integer charges returns nothing
native          GetItemUserData takes item whichItem returns integer
native          SetItemUserData takes item whichItem, integer data returns nothing

//============================================================================
// Unit API
// Facing arguments are specified in degrees
native          CreateUnit              takes player id, integer unitid, real x, real y, real face returns unit
native          CreateUnitByName        takes player whichPlayer, string unitname, real x, real y, real face returns unit
native          CreateUnitAtLoc         takes player id, integer unitid, location whichLocation, real face returns unit
native          CreateUnitAtLocByName   takes player id, string unitname, location whichLocation, real face returns unit
native          CreateCorpse            takes player whichPlayer, integer unitid, real x, real y, real face returns unit

native          KillUnit            takes unit whichUnit returns nothing
native          RemoveUnit          takes unit whichUnit returns nothing
native          ShowUnit            takes unit whichUnit, boolean show returns nothing

native          SetUnitState        takes unit whichUnit, unitstate whichUnitState, real newVal returns nothing
native          SetUnitX            takes unit whichUnit, real newX returns nothing
native          SetUnitY            takes unit whichUnit, real newY returns nothing
native          SetUnitPosition     takes unit whichUnit, real newX, real newY returns nothing
native          SetUnitPositionLoc  takes unit whichUnit, location whichLocation returns nothing
native          SetUnitFacing       takes unit whichUnit, real facingAngle returns nothing
native          SetUnitFacingTimed  takes unit whichUnit, real facingAngle, real duration returns nothing
native          SetUnitMoveSpeed    takes unit whichUnit, real newSpeed returns nothing
native          SetUnitFlyHeight    takes unit whichUnit, real newHeight, real rate returns nothing
native          SetUnitTurnSpeed    takes unit whichUnit, real newTurnSpeed returns nothing
native          SetUnitPropWindow   takes unit whichUnit, real newPropWindowAngle returns nothing
native          SetUnitAcquireRange takes unit whichUnit, real newAcquireRange returns nothing
native          SetUnitCreepGuard   takes unit whichUnit, boolean creepGuard returns nothing

native          GetUnitAcquireRange     takes unit whichUnit returns real
native          GetUnitTurnSpeed        takes unit whichUnit returns real
native          GetUnitPropWindow       takes unit whichUnit returns real
native          GetUnitFlyHeight        takes unit whichUnit returns real

native          GetUnitDefaultAcquireRange      takes unit whichUnit returns real
native          GetUnitDefaultTurnSpeed         takes unit whichUnit returns real
native          GetUnitDefaultPropWindow        takes unit whichUnit returns real
native          GetUnitDefaultFlyHeight         takes unit whichUnit returns real

native          SetUnitOwner        takes unit whichUnit, player whichPlayer, boolean changeColor returns nothing
native          SetUnitColor        takes unit whichUnit, playercolor whichColor returns nothing

native          SetUnitScale        takes unit whichUnit, real scaleX, real scaleY, real scaleZ returns nothing
native          SetUnitTimeScale    takes unit whichUnit, real timeScale returns nothing
native          SetUnitBlendTime    takes unit whichUnit, real blendTime returns nothing
native          SetUnitVertexColor  takes unit whichUnit, integer red, integer green, integer blue, integer alpha returns nothing

native          QueueUnitAnimation          takes unit whichUnit, string whichAnimation returns nothing
native          SetUnitAnimation            takes unit whichUnit, string whichAnimation returns nothing
native          SetUnitAnimationByIndex     takes unit whichUnit, integer whichAnimation returns nothing
native          SetUnitAnimationWithRarity  takes unit whichUnit, string whichAnimation, raritycontrol rarity returns nothing
native          AddUnitAnimationProperties  takes unit whichUnit, string animProperties, boolean add returns nothing

native          SetUnitLookAt       takes unit whichUnit, string whichBone, unit lookAtTarget, real offsetX, real offsetY, real offsetZ returns nothing
native          ResetUnitLookAt     takes unit whichUnit returns nothing

native          SetUnitRescuable    takes unit whichUnit, player byWhichPlayer, boolean flag returns nothing
native          SetUnitRescueRange  takes unit whichUnit, real range returns nothing

native          SetHeroStr          takes unit whichHero, integer newStr, boolean permanent returns nothing
native          SetHeroAgi          takes unit whichHero, integer newAgi, boolean permanent returns nothing
native          SetHeroInt          takes unit whichHero, integer newInt, boolean permanent returns nothing

native          GetHeroStr          takes unit whichHero, boolean includeBonuses returns integer
native          GetHeroAgi          takes unit whichHero, boolean includeBonuses returns integer
native          GetHeroInt          takes unit whichHero, boolean includeBonuses returns integer

native          UnitStripHeroLevel  takes unit whichHero, integer howManyLevels returns boolean

native          GetHeroXP           takes unit whichHero returns integer
native          SetHeroXP           takes unit whichHero, integer newXpVal,  boolean showEyeCandy returns nothing

native          GetHeroSkillPoints      takes unit whichHero returns integer
native          UnitModifySkillPoints   takes unit whichHero, integer skillPointDelta returns boolean

native          AddHeroXP           takes unit whichHero, integer xpToAdd,   boolean showEyeCandy returns nothing
native          SetHeroLevel        takes unit whichHero, integer level,  boolean showEyeCandy returns nothing
constant native GetHeroLevel        takes unit whichHero returns integer
constant native GetUnitLevel        takes unit whichUnit returns integer
native          GetHeroProperName   takes unit whichHero returns string
native          SuspendHeroXP       takes unit whichHero, boolean flag returns nothing
native          IsSuspendedXP       takes unit whichHero returns boolean
native          SelectHeroSkill     takes unit whichHero, integer abilcode returns nothing
native          GetUnitAbilityLevel takes unit whichUnit, integer abilcode returns integer
native          DecUnitAbilityLevel takes unit whichUnit, integer abilcode returns integer
native          IncUnitAbilityLevel takes unit whichUnit, integer abilcode returns integer
native          SetUnitAbilityLevel takes unit whichUnit, integer abilcode, integer level returns integer
native          ReviveHero          takes unit whichHero, real x, real y, boolean doEyecandy returns boolean
native          ReviveHeroLoc       takes unit whichHero, location loc, boolean doEyecandy returns boolean
native          SetUnitExploded     takes unit whichUnit, boolean exploded returns nothing
native          SetUnitInvulnerable takes unit whichUnit, boolean flag returns nothing
native          PauseUnit           takes unit whichUnit, boolean flag returns nothing
native          IsUnitPaused        takes unit whichHero returns boolean
native          SetUnitPathing      takes unit whichUnit, boolean flag returns nothing

native          ClearSelection      takes nothing returns nothing
native          SelectUnit          takes unit whichUnit, boolean flag returns nothing

native          GetUnitPointValue       takes unit whichUnit returns integer
native          GetUnitPointValueByType takes integer unitType returns integer
//native        SetUnitPointValueByType takes integer unitType, integer newPointValue returns nothing

native          UnitAddItem             takes unit whichUnit, item whichItem returns boolean
native          UnitAddItemById         takes unit whichUnit, integer itemId returns item
native          UnitAddItemToSlotById   takes unit whichUnit, integer itemId, integer itemSlot returns boolean
native          UnitRemoveItem          takes unit whichUnit, item whichItem returns nothing
native          UnitRemoveItemFromSlot  takes unit whichUnit, integer itemSlot returns item
native          UnitHasItem             takes unit whichUnit, item whichItem returns boolean
native          UnitItemInSlot          takes unit whichUnit, integer itemSlot returns item
native          UnitInventorySize       takes unit whichUnit returns integer

native          UnitDropItemPoint       takes unit whichUnit, item whichItem, real x, real y returns boolean
native          UnitDropItemSlot        takes unit whichUnit, item whichItem, integer slot returns boolean
native          UnitDropItemTarget      takes unit whichUnit, item whichItem, widget target returns boolean

native          UnitUseItem             takes unit whichUnit, item whichItem returns boolean
native          UnitUseItemPoint        takes unit whichUnit, item whichItem, real x, real y returns boolean
native          UnitUseItemTarget       takes unit whichUnit, item whichItem, widget target returns boolean

constant native GetUnitX            takes unit whichUnit returns real
constant native GetUnitY            takes unit whichUnit returns real
constant native GetUnitLoc          takes unit whichUnit returns location
constant native GetUnitFacing       takes unit whichUnit returns real
constant native GetUnitMoveSpeed    takes unit whichUnit returns real
constant native GetUnitDefaultMoveSpeed takes unit whichUnit returns real
constant native GetUnitState        takes unit whichUnit, unitstate whichUnitState returns real
constant native GetOwningPlayer     takes unit whichUnit returns player
constant native GetUnitTypeId       takes unit whichUnit returns integer
constant native GetUnitRace         takes unit whichUnit returns race
constant native GetUnitName         takes unit whichUnit returns string
constant native GetUnitFoodUsed     takes unit whichUnit returns integer
constant native GetUnitFoodMade     takes unit whichUnit returns integer
constant native GetFoodMade         takes integer unitId returns integer
constant native GetFoodUsed         takes integer unitId returns integer
native          SetUnitUseFood      takes unit whichUnit, boolean useFood returns nothing

constant native GetUnitRallyPoint           takes unit whichUnit returns location
constant native GetUnitRallyUnit            takes unit whichUnit returns unit
constant native GetUnitRallyDestructable    takes unit whichUnit returns destructable

constant native IsUnitInGroup       takes unit whichUnit, group whichGroup returns boolean
constant native IsUnitInForce       takes unit whichUnit, force whichForce returns boolean
constant native IsUnitOwnedByPlayer takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitAlly          takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitEnemy         takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitVisible       takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitDetected      takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitInvisible     takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitFogged        takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitMasked        takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitSelected      takes unit whichUnit, player whichPlayer returns boolean
constant native IsUnitRace          takes unit whichUnit, race whichRace returns boolean
constant native IsUnitType          takes unit whichUnit, unittype whichUnitType returns boolean
constant native IsUnit              takes unit whichUnit, unit whichSpecifiedUnit returns boolean
constant native IsUnitInRange       takes unit whichUnit, unit otherUnit, real distance returns boolean
constant native IsUnitInRangeXY     takes unit whichUnit, real x, real y, real distance returns boolean
constant native IsUnitInRangeLoc    takes unit whichUnit, location whichLocation, real distance returns boolean
constant native IsUnitHidden        takes unit whichUnit returns boolean
constant native IsUnitIllusion      takes unit whichUnit returns boolean

constant native IsUnitInTransport   takes unit whichUnit, unit whichTransport returns boolean
constant native IsUnitLoaded        takes unit whichUnit returns boolean

constant native IsHeroUnitId        takes integer unitId returns boolean
constant native IsUnitIdType        takes integer unitId, unittype whichUnitType returns boolean

native UnitShareVision              takes unit whichUnit, player whichPlayer, boolean share returns nothing
native UnitSuspendDecay             takes unit whichUnit, boolean suspend returns nothing
native UnitAddType                  takes unit whichUnit, unittype whichUnitType returns boolean
native UnitRemoveType               takes unit whichUnit, unittype whichUnitType returns boolean

native UnitAddAbility               takes unit whichUnit, integer abilityId returns boolean
native UnitRemoveAbility            takes unit whichUnit, integer abilityId returns boolean
native UnitMakeAbilityPermanent     takes unit whichUnit, boolean permanent, integer abilityId returns boolean
native UnitRemoveBuffs              takes unit whichUnit, boolean removePositive, boolean removeNegative returns nothing
native UnitRemoveBuffsEx            takes unit whichUnit, boolean removePositive, boolean removeNegative, boolean magic, boolean physical, boolean timedLife, boolean aura, boolean autoDispel returns nothing
native UnitHasBuffsEx               takes unit whichUnit, boolean removePositive, boolean removeNegative, boolean magic, boolean physical, boolean timedLife, boolean aura, boolean autoDispel returns boolean
native UnitCountBuffsEx             takes unit whichUnit, boolean removePositive, boolean removeNegative, boolean magic, boolean physical, boolean timedLife, boolean aura, boolean autoDispel returns integer
native UnitAddSleep                 takes unit whichUnit, boolean add returns nothing
native UnitCanSleep                 takes unit whichUnit returns boolean
native UnitAddSleepPerm             takes unit whichUnit, boolean add returns nothing
native UnitCanSleepPerm             takes unit whichUnit returns boolean
native UnitIsSleeping               takes unit whichUnit returns boolean
native UnitWakeUp                   takes unit whichUnit returns nothing
native UnitApplyTimedLife           takes unit whichUnit, integer buffId, real duration returns nothing
native UnitIgnoreAlarm              takes unit whichUnit, boolean flag returns boolean
native UnitIgnoreAlarmToggled       takes unit whichUnit returns boolean
native UnitResetCooldown            takes unit whichUnit returns nothing
native UnitSetConstructionProgress  takes unit whichUnit, integer constructionPercentage returns nothing
native UnitSetUpgradeProgress       takes unit whichUnit, integer upgradePercentage returns nothing
native UnitPauseTimedLife           takes unit whichUnit, boolean flag returns nothing
native UnitSetUsesAltIcon           takes unit whichUnit, boolean flag returns nothing

native UnitDamagePoint              takes unit whichUnit, real delay, real radius, real x, real y, real amount, boolean attack, boolean ranged, attacktype attackType, damagetype damageType, weapontype weaponType returns boolean
native UnitDamageTarget             takes unit whichUnit, widget target, real amount, boolean attack, boolean ranged, attacktype attackType, damagetype damageType, weapontype weaponType returns boolean

native IssueImmediateOrder          takes unit whichUnit, string order returns boolean
native IssueImmediateOrderById      takes unit whichUnit, integer order returns boolean
native IssuePointOrder              takes unit whichUnit, string order, real x, real y returns boolean
native IssuePointOrderLoc           takes unit whichUnit, string order, location whichLocation returns boolean
native IssuePointOrderById          takes unit whichUnit, integer order, real x, real y returns boolean
native IssuePointOrderByIdLoc       takes unit whichUnit, integer order, location whichLocation returns boolean
native IssueTargetOrder             takes unit whichUnit, string order, widget targetWidget returns boolean
native IssueTargetOrderById         takes unit whichUnit, integer order, widget targetWidget returns boolean
native IssueInstantPointOrder       takes unit whichUnit, string order, real x, real y, widget instantTargetWidget returns boolean
native IssueInstantPointOrderById   takes unit whichUnit, integer order, real x, real y, widget instantTargetWidget returns boolean
native IssueInstantTargetOrder      takes unit whichUnit, string order, widget targetWidget, widget instantTargetWidget returns boolean
native IssueInstantTargetOrderById  takes unit whichUnit, integer order, widget targetWidget, widget instantTargetWidget returns boolean
native IssueBuildOrder              takes unit whichPeon, string unitToBuild, real x, real y returns boolean
native IssueBuildOrderById          takes unit whichPeon, integer unitId, real x, real y returns boolean

native IssueNeutralImmediateOrder       takes player forWhichPlayer, unit neutralStructure, string unitToBuild returns boolean
native IssueNeutralImmediateOrderById   takes player forWhichPlayer,unit neutralStructure, integer unitId returns boolean
native IssueNeutralPointOrder           takes player forWhichPlayer,unit neutralStructure, string unitToBuild, real x, real y returns boolean
native IssueNeutralPointOrderById       takes player forWhichPlayer,unit neutralStructure, integer unitId, real x, real y returns boolean
native IssueNeutralTargetOrder          takes player forWhichPlayer,unit neutralStructure, string unitToBuild, widget target returns boolean
native IssueNeutralTargetOrderById      takes player forWhichPlayer,unit neutralStructure, integer unitId, widget target returns boolean

native GetUnitCurrentOrder          takes unit whichUnit returns integer

native SetResourceAmount            takes unit whichUnit, integer amount returns nothing
native AddResourceAmount            takes unit whichUnit, integer amount returns nothing
native GetResourceAmount            takes unit whichUnit returns integer

native WaygateGetDestinationX       takes unit waygate returns real
native WaygateGetDestinationY       takes unit waygate returns real
native WaygateSetDestination        takes unit waygate, real x, real y returns nothing
native WaygateActivate              takes unit waygate, boolean activate returns nothing
native WaygateIsActive              takes unit waygate returns boolean

native AddItemToAllStock            takes integer itemId, integer currentStock, integer stockMax returns nothing
native AddItemToStock               takes unit whichUnit, integer itemId, integer currentStock, integer stockMax returns nothing
native AddUnitToAllStock            takes integer unitId, integer currentStock, integer stockMax returns nothing
native AddUnitToStock               takes unit whichUnit, integer unitId, integer currentStock, integer stockMax returns nothing

native RemoveItemFromAllStock       takes integer itemId returns nothing
native RemoveItemFromStock          takes unit whichUnit, integer itemId returns nothing
native RemoveUnitFromAllStock       takes integer unitId returns nothing
native RemoveUnitFromStock          takes unit whichUnit, integer unitId returns nothing

native SetAllItemTypeSlots          takes integer slots returns nothing
native SetAllUnitTypeSlots          takes integer slots returns nothing
native SetItemTypeSlots             takes unit whichUnit, integer slots returns nothing
native SetUnitTypeSlots             takes unit whichUnit, integer slots returns nothing

native GetUnitUserData              takes unit whichUnit returns integer
native SetUnitUserData              takes unit whichUnit, integer data returns nothing

//============================================================================
// Player API
constant native Player              takes integer number returns player
constant native GetLocalPlayer      takes nothing returns player
constant native IsPlayerAlly        takes player whichPlayer, player otherPlayer returns boolean
constant native IsPlayerEnemy       takes player whichPlayer, player otherPlayer returns boolean
constant native IsPlayerInForce     takes player whichPlayer, force whichForce returns boolean
constant native IsPlayerObserver    takes player whichPlayer returns boolean
constant native IsVisibleToPlayer           takes real x, real y, player whichPlayer returns boolean
constant native IsLocationVisibleToPlayer   takes location whichLocation, player whichPlayer returns boolean
constant native IsFoggedToPlayer            takes real x, real y, player whichPlayer returns boolean
constant native IsLocationFoggedToPlayer    takes location whichLocation, player whichPlayer returns boolean
constant native IsMaskedToPlayer            takes real x, real y, player whichPlayer returns boolean
constant native IsLocationMaskedToPlayer    takes location whichLocation, player whichPlayer returns boolean

constant native GetPlayerRace           takes player whichPlayer returns race
constant native GetPlayerId             takes player whichPlayer returns integer
constant native GetPlayerUnitCount      takes player whichPlayer, boolean includeIncomplete returns integer
constant native GetPlayerTypedUnitCount takes player whichPlayer, string unitName, boolean includeIncomplete, boolean includeUpgrades returns integer
constant native GetPlayerStructureCount takes player whichPlayer, boolean includeIncomplete returns integer
constant native GetPlayerState          takes player whichPlayer, playerstate whichPlayerState returns integer
constant native GetPlayerScore          takes player whichPlayer, playerscore whichPlayerScore returns integer
constant native GetPlayerAlliance       takes player sourcePlayer, player otherPlayer, alliancetype whichAllianceSetting returns boolean

constant native GetPlayerHandicap       takes player whichPlayer returns real
constant native GetPlayerHandicapXP     takes player whichPlayer returns real
constant native SetPlayerHandicap       takes player whichPlayer, real handicap returns nothing
constant native SetPlayerHandicapXP     takes player whichPlayer, real handicap returns nothing

constant native SetPlayerTechMaxAllowed takes player whichPlayer, integer techid, integer maximum returns nothing
constant native GetPlayerTechMaxAllowed takes player whichPlayer, integer techid returns integer
constant native AddPlayerTechResearched takes player whichPlayer, integer techid, integer levels returns nothing
constant native SetPlayerTechResearched takes player whichPlayer, integer techid, integer setToLevel returns nothing
constant native GetPlayerTechResearched takes player whichPlayer, integer techid, boolean specificonly returns boolean
constant native GetPlayerTechCount      takes player whichPlayer, integer techid, boolean specificonly returns integer

native SetPlayerUnitsOwner takes player whichPlayer, integer newOwner returns nothing
native CripplePlayer takes player whichPlayer, force toWhichPlayers, boolean flag returns nothing

native SetPlayerAbilityAvailable        takes player whichPlayer, integer abilid, boolean avail returns nothing

native SetPlayerState   takes player whichPlayer, playerstate whichPlayerState, integer value returns nothing
native RemovePlayer     takes player whichPlayer, playergameresult gameResult returns nothing

// Used to store hero level data for the scorescreen
// before units are moved to neutral passive in melee games
//
native CachePlayerHeroData takes player whichPlayer returns nothing

//============================================================================
// Fog of War API
native  SetFogStateRect      takes player forWhichPlayer, fogstate whichState, rect where, boolean useSharedVision returns nothing
native  SetFogStateRadius    takes player forWhichPlayer, fogstate whichState, real centerx, real centerY, real radius, boolean useSharedVision returns nothing
native  SetFogStateRadiusLoc takes player forWhichPlayer, fogstate whichState, location center, real radius, boolean useSharedVision returns nothing
native  FogMaskEnable        takes boolean enable returns nothing
native  IsFogMaskEnabled     takes nothing returns boolean
native  FogEnable            takes boolean enable returns nothing
native  IsFogEnabled         takes nothing returns boolean

native CreateFogModifierRect        takes player forWhichPlayer, fogstate whichState, rect where, boolean useSharedVision, boolean afterUnits returns fogmodifier
native CreateFogModifierRadius      takes player forWhichPlayer, fogstate whichState, real centerx, real centerY, real radius, boolean useSharedVision, boolean afterUnits returns fogmodifier
native CreateFogModifierRadiusLoc   takes player forWhichPlayer, fogstate whichState, location center, real radius, boolean useSharedVision, boolean afterUnits returns fogmodifier
native DestroyFogModifier           takes fogmodifier whichFogModifier returns nothing
native FogModifierStart             takes fogmodifier whichFogModifier returns nothing
native FogModifierStop              takes fogmodifier whichFogModifier returns nothing

//============================================================================
// Game API
native VersionGet takes nothing returns version
native VersionCompatible takes version whichVersion returns boolean
native VersionSupported takes version whichVersion returns boolean

native EndGame takes boolean doScoreScreen returns nothing

// Async only!
native          ChangeLevel         takes string newLevel, boolean doScoreScreen returns nothing
native          RestartGame         takes boolean doScoreScreen returns nothing
native          ReloadGame          takes nothing returns nothing
// %%% SetCampaignMenuRace is deprecated.  It must remain to support
// old maps which use it, but all new maps should use SetCampaignMenuRaceEx
native          SetCampaignMenuRace takes race r returns nothing
native          SetCampaignMenuRaceEx takes integer campaignIndex returns nothing
native          ForceCampaignSelectScreen takes nothing returns nothing

native          LoadGame            takes string saveFileName, boolean doScoreScreen returns nothing
native          SaveGame            takes string saveFileName returns nothing
native          RenameSaveDirectory takes string sourceDirName, string destDirName returns boolean
native          RemoveSaveDirectory takes string sourceDirName returns boolean
native          CopySaveGame        takes string sourceSaveName, string destSaveName returns boolean
native          SaveGameExists      takes string saveName returns boolean
native          SyncSelections      takes nothing returns nothing
native          SetFloatGameState   takes fgamestate whichFloatGameState, real value returns nothing
constant native GetFloatGameState   takes fgamestate whichFloatGameState returns real
native          SetIntegerGameState takes igamestate whichIntegerGameState, integer value returns nothing
constant native GetIntegerGameState takes igamestate whichIntegerGameState returns integer


//============================================================================
// Campaign API
native  SetTutorialCleared      takes boolean cleared returns nothing
native  SetMissionAvailable     takes integer campaignNumber, integer missionNumber, boolean available returns nothing
native  SetCampaignAvailable    takes integer campaignNumber, boolean available  returns nothing
native  SetOpCinematicAvailable takes integer campaignNumber, boolean available  returns nothing
native  SetEdCinematicAvailable takes integer campaignNumber, boolean available  returns nothing
native  GetDefaultDifficulty    takes nothing returns gamedifficulty
native  SetDefaultDifficulty    takes gamedifficulty g returns nothing
native  SetCustomCampaignButtonVisible  takes integer whichButton, boolean visible returns nothing
native  GetCustomCampaignButtonVisible  takes integer whichButton returns boolean
native  DoNotSaveReplay         takes nothing returns nothing

//============================================================================
// Dialog API
native DialogCreate                 takes nothing returns dialog
native DialogDestroy                takes dialog whichDialog returns nothing
native DialogClear                  takes dialog whichDialog returns nothing
native DialogSetMessage             takes dialog whichDialog, string messageText returns nothing
native DialogAddButton              takes dialog whichDialog, string buttonText, integer hotkey returns button
native DialogAddQuitButton          takes dialog whichDialog, boolean doScoreScreen, string buttonText, integer hotkey returns button
native DialogDisplay                takes player whichPlayer, dialog whichDialog, boolean flag returns nothing

// Creates a new or reads in an existing game cache file stored
// in the current campaign profile dir
//
native  ReloadGameCachesFromDisk takes nothing returns boolean

native  InitGameCache    takes string campaignFile returns gamecache
native  SaveGameCache    takes gamecache whichCache returns boolean

native  StoreInteger					takes gamecache cache, string missionKey, string key, integer value returns nothing
native  StoreReal						takes gamecache cache, string missionKey, string key, real value returns nothing
native  StoreBoolean					takes gamecache cache, string missionKey, string key, boolean value returns nothing
native  StoreUnit						takes gamecache cache, string missionKey, string key, unit whichUnit returns boolean
native  StoreString						takes gamecache cache, string missionKey, string key, string value returns boolean

native SyncStoredInteger        takes gamecache cache, string missionKey, string key returns nothing
native SyncStoredReal           takes gamecache cache, string missionKey, string key returns nothing
native SyncStoredBoolean        takes gamecache cache, string missionKey, string key returns nothing
native SyncStoredUnit           takes gamecache cache, string missionKey, string key returns nothing
native SyncStoredString         takes gamecache cache, string missionKey, string key returns nothing

native  HaveStoredInteger					takes gamecache cache, string missionKey, string key returns boolean
native  HaveStoredReal						takes gamecache cache, string missionKey, string key returns boolean
native  HaveStoredBoolean					takes gamecache cache, string missionKey, string key returns boolean
native  HaveStoredUnit						takes gamecache cache, string missionKey, string key returns boolean
native  HaveStoredString					takes gamecache cache, string missionKey, string key returns boolean

native  FlushGameCache						takes gamecache cache returns nothing
native  FlushStoredMission					takes gamecache cache, string missionKey returns nothing
native  FlushStoredInteger					takes gamecache cache, string missionKey, string key returns nothing
native  FlushStoredReal						takes gamecache cache, string missionKey, string key returns nothing
native  FlushStoredBoolean					takes gamecache cache, string missionKey, string key returns nothing
native  FlushStoredUnit						takes gamecache cache, string missionKey, string key returns nothing
native  FlushStoredString					takes gamecache cache, string missionKey, string key returns nothing

// Will return 0 if the specified value's data is not found in the cache
native  GetStoredInteger				takes gamecache cache, string missionKey, string key returns integer
native  GetStoredReal					takes gamecache cache, string missionKey, string key returns real
native  GetStoredBoolean				takes gamecache cache, string missionKey, string key returns boolean
native  GetStoredString					takes gamecache cache, string missionKey, string key returns string
native  RestoreUnit						takes gamecache cache, string missionKey, string key, player forWhichPlayer, real x, real y, real facing returns unit


native  InitHashtable    takes nothing returns hashtable

native  SaveInteger						takes hashtable table, integer parentKey, integer childKey, integer value returns nothing
native  SaveReal						takes hashtable table, integer parentKey, integer childKey, real value returns nothing
native  SaveBoolean						takes hashtable table, integer parentKey, integer childKey, boolean value returns nothing
native  SaveStr							takes hashtable table, integer parentKey, integer childKey, string value returns boolean
native  SavePlayerHandle				takes hashtable table, integer parentKey, integer childKey, player whichPlayer returns boolean
native  SaveWidgetHandle				takes hashtable table, integer parentKey, integer childKey, widget whichWidget returns boolean
native  SaveDestructableHandle			takes hashtable table, integer parentKey, integer childKey, destructable whichDestructable returns boolean
native  SaveItemHandle					takes hashtable table, integer parentKey, integer childKey, item whichItem returns boolean
native  SaveUnitHandle					takes hashtable table, integer parentKey, integer childKey, unit whichUnit returns boolean
native  SaveAbilityHandle				takes hashtable table, integer parentKey, integer childKey, ability whichAbility returns boolean
native  SaveTimerHandle					takes hashtable table, integer parentKey, integer childKey, timer whichTimer returns boolean
native  SaveTriggerHandle				takes hashtable table, integer parentKey, integer childKey, trigger whichTrigger returns boolean
native  SaveTriggerConditionHandle		takes hashtable table, integer parentKey, integer childKey, triggercondition whichTriggercondition returns boolean
native  SaveTriggerActionHandle			takes hashtable table, integer parentKey, integer childKey, triggeraction whichTriggeraction returns boolean
native  SaveTriggerEventHandle			takes hashtable table, integer parentKey, integer childKey, event whichEvent returns boolean
native  SaveForceHandle					takes hashtable table, integer parentKey, integer childKey, force whichForce returns boolean
native  SaveGroupHandle					takes hashtable table, integer parentKey, integer childKey, group whichGroup returns boolean
native  SaveLocationHandle				takes hashtable table, integer parentKey, integer childKey, location whichLocation returns boolean
native  SaveRectHandle					takes hashtable table, integer parentKey, integer childKey, rect whichRect returns boolean
native  SaveBooleanExprHandle			takes hashtable table, integer parentKey, integer childKey, boolexpr whichBoolexpr returns boolean
native  SaveSoundHandle					takes hashtable table, integer parentKey, integer childKey, sound whichSound returns boolean
native  SaveEffectHandle				takes hashtable table, integer parentKey, integer childKey, effect whichEffect returns boolean
native  SaveUnitPoolHandle				takes hashtable table, integer parentKey, integer childKey, unitpool whichUnitpool returns boolean
native  SaveItemPoolHandle				takes hashtable table, integer parentKey, integer childKey, itempool whichItempool returns boolean
native  SaveQuestHandle					takes hashtable table, integer parentKey, integer childKey, quest whichQuest returns boolean
native  SaveQuestItemHandle				takes hashtable table, integer parentKey, integer childKey, questitem whichQuestitem returns boolean
native  SaveDefeatConditionHandle		takes hashtable table, integer parentKey, integer childKey, defeatcondition whichDefeatcondition returns boolean
native  SaveTimerDialogHandle			takes hashtable table, integer parentKey, integer childKey, timerdialog whichTimerdialog returns boolean
native  SaveLeaderboardHandle			takes hashtable table, integer parentKey, integer childKey, leaderboard whichLeaderboard returns boolean
native  SaveMultiboardHandle			takes hashtable table, integer parentKey, integer childKey, multiboard whichMultiboard returns boolean
native  SaveMultiboardItemHandle		takes hashtable table, integer parentKey, integer childKey, multiboarditem whichMultiboarditem returns boolean
native  SaveTrackableHandle				takes hashtable table, integer parentKey, integer childKey, trackable whichTrackable returns boolean
native  SaveDialogHandle				takes hashtable table, integer parentKey, integer childKey, dialog whichDialog returns boolean
native  SaveButtonHandle				takes hashtable table, integer parentKey, integer childKey, button whichButton returns boolean
native  SaveTextTagHandle				takes hashtable table, integer parentKey, integer childKey, texttag whichTexttag returns boolean
native  SaveLightningHandle				takes hashtable table, integer parentKey, integer childKey, lightning whichLightning returns boolean
native  SaveImageHandle					takes hashtable table, integer parentKey, integer childKey, image whichImage returns boolean
native  SaveUbersplatHandle				takes hashtable table, integer parentKey, integer childKey, ubersplat whichUbersplat returns boolean
native  SaveRegionHandle				takes hashtable table, integer parentKey, integer childKey, region whichRegion returns boolean
native  SaveFogStateHandle				takes hashtable table, integer parentKey, integer childKey, fogstate whichFogState returns boolean
native  SaveFogModifierHandle			takes hashtable table, integer parentKey, integer childKey, fogmodifier whichFogModifier returns boolean
native  SaveAgentHandle					takes hashtable table, integer parentKey, integer childKey, agent whichAgent returns boolean
native  SaveHashtableHandle				takes hashtable table, integer parentKey, integer childKey, hashtable whichHashtable returns boolean


native  LoadInteger					takes hashtable table, integer parentKey, integer childKey returns integer
native  LoadReal					takes hashtable table, integer parentKey, integer childKey returns real
native  LoadBoolean				    takes hashtable table, integer parentKey, integer childKey returns boolean
native  LoadStr 					takes hashtable table, integer parentKey, integer childKey returns string
native  LoadPlayerHandle			takes hashtable table, integer parentKey, integer childKey returns player
native  LoadWidgetHandle			takes hashtable table, integer parentKey, integer childKey returns widget
native  LoadDestructableHandle		takes hashtable table, integer parentKey, integer childKey returns destructable
native  LoadItemHandle				takes hashtable table, integer parentKey, integer childKey returns item
native  LoadUnitHandle				takes hashtable table, integer parentKey, integer childKey returns unit
native  LoadAbilityHandle			takes hashtable table, integer parentKey, integer childKey returns ability
native  LoadTimerHandle				takes hashtable table, integer parentKey, integer childKey returns timer
native  LoadTriggerHandle			takes hashtable table, integer parentKey, integer childKey returns trigger
native  LoadTriggerConditionHandle	takes hashtable table, integer parentKey, integer childKey returns triggercondition
native  LoadTriggerActionHandle		takes hashtable table, integer parentKey, integer childKey returns triggeraction
native  LoadTriggerEventHandle		takes hashtable table, integer parentKey, integer childKey returns event
native  LoadForceHandle				takes hashtable table, integer parentKey, integer childKey returns force
native  LoadGroupHandle				takes hashtable table, integer parentKey, integer childKey returns group
native  LoadLocationHandle			takes hashtable table, integer parentKey, integer childKey returns location
native  LoadRectHandle				takes hashtable table, integer parentKey, integer childKey returns rect
native  LoadBooleanExprHandle		takes hashtable table, integer parentKey, integer childKey returns boolexpr
native  LoadSoundHandle				takes hashtable table, integer parentKey, integer childKey returns sound
native  LoadEffectHandle			takes hashtable table, integer parentKey, integer childKey returns effect
native  LoadUnitPoolHandle			takes hashtable table, integer parentKey, integer childKey returns unitpool
native  LoadItemPoolHandle			takes hashtable table, integer parentKey, integer childKey returns itempool
native  LoadQuestHandle				takes hashtable table, integer parentKey, integer childKey returns quest
native  LoadQuestItemHandle			takes hashtable table, integer parentKey, integer childKey returns questitem
native  LoadDefeatConditionHandle	takes hashtable table, integer parentKey, integer childKey returns defeatcondition
native  LoadTimerDialogHandle		takes hashtable table, integer parentKey, integer childKey returns timerdialog
native  LoadLeaderboardHandle		takes hashtable table, integer parentKey, integer childKey returns leaderboard
native  LoadMultiboardHandle		takes hashtable table, integer parentKey, integer childKey returns multiboard
native  LoadMultiboardItemHandle	takes hashtable table, integer parentKey, integer childKey returns multiboarditem
native  LoadTrackableHandle			takes hashtable table, integer parentKey, integer childKey returns trackable
native  LoadDialogHandle			takes hashtable table, integer parentKey, integer childKey returns dialog
native  LoadButtonHandle			takes hashtable table, integer parentKey, integer childKey returns button
native  LoadTextTagHandle			takes hashtable table, integer parentKey, integer childKey returns texttag
native  LoadLightningHandle			takes hashtable table, integer parentKey, integer childKey returns lightning
native  LoadImageHandle				takes hashtable table, integer parentKey, integer childKey returns image
native  LoadUbersplatHandle			takes hashtable table, integer parentKey, integer childKey returns ubersplat
native  LoadRegionHandle			takes hashtable table, integer parentKey, integer childKey returns region
native  LoadFogStateHandle			takes hashtable table, integer parentKey, integer childKey returns fogstate
native  LoadFogModifierHandle		takes hashtable table, integer parentKey, integer childKey returns fogmodifier
native  LoadHashtableHandle			takes hashtable table, integer parentKey, integer childKey returns hashtable

native  HaveSavedInteger					takes hashtable table, integer parentKey, integer childKey returns boolean
native  HaveSavedReal						takes hashtable table, integer parentKey, integer childKey returns boolean
native  HaveSavedBoolean					takes hashtable table, integer parentKey, integer childKey returns boolean
native  HaveSavedString					    takes hashtable table, integer parentKey, integer childKey returns boolean
native  HaveSavedHandle     				takes hashtable table, integer parentKey, integer childKey returns boolean

native  RemoveSavedInteger					takes hashtable table, integer parentKey, integer childKey returns nothing
native  RemoveSavedReal						takes hashtable table, integer parentKey, integer childKey returns nothing
native  RemoveSavedBoolean					takes hashtable table, integer parentKey, integer childKey returns nothing
native  RemoveSavedString					takes hashtable table, integer parentKey, integer childKey returns nothing
native  RemoveSavedHandle					takes hashtable table, integer parentKey, integer childKey returns nothing

native  FlushParentHashtable						takes hashtable table returns nothing
native  FlushChildHashtable					takes hashtable table, integer parentKey returns nothing


//============================================================================
// Randomization API
native GetRandomInt takes integer lowBound, integer highBound returns integer
native GetRandomReal takes real lowBound, real highBound returns real

native CreateUnitPool           takes nothing returns unitpool
native DestroyUnitPool          takes unitpool whichPool returns nothing
native UnitPoolAddUnitType      takes unitpool whichPool, integer unitId, real weight returns nothing
native UnitPoolRemoveUnitType   takes unitpool whichPool, integer unitId returns nothing
native PlaceRandomUnit          takes unitpool whichPool, player forWhichPlayer, real x, real y, real facing returns unit

native CreateItemPool           takes nothing returns itempool
native DestroyItemPool          takes itempool whichItemPool returns nothing
native ItemPoolAddItemType      takes itempool whichItemPool, integer itemId, real weight returns nothing
native ItemPoolRemoveItemType   takes itempool whichItemPool, integer itemId returns nothing
native PlaceRandomItem          takes itempool whichItemPool, real x, real y returns item

// Choose any random unit/item. (NP means Neutral Passive)
native ChooseRandomCreep        takes integer level returns integer
native ChooseRandomNPBuilding   takes nothing returns integer
native ChooseRandomItem         takes integer level returns integer
native ChooseRandomItemEx       takes itemtype whichType, integer level returns integer
native SetRandomSeed            takes integer seed returns nothing

//============================================================================
// Visual API
native SetTerrainFog                takes real a, real b, real c, real d, real e returns nothing
native ResetTerrainFog              takes nothing returns nothing

native SetUnitFog                   takes real a, real b, real c, real d, real e returns nothing
native SetTerrainFogEx              takes integer style, real zstart, real zend, real density, real red, real green, real blue returns nothing
native DisplayTextToPlayer          takes player toPlayer, real x, real y, string message returns nothing
native DisplayTimedTextToPlayer     takes player toPlayer, real x, real y, real duration, string message returns nothing
native DisplayTimedTextFromPlayer   takes player toPlayer, real x, real y, real duration, string message returns nothing
native ClearTextMessages            takes nothing returns nothing
native SetDayNightModels            takes string terrainDNCFile, string unitDNCFile returns nothing
native SetSkyModel                  takes string skyModelFile returns nothing
native EnableUserControl            takes boolean b returns nothing
native EnableUserUI                 takes boolean b returns nothing
native SuspendTimeOfDay             takes boolean b returns nothing
native SetTimeOfDayScale            takes real r returns nothing
native GetTimeOfDayScale            takes nothing returns real
native ShowInterface                takes boolean flag, real fadeDuration returns nothing
native PauseGame                    takes boolean flag returns nothing
native UnitAddIndicator             takes unit whichUnit, integer red, integer green, integer blue, integer alpha returns nothing
native AddIndicator                 takes widget whichWidget, integer red, integer green, integer blue, integer alpha returns nothing
native PingMinimap                  takes real x, real y, real duration returns nothing
native PingMinimapEx                takes real x, real y, real duration, integer red, integer green, integer blue, boolean extraEffects returns nothing
native EnableOcclusion              takes boolean flag returns nothing
native SetIntroShotText             takes string introText returns nothing
native SetIntroShotModel            takes string introModelPath returns nothing
native EnableWorldFogBoundary       takes boolean b returns nothing
native PlayModelCinematic           takes string modelName returns nothing
native PlayCinematic                takes string movieName returns nothing
native ForceUIKey                   takes string key returns nothing
native ForceUICancel                takes nothing returns nothing
native DisplayLoadDialog            takes nothing returns nothing
native SetAltMinimapIcon            takes string iconPath returns nothing
native DisableRestartMission        takes boolean flag returns nothing

native CreateTextTag                takes nothing returns texttag
native DestroyTextTag               takes texttag t returns nothing
native SetTextTagText               takes texttag t, string s, real height returns nothing
native SetTextTagPos                takes texttag t, real x, real y, real heightOffset returns nothing
native SetTextTagPosUnit            takes texttag t, unit whichUnit, real heightOffset returns nothing
native SetTextTagColor              takes texttag t, integer red, integer green, integer blue, integer alpha returns nothing
native SetTextTagVelocity           takes texttag t, real xvel, real yvel returns nothing
native SetTextTagVisibility         takes texttag t, boolean flag returns nothing
native SetTextTagSuspended          takes texttag t, boolean flag returns nothing
native SetTextTagPermanent          takes texttag t, boolean flag returns nothing
native SetTextTagAge                takes texttag t, real age returns nothing
native SetTextTagLifespan           takes texttag t, real lifespan returns nothing
native SetTextTagFadepoint          takes texttag t, real fadepoint returns nothing

native SetReservedLocalHeroButtons  takes integer reserved returns nothing
native GetAllyColorFilterState      takes nothing returns integer
native SetAllyColorFilterState      takes integer state returns nothing
native GetCreepCampFilterState      takes nothing returns boolean
native SetCreepCampFilterState      takes boolean state returns nothing
native EnableMinimapFilterButtons   takes boolean enableAlly, boolean enableCreep returns nothing
native EnableDragSelect             takes boolean state, boolean ui returns nothing
native EnablePreSelect              takes boolean state, boolean ui returns nothing
native EnableSelect                 takes boolean state, boolean ui returns nothing

//============================================================================
// Trackable API
native CreateTrackable      takes string trackableModelPath, real x, real y, real facing returns trackable

//============================================================================
// Quest API
native CreateQuest          takes nothing returns quest
native DestroyQuest         takes quest whichQuest returns nothing
native QuestSetTitle        takes quest whichQuest, string title returns nothing
native QuestSetDescription  takes quest whichQuest, string description returns nothing
native QuestSetIconPath     takes quest whichQuest, string iconPath returns nothing

native QuestSetRequired     takes quest whichQuest, boolean required   returns nothing
native QuestSetCompleted    takes quest whichQuest, boolean completed  returns nothing
native QuestSetDiscovered   takes quest whichQuest, boolean discovered returns nothing
native QuestSetFailed       takes quest whichQuest, boolean failed     returns nothing
native QuestSetEnabled      takes quest whichQuest, boolean enabled    returns nothing
    
native IsQuestRequired     takes quest whichQuest returns boolean
native IsQuestCompleted    takes quest whichQuest returns boolean
native IsQuestDiscovered   takes quest whichQuest returns boolean
native IsQuestFailed       takes quest whichQuest returns boolean
native IsQuestEnabled      takes quest whichQuest returns boolean

native QuestCreateItem          takes quest whichQuest returns questitem
native QuestItemSetDescription  takes questitem whichQuestItem, string description returns nothing
native QuestItemSetCompleted    takes questitem whichQuestItem, boolean completed returns nothing

native IsQuestItemCompleted     takes questitem whichQuestItem returns boolean

native CreateDefeatCondition            takes nothing returns defeatcondition
native DestroyDefeatCondition           takes defeatcondition whichCondition returns nothing
native DefeatConditionSetDescription    takes defeatcondition whichCondition, string description returns nothing

native FlashQuestDialogButton   takes nothing returns nothing
native ForceQuestDialogUpdate   takes nothing returns nothing

//============================================================================
// Timer Dialog API
native CreateTimerDialog                takes timer t returns timerdialog
native DestroyTimerDialog               takes timerdialog whichDialog returns nothing
native TimerDialogSetTitle              takes timerdialog whichDialog, string title returns nothing
native TimerDialogSetTitleColor         takes timerdialog whichDialog, integer red, integer green, integer blue, integer alpha returns nothing
native TimerDialogSetTimeColor          takes timerdialog whichDialog, integer red, integer green, integer blue, integer alpha returns nothing
native TimerDialogSetSpeed              takes timerdialog whichDialog, real speedMultFactor returns nothing
native TimerDialogDisplay               takes timerdialog whichDialog, boolean display returns nothing
native IsTimerDialogDisplayed           takes timerdialog whichDialog returns boolean
native TimerDialogSetRealTimeRemaining  takes timerdialog whichDialog, real timeRemaining returns nothing

//============================================================================
// Leaderboard API

// Create a leaderboard object
native CreateLeaderboard                takes nothing returns leaderboard
native DestroyLeaderboard               takes leaderboard lb returns nothing

native LeaderboardDisplay               takes leaderboard lb, boolean show returns nothing
native IsLeaderboardDisplayed           takes leaderboard lb returns boolean

native LeaderboardGetItemCount          takes leaderboard lb returns integer

native LeaderboardSetSizeByItemCount    takes leaderboard lb, integer count returns nothing
native LeaderboardAddItem               takes leaderboard lb, string label, integer value, player p returns nothing
native LeaderboardRemoveItem            takes leaderboard lb, integer index returns nothing
native LeaderboardRemovePlayerItem      takes leaderboard lb, player p returns nothing
native LeaderboardClear                 takes leaderboard lb returns nothing

native LeaderboardSortItemsByValue      takes leaderboard lb, boolean ascending returns nothing
native LeaderboardSortItemsByPlayer     takes leaderboard lb, boolean ascending returns nothing
native LeaderboardSortItemsByLabel      takes leaderboard lb, boolean ascending returns nothing

native LeaderboardHasPlayerItem         takes leaderboard lb, player p returns boolean
native LeaderboardGetPlayerIndex        takes leaderboard lb, player p returns integer
native LeaderboardSetLabel              takes leaderboard lb, string label returns nothing
native LeaderboardGetLabelText          takes leaderboard lb returns string

native PlayerSetLeaderboard             takes player toPlayer, leaderboard lb returns nothing
native PlayerGetLeaderboard             takes player toPlayer returns leaderboard

native LeaderboardSetLabelColor         takes leaderboard lb, integer red, integer green, integer blue, integer alpha returns nothing
native LeaderboardSetValueColor         takes leaderboard lb, integer red, integer green, integer blue, integer alpha returns nothing
native LeaderboardSetStyle              takes leaderboard lb, boolean showLabel, boolean showNames, boolean showValues, boolean showIcons returns nothing

native LeaderboardSetItemValue          takes leaderboard lb, integer whichItem, integer val returns nothing
native LeaderboardSetItemLabel          takes leaderboard lb, integer whichItem, string val returns nothing
native LeaderboardSetItemStyle          takes leaderboard lb, integer whichItem, boolean showLabel, boolean showValue, boolean showIcon returns nothing
native LeaderboardSetItemLabelColor     takes leaderboard lb, integer whichItem, integer red, integer green, integer blue, integer alpha returns nothing
native LeaderboardSetItemValueColor     takes leaderboard lb, integer whichItem, integer red, integer green, integer blue, integer alpha returns nothing

//============================================================================
// Multiboard API
//============================================================================

// Create a multiboard object
native CreateMultiboard                 takes nothing returns multiboard
native DestroyMultiboard                takes multiboard lb returns nothing

native MultiboardDisplay                takes multiboard lb, boolean show returns nothing
native IsMultiboardDisplayed            takes multiboard lb returns boolean

native MultiboardMinimize               takes multiboard lb, boolean minimize returns nothing
native IsMultiboardMinimized            takes multiboard lb returns boolean
native MultiboardClear                  takes multiboard lb returns nothing

native MultiboardSetTitleText           takes multiboard lb, string label returns nothing
native MultiboardGetTitleText           takes multiboard lb returns string
native MultiboardSetTitleTextColor      takes multiboard lb, integer red, integer green, integer blue, integer alpha returns nothing

native MultiboardGetRowCount            takes multiboard lb returns integer
native MultiboardGetColumnCount         takes multiboard lb returns integer

native MultiboardSetColumnCount         takes multiboard lb, integer count returns nothing
native MultiboardSetRowCount            takes multiboard lb, integer count returns nothing

// broadcast settings to all items
native MultiboardSetItemsStyle          takes multiboard lb, boolean showValues, boolean showIcons returns nothing
native MultiboardSetItemsValue          takes multiboard lb, string value returns nothing
native MultiboardSetItemsValueColor     takes multiboard lb, integer red, integer green, integer blue, integer alpha returns nothing
native MultiboardSetItemsWidth          takes multiboard lb, real width returns nothing
native MultiboardSetItemsIcon           takes multiboard lb, string iconPath returns nothing


// funcs for modifying individual items
native MultiboardGetItem                takes multiboard lb, integer row, integer column returns multiboarditem
native MultiboardReleaseItem            takes multiboarditem mbi returns nothing

native MultiboardSetItemStyle           takes multiboarditem mbi, boolean showValue, boolean showIcon returns nothing
native MultiboardSetItemValue           takes multiboarditem mbi, string val returns nothing
native MultiboardSetItemValueColor      takes multiboarditem mbi, integer red, integer green, integer blue, integer alpha returns nothing
native MultiboardSetItemWidth           takes multiboarditem mbi, real width returns nothing
native MultiboardSetItemIcon            takes multiboarditem mbi, string iconFileName returns nothing

// meant to unequivocally suspend display of existing and
// subsequently displayed multiboards
//
native MultiboardSuppressDisplay        takes boolean flag returns nothing

//============================================================================
// Camera API
native SetCameraPosition            takes real x, real y returns nothing
native SetCameraQuickPosition       takes real x, real y returns nothing
native SetCameraBounds              takes real x1, real y1, real x2, real y2, real x3, real y3, real x4, real y4 returns nothing
native StopCamera                   takes nothing returns nothing
native ResetToGameCamera            takes real duration returns nothing
native PanCameraTo                  takes real x, real y returns nothing
native PanCameraToTimed             takes real x, real y, real duration returns nothing
native PanCameraToWithZ             takes real x, real y, real zOffsetDest returns nothing
native PanCameraToTimedWithZ        takes real x, real y, real zOffsetDest, real duration returns nothing
native SetCinematicCamera           takes string cameraModelFile returns nothing
native SetCameraRotateMode          takes real x, real y, real radiansToSweep, real duration returns nothing
native SetCameraField               takes camerafield whichField, real value, real duration returns nothing
native AdjustCameraField            takes camerafield whichField, real offset, real duration returns nothing
native SetCameraTargetController    takes unit whichUnit, real xoffset, real yoffset, boolean inheritOrientation returns nothing
native SetCameraOrientController    takes unit whichUnit, real xoffset, real yoffset returns nothing

native CreateCameraSetup                    takes nothing returns camerasetup
native CameraSetupSetField                  takes camerasetup whichSetup, camerafield whichField, real value, real duration returns nothing
native CameraSetupGetField                  takes camerasetup whichSetup, camerafield whichField returns real
native CameraSetupSetDestPosition           takes camerasetup whichSetup, real x, real y, real duration returns nothing
native CameraSetupGetDestPositionLoc        takes camerasetup whichSetup returns location
native CameraSetupGetDestPositionX          takes camerasetup whichSetup returns real
native CameraSetupGetDestPositionY          takes camerasetup whichSetup returns real
native CameraSetupApply                     takes camerasetup whichSetup, boolean doPan, boolean panTimed returns nothing
native CameraSetupApplyWithZ                takes camerasetup whichSetup, real zDestOffset returns nothing
native CameraSetupApplyForceDuration        takes camerasetup whichSetup, boolean doPan, real forceDuration returns nothing
native CameraSetupApplyForceDurationWithZ   takes camerasetup whichSetup, real zDestOffset, real forceDuration returns nothing

native CameraSetTargetNoise             takes real mag, real velocity returns nothing
native CameraSetSourceNoise             takes real mag, real velocity returns nothing

native CameraSetTargetNoiseEx           takes real mag, real velocity, boolean vertOnly returns nothing
native CameraSetSourceNoiseEx           takes real mag, real velocity, boolean vertOnly returns nothing

native CameraSetSmoothingFactor         takes real factor returns nothing

native SetCineFilterTexture             takes string filename returns nothing
native SetCineFilterBlendMode           takes blendmode whichMode returns nothing
native SetCineFilterTexMapFlags         takes texmapflags whichFlags returns nothing
native SetCineFilterStartUV             takes real minu, real minv, real maxu, real maxv returns nothing
native SetCineFilterEndUV               takes real minu, real minv, real maxu, real maxv returns nothing
native SetCineFilterStartColor          takes integer red, integer green, integer blue, integer alpha returns nothing
native SetCineFilterEndColor            takes integer red, integer green, integer blue, integer alpha returns nothing
native SetCineFilterDuration            takes real duration returns nothing
native DisplayCineFilter                takes boolean flag returns nothing
native IsCineFilterDisplayed            takes nothing returns boolean

native SetCinematicScene                takes integer portraitUnitId, playercolor color, string speakerTitle, string text, real sceneDuration, real voiceoverDuration returns nothing
native EndCinematicScene                takes nothing returns nothing
native ForceCinematicSubtitles          takes boolean flag returns nothing

native GetCameraMargin                  takes integer whichMargin returns real

// These return values for the local players camera only...
constant native GetCameraBoundMinX          takes nothing returns real
constant native GetCameraBoundMinY          takes nothing returns real
constant native GetCameraBoundMaxX          takes nothing returns real
constant native GetCameraBoundMaxY          takes nothing returns real
constant native GetCameraField              takes camerafield whichField returns real
constant native GetCameraTargetPositionX    takes nothing returns real
constant native GetCameraTargetPositionY    takes nothing returns real
constant native GetCameraTargetPositionZ    takes nothing returns real
constant native GetCameraTargetPositionLoc  takes nothing returns location
constant native GetCameraEyePositionX       takes nothing returns real
constant native GetCameraEyePositionY       takes nothing returns real
constant native GetCameraEyePositionZ       takes nothing returns real
constant native GetCameraEyePositionLoc     takes nothing returns location

//============================================================================
// Sound API
//
native NewSoundEnvironment          takes string environmentName returns nothing

native CreateSound                  takes string fileName, boolean looping, boolean is3D, boolean stopwhenoutofrange, integer fadeInRate, integer fadeOutRate, string eaxSetting returns sound
native CreateSoundFilenameWithLabel takes string fileName, boolean looping, boolean is3D, boolean stopwhenoutofrange, integer fadeInRate, integer fadeOutRate, string SLKEntryName returns sound
native CreateSoundFromLabel         takes string soundLabel, boolean looping, boolean is3D, boolean stopwhenoutofrange, integer fadeInRate, integer fadeOutRate returns sound
native CreateMIDISound              takes string soundLabel, integer fadeInRate, integer fadeOutRate returns sound

native SetSoundParamsFromLabel      takes sound soundHandle, string soundLabel returns nothing
native SetSoundDistanceCutoff       takes sound soundHandle, real cutoff returns nothing
native SetSoundChannel              takes sound soundHandle, integer channel returns nothing
native SetSoundVolume               takes sound soundHandle, integer volume returns nothing
native SetSoundPitch                takes sound soundHandle, real pitch returns nothing

// the following method must be called immediately after calling "StartSound" 
native SetSoundPlayPosition         takes sound soundHandle, integer millisecs returns nothing

// these calls are only valid if the sound was created with 3d enabled
native SetSoundDistances            takes sound soundHandle, real minDist, real maxDist returns nothing
native SetSoundConeAngles           takes sound soundHandle, real inside, real outside, integer outsideVolume returns nothing
native SetSoundConeOrientation      takes sound soundHandle, real x, real y, real z returns nothing
native SetSoundPosition             takes sound soundHandle, real x, real y, real z returns nothing
native SetSoundVelocity             takes sound soundHandle, real x, real y, real z returns nothing
native AttachSoundToUnit            takes sound soundHandle, unit whichUnit returns nothing

native StartSound                   takes sound soundHandle returns nothing
native StopSound                    takes sound soundHandle, boolean killWhenDone, boolean fadeOut returns nothing
native KillSoundWhenDone            takes sound soundHandle returns nothing

// Music Interface. Note that if music is disabled, these calls do nothing
native SetMapMusic                  takes string musicName, boolean random, integer index returns nothing
native ClearMapMusic                takes nothing returns nothing

native PlayMusic                    takes string musicName returns nothing
native PlayMusicEx                  takes string musicName, integer frommsecs, integer fadeinmsecs returns nothing
native StopMusic                    takes boolean fadeOut returns nothing
native ResumeMusic                  takes nothing returns nothing

native PlayThematicMusic            takes string musicFileName returns nothing
native PlayThematicMusicEx          takes string musicFileName, integer frommsecs returns nothing
native EndThematicMusic             takes nothing returns nothing

native SetMusicVolume               takes integer volume returns nothing
native SetMusicPlayPosition         takes integer millisecs returns nothing
native SetThematicMusicPlayPosition takes integer millisecs returns nothing

// other music and sound calls
native SetSoundDuration             takes sound soundHandle, integer duration returns nothing
native GetSoundDuration             takes sound soundHandle returns integer
native GetSoundFileDuration         takes string musicFileName returns integer

native VolumeGroupSetVolume         takes volumegroup vgroup, real scale returns nothing
native VolumeGroupReset             takes nothing returns nothing

native GetSoundIsPlaying            takes sound soundHandle returns boolean
native GetSoundIsLoading            takes sound soundHandle returns boolean

native RegisterStackedSound         takes sound soundHandle, boolean byPosition, real rectwidth, real rectheight returns nothing
native UnregisterStackedSound       takes sound soundHandle, boolean byPosition, real rectwidth, real rectheight returns nothing

//============================================================================
// Effects API
//
native AddWeatherEffect             takes rect where, integer effectID returns weathereffect
native RemoveWeatherEffect          takes weathereffect whichEffect returns nothing
native EnableWeatherEffect          takes weathereffect whichEffect, boolean enable returns nothing

native TerrainDeformCrater          takes real x, real y, real radius, real depth, integer duration, boolean permanent returns terraindeformation
native TerrainDeformRipple          takes real x, real y, real radius, real depth, integer duration, integer count, real spaceWaves, real timeWaves, real radiusStartPct, boolean limitNeg returns terraindeformation
native TerrainDeformWave            takes real x, real y, real dirX, real dirY, real distance, real speed, real radius, real depth, integer trailTime, integer count returns terraindeformation
native TerrainDeformRandom          takes real x, real y, real radius, real minDelta, real maxDelta, integer duration, integer updateInterval returns terraindeformation
native TerrainDeformStop            takes terraindeformation deformation, integer duration returns nothing
native TerrainDeformStopAll         takes nothing returns nothing

native AddSpecialEffect             takes string modelName, real x, real y returns effect
native AddSpecialEffectLoc          takes string modelName, location where returns effect
native AddSpecialEffectTarget       takes string modelName, widget targetWidget, string attachPointName returns effect
native DestroyEffect                takes effect whichEffect returns nothing

native AddSpellEffect               takes string abilityString, effecttype t, real x, real y returns effect
native AddSpellEffectLoc            takes string abilityString, effecttype t,location where returns effect
native AddSpellEffectById           takes integer abilityId, effecttype t,real x, real y returns effect
native AddSpellEffectByIdLoc        takes integer abilityId, effecttype t,location where returns effect
native AddSpellEffectTarget         takes string modelName, effecttype t, widget targetWidget, string attachPoint returns effect
native AddSpellEffectTargetById     takes integer abilityId, effecttype t, widget targetWidget, string attachPoint returns effect

native AddLightning                 takes string codeName, boolean checkVisibility, real x1, real y1, real x2, real y2 returns lightning
native AddLightningEx               takes string codeName, boolean checkVisibility, real x1, real y1, real z1, real x2, real y2, real z2 returns lightning
native DestroyLightning             takes lightning whichBolt returns boolean
native MoveLightning                takes lightning whichBolt, boolean checkVisibility, real x1, real y1, real x2, real y2 returns boolean
native MoveLightningEx              takes lightning whichBolt, boolean checkVisibility, real x1, real y1, real z1, real x2, real y2, real z2 returns boolean
native GetLightningColorA           takes lightning whichBolt returns real
native GetLightningColorR           takes lightning whichBolt returns real
native GetLightningColorG           takes lightning whichBolt returns real
native GetLightningColorB           takes lightning whichBolt returns real
native SetLightningColor            takes lightning whichBolt, real r, real g, real b, real a returns boolean

native GetAbilityEffect             takes string abilityString, effecttype t, integer index returns string
native GetAbilityEffectById         takes integer abilityId, effecttype t, integer index returns string
native GetAbilitySound              takes string abilityString, soundtype t returns string
native GetAbilitySoundById          takes integer abilityId, soundtype t returns string

//============================================================================
// Terrain API
//
native GetTerrainCliffLevel         takes real x, real y returns integer
native SetWaterBaseColor            takes integer red, integer green, integer blue, integer alpha returns nothing
native SetWaterDeforms              takes boolean val returns nothing
native GetTerrainType               takes real x, real y returns integer
native GetTerrainVariance           takes real x, real y returns integer
native SetTerrainType               takes real x, real y, integer terrainType, integer variation, integer area, integer shape returns nothing
native IsTerrainPathable            takes real x, real y, pathingtype t returns boolean
native SetTerrainPathable           takes real x, real y, pathingtype t, boolean flag returns nothing

//============================================================================
// Image API
//
native CreateImage                  takes string file, real sizeX, real sizeY, real sizeZ, real posX, real posY, real posZ, real originX, real originY, real originZ, integer imageType returns image
native DestroyImage                 takes image whichImage returns nothing
native ShowImage                    takes image whichImage, boolean flag returns nothing
native SetImageConstantHeight       takes image whichImage, boolean flag, real height returns nothing
native SetImagePosition             takes image whichImage, real x, real y, real z returns nothing
native SetImageColor                takes image whichImage, integer red, integer green, integer blue, integer alpha returns nothing
native SetImageRender               takes image whichImage, boolean flag returns nothing
native SetImageRenderAlways         takes image whichImage, boolean flag returns nothing
native SetImageAboveWater           takes image whichImage, boolean flag, boolean useWaterAlpha returns nothing
native SetImageType                 takes image whichImage, integer imageType returns nothing

//============================================================================
// Ubersplat API
//
native CreateUbersplat              takes real x, real y, string name, integer red, integer green, integer blue, integer alpha, boolean forcePaused, boolean noBirthTime returns ubersplat
native DestroyUbersplat             takes ubersplat whichSplat returns nothing
native ResetUbersplat               takes ubersplat whichSplat returns nothing
native FinishUbersplat              takes ubersplat whichSplat returns nothing
native ShowUbersplat                takes ubersplat whichSplat, boolean flag returns nothing
native SetUbersplatRender           takes ubersplat whichSplat, boolean flag returns nothing
native SetUbersplatRenderAlways     takes ubersplat whichSplat, boolean flag returns nothing

//============================================================================
// Blight API
//
native SetBlight                takes player whichPlayer, real x, real y, real radius, boolean addBlight returns nothing
native SetBlightRect            takes player whichPlayer, rect r, boolean addBlight returns nothing
native SetBlightPoint           takes player whichPlayer, real x, real y, boolean addBlight returns nothing
native SetBlightLoc             takes player whichPlayer, location whichLocation, real radius, boolean addBlight returns nothing
native CreateBlightedGoldmine   takes player id, real x, real y, real face returns unit
native IsPointBlighted          takes real x, real y returns boolean

//============================================================================
// Doodad API
//
native SetDoodadAnimation       takes real x, real y, real radius, integer doodadID, boolean nearestOnly, string animName, boolean animRandom returns nothing
native SetDoodadAnimationRect   takes rect r, integer doodadID, string animName, boolean animRandom returns nothing

//============================================================================
// Computer AI interface
//
native StartMeleeAI         takes player num, string script                 returns nothing
native StartCampaignAI      takes player num, string script                 returns nothing
native CommandAI            takes player num, integer command, integer data returns nothing
native PauseCompAI          takes player p,   boolean pause                 returns nothing
native GetAIDifficulty      takes player num                                returns aidifficulty

native RemoveGuardPosition  takes unit hUnit                                returns nothing
native RecycleGuardPosition takes unit hUnit                                returns nothing
native RemoveAllGuardPositions takes player num                             returns nothing

//============================================================================
native Cheat            takes string cheatStr returns nothing
native IsNoVictoryCheat takes nothing returns boolean
native IsNoDefeatCheat  takes nothing returns boolean

native Preload          takes string filename returns nothing
native PreloadEnd       takes real timeout returns nothing

native PreloadStart     takes nothing returns nothing
native PreloadRefresh   takes nothing returns nothing
native PreloadEndEx     takes nothing returns nothing

native PreloadGenClear  takes nothing returns nothing
native PreloadGenStart  takes nothing returns nothing
native PreloadGenEnd    takes string filename returns nothing
native Preloader        takes string filename returns nothing
