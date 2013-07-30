//最萌小汐制作,适用于YDWE1.20及以上版本,需要启用预处理功能(关闭cJass编译器)

globals
    camerasetup yd_NullTempCam
    region      yd_NullTempRegion
    unit        yd_NullTempUnit
    item        yd_NullTempItem
    group       yd_NullTempGroup
    force       yd_NullTempForce
endglobals

function YDWEPolledWaitNull takes real duration returns nothing
    local timer t
    local real  timeRemaining

    if (duration > 0) then
        set t = CreateTimer()
        call TimerStart(t, duration, false, null)
        loop
            set timeRemaining = TimerGetRemaining(t)
            exitwhen timeRemaining <= 0

            // If we have a bit of time left, skip past 10% of the remaining
            // duration instead of checking every interval, to minimize the
            // polling on long waits.
            if (timeRemaining > bj_POLLED_WAIT_SKIP_THRESHOLD) then
                call TriggerSleepAction(0.1 * timeRemaining)
            else
                call TriggerSleepAction(bj_POLLED_WAIT_INTERVAL)
            endif
        endloop
        call DestroyTimer(t)
    endif
    set t = null
endfunction

function YDWEPlaySoundNull takes string soundName returns nothing
    local sound soundHandle = CreateSound(soundName, false, false, true, 12700, 12700, "")
    call StartSound(soundHandle)
    call KillSoundWhenDone(soundHandle)
    set soundHandle = null
endfunction

function YDWEGetCurrentCameraSetupNull takes nothing returns camerasetup
    local camerasetup theCam = CreateCameraSetup()
    local real duration = 0
    call CameraSetupSetField(theCam, CAMERA_FIELD_TARGET_DISTANCE, GetCameraField(CAMERA_FIELD_TARGET_DISTANCE), duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_FARZ,            GetCameraField(CAMERA_FIELD_FARZ),            duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_ZOFFSET,         GetCameraField(CAMERA_FIELD_ZOFFSET),         duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_ANGLE_OF_ATTACK, bj_RADTODEG * GetCameraField(CAMERA_FIELD_ANGLE_OF_ATTACK), duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_FIELD_OF_VIEW,   bj_RADTODEG * GetCameraField(CAMERA_FIELD_FIELD_OF_VIEW),   duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_ROLL,            bj_RADTODEG * GetCameraField(CAMERA_FIELD_ROLL),            duration)
    call CameraSetupSetField(theCam, CAMERA_FIELD_ROTATION,        bj_RADTODEG * GetCameraField(CAMERA_FIELD_ROTATION),        duration)
    call CameraSetupSetDestPosition(theCam, GetCameraTargetPositionX(), GetCameraTargetPositionY(), duration)
    set yd_NullTempCam = theCam
    set theCam = null
    return yd_NullTempCam
endfunction

function YDWETriggerRegisterEnterRectSimpleNull takes trigger trig, rect r returns event
    local region rectRegion = CreateRegion()
    call RegionAddRect(rectRegion, r)
    set yd_NullTempRegion = rectRegion
    set rectRegion = null
    return TriggerRegisterEnterRegion(trig, yd_NullTempRegion, null)
endfunction

function YDWETriggerRegisterLeaveRectSimpleNull takes trigger trig, rect r returns event
    local region rectRegion = CreateRegion()
    call RegionAddRect(rectRegion, r)
    set yd_NullTempRegion = rectRegion
    set rectRegion = null
    return TriggerRegisterLeaveRegion(trig, yd_NullTempRegion, null)
endfunction

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


function YDWEIssueBuildOrderByIdLocBJNull takes unit whichPeon, integer unitId, location loc returns boolean
    if (unitId == 'ugol') then
        return YDWEIssueHauntOrderAtLocBJNull(whichPeon, loc)
    else
        return IssueBuildOrderById(whichPeon, unitId, GetLocationX(loc), GetLocationY(loc))
    endif
endfunction

function YDWEWakePlayerUnitsNull takes player whichPlayer returns nothing
    local group g = CreateGroup()
    call GroupEnumUnitsOfPlayer(g, whichPlayer, null)
    call ForGroup(g, function WakePlayerUnitsEnum)
    call DestroyGroup(g)
    set g = null
endfunction


function YDWEEnableCreepSleepBJNull takes boolean enable returns nothing
    call SetPlayerState(Player(PLAYER_NEUTRAL_AGGRESSIVE), PLAYER_STATE_NO_CREEP_SLEEP, IntegerTertiaryOp(enable, 0, 1))

    // If we're disabling, attempt to wake any already-sleeping creeps.
    if (not enable) then
        call YDWEWakePlayerUnitsNull(Player(PLAYER_NEUTRAL_AGGRESSIVE))
    endif
