/*
    cj_print.j v 0.93

    The string formatting functions. Overloaded to take up to 16 arguments.
    For examples of usage and extending refer to cJass Manual.

    This file is part of cJass standard library
    © 2009 ADOLF \\ cjass.xgm.ru
*/

#guard cj_print

library_once cjsprintf initializer cj_sprintf_updatePlayers {

    include "cj_types_priv.j"

    #if WAR3VER == WAR3VER_23
        undef cj_sprintf_GetHandleID

        int cj_sprintf_GetHandleID (handle h) { return h; return 0x00 } 
    #endif

    define {
        cj_sprintf_badArg = @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) \
                            @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) \
                            @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) \
                            @(***unknown arg***) @(***unknown arg***) @(***unknown arg***) @(***unknown arg***)

        cj_sprintf_argStr = !@@a

        cj_sprintf_GetHandleID (h) = GetHandleId(h)

        cj_sprintf_artTyp_Const = "^h^=<I2S(cj_sprintf_GetHandleID($))>^pc^=<cj_sprintf_pc[GetPlayerId($)]>^p^=<cj_sprintf_p[GetPlayerId($)]>^igold^=|cffffdc00<I2S($)>|r^ilumb^=|cff00c850<I2S($)>|r^ipc^=<cj_sprintf_pc[$]>^ip^=<cj_sprintf_p[$]>^i^=<I2S($)>^r^=<R2S($)>^b^=<cj_sprintf_b2s($)>^s^=<$>^v^=$"
        cj_sprintf_argTyp_Base  = cj_sprintf_artTyp_Const
        cj_sprintf_argTyp_User  =
        cj_sprintf_argTyp_Err   = "^^=<(***unknown type***)>^^^=<(***unknown type***)>^^^^=<(***unknown type***)>^^^^^=<(**unknown type***)>^^^^^^=<(***unknown type***)>^^^^^^^=<(***unknown type***)>^^^^^^^^=<(***unknown type***)>"

        // parse string
        <call sprintf()> =
        sprintf () = null
        sprintf (str) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e
        sprintf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e
        // def output
        <call printf()> =
        printf () = 
        printf (str) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        printf (str, arg_00) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        printf (str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = DisplayTimedTextToPlayer(GetLocalPlayer(), 0., 0., 60.,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // BJDebugMsg
        <call sBJDebugMsg()> =
        sBJDebugMsg() = 
        sBJDebugMsg(str) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sBJDebugMsg(str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = BJDebugMsg( !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // DisplayTextToPlayer
        sDisplayTextToPlayer (p, x, y) = DisplayTextToPlayer (p, x, y, "")
        sDisplayTextToPlayer (p, x, y, str) = DisplayTextToPlayer (p, x, y, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sDisplayTextToPlayer (p, x, y, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = DisplayTextToPlayer (p, x, y,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // DisplayTimedTextToPlayer
        sDisplayTimedTextToPlayer (p, x, y, r) = DisplayTimedTextToPlayer (p, x, y, r, "")
        sDisplayTimedTextToPlayer (p, x, y, r, str) = DisplayTimedTextToPlayer (p, x, y, r, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextToPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = DisplayTimedTextToPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // DisplayTimedTextFromPlayer
        sDisplayTimedTextFromPlayer (p, x, y, r) = DisplayTimedTextFromPlayer (p, x, y, r, "")
        sDisplayTimedTextFromPlayer (p, x, y, r, str) = DisplayTimedTextFromPlayer (p, x, y, r, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sDisplayTimedTextFromPlayer (p, x, y, r, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = DisplayTimedTextFromPlayer (p, x, y, r,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // SetTextTagText
        sSetTextTagText (t, h) = SetTextTagText (t, "", h)
        sSetTextTagText (t, str, h) = SetTextTagText (t, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e, h)
        sSetTextTagText (t, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f, h) = SetTextTagText (t,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e, h)

        // QuestSetTitle
        sQuestSetTitle (q) = QuestSetTitle (q, "")
        sQuestSetTitle (q, str) = QuestSetTitle (q, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sQuestSetTitle (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = QuestSetTitle (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // QuestSetDescription
        sQuestSetDescription (q) = QuestSetDescription (q, "")
        sQuestSetDescription (q, str) = QuestSetDescription (q, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sQuestSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = QuestSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // QuestItemSetDescription
        sQuestItemSetDescription (q) = QuestItemSetDescription (q, "")
        sQuestItemSetDescription (q, str) = QuestItemSetDescription (q, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sQuestItemSetDescription (q, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = QuestItemSetDescription (q,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // MultiboardSetTitleText
        sMultiboardSetTitleText (mb) = MultiboardSetTitleText (mb, "")
        sMultiboardSetTitleText (mb, str) = MultiboardSetTitleText (mb, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sMultiboardSetTitleText (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = MultiboardSetTitleText (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // MultiboardSetItemsValue
        sMultiboardSetItemsValue (mb) = MultiboardSetItemsValue (mb, "")
        sMultiboardSetItemsValue (mb, str) = MultiboardSetItemsValue (mb, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemsValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = MultiboardSetItemsValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // MultiboardSetItemValue
        sMultiboardSetItemValue (mb) = MultiboardSetItemValue (mb, "")
        sMultiboardSetItemValue (mb, str) = MultiboardSetItemValue (mb, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sMultiboardSetItemValue (mb, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = MultiboardSetItemValue (mb,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // DialogSetMessage
        sDialogSetMessage (d) = DialogSetMessage (d, "")
        sDialogSetMessage (d, str) = DialogSetMessage (d, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e )
        sDialogSetMessage (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f) = DialogSetMessage (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e )

        // DialogAddButton
        sDialogAddButton (d, i) = DialogAddButton (d, "", i)
        sDialogAddButton (d, str, i) = DialogAddButton (d, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e, i )
        sDialogAddButton (d, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f, i) = DialogAddButton (d,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e, i )

        // DialogAddQuitButton
        sDialogAddQuitButton (d, b, i) = DialogAddQuitButton (d, b, "", i)
        sDialogAddQuitButton (d, b, str, i) = DialogAddQuitButton (d, b, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e, i )
        sDialogAddQuitButton (d, b, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f, i) = DialogAddQuitButton (d, b,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e, i )

        // LeaderboardAddItem
        sLeaderboardAddItem(l, p) = LeaderboardAddItem(l, "", p )
        sLeaderboardAddItem(l, str, p) = LeaderboardAddItem(l, !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                @arg_00 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                        @arg_00 ## @arg_01 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                 @arg_00 ## @arg_01 ## @arg_02 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## cj_sprintf_badArg !@@e, p )
        sLeaderboardAddItem(l, str, arg_00, arg_01, arg_02, arg_03, arg_04, arg_05, arg_06, arg_07, arg_08, arg_09, arg_0a, arg_0b, arg_0c, arg_0d, arg_0e, arg_0f, p) = LeaderboardAddItem(l,  !@@s ## str cj_sprintf_argStr ## cj_sprintf_argTyp_Base ## cj_sprintf_argTyp_User ## cj_sprintf_argTyp_Err ## \
                                                                                                                                                        @arg_00 ## @arg_01 ## @arg_02 ## @arg_03 ## @arg_04 ## @arg_05 ## @arg_06 ## @arg_07 ## @arg_08 ## @arg_09 ## @arg_0a ## @arg_0b ## @arg_0c ## @arg_0d ## @arg_0e ## @arg_0f ## cj_sprintf_badArg !@@e, p )

    }

    string array cj_sprintf_p, cj_sprintf_pc
    
    void cj_sprintf_updatePlayers () {
        cj_sprintf_p [0x00] = GetPlayerName(Player(0x00))
        cj_sprintf_p [0x01] = GetPlayerName(Player(0x01))
        cj_sprintf_p [0x02] = GetPlayerName(Player(0x02))
        cj_sprintf_p [0x03] = GetPlayerName(Player(0x03))
        cj_sprintf_p [0x04] = GetPlayerName(Player(0x04))
        cj_sprintf_p [0x05] = GetPlayerName(Player(0x05))
        cj_sprintf_p [0x06] = GetPlayerName(Player(0x06))
        cj_sprintf_p [0x07] = GetPlayerName(Player(0x07))
        cj_sprintf_p [0x08] = GetPlayerName(Player(0x08))
        cj_sprintf_p [0x09] = GetPlayerName(Player(0x09))
        cj_sprintf_p [0x0a] = GetPlayerName(Player(0x0a))
        cj_sprintf_p [0x0b] = GetPlayerName(Player(0x0b))
        cj_sprintf_p [0x0c] = GetPlayerName(Player(0x0c))
        cj_sprintf_p [0x0d] = GetPlayerName(Player(0x0d))
        cj_sprintf_p [0x0e] = GetPlayerName(Player(0x0e))
        cj_sprintf_p [0x0f] = GetPlayerName(Player(0x0f))

        cj_sprintf_pc [0x00] = "|cffff0303" + GetPlayerName(Player(0x00)) + "|r"
        cj_sprintf_pc [0x01] = "|cff0042ff" + GetPlayerName(Player(0x01)) + "|r"
        cj_sprintf_pc [0x02] = "|cff1ce6b9" + GetPlayerName(Player(0x02)) + "|r"
        cj_sprintf_pc [0x03] = "|cff540081" + GetPlayerName(Player(0x03)) + "|r"
        cj_sprintf_pc [0x04] = "|cffffff01" + GetPlayerName(Player(0x04)) + "|r"
        cj_sprintf_pc [0x05] = "|cfffe8a0e" + GetPlayerName(Player(0x05)) + "|r"
        cj_sprintf_pc [0x06] = "|cff20c000" + GetPlayerName(Player(0x06)) + "|r"
        cj_sprintf_pc [0x07] = "|cffe55bb0" + GetPlayerName(Player(0x07)) + "|r"
        cj_sprintf_pc [0x08] = "|cff959697" + GetPlayerName(Player(0x08)) + "|r"
        cj_sprintf_pc [0x09] = "|cff7ebff1" + GetPlayerName(Player(0x09)) + "|r"
        cj_sprintf_pc [0x0a] = "|cff106246" + GetPlayerName(Player(0x0a)) + "|r"
        cj_sprintf_pc [0x0b] = "|cff4e2a04" + GetPlayerName(Player(0x0b)) + "|r"
        cj_sprintf_pc [0x0c] = "|cff282828" + GetPlayerName(Player(0x0c)) + "|r"
        cj_sprintf_pc [0x0d] = "|cff282828" + GetPlayerName(Player(0x0d)) + "|r"
        cj_sprintf_pc [0x0e] = "|cff282828" + GetPlayerName(Player(0x0e)) + "|r"
        cj_sprintf_pc [0x0f] = "|cff282828" + GetPlayerName(Player(0x0f)) + "|r"
    }
   
    string cj_sprintf_b2s (bool b) {
        if b { return "true" }
        return "false"
    }

}