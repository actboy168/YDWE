library GroupUtils
//******************************************************************************
//* BY: Rising_Dusk
//* 
//* This library is a simple implementation of a stack for groups that need to
//* be in the user's control for greater than an instant of time. Additionally,
//* this library provides a single, global group variable for use with user-end
//* enumerations. It is important to note that users should not be calling
//* DestroyGroup() on the global group, since then it may not exist for when it
//* it is next needed.
//*
//* The group stack removes the need for destroying groups and replaces it with
//* a recycling method.
//*     function NewGroup takes nothing returns group
//*     function ReleaseGroup takes group g returns boolean
//*     function GroupRefresh takes group g returns nothing
//* 
//* NewGroup grabs a currently unused group from the stack or creates one if the
//* stack is empty. You can use this group however you'd like, but always
//* remember to call ReleaseGroup on it when you are done with it. If you don't
//* release it, it will 'leak' and your stack may eventually overflow if you
//* keep doing that.
//* 
//* GroupRefresh cleans a group of any shadow references which may be clogging
//* its hash table. If you remove a unit from the game who is a member of a unit
//* group, it will 'effectively' remove the unit from the group, but leave a
//* shadow in its place. Calling GroupRefresh on a group will clean up any
//* shadow references that may exist within it.
//******************************************************************************
    globals
        //* Group for use with all instant enumerations
        group ENUM_GROUP = CreateGroup()
        //* Temporary references for GroupRefresh
        private boolean Flag                             = false
        private group Refr                               = null
        //* Assorted constants
        private constant integer MAX_HANDLE_COUNT        = 408000
        private constant integer MIN_HANDLE_ID           = 0x100000
        //* Arrays and counter for the group stack
        private group array Groups
        private integer array Status[MAX_HANDLE_COUNT]
        private integer Count                            = 0
    endglobals

    private function AddEx takes nothing returns nothing
        if Flag then
            call GroupClear(Refr)
            set Flag = false
        endif
        call GroupAddUnit(Refr, GetEnumUnit())
    endfunction
    
    function GroupRefresh takes group g returns nothing
        set Flag = true
        set Refr = g
        call ForGroup(Refr, function AddEx)
        if Flag then
            call GroupClear(g)
        endif
    endfunction

    function NewGroup takes nothing returns group
        if Count == 0 then
            set Groups[0] = CreateGroup()
        else
            set Count = Count - 1
        endif
        set Status[GetHandleId(Groups[Count])-MIN_HANDLE_ID] = 1
        return Groups[Count]
    endfunction

    function ReleaseGroup takes group g returns boolean
        local integer stat = Status[GetHandleId(g)-MIN_HANDLE_ID]
        if g == null then
            debug call BJDebugMsg(SCOPE_PREFIX+" Error: Null groups cannot be released")
            return false
        elseif stat == 0 then
            debug call BJDebugMsg(SCOPE_PREFIX+" Error: Group not part of stack")
            return false
        elseif stat == 2 then
            debug call BJDebugMsg(SCOPE_PREFIX+" Error: Groups cannot be multiply released")
            return false
        elseif Count == 8191 then
            debug call BJDebugMsg(SCOPE_PREFIX+" Error: Max groups achieved, destroying group")
            call DestroyGroup(g)
            return false
        else
            call GroupClear(g)
            set Groups[Count]                = g
            set Count                        = Count + 1
            set Status[GetHandleId(g)-MIN_HANDLE_ID] = 2
        endif
        return true
    endfunction
    
endlibrary