endfunction

function YDWEPauseAllUnitsBJNull takes boolean pause returns nothing
    local integer index
    local player  indexPlayer
    local group   g

    set bj_pauseAllUnitsFlag = pause
    set g = CreateGroup()
    set index = 0
    loop
        set indexPlayer = Player( index )

        // If this is a computer slot, pause/resume the AI.
        if (GetPlayerController( indexPlayer ) == MAP_CONTROL_COMPUTER) then
            call PauseCompAI( indexPlayer, pause )
        endif

        // Enumerate and unpause every unit owned by the player.
        call GroupEnumUnitsOfPlayer( g, indexPlayer, null )
        call ForGroup( g, function PauseAllUnitsBJEnum )
        call GroupClear( g )

        set index = index + 1
        exitwhen index == bj_MAX_PLAYER_SLOTS
    endloop
    call DestroyGroup(g)
    set g = null
endfunction

function YDWEReplaceUnitBJNull takes unit whichUnit, integer newUnitId, integer unitStateMethod returns unit
    local unit    oldUnit = whichUnit
    local unit    newUnit
    local boolean wasHidden
    local integer index
    local item    indexItem
    local real    oldRatio

    // If we have bogus data, don't attempt the replace.
    if (whichUnit == null) then
        set bj_lastReplacedUnit = whichUnit
        return bj_lastReplacedUnit
    endif

    // Hide the original unit.
    set wasHidden = IsUnitHidden(oldUnit)
    call ShowUnit(oldUnit, false)

    // Create the replacement unit.
    if (newUnitId == 'ugol') then
        set newUnit = CreateBlightedGoldmine(GetOwningPlayer(oldUnit), GetUnitX(oldUnit), GetUnitY(oldUnit), GetUnitFacing(oldUnit))
    else
        set newUnit = CreateUnit(GetOwningPlayer(oldUnit), newUnitId, GetUnitX(oldUnit), GetUnitY(oldUnit), GetUnitFacing(oldUnit))
    endif

    // Set the unit's life and mana according to the requested method.
    if (unitStateMethod == bj_UNIT_STATE_METHOD_RELATIVE) then
        // Set the replacement's current/max life ratio to that of the old unit.
        // If both units have mana, do the same for mana.
        if (GetUnitState(oldUnit, UNIT_STATE_MAX_LIFE) > 0) then
            set oldRatio = GetUnitState(oldUnit, UNIT_STATE_LIFE) / GetUnitState(oldUnit, UNIT_STATE_MAX_LIFE)
            call SetUnitState(newUnit, UNIT_STATE_LIFE, oldRatio * GetUnitState(newUnit, UNIT_STATE_MAX_LIFE))
        endif

        if (GetUnitState(oldUnit, UNIT_STATE_MAX_MANA) > 0) and (GetUnitState(newUnit, UNIT_STATE_MAX_MANA) > 0) then
            set oldRatio = GetUnitState(oldUnit, UNIT_STATE_MANA) / GetUnitState(oldUnit, UNIT_STATE_MAX_MANA)
            call SetUnitState(newUnit, UNIT_STATE_MANA, oldRatio * GetUnitState(newUnit, UNIT_STATE_MAX_MANA))
        endif
    elseif (unitStateMethod == bj_UNIT_STATE_METHOD_ABSOLUTE) then
        // Set the replacement's current life to that of the old unit.
        // If the new unit has mana, do the same for mana.
        call SetUnitState(newUnit, UNIT_STATE_LIFE, GetUnitState(oldUnit, UNIT_STATE_LIFE))
        if (GetUnitState(newUnit, UNIT_STATE_MAX_MANA) > 0) then
            call SetUnitState(newUnit, UNIT_STATE_MANA, GetUnitState(oldUnit, UNIT_STATE_MANA))
        endif
    elseif (unitStateMethod == bj_UNIT_STATE_METHOD_DEFAULTS) then
        // The newly created unit should already have default life and mana.
    elseif (unitStateMethod == bj_UNIT_STATE_METHOD_MAXIMUM) then
        // Use max life and mana.
        call SetUnitState(newUnit, UNIT_STATE_LIFE, GetUnitState(newUnit, UNIT_STATE_MAX_LIFE))
        call SetUnitState(newUnit, UNIT_STATE_MANA, GetUnitState(newUnit, UNIT_STATE_MAX_MANA))
    else
        // Unrecognized unit state method - ignore the request.
    endif

    // Mirror properties of the old unit onto the new unit.
    //call PauseUnit(newUnit, IsUnitPaused(oldUnit))
    call SetResourceAmount(newUnit, GetResourceAmount(oldUnit))

    // If both the old and new units are heroes, handle their hero info.
    if (IsUnitType(oldUnit, UNIT_TYPE_HERO) and IsUnitType(newUnit, UNIT_TYPE_HERO)) then
        call SetHeroXP(newUnit, GetHeroXP(oldUnit), false)

        set index = 0
        loop
            set indexItem = UnitItemInSlot(oldUnit, index)
            if (indexItem != null) then
                call UnitRemoveItem(oldUnit, indexItem)
                call UnitAddItem(newUnit, indexItem)
            endif

            set index = index + 1
            exitwhen index >= bj_MAX_INVENTORY
        endloop
    endif

    // Remove or kill the original unit.  It is sometimes unsafe to remove
    // hidden units, so kill the original unit if it was previously hidden.
    if wasHidden then
        call KillUnit(oldUnit)
        call RemoveUnit(oldUnit)
    else
        call RemoveUnit(oldUnit)
    endif

    set bj_lastReplacedUnit = newUnit
    set oldUnit = null
    set newUnit = null
    set indexItem = null
    return bj_lastReplacedUnit
endfunction
	
function YDWEEnumDestructablesInCircleBJFilterNull takes nothing returns boolean
    local real dx = GetDestructableX(GetFilterDestructable())
    local real dy = GetDestructableY(GetFilterDestructable())

    set dx = dx - GetLocationX(bj_enumDestructableCenter)
    set dy = dy - GetLocationY(bj_enumDestructableCenter)
	
    return SquareRoot(dx * dx + dy * dy) <= bj_enumDestructableRadius
endfunction

function YDWEEnumDestructablesInCircleBJNull takes real radius, location loc, code actionFunc returns nothing
    local rect r

    if (radius >= 0) then
        set bj_enumDestructableCenter = loc
        set bj_enumDestructableRadius = radius
        set r = GetRectFromCircleBJ(loc, radius)
        call EnumDestructablesInRect(r, Filter(function YDWEEnumDestructablesInCircleBJFilterNull), actionFunc)
        call RemoveRect(r)
    endif
    set r = null
endfunction

function YDWENudgeUnitsInRectEnumNull takes nothing returns nothing
    local unit nudgee = GetEnumUnit()

    call SetUnitPosition(nudgee, GetUnitX(nudgee), GetUnitY(nudgee))
    set nudgee = null
endfunction

function YDWENudgeItemsInRectEnumNull takes nothing returns nothing
    local item nudgee = GetEnumItem()

    call SetItemPosition(nudgee, GetItemX(nudgee), GetItemY(nudgee))
    set nudgee = null
endfunction

function YDWENudgeObjectsInRectNull takes rect nudgeArea returns nothing
    local group g = CreateGroup()
    call GroupEnumUnitsInRect(g, nudgeArea, null)
    call ForGroup(g, function YDWENudgeUnitsInRectEnumNull)
    call DestroyGroup(g)
	set g = null

    call EnumItemsInRect(nudgeArea, null, function YDWENudgeItemsInRectEnumNull)
endfunction

function YDWENearbyElevatorExistsEnumNull takes nothing returns nothing
    local destructable d     = GetEnumDestructable()
    local integer      dType = GetDestructableTypeId(d)

    if (dType == bj_ELEVATOR_CODE01) or (dType == bj_ELEVATOR_CODE02) then
        set bj_elevatorNeighbor = d
    endif
    set d = null
endfunction


function YDWENearbyElevatorExistsNull takes real x, real y returns boolean
    local real findThreshold = 32
    local rect r

    // If another elevator is overlapping this one, ignore the wall.
    set r = Rect(x - findThreshold, y - findThreshold, x + findThreshold, y + findThreshold)
    set bj_elevatorNeighbor = null
    call EnumDestructablesInRect(r, null, function YDWENearbyElevatorExistsEnumNull)
    call RemoveRect(r)
    set r = null

    return bj_elevatorNeighbor != null
endfunction

function YDWEEnumUnitsSelectedNull takes player whichPlayer, boolexpr enumFilter, code enumAction returns nothing
    local group g = CreateGroup()
    call SyncSelections()
    call GroupEnumUnitsSelected(g, whichPlayer, enumFilter)
    call DestroyBoolExpr(enumFilter)
    call ForGroup(g, enumAction)
    call DestroyGroup(g)
    set g = null
endfunction

function YDWEChangeElevatorWallBlockerNull takes real x, real y, real facing, boolean open returns nothing
    local destructable blocker = null
    local real         findThreshold = 32
    local real         nudgeLength   = 4.25 * bj_CELLWIDTH
    local real         nudgeWidth    = 1.25 * bj_CELLWIDTH
    local rect         r

    // Search for the pathing blocker within the general area.
    set r = Rect(x - findThreshold, y - findThreshold, x + findThreshold, y + findThreshold)
    set bj_elevatorWallBlocker = null
    call EnumDestructablesInRect(r, null, function FindElevatorWallBlockerEnum)
    call RemoveRect(r)
    set blocker = bj_elevatorWallBlocker

    // Ensure that the blocker exists.
    if (blocker == null) then
        set blocker = CreateDeadDestructable(bj_ELEVATOR_BLOCKER_CODE, x, y, facing, 1, 0)
    elseif (GetDestructableTypeId(blocker) != bj_ELEVATOR_BLOCKER_CODE) then
        // If a different destructible exists in the blocker's spot, ignore
        // the request.  (Two destructibles cannot occupy the same location
        // on the map, so we cannot create an elevator blocker here.)
        return
    endif

    if (open) then
        // Ensure that the blocker is dead.
        if (GetDestructableLife(blocker) > 0) then
            call KillDestructable(blocker)
        endif
    else
        // Ensure that the blocker is alive.
        if (GetDestructableLife(blocker) <= 0) then
            call DestructableRestoreLife(blocker, GetDestructableMaxLife(blocker), false)
        endif

        // Nudge any objects standing in the blocker's way.
        if (facing == 0) then
            set r = Rect(x - nudgeWidth/2, y - nudgeLength/2, x + nudgeWidth/2, y + nudgeLength/2)
            call YDWENudgeObjectsInRectNull(r)
            call RemoveRect(r)
        elseif (facing == 90) then
            set r = Rect(x - nudgeLength/2, y - nudgeWidth/2, x + nudgeLength/2, y + nudgeWidth/2)
            call YDWENudgeObjectsInRectNull(r)
            call RemoveRect(r)
        else
            // Unrecognized blocker angle - don't nudge anything.
        endif
    endif
    set blocker = null
endfunction


function YDWEChangeElevatorWallsNull takes boolean open, integer walls, destructable d returns nothing
    local real x = GetDestructableX(d)
    local real y = GetDestructableY(d)
    local real distToBlocker = 192
    local real distToNeighbor = 256

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_EAST) then
        if (not YDWENearbyElevatorExistsNull(x + distToNeighbor, y)) then
            call YDWEChangeElevatorWallBlockerNull(x + distToBlocker, y, 0, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_NORTH) then
        if (not YDWENearbyElevatorExistsNull(x, y + distToNeighbor)) then
            call YDWEChangeElevatorWallBlockerNull(x, y + distToBlocker, 90, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_SOUTH) then
        if (not YDWENearbyElevatorExistsNull(x, y - distToNeighbor)) then
            call YDWEChangeElevatorWallBlockerNull(x, y - distToBlocker, 90, open)
        endif
    endif

    if (walls == bj_ELEVATOR_WALL_TYPE_ALL) or (walls == bj_ELEVATOR_WALL_TYPE_WEST) then
        if (not YDWENearbyElevatorExistsNull(x - distToNeighbor, y)) then
            call YDWEChangeElevatorWallBlockerNull(x - distToBlocker, y, 0, open)
        endif
    endif
endfunction

function YDWEGetUnitsInRectMatchingNull takes rect r, boolexpr filter returns group
    local group g = CreateGroup()
    call GroupEnumUnitsInRect(g, r, filter)
    call DestroyBoolExpr(filter)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsInRectAllNull takes rect r returns group
    return YDWEGetUnitsInRectMatchingNull(r, null)
endfunction

function YDWEGetUnitsInRectOfPlayerNull takes rect r, player whichPlayer returns group
    local group g = CreateGroup()
    set bj_groupEnumOwningPlayer = whichPlayer
    call GroupEnumUnitsInRect(g, r, filterGetUnitsInRectOfPlayer)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsInRangeOfLocMatchingNull takes real radius, location whichLocation, boolexpr filter returns group
    local group g = CreateGroup()
    call GroupEnumUnitsInRangeOfLoc(g, whichLocation, radius, filter)
    call DestroyBoolExpr(filter)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsInRangeOfLocAllNull takes real radius, location whichLocation returns group
    return YDWEGetUnitsInRangeOfLocMatchingNull(radius, whichLocation, null)
endfunction


function YDWEGetUnitsOfTypeIdAllNull takes integer unitid returns group
    local group   result = CreateGroup()
    local group   g      = CreateGroup()
    local integer index

    set index = 0
    loop
        set bj_groupEnumTypeId = unitid
        call GroupClear(g)
        call GroupEnumUnitsOfPlayer(g, Player(index), filterGetUnitsOfTypeIdAll)
        call GroupAddGroup(g, result)

        set index = index + 1
        exitwhen index == bj_MAX_PLAYER_SLOTS
    endloop
    call DestroyGroup(g)
    set g = null
    set yd_NullTempGroup = result
    set result = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsOfPlayerMatchingNull takes player whichPlayer, boolexpr filter returns group
    local group g = CreateGroup()
    call GroupEnumUnitsOfPlayer(g, whichPlayer, filter)
    call DestroyBoolExpr(filter)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsOfPlayerAllNull takes player whichPlayer returns group
    return YDWEGetUnitsOfPlayerMatchingNull(whichPlayer, null)
endfunction


function YDWEGetUnitsOfPlayerAndTypeIdNull takes player whichPlayer, integer unitid returns group
    local group g = CreateGroup()
    set bj_groupEnumTypeId = unitid
    call GroupEnumUnitsOfPlayer(g, whichPlayer, filterGetUnitsOfPlayerAndTypeId)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetUnitsSelectedAllNull takes player whichPlayer returns group
    local group g = CreateGroup()
    call SyncSelections()
    call GroupEnumUnitsSelected(g, whichPlayer, null)
    set yd_NullTempGroup = g
    set g = null
    return yd_NullTempGroup
endfunction

function YDWEGetForceOfPlayerNull takes player whichPlayer returns force
    local force f = CreateForce()
    call ForceAddPlayer(f, whichPlayer)
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

function YDWEGetPlayersByMapControlNull takes mapcontrol whichControl returns force
    local force f = CreateForce()
    local integer playerIndex
    local player  indexPlayer

    set playerIndex = 0
    loop
        set indexPlayer = Player(playerIndex)
        if GetPlayerController(indexPlayer) == whichControl then
            call ForceAddPlayer(f, indexPlayer)
        endif

        set playerIndex = playerIndex + 1
        exitwhen playerIndex == bj_MAX_PLAYER_SLOTS
    endloop
    set indexPlayer = null
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

function YDWEGetPlayersAlliesNull takes player whichPlayer returns force
    local force f = CreateForce()
    call ForceEnumAllies(f, whichPlayer, null)
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

function YDWEGetPlayersEnemiesNull takes player whichPlayer returns force
    local force f = CreateForce()
    call ForceEnumEnemies(f, whichPlayer, null)
    set yd_NullTempForce = f
    set f = null
    return yd_NullTempForce
endfunction

function YDWEGetPlayersMatchingNull takes boolexpr filter returns force
    local force f = CreateForce()
    call ForceEnumPlayers(f, filter)
    call DestroyBoolExpr(filter)
    set yd_NullTempForce = f
    set f =null
    return yd_NullTempForce
endfunction

function YDWEGetRandomSubGroupEnumNull takes nothing returns nothing
    if (bj_randomSubGroupWant > 0) then
        if (bj_randomSubGroupWant >= bj_randomSubGroupTotal) or (GetRandomReal(0,1) < bj_randomSubGroupChance) then
            // We either need every remaining unit, or the unit passed its chance check.
            call GroupAddUnit(bj_randomSubGroupGroup, GetEnumUnit())
            set bj_randomSubGroupWant = bj_randomSubGroupWant - 1
        endif
    endif
    set bj_randomSubGroupTotal = bj_randomSubGroupTotal - 1
    // Fix by: xylign
    set bj_randomSubGroupChance = bj_randomSubGroupWant / I2R(bj_randomSubGroupTotal)
endfunction

function YDWEGetRandomSubGroupNull takes integer count, group sourceGroup returns group
    set bj_randomSubGroupGroup = CreateGroup()
    set bj_randomSubGroupWant  = count
    set bj_randomSubGroupTotal = CountUnitsInGroup(sourceGroup)

    if (bj_randomSubGroupWant <= 0 or bj_randomSubGroupTotal <= 0) then
        return bj_randomSubGroupGroup
    endif

    set bj_randomSubGroupChance = I2R(bj_randomSubGroupWant) / I2R(bj_randomSubGroupTotal)
    call ForGroup(sourceGroup, function YDWEGetRandomSubGroupEnumNull)
    return bj_randomSubGroupGroup
endfunction

function YDWECountLivingPlayerUnitsOfTypeIdNull takes integer unitId, player whichPlayer returns integer
    local group g
    local integer matchedCount

    set g = CreateGroup()
    set bj_livingPlayerUnitsTypeId = unitId
    call GroupEnumUnitsOfPlayer(g, whichPlayer, filterLivingPlayerUnitsOfTypeId)
    set matchedCount = CountUnitsInGroup(g)
    call DestroyGroup(g)
    set g = null

    return matchedCount
endfunction

function YDWESetUnitFacingToFaceLocTimedNull takes unit whichUnit, location target, real duration returns nothing
    local location unitLoc = GetUnitLoc(whichUnit)

    call SetUnitFacingTimed(whichUnit, AngleBetweenPoints(unitLoc, target), duration)
    call RemoveLocation(unitLoc)
    set unitLoc = null
endfunction

function YDWESetUnitFacingToFaceUnitTimedNull takes unit whichUnit, unit target, real duration returns nothing
    local location unitLoc = GetUnitLoc(target)

    call YDWESetUnitFacingToFaceLocTimedNull(whichUnit, unitLoc, duration)
    call RemoveLocation(unitLoc)
    set unitLoc = null
endfunction

function YDWEMakeUnitsPassiveForPlayerNull takes player whichPlayer returns nothing
    local group   playerUnits = CreateGroup()
    call CachePlayerHeroData(whichPlayer)
    call GroupEnumUnitsOfPlayer(playerUnits, whichPlayer, null)
    call ForGroup(playerUnits, function MakeUnitsPassiveForPlayerEnum)
    call DestroyGroup(playerUnits)
    set playerUnits = null
endfunction

function YDWEMakeUnitsPassiveForTeamNull takes player whichPlayer returns nothing
    local integer playerIndex
    local player  indexPlayer

    set playerIndex = 0
    loop
        set indexPlayer = Player(playerIndex)
        if PlayersAreCoAllied(whichPlayer, indexPlayer) then
            call YDWEMakeUnitsPassiveForPlayerNull(indexPlayer)
        endif

        set playerIndex = playerIndex + 1
        exitwhen playerIndex == bj_MAX_PLAYERS
    endloop
    set indexPlayer = null
endfunction


function YDWEMultiboardSetItemStyleBJNull takes multiboard mb, integer col, integer row, boolean showValue, boolean showIcon returns nothing
    local integer curRow = 0
    local integer curCol = 0
    local integer numRows = MultiboardGetRowCount(mb)
    local integer numCols = MultiboardGetColumnCount(mb)
    local multiboarditem mbitem = null

    // Loop over rows, using 1-based index
    loop
        set curRow = curRow + 1
        exitwhen curRow > numRows

        // Apply setting to the requested row, or all rows (if row is 0)
        if (row == 0 or row == curRow) then
            // Loop over columns, using 1-based index
            set curCol = 0
            loop
                set curCol = curCol + 1
                exitwhen curCol > numCols

                // Apply setting to the requested column, or all columns (if col is 0)
                if (col == 0 or col == curCol) then
                    set mbitem = MultiboardGetItem(mb, curRow - 1, curCol - 1)
                    call MultiboardSetItemStyle(mbitem, showValue, showIcon)
                    call MultiboardReleaseItem(mbitem)
                endif
            endloop
        endif
    endloop
    set mbitem = null
endfunction

function YDWEMultiboardSetItemValueBJNull takes multiboard mb, integer col, integer row, string val returns nothing
    local integer curRow = 0
    local integer curCol = 0
    local integer numRows = MultiboardGetRowCount(mb)
    local integer numCols = MultiboardGetColumnCount(mb)
    local multiboarditem mbitem = null

    // Loop over rows, using 1-based index
    loop
        set curRow = curRow + 1
        exitwhen curRow > numRows

        // Apply setting to the requested row, or all rows (if row is 0)
        if (row == 0 or row == curRow) then
            // Loop over columns, using 1-based index
            set curCol = 0
            loop
                set curCol = curCol + 1
                exitwhen curCol > numCols

                // Apply setting to the requested column, or all columns (if col is 0)
                if (col == 0 or col == curCol) then
                    set mbitem = MultiboardGetItem(mb, curRow - 1, curCol - 1)
                    call MultiboardSetItemValue(mbitem, val)
                    call MultiboardReleaseItem(mbitem)
                endif
            endloop
        endif
    endloop
    set mbitem = null
endfunction

function YDWEMultiboardSetItemColorBJNull takes multiboard mb, integer col, integer row, real red, real green, real blue, real transparency returns nothing
    local integer curRow = 0
    local integer curCol = 0
    local integer numRows = MultiboardGetRowCount(mb)
    local integer numCols = MultiboardGetColumnCount(mb)
    local multiboarditem mbitem = null

    // Loop over rows, using 1-based index
    loop
        set curRow = curRow + 1
        exitwhen curRow > numRows

        // Apply setting to the requested row, or all rows (if row is 0)
        if (row == 0 or row == curRow) then
            // Loop over columns, using 1-based index
            set curCol = 0
            loop
                set curCol = curCol + 1
                exitwhen curCol > numCols

                // Apply setting to the requested column, or all columns (if col is 0)
                if (col == 0 or col == curCol) then
                    set mbitem = MultiboardGetItem(mb, curRow - 1, curCol - 1)
                    call MultiboardSetItemValueColor(mbitem, PercentTo255(red), PercentTo255(green), PercentTo255(blue), PercentTo255(100.0-transparency))
                    call MultiboardReleaseItem(mbitem)
                endif
            endloop
        endif
    endloop
    set mbitem = null
endfunction

function YDWEMultiboardSetItemWidthBJNull takes multiboard mb, integer col, integer row, real width returns nothing
    local integer curRow = 0
    local integer curCol = 0
    local integer numRows = MultiboardGetRowCount(mb)
    local integer numCols = MultiboardGetColumnCount(mb)
    local multiboarditem mbitem = null

    // Loop over rows, using 1-based index
    loop
        set curRow = curRow + 1
        exitwhen curRow > numRows

        // Apply setting to the requested row, or all rows (if row is 0)
        if (row == 0 or row == curRow) then
            // Loop over columns, using 1-based index
            set curCol = 0
            loop
                set curCol = curCol + 1
                exitwhen curCol > numCols

                // Apply setting to the requested column, or all columns (if col is 0)
                if (col == 0 or col == curCol) then
                    set mbitem = MultiboardGetItem(mb, curRow - 1, curCol - 1)
                    call MultiboardSetItemWidth(mbitem, width/100.0)
                    call MultiboardReleaseItem(mbitem)
                endif
            endloop
        endif
    endloop
    set mbitem = null
endfunction

function YDWEMultiboardSetItemIconBJNull takes multiboard mb, integer col, integer row, string iconFileName returns nothing
    local integer curRow = 0
    local integer curCol = 0
    local integer numRows = MultiboardGetRowCount(mb)
    local integer numCols = MultiboardGetColumnCount(mb)
    local multiboarditem mbitem = null

    // Loop over rows, using 1-based index
    loop
        set curRow = curRow + 1
        exitwhen curRow > numRows

        // Apply setting to the requested row, or all rows (if row is 0)
        if (row == 0 or row == curRow) then
            // Loop over columns, using 1-based index
            set curCol = 0
            loop
                set curCol = curCol + 1
                exitwhen curCol > numCols

                // Apply setting to the requested column, or all columns (if col is 0)
                if (col == 0 or col == curCol) then
                    set mbitem = MultiboardGetItem(mb, curRow - 1, curCol - 1)
                    call MultiboardSetItemIcon(mbitem, iconFileName)
                    call MultiboardReleaseItem(mbitem)
                endif
            endloop
        endif
    endloop
    set mbitem = null
endfunction

function YDWETriggerActionUnitRescuedBJNull takes nothing returns nothing
    local unit theUnit = GetTriggerUnit()

    if IsUnitType(theUnit, UNIT_TYPE_STRUCTURE) then
        call RescueUnitBJ(theUnit, GetOwningPlayer(GetRescuer()), bj_rescueChangeColorBldg)
    else
        call RescueUnitBJ(theUnit, GetOwningPlayer(GetRescuer()), bj_rescueChangeColorUnit)
    endif
    set theUnit = null
endfunction


function YDWETryInitRescuableTriggersBJNull takes nothing returns nothing
    local integer index

    if (bj_rescueUnitBehavior == null) then
        set bj_rescueUnitBehavior = CreateTrigger()
        set index = 0
        loop
            call TriggerRegisterPlayerUnitEvent(bj_rescueUnitBehavior, Player(index), EVENT_PLAYER_UNIT_RESCUED, null)
            set index = index + 1
            exitwhen index == bj_MAX_PLAYER_SLOTS
        endloop
        call TriggerAddAction(bj_rescueUnitBehavior, function YDWETriggerActionUnitRescuedBJNull)
    endif
endfunction

function YDWEMakeUnitRescuableToForceBJEnumNull takes nothing returns nothing
    call YDWETryInitRescuableTriggersBJNull()
    call SetUnitRescuable(bj_makeUnitRescuableUnit, GetEnumPlayer(), bj_makeUnitRescuableFlag)
endfunction


function YDWEMakeUnitRescuableToForceBJNull takes unit whichUnit, boolean isRescuable, force whichForce returns nothing
    // Flag the unit as rescuable/unrescuable for the appropriate players.
    set bj_makeUnitRescuableUnit = whichUnit
    set bj_makeUnitRescuableFlag = isRescuable
    call ForForce(whichForce, function YDWEMakeUnitRescuableToForceBJEnumNull)
endfunction

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


function YDWEBlightGoldMineForPlayerNull takes unit goldMine, player whichPlayer returns unit
    set bj_lastHauntedGoldMine = YDWEBlightGoldMineForPlayerBJNull(goldMine, whichPlayer)
    return bj_lastHauntedGoldMine
endfunction

function YDWESetPlayerColorBJNull takes player whichPlayer, playercolor color, boolean changeExisting returns nothing
    local group g

    call SetPlayerColor(whichPlayer, color)
    if changeExisting then
        set bj_setPlayerTargetColor = color
        set g = CreateGroup()
        call GroupEnumUnitsOfPlayer(g, whichPlayer, null)
        call ForGroup(g, function SetPlayerColorBJEnum)
        call DestroyGroup(g)
    endif
    set g = null
endfunction

function YDWEInitRescuableBehaviorBJNull takes nothing returns nothing
    local integer index

    set index = 0
    loop
        // If at least one player slot is "Rescuable"-controlled, init the
        // rescue behavior triggers.
        if (GetPlayerController(Player(index)) == MAP_CONTROL_RESCUABLE) then
            call YDWETryInitRescuableTriggersBJNull()
            return
        endif
        set index = index + 1
        exitwhen index == bj_MAX_PLAYERS
    endloop
endfunction

function YDWEDelayedSuspendDecayStopAnimEnumNull takes nothing returns nothing
    local unit enumUnit = GetEnumUnit()

    if (GetUnitState(enumUnit, UNIT_STATE_LIFE) <= 0) then
        call SetUnitTimeScale(enumUnit, 0.0001)
    endif
    set enumUnit = null
endfunction

function YDWEDelayedSuspendDecayFleshEnumNull takes nothing returns nothing
    local unit enumUnit = GetEnumUnit()

    if (GetUnitState(enumUnit, UNIT_STATE_LIFE) <= 0) then
        call UnitSuspendDecay(enumUnit, true)
        call SetUnitTimeScale(enumUnit, 10.0)
        call SetUnitAnimation(enumUnit, "decay flesh")
    endif
    set enumUnit = null
endfunction

function YDWEDelayedSuspendDecayBoneEnumNull takes nothing returns nothing
    local unit enumUnit = GetEnumUnit()

    if (GetUnitState(enumUnit, UNIT_STATE_LIFE) <= 0) then
        call UnitSuspendDecay(enumUnit, true)
        call SetUnitTimeScale(enumUnit, 0.0001)
    endif
    set enumUnit = null
endfunction

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

function YDWESmartCameraPanBJNull takes player whichPlayer, location loc, real duration returns nothing
    local real dist2
	local real dx
	local real dy
    if (GetLocalPlayer() == whichPlayer) then
        // Use only local code (no net traffic) within this block to avoid desyncs.
		set dx = GetCameraTargetPositionX() - GetLocationX(loc)
		set dy = GetCameraTargetPositionY() - GetLocationY(loc) 
        set dist2 = dx * dx + dy * dy
        if (dist2 >= bj_SMARTPAN_TRESHOLD_SNAP*bj_SMARTPAN_TRESHOLD_SNAP) then
            // If the user is too far away, snap the camera.
            call PanCameraToTimed(GetLocationX(loc), GetLocationY(loc), 0)
        elseif (dist2 >= bj_SMARTPAN_TRESHOLD_PAN*bj_SMARTPAN_TRESHOLD_PAN) then
            // If the user is moderately close, pan the camera.
            call PanCameraToTimed(GetLocationX(loc), GetLocationY(loc), duration)
        else
            // User is close enough, so don't touch the camera.
        endif
    endif
endfunction

function YDWEGetInventoryIndexOfItemTypeBJNull takes unit whichUnit, integer itemId returns integer
    local integer index = 0

	if itemId != 0 then
		loop
			if GetItemTypeId(UnitItemInSlot(whichUnit, index)) == itemId then
				return index + 1
			endif
	
			set index = index + 1
			exitwhen index >= bj_MAX_INVENTORY
		endloop
	endif	
    return 0
endfunction

function YDWEGetItemOfTypeFromUnitBJNull takes unit whichUnit, integer itemId returns item
    local integer index = 0
    loop
        set yd_NullTempItem = UnitItemInSlot(whichUnit, index)
        if GetItemTypeId(yd_NullTempItem) == itemId then
            return yd_NullTempItem
        endif

        set index = index + 1
        exitwhen index >= bj_MAX_INVENTORY
    endloop
    return null
endfunction

function YDWEUnitHasItemOfTypeBJNull takes unit whichUnit, integer itemId returns boolean
    local integer index = 0

	if itemId != 0 then
		loop
			if GetItemTypeId(UnitItemInSlot(whichUnit, index)) == itemId then
				return true
			endif
	
			set index = index + 1
			exitwhen index >= bj_MAX_INVENTORY
		endloop
	endif	
    return false
endfunction
