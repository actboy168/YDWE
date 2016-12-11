;;-------------------------------------------------------------------------
;;
;;	Adic Helper [cJass]
;;	v 01 04 01 05
;;
;;	© 2009 ADOLF aka ADX 
;;	http://cjass.xgm.ru
;;
;;-------------------------------------------------------------------------

;;-------------------------------------------------------------------------
	.686
	.model flat
	.xmm
;;-------------------------------------------------------------------------
	includelib	\masm32\lib\kernel32.lib
	includelib	\masm32\lib\user32.lib
	includelib	\masm32\lib\gdi32.lib
	includelib	\masm32\lib\comctl32.lib
	includelib	\masm32\lib\shell32.lib

	includelib	SFmpq.lib

;;	include		\masm32\include\kernel32.inc
	include		\masm32\include\gdi32.inc
	include		\masm32\include\windows.inc
	include		\masm32\include\comctl32.inc
;;-------------------------------------------------------------------------
	extern	_imp__ExitProcess@4:dword
	extern	_imp__GetCommandLineA@0:dword
	extern	_imp__CreateThread@24:dword
	extern	_imp__CloseHandle@4:dword
	extern	_imp__GlobalAlloc@8:dword
	extern	_imp__GlobalLock@4:dword
	extern	_imp__GlobalUnlock@4:dword
	extern	_imp__GlobalFree@4:dword
	extern	_imp__CreateFileA@28:dword
	extern	_imp__WriteFile@20:dword
	extern	_imp__DeleteFileA@4:dword
	extern	_imp__GetCurrentDirectoryA@8:dword
	extern	_imp__SetCurrentDirectoryA@4:dword
	extern	_imp__ReadFile@20:dword
	extern	_imp__GetFileSize@8:dword
	extern	_imp__SetThreadPriority@8:dword

	extern	_imp__MessageBoxA@16:dword
	extern	_imp__RegisterClassA@4:dword
	extern	_imp__CreateWindowExA@48:dword
	extern	_imp__GetSystemMetrics@4:dword
	extern	_imp__GetMessageA@16:dword
	extern	_imp__DispatchMessageA@4:dword
	extern	_imp__DefWindowProcA@16:dword
	extern	_imp__LoadIconA@8:dword
	extern	_imp__PostMessageA@16:dword
	extern	_imp__SendMessageA@16:dword
	extern	_imp__ShowWindow@8:dword
	extern	_imp__RedrawWindow@16:dword
	extern	_imp__LoadCursorA@8:dword
	extern	_imp__SetCursor@4:dword
;;	extern	_imp__DrawIcon@16:dword
	extern	_imp__DestroyWindow@4:dword
	extern	_imp__SetWindowTextA@8:dword
	extern	_imp__MoveWindow@24:dword
	extern	_imp__MessageBeep@4:dword
	extern	_imp__SetFocus@4:dword

	extern	_imp__MpqOpenArchiveForUpdate@12:dword
	extern	_imp__MpqDeleteFile@8:dword
	extern	_imp__SFileOpenFileEx@16:dword
	extern	_imp__MpqCompactArchive@4:dword
	extern	_imp__MpqCloseUpdatedArchive@8:dword
	extern	_imp__SFileGetFileSize@8:dword
	extern	_imp__SFileReadFile@20:dword
	extern	_imp__MpqAddFileToArchiveEx@24:dword

	extern	_imp__BeginPaint@8:dword
	extern	_imp__EndPaint@8:dword
	extern	_imp__SetBkMode@8:dword
	extern	_imp__GetStockObject@4:dword
	extern	_imp__SelectObject@8:dword
	extern	_imp__SetBkMode@8:dword
	extern	_imp__DrawTextA@20:dword
	extern	_imp__SetTextColor@8:dword
	extern	_imp__CreateFontIndirectA@4:dword
	
	extern	_imp__ShellExecuteA@24:dword

	extern	_imp__GetLocalTime@4:dword
	
;;dbg
;;extern	_imp__wsprintfA:dword
;;-------------------------------------------------------------------------
.data

	WM_PROCEND		equ	WM_USER+02h
	WM_CJ_ERROR		equ	WM_USER+03h

	_dWndStlEx		dd	WS_VISIBLE

;	align			04h
	_sWinName		db	"AdicHelper 1.4.1.5", 00h
	_sTollInfo		db	"cJass parser and optimizer AdicHelper v 1.4.1.5", 0dh, 0ah, "(c) ADOLF aka ADX, 2009", 00h
	_sSiteAdr		db	"http://cjass.xgm.ru", 00h
	
	_sOpen			db	"open", 00h

	_sAttr			db	"(attributes)", 00h
	_sWJ			db	"war3map.j", 00h
	_sWJP			db	"parsed_war3map.j", 00h
	_sWJO			db	"optimized_war3map.j", 00h
	_sSynErr		db	"AdicHelper: syntax error", 00h

	_dfilename		dd	offset _sWJP

	_dDefTableSD		equ	$

	_dMapProcCode		dd	00h	;; used in string preprocessor

	;; used in setdef
				dd	00h, 00h, 00h

	_hWndCls		dd	CS_NOCLOSE or CS_HREDRAW or CS_VREDRAW or CS_BYTEALIGNWINDOW,
					offset _hWndProc, 00h, 00h, 400000h, ?, 10011h, COLOR_WINDOW, 00h, offset _sWJ
	_hErrCls		dd	CS_HREDRAW or CS_VREDRAW,
					offset _hErrProc, 00h, 00h, 400000h, ?, 10011h,	COLOR_WINDOW, 00h, offset _sSynErr

	;; fonts
	_xOutFont		dd	0bh, 00h, 00h, 00h, 0190h
				db	00h, 00h ,00h, ANSI_CHARSET, 00h, 00h, 00h, 01h
				db	"Lucida console", 00h
	_xGuiFont		dd	0dh, 00h, 00h, 00h, 0190h
				db	00h, 00h ,00h, ANSI_CHARSET, 00h, 00h, 00h, 01h
				db	"MS Sans Serif", 00h
	_xWWWFont		dd	0dh, 00h, 00h, 00h, 0190h
				db	00h, 01h ,00h, ANSI_CHARSET, 00h, 00h, 00h, 01h
				db	"MS Sans Serif", 00h

	_dFreeScope		dd	00h

	_dDbgOff		dd	offset _lCRDebugRem	;; is debug mode

	_dSynDesc		dd	offset _xSynDesc	;; ex syntax (zinc and boa) pointer

	_dSortSteps		dd	0010h, 0050h, 0130h, 0290h, 06d0h, 0d10h, 1f90h, 3a10h, 8710h, 0f410h, 0ffffffffh
	;;
	;; 10 bits offset oriented
	;;

	_dCurrStr		dd	offset _sProg_00

	_xRect_00		dd	0010h, 10h, 0172h, 40h
	_xRect_01		dd	0010h, 24h, 0172h, 40h
	_xRect_toRedraw		dd	0118h, 10h, 0172h, 1ch

	_xRect_02		dd	0010h, 08h, 0172h, 20h

	_dEnumStrPoint		dd	offset _xEnumStr+0dh
	_dEnumTablePointer	dd	offset _xEnumTable+0ch
;;	_dEnumLabelPointer	dd	offset _xEnumLabel

	_dEnumDefTable		dd	offset _xEnumDefTable, offset _xEnumDefTable+0ch
	_xEnumDefTable		dd	80000000h, 0ffffffffh, offset _xEnumDefTable+0ch, 00000000h, 7fffffffh, 00000000h

	_sDateL			db	"DATE", 02h
	_sTimeL			db	"TIME", 02h
	_sDebugL		db	"DEBUG", 02h
	_sCountL		db	"COUNTER", 02h
	_sWeatherL		db	"WEATHER_ON_MARS", 02h
	_sWarVer		db	"WAR3VER", 02h
	_sFuncNameL		db	"FUNCNAME", 02h

	_sTrue			db	"1", 03h
	_sVer24			db	"WAR3VER_24", 03h
	_sVer23			db	"WAR3VER_23", 03h
	_sVerUndef		db	"WAR3VER_00", 03h
	_sCntSpec		db	01h, 36h, 03h
	_sWether		db	01h, 37h, 03h
	_sFuncName		db	66h, 66h, 01h, 38h, 03h

	_dWarVerSL		dd	offset _sVerUndef	;; if zero - do not remove unused code

	_sProg_00		db	"Parsing: preparing...", 00h
	_sProg_01		db	"Parsing: processing...", 00h
	_sProg_02		db	"Parsing: build Find'n'Replace table...", 00h
	_sProg_03		db	"Parsing: find'n'Replace: process...", 00h
	_sProg_04		db	"Parsing: killing Vexorian...", 00h
	_sProg_05		db	"Parsing: success!", 00h

	_sProg_06		db	"Optimization: preparing...", 00h
	_sProg_07		db	"Optimization: removing unused code...", 00h
	_sProg_08		db	"Optimization: success!", 00h

	_sExit			db	"Exit", 00h

	_sProgBar		db	"msctls_progress32", 00h
	_sEditWnd		db	"edit",	00h
	_sListWnd		db	"listbox", 00h
	_sButton		db	"button", 00h

	_sErr_Arch		db	"Error: cannot open archive", 00h
	_sErr_Code		db	"Error: cannot find war3map.j", 00h

	_sErr_Title		db	"Compiling is stopped", 00h

	_sErr_Base		db	"[00] Critical syntax error", 00h
	_sErr_UnclosedString	db	"[01] Critical error: string unclosed or too big", 00h
	_sErr_CantOpenFile	db	"[02] Critical error: cannot open included file", 00h
	_sErr_BadBlockInFile	db	"[03] Critical error: unclosed block in included file", 00h
	_sErr_BadBlock		db	"[04] Critical error: excessive block closing bracket", 00h
	_sErr_UnclosedBlock	db	"[05] Critical error: unclosed block", 00h
	_sErr_EndLibScope	db	"[06] Critical error: excessive endlibrary/endscope", 00h
	_sErr_BadDef		db	"[07] Critical error: word cannot be defined: maybe missing enddefine?", 00h
	_sErr_BadChar		db	"[08] Critical error: bad char", 00h
	_sErr_UnclosedLib	db	"[09] Critical error: missing endlibrary or endscope", 00h
	_sErr_UnclosedComment	db	"[10] Critical error: unclosed comment block", 00h
	_sErr_BadComment	db	"[11] Critical error: comment closed without opening", 00h
	_sErr_ValueRedefined	db	"[12] Critical error: macro defined twice with different arguments", 00h
	_sErr_ValueRedefinedEX	db	"[**] Redeclared here", 00h
	_sErr_MissDefArg	db	"[13] Critical error: missing define arguments", 00h
	_sErr_DefArg		db	"[14] Critical error: too many arguments passed to define", 00h
	_sErr_UnknowBlock	db	"[15] Critical error: unknown block", 00h
	_sErr_IncorrectLiteral	db	"[16] Critical error: incorrect literal", 00h
	_sErr_PreProc		db	"[17] Critical error: incorrect preprocessor instruction", 00h
	_sErr_Ude		db	"[18] Critical error: used defined error", 00h
	_sErr_FailedDef		db	"[19] Critical error: defines cannot be declared in #if/#endif block. Please declare them outside of this block and use setdef instruction", 00h

	_bFCLL			db	40h		;; locals
	_bFCLLMAX		db	40h		;; locals max

	_dFCPL			dd	offset 	_bFuncPostEX		;; postix index - pointer

	_sCodeConst		db	"true", 00h
				db	"false", 00h
				db	"function", 00h
				db	"Player", 00h
				db	"GetRandomInt", 00h
				db	"GetRandomReal", 00h
				db	"Deg2Rad", 00h
				db	"Rad2Deg", 00h
				db	"Sin", 00h
				db	"Cos", 00h
				db	"Tan", 00h
				db	"Asin", 00h
				db	"Acos", 00h
				db	"Atan", 00h
				db	"Atan2", 00h
				db	"SquareRoot", 00h
				db	"Pow", 00h
				db	"I2R", 00h
				db	"R2I", 00h
				db	"I2S", 00h
				db	"R2S", 00h
				db	"R2SW", 00h
				db	"S2I", 00h
				db	"S2R", 00h
				db	"SubString", 00h
				db	"StringLength", 00h
				db	"StringCase", 00h
				db	"Condition", 00h
				db	"Filter", 00h

				db	00h

	_sCodeNXFunc		db	"laeRmodnaRteG", 00h
				db	"tnImodnaRteG", 00h
				db	"reyalP", 00h
				db	"reggirTetaerC", 00h
				db	"noigeRetaerC", 00h
				db	"noitacoL", 00h
				db	"tceR", 00h
				db	"ecroFetaerC", 00h
				db	"puorGetaerC", 00h
				db	"remiTetaerC", 00h
				db	"tnImodnaRteG", 00h
				db	"laeRmodnaRteG", 00h

				db	00h

	;;
	;; 0...9 ; A...Z ; _ ; a...z
	;;

	_bAscii_00 \
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 01h
	db	00h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h

	;;
	;; 0...9 ; A...Z ; _ ; a...z ; , and .
	;;

	_bAscii_03 \
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 01h, 00h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 00h
	db	00h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 01h
	db	00h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h
	db	01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 01h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	db	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h

	;;
	;; remove bs 00
	;;
	;;	00h,       01h,       02h,       03h,       04h,       05h,       06h,       07h,       08h,       09h,       0ah,       0bh,       0ch,       0dh,       0eh,       0fh
	_bAscii_01 \
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00	;; 00h - 0fh
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00	;; 10h - 1fh
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_02, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_01, _lCRDD_01, _lCRDD_00, _lCRDD_01, _lCRDD_02, _lCRDD_01 	;; 20h - 2fh
	db	_lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00	;; 30h - 3fh
	db	_lCRDD_00, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02	;; 40h - 4fh
	db	_lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00	;; 50h - 5fh
	db	_lCRDD_00, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02	;; 60h - 6fh
	db	_lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_02, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00	;; 70h - 7fh

	_lCRDD_00	equ		offset _lCRScanBS - _lBSRemBase
	_lCRDD_01	equ		offset _lCRIncDec - _lBSRemBase
	_lCRDD_02	equ		offset _lCRBSNext - _lBSRemBase
	_lCRDD_03	equ		offset _lCRBSAdd  - _lBSRemBase

	;;
	;; remove bs 01
	;;
	_bAscii_02 \
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00
	db	_lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_03, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_03, _lCRDD_00
	db	_lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00
	db	_lCRDD_00, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03
	db	_lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_03
	db	_lCRDD_00, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03
	db	_lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_03, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00, _lCRDD_00

	;;----------------
	;; for debug
	;;_sDbgSrc	db		"%d", 00h
	;;_sDbgDest	db		20h	dup(00h)
	;;----------------

;;-------------------------------------------------------------------------
.data?

	_sTime			db	09h	dup(?)
	_sDate			db	0bh	dup(?)
	_dCounterV		dd	?

	_bCodeSys		db	?	;; 0 = not return and not if construction
	_bCodePosOp		db	?	;; 0 = no post operation (++ or --)

	_bTempBool		db	?
	_bTempType		db	?

	_dBuffer		dd	?	;; buffer
	_hOLMacro		dd	?	;; used in macro overloading

	_dGuiFont		dd	?
	_dWWWFont		dd	?	;; used in string preprocessor

	_fScr			dd	?

	_dUndefPnt		dd	?	;; script position in undefined

	_hWnd			dd	?
	_hPrg			dd	?
	_hList			dd	?
	_hBtn			dd	?	;; used in #if ...

;;	_hIconCJ		dd	?	
	_hTrd			dd	?

	_dStackPos		dd	?
	_dPreStackPos		dd	?

	_dErrorCodeStart	dd	?	;; edi position of bad code
	_xErrorTable		dd	08h	dup(?)

	_dStdCursor		dd	?
	_dExCursor		dd	?

	_sMapPath		db	0200h	dup(?)
	_dMapPathToEX		equ	0200h
	_sMapPathEX		db	0200h	dup(?)	;; for include
	_dMapPathEnd		dd	?

	_sCurrDir		db	0200h	dup(?)
	_dCurrDirEnd		dd	?

	align			10h	
	_lDefX			dd	00010000h	dup(?)	;; 80h bits defBlocks

	_dScopeIn		dd	0100h	dup(?)
	_dScopeOut		dd	0100h	dup(?)

	_dDefTable		dd	0080h	dup(?)		;; used in f'n'p	;; used in code romoning (variables)
	_dDefTableEX		dd	0080h	dup(?)					;; used in code romoning (function)

	_dDefArgs		dd	0080h	dup(?)

	_xPntStr		PAINTSTRUCT	 <?>

	_dFCL			dd	?		;; locals	
	_dFCB			dd	?		;; base code of function

	_dBCP			dd	?		;; base code pointer

	align			10h
	_bFuncCodeLocals	db	00010000h	dup(?)	;; locals			;; used also in string preprocessor	;; used in guard	;; used in removing unused code - variables
	align			10h
	_bFuncCodeBase		db	000a0000h	dup(?)	;; base code of function	;; used also in string preprocessor				;; used in removing unused code - functions
	_bFuncCodeOneLine	db	00004000h	dup(?)	;; one line of func code
	_bFuncPostEX		db	00000200h	dup(?)	;; postix index

	_xEnumStr		db	00008000h	dup(?)	;; enum string
	_xEnumTable		dd	00008000h	dup(?)	;; enum structs			;; used also in #if
	_xEnumLabel		dd	00000100h	dup(?)	;; enum labels

	_xSysTime		dw	08h		dup(?)	;; system time

	_dVarParams		dd	?		;; variable group params

	_dLastFuncName		dd	?		;; used in FUNCNAME macro

	_bIsPPCEn		db	?		;; in #if/#endif block

	_xSynDesc		dd	0400h		dup(?)	;; ex syntax (zinc and boa) descriptor

	_bStrXX			db	?		;; uses to add bs at strings

;;-------------------------------------------------------------------------
.code

	_next	equ	@f
	_prew	equ	@b
	_lbl	equ	@@

	;;----------------
	;; debug print reg
	_printReg macro reg
	pushad
	
	push	reg
	push	offset _sDbgSrc
	push	offset _sDbgDest
	
	call	_imp__wsprintfA
	add	esp,			0ch
	
	push	00h
	push	00h
	push	offset _sDbgDest
	push	00h
	call	_imp__MessageBoxA@16	
	
	popad
	endm
	;;----------------

	;;-------------------------------------------------------------------------

	;;	struct defBlock
	;;
	;;	_sDFLabel	dd	;; adress of string [define label]
	;;	_sDFValue	dd	;; adress of string [define value]
	;;	_sDFScope	dd	;; scope id
	;;	_dValueLenght	dd	;; arg count
		_dDFSize	equ	10h

	;;	struct errorTable
	;;
	;;	_hString	dd	;; offset of info string
	;;	_hStarsS	dd	;; selection end
	;;	_hEndS		dd	;; selection start
	;;			dd	;; not use

	;;	struct enumLabels
	;;
	;;	_hName		dd	;; pointer to string
	;;	_hNegBase	dd	;; negative enumTable
	;;	_hPosBase	dd	;; positive enumTable

	;;	struct enumTable
	;;
	;;	_hMin		dd
	;;	_hMax		dd
	;;	_hNext		dd	;; next enumTable

	;;	struct globVar/func
	;;
	;;	_hAddrS		dd	;; address - start
	;;	_hAddrE		dd	;; end
	;;	_hName		dd	;; var Name
	;;	_wCnt		dw	;; counter
	;;	_		db
	;;	_		db

	;;	struct funcCode
	;;
	;;	_hAddrS		dd	;; address - start
	;;	_hAddrE		dd	;; end
	;;	_hName		dd	;; var Name
	;;	_wCnt		dw	;; counter
	;;	_bIsChecked	db	;; is checked
	;;	_		db

	;;-------------------------------------------------------------------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc optimize
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_lMapOptimizeCode:

	;;----------------
	mov	_dCurrStr,			offset _sProg_06
	mov	eax,				10h
	call	_lSetProg
	;;----------------

	;;----------------
	;; code preparation
	xor	ebx,			ebx	;; use bl only
	jmp	_lOptPreStartEX
	_lOptPreStart:
	movsb
	_lOptPreStartEX:
	mov	eax,			dword ptr [esi]

		;;----------------
		;; nl
		cmp	ax,			0a0dh	;; nl
		jne	_next

		_lOptAddNLEX:
		cmp	byte ptr [edi-01h],	0ah	;; nl
		jne	_lOptAddNL
		add	esi,			02h
		jmp	_lOptPreStartEX

		_lOptAddNL:
		mov	byte ptr [edi],		0ah	;; nl
		add	esi,			02h
		inc	edi
		jmp	_lOptPreStartEX
		;;----------------

		;;----------------
		;; comments
		_lbl:
		cmp	ax,			2f2fh	;; //
		jne	_next

		_lOptComm:
		inc	esi
		cmp	byte ptr [esi],		00h
		je	_lOptPreEnd
		cmp	word ptr [esi],		0a0dh
		jne	_lOptComm
		jmp	_lOptAddNLEX
		;;----------------

		;;----------------
		;; strings
		_lbl:
		cmp	eax,			2b202222h	;; "" +
		jne	_next
		add	esi,			04h
		jmp	_lOptPreStartEX

		_lbl:
		cmp	eax,			2222202bh	;; + ""
		jne	_next
		add	esi,			04h
		jmp	_lOptPreStartEX

		_lbl:
		cmp	eax,			202b2022h	;; " + 
		jne	_next
		cmp	byte ptr [esi+04h],	22h		;; "
		jne	_next
		add	esi,			05h
		jmp	_lOptPreStartEX

		_lbl:
		cmp	al,			22h		;; "
		jne	_next

		_lOptStr:
		movsb
		_lOptStrSX:
		cmp	byte ptr [esi],		5ch		;; \ 
		je	_lOptStrEX
		cmp	byte ptr [esi],		22h		;; "
		jne	_lOptStr
		jmp	_lOptPreStart

		_lOptStrEX:
		movsw
		jmp	_lOptStrSX
		;;----------------

		;;----------------
		;; bs
		_lbl:
		cmp	al,				20h	;; bs
		jne	_next
		jmp	_lOptRemoveBS

		_lbl:
		cmp	al,				09h	;; tab
		jne	_next

		_lOptRemoveBS:
		;; prew char
		mov	bl,				byte ptr [edi-01h]
		cmp	byte ptr [_bAscii_00+ebx],	bh	;; bh = 00h
		jne	_lOptBS

		inc	esi
		jmp	_lOptPreStartEX

		;; next char
		_lOptBS:
		mov	bl,				ah
		cmp	byte ptr [_bAscii_00+ebx],	bh	;; bh = 00h
		jne	_lOptPreStart

		inc	esi
		jmp	_lOptPreStartEX
		;;----------------

		;;----------------
		;; null
		_lbl:
		cmp	al,			00h	;; null
		jne	_lOptPreStart

		cmp	byte ptr [edi-01h],	0ah	;; nl
		jne	_lOptPreEnd
		dec	edi
		mov	byte ptr [edi],		00h	;; null

		;;----------------

	_lOptPreEnd:
	;;----------------

	;;----------------
	mov	_dCurrStr,			offset _sProg_07
	mov	eax,				20h
	call	_lSetProg
	;;----------------

	;;----------------
	;; removing unused code
	cmp	dword ptr [_dWarVerSL],			00h
	jne	_next

	add	esi,					04h
	jmp	_lOptCC_NoRemove

	_lbl:
	push	edi
	push	esi


		;;----------------
		;; variables
		mov	ebx,				offset _bFuncCodeLocals
		add	esi,				0ch	;; globals_

		_lOptCC_VarStr:
		cmp	dword ptr [esi],		67646e65h	;; endg
		jne	_lOptCC_VarAdd
		cmp	dword ptr [esi+04h],		61626f6ch	;; loba
		jne	_lOptCC_VarAdd
		cmp	word ptr [esi+08h],		736ch		;; ls
		jne	_lOptCC_VarAdd
		cmp	byte ptr [esi+0ah],		0ah		;; nl
		jne	_lOptCC_VarAdd

		add	esi,				0bh

			;;----------------
			;; go to sort
			push	esi			;; store esi before sort
			cmp	ebx,			offset _bFuncCodeLocals+10h
			jbe	_lOptCC_VarNoSort

			mov	ebp,			offset _bFuncCodeLocals
			sub	ebx,			ebp
			call	_lOptCC_SortIn

			_lOptCC_VarNoSort:
			mov	eax,			offset _bFuncCodeLocals-10h
			mov	edx,			offset _dDefTable
			push	offset _lOptCC_Func
			jmp	_lOptCC_BuildIn
			;;----------------

		_lOptCC_VarAdd:
		mov	dword ptr [ebx],		esi		;; start

		cmp	dword ptr [esi],		736e6f63h	;; cons
		jne	_next
		cmp	dword ptr [esi+04h],		746e6174h	;; tant
		jne	_next
		cmp	byte ptr [esi+08h],		20h		;; bs
		add	esi,				09h		;; remove constant
		_lbl:
		inc	esi
		cmp	byte ptr [esi],			20h		;; bs
		jne	_prew
		inc	esi

		cmp	dword ptr [esi],		61727261h	;; arra
		jne	_next
		cmp	word ptr [esi+04h],		2079h		;; y_
		jne	_next
		add	esi,				06h
		_lbl:

		mov	dword ptr [ebx+08h],		esi		;; var name

		_lbl:
		inc	esi
		cmp	byte ptr [esi],			0ah		;; nl
		jne	_prew
		inc	esi
		mov	dword ptr [ebx+04h],		esi		;; end
		add	ebx,				10h

		jmp	_lOptCC_VarStr
		;;----------------

		;;----------------
		;; functions
		_lOptCC_Func:
		pop	esi
		mov	ebx,				offset _bFuncCodeBase

		_lOptCC_FuncStr:
		cmp	byte ptr [esi],			00h
		jne	_lOptCC_FuncAdd

			;;----------------
			;; go to sort
			cmp	ebx,			offset _bFuncCodeBase+10h
			jbe	_lOptCC_FuncNoSort

			mov	ebp,			offset _bFuncCodeBase
			sub	ebx,			ebp
			call	_lOptCC_SortIn

			_lOptCC_FuncNoSort:
			mov	eax,			offset _bFuncCodeBase-10h
			mov	edx,			offset _dDefTableEX
			push	offset _lOptCC_ChStr
			jmp	_lOptCC_BuildIn
			;;----------------

		_lOptCC_FuncAdd:
		mov	dword ptr [ebx],		esi
		cmp	dword ptr [esi],		736e6f63h	;; cons
		jne	_next
		add	esi,				09h
		_lbl:
		add	esi,				09h
		mov	dword ptr [ebx+08h],		esi

		_lbl:
		inc	esi
		cmp	dword ptr [esi],		6b617420h	;; _tak
		jne	_prew

		cmp	dword ptr [esi+07h],		68746f6eh	;; noth
		jne	_lOptCC_FuncGetEnd
		cmp	dword ptr [esi+0bh],		20676e69h	;; ing_
		jne	_lOptCC_FuncGetEnd

			;;----------------
			;; is optional
			mov	word ptr [ebx+0ch],		01h
			;;----------------

		_lOptCC_FuncGetEnd:
		inc	esi
		cmp	dword ptr [esi],		646e650ah	;; _end
		jne	_lOptCC_FuncGetEnd
		cmp	dword ptr [esi+04h],		636e7566h	;; func
		jne	_lOptCC_FuncGetEnd

		add	esi,				0dh
		mov	dword ptr [ebx+04h],		esi
		add	ebx,				10h
		jmp	_lOptCC_FuncStr
		;;----------------

		;;----------------
		;; sort
			;;----------------
			;; set step
			_lOptCC_SortIn:
			mov	eax,			offset _dSortSteps-04h
			_lbl:
			add	eax,			04h
			mov	ecx,			dword ptr [eax+04h]
			lea	ecx,			dword ptr [ecx+ecx*02h]
			cmp	ebx,			ecx
			jg	_prew
			;;----------------

		_lOptCC_SortStr:
		mov	ecx,			dword ptr [eax]
		lea	ebx,			dword ptr [ebp+ecx]

		_lOptCC_SortGo:
		mov	esi,			dword ptr [ebx+08h]
		mov	edi,			ebx
		mov	dl,			byte ptr [esi]
		movaps	xmm1,			[ebx]

		_lbl:
		sub	edi,			ecx
		cmp	edi,			ebp
		jb	_lOptCC_SortNext
		mov	esi,			dword ptr [edi+08h]
		cmp	byte ptr [esi],		dl
		jb	_lOptCC_SortNext

		movaps	xmm0,			[edi]
		movaps	[edi+ecx],		xmm0

		jmp	_prew

		_lOptCC_SortNext:
		movaps	[edi+ecx],		xmm1
		add	ebx,			10h
		cmp	dword ptr [ebx],	00h
		jnz	_lOptCC_SortGo

		sub	eax,			04h
		cmp	eax,			offset _dSortSteps-04h
		jne	_lOptCC_SortStr

		_lOptCC_SortEnd:
		retn
		;;----------------

		;;----------------
		;; build table
		_lOptCC_BuildIn:
		xor	ebx,			ebx
		xor	ecx,			ecx

		_lOptCC_VarDT:
		add	eax,			10h
		cmp	dword ptr [eax],	00h
		je	_lOptCC_VarDTEnd

		mov	ebp,			dword ptr [eax+08h]
		mov	bl,			byte ptr [ebp]
		cmp	cl,			bl
		je	_lOptCC_VarDT
		mov	dword ptr[edx+ebx*04h],	eax
		mov	cl,			bl
		jmp	_lOptCC_VarDT

		_lOptCC_VarDTEnd:
		retn
		;;----------------

		;;----------------
		;; checking
		_lOptCC_ChStr:

			;;----------------
			;; check next function
			_lOptCC_ChNext:
			xor	ebx,			ebx
			_lOptCC_ChNextEX:
			mov	eax,			offset _bFuncCodeBase

			_lOptCC_ChIn:
			cmp	dword ptr [eax],	00h
			je	_lOptCC_RemIn

			cmp	word ptr [eax+0ch],	00h
			jne	_lOptCC_ChCall
			add	eax,			10h
			jmp	_lOptCC_ChIn

			_lOptCC_ChCall:
			cmp	byte ptr [eax+0eh],	00h
			je	_lOptCC_ChInFunc
			add	eax,			10h
			jmp	_lOptCC_ChIn

				;;----------------
				;; in function
				_lOptCC_ChInFunc:
				mov	esi,			dword ptr [eax+08h]
				mov	byte ptr [eax+0eh],	01h

				_lbl:
				inc	esi
				cmp	byte ptr [esi-01h],	0ah		;; nl
				jne	_prew

				_lOptCC_ChInFunc_GetWord:
				mov	bl,			byte ptr [esi]
				cmp	byte ptr [_bAscii_00+ebx],	bh
				je	_lOptCC_ChInFunc_Inc

				cmp	dword ptr [esi],	66646e65h	;; endf
				jne	_lOptCC_ChInFunc_CheckWord
				cmp	byte ptr [esi-01h],	0ah		;; nl
				je	_lOptCC_ChNextEX

					;;----------------
					_lOptCC_ChInFunc_CheckWord:
					mov	bl,			byte ptr [esi]
					lea	edx,			[_dDefTable+ebx*04h]
					mov	edx,			dword ptr [edx]
					test	edx,			edx
					jz	_lOptCC_ChInFunc_ReCheck

					_lOptCC_ChInFunc_CheckWordEX:
					mov	ecx,			esi
					mov	ebp,			dword ptr [edx+08h]
					mov	bl,			byte ptr [ecx]
					cmp	byte ptr [ebp],		bl
					jne	_lOptCC_ChInFunc_ReCheck
					inc	ecx
					inc	ebp

					_lOptCC_ChInFunc_CheckWordFX:
					mov	bl,			byte ptr [ecx]
					cmp	byte ptr [_bAscii_00+ebx],	bh
					je	_lOptCC_ChInFunc_CheckWordOX

					cmp	byte ptr [ebp],		bl
					jne	_lOptCC_ChInFunc_NextFX
					inc	ebp
					inc	ecx
					jmp	_lOptCC_ChInFunc_CheckWordFX

					_lOptCC_ChInFunc_CheckWordOX:
					mov	bl,			byte ptr [ebp]
					cmp	byte ptr [_bAscii_00+ebx],	bh
					jne	_lOptCC_ChInFunc_NextFX

					inc	word ptr [edx+0ch]
					;;----------------

				_lOptCC_ChInFunc_NextWord:
				inc	esi
				mov	bl,			byte ptr [esi]
				cmp	byte ptr [_bAscii_00+ebx],	bh
				jne	_lOptCC_ChInFunc_NextWord
				jmp	_lOptCC_ChInFunc_GetWord

				_lOptCC_ChInFunc_NextFX:
				add	edx,			10h
				cmp	dword ptr [edx],	00h
				je	_lOptCC_ChInFunc_ReCheck
				jmp	_lOptCC_ChInFunc_CheckWordEX

				_lOptCC_ChInFunc_Inc:
				cmp	bl,			22h	;; "
				je	_lOptCC_ChInFunc_IncStr
				inc	esi
				jmp	_lOptCC_ChInFunc_GetWord

				_lOptCC_ChInFunc_IncStrEX:
				inc	esi
				_lOptCC_ChInFunc_IncStr:
				inc	esi
				cmp	byte ptr [esi],		5ch	;; \ 
				je	_lOptCC_ChInFunc_IncStrEX
				cmp	byte ptr [esi],		22h	;; "
				jne	_lOptCC_ChInFunc_IncStr
				inc	esi
				jmp	_lOptCC_ChInFunc_GetWord

				_lOptCC_ChInFunc_ReCheck:
				cmp	edx,			offset _bFuncCodeBase
				jge	_lOptCC_ChInFunc_NextWord

				mov	bl,			byte ptr [esi]
				lea	edx,			[_dDefTableEX+ebx*04h]
				mov	edx,			dword ptr [edx]
				test	edx,			edx
				jz	_lOptCC_ChInFunc_NextWord
				jmp	_lOptCC_ChInFunc_CheckWordEX
				;;----------------
			;;----------------

			;;----------------
			;; remove unused
			_lOptCC_RemIn:
			mov	al,		02h

				;;----------------
				;; functions
				mov	edx,			offset _bFuncCodeBase-10h

				_lOptCC_RemFunc:
				add	edx,			10h
				cmp	dword ptr [edx],	00h
				je	_lOptCC_RemVarIn

				cmp	word ptr [edx+0ch],	00h
				jne	_lOptCC_RemFunc

					;;----------------
					;; remove
					mov	edi,			dword ptr [edx]
					mov	ecx,			dword ptr [edx+04h]
					sub	ecx,			edi
					rep	stosb

					jmp	_lOptCC_RemFunc
					;;----------------
				;;----------------

				;;----------------
				;; variables
				_lOptCC_RemVarIn:
				mov	edx,			offset _bFuncCodeLocals-10h

				_lOptCC_RemVar:
				add	edx,			10h
				cmp	dword ptr [edx],	00h
				je	_lOptCC_RemParse

				cmp	word ptr [edx+0ch],	00h
				jne	_lOptCC_RemParse

					;;----------------
					;; remove
					mov	ebp,			dword ptr [edx+08h]

					_lbl:
					inc	ebp
					cmp	byte ptr [ebp],		0ah		;; nl
					je	_lOptCC_RemVarEX
					cmp	byte ptr [ebp],		3dh		;; =
					jne	_prew

						;;----------------
						;; check
						_lOptCC_RemVarCheckStart:
						inc	ebp
						cmp	byte ptr [ebp],		0ah		;; nl
						je	_lOptCC_RemVarEX
						cmp	byte ptr [ebp],		28h		;; (
						jne	_lOptCC_RemVarCheckStart

						mov	bl,			byte ptr [ebp-01h]
						cmp	byte ptr [_bAscii_00+ebx],	bh
						je	_lOptCC_RemVarCheckStart

							;;----------------
							mov	esi,			offset _sCodeNXFunc

							_lOptCC_RemVarCheck:
							lea	ecx,			[ebp-01h]

							_lbl:
							mov	bl,			byte ptr [ecx]
							cmp	bl,			byte ptr [esi]
							jne	_lOptCC_RemVarCheckFX
							dec	ecx
							inc	esi
							jmp	_prew

							_lOptCC_RemVarCheckFX:
							cmp	byte ptr [_bAscii_00+ebx],	bh
							jne	_lOptCC_RemVarCheckNext
							cmp	byte ptr [esi],		00h
							je	_lOptCC_RemVarCheckStart

							_lOptCC_RemVarCheckNext:
							cmp	byte ptr [esi],		00h
							jne	_lOptCC_RemVarCheckNextEX
							inc	esi
							cmp	byte ptr [esi],		00h
							jne	_lOptCC_RemVarCheck
							jmp	_lOptCC_RemVarCheckStart

							_lOptCC_RemVarCheckNextEX:
							inc	esi
							jmp	_lOptCC_RemVarCheckNext
							;;----------------

						;;----------------

						;;----------------
						_lOptCC_RemVarEX:
						mov	edi,			dword ptr [edx]
						mov	ecx,			dword ptr [edx+04h]
						sub	ecx,			edi
						rep	stosb

						jmp	_lOptCC_RemVar
						;;----------------
					;;----------------
				;;----------------

				;;----------------
				;; parse code
				_lOptCC_RemParse:
				pop	esi
				pop	edi
				add	esi,			04h
				add	edi,			04h

				_lOptCC_RemParseEX:
				lodsb
				cmp	al,			02h
				je	_lOptCC_RemParseEX
				test	al,			al
				jz	_lOptCC_RemParseEnd
				stosb
				jmp	_lOptCC_RemParseEX

				_lOptCC_RemParseEnd:
				add	esi,			03h
				;;----------------
			;;----------------
		;;----------------
	_lOptCC_NoRemove:
	;;----------------

	;;----------------
	mov	_dCurrStr,			offset _sProg_08
	mov	eax,				64h
	call	_lSetProg
	;;----------------

	sub	edi,			esi		;; edi = new script size

	retn

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; optimize endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc parse
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;; esi=Src
	;; edi=Dest

	;; stack:
	;;
	;; edi
	;; mem
	;; mem
	;; archive
	;; attributes

	_lMapParseCode:

	;;----------------
	mov	_dCurrStr,			offset _sProg_01
	mov	eax,				10h
	call	_lSetProg
	;;----------------

	;;----------------
	;; comments removing

		;;----------------
		;; utf 8 bom
		cmp	word ptr [esi],		0bbefh
		jne	_lCrStr
		cmp	byte ptr [esi+02h],	0bfh
		jne	_lCrStr

		add	esi,			03h
		;;----------------

	_lCrStr:
	push	00h					;; for safe
	mov	_dStackPos,			esp	;; save stack
	mov	dword ptr [_dErrorCodeStart],	edi	;; for syntax error
	jmp	_lCRScanLineSx

		;;----------------
		;; line start
		_lCRScanLine:

			;;----------------
			cmp	dword ptr [_hBtn],	00h
			je	_lCRScanLineFx

			mov	dword ptr [_hBtn],	00h
			lea	eax,			[edi+02h]
			push	eax
			mov	dword ptr [edi],	78016101h		;; #a#x
			add	edi,			08h
			jmp	_lCRScanLineDx
			;;----------------

		_lCRScanLineFx:
		cmp	word ptr [edi-02h],	0a0dh	;; nl
		je	_lCRScanLineDx
		cmp	byte ptr [edi-01h],	00h
		je	_lCRScanLineDx
		mov	word ptr [edi],		0a0dh	;; nl
		add	edi,			02h
		_lCRScanLineDx:
		cmp	byte ptr [esi],		0ah
		jne	_next
		inc	esi
		jmp	_lCRScanLineDx
		_lbl:
		cmp	byte ptr [esi],		0dh
		jne	_lCRScanLineSx
		inc	esi
		jmp	_lCRScanLineDx

		_lCRScanLineSx:			;; <---
		mov	eax,			dword ptr [esi]
		;;----------------

		;;----------------
		;; remove bs and tabs in line start
		_lbl:
		cmp	al,			09h
		jne	_next
		inc	esi
		jmp	_lCRScanLineSx

		_lbl:
		cmp	al,			20h
		jne	_next
		inc	esi
		jmp	_lCRScanLineSx
		;;----------------

		;;----------------
		;; define test
		_lbl:
		cmp	byte ptr [_bIsPPCEn],	00h
		je	_next

		cmp	eax,			69666564h	;; defi
		jne	_next
		cmp	word ptr [esi+04h],	656eh		;; ne
		jne	_next
		cmp	byte ptr [esi+06h],	20h
		jg	_next

		_lDefErrFGS:
		mov	dword ptr [_xErrorTable],	offset _sErr_FailedDef
		mov	dword ptr [_xErrorTable+04h],	edi
		_lDefErrFGSEX:
		movsb
		cmp	word ptr [esi],			0a0dh	;; nl
		jne	_lDefErrFGSEX
		mov	dword ptr [_xErrorTable+08h],	edi
		jmp	_lErrIn	
		;;----------------

		;;----------------
		;; pre proc
		_lbl:
		cmp	al,			23h		;; #
		jne	_next

			;;----------------
			;; #if
			cmp	eax,			20666923h	;; #if_
			jne	_lCRElifDX
			mov	byte ptr [_bIsPPCEn],	al

			mov	dword ptr [_hBtn],	eax
			mov	word ptr [edi],		6901h		;; #i
			add	esi,			02h
			add	edi,			02h
			jmp	_lCRScan

			_lCRElifDX:
			cmp	word ptr [esi+01h],	6669h		;; if
			jne	_lCRElifOS

				;;----------------
				;; error
				_lCRErrPrePorc:
				mov	dword ptr [_xErrorTable],	offset _sErr_PreProc
				mov	dword ptr [_xErrorTable+04h],	edi
				_lCRErrPrePorcEX:
				movsb
				cmp	word ptr [esi],			0a0dh	;; nl
				jne	_lCRErrPrePorcEX
				mov	dword ptr [_xErrorTable+08h],	edi
				jmp	_lErrIn
				;;----------------
			;;----------------

			;;----------------
			;; #else #elseif

				;;----------------
				;; close
				_lCRElifOX:
				mov	dword ptr [_hBtn],	eax
				_lCRElifFF:
				mov	word ptr [esi],		6901h		;; #i

				pop	eax
				test	eax,			eax
				jz	_lBlockErr

				mov	dword ptr [eax+02h],	edi
				mov	word ptr [edi],		7901h		;; #y
				mov	dword ptr [edi+02h],	eax
				mov	dword ptr [edi+06h],	06060606h	;; ex backspace
				mov	dword ptr [edi+0ah],	06060606h
				mov	word ptr [edi+0eh],	0a0dh		;; new line

				add	edi,			10h
				movsd
				jmp	_lCRScan
				;;----------------

			_lCRElifOS:
			cmp	eax,			736c6523h	;; #els
			jne	_lCRElifER
			cmp	dword ptr [esi+04h],	20666965h	;; eif_
			je	_lCRElifOX

			cmp	byte ptr [esi+04h],	65h		;; e
			jne	_lCRElifER
			cmp	byte ptr [esi+05h],	20h
			jbe	_lCRElifOX
			;;----------------

			;;----------------
			;; error
			_lCRElifER:
			cmp	eax,			72726523h	;; #err
			jne	_lCRElif_AAX
			cmp	word ptr [esi+04h],	726fh		;; or
			jne	_lCRElif_AAX
			cmp	byte ptr [esi+06h],	22h
			jg	_lCRElif_AAX

			mov	word ptr [esi],		6701h		;; #g
			movsd
			movsw
			jmp	_lCRScan
			;;----------------

			;;----------------
			_lCRElif_AAX:
			cmp	eax,			66656423h	;; #def
			je	_lCRElif_SOX
			cmp	eax,			74657323h	;; #set
			je	_lCRElif_OOX
			cmp	eax,			646e7523h	;; #und
			je	_lCRElif_OOX
			cmp	eax,			636e6923h	;; #inc
			jne	_lCRElif_GU

			_lCRElif_OOX:
			inc	esi
			jmp	_lCRScanLineSx

			_lCRElif_SOX:
			inc	esi
			cmp	byte ptr [_bIsPPCEn],	00h
			je	_lCRScanLineSx
			jmp	_lDefErrFGS
			;;----------------

			;;----------------
			;; guard
			_lCRElif_GU:
			cmp	eax,			61756723h	;; #gua
			jne	_lCRElifFX
			cmp	word ptr [esi+04h],	6472h		;; rd
			jne	_lCRElifFX

			add	esi,			06h
			xor	eax,			eax

			_lCRElif_GU_01:
			inc	esi
			cmp	byte ptr [esi],		20h		;; bs
			je	_lCRElif_GU_01
			cmp	byte ptr [esi],		09h		;; tab
			je	_lCRElif_GU_01

			mov	ebp,			offset _bFuncCodeLocals

			_lCRElif_GU_02:
			mov	edx,			esi

			_lCRElif_GU_03:
			mov	al,			byte ptr [edx]
			cmp	al,			byte ptr [ebp]
			jne	_lCRElif_GU_04
			inc	ebp
			inc	edx
			jmp	_lCRElif_GU_03

			_lCRElif_GU_04:
			cmp	byte ptr [ebp],		00h
			jne	_lCRElif_GU_Next
			cmp	byte ptr [_bAscii_00+eax],	ah
			je	_lRetnGuard

			_lCRElif_GU_Next:
			inc	ebp
			cmp	byte ptr [ebp],		00h
			jne	_lCRElif_GU_Next
			inc	ebp
			cmp	byte ptr [ebp],		00h
			jne	_lCRElif_GU_02

			_lCRElif_GU_Add:
			mov	al,			byte ptr [esi]
			cmp	byte ptr [_bAscii_00+eax],	ah
			je	_lCRScan
			mov	byte ptr [ebp],		al
			inc	esi
			inc	ebp
			jmp	_lCRElif_GU_Add
			;;----------------

			;;----------------
			;; #endif
			_lCRElifFX:
			cmp	eax,			646e6523h	;; #end
			jne	_lCRErrPrePorc
			cmp	word ptr [esi+04h],	6669h		;; if
			jne	_lCRErrPrePorc
			mov	byte ptr [_bIsPPCEn],	00h

			jmp	_lCRElifFF
			;;----------------
		;;----------------

		;;----------------
		;; textmacros

			;;----------------
			;; start
			_lCRTTInOnce:
			cmp	dword ptr [esi+08h],	636e5f6fh	;; o_nc
			jne	_next
			cmp	word ptr [esi+0ch],	2065h		;; e_
			jne	_next
			add	esi,			0dh
			jmp	_lCRTTInOX

			_lbl:
			cmp	eax,			74786574h	;; text	
			jne	_next
			cmp	dword ptr [esi+04h],	7263616dh	;; macr
			jne	_next
			cmp	word ptr [esi+08h],	206fh		;; o_
			jne	_lCRTTInOnce

			add	esi,			09h
			_lCRTTInOX:
			mov	dword ptr [edi-04h],	69666564h	;; defi
			mov	word ptr [edi],		656eh		;; ne
			add	edi,			02h

			mov	eax,			esi
			_lCRTTIn:
			inc	eax
			cmp	byte ptr [eax],		0ah		;; nl
			je	_lCRTTInEnd
			cmp	dword ptr [eax],	656b6174h	;; take
			jne	_lCRTTIn
			cmp	word ptr [eax+04h],	2073h		;; s_
			jne	_lCRTTIn

			mov	dword ptr [eax],	20202020h	;; bs
			mov	word ptr [eax+04h],	1320h		;; _( 

			_lCRTTInEX:
			inc	eax
			cmp	byte ptr [eax],		2ch		;; ,
			jne	_lCRTTInFF
			mov	byte ptr [eax],		14h
			jmp	_lCRTTInEX

			_lCRTTInFF:
			cmp	byte ptr [eax],		0ah		;; nl
			jne	_lCRTTInEX

			cmp	byte ptr [eax-01h],	0dh
			jne	_lCRTTInFX
			mov	byte ptr [eax-01h],	20h		;; bs

			_lCRTTInFX:
			mov	byte ptr [eax],		11h
			jmp	_lCRScan

			_lCRTTInEnd:
			cmp	byte ptr [eax-01h],	0dh
			jne	_lCRTTInDX
			mov	byte ptr [eax-01h],	20h
			_lCRTTInDX:
			mov	byte ptr [eax],		12h
			jmp	_lCRScan
			;;----------------

			;;----------------
			;; end
			_lbl:
			cmp	eax,			74646e65h	;; endt
			jne	_next
			cmp	dword ptr [esi+04h],	6d747865h	;; extm
			jne	_next
			cmp	dword ptr [esi+08h],	6f726361h	;; acro
			jne	_next
			cmp	byte ptr [esi+0ch],	20h		;; _
			jg	_next

			sub	edi,			04h
			add	esi,			0ch

			pop	eax
			test	eax,			eax
			jz	_lBlockErr

			cmp	word ptr [edi-02h],	0a0dh		;; new line
			je	_lCLTMNE
			mov	word ptr [edi],		0a0dh		;; new line
			add	edi,			02h
			_lCLTMNE:
			mov	dword ptr [eax+02h],	edi
			mov	word ptr [edi],		7901h		;; #y
			mov	dword ptr [edi+02h],	eax
			mov	dword ptr [edi+06h],	06060606h	;; ex backspace
			mov	dword ptr [edi+0ah],	06060606h
			mov	word ptr [edi+0eh],	0a0dh		;; new line

			add	edi,			10h
			jmp	_lCRScanLine
			;;----------------

			;;----------------
			;; run
			_lbl:
			cmp	eax,			746e7572h	;; runt
			jne	_next
			cmp	dword ptr [esi+04h],	6d747865h	;; extm
			jne	_next
			cmp	dword ptr [esi+08h],	6f726361h	;; acro
			jne	_next
			cmp	byte ptr [esi+0ch],	20h		;; _
			jne	_next

			lea	eax,			[esi+0ah]
			sub	edi,			04h
			add	esi,			0dh
			mov	ebx,			eax

			_lCRMacroRun:
			inc	eax
			cmp	byte ptr [eax],		22h		;; "
			jne	_lCRMacroRunEX

			test	ebx,			ebx
			jz	_lCRMacroRunB

			_lCRMacroRunA:
			mov	byte ptr [eax],		0bh
			xor	ebx,			ebx

				;;----------------
				;; special for Strilanc
				cmp	word ptr [eax+01h],	2f2fh		;; //
				jne	_lCRMacroRun

				mov	word ptr [eax+01h],	3501h		;; #5
				jmp	_lCRMacroRun
				;;----------------

			_lCRMacroRunB:
			mov	byte ptr [eax],		0bh
			inc	ebx
			jmp	_lCRMacroRun

			_lCRMacroRunEX:
			cmp	byte ptr [eax],		0ah		;; nl
			jne	_lCRMacroRun

			cmp	ebx,			01h
			jbe	_lCRScan

			_lCRMacroRunSF:
			dec	eax
			cmp	byte ptr [eax],		0ah
			je	_lCRScan
			cmp	byte ptr [eax],		28h		;; ( 
			je	_lCRMacroRunDF
			cmp	byte ptr [eax],		29h		;; )  
			jne	_lCRMacroRunSF
			_lCRMacroRunDF:
			mov	byte ptr [eax],		20h		;; bs
			jmp	_lCRMacroRunSF
			;;----------------
		;;----------------

		;;----------------
		;; zinc and boa
		_lbl:
		cmp	eax,			636e697ah	;; zinc
		jne	_next
		cmp	byte ptr [esi+04h],	20h
		jg	_next
		jmp	_lCRFXSyn

		_lbl:
		cmp	ax,			6f62h		;; bo
		jne	_next
		cmp	byte ptr [esi+02h],	61h		;; a
		jne	_next
		cmp	byte ptr [esi+03h],	20h
		jg	_next

		_lCRFXSyn:
		mov	eax,			dword ptr [_dSynDesc]
		mov	dword ptr [eax],	esi
		mov	dword ptr [edi-04h],	0a0d3901h		;; #9 nl

		_lCREndFXSyn:
		inc	esi

;;		cmp	byte ptr [esi],		00h
;;		je

		cmp	dword ptr [esi],	7a646e65h	;; endz
		jne	_lCREndFXSynEX
		cmp	word ptr [esi+04h],	6e69h		;; in
		jne	_lCREndFXSynEX
		cmp	byte ptr [esi+06h],	63h		;; c
		jne	_lCREndFXSynEX
		cmp	byte ptr [esi+07h],	20h		;; bs
		jb	_lCREndFXSynDX

		_lCREndFXSynEX:
		cmp	dword ptr [esi],	62646e65h	;; endb
		jne	_lCREndFXSyn
		cmp	word ptr [esi+04h],	616fh		;; oa
		jne	_lCREndFXSyn
		cmp	byte ptr [esi+06h],	20h		;; bs
		jg	_lCREndFXSyn

		_lCREndFXSynDX:
		inc	esi
		cmp	word ptr [esi-02h],	0a0dh
		jne	_lCREndFXSynDX

		mov	dword ptr [eax+04h],	esi
		add	eax,			08h
		mov	dword ptr [_dSynDesc],	eax
		jmp	_lCRScanLineSx
		;;----------------

		;;----------------
		;; debug?
		_lbl:
		cmp	eax,			75626564h	;; debu
		jne	_next
		cmp	byte ptr [esi+04h],	67h		;; g
		jne	_next
		cmp	byte ptr [esi+05h],	20h		;; bs or tab
		jg	_next
		jmp	dword ptr [_dDbgOff]

		_lCRDebugRem:
		inc	esi
		cmp	byte ptr [esi],		0ah		;; nl
		jne	_lCRDebugRem
		jmp	_lCRScanLine

		_lCRDebugAdd:
		add	esi,			06h
		jmp	_lCRScanLineSx
		;;----------------

		;;----------------
		;; include?
		_lbl:
		cmp	eax,			6c636e69h	;; incl
		jne	_lCRScanEx
		cmp	dword ptr [esi+04h],	20656475h	;; ude_
		jne	_lCRScanEx
		lea	ebx,			[esi+07h]

		_lIncSearch:
		inc	ebx
		cmp	byte ptr [ebx],		22h		;; "
		je	_lIncParveName
		cmp	byte ptr [ebx],		20h		;; bs
		je	_lIncSearch
		cmp	byte ptr [ebx],		09h		;; tab
		je	_lIncSearch

			;;----------------
			;; error
			mov	dword ptr [_xErrorTable],	offset _sErr_Base
			movsd
			movsd
			mov	dword ptr [_xErrorTable+04h],	edi
			_lIncludeErrEX:
			movsb
			cmp	word ptr [esi],			0a0dh
			jne	_lIncludeErrEX
			mov	dword ptr [_xErrorTable+08h],	edi
			jmp	_lErrIn
			;;----------------

		_lIncParveName:
		mov	edx,			dword ptr [_dCurrDirEnd]
		mov	ebp,			dword ptr [_dMapPathEnd]
		mov	ecx,			ebx
		inc	ebx
		_lIncLib:
		inc	ecx
		mov	al,			byte ptr [ecx]
		inc	edx
		inc	ebp
		cmp	al,			22h		;; "
		mov	byte ptr [edx],		al
		mov	byte ptr [ebp],		al
		jne	_lIncLib
		mov	byte ptr [ecx],		00h
		mov	byte ptr [edx],		00h
		mov	byte ptr [ebp],		00h

		lea	esi,			[ecx+01h]	;; new sorc script position

			;;----------------
			;; open file
			push	00h
			push	FILE_ATTRIBUTE_NORMAL
			push	OPEN_EXISTING
			push	00h
			push	FILE_SHARE_READ
			push	GENERIC_READ
			push	offset _sCurrDir
			call	_imp__CreateFileA@28

			inc	eax
			jnz	_lIncNoReopen

			push	00h
			push	FILE_ATTRIBUTE_NORMAL
			push	OPEN_EXISTING
			push	00h
			push	FILE_SHARE_READ
			push	GENERIC_READ
			push	offset _sMapPathEX
			call	_imp__CreateFileA@28

			inc	eax
			jnz	_lIncNoReopen

			push	00h
			push	FILE_ATTRIBUTE_NORMAL
			push	OPEN_EXISTING
			push	00h
			push	FILE_SHARE_READ
			push	GENERIC_READ
			push	ebx
			call	_imp__CreateFileA@28

			inc	eax
			jnz	_lIncNoReopen

				;;----------------
				;; cannot open file
				mov	ecx,				esi
				mov	dword ptr [_xErrorTable],	offset _sErr_CantOpenFile
				mov	dword ptr [_xErrorTable+04h],	edi
				_lIncludeErr:
				dec	esi
				cmp	byte ptr [esi-01h],		0ah
				jne	_lIncludeErr
				sub	ecx,				esi
				rep	movsb
				mov	byte ptr [edi-01h],		22h	;; "
				mov	dword ptr [_xErrorTable+08h],	edi	
				jmp	_lErrIn
				;;----------------
			;;----------------

			;;----------------
			;; read file
			_lIncNoReopen:
			dec	eax					;; restore eax
			sub	esp,			08h		;; old esi, mem handle, mem address
			mov	ebx,			eax		;; ebx = file handle
			
			push	esi					;; old script position

			push	00h
			push	eax
			call	_imp__GetFileSize@8

			mov	ebp,			eax		;; ebp = file size

			push	00h					;; ---> _imp__ReadFile@20
			push	offset _dBuffer				;; ---> _imp__ReadFile@20
			push	eax					;; ---> _imp__ReadFile@20
			add	eax,			04h

			push	eax
			push	GMEM_ZEROINIT
			call	_imp__GlobalAlloc@8
			push	eax
			mov	dword ptr [esp+14h],	eax
			call	_imp__GlobalLock@4
			mov	esi,			eax
			mov	dword ptr [esp+14h],	eax
			add	ebp,			eax

			push	eax
			push	ebx
			call	_imp__ReadFile@20

			push	ebx
			call	_imp__CloseHandle@4

			mov	dword ptr [ebp],	00007201h	;; #r__
			push	00h					;; for safe

			jmp	_lCRScanLine
			;;----------------
		;;----------------

		;;----------------
		;; remove bs
		_lBSRemBase	equ		$
		_lCRScanBS:
		inc	esi

		_lCRScan:
		mov	eax,			dword ptr [esi]

		_lCRScanEx:	;; <---
		cmp	al,			20h
		jne	_next

			;;----------------
			;; check prew blocks
			cmp	word ptr [edi-06h],	7801h	;; #x
			je	_lCRScanBS
			cmp	word ptr [edi-06h],	7901h	;; #y
			je	_lCRScanBS
			;;----------------

		_lCRBSStart:	;; test previous char
		xor	ebx,			ebx
		mov	bl,			byte ptr [edi-01h]
		mov	bl,			byte ptr [_bAscii_01+ebx]
		add	ebx,			_lBSRemBase
		jmp	ebx

		_lCRBSNext:	;; test next chaqr
		xor	ebx,			ebx
		mov	bl,			ah
		mov	bl,			byte ptr [_bAscii_02+ebx]
		add	ebx,			_lBSRemBase
		jmp	ebx

		_lCRBSAdd:
		mov	byte ptr [edi],		20h	;; _
		inc	esi
		inc	edi
		jmp	_lCRScan

		_lCRIncDec:
		cmp	ah,			2bh	;; ++
		je	_lCRBSAdd
		cmp	ah,			2dh	;; --
		je	_lCRBSAdd
		jmp	_lCRScanBS

		_lbl:
		cmp	al,			09h	;; tab
		je	_lCRBSStart
		;;----------------

		;;----------------
		;; new line
		cmp	al,			0ah
		je	_lCRScanLine
		cmp	al,			0dh
		je	_lCRScanLine
		;;----------------

		;;----------------
		;; error with 80+ char
		cmp	al,			80h
		jb	_next

		mov	dword ptr [_xErrorTable],	offset _sErr_BadChar
		mov	dword ptr [_xErrorTable+04h],	edi
		movsb
		mov	dword ptr [_xErrorTable+08h],	edi
		jmp	_lErrIn
		;;----------------

		;;----------------
		;; ascii int
		_lbl:
		cmp	al,			27h		;; '
		jne	_next

		xor	eax,			eax
		mov	al,			byte ptr [edi-01h]
		inc	esi
		cmp	byte ptr [_bAscii_00+eax],	ah
		je	_lCRASCII_FX
		mov	byte ptr [edi],		20h		;; bs
		inc	edi

		_lCRASCII_FX:
		mov	word ptr [edi],		7830h		;; 0x
		add	edi,			02h
		mov	ecx,			05h

		_lCRASCII:
		mov	al,			byte ptr [esi]
		cmp	al,			27h		;; '
		je	_lCRASCII_EX

		shr	eax,			04h
		and	al,			0fh
		cmp	al,			0ah
		sbb	al,			69h
		das
		stosb
		mov	al,			byte ptr [esi]
		and	al,			0fh
		cmp	al,			0ah
		sbb	al,			69h
		das
		stosb

		inc	esi
		dec	ecx
		jnz	_lCRASCII

			;;----------------
			;; error
			mov	dword ptr [_xErrorTable],	offset _sErr_IncorrectLiteral
			dec	edi
			mov	dword ptr [_xErrorTable+04h],	edi
			inc	edi
			mov	dword ptr [_xErrorTable+08h],	edi
			jmp	_lErrIn
			;;----------------

		_lCRASCII_EX:
		inc	esi
		jmp	_lCRScan
		;;----------------

		;;----------------
		;; textmacro fx
		_lbl:
		cmp	al,			11h
		jne	_next
		inc	esi

		lea	eax,			[edi+06h]
		mov	dword ptr [edi],	74747474h	;; tttt
		mov	dword ptr [edi+04h],	78013d29h	;; )={ 
		add	edi,			0ch
		push	eax
		jmp	_lCRScan

		_lbl:
		cmp	al,			12h
		jne	_next
		inc	esi

		lea	eax,			[edi+01h]
		mov	dword ptr [edi],	0078013dh	;; ={ 
		add	edi,			07h
		push	eax
		jmp	_lCRScan

		_lbl:
		cmp	al,			13h
		jne	_next

		inc	esi
		mov	byte ptr [edi],		28h		;; ( 
		mov	dword ptr [edi+01h],	74747474h
		add	edi,			05h
		jmp	_lCRScan

		_lbl:
		cmp	al,			14h
		jne	_next

		_lCRGFDD:
		inc	esi
		cmp	byte ptr [esi],		09h		;; tab
		je	_lCRGFDD
		cmp	byte ptr [esi],		20h		;; bs
		je	_lCRGFDD

;		inc	edi
;		_lCRGFGG:
;		dec	edi
;		cmp	byte ptr [edi],		20h

		mov	dword ptr [edi],	74747474h	;; tttt
		mov	byte ptr [edi+04h],	2ch		;; ,
		mov	dword ptr [edi+05h],	74747474h	;; tttt
		add	edi,			09h
		jmp	_lCRScan
		;;----------------

		;;----------------
		;; comments? vJass parser instruction?
		_lbl:
		cmp	ax,			2f2fh		;; //
		jne	_lCRCommTest_01
		cmp	byte ptr [esi+02h],	21h		;; !	
		jne	_lComm_00
		mov	dword ptr [edi],	20212f2fh	;; //!_
		add	edi,	04h
		add	esi, 	03h
		jmp	_lCRScanLineSx
		_lComm_00:
		inc	esi
		cmp	byte ptr [esi],		00h
		je	_lCREnd
		cmp	word ptr [esi],		0a0dh
		jne	_lComm_00
		jmp	_lCRScanLine

		_lCRCommTest_01:
		cmp	ax,			2a2fh		;; /*
		jne	_next

		mov	edx,			01h
		_lCRCommNextEx:
		inc	esi
		_lCRCommNext:
		inc	esi
		cmp	byte ptr [esi],		00h
		je	_lCRErrorComm
		cmp	word ptr [esi],		2f2fh		;; //
		jne	_lCRCommNextX
		inc	esi
		_lCRCommRemX:
		inc	esi
		cmp	byte ptr [esi],		00h
		je	_lCREnd
		cmp	word ptr [esi],		0a0dh
		jne	_lCRCommRemX
		jmp	_lCRCommNextEx
		_lCRCommNextX:
		cmp	word ptr [esi],		2f2ah		;; */
		je	_lCRCommSX
		cmp	word ptr [esi],		2a2fh		;; /*
		jne	_lCRCommNext
		inc	edx
		jmp	_lCRCommNextEx
		_lCRCommSX:
		dec	edx
		jnz	_lCRCommNextEx
		add	esi,			02h
		jmp	_lCRScan

			;;----------------
			;; error
			_lCRErrorComm:
			mov	dword ptr [_xErrorTable],	offset _sErr_UnclosedComment
			mov	dword ptr [_xErrorTable+04h],	edi
			mov	dword ptr [edi],		2a2fh	;; /*
			add	edi,				02h
			mov	dword ptr [_xErrorTable+08h],	edi
			jmp	_lErrIn
			;;----------------
		;;----------------

		;;----------------
		;; textmacro arg
		_lbl:
		cmp	al,			24h		;; $
		jne	_next

		xor	eax,			eax
		mov	ebx,			esi
		inc	esi

		_lCRMArgFX:
		inc	ebx
		mov	al,			byte ptr [ebx]
		cmp	byte ptr [_bAscii_00+eax],	ah
		jne	_lCRMArgFX

		cmp	byte ptr [ebx],		24h		;; $
		je	_lCRMArgSF
		mov	word ptr [edi],		7830h		;; 0x
		add	edi,			02h
		jmp	_lCRScan

		_lCRMArgSF:
		mov	al,			byte ptr [esi-02h]
		cmp	byte ptr [_bAscii_00+eax],	ah
		jz	_lCRMArg_00

		mov	word ptr [edi],		2323h		;; ##
		mov	dword ptr [edi+02h],	74747474h	;; tttt
		add	edi,			06h
		jmp	_lCRMArg_01

		_lCRMArg_00:
		mov	dword ptr [edi],	74747474h	;; tttt
		add	edi,			04h

		_lCRMArg_01:
		movsb
		cmp	esi,			ebx
		jb	_lCRMArg_01

		inc	esi
		mov	al,			byte ptr [esi]
		cmp	byte ptr [_bAscii_00+eax],	ah

		je	_lCRMArg_02

		mov	dword ptr [edi],	74747474h	;; tttt
		mov	word ptr [edi+04h],	2323h		;; ##
		add	edi,			06h
		jmp	_lCRScan

		_lCRMArg_02:
		mov	dword ptr [edi],	74747474h	;; tttt
		add	edi,			04h
		cmp	byte ptr [esi],		24h		;; $
		jne	_lCRScan
		mov	word ptr [edi],		2323h		;; ##
		add	edi,			02h
		jmp	_lCRScan

		_lbl:
		cmp	ax,			3501h		;; #5
		jne	_next

		add	esi,			02h
		mov	word ptr [edi],		2f2fh		;; //
		add	edi,			02h
		jmp	_lCRScan
		;;----------------

		;;----------------
		;; error with */
		_lbl:
		cmp	ax,			2f2ah		;; */
		jne	_next

		mov	dword ptr [_xErrorTable],	offset _sErr_BadComment
		mov	dword ptr [_xErrorTable+04h],	edi
		mov	dword ptr [edi],		2f2ah	;; /*
		add	edi,				02h
		mov	dword ptr [_xErrorTable+08h],	edi
		jmp	_lErrIn
		;;----------------

		;;----------------
		;; line ex
		_lbl:
		cmp	al,			5ch		;; \ 
		jne	_next
		mov	edx,			esi

		_lCRSBS_00:
		inc	edx

		cmp	byte ptr [edx],		20h		;; bs
		je	_lCRSBS_00
		cmp	byte ptr [edx],		09h		;; tab
		je	_lCRSBS_00
		cmp	byte ptr [edx],		5ch		;; \ 
		je	_lCRSBS_00

		cmp	word ptr [edx],		2f2fh		;; //
		je	_lCRSBS_02

		cmp	word ptr [edx],		6201h		;; #b
		je	_lCRScanBS

		cmp	word ptr [edx],		0a0dh		;; nl
		je	_lCRSBS_01

		_lCRSBS_03:
		movsb
		jmp	_lCRScan

		_lCRSBS_01:
		inc	esi
		mov	word ptr [edx],		6201h		;; #b
		jmp	_lCRScan

		_lCRSBS_02:
		cmp	byte ptr [edx+02h],	21h		;; !
		je	_lCRSBS_03

		_lCRSBS_04:
		mov	byte ptr [edx],		20h		;; bs
		inc	edx

		cmp	word ptr [edx],		0a0dh		;; nl
		jne	_lCRSBS_04

		inc	esi
		mov	word ptr [edx],		6201h		;; #b
		jmp	_lCRScan
		;;----------------

		;;----------------
		;; ex new line
		_lbl:
		cmp	al,			3bh		;; ;
		jne	_next
		inc	esi
		jmp	_lCRScanLine
		;;----------------

		;;----------------
		;; blocks
			;;----------------
			;; x
			_lbl:
			cmp	al,			7bh		;; {
			jne	_next

			_lCRBlockCheck:
			cmp	word ptr [edi-02h],	0a0dh
			jne	_lCRBlockCheckEnd
			sub	edi,			02h
			_lCRBlockCheckEnd:
			push	edi
			mov	word ptr [edi],		7801h		;; #x
			inc	esi
			add	edi,			06h
			jmp	_lCRScan				;; ???
			;;----------------

			;;----------------
			;; y
			_lbl:
			cmp	al,			7dh		;; }
			jne	_next
			_lCRCloseBlock:
			pop	eax
			test	eax,			eax
			jz	_lBlockErr
			cmp	word ptr [edi-02h],	0a0dh		;; new line
			je	_lCLAddNE
			mov	word ptr [edi],		0a0dh		;; new line

			add	edi,			02h
			_lCLAddNE:
			mov	dword ptr [eax+02h],	edi
			mov	word ptr [edi],		7901h		;; #y
			mov	dword ptr [edi+02h],	eax
			mov	dword ptr [edi+06h],	06060606h	;; ex backspace
			mov	dword ptr [edi+0ah],	06060606h
			mov	word ptr [edi+0eh],	0a0dh		;; new line

			inc	esi
			add	edi,			10h
			jmp	_lCRScanLine

				;;----------------
				;; error
				_lBlockErr:
				mov	dword ptr [_xErrorTable],	offset _sErr_BadBlock
				mov	dword ptr [_xErrorTable+04h],	edi
				mov	byte ptr [edi],			7dh	;; }
				inc	edi
				mov	dword ptr [_xErrorTable+08h],	edi

				jmp	_lErrIn
				;;----------------
			;;----------------
		;;----------------

		;;----------------
		;; return from included file
		_lbl:
		cmp	ax,			7201h		;; #r
		jne	_next
		_lRetnGuard:
		pop	eax					;; for safe
		test	eax,			eax
		jnz	_lIncRetErr
		pop	esi
		call	_imp__GlobalUnlock@4
		call	_imp__GlobalFree@4
		jmp	_lCRScanLine

			;;----------------
			;; error
			_lIncRetErr:
			mov	dword ptr [_xErrorTable],	offset _sErr_BadBlockInFile
			mov	dword ptr [_xErrorTable+04h],	eax
			inc	eax
			mov	dword ptr [_xErrorTable+08h],	eax
			jmp	_lErrIn
			;;----------------
		;;----------------

		;;----------------
		;; #b
		_lbl:
		cmp	ax,			6201h		;; #b
		jne	_next
		add	esi,			02h
		mov	byte ptr [edi],		20h		;; bs
		inc	edi
		jmp	_lCRScan
		;;----------------

		;;----------------
		;; strings
		_lbl:
		cmp	al,			22h		;; "
		jne	_next
		mov	ebp,			edi
		mov	ecx,			0800h
		jmp	_lCRStringDX

		_lCRStringEX:
		movsb
		_lCRStringDX:
		movsb
		_lCRStringSX:
		cmp	word ptr [esi],		0a0dh		;; nl
		jne	_lCRStringNext
		mov	word ptr [edi],		6e5ch		;; \n
		add	esi,			02h
		add	edi,			02h
		jmp	_lCRStringSX
		_lCRStringNext:
		cmp	byte ptr [esi],		00h		;; null
		je	_lCRStringError
		cmp	byte ptr [esi],		5ch		;; \ 
		je	_lCRStringEX
		dec	ecx
		jz	_lCRStringError
		cmp	byte ptr [esi],		22h		;; "
		jne	_lCRStringDX
		movsb
		jmp	_lCRScan

			;;----------------
			;; not closed string
			_lCRStringError:
			mov	dword ptr [_xErrorTable],	offset _sErr_UnclosedString
			mov	dword ptr [_xErrorTable+04h],	ebp
			mov	dword ptr [_xErrorTable+08h],	edi

			jmp	_lErrIn
			;;----------------
		;;----------------

		;;----------------
		;; null and other
		_lbl:
		cmp	byte ptr [esi],		00h		;; null
		je	_lCREnd
		movsb
		jmp	_lCRScan
		;;----------------

		;;----------------
		_lCREnd:
		cmp	dword ptr [_dStackPos],	esp			
		je	_next
			;;----------------
			;; error
			mov	dword ptr [_xErrorTable],	offset _sErr_UnclosedBlock
			pop	eax
			add	eax,				02h
			mov	dword ptr [_xErrorTable+04h],	eax
			inc	eax
			mov	dword ptr [_xErrorTable+08h],	eax
			jmp	_lErrIn
			;;----------------
		_lbl:
		pop	eax					;; for safe
		;;----------------

	mov	esi,			dword ptr [esp+04h]
	add	edi,			04h
	mov	dword ptr [_dSynDesc],	offset _xSynDesc	;; reset
	push	edi
	;;----------------	

	mov	_dCurrStr,	offset _sProg_02
	mov	eax,		28h
	call	_lSetProg

	;;----------------
	;; build define block
	dec	esi
	mov	ebx,		offset _lDefX

		;;----------------
		;; sysdefines

			;;----------------
			;; date time
			push	offset _xSysTime
			call	_imp__GetLocalTime@4

			mov	ecx,				0ah
			xor	eax,				eax
			xor	edx,				edx
			mov	ax,				word ptr [_xSysTime]

			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+03h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+02h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+01h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate],		dl

			mov	byte ptr [_sDate+04h],		2eh	;; .

			mov	al,				byte ptr [_xSysTime+02h]

			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+06h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+05h],		dl

			mov	byte ptr [_sDate+07h],		2eh	;; .

			mov	al,				byte ptr [_xSysTime+06h]

			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+09h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sDate+08h],		dl

			mov	byte ptr [_sDate+0ah],		03h

			mov	al,				byte ptr [_xSysTime+08h]

			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime+01h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime],		dl

			mov	byte ptr [_sTime+02h],		3ah	;; :

			mov	al,				byte ptr [_xSysTime+0ah]

			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime+04h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime+03h],		dl

			mov	byte ptr [_sTime+05h],		3ah	;; :

			mov	al,				byte ptr [_xSysTime+0ch]

			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime+07h],		dl
			xor	edx,				edx
			div	ecx
			add	dl,				30h
			mov	byte ptr [_sTime+06h],		dl

			mov	byte ptr [_sTime+08h],		03h

			mov	dword ptr [ebx],		offset _sDateL
			mov	dword ptr [ebx+04h],		offset _sDate
			mov	dword ptr [ebx+10h],		offset _sTimeL
			mov	dword ptr [ebx+14h],		offset _sTime
			;;----------------

			;;----------------
			;; weather
			mov	dword ptr [ebx+20h],		offset _sWeatherL
			mov	dword ptr [ebx+24h],		offset _sWether
			;;----------------

			;;----------------
			;; version
			_lbl:
			mov	eax,				dword ptr [_dWarVerSL]
			mov	dword ptr [ebx+30h],		offset _sWarVer
			mov	dword ptr [ebx+34h],		eax
			;;----------------

			;;----------------
			;; count
			mov	dword ptr [ebx+40h],		offset _sCountL
			mov	dword ptr [ebx+44h],		offset _sCntSpec
			;;----------------

			;;----------------
			;; funcname
			mov	dword ptr [ebx+50h],		offset _sFuncNameL
			mov	dword ptr [ebx+54h],		offset _sFuncName
			;;----------------

			;;----------------
			;; debug
			cmp	dword ptr [_dDbgOff],		offset _lCRDebugAdd
			jne	_next

			mov	dword ptr [ebx+60h],		offset _sDebugL
			mov	dword ptr [ebx+64h],		offset _sTrue
			add	ebx,				10h
			;;----------------

		_lbl:
		add	ebx,				60h
		;;----------------

	xor	ecx,		ecx		;; current scope = 00h
	mov	_dStackPos,	esp		;; save stack

		;;----------------
		;; line start
		_lLineStart:
		inc	esi
		_lLineStartEx:
		mov	eax,			dword ptr [esi]
		;;----------------

		;;----------------
		;; define in
		_lbl:
		cmp	eax, 			69666564h	;; defi
		jne	_next
		cmp	word  ptr [esi+04h],	656eh		;; ne
		jne	_next
		cmp	byte ptr [esi+06h],	20h		;; _
		jg	_lDefInEX

		mov	eax,			esi
		jne	_lBlockCheckPre
		inc	esi
		_lBlockCheckPre:
		add	esi,			06h
		cmp	word ptr [esi],		0a0dh		;; nl
		jne	_lBlockCheck_01				;; single constant or block
		add	esi,			02h
		mov	edx,			offset _lDefEx
		mov	word ptr [eax],		6401h		;; #d
		jmp	_lDefEx

		_lBlockCheck_01:
		cmp	word ptr [esi],		7801h		;; #x
		jne	_lBlockCheck_02				;; single constant
		mov	word ptr [eax],		6401h		;; #d
		mov	eax,			dword ptr [esi+02h]
		add	esi,			06h
		mov	dword ptr [eax],	6e650a0dh	;; __en
		mov	dword ptr [eax+04h],	66656464h	;; ddef
		mov	dword ptr [eax+08h],	20656e69h	;; ine_
		mov	edx,			offset _lDefEx
		jmp	_lDefEx
		_lBlockCheck_02:
		mov	edx,			offset _lLineStartEx
		mov	word ptr [eax],		6301h		;; #c
		jmp	_lDefEx

		_lDefInEX:
		cmp	byte ptr [esi+06h],	3ch		;; <
		jne	_next
		mov	word ptr [esi],		6301h		;; #c
		mov	edx,			offset _lLineStartEx
		add	esi,			06h
		jmp	_lDefEx
		;;----------------

		;;----------------
		;; enum in
		_lbl:
		cmp	eax,			6d756e65h	;; enum
		jne	_next
		cmp	byte ptr [esi+04h],	28h		;; ( 
		je	_lEnumInEX
		cmp	byte ptr [esi+04h],	20h		;; bs
		jg	_next
		je	_lEnumInBase
		mov	ebp,				offset _dEnumDefTable-04h
		jmp	_lEnumInBlock
		;;----------------

		;;----------------
		;; undef
		_lbl:
		cmp	eax,			65646e75h	;; unde
		jne	_next
		cmp	word ptr [esi+04h],	2066h		;; f_
		je	_lUndefNorm

		cmp	word ptr [esi+04h],	3c66h		;; f<
		jne	_next

			;;----------------
			;; hard
			mov	word ptr [esi],		7501h		;; #u
			add	esi,			06h
			_lUndefHard:
			inc	esi
			cmp	byte ptr [esi],		3eh	;; >
			jne	_lUndefHard
			mov	byte ptr [esi],		02h
			jmp	_lLineStartEx
			;;----------------

			;;----------------
			;; norm
			_lUndefNorm:
			mov	word ptr [esi],		7501h		;; #u
			add	esi,			06h
			_lUndefNormEX:
			inc	esi
			cmp	word ptr [esi],		0a0dh	;; nl
			jne	_lUndefNormEX
			mov	byte ptr [esi],		02h
			jmp	_lLineStartEx
			;;----------------
		;;----------------

		;;----------------
		;; setdef
		_lbl:
		cmp	eax,			64746573h	;; setd
		jne	_next
		cmp	word ptr [esi+04h],	6665h		;; ef
		jne	_next
		cmp	byte ptr [esi+06h],	3ch		;; <
		je	_lSetdefHard
		cmp	byte ptr [esi+06h],	20h		;; bs
		jne	_next

		push	ebx
		mov	edx,			offset _lSerdefExit
		xor	eax,			eax
		xor	ebp,			ebp
		lea	edi,			[esi+02h]
		mov	byte ptr [esi],		08h		;; setdef
		add	esi,			06h
		mov	ebx,			_dDefTableSD+_dDFSize

		_lSetdef:
		inc	esi
		mov	al,			byte ptr [esi]
		cmp	byte ptr [_bAscii_00+eax],	ah	;; ah = 00h
		jne	_lSetdef

		_lSetdefEX:
		cmp	byte ptr [esi],		3dh		;; =
		je	_lSetdefClose
		cmp	word ptr [esi],		2928h		;; () 
		je	_lSetdefFVEX

			;;----------------
			;; macro
			mov	byte ptr [esi],		02h
			mov	ebp,			esi
			_lSetdefArg:
			inc	esi
			cmp	byte ptr [esi],		3dh		;; =
			jne	_lSetdefArg
			;;----------------

			;;----------------
			;; close setdef
			_lSetdefClose:
			mov	byte ptr [esi],		02h
			inc	esi

			cmp	word ptr [esi],		7801h		;; #x
			je	_lSetdefMultiline

			mov	dword ptr [edi],		esi
			dec	edi
			mov	dword ptr [_dDefTableSD+04h],	esi
			push	edi
			jmp	_lDFFindValueEndRE			;; !!!

			_lSetdefMultiline:
			mov	eax,			dword ptr [esi+02h]
			add	esi,			06h
			mov	dword ptr [edi],	esi
			mov	byte ptr [eax],		03h
			jmp	_lDFFindValueEndEG
			;;----------------

		_lSetdefFVEX:
		add	esi,			02h
		jmp	_lSetdefClose

		_lSetdefHard:
push	ebx
mov	edx,			offset _lSerdefExit
xor	ebp,			ebp
lea	edi,			[esi+02h]
mov	byte ptr [esi],		08h		;; setdef
add	esi,			06h
mov	ebx,			_dDefTableSD+_dDFSize
		_lSetdefHardEX:
		inc	esi
		cmp	byte ptr [esi],		3eh	;; >
		jne	_lSetdefHardEX
mov	byte ptr [esi],		02h
inc	esi
		jmp	_lSetdefEX

			;;----------------
			;; restore ebx
			_lSerdefExit:
			pop	edi
			mov	eax,			dword ptr [_dDefTableSD+0ch]
			mov	byte ptr [edi],		al
			pop	ebx
			jmp	_lLineStartEx
			;;----------------
		;;----------------

		;;----------------
		;; scope
		_lbl:
		cmp	eax, 			706f6373h	;; scop
		jne	_next
		cmp	byte  ptr [esi+04h],	65h		;; e
		jne	_next
		cmp	byte ptr [esi+05h],	20h		;; _
		jg	_next
		add	esi,			05h
		_lScopeIn:
		inc	esi
		cmp	word ptr [esi],		0a0dh
		je	_lLSIn
		cmp	word ptr [esi],		7801h		;; #x
		jne	_lScopeIn
		mov	eax,			dword ptr [esi+02h]
		mov	dword ptr [esi],	05050505h
		mov	word ptr [esi+04h],	0a0dh
		add	esi,			04h
		cmp	word ptr [eax-02h],	0a0dh
		je	_lScopeInEX
		mov	word ptr [eax],		0a0dh
		add	eax,			02h
		_lScopeInEX:
		mov	dword ptr [eax],	73646e65h	;; ends
		mov	dword ptr [eax+04h],	65706f63h	;; cope
		_lLSIn:
		push	ecx
		inc	dword ptr [_dFreeScope]
		mov	ecx,			_dFreeScope
		lea	eax,			[offset _dScopeIn+ecx*04h]
		mov	dword ptr [eax],	esi
		jmp	_lNextLineSx
		;;----------------

		;;----------------
		;; library
		_lbl:
		cmp	eax,			7262696ch	;; libr
		jne	_next
		cmp	dword ptr [esi+03h],	79726172h	;; rary
		jne	_next
		cmp	dword ptr [esi+07h],	636e6f5fh	;; _onc
		jne	_lLibExTest
		cmp	byte ptr [esi+0bh],	65h		;; e
		jne	_lLibExTest
		cmp	byte ptr [esi+0ch],	20h		;; _
		jg	_lLibExTest
		add	esi,			0eh
		jmp	_lLibInEX
		_lLibExTest:
		cmp	byte ptr [esi+07h],	20h		;; _
		jg	_next
		add	esi,			08h
		_lLibIn:
		inc	esi
		_lLibInEX:
		cmp	word ptr [esi],		0a0dh
		je short	_lLSIn
		cmp	word ptr [esi],		7801h		;; #x
		jne	_lLibIn
		mov	eax,			dword ptr [esi+02h]
		mov	dword ptr [esi],	05050505h
		mov	word ptr [esi+04h],	0a0dh
		add	esi,			04h
		cmp	word ptr [eax-02h],	0a0dh
		je	_lLibInSX
		mov	word ptr [eax],		0a0dh
		add	eax,			02h
		_lLibInSX:
		mov	word ptr [eax],		6e65h		;; en
		mov	dword ptr [eax+02h],	62696c64h	;; dlib
		mov	dword ptr [eax+06h],	79726172h	;; rary
		jmp	_lLSIn
		;;----------------

		;;----------------
		;; endscope
		_lbl:
		cmp	eax,			73646e65h	;; ends
		jne	_next
		cmp	dword ptr [esi+04h],	65706f63h	;; cope
		jne	_next
		cmp	byte ptr [esi+08h],	20h		;; _
		jg	_next
		;;----------------

		;;----------------
		;; scope/library out
		_lOut:
		test	ecx,			ecx
		jz	_lLibScpErr
		lea	eax,			[offset _dScopeOut+ecx*04h]
		mov	dword ptr [eax],	esi
		pop	ecx
		jmp	_lNextLineSx

			;;----------------
			;; error
			_lLibScpErr:
			mov	dword ptr [_xErrorTable],	offset _sErr_EndLibScope
			mov	dword ptr [_xErrorTable+04h],	esi
			_lLibScpErrEX:
			inc	esi
			cmp	word ptr [esi],			0a0dh	;; nl
			jne	_lLibScpErrEX
			mov	dword ptr [_xErrorTable+08h],	esi
			jmp	_lErrIn
			;;----------------
		;;----------------

		;;----------------
		;; endlibrary
		_lbl:
		cmp	eax,				6c646e65h	;; endl
		jne	_lNextLineSx
		cmp	dword ptr [esi+04h],		61726269h	;; ibra
		jne	_lNextLineSx
		cmp	word ptr [esi+08h],		7972h		;; ry
		jne	_lNextLineSx
		cmp	byte ptr [esi+0ah],		20h		;; _
		jbe	_lOut
		;;----------------

		;;----------------
		;; search new line
		_lNextLineEx:
		inc	esi
		_lNextLineSx:
		cmp	word  ptr [esi],		0a0dh		;; new line
		je	_next
		cmp	word ptr [esi],			7801h		;; #x
		je	_lNextLineDx
		cmp	word ptr [esi],			7901h		;; #y
		je	_lNextLineDx
		cmp	byte  ptr [esi],		00h
		jne	_lNextLineEx
		jmp	_lScanEnd
		_lbl:
		inc	esi
		jmp	_lLineStart

		_lNextLineDx:
		add	esi,				06h
		jmp	_lLineStartEx ; _lNextLineSx
		;;----------------

		;;----------------
		;; build enum
			;;----------------
			;; named enum

				;;----------------
				;; error
				_lEnumInErr:
				mov	dword ptr [_xErrorTable],	offset _sErr_Base
				mov	dword ptr [_xErrorTable+04h],	esi
				inc	esi
				mov	dword ptr [_xErrorTable+08h],	esi
				jmp	_lErrIn
				;;----------------

			_lEnumInEX:
			xor	eax,				eax
			add	esi,				05h
			mov	dword ptr [_dBuffer],		esi
			mov	ebp,				offset _xEnumLabel
			mov	edi,				dword ptr [ebp]
			test	edi,				edi
			jz	_lEnumInEXCreateNew

			_lEnumInEXCheck:
			lodsb
			cmp	byte ptr [_bAscii_00+eax],	ah	;; ah = 00h
			jne	_lEnumInEXCheckEX

			cmp	al,				29h	;; ) 
			jne	_lEnumInErr
			scasb
			jne	_lEnumInEXGetNext
			mov	eax,				dword ptr [_dBuffer]
			sub	eax,				05h
			jmp	_lEnumInBlockEX

			_lEnumInEXCheckEX:
			scasb
			je	_lEnumInEXCheck

			_lEnumInEXGetNext:
			add	ebp,				0ch
			mov	edi,				dword ptr [ebp]
			mov	esi,				dword ptr [_dBuffer]
			test	edi,				edi
			jnz	_lEnumInEXCheck

			_lEnumInEXCreateNew:
			mov	dword ptr [ebp],		esi
			mov	edi,				dword ptr [_dEnumTablePointer]
			mov	dword ptr [edi],		80000000h
			mov	dword ptr [edi+04h],		0ffffffffh
			lea	eax,				[edi+0ch]
			mov	dword ptr [edi+08h],		eax
			mov	dword ptr [ebp+04h],		edi
			mov	dword ptr [edi+0ch],		00000000h
			mov	dword ptr [edi+10h],		7fffffffh
			mov	dword ptr [edi+14h],		00000000h
			lea	eax,				[edi+0ch]
			mov	dword ptr [ebp+08h],		eax
			add	edi,				18h
			mov	dword ptr [_dEnumTablePointer],	edi
			mov	eax,				dword ptr [_dBuffer]
			sub	eax,				05h

			_lbl:
			inc	esi
			cmp	byte ptr [esi-01h],		29h		;; ) 
			jne	_prew

			jmp	_lEnumInBlockEX
			;;----------------

			;;----------------
			;; base enum
			_lEnumInBase:
			mov	word ptr [esi],			6301h		;; #c
			add	esi,				05h
			mov	ebp,				offset _dEnumDefTable-04h
			mov	dword ptr [_dBuffer],		offset _lLineStartEx
			jmp	_lEnumBStart
			;;----------------

			;;----------------
			;; block enum
			_lEnumInBlock:
			mov	eax,				esi
			add	esi,				04h
			_lEnumInBlockEX:
			cmp	word ptr [esi],			0a0dh		;; nl
			je	_lEnumInBlockFX
			cmp	word ptr [esi],			7801h		;; #x
			je	_lEnumInBlockSX
			;;----------------

			;;----------------
			;; single lined
			mov	dword ptr [_dBuffer],		offset _lLineStartEx
			mov	word ptr [eax],			6301h		;; #c
			jmp	_lEnumBStart
			;;----------------		

			;;----------------
			_lEnumInBlockSX:
			add	esi,				04h
			_lEnumInBlockFX:
			add	esi,				02h
			mov	dword ptr [_dBuffer],		offset _lEnumMStart
			mov	word ptr [eax],			6401h		;; #d
			jmp	_lEnumBStart
			;;----------------

			;;----------------
			;; start

				;;----------------
				;; exit from block
				_lEnumMStart:
				cmp	word ptr [esi],			7901h		;; #y
				jne	_lEnumMStartEX

				mov	word ptr [esi+0eh],		6401h		;; #d
				add	esi,				10h
				jmp	_lLineStartEx

				_lEnumMStartEX:
				cmp	dword ptr [esi],		65646e65h	;; ende
				jne	_lEnumBStart
				cmp	word ptr [esi+04h],		756eh		;; nu
				jne	_lEnumBStart
				cmp	byte ptr [esi+06h],		6dh		;; m
				jne	_lEnumBStart
				cmp	byte ptr [esi+07h],		20h		;;
				jg	_lEnumBStart
				cmp	word ptr [esi+07h],		0a0dh		;; nl
				jne	_lEnumInErr

				mov	word ptr [esi+07h],		6401h		;; #d
				add	esi,				09h
				jmp	_lLineStartEx
				;;----------------

			_lEnumBStart:

				;;----------------
				;; private ?
				cmp	dword ptr [esi],		76697270h	;; priv
				jne	_next
				cmp	dword ptr [esi+04h],		20657461h	;; ate_
				jne	_next
				add	esi,				08h
				mov	dword ptr [ebx+08h],		ecx
				;;----------------

			_lbl:
			mov	dword ptr [ebx],		esi	;; label

			cmp	byte ptr [esi],			3ch	;; <
			jne	_lEnumNorm

				;;----------------
				;; hard enum
				inc	dword ptr [ebx]
				_lEnumHard:
				inc	esi
				cmp	byte ptr [esi],			3eh	;; >
				jne	_lEnumHard

				mov	byte ptr [esi],			02h	;; label end 
				inc	esi
				jmp	_lEnumCheckRange
				;;----------------

				;;----------------
				;; norm enum
				_lEnumNorm:
				xor	eax,				eax
				dec	esi
				_lEnumNormEX:
				inc	esi
				mov	al,				byte ptr [esi]
				cmp	byte ptr [_bAscii_00+eax],	ah	;; ah = 00h
				jnz	_lEnumNormEX
				;;----------------

				;;----------------
				;; ranged?
				_lEnumCheckRange:
				cmp	byte ptr [esi],				28h	;; ( 
				je	_lEnumRanged
				;;----------------

				;;----------------
				;; simple
				cmp	byte ptr [esi],			2ch	;; ,
				jne	_next

				mov	byte ptr [esi],			02h	;; label end
				inc	esi
				jmp	_lEnumNextLabel

				_lbl:
				cmp	word ptr [esi],			0a0dh	;; nl
;				jne
				mov	word ptr [esi],			0702h	;; label end ; enum out
				add	esi,				02h

				_lEnumNextLabel:
				mov	edi,				dword ptr [ebp+08h]
				mov	eax,				dword ptr [edi]
				cmp	eax,				dword ptr [edi+04h]
;				je
				inc	dword ptr [edi]
				;;----------------

				;;----------------
				;; calculate string
				push	ebp

				mov	edi,				dword ptr [_dEnumStrPoint]
				mov	ebp,				0ah
				mov	byte ptr [edi],			03h	;; define value end
				dec	edi
				_lbl:
				xor	edx,				edx
				div	ebp
				add	edx,				30h
				mov	byte ptr [edi],			dl
				test	eax,				eax
				jnz	_prew

				mov	dword ptr [ebx+04h],		edi
				add	ebx,				_dDFSize

				add	edi,				0dh
				mov	dword ptr [_dEnumStrPoint],	edi

				pop	ebp
				jmp	dword ptr [_dBuffer]
				;;----------------

				;;----------------
				;; ranged
				_lEnumRanged:
				;;----------------
			;;----------------

		;;----------------

		;;----------------
		;; build defines
		_lDefEx:
		mov	eax,			dword ptr [esi]

		cmp	ax,			0a0dh
		jne	_lDefCheck
		add	esi,			02h
		jmp	_lDefEx

			;;----------------
			;; exit
			_lDefCheck:
			cmp	eax,			64646e65h	;; endd
			jne	_next
			cmp	dword ptr [esi+04h],	6e696665h	;; efin
			jne	_next
			cmp	byte ptr [esi+08h],	65h		;; e
			jne	_next
			cmp	byte ptr [esi+09h],	20h		;; _
			jg	_next

			cmp	byte ptr [esi+0ah],	06h
			jne	_lDefExitEX
			add	esi,			05h

			_lDefExitEX:
			add	esi,			09h
			mov	word ptr [esi-02h],	6401h		;; #d
			jmp	_lLineStartEx
			;;----------------

			;;----------------
			;; private ?
			_lDefPrivEX:
			cmp	dword ptr [esi+04h],	3c657461h	;; ate<
			jne	_next
			add	esi,			07h
			jmp	_lDefPrivSX

			_lbl:
			cmp	eax,			76697270h	;; priv
			jne	_next
			cmp	dword ptr [esi+04h],	20657461h	;; ate_
			jne	_lDefPrivEX
			add	esi,			08h
			_lDefPrivSX:
			mov	dword ptr [ebx+08h],	ecx
			;;----------------

			;;----------------
			;; define label
			_lbl:
			mov	dword ptr [ebx],	esi	;; find label
			mov	al,			byte ptr [esi]
			xor	ebp,			ebp
			;;----------------

		cmp	al,			3ch	;; <
		jne	_lDefNorm
		inc	dword ptr [ebx]

		_lDefHard:
		inc	esi
		cmp	byte ptr [esi],		3eh	;; >
		jne	_lDefHard
		mov	byte ptr [esi],		02h
		inc	esi
		cmp	byte ptr [esi],		3dh	;; =
		je	_lDFFindValue
		cmp	byte ptr [esi],		28h	;; (
		je	_lDFFindArgValue
		cmp	word ptr [esi],		0a0dh	;; nl
		je	_lDFNull

			;;----------------
			;; error
			_lDefErrorBase:
			mov	dword ptr [_xErrorTable],	offset _sErr_BadDef
			mov	dword ptr [_xErrorTable+04h],	esi
			inc	esi
			mov	dword ptr [_xErrorTable+08h],	esi
			jmp	_lErrIn
			;;----------------

			;;----------------
			;; check first char
			_lDefNorm:
			cmp	al,			41h
			jb	_lDefErrorBase
			cmp	al,			5ah
			jbe	_next
			cmp	al,			61h
			jb	_lDefErrorBase
			cmp	al,			7ah
			jg	_lDefErrorBase
			;;----------------

			;;----------------
			;; check define label
			_lbl:
			inc	esi
			mov	al,			byte ptr [esi]
			cmp	al,			3dh	;; =
			je	_lDFFindValue
			cmp	al,			28h	;; (
			je	_lDFFindArgValue
			cmp	word ptr [esi],		0a0dh	;; nl
			je	_lDFNull

			cmp	al,			5fh
			je	_prew
			cmp	al,			30h
			jb	_lDefErrorBase
			cmp	al,			39h
			jbe	_prew
			cmp	al,			41h
			jb	_lDefErrorBase
			cmp	al,			5ah
			jbe	_prew
			cmp	al,			61h
			jb	_lDefErrorBase
			cmp	al,			7ah
			jg	_lDefErrorBase
			jmp	_prew
			;;----------------

			;;----------------
			;; null define
			_lDFNullEX:
			mov	dword ptr [esi],	02h
			add	esi,			02h
			_lDFNull:
			mov	word ptr [esi],		0302h		;; label end ; def end
			inc	esi
			mov	dword ptr [ebx+04h],	esi
			add	ebx,			_dDFSize
			inc	esi
			jmp	edx
			;;----------------

			;;----------------
			;; macros
			_lDFFindArgValue:
			cmp	dword ptr [esi],	0a0d2928h	;; () nl
			je	_lDFNullEX
			cmp	word ptr [esi],		2928h		;; ()
			jne	_lDFFindArgValueSX
			mov	byte ptr [esi],		02h
			add	esi,			03h
			jmp	_lDFFindValueTX

			_lDFFindArgValueSX:
			mov	byte ptr [esi],		02h
			mov	ebp,			esi
			_lDFFindArgValueRe:
			inc	esi
			cmp	word ptr [esi],		0a0dh		;; nl
			je	_lDFFindValueEndFS
			cmp	byte ptr [esi],		3dh		;; =
			jne	_lDFFindArgValueRe
			;;----------------

			;;----------------
			;; close define
			_lDFFindValue:
			mov	byte ptr [esi],		02h
			inc	esi
			;;----------------

			;;----------------
			;; is it multilined define?
			_lDFFindValueTX:
			cmp	word ptr [esi],		7801h		;; #x
			je	_lDFFindValueEndEx
			;;----------------

			;;----------------
			mov	dword ptr [ebx+04h],	esi
			add	ebx,			_dDFSize
			;;----------------

			;;----------------
			_lDFFindValueEndRE:
			cmp	word ptr [esi],		0a0dh		;; nl
			je	_lDFFindValueEndRR

			_lDFFindValueEnd:
			inc	esi
			cmp	word ptr [esi],		0a0dh		;; nl
			jne	_lDFFindValueEnd
			_lDFFindValueEndRR:
			mov	byte ptr [esi],		03h
			add	esi,			02h
			jmp	_lDFArcCheckStart

			_lDFFindValueEndFS:
			mov	word ptr [esi],		0302h		;; label end ; def end
			inc	esi
			mov	dword ptr [ebx+04h],	esi
			add	ebx,			_dDFSize
			inc	esi
			jmp	_lDFArcCheckStart
			;;----------------

			;;----------------
			;; multilined define
			_lDFFindValueEndEx:
			mov	eax,			dword ptr [esi+02h]
			add	esi,			06h
			mov	dword ptr [ebx+04h],	esi
			add	ebx,			_dDFSize
			mov	byte ptr [eax],		03h
			_lDFFindValueEndEG:
			lea	esi,			[eax+05h]
			_lDFFindValueEndES:
			inc	esi
			cmp	word ptr [esi],		0a0dh
			jne	_lDFFindValueEndES
			add	esi,			02h
;;			jmp	_lDFArcCheckStart
			;;----------------

			;;----------------
			;; parse arguments
			_lDFArcCheckStart:
			test	ebp,			ebp
			jnz	_lDFEAXLoad
			jmp 	edx			;; exit

			_lDFEAXLoad:
			mov	ah,			7fh
			_lDFNextArg:
			inc	ebp
			cmp	byte ptr [ebp],		29h	;; )
			jne	_lDFArgReplace
			jmp	edx			;; exit - all arguments parsed

			_lDFArgReplace:
			push	ebp
			inc	ah
			inc	dword ptr [ebx+0ch-_dDFSize]
			mov	edi,			dword ptr [ebx+04h-_dDFSize]

			dec	edi
			_lDFArgSearchWord:
			inc	edi
			_lDFArgSearchWordEx:
			cmp	byte ptr [edi],		03h
			je	_lDFArgCheckEnd

				;;----------------
				;; strings
				cmp	byte ptr [edi],		22h	;; "
				jne	_lDFArgSearchWordSx
				_lDFArgSearchWordDx:
				inc	edi
				_lDFArgSearchWordRx:
				cmp	byte ptr [edi],		03h
				je	_lDFArgCheckEnd
				cmp	byte ptr [edi],		5ch	;; \ 
				jne	_lDFArgSearchWordFx
				add	edi,			02h
				jmp	_lDFArgSearchWordRx
				_lDFArgSearchWordFx:
				cmp	byte ptr [edi],		22h	;; "
				jne	_lDFArgSearchWordDx
				jmp	_lDFArgSearchWord
				;;----------------

			_lDFArgSearchWordSx:
			cmp	byte ptr [edi],		01h
			je	_lDFArgSearchWordSS
			cmp	byte ptr [edi],		30h
			jb	_lDFArgSearchWord
			cmp	byte ptr [edi],		3ah
			jb	_lDFArgNextWord
			cmp	byte ptr [edi],		41h
			jb	_lDFArgSearchWord
			cmp	byte ptr [edi],		5bh
			jb	_lDFArgCheckWord
			cmp	byte ptr [edi],		5fh
			je	_lDFArgCheckWord
			cmp	byte ptr [edi],		61h
			jb	_lDFArgSearchWord
			cmp	byte ptr [edi],		7bh
			jb	_lDFArgCheckWord
			cmp	byte ptr [edi],		7ch
			je	_lDFArgSearchWord

			_lDFArgNextWord:
			inc	edi
			_lDFArgNextWordSS:
			cmp	byte ptr [edi],		30h
			jb	_lDFArgSearchWordEx
			cmp	byte ptr [edi],		3ah
			jb	_lDFArgNextWord
			cmp	byte ptr [edi],		41h
			jb	_lDFArgSearchWordEx
			cmp	byte ptr [edi],		5bh
			jb	_lDFArgNextWord
			cmp	byte ptr [edi],		5fh
			je	_lDFArgNextWord
			cmp	byte ptr [edi],		61h
			jb	_lDFArgSearchWordEx
			cmp	byte ptr [edi],		7bh
			jb	_lDFArgNextWord
			jmp	_lDFArgSearchWordEx

				;;----------------
				;; special
				_lDFArgSearchWordSS:
				cmp	word ptr [edi],		7801h	;; #x
				je	_lDFArgSearchWordSSEX
				cmp	word ptr [edi],		7901h	;; #y
				je	_lDFArgSearchWordSSEX

				cmp	word ptr [edi],		6901h	;; #i
				jne	_lDFArgSearchWordSSNorm

				add	edi,			02h
				jmp	_lDFArgSearchWordSx

				_lDFArgSearchWordSSEX:
				add	edi,			06h
				jmp	_lDFArgSearchWordSx

				_lDFArgSearchWordSSNorm:
				add	edi,			02h
				jmp	_lDFArgSearchWordSx
				;;----------------

			_lDFArgCheckWord:
			mov	ebp,			dword ptr [esp]
			mov	dword ptr [_dBuffer],	edi
			jmp	_lDFArgCheckWordStartEX

			_lDFArgCheckWordStart:
			inc	ebp
			inc	edi
			_lDFArgCheckWordStartEX:
			mov	al,			byte ptr [ebp]
			cmp	al,			29h
			je	_lDFArgCheckWordEnd
			cmp	al,			2ch
			je	_lDFArgCheckWordEnd
			cmp	al,			30h
			jb	_lDFArgCheckError ;_lDFArgCheckWordEnd
			cmp	al,			3ah
			jb	_lDFArgCheckTest
			cmp	al,			41h
			jb	_lDFArgCheckError ;_lDFArgCheckWordEnd
			cmp	al,			5bh
			jb	_lDFArgCheckTest
			cmp	al,			5fh
			je	_lDFArgCheckTest
			cmp	al,			61h
			jb	_lDFArgCheckError ;_lDFArgCheckWordEnd
			cmp	al,			7bh
			jb	_lDFArgCheckTest

				;;----------------
				;; error
				_lDFArgCheckError:
				mov	dword ptr [_xErrorTable],	offset _sErr_BadChar
				mov	dword ptr [_xErrorTable+04h],	ebp
				inc	ebp
				mov	dword ptr [_xErrorTable+08h],	ebp
				jmp	_lErrIn
				;;----------------

			_lDFArgCheckTest:
			cmp	al,			byte ptr [edi]
			jne	_lDFArgNextWordSS
			jmp	_lDFArgCheckWordStart

			_lDFArgCheckWordEnd:
			mov	al,			byte ptr [edi]
			cmp	al,			30h
			jb	_lDFArgCheckWordEndFx
			cmp	al,			3ah
			jb	_lDFArgNextWord
			cmp	al,			41h
			jb	_lDFArgCheckWordEndFx
			cmp	al,			5bh
			jb	_lDFArgNextWord
			cmp	al,			5fh
			je	_lDFArgNextWord
			cmp	al,			61h
			jb	_lDFArgCheckWordEndFx
			cmp	al,			7bh
			jb	_lDFArgNextWord			

			_lDFArgCheckWordEndFx:
			mov	ebp,			dword ptr [_dBuffer]
			_lDFArgCheckWordEndEx:
			mov	byte ptr [ebp],		05h	;; ex backspace
			inc	ebp
			cmp	ebp,			edi
			jne	_lDFArgCheckWordEndEx
			mov	byte ptr [edi-01h],	ah
			jmp	_lDFArgSearchWordEx

			_lDFArgCheckEnd:
			pop	ebp
			_lDFArgCheckEndEx:
			inc	ebp
			cmp	byte ptr [ebp],		29h		;; ) 
			jne	_lDFArgCheckEndSx
			jmp	edx
			_lDFArgCheckEndSx:
			cmp	byte ptr [ebp],		2ch		;; ,
			jne	_lDFArgCheckEndEx
			jmp	_lDFNextArg
			;;----------------

		;;----------------
		;; lib check
		_lScanEnd:
		cmp	esp,				_dStackPos
		je	_next

		lea	eax,				dword ptr [offset _dScopeIn+ecx*04h]
		mov	ebx,				dword ptr [eax]
		mov	dword ptr [_xErrorTable],	offset _sErr_UnclosedLib
		mov	eax,				ebx
		_lUncLibErr:
		dec	ebx
		cmp	word ptr [ebx-02h],		0a0dh
		jne	_lUncLibErr
		mov	dword ptr [_xErrorTable+04h],	ebx
		mov	dword ptr [_xErrorTable+08h],	eax
		jmp	_lErrIn
		;;----------------
	_lbl:
	;;----------------

	;;----------------
	;; sort define block
	cmp	ebx,			offset _lDefX+10h
	jbe	_lDFSortEnd

		;;----------------
		;; set step
		mov	eax,			offset _dSortSteps-04h

		sub	ebx,			offset _lDefX
		shr	edx,			02h		;; ??? why ?!
		_lbl:
		add	eax,			04h
		mov	ebp,			dword ptr [eax+04h]
		lea	ebp,			dword ptr [ebp+ebp*02h]
		cmp	ebx,			ebp
		jg	_prew
		;;----------------

	_lDFSordStart:
	mov	ecx,			dword ptr [eax]
	lea	ebx,			dword ptr [_lDefX+ecx]

	_lDFSordGo:
	mov	edi,			ebx
	mov	ebp,			dword ptr [ebx+08h]
	mov	esi,			dword ptr [ebx]
	mov	dl,			byte ptr [esi]
	movaps	xmm1,			[ebx]

	_lbl:
	sub	edi,			ecx
	cmp	edi,			offset _lDefX-10h
	jbe	_lDFSortNext
	mov	esi,			dword ptr [edi]
	cmp	byte ptr [esi],		dl
	jb	_lDFSortNext
	je	_lDFSortNextEx

	_lDFSortRe:
	movaps	xmm0,			[edi]
	movaps	[edi+ecx],		xmm0

	jmp	_prew
	
	_lDFSortNextEx:
	cmp	ebp,			dword ptr [edi+08h]
	jg	_lDFSortRe	

	_lDFSortNext:
	movaps	[edi+ecx],		xmm1	

	add	ebx,			10h
	cmp	dword ptr [ebx],	00h
	jnz	_lDFSordGo

	_lDFNextStep:
	sub	eax,			04h
	cmp	eax,			offset _dSortSteps-04h
	jne	_lDFSordStart

	_lDFSortEnd:
	;;----------------

	;;----------------
	;; build find'n'replace table
	mov	eax,			offset _lDefX-10h
	mov	edx,			offset _dDefTable
	xor	ebx,			ebx
	xor	ecx,			ecx

	_lDFTableBld:
	add	eax,			10h
	cmp	dword ptr [eax],	00h
	je	_lDFBuildEnd
	mov	ebp,			dword ptr [eax]
	mov	bl,			byte ptr [ebp]

	cmp	cl,			bl
	je	_lDFTableBld
	mov	[edx+ebx*04h],		eax
	mov	cl,			bl
	jmp	_lDFTableBld

	_lDFBuildEnd:
	;;----------------

	;;----------------
	;; check defines table
	mov	ebx,			offset _lDefX-10h

	_lDFTableCheckNext:
	add	ebx,			10h
	cmp	dword ptr [ebx],	00h
	jz	_lDFTableCheckEnd

	mov	edx,			dword ptr [ebx]			;; edx = string addr
	mov	ah,			byte ptr [edx]
	mov	ebp,			ebx				;; ebp = checked struct

		;;----------------
		;; check next defines
		_lDFTableCheckStart:
		add	ebp,			10h
		cmp	dword ptr [ebp],	00h
		je	_lDFTableCheckNext
		mov	edi,			dword ptr [ebp]		;; checked string
		cmp	byte ptr [edi],		ah
		jne	_lDFTableCheckNext

			;;----------------
			;; check own lib
			mov	esi,			dword ptr [ebp+08h]
			cmp	esi,			dword ptr [ebx+08h]
			jne	_lDFTableCheckStart
			;;----------------

			;;----------------
			;; check string 
			mov	esi,			edx			;; base string, edi loaded before

			_lbl:
			lodsb
			cmp	al,			02h
			je	_next
			scasb
			je	_prew
			jmp	_lDFTableCheckStart

			_lbl:
			scasb
			jne	_lDFTableCheckStart
			;;----------------

			;;----------------
			;; check arg count
			mov	esi,			dword ptr [ebp+0ch]
			cmp	esi,			dword ptr [ebx+0ch]
			je	_lDFTableCheckValue	;_lDFTableCheckStart

				;;----------------
				;; resort
				mov	esi,			ebp
				mov	edi,			ebx
				jb	_lDFTableCheckSort
				add	edi,			10h

				_lDFTableCheckSort:
				cmp	edi,			esi
				je	_lDFTableCheckSortEnd

				movaps	xmm0,			[esi]

				_lDFTableCheckSortEX:
				sub	esi,			10h
				movaps	xmm1,			[esi]
				movaps	[esi+10h],		xmm1

				cmp	edi,			esi
				jne	_lDFTableCheckSortEX

				movaps	[edi],			xmm0

				_lDFTableCheckSortEnd:
				mov	dword ptr [ebx+18h],	0ffffffffh	;; modify to overload define
				jmp	_lDFTableCheckStart
				;;----------------

			;;----------------

			;;----------------
			;; check value
			_lDFTableCheckValue:
			mov	esi,			dword ptr [ebp+04h]
			mov	edi,			dword ptr [ebx+04h]

			_lbl:
			lodsb
			cmp	al,			03h
			je	_next
			scasb
			je	_prew
			jmp	_lDFTableError

			_lbl:
			scasb
			je	_lDFTableCheckStart
			;;----------------

			;;----------------
			;; error
			_lDFTableError:
			mov	dword ptr [_xErrorTable],	offset _sErr_ValueRedefined
			mov	edi,				dword ptr [ebp]
			mov	dword ptr [_xErrorTable+04h],	edi
			_lbl:
			inc	edi
			cmp	byte ptr [edi],			02h
			jne	_prew
			mov	dword ptr [_xErrorTable+08h],	edi

			mov	dword ptr [_xErrorTable+10h],	offset _sErr_ValueRedefinedEX
			mov	edi,				dword ptr [ebx]
			mov	dword ptr [_xErrorTable+14h],	edi
			_lbl:
			inc	edi
			cmp	byte ptr [edi],			02h
			jne	_prew
			mov	dword ptr [_xErrorTable+18h],	edi

			jmp	_lErrIn
			;;----------------

		;;----------------

	_lDFTableCheckEnd:
	;;----------------

	mov	_dCurrStr,		offset _sProg_03
	mov	eax,			48h
	call	_lSetProg

	;;----------------
	;; find'n'replacing
	mov	_dStackPos,			esp		;; save stack
	mov	edi,				dword ptr [esp]
	mov	esi,				dword ptr [esp+08h]
	xor	eax,				eax
	jmp	_lXFPStart

	_lXFPNewWord:
	movsb
	_lXFPStart:

		;;----------------
		;; string preprocessor in
		cmp	dword ptr [esi],		73404021h	;; !@@s
		je	_lXFPStrPrIn
		cmp	dword ptr [esi],		65404021h	;; !@@e
		je	_lXFPStrPrOut
		;;----------------

	_lXFPStartFX:
	mov	al,				byte ptr [esi]

	cmp	al,				80h
	jb	_lXFPDFF

		;;----------------
		;; arguments
		inc	esi
		sub	eax,				80h
		push	esi
		lea	edx,				dword ptr [esp+eax*08h+10h]
		cmp	dword ptr [edx],		00ffh
		jg	_lFXPCSX
		add	edx,				04h
		_lFXPCSX:
		mov	esi,				dword ptr [edx]
		jmp	_lXFPStart
		;;----------------

		;;----------------
		;; check word
		_lXFPDFF:
		cmp	al,				41h
		jb	_lXFPGetNextEx

		lea	edx,				[_dDefTable+eax*04h]
		mov	edx,				dword ptr [edx]		;; edx = def block address
		test	edx,				edx
		jz	_lXFPGetNextEx

		mov	ecx,				esi			;; ecx = temt src script position
		mov	dword ptr [_dUndefPnt],		esi			;; for undef
		_lXFPCheck:
		mov	ebx,				dword ptr [edx]
		mov	al,				byte ptr [esi]
		cmp	al,				byte ptr [ebx]
		jne	_lXFPGetNext

		_lXFPCheckStart:
		inc	esi
		inc	ebx
		mov	al,				byte ptr [esi]
		cmp	al,				byte ptr [ebx]
		je	_lXFPCheckStart
		cmp	byte ptr [ebx],			02h
		jne	_lXFPNext

		cmp	byte ptr [_bAscii_00+eax],	ah		;; ah = 00h
		je	_lXFPFound

		mov	al,				byte ptr [ebx-01h]
		cmp	byte ptr [_bAscii_00+eax],	ah		;; ah = 00h
		jne	_lXFPNext
		;;----------------

		;;----------------
		;; is private
		_lXFPFound:
		mov	ebx,			dword ptr [edx+08h]
		test	ebx,			ebx
		jnz	_lXFPLibTest
		;;----------------

		;;----------------
		;; arguments?
		_lXFPAct:
		mov	dword ptr [_hOLMacro],	edx
		mov	ecx,			dword ptr [edx+04h]
		cmp	dword ptr [edx+0ch],	00h
		jne	_lXFPActArg

			;;----------------
			;; overloaded macros
			cmp	dword ptr [edx+18h],	0ffffffffh	;; is macros overload
			jne	_lXFPNoOverLoad
			cmp	byte ptr [esi],		28h		;; (
			jne	_lXFPNoOverLoad
			cmp	word ptr [esi],		2928h		;; ()
			jne	_lXPFArgErrEX
			add	esi,			02h
			;;----------------

		_lXFPNoOverLoad:

			;;----------------
			;; undefined?
			test	ecx,			ecx
			jz	_lXFPUndefined
			;;----------------

		push	esi
		push	00h
		mov	esi,			ecx
		xor	eax,			eax
		jmp	_lXFPStart

		_lXFPActArg:
		mov	edx,			dword ptr [edx+0ch]
		mov	eax,			offset _dDefArgs
		cmp	byte ptr [esi],		28h		;; (
		mov	ebp,			edx
		je	_lXFPNextArg

			;;----------------
			;; error
			_lXPFArgErr:
			mov	dword ptr [_xErrorTable],	offset _sErr_MissDefArg
			mov	dword ptr [_xErrorTable+08h],	esi
			dec	esi
			mov	dword ptr [_xErrorTable+04h],	esi
			jmp	_lErrIn
			;;----------------

			;;----------------
			;; next macro
			_lXPFArgErrEX:
			cmp	byte ptr [esi],		28h		;; (
			jne	_lXPFArgErr
			add	edx,			10h
			mov	dword ptr [_hOLMacro],	edx
			mov	ecx,			dword ptr [edx+04h]
			jmp	_lXFPActArg
			;;----------------
		;;----------------

		;;----------------
		;; parse arguments
		_lXFPNextArgAdd:
		mov	byte ptr [esi],		04h

		_lXFPNextArg:
		dec	edx
		js	_lXFPArgEnd

		_lXFPNextArgFX:
		inc	esi
		cmp	byte ptr [esi],		05h
		je	_lXFPNextArgFX

			;;----------------
			;; arg transfer
			cmp	byte ptr [esi],		80h
			jb	_lXFPNextArgEEX

			xor	ebx,			ebx
			mov	bl,			byte ptr [esi]
			sub	bl,			80h
			lea	ebx,			dword ptr [esp+ebx*08h+0ch]
			mov	ebx,			dword ptr [ebx]
			mov	dword ptr [eax],	ebx
			mov	dword ptr [eax+04h],	00h
			add	eax,			08h
			inc	esi

			cmp	byte ptr [esi],		29h	;; )
			jne	_lXFPMArgTrns
			test	edx,			edx
			jnz	_lXPFArgErr
;;			mov	byte ptr [esi],		04h
			jmp	_lXFPArgEndOX

			_lXFPMArgTrns:
			cmp	byte ptr [esi],		2ch	;; ,
			jne	_lXFPMArgErr
;;			jmp	_lXFPNextArgAdd
			jmp	_lXFPNextArg
			;;----------------

			;;----------------
			;; complex arg
			_lXFPNextArgEEX:
			mov	dword ptr [eax],	esi
			add	eax,			04h

			xor	ebx,			ebx

			cmp	byte ptr [esi],		0bh
			je	_lXFPHardArg
			cmp	byte ptr [esi],		3ch	;; <
			jne	_lXFPArgSX			;; simple arg
			inc	dword ptr [eax-04h]
			inc	ebx
			_lXFPMArg:
			inc	esi
			cmp	byte ptr [esi],		22h	;; "
			je	_lXFPMArgStr
			cmp	byte ptr [esi],		80h
			jb	_lXFPMArgFX

			shl	ebx,			10h	;; save bx
			mov	bx,			bp	;; defines arg count
			add	bx,			80h
			cmp	byte ptr [esi],		bl

			jg	_lXFPMArgOX

			mov	bh,			byte ptr [esi]
			sub	bh,			7fh
			add	bh,			bl
			mov	byte ptr [esi],		bh

			_lXFPMArgOX:
			shr	ebx,			10h	;; load bx

			_lXFPMArgFX:
			cmp	byte ptr [esi],		3eh	;; >
			je	_lXFPMArgEX
			cmp	byte ptr [esi],		3ch	;; <
			jne	_lXFPMArg
			inc	ebx
			jmp	_lXFPMArg

			_lXFPMArgEX:
			dec	ebx
			jnz	_lXFPMArg
			mov	byte ptr [esi],		04h
			mov	dword ptr [eax],	3eh

			_lXFPMArgETT:
			add	eax,			04h

			inc	esi
			cmp	byte ptr [esi],		29h	;; )
			jne	_lXFPMArgDX
			test	edx,			edx
			jnz	_lXPFArgErr
			jmp	_lXFPArgEndOX

			_lXFPMArgDX:
			cmp	byte ptr [esi],		2ch	;; ,
;;			jne	_lXFPMArgErr
;;			jmp	_lXFPNextArg
			je	_lXFPNextArg

				;;----------------
				;; error
				_lXFPMArgErr:
				mov	dword ptr [_xErrorTable],	offset _sErr_Base
				mov	dword ptr [_xErrorTable+04h],	esi
				dec	esi
				mov	dword ptr [_xErrorTable+08h],	esi
				jmp	_lErrIn
				;;----------------

			_lXFPMArgStrEX:
			inc	esi
			_lXFPMArgStr:
			inc	esi
			cmp	byte ptr [esi],		5ch	;; \ 
			je	_lXFPMArgStrEX
			cmp	byte ptr [esi],		22h	;; "
			jne	_lXFPMArgStr
			jmp	_lXFPMArg
			;;----------------

			;;----------------
			;; hard arg
			_lXFPHardArg:
			inc	dword ptr [eax-04h]
			inc	ebx
			_lXFPHardArgEX:
			inc	esi
			cmp	byte ptr [esi],		0bh
			jne	_lXFPHardArgEX

			mov	byte ptr [esi],		04h
			mov	dword ptr [eax],	0bh
			jmp	_lXFPMArgETT
			;;----------------

			;;----------------
			;; simple arg
			_lXFPArgDX:
			inc	ebx

			_lXFPArgEX:
			inc	esi

			_lXFPArgSX:
			cmp	byte ptr [esi],		22h	;; "
			je	_lXFPArgGX
			cmp	byte ptr [esi],		28h	;; ( 
			je	_lXFPArgDX
			cmp	byte ptr [esi],		29h	;; )
			je	_lXFPArgXX
			cmp	byte ptr [esi],		2ch	;; ,
			jne	_lXFPArgEX
			test	ebx,			ebx
			jnz	_lXFPArgEX
			mov	dword ptr [eax],	2ch
			add	eax,			04h
			jmp	_lXFPNextArgAdd

			_lXFPArgXX:
			dec	ebx
			jns	_lXFPArgEX
			test	edx,			edx
			jnz	_lXPFArgErr
			mov	dword ptr [eax],	29h
			add	eax,			04h
			jmp	_lXFPNextArgAdd

			_lXFPArgGS:
			inc	esi
			_lXFPArgGX:
			inc	esi
			cmp	byte ptr [esi],		5ch	;; \ 
			je	_lXFPArgGS
			cmp	byte ptr [esi],		22h	;; "
			jne	_lXFPArgGX
			jmp	_lXFPArgEX
			;;----------------

			;;----------------
			_lXFPArgEnd:
			cmp	dword ptr [eax-04h],	3eh	;; >
			jne	_lXFPArgEndSX
			cmp	byte ptr [esi-01h],	29h	;; )
			jne	_lXFPOverload
			_lXFPArgEndSX:
			cmp	dword ptr [eax-04h],	29h	;; )
			jne	_lXFPOverload
			_lXFPArgEndOX:

				;;----------------
				;; undefined?
				test	ecx,			ecx
				jz	_lXFPUndefined
				;;----------------

			inc	esi
			_lXFPArgEndEX:
			push	dword ptr [eax-08h]
			push	dword ptr [eax-04h]
			sub	eax,			08h
			cmp	eax,			offset _dDefArgs
			jne	_lXFPArgEndEX

			push	esi
			push	ebp
			mov	esi,			ecx
			xor	eax,			eax
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; next overloaded macros
			_lXFPOverload:
			mov	ebx,			dword ptr [_hOLMacro]
			add	ebx,			10h
			cmp	dword ptr [ebx+08h],	0ffffffffh
			je	_lXFPOverloadEX

				;;----------------
				;; error
				mov	dword ptr [_xErrorTable],	offset _sErr_DefArg
				mov	dword ptr [_xErrorTable+04h],	esi
				dec	esi
				mov	dword ptr [_xErrorTable+08h],	esi
				jmp	_lErrIn
				;;----------------

			_lXFPOverloadEX:
			mov	ecx,			dword ptr [ebx+04h]
			mov	dword ptr [_hOLMacro],	ebx

			mov	edx,			dword ptr [ebx+0ch]
			mov	ebp,			edx
			sub	edx,			dword ptr [ebx-04h]

			jmp	_lXFPNextArg
			;;----------------
		;;----------------

		;;----------------
		;; check own lib
		_lXFPLibTest:
		cmp	ecx,			dword ptr [_dScopeIn+ebx*04h]
		jb	_lXFPNext
		cmp	ecx,			dword ptr [_dScopeOut+ebx*04h]
		jb	_lXFPAct
		;;----------------

		;;----------------
		;; check next define
		_lXFPNext:
		mov	esi,			ecx
		_lXFPNextEX:
		add	edx,			10h
		cmp	dword ptr [edx],	00h
		je	_lXFPGetNext
			;;----------------
			;; overloaded?
			cmp	dword ptr [edx+08h],	0ffffffffh
			je	_lXFPNextEX
			;;----------------
		jmp	_lXFPCheck
		;;----------------

		;;----------------
		;; undefined
		_lXFPUndefined:
		mov	esi,			dword ptr [_dUndefPnt]

			;;----------------
			;; remove arg
			_lXFPUndefinedRE:
			cmp	eax,			offset _dDefArgs	;; ???
			jbe	_lXFPUndefinedEnd

			mov	dl,			byte ptr [eax-04h]
			test	dl,			dl
			jz	_lXFPUndefinedREEX
			mov	ebx,			dword ptr [eax-08h]
			dec	ebx
			
			_lXFPUndefinedRemArg:
			inc	ebx
			_lXFPUndefinedRemArgEX:
			cmp	word ptr [ebx],		7801h	;; #x
			je	_lXFPUndefinedRemArgSX
			cmp	word ptr [ebx],		7901h	;; #y
			je	_lXFPUndefinedRemArgSX
			cmp	byte ptr [ebx],		04h
			jne	_lXFPUndefinedRemArg

			mov	byte ptr [ebx],		dl

			_lXFPUndefinedREEX:
			sub	eax,			08h
			jmp	_lXFPUndefinedRE

			_lXFPUndefinedRemArgSX:
			add	ebx,			06h
			jmp	_lXFPUndefinedRemArgEX
			;;----------------

		_lXFPUndefinedEnd:
		xor	eax,			eax
		jmp	_lXFPGetNext
		;;----------------

		;;----------------
		;; remove bs
		_lXFPBSRem:
		xor	eax,				eax
		mov	al,				byte ptr [edi-01h]	;; bs
		cmp	byte ptr [_bAscii_00+eax],	ah
		jne	_lXFPNewWord
		cmp	byte ptr [esi+01h],		2bh			;; +
		je	_lXFPNewWord
		cmp	byte ptr [esi+01h],		2dh			;; -
		je	_lXFPNewWord

		inc	esi
		jmp	_lXFPStartFX
		;;----------------

		;;----------------
		;; get next word and parse text
		_lXFPGetNext:
		movsb
		mov	al,			byte ptr [esi]
		_lXFPGetNextEx:
		cmp	al,			05h	;; ex backspace
		jne	_next
		inc	esi
		jmp	_lXFPStart
		_lbl:
		cmp	al,			20h
		jb	_lXFPXX
		je	_lXFPBSRem
		cmp	al,			22h	;; "
		je	_lXFPString
		cmp	word ptr [esi],		2323h	;; ##
		je	_lXFPConf
		cmp	al,			30h
		jb	_lXFPNewWord
		cmp	al,			3ah
		jb	_lXFPGetNext
		cmp	al,			41h
		jb	_lXFPNewWord
		cmp	al,			5bh
		jb	_lXFPGetNext
		cmp	al,			5fh
		jb	_lXFPNewWord
		cmp	al,			60h	;; `
		je	_lXPFRepStr
		cmp	al,			7ah
		jg	_lXFPNewWord
		jmp	_lXFPGetNext

		_lXFPConf:
		add	esi,			02h
		jmp	_lXFPStart

			;;----------------
			;; string
			_lXFPStringEX:
			movsb
			_lXFPString:
			movsb
			cmp	byte ptr [esi],		5ch	;; \ 
			je	_lXFPStringEX
			cmp	byte ptr [esi],		22h	;; "
			jne	_lXFPString
			jmp	_lXFPNewWord

			_lXPFRepStr:
			mov	byte ptr [edi],		22h	;; "
			inc	esi
			inc	edi
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; system
			_lXFPXX:
			cmp	al,			08h
			je	_lXFPSetdef
			cmp	al,			04h
			je	_lXFPArgOut
			cmp	al,			03h
			je	_lXFPDefOut
			cmp	al,			0bh
			je	_lXFPTMArg

			cmp	word ptr [esi],		3801h	;; #8
			jne	_lXFPXX_Next
			movsw
			jmp	_lXFPStart

			_lXFPXX_Next:
			cmp	word ptr [esi],		6401h	;; #d
			je	_lXFP_00
			cmp	word ptr [esi],		6701h	;; #g
			je	_lXFP_0a
			cmp	word ptr [esi],		6301h	;; #c
			je	_lXFP_01
			cmp	word ptr [esi],		7801h	;; #x
			je	_lXFP_02
			cmp	word ptr [esi],		7901h	;; #y
			je	_lXFP_02
			cmp	word ptr [esi],		7301h	;; #s
			je	_lXFP_03
			cmp	word ptr [esi],		6901h	;; #i
			je	_lXFP_04
			cmp	word ptr [esi],		6101h	;; #a
			je	_lXFP_05
			cmp	word ptr [esi],		6501h	;; #e
			je	_lXFP_06
			cmp	word ptr [esi],		7501h	;; #u
			je	_lXFP_07
			cmp	word ptr [esi],		3601h	;; #6
			je	_lXFP_08
			cmp	word ptr [esi],		3701h	;; #7
			je	_lXFP_09
			cmp	al,			00h
			je	_lXFPEnd
			jmp	_lXFPNewWord
			;;----------------

			;;----------------
			_lXFPTMArg:
			mov	byte ptr [edi],		22h	;; "
			inc	esi
			inc	edi
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; set esi
			_lXFP_03:
			mov	esi,			dword ptr [esi+02h]
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; arg out
			_lXFPArgOut:
			pop	esi
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; error
			_lXFP_0a:
			mov	dword ptr [_xErrorTable],	offset _sErr_Ude
			mov	dword ptr [_xErrorTable+04h],	esi
			_lXFP_0a_EX:
			inc	esi
			cmp	word ptr [esi],			0a0dh		;; nl
			jne	_lXFP_0a_EX
			mov	dword ptr [_xErrorTable+08h],	esi
			jmp	_lErrIn
			;;----------------

			;;----------------
			;; setdef
			_lXFPSetdef:
			mov	al,			byte ptr [esi+07h]
			lea	edx,			[_dDefTable+eax*04h]
			mov	edx,			dword ptr [edx]		;; edx = def block address
			test	edx,			edx
			jnz	_lXFPSetdefCheck

;; error
			_lXFPSetdefCheckNext:
			add	edx,			10h
			_lXFPSetdefCheck:
			lea	ecx,			[esi+07h]
			mov	ebx,			dword ptr [edx]
			mov	al,			byte ptr [ecx]
			cmp	al,			byte ptr [ebx]
;;			jne
			_lXFPSetdefCheckEX:
			inc	ecx
			inc	ebx
			mov	al,			byte ptr [ecx]
			cmp	al,			02h			;; def label end
			je	_lXFPSetdefCheckEnd
			cmp	al,			byte ptr [ebx]
			je	_lXFPSetdefCheckEX
			jmp	_lXFPSetdefCheckNext

			_lXFPSetdefCheckEnd:
			cmp	al,			byte ptr [ebx]
			jne	_lXFPSetdefCheckNext

				;;----------------
				;; check lib
				mov	ecx,			dword ptr [edx+08h]

				test	ecx,			ecx
				jz	_lXFPSetdefGlob

				cmp	esi,			dword ptr [_dScopeIn+ecx*04h]
				jb	_lXFPSetdefCheckNext
				cmp	esi,			dword ptr [_dScopeOut+ecx*04h]
				jg	_lXFPSetdefCheckNext
				;;----------------

				;;----------------
				;; check arg count
				_lXFPSetdefGlob:
				mov	al,			byte ptr [esi+01h]
				cmp	eax,			dword ptr [edx+0ch]
				jne	_lXFPSetdefCheckNext
				;;----------------

			mov	ecx,			dword ptr [esi+02h]
			mov	dword ptr [edx+04h],	ecx			;; huh...

			add	esi,			08h
			_lXFPSetdefEnd:
			inc	esi
			cmp	byte ptr [esi],		03h
			jne	_lXFPSetdefEnd
			add	esi,			02h
;;			cmp	byte ptr [esi-01h],	79h	;; y
;;			jne	_lXFPStart
;;			add	esi,			0eh
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; undef
			_lXFP_07:
			mov	al,			byte ptr [esi+06h]
			lea	edx,			[_dDefTable+eax*04h]
			mov	edx,			dword ptr [edx]		;; edx = def block address
			test	edx,			edx
			jnz	_lXFPUndefCheck

;; error
			_lXFPUndefCheckNext:
			add	edx,			10h
			_lXFPUndefCheck:
			lea	ecx,			[esi+06h]
			mov	ebx,			dword ptr [edx]
			mov	al,			byte ptr [ecx]
			cmp	al,			byte ptr [ebx]
;;			jne
			_lXFPUndefCheckEX:
			inc	ecx
			inc	ebx
			mov	al,			byte ptr [ecx]
			cmp	al,			02h			;; def label end
			je	_lXFPUndefCheckEnd
			cmp	al,			byte ptr [ebx]
			je	_lXFPUndefCheckEX
			jmp	_lXFPUndefCheckNext

			_lXFPUndefCheckEnd:
			cmp	al,			byte ptr [ebx]
			jne	_lXFPUndefCheckNext

				;;----------------
				;; check lib
				mov	ebx,			dword ptr [edx+08h]

				test	ebx,			ebx
				jz	_lXFPUndefRem

				cmp	esi,			dword ptr [_dScopeIn+ebx*04h]
				jb	_lXFPUndefCheckNext
				cmp	esi,			dword ptr [_dScopeOut+ebx*04h]
				jg	_lXFPUndefCheckNext

				jmp	_lXFPUndefRem
				;;----------------

			_lXFPUndefRemEX:
			add	edx,			10h
			_lXFPUndefRem:
			mov	dword ptr [edx+04h],	00000000h		;; huh...
			cmp	dword ptr [edx+18h],	0ffffffffh		;; is overloaded?
			je	_lXFPUndefRemEX

			lea	esi,			[ecx+01h]

			cmp	byte ptr [esi],		0ah
			jne	_lXFPStartFX
			inc	esi
			jmp	_lXFPStartFX
			;;----------------

			;;----------------
			;; counter
			_lXFP_08:
			add	esi,			02h
			mov	eax,			dword ptr [_dCounterV]

			inc	edi
			cmp	eax,			0ah
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			64h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			03e8h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			2710h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			000186a0h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			000f4240h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			00989680h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			05f5e100h
			jb	_lXFP_08_FX

			inc	edi
			cmp	eax,			3b9aca00h
			jb	_lXFP_08_FX

			inc	edi

			mov	ecx,			0ah

			_lXFP_08_FX:
			mov	ebp,			edi
			_lXFP_08_EX:
			xor	edx,			edx
			dec	ebp
			div	ecx
			add	dl,			30h
			mov	byte ptr [ebp],		dl
			test	eax,			eax
			jnz	_lXFP_08_EX

			inc	dword ptr [_dCounterV]
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; weather
			_lXFP_09:
			add	esi,			02h
			rdtsc
			and	eax,			01h
			or	eax,			30h
			stosb
			xor	eax,			eax
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; def out
			_lXFPDefOut:
			pop	eax
			pop	esi
			_lXFPDefOutEX:
			test	eax,			eax
			jz	_lXFPStart
			mov	ecx,			dword ptr [esp]
			test	ecx,			ecx
			jz	_lXFPDefOutSBX
			mov	ebp,			dword ptr [esp+04h]
			dec	ebp
			_lXFPDefOutDX:
			inc	ebp
			_lXFPDefOutAX:
			cmp	word ptr [ebp],		7801h	;; #x
			je	_lXFPDefOutBX
			cmp	word ptr [ebp],		7901h	;; #y
			je	_lXFPDefOutBX
			cmp	byte ptr [ebp],		04h
			jne	_lXFPDefOutDX

			mov	byte ptr [ebp],		cl
			_lXFPDefOutSBX:
			add	esp,			08h
			dec	eax
			jmp	_lXFPDefOutEX

			_lXFPDefOutBX:
			add	ebp,			06h
			jmp	_lXFPDefOutAX
			;;----------------

			;;----------------
			;; #i

				;;----------------
				;; #if
				_lXFP_04:
				cmp	word ptr [esi+02h],		2066h		;; f_ ;; #if_
				jne	_lXFP_04_FX

				mov	dword ptr [_xEnumTable],	edi
				add	esi,				04h
				mov	edi,				offset _xEnumTable+04h
				jmp	_lXFPStartFX
				;;----------------

				;;----------------
				;; #elseif
				_lXFP_04_FX:
				cmp	dword ptr [esi+02h],		6965736ch	;; lsei
				jne	_lXFP_04_DX
				cmp	word ptr [esi+06h],		2066h		;; f_ ;; #if_
				jne	_lXFP_04_DX

				mov	dword ptr [_xEnumTable],	edi
				add	esi,				08h
				mov	edi,				offset _xEnumTable+04h
				jmp	_lXFPStartFX
				;;----------------

				;;----------------
				;; #else
				_lXFP_04_DX:
				cmp	dword ptr [esi+02h],		0165736ch	;; lse#
				jne	_lXFP_04_RX

				mov	ecx,				dword ptr [esi+09h]
				mov	word ptr [ecx],			0606h
				mov	dword ptr [ecx+02h],		06060606h
				add	esi,				0dh
				jmp	_lXFPStartFX
				;;----------------

				;;----------------
				;; #endif
				_lXFP_04_RX:
				cmp	dword ptr [esi+02h],		6669646eh	;; ndif
				jne	_lCRErrPrePorc					;; omg!
				add	esi,				06h
				jmp	_lXFPStartFX
				;;----------------
			;;----------------

			;;----------------
			;; #e
			_lXFP_06:
			mov	word ptr [esi],		6901h	;; #i
			_lXFP_06_EX:
			inc	esi
			cmp	word ptr [esi],		7801h	;; #x
			jne	_lXFP_06_EX

			mov	esi,			dword ptr [esi+02h]
			add	esi,			10h
			jmp	_lXFPStartFX			
			;;----------------

			;;----------------
			;; #d
			_lXFP_00:
			inc	esi

			_lXFP_00_Re:
			cmp	word ptr [esi],		7801h	;; #x
			je	_lXFP_00_FX
			cmp	word ptr [esi],		7901h	;; #y
			je	_lXFP_00_FX

			cmp	word ptr [esi],		6401h	;; #d
			jne	_lXFP_00
			add	esi,			02h
			jmp	_lXFPStart

			_lXFP_00_FX:
			add	esi,			06h
			jmp	_lXFP_00_Re
			;;----------------

			;;----------------
			;; #a
			_lXFP_05:
			xor	ebx,			ebx
			mov	dword ptr [edi],	ebx
			mov	edi,			dword ptr [_xEnumTable]
			mov	edx,			offset _xEnumTable+04h
			mov	ecx,			edx

				;;----------------
				;; true ?
				cmp	word ptr [ecx],		0031h		;; 1
				je	_lXFP_05_SX

				cmp	dword ptr [ecx],	65757274h	;; true
				jne	_lXFP_05_lStr
				cmp	byte ptr [ecx+04h],	00h
				jne	_lXFP_05_lStr

				jmp	_lXFP_05_SX
				;;----------------

			_lXFP_05_AX:
			inc	ecx
			_lXFP_05_lStr:
			cmp	byte ptr [ecx],		00h
			je	_lXFP_05_Fail

			cmp	word ptr [ecx],		3d3dh	;; ==
			je	_lXFP_05_BX			;; ebx = 00h
			cmp	word ptr [ecx],		3d21h	;; !=
			jne	_lXFP_05_AX
			mov	ebx,			eax	;; non zero

			_lXFP_05_BX:
			add	ecx,			02h
			_lXFP_05_OX:
			mov	al,			byte ptr [edx]
			cmp	al,			byte ptr [ecx]
			jne	_lXFP_05_NE
			inc	edx
			inc	ecx
			jmp	_lXFP_05_OX

			_lXFP_05_NE:
			cmp	word ptr [edx],		3d3dh	;; ==
			je	_lXFP_05_NE_EX
			cmp	word ptr [edx],		3d21h	;; !=
			jne	_lXFP_05_NE_FX

			_lXFP_05_NE_EX:
			cmp	byte ptr [ecx],		00h
			je	_lXFP_05_XX

				;;----------------
				;; fail
				_lXFP_05_NE_FX:
				test	ebx,			ebx
				jnz	_lXFP_05_SX

				_lXFP_05_Fail:
				mov	esi,			dword ptr [esi+04h]
				add	esi,			10h
				jmp	_lXFPStartFX
				;;----------------

				;;----------------
				_lXFP_05_XX:
				test	ebx,			ebx
				jnz	_lXFP_05_Fail

				_lXFP_05_SX:
				mov	ecx,			dword ptr [esi+04h]
				mov	word ptr [ecx],		0606h
				mov	dword ptr [ecx+02h],	06060606h
				add	esi,			08h

				_lXFP_05_FX:
				add	ecx,			10h
				cmp	dword ptr [ecx],	646e6901h	;; #ind ;; #end
				je	_lXFPStartFX

				mov	word ptr [ecx],		6501h		;; #e
				_lXFP_05_Remove:
				inc	ecx
				cmp	word ptr [ecx],		7801h		;; #x
				jne	_lXFP_05_Remove
				mov	ecx,			dword ptr [ecx+02h]
				jmp	_lXFP_05_FX
				;;----------------
			;;----------------

			;;----------------
			;; #c
			_lXFP_01_Ex:
			add	esi,			05h
			_lXFP_01:
			inc	esi
			cmp	byte ptr [esi],		07h	;; enum out
			jne	_lXFP_01_Sx	
			inc	esi
			jmp	_lXFPStart
			_lXFP_01_Sx:
			cmp	word ptr [esi],		7801h	;; #x
			je	_lXFP_01_Ex
			cmp	word ptr [esi],		7901h	;; #y
			je	_lXFP_01_Ex
			cmp	word ptr [esi],		0302h	;; null define
			je	_lXFP_01_Dx
			cmp	byte ptr [esi],		03h
			jne	_lXFP_01
			add	esi,			02h
			cmp	byte ptr [esi-01h],	79h	;; y
			jne	_lXFPStart
			add	esi,			0eh
			jmp	_lXFPStart

			_lXFP_01_Dx:				;; ???
			add	esi,			02h
			cmp	byte ptr [esi],		0ah
			jne	_lXFPStart
			inc	esi
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; blocks
			_lXFP_02:
			movsw
			mov	edx,			dword ptr [esi]
			sub	edi,			02h
			mov	dword ptr [edx+02h],	edi
			add	edi,			06h
			add	esi,			04h
			jmp	_lXFPStart
			;;----------------
		;;----------------

		;;----------------
		;; string preprocessor

			;;----------------
			;; in
			_lXFPStrPrIn:
			add	esi,				04h
			mov	dword ptr [_dMapProcCode],	edi
			mov	edi,				offset _bFuncCodeBase
			jmp	_lXFPStart
			;;----------------

			;;----------------
			;; out
			_lXFPStrPrOut:
			movsd							;; copy !@@e

			mov	dword ptr [_dWWWFont],		esi
			mov	esi,				offset _bFuncCodeBase	;; esi = old buffer
			mov	edi, 				offset _bFuncCodeLocals	;; edi = new buffer

			mov	ebx,				esi
			_lXFPStrPrOutAX:
			inc	ebx
			cmp	dword ptr [ebx],		61404021h	;; !@@a
			jne	_lXFPStrPrOutAX					;; ebx = arg types

			lea	ecx,				[ebx+03h]
			_lXFPStrPrOutBX:
			inc	ecx
;;			cmp	dword ptr [ecx],		65404021h	;; !@@e
;;			error - no args
			cmp	byte ptr [ecx-01h],		40h		;; @
			jne	_lXFPStrPrOutBX					;; ecx = first arg

			jmp	_lXFPStrPrOutDX

			_lXFPStrPrOutEX:
			stosb
			_lXFPStrPrOutCX:
			cmp	esi,				ebx
			jne	_lXFPStrPrOutDX

				;;----------------
				;; huh...
				mov	word ptr [edi],		7301h		;; #s
				mov	ebx,			dword ptr [_dWWWFont]
				mov	dword ptr [edi+02h],	ebx
				mov	esi,			offset _bFuncCodeLocals
				mov	edi,			dword ptr [_dMapProcCode]
				jmp	_lXFPStart
				;;----------------

			_lXFPStrPrOutSX:
			cmp	byte ptr [esi],			5eh		;; ^
			je	_lXFPStrPrOutTX
			cmp	byte ptr [esi],			25h		;; %
			je	_lXFPStrPrOutTX
			stosb
			_lXFPStrPrOutTX:
			movsb
			_lXFPStrPrOutDX:
			lodsb
			cmp	al,				5ch		;; \ 
			je	_lXFPStrPrOutSX
			cmp	al,				25h		;; %
			je	lXFPStrPr_GX
			cmp	al,				5eh		;; ^
			jne	_lXFPStrPrOutEX

				;;----------------
				;; find type
				lXFPStrPr_GX:
				lea	ebp,				[ebx+06h]	;; ebp = first arg type

				_lXFPStrPrOutJX:
				mov	edx,				esi		;; edx = position in string

				_lXFPStrPrOutFX:
				mov	al,				byte ptr [ebp]

				cmp	al,				5eh		;; ^
				je	_lXFPStrPrOutGX

				cmp	al,				byte ptr [edx]
				jne	_lXFPStrPrOutHX
				inc	ebp
				inc	edx
				jmp	_lXFPStrPrOutFX

				_lXFPStrPrOutHX:
				inc	ebp
				cmp	byte ptr [ebp-01h],		5eh		;; ^
				jne	_lXFPStrPrOutHX
				jmp	_lXFPStrPrOutJX

				_lXFPStrPrOutGX:
				mov	esi,				edx

					;;----------------
					;; replace
					add	ebp,				02h	;; remove ^=

					_lXFPStrPrOutLX:
					mov	al,				byte ptr [ebp]
					cmp	al,				3ch	;; <
					je	_lXFPStrPrOutIX
					cmp	al,				3eh	;; >
					je	_lXFPStrPrOutKX
					cmp	al,				24h	;; $
					je	_lXFPStrPrOutMX
					cmp	al,				5eh	;; ^
					je	_lXFPStrPrOutCX
					cmp	al,				22h	;; "
					je	_lXFPStrPrOutCX
					mov	byte ptr [edi],		al
					inc	edi
					inc	ebp
					jmp	_lXFPStrPrOutLX

					_lXFPStrPrOutMX:
					inc	ebp
					_lXFPStrPrOutNX:
					cmp	dword ptr [ecx],		65404021h	;; !@@e
					je	_lXFPStrPrOutLX
					mov	al,				byte ptr [ecx]
					cmp	al,				40h		;; @
					je	_lXFPStrPrOutOX
					mov	byte ptr [edi],		al
					inc	edi
					inc	ecx
					jmp	_lXFPStrPrOutNX
					_lXFPStrPrOutOX:
					inc	ecx
					jmp	_lXFPStrPrOutLX

					_lXFPStrPrOutIX:
					inc	ebp
;;					cmp	byte ptr [edx-01h],		22h	;; "
;;					je	_lXFPStrPrOutUX				;; ???
					mov	word ptr [edi],			2b22h	;; "+
					add	edi,				02h
					jmp	_lXFPStrPrOutLX

;;					_lXFPStrPrOutUX:
;;					inc	edx
;;					jmp	_lXFPStrPrOutLX

					_lXFPStrPrOutKX:
					inc	ebp
;;					cmp	byte ptr [esi],			22h	;; " ;; [edx+01h] ???
;;					je	_lXFPStrPrOutPX
					mov	word ptr [edi],			222bh	;; +"
					add	edi,				02h
					jmp	_lXFPStrPrOutLX

					_lXFPStrPrOutPX:
					inc	esi
					jmp	_lXFPStrPrOutLX
					;;----------------
				;;----------------



			;;----------------
		;;----------------

	_lXFPEnd:
	mov	esp,			_dStackPos	;; load stack
	add	esi,			04h
	add	edi,			04h
	;;----------------

	mov	_dCurrStr,	offset _sProg_04
	mov	eax,		52h
	call	_lSetProg

	;;----------------
	;; capture world...
	mov	dword ptr [_dErrorCodeStart],	edi	;; for syntax error
	mov	_dStackPos,			esp

	xor 	ebx,				ebx	;; f l a g s
							;; 0 0 0 0 b
							;; | | | |
							;; | | | is in function (0 - out)
							;; | | | dont add set/call/local etc
							;; | | |
							;; | | is int struct (0 - out)
							;; | | dont add globals/endglobals
							;; | |
							;; | is in interface (0 - out)
							;; | not entry to functions/methods
							;; |
							;; is in globals (0 - out)
							;; not add nothing


	push	ebx			;; 00h on stack top
	jmp	_lFNPLine

	_lFNPLineEx:
	movsw

	_lFNPLine:
	mov	eax,			dword ptr [esi]

	cmp	al,			00h
	je	_lFNPEnd

	cmp	ax,			0a0dh
	jne	_lFNPParseStart
	add	esi,			02h
	jmp	_lFNPLine

		;;----------------
		;; global instruction parsing
		_lFNPParseStart:

			;;----------------
			;; nocjass
			cmp	ax,			3901h		;; #9
			jne	_next

			lea	edx,			[esi+02h]
			mov	eax,			dword ptr [_dSynDesc]
			mov	esi,			dword ptr [eax]
			mov	ecx,			dword ptr [eax+04h]
			add	eax,			08h
			mov	dword ptr [_dSynDesc],	eax
			mov	dword ptr [edi],	20212f2fh	;; //!_
			add	edi,			04h
			sub	ecx,			esi
			rep	movsb
			mov	esi,			edx
			jmp	_lFNPLine
			;;----------------

		_lbl:
		test	ebx,			01b
		jp	_next
		mov	edi,			offset _bFuncCodeOneLine

			;;----------------
			;; comment
			_lbl:
			cmp	ax,			2f2fh		;; //
			jne	_next

			cmp	byte ptr [esi+02h],	21h		;; !
			je	_lFNPExCode

			_lFNPCommExs:
			inc	esi
			cmp	word ptr [esi],		0a0dh		;; nl
			jne	_lFNPCommExs
			add	esi,			02h
			mov	eax,			dword ptr [esi]
			jmp	_lFNPParseStart
			;;----------------

		_lbl:
		cmp	al,			0ch
		je	_lFNPVarX

		_lbl:
		test	ebx,			01b
		jp	_lFNPOutside				;; globals ? ;; out the function
		;;----------------

		;;----------------
		;; in function
		cmp	eax,			6c6c6163h	;; call
		jne	_next
		cmp	byte ptr [esi+04h],	20h		;; _
		je	_lFNPCopyParse

		_lbl:
		cmp	eax,			20746573h	;; set_
		je	_lFNPCopyParse

		_lbl:
		cmp	eax,			61636f6ch	;; loca
		jne	_next
		cmp	word ptr [esi+04h],	206ch		;; l_
		jne	_next
		jmp	_lFNVarParse

		_lbl:
		cmp	eax,			74617473h	;; stat
		jne	_next
		cmp	dword ptr [esi+04h],	69206369h	;; ic_i
		jne	_next
		cmp	word ptr [esi+08h],	2066h		;; f_
		je	_lFNPCopyParse

			;;----------------
			;; if
			_lbl:
			cmp	ax,			6669h		;; if
			jne	_next
			cmp	byte ptr [esi+02h],	2eh		;; _ ;; !!!!
			jg	_next

			movsw
			mov	dword ptr [edi],	20202020h	;; bs
			mov	dword ptr [edi+04h],	20202020h	;; bs
			mov	dword ptr [edi+08h],	20202020h	;; bs
			add	edi,			0ch
			mov	byte ptr [_bCodeSys],	al
			mov	byte ptr [_bCodePosOp],	00h

			jmp	_FNPIf

			_lbl:
			cmp	eax,			65736c65h	;; else
			jne	_next
			cmp	word ptr [esi+04h],	0a0dh		;; new line
			je	_lFNPCopyParse
			cmp	word ptr [esi+04h],	7801h		;; #x
			jne	_lFNPIfEX

			mov	ecx,			dword ptr [esi+06h]
			mov	dword ptr [esi+04h],	06060606h
			cmp	word ptr [esi+0ah],	0a0dh		;; new line
			je	_lFNPIfElseFX
			mov	word ptr [esi+08h],	0a0dh		;; new line
			jmp	_lFNPIfBlockEx
			_lFNPIfElseFX:
			mov	word ptr [esi+08h],	0606h		;; new line
			jmp	_lFNPIfBlockEx

			_lFNPIfEX:
			cmp	word ptr [esi+04h],	6669h		;; if
			jne	_next
			cmp	byte ptr [esi+06h],	2ch		;; _ ;; !!!!
			jg	_next

			_FNPIf:
			mov	eax,			esi
			_FNPIfx:
			inc	eax
			cmp	dword ptr [eax],	6e656874h	;; then
			je	_FNPIfs
			cmp	word ptr [eax],		7801h		;; #x
			je	_lFNPIfBlock
			cmp	word ptr [eax],		0a0dh
			jne	_FNPIfx
			mov	word ptr [eax],		3001h		;; #0
			jmp	_lFNPCopyParse
			_FNPIfs:
			cmp	byte ptr [eax+04h],	20h
			je	_lFNPCopyParse
			cmp	word ptr [eax+04h],	0a0dh
			je	_lFNPCopyParse
			jmp	_FNPIfx

			_lFNPIfBlock:
			mov	ecx,			dword ptr [eax+02h]

			mov	dword ptr [eax],	06060606h
			mov	word ptr [eax+04h],	3001h		;; #0

			cmp	dword ptr [ecx+10h],	65736c65h	;; else
			jne	_lFNPIfBlockEx
			cmp	word ptr [ecx+14h],	6669h		;; if
			jne	_lFNPIfBlockSx
			cmp	byte ptr [ecx+16h],	2dh		;; _ ;; !!!
			jbe	_lFNPIfBlockNull
			_lFNPIfBlockSx:
			cmp	byte ptr [ecx+14h],	2dh		;; _ ;; !!!
			jbe	_lFNPIfBlockNull

			_lFNPIfBlockEx:
			mov	dword ptr [ecx],	69646e65h	;; endi
			mov	word ptr [ecx+04h],	0666h		;; f_
			jmp	_lFNPCopyParse

			_lFNPIfBlockNull:
			mov	dword ptr [ecx],	06060606h	;; ex bs
			mov	word ptr [ecx+04h],	0606h
			cmp	word ptr [ecx-02h],	0a0dh		;; new line
			jne	_lFNPCopyParse
			mov	word ptr [ecx-02h],	0606h
			jmp	_lFNPCopyParse

			_lbl:
			cmp	eax,			69646e65h	;; endi
			jne	_next
			cmp	byte ptr [esi+04h],	66h		;; f
			jne	_next
			cmp	byte ptr [esi+05h],	20h		;; _
			jbe	_lFNPCopyParse
			;;----------------

			;;----------------
			;; loop
			_lbl:
			cmp	eax,			706f6f6ch	;; loop
			jne	_next
			cmp	byte ptr [esi+04h],	20h		;; _
			jg	_next
			call	_lFNPCheckBlock
			test	eax,			eax
			jz	_lFNPCopyParse
			mov	dword ptr [eax],	6c646e65h	;; endl
			mov	dword ptr [eax+04h],	06706f6fh	;; oop_
			jmp	_lFNPCopyParse

			_lbl:
			cmp	eax,			6c646e65h	;; endl
			jne	_next
			cmp	word ptr [esi+04h],	6f6fh		;; oo
			jne	_next
			cmp	byte ptr [esi+06h],	70h		;; p
			jne	_next
			cmp	byte ptr [esi+07h],	20h		;; _
			jbe	_lFNPCopyParse

			_lbl:
			cmp	eax,			74697865h	;; exit
			jne	_next
			cmp	dword ptr [esi+04h],	6e656877h	;; when
			jne	_next
			cmp	byte ptr [esi+08h],	2dh		;; _ ;; !!!!
			jg	_next

			movsd
			movsd
			mov	dword ptr [edi],	20202020h
			mov	word ptr [edi+04h],	2020h
			add	edi,			06h

			mov	byte ptr [_bCodeSys],	al
			mov	byte ptr [_bCodePosOp],	00h

			jmp	_lFNPCopyParse

				;;----------------
				;; whilenot
				_lbl:
				cmp	eax,			6c696877h	;; whil
				jne	_next
				cmp	dword ptr [esi+04h],	746f6e65h	;; enot
				jne	_next
				cmp	byte ptr [esi+08h],	2dh		;; _ ;; !!!!
				jg	_next
				call	_lFNPCheckBlock
				add	esi,			08h
				test	eax,			eax
				jz	_lFNPWhileEX
				mov	dword ptr [eax],	6c646e65h	;; endl
				mov	dword ptr [eax+04h],	06706f6fh	;; oop_
				_lFNPWhileEX:
				mov	eax,			dword ptr [_dFCB]
				mov	dword ptr [eax],	706f6f6ch	;; loop
				mov	word ptr [eax+04h],	0a0dh		;; new line
				add	eax,			06h
				mov	dword ptr [_dFCB],	eax
				cmp	byte ptr [esi],		10h		;; 0a0dh or #
				jbe	_lFNPCopyParse
				mov	dword ptr [edi],	74697865h	;; exit
				mov	dword ptr [edi+04h],	6e656877h	;; when
				mov	byte ptr [edi+08h],	20h		;; _
				add	edi,			09h
				jmp	_lFNPCopyParse

				_lbl:
				cmp	eax,			77646e65h	;; endw
				jne	_next
				cmp	dword ptr [esi+04h],	656c6968h	;; hile
				jne	_next
				cmp	dword ptr [esi+08h],	0d746f6eh	;; not_
				jne	_next
				add	esi,			0bh
				mov	dword ptr [edi],	6c646e65h	;; endl
				mov	dword ptr [edi+04h],	00706f6fh	;; oop_
				add	edi,			07h
				jmp	_lFNPCopyParse
				;;----------------

				;;----------------
				;; do
				_lbl:
				cmp	ax,			6f64h		;; do
				jne	_next
				cmp	byte ptr [esi+02h],	21h
				jg	_next

				mov	dword ptr [edi],	706f6f6ch	;; loop
				mov	word ptr [edi+04h],	0a0dh		;; new line
				add	edi,			06h

				call	_lFNPCheckBlock
				add	esi,			02h
				test	eax,			eax
				jz	_lFNPCopyParse

				cmp	dword ptr [eax+10h],	6c696877h	;; whil
				jne	_lFNPDo
				cmp	dword ptr [eax+14h],	746f6e65h	;; enot
				jne	_lFNPDo
				cmp	byte ptr [eax+18h],	2dh		;; _ ;; !!!!
				jg	_lFNPDo

				mov	dword ptr [eax],	06060606h
				mov	word ptr [eax+04h],	0606h
				mov	dword ptr [eax+0eh],	78650a0dh	;; __ex
				mov	dword ptr [eax+12h],	68777469h	;; itwh
				mov	word ptr [eax+16h],	6e65h		;; en

				add	eax,			11h
				_lFNPDoEX:
				inc	eax
				cmp	word ptr [eax],		0a0dh		;; nl
				jne	_lFNPDoEX
				mov	word ptr [eax],		6601h		;; #f
				jmp	_lFNPCopyParse

				_lFNPDo:
				mov	dword ptr [eax],	6c646e65h	;; endl
				mov	dword ptr [eax+04h],	06706f6fh	;; oop_
				jmp	_lFNPCopyParse

				_lbl:
				cmp	eax,			64646e65h	;; endd
				jne	_next
				cmp	word ptr [esi+04h],	206fh		;; o_
				jne	_next
				cmp	dword ptr [esi+06h],	6c696877h	;; whil
				jne	_next
				cmp	dword ptr [esi+0ah],	746f6e65h	;; enot
				jne	_next
				cmp	byte ptr [esi+0eh],	2dh		;; _ ;; !!!!
				jg	_next

				mov	dword ptr [esi],	74697865h	;; exit
				mov	dword ptr [esi+04h],	6e656877h	;; when
				mov	dword ptr [esi+08h],	06060606h
				mov	word ptr [esi+0ch],	06060606h
				mov	eax,			esi
				jmp	_lFNPDoEX
				;;----------------
			;;----------------

		_lbl:
		cmp	eax,			75746572h	;; retu
		jne	_next
		cmp	word ptr [esi+04h],	6e72h		;; rn
		jne	_next
		cmp	byte ptr [esi+06h],	2eh		;; _ ;; !!!!	
		jg	_next

		movsd
		movsw
		mov	dword ptr [edi],	20202020h	;; bs
		mov	dword ptr [edi+04h],	20202020h	;; bs
		add	edi,			08h
		mov	byte ptr [_bCodeSys],	al
		mov	byte ptr [_bCodePosOp],	00h
	
		jmp	_lFNPCopyParse

			;;----------------
			;; ++ -- pre
			_lbl:
			cmp	ax,			2b2bh		;; ++
			jne	_next

				;;----------------
				_lFNPIDPreX:
				mov	ecx,			edi
				mov	edx,			esi

				mov	edi,			esi
				lea	esi,			[esi+02h]

				_lFNPIDPreXfx:
				movsb
				cmp	word ptr [esi],		0a0dh	;; nl
				jne	_lFNPIDPreXfx

				mov	word ptr [edi],		ax
				add	edi,			02h

				mov	edi,			ecx
				mov	esi,			edx
				jmp	_lFNPFuncScanIn
				;;----------------

			_lbl:
			cmp	ax,			2d2dh		;; --
			je	_lFNPIDPreX
			;;----------------

		_lbl:
		cmp	eax,			66646e65h	;; endf
		jne	_next
		cmp	dword ptr [esi+04h],	74636e75h	;; unct
		jne	_next
		cmp	word ptr [esi+08h],	6f69h		;; io
		jne	_next
		cmp	byte ptr [esi+0ah],	6eh		;; n
		jne	_next
		cmp	byte ptr [esi+0bh],	20h		;; _	
		jg	_next

			;;----------------
			;; function out
			_lFNPFuncOut:
			mov	edi,			dword ptr [_dBCP]

				;;----------------
				;; sys locals
					;;----------------
					;; type
					cmp	byte ptr [_bTempType],	00h
					je	_lFNPFuncOutAX

					mov	ecx,			edi
					_lFNPFuncOutBX:
					dec	ecx
					cmp	byte ptr [ecx],		20h
					jne	_lFNPFuncOutBX

					mov	dword ptr [edi],	61636f6ch	;; loca
					mov	word ptr [edi+04h],	206ch		;; l_
					add	edi,			06h

					_lFNPFuncOutCX:
					inc	ecx
					mov	al,			byte ptr [ecx]
					cmp	al,			0dh		;; nl
					je	_lFNPFuncOutDX
					stosb
					jmp	_lFNPFuncOutCX
					_lFNPFuncOutDX:
					mov	dword ptr [edi],	5f6a6320h	;; _cj_
					mov	dword ptr [edi+04h],	36363676h	;; v666
					mov	dword ptr [edi+08h],	0a0d725fh	;; _r__
					add	edi,			0ch
					mov	byte ptr [_bTempType],	00h
					;;----------------

					;;----------------
					;; bool
					_lFNPFuncOutAX:
					cmp	byte ptr [_bTempBool],	00h
					je	_lFNPFuncOutTT

					mov	dword ptr [edi],	61636f6ch	;; loca
					mov	dword ptr [edi+04h],	6f62206ch	;; l bo
					mov	dword ptr [edi+08h],	61656c6fh	;; olea
					mov	dword ptr [edi+0ch],	6a63206eh	;; n cj
					mov	dword ptr [edi+10h],	3636765fh	;; _v66
					mov	dword ptr [edi+14h],	0a625f36h	;; 6_b_
					mov	byte ptr [edi+18h],	0dh		;; _
					add	edi,			19h
					mov	byte ptr [_bTempBool],	00h
					;;----------------

					;;----------------
					;; index
					_lFNPFuncOutTT:
					mov	al,			byte ptr [_bFCLLMAX]
					mov	byte ptr [_bFCLLMAX],	40h
					_lFNPFuncOutIS:
					cmp	al,			40h
					je	_lFNPFuncOutEX

					mov	dword ptr [edi],	61636f6ch	;; loca
					mov	dword ptr [edi+04h],	6e69206ch	;; l_in
					mov	dword ptr [edi+08h],	65676574h	;; tege
					mov	dword ptr [edi+0ch],	78712072h	;; r_qx
					mov	byte ptr [edi+10h],	al
					mov	word ptr [edi+11h],	0a0dh		;; nl
					add	edi,			13h
					dec	al
					jmp	_lFNPFuncOutIS
					;;----------------
				;;----------------

			_lFNPFuncOutEX:
			mov	eax,			esi
			mov	esi,			offset _bFuncCodeLocals
			mov	ecx,			dword ptr [_dFCL]
			sub	ecx,			offset _bFuncCodeLocals

			rep	movsb

			mov	esi,			offset _bFuncCodeBase
			mov	ecx,			dword ptr [_dFCB]
			sub	ecx,			offset _bFuncCodeBase

			rep	movsb

			xor	ebx,			01b
			mov	esi,			eax
			jmp	_lFNPCopy
			;;----------------

		_lbl:
		cmp	eax,			6d646e65h	;; endm
		jne	_next
		cmp	dword ptr [esi+04h],	6f687465h	;; etho
		jne	_next
		cmp	byte ptr [esi+08h],	64h		;; d
		jne	_next
		cmp	byte ptr [esi+09h],	20h		
		jg	_next
		jmp	_lFNPFuncOut

			;;----------------
			;; in function scan
			_lbl:
			_lFNPFuncScanIn:
			mov	ecx,			esi	;; temp script posiyion
			xor	ebp,			ebp	;; temp flags

			_lFNPInFuncScan:
			inc	ecx
			mov	eax,			dword ptr [ecx]

			cmp	al,			22h	;; "
			jne	_next
			_lFNPInFuncScanString:
			inc	ecx
			cmp	byte ptr [ecx],		5ch	;; \ 
			jne	_lFNPInFuncScanStringEX
			add	ecx,			02h
			_lFNPInFuncScanStringEX:
			cmp	byte ptr [ecx],		22h	;; "
			jne	_lFNPInFuncScanString
			jmp	_lFNPInFuncScan

			_lbl:
			cmp	ax,			0a0dh
			jne	_next
			cmp	ebp,			0100b		;; adding call
			jb	_lFNPCopyParse
			mov	dword ptr [edi],	6c6c6163h	;; call
			mov	byte ptr [edi+04h],	20h		;; _
			add	edi,			05h
			jmp	_lFNPCopyParse

			_lbl:
			cmp	al,			28h	;; (
			jne	_next
			or	ebp,			0100b	;; call pre
			jmp	_lFNPInFuncScan

			_lbl:
			cmp	al,	 		3dh	;; =
			jne	_next
			cmp	ax,			3d3dh	;; ==
			jne	_lFNPSetEX
			inc	ecx
			jmp	_lFNPInFuncScan

			_lFNPSetEX:
			cmp	ax,			3d3dh	;; set pre
			je	_lFNPInFuncScan
			cmp	byte ptr [ecx-01h],	3ch
			je	_lFNPInFuncScan
			cmp	byte ptr [ecx-01h],	3eh
			je	_lFNPInFuncScan
			cmp	byte ptr [ecx-01h],	21h
			je	_lFNPInFuncScan
			_lFNPSet:
			mov	dword ptr [edi],	20746573h	;; set_
			add	edi,			04h
			jmp	_lFNPCopyParse

			_lbl:
			cmp	eax,	 		0a0d2b2bh	;; ++
			jne	_next
			mov	word ptr [ecx],		7001h		;; #p
			jmp	_lFNPSet
			_lbl:
			cmp	eax,	 		06062b2bh	;; ++
			jne	_next
			mov	word ptr [ecx],		7001h		;; #p
			jmp	_lFNPSet

			_lbl:
			cmp	eax,	 		0a0d2d2dh	;; --
			jne	_next
			mov	word ptr [ecx],		6d01h		;; #m
			jmp	_lFNPSet
			_lbl:
			cmp	eax,	 		06062d2dh	;; --
			jne	_next
			mov	word ptr [ecx],		6d01h		;; #m
			jmp	_lFNPSet

			_lbl:
			cmp	al,			20h
			jne	_next
			or	ebp,			01b	;; local pre
			jmp	_lFNPInFuncScan

			_lbl:
			cmp	al,			5bh
			jne	_next
			or	ebp,			10b	;; local not
			jmp	_lFNPInFuncScan

			_lbl:
			cmp	ebp,			01h
			jne	_lFNPInFuncScan
			cmp	al,			41h
			jb	_lFNPInFuncScan
			cmp	al,			5ah
			jbe	_lFNPLocal
			cmp	al,			61h
			jb	_lFNPInFuncScan
			cmp	al,			7ah
			jg	_lFNPInFuncScan

			_lFNPLocal:
			mov	dword ptr [edi],	61636f6ch	;; loca
			mov	dword ptr [edi+04h],	206ch		;; l_
			add	edi,			06h
			jmp	_lFNVarParse
			;;----------------
		;;----------------

		;;----------------
		;; ex code parsing
		_lFNPExCode:
		mov	dword ptr [edi],	20212f2fh	;; //!_
		add	esi,			03h
		add	edi,			04h
		mov	eax,			dword ptr [esi]

		cmp	ax,			0a0dh
		je	_lFNPCopy

		cmp	eax,			656a6e69h	;; inje
		jne	_next
		cmp	word ptr [esi+04h],	7463h		;; ct
		jne	_next
		cmp	byte ptr [esi+06h],	20h		;; _
		jg	_next
		lea	edx,			[esi+06h]
		_lFNPInjectLoop:
		inc	edx
		mov	eax,			dword ptr [edx]
		cmp	ax,			0a0dh
		je	_lFNPCopy	
		cmp	eax,			6e69616dh	;; main
		jne	_lFNPInjectLoopEX
		cmp	byte ptr [edx+04h],	20h		;; _
		jg	_lFNPInjectLoopEX
		jmp	_lFNPCopyFuncIn
		_lFNPInjectLoopEX:
		cmp	eax,			666e6f63h	;; conf
		jne	_lFNPInjectLoop
		cmp	word ptr [edx+04h],	6769h		;; if
		jne	_lFNPInjectLoop
		cmp	byte ptr [edx+06h],	20h		;; _
		jg	_lFNPInjectLoop
		jmp	_lFNPCopyFuncIn

;;		_lbl:
;;		cmp	eax,			6f706d69h	;; impo
;;		jne	_next
;;		cmp	word ptr [esi+04h],	7472h		;; rt
;;		jne	_next
;;		movsd
;;		movsw
;;		mov	byte ptr [edi],		20h		;; bs
;;		inc	edi
;;		jmp	_lFNPCopy

		_lbl:
		cmp	eax,			69646e65h	;; endi
		jne	_lFNPCopy
		cmp	dword ptr [esi+04h],	63656a6eh	;; njec
		jne	_lFNPCopy
		cmp	byte ptr [esi+08h],	74h		;; t
		jne	_lFNPCopy
		cmp	byte ptr [esi+09h],	20h		;; _
		jg	_lFNPCopy

		mov	eax,			dword ptr [_dFCB]
		mov	dword ptr [eax],	20212f2fh	;; //!_
		add	eax,			04h
		mov	dword ptr [_dFCB],	eax

		jmp	_lFNPFuncOut
		;;----------------

		;;----------------
		;; outside the function
		_lFNPOutside:
		cmp	eax,			65736c65h	;; else
		jne	_next
		cmp	byte ptr [esi+04h],	20h		;; bs
		jb	_lFNPCopy

		_lbl:
		cmp	eax,			69646e65h	;; endi
		jne	_next
		cmp	byte ptr [esi+04h],	66h		;; f
		jne	_next
		cmp	byte ptr [esi+05h],	20h		;; bs
		jb	_lFNPCopy

		_lbl:
		cmp	eax,			74617473h	;; stat
		jne	_lFNPGlob
		cmp	dword ptr [esi+04h],	69206369h	;; ic_i
		jne	_lFNPGlob
		cmp	word ptr [esi+08h],	2066h		;; f_
		je	_lFNPCopy
		;;----------------

		;;----------------
		;; in globals
		_lFNPGlob:
		cmp	ebx,			1000b
		jb	_lFNPInCode

		cmp	eax,			67646e65h	;; endg
		jne	_lFNVarParse
		cmp	dword ptr [esi+04h],	61626f6ch	;; loba
		jne	_lFNVarParse
		cmp	word ptr [esi+08h],	736ch		;; ls
		jne	_lFNVarParse
		cmp	byte ptr [esi+0ah],	20h		;; _
		jg	_lFNVarParse
		xor	ebx,			1000b
		jmp	_lFNPCopy
		;;----------------

		;;----------------
		;; in code
		_lFNPInCode:
		cmp	eax,			626f6c67h	;; glob
		jne	_next
		cmp	dword ptr [esi+03h],	736c6162h	;; bals
		jne	_next
		cmp	byte ptr [esi+07h],	20h		;; _
		jg	_next
		or	ebx,			1000b
		jmp	_lFNPCopy

		_lbl:
		cmp	eax,			73646e65h	;; ends
		jne	_next
		cmp	dword ptr [esi+04h],	63757274h	;; truc
		jne	_next
		cmp	byte ptr [esi+08h],	74h		;; t
		jne	_next
		cmp	byte ptr [esi+09h],	20h		;; _
		jg	_next
		xor	ebx,			10b
		jmp	_lFNPCopy

		_lbl:
		cmp	eax,			6d646e65h	;; endm
		jne	_next
		cmp	dword ptr [esi+04h],	6c75646fh	;; odul
		jne	_next
		cmp	byte ptr [esi+08h],	65h		;; e
		jne	_next
		cmp	byte ptr [esi+09h],	20h		;; _
		jg	_next
		xor	ebx,			10b
		jmp	_lFNPCopy

		_lbl:
		cmp	eax,			69646e65h	;; endi
		jne	_next
		cmp	dword ptr [esi+04h],	7265746eh	;; nter
		jne	_next
		cmp	dword ptr [esi+08h],	65636166h	;; face
		jne	_next
		cmp	byte ptr [esi+0ch],	20h		;; _
		jg	_next
		xor	ebx,			0100b
		jmp	_lFNPCopy

		_lbl:
		test	ebx,			ebx
		jz	_lFNPOutStruct

			;;----------------
			;; in the struct
			cmp	eax,			656c6564h	;; dele
			jne	_next
			cmp	dword ptr [esi+04h],	65746167h	;; gate
			jne	_next
			cmp	byte ptr [esi+08h],	20h		;; _
			jbe	_lFNPCopy

			_lbl:
			cmp	eax,			6c706d69h	;; impl
			jne	_next
			cmp	dword ptr [esi+04h],	6e656d65h	;; emen
			jne	_next
			cmp	word ptr [esi+06h],	2074h		;; t_
			je	_lFNPCopy

			_lbl:
			lea	ecx,			[esi-01h]
			_lFNPMSStart:
			inc	ecx
			mov	eax,			dword ptr [ecx]

			cmp	al,			3dh		;; =
			je	_lFNVarParse

			cmp	al,			28h		;; (
			je	_lFNPExFuncDef

			cmp	ax,			0a0dh
			je	_lFNVarParse

			cmp	eax,			7265706fh	;; oper
			jne	_lFNPMSStartEX
			cmp	dword ptr [ecx+04h],	726f7461h	;; ator
			jne	_lFNPMSStartEX
			cmp	byte ptr [ecx+08h],	3ch		;; <
			je	_lFNPExFuncDefEX
			cmp	byte ptr [ecx+08h],	3dh		;; =
			je	_lFNPExFuncDefEX
;;			cmp	byte ptr [ecx+08h],	3eh		;; >
;;			je	_lFNPExFuncDef
			cmp	byte ptr [ecx+08h],	21h		;; !
			je	_lFNPExFuncDefEX
			cmp	byte ptr [ecx+08h],	5bh		;; [ 
			je	_lFNPExFuncDefEX
			cmp	byte ptr [ecx+08h],	20h		;; bs
			je	_lFNPExFuncDefEX

			_lFNPMSStartEX:
			cmp	eax,			6874656dh	;; meth
			jne	_lFNPMSStart
			cmp	word ptr [ecx+04h],	646fh		;; od
			jne	_lFNPMSStart
			cmp	byte ptr [ecx+06h],	20h		;; _
			jg	_lFNPMSStart

			lea	eax,				[ecx+09h]
			mov	dword ptr [_dLastFuncName],	eax

			jmp	_lFNPCopyFuncIn
			;;----------------

			;;----------------
			;; out the struct
			_lFNPOutStruct:
			cmp	eax,			7262696ch	;; libr
			jne	_next
			cmp	dword ptr [esi+03h],	79726172h	;; rary
			jne	_next
			cmp	dword ptr [esi+07h],	636e6f5fh	;; _onc
			jne	_lFNPLibTestEX
			cmp	byte ptr [esi+0bh],	65h		;; e
			jne	_lFNPLibTestEX
			cmp	byte ptr [esi+0ch],	20h		;; _
			jg	_next
			_lFNPLibBlockEX:
			call	_lFNPCheckBlock
			test	eax,			eax
			jz	_lFNPCopy
			mov	dword ptr [eax],	6c646e65h	;; endl
			mov	dword ptr [eax+04h],	61726269h	;; ibra
			mov	word ptr [eax+08h],	7972h		;; ry
			jmp	_lFNPCopy
			_lFNPLibTestEX:
			cmp	byte ptr [esi+07h],	20h		;; _
			jbe	_lFNPLibBlockEX

			_lbl:
			cmp	eax, 			706f6373h	;; scop
			jne	_next
			cmp	byte  ptr [esi+04h],	65h		;; e
			jne	_next
			cmp	byte ptr [esi+05h],	20h		;; _
			jg	_next
			call	_lFNPCheckBlock
			test	eax,			eax
			jz	_lFNPCopy
			mov	dword ptr [eax],	73646e65h	;; ends
			mov	dword ptr [eax+04h],	65706f63h	;; cope
			jmp	_lFNPCopy

			_lbl:
			cmp	eax,			6c646e65h	;; endl
			jne	_next
			cmp	dword ptr [esi+04h],	61726269h	;; ibra
			jne	_next
			cmp	word ptr [esi+08h],	7972h		;; ry
			jne	_next
			cmp	byte ptr [esi+0ah],	20h		;; _
			jbe	_lFNPCopy

			_lbl:
			cmp	eax,			73646e65h	;; ends
			jne	_next
			cmp	dword ptr [esi+04h],	65706f63h	;; cope
			jne	_next
			cmp	byte ptr [esi+08h],	20h		;; _
			jbe	_lFNPCopy

				;;----------------
				;; public/private loop
				_lbl:
				lea	ecx,			[esi-01h]
				_lFNPInCodeScan:
				inc	ecx
				_lFNPInCodeScanEX:
				mov	eax,			dword ptr [ecx]

				cmp	eax,			76697270h	;; priv
				jne	_next
				cmp	dword ptr [ecx+04h],	20657461h	;; ate_
				jne	_next
				add	ecx,			08h
				jmp	_lFNPInCodeScanEX

				_lbl:
				cmp	eax,			6c627570h	;; publ
				jne	_next
				_lFNPScanIc:
				cmp	word ptr [ecx+04h],	6369h		;; ic
				jne	_next
				cmp	byte ptr [ecx+06h],	20h		;; _
				jne	_next
				add	ecx,			07h
				jmp	_lFNPInCodeScanEX

				_lbl:
				cmp	eax,			736e6f63h	;; cons
				jne	_next
				cmp	dword ptr [ecx+04h],	746e6174h	;; tant
				jne	_next
				cmp	byte ptr [ecx+08h],	20h		;; _
				jne	_next
				add	ecx,			09h
				jmp	_lFNPInCodeScanEX

				_lbl:
				cmp	eax,			74617473h	;; stat
				je	_lFNPScanIc

					;;----------------
					;; function, native function and function interface
					_lbl:
					cmp	eax,			6974616eh	;; nati
					jne	_next
					cmp	word ptr [ecx+04h],	6576h		;; ve
					jne	_next
					cmp	byte ptr [ecx+06h],	20h		;; bs
					jne	_next

					_lXFPNative:
					inc	ecx
					cmp	word ptr [ecx],		0a0dh		;; nl
					je	_lFNPCopy
					cmp	byte ptr [ecx],		28h		;; (
					jne	_lXFPNative
					jmp	_lFNPExFuncDef

					_lbl:
					cmp	eax,			636e7566h	;; func
					jne	_next
					cmp	dword ptr [ecx+04h],	6e6f6974h	;; tion
					jne	_next
					cmp	byte ptr [ecx+08h],	20h		;; _
					jne	_next
					cmp	ebx,			0100b
					jge	_lFNPCopy

					lea	eax,				[ecx+09h]
					mov	dword ptr [_dLastFuncName],	eax

					cmp	dword ptr [ecx+09h],	65746e69h	;; inte
					jne	_lFNPCopyFuncIn
					cmp	dword ptr [ecx+0dh],	63616672h	;; rfac
					jne	_lFNPCopyFuncIn
					cmp	word ptr [ecx+11h],	2065h		;; e_
					je	_lFNPCopy
					;;----------------

				_lbl:
				cmp	eax,			75727473h	;; stru
				jne	_next
				cmp	word ptr [ecx+04h],	7463h		;; ct
				jne	_next
				cmp	byte ptr [ecx+06h],	20h		;; _
				jg	_next
				or	ebx,			10b
				call	_lFNPCheckBlock
				test	eax,			eax
				jz	_lFNPCopy
				mov	dword ptr [eax],	73646e65h	;; ends
				mov	dword ptr [eax+04h],	63757274h	;; truc
				mov	byte ptr [eax+08h],	74h		;; t
				jmp	_lFNPCopy

				_lbl:
				cmp	eax,			75646f6dh	;; modu
				jne	_next
				cmp	word ptr [ecx+04h],	656ch		;; le
				jne	_next
				cmp	byte ptr [ecx+06h],	20h		;; _
				jg	_next
				or	ebx,			10b
				call	_lFNPCheckBlock
				test	eax,			eax
				jz	_lFNPCopy
				mov	dword ptr [eax],	6d646e65h	;; endm
				mov	dword ptr [eax+04h],	6c75646fh	;; odul
				mov	byte ptr [eax+08h],	65h		;; e
				jmp	_lFNPCopy

				_lbl:
				cmp	eax,			65746e69h	;; inte
				jne	_next
				cmp	dword ptr [ecx+04h],	63616672h	;; rfac
				jne	_next
				cmp	byte ptr [ecx+08h],	65h		;; e
				jne	_next
				cmp	byte ptr [ecx+09h],	20h		;; _
				jg	_next
				or	ebx,			0100b
				call	_lFNPCheckBlock
				test	eax,			eax
				jz	_lFNPCopy
				mov	dword ptr [eax],	69646e65h	;; endi
				mov	dword ptr [eax+04h],	7265746eh	;; nter
				mov	dword ptr [eax+08h],	65636166h	;; face
				jmp	_lFNPCopy

				_lbl:
				cmp	eax,			7779656bh	;; keyw
				jne	_next
				cmp	dword ptr [ecx+03h],	64726f77h	;; word
				jne	_next
				cmp	byte ptr [ecx+07h],	20h		;; _
				jbe	_lFNPCopy

				_lbl:
				cmp	eax,			6b6f6f68h	;; hook
				jne	_next
				cmp	byte ptr [esi+04h],	20h		;; bs
				je	_lFNPCopy

				_lbl:
				cmp	eax,			65707974h	;; type
				jne	_lFNPGlobalsScan
				cmp	byte ptr [ecx+04h],	20h		;; _
				jbe	_lFNPCopy

				_lFNPGlobalsScanEx:
				inc	ecx
				mov	eax,			dword ptr [ecx]
				_lFNPGlobalsScan:
				cmp	ax,			0a0dh
				je	_lFNPAddGlobals
				cmp	al,			3dh		;; =
				je	_lFNPAddGlobalsEx

				cmp	al,			28h		;; (
				jne	_lFNPGlobalsScanEx
				cmp	word ptr [ecx-02h],	0a0dh		;; nl
				jne	_lFNPExFuncDef

				jmp	_lFNPGlobalsScanEx

				_lFNPAddGlobalsEx:
				inc	ecx
				cmp	word ptr [ecx],		0a0dh
				jne	_lFNPAddGlobalsEx
				_lFNPAddGlobals:
				mov	word ptr [ecx],		3301h		;; #3
				mov	dword ptr [edi],	626f6c67h	;; glob
				mov	dword ptr [edi+04h],	0d736c61h	;; als_
				mov	byte ptr [edi+08h],	0ah		;; _
				add	edi,			09h
				jmp	_lFNVarParse
				;;----------------
			;;----------------
		;;----------------

		;;----------------
		;; ex function define
		_lFNPExFuncDefEX:		;; from overloading operators
		inc	ecx
		cmp	byte ptr [ecx],		28h		;; ( 
		jne	_lFNPExFuncDefEX

		_lFNPExFuncDef:		
		dec	ecx			;; ecx = func name
		cmp	word ptr [ecx-02h],	0a0dh		;; nl
		je	_lFNPCopy
		cmp	byte ptr [ecx-01h],	20h		;; _
		jne	_lFNPExFuncDef

		;; is operator?
		cmp	dword ptr [ecx-05h],	726f7461h	;; ator
		jne	_next
		cmp	dword ptr [ecx-09h],	7265706fh	;; oper
		jne	_next
		sub	ecx,			09h

		_lbl:
		mov	edx,			ecx
		_lbl:				;; edx = type
		dec	edx
		cmp	word ptr [edx-02h],	0a0dh		;; new line
		je	_lFNPExFuncDefDX
		cmp	byte ptr [edx-01h],	20h		;; _
		jne	_prew

		_lbl:				;; copy func params
		movsb
		_lFNPExFuncDefDX:
		cmp	esi,			edx
		jne	_prew

			;;----------------
			;; add func or method
			cmp	dword ptr [edi-07h],	6974616eh	;; nati
			jne	_lFNPExFuncDefTTTF
			cmp	word ptr [edi-03h],	6576h		;; ve
			jne	_lFNPExFuncDefTTTF
			cmp	byte ptr [edi-01h],	20h		;; bs
			je	_next

			_lFNPExFuncDefTTTF:
			cmp	ebx,			10b
			jb	_lFNPExFuncDefFF

			mov	dword ptr [edi],	6874656dh	;; meth
			mov	dword ptr [edi+04h],	0020646fh	;; od_
			add	edi,			07h
			jmp	_next
			_lFNPExFuncDefFF:
			mov	dword ptr [edi],	636e7566h	;; func
			mov	dword ptr [edi+04h],	6e6f6974h	;; tion
			mov	byte ptr [edi+08h],	20h		;; _
			add	edi,			09h
			;;----------------

		_lbl:
		mov	esi,			ecx	;; copy func name

		mov	dword ptr [_dLastFuncName],	ecx

		_lbl:
		movsb
		cmp	byte ptr [esi],		28h		;; (
		jne	_prew

		_lbl:				;; adding takes
		mov	dword ptr [edi],	6b617420h	;; _tak
		mov	dword ptr [edi+04h],	00207365h	;; es_
		add	edi,			07h

		cmp	word ptr [esi],		2928h		;; ()
		jne	_next

		mov	dword ptr [edi],	68746f6eh	;; noth
		mov	dword ptr [edi+04h],	00676e69h	;; ing
		add	edi,			07h
		inc	esi
		jmp	_lFNPExFuncDefSX

		_lbl:
		inc	esi
		_lbl:				;; copy arguments
		movsb
		cmp	byte ptr [esi],		29h		;; )
		jne	_prew

		_lFNPExFuncDefSX:		;; add returns
		inc	esi
		mov	dword ptr [edi],	74657220h	;; _ret
		mov	dword ptr [edi+04h],	736e7275h	;; urns
		mov	byte ptr [edi+08h],	20h		;; _
		add	edi,			09h

		_lbl:				;; copy func type
		mov	al,			byte ptr [edx]
		cmp	al,			20h
		je	_next
		mov	byte ptr [edi],		al
		inc	edx
		inc	edi
		jmp	_prew

		_lbl:				;; func in?
		cmp	ebx,			0100b
		jge	_lFNPExFuncDefUX

		;; add endfunction
		cmp	word ptr [esi],		7801h		;; #x
		je	_lFNPExFuncDefEnd			;; ???

		_lFNPExFuncDefUX:
		mov	word ptr [edi],		0a0dh		;; nl
		add	edi,			02h
		jmp	_lFNPLine

		_lFNPExFuncDefEnd:
		mov	eax,			dword ptr [esi+02h]
		add	esi,			06h
		mov	word ptr [edi],		0a0dh		;; new line
		add	edi,			02h

			;;----------------
			;; add endfunc or endmethod
			cmp	word ptr [eax-02h],	0a0dh		;; new line
			je	_next
			mov	word ptr [eax],		0a0dh		;; new line
			add	eax,			02h
			_lbl:
			cmp	ebx,			10b
			jb	_lFNPExFuncDefEF
			mov	dword ptr [eax],	6d646e65h	;; endm
			mov	dword ptr [eax+04h],	6f687465h	;; etho
			mov	byte ptr [eax+08h],	64h		;; d
			jmp	_next
			_lFNPExFuncDefEF:
			mov	dword ptr [eax],	66646e65h	;; endf
			mov	dword ptr [eax+04h],	74636e75h	;; unct
			mov	dword ptr [eax+08h],	066e6f69h	;; ion
			;;----------------

		_lbl:
		mov	dword ptr [_dBCP],	edi	;; system in
		or	ebx,			01h

		mov	dword ptr [_dFCL],	offset _bFuncCodeLocals
		mov	dword ptr [_dFCB],	offset _bFuncCodeBase

		jmp	_lFNPLine
		;;----------------

		;;----------------
		;; parse comma in variables declaration
		_lFNVarParse:
		lea	ecx,			[esi-01h]
		xor	edx,			edx

		_lFNVarParseEx:
		inc	ecx
		mov	eax,			dword ptr [ecx]

		cmp	al,			22h	;; "
		jne	_next
		_lFNVarParseString:
		inc	ecx
		cmp	byte ptr [ecx],		5ch	;; \ 
		jne	_lFNVarParseStringEX
		add	ecx,			02h
		_lFNVarParseStringEX:
		cmp	byte ptr [ecx],		22h	;; "
		jne	_lFNVarParseString
		jmp	_lFNVarParseEx

		_lbl:
		cmp	al,			28h	;; (
		jne	_next
		inc	edx
		jmp	_lFNVarParseEx

		_lbl:
		cmp	al,			29h	;; )
		jne	_next
		dec	edx
		jmp	_lFNVarParseEx

		_lbl:
		cmp	al,			2ch	;; ,
		jne	_next
		test	edx,			edx
		jnz	_lFNVarParseEx
		mov	byte ptr [ecx],		0eh	;; ;
		jmp	_lFNVarParseEx

		_lbl:
		cmp	ax,			3301h	;; #3
		je	_lFNPCopyParse
		cmp	ax,			0a0dh
		jne	_lFNVarParseEx

		jmp	_lFNPCopyParse
		;;----------------

		;;----------------
		;; check blocks
		_lFNPCheckBlock:
		mov	ecx,			esi
		_lFNPCheckBlockEx:
		inc	ecx
		cmp	word ptr [ecx],		0a0dh
		je	_lFNPCheckBlockSx
		cmp	word ptr [ecx],		7801h		;; #x
		jne	_lFNPCheckBlockEx

		mov	eax,			dword ptr [ecx+02h]
		mov	dword ptr [ecx],	06060606h
		mov	word ptr [ecx+04h],	0a0dh

		cmp	word ptr [eax-02h],	0a0dh
		je	_lFNPCheckBlockRet
		mov	word ptr [eax],		0a0dh
		add	eax,			02h
		_lFNPCheckBlockRet:
		retn

		_lFNPCheckBlockSx:
		xor	eax,			eax
		retn
		;;----------------

		;;----------------
		;; copy line (function in)
		_lFNPCopyFuncIn:
		movsb
		_lFNPCopyFuncInEx:
		mov	eax,			dword ptr [esi]
;;		cmp	al,			00h
;;		je	_lFNPEnd
		cmp	ax,			7801h	;; #x
		je	_lFNPUnBlockErr

		cmp	al,			3ch	;; < = >
		jb	_lFNPCopyFuncInSx
		cmp	al,			3eh	;; >
		jg	_lFNPCopyFuncInDx

		_lFNPCopyFuncInOx:
		cmp	byte ptr [esi+01h],	3dh	;; = ;; ==
		je	_lFNPCopyFuncIn
		mov	ah,			20h	;; _
		mov	word ptr [edi],		ax
		add	edi,			02h
		inc	esi
		jmp	_lFNPCopyFuncInEx
		
		_lFNPCopyFuncInDx:
		cmp	al,			5dh	;; ] 
		jne	_lFNPCopyFuncInSx
		cmp	ax,			3d5dh	;; ]=
		je	_lFNPCopyFuncIn
		jmp	_lFNPCopyFuncInOx

		_lFNPCopyFuncInSx:
		cmp	ax,			0a0dh
		jne	_next

		movsw

			;;----------------
			;; interfaces ?
			cmp	ebx,			0100b
			je	_lFNPLine			
			;;----------------

		mov	dword ptr [_dBCP],	edi
		or	ebx,			01h

		mov	dword ptr [_dFCL],	offset _bFuncCodeLocals
		mov	dword ptr [_dFCB],	offset _bFuncCodeBase

		jmp	_lFNPLine

		_lbl:
		cmp	al,			06h	;; ex _
		jne	_lFNPCopyFuncIn
		inc	esi
		jmp	_lFNPCopyFuncInEx
		;;----------------

		;;----------------
		;; unknow block
		_lFNPUnBlockErr:
		mov	dword ptr [_xErrorTable],	offset _sErr_UnknowBlock
		mov	dword ptr [_xErrorTable+04h],	edi
		mov	dword ptr [edi],		007b0a0dh	;; nl {
		add	edi,				03h
		mov	dword ptr [_xErrorTable+08h],	edi
		jmp	_lErrIn
		;;----------------

		;;----------------
		;; copy line (no parse)
		_lFNPCopySTR:
		cmp	byte ptr [_bStrXX],	00h
		je	_lFNPCopySTREX
		movsb
		mov	byte ptr [edi],		20h
		inc	edi
		jmp	_lFNPCopyEx

		_lFNPCopySTREX:
		mov	byte ptr [_bStrXX],	al
		mov	byte ptr [edi],		20h
		inc	edi

		_lFNPCopy:
		movsb
		_lFNPCopyEx:
		mov	eax,			dword ptr [esi]
		cmp	al,			00h
		je	_lFNPEnd
		cmp	al,			22h	;; "
		je	_lFNPCopySTR
		cmp	ax,			7801h	;; #x
		je	_lFNPUnBlockErr
		cmp	ax,			0a0dh
		jne	_lFNPCopySx
		test	ebx,			01b
		jp	_lFNPLineEx

			;;----------------
			;; add line ex
			movsw				;; copy nl

			mov	eax,			esi
			mov	esi,			offset _bFuncCodeOneLine
			mov	ecx,			edi
			sub	ecx,			esi
			mov	edi,			dword ptr [_dFCB]
			rep	movsb
			mov	esi,			eax
			mov	dword ptr [_dFCB],	edi
			jmp	_lFNPLine
			;;----------------

		_lFNPCopySx:
		cmp	al,			06h	;; ex _
		jne	_lFNPCopy
		inc	esi
		jmp	_lFNPCopyEx
		;;----------------

		;;----------------
		;; copy line (with parse)
		_lFNPCopyParse:
		xor	eax,				eax
		jmp	_lFNPCopyParseNext

		_lFNPCopyParseCX:
		stosb
		_lFNPCopyParseNext:
		lodsb

		cmp	byte ptr [_bAscii_00+eax],	00h
		jz	_lFNPCopyParseSym
		jmp	_lFNPCopyParseCX

			;;----------------
			;; symbols
			_lFNPCopyParseSym:
			mov	ecx,			dword ptr [esi-01h]

			cmp	cx,			7c7ch		;; ||
			jne	_next
			inc	esi
			mov	dword ptr [edi],	20726f20h	;; _or_
			add	edi,			04h
			jmp	_lFNPCopyParseNext

			_lbl:
			cmp	cl,			21h		;; !
			jne	_next
			cmp	cx,			3d21h		;; !=
			jne	_lFNPNot
			stosb
			movsb
			jmp	_lFNPCopyParseNext
			_lFNPNot:
			mov	dword ptr [edi],	746f6e20h	;; _not
			mov	dword ptr [edi+04h],	20h		;; _
			add	edi,			05h
			jmp	_lFNPCopyParseNext

			_lbl:
			cmp	cx,			2626h		;; &&
			jne	_next
			inc	esi
			mov	dword ptr [edi],	646e6120h	;; _and
			mov	dword ptr [edi+04h],	20h		;; _
			add	edi,			05h
			jmp	_lFNPCopyParseNext

			_lbl:
			cmp	cx,			2b2bh		;; ++
			jne	_next
			push	2bh					;; +
			jmp	_lFNPIncDecPreX

			_lbl:
			cmp	cx,			2d2dh		;; --
			jne	_next
			push	2dh					;; -

				;;----------------
				;; inc dec pre
				_lFNPIncDecPreX:
				push	edi
				inc	esi

				mov	al,			byte ptr [esi]
				cmp	byte ptr [_bAscii_00+eax],	00h
				jz	_lFNPCopyParseNext

				mov	al,			byte ptr [esi-03h]
				cmp	byte ptr [_bAscii_00+eax],	00h
				jz	_lFNPCopyParseNext

				mov	byte ptr [edi],		20h		;; _
				inc	edi
				inc	dword ptr [esp]

				jmp	_lFNPCopyParseNext
				;;----------------

				;;----------------
				;; string
				_lbl:
				cmp	cl,			22h		;; "
				jne	_next
				stosb
				_lFNPCopyString:
				cmp	word ptr [esi],		3801h		;; #8
				je	_lFNPCopyStringFN
				movsb
				_lFNPCopyStringSX:
				cmp	byte ptr [edi-01h],	22h		;; "
				je	_lFNPCopyParseNext
				cmp	byte ptr [edi-01h],	5ch		;; \ 
				je	_lFNPCopyStringEX
				jmp	_lFNPCopyString
				_lFNPCopyStringEX:
				movsw
				jmp	_lFNPCopyStringSX

				_lFNPCopyStringFN:
				sub	edi,			02h
				add	esi,			02h
				xor	eax,			eax
				mov	ecx,			dword ptr [_dLastFuncName]

				_lFNPCopyStringFNFX:
				mov	al,			byte ptr [ecx]
				cmp	byte ptr [_bAscii_00+eax],		ah
				je	_lFNPCopyString
				stosb
				inc	ecx
				jmp	_lFNPCopyStringFNFX
				;;----------------

			_lbl:
			cmp	cx,			3d2bh		;; +=
			jne	_next
			mov	al,			2bh		;; +
			jmp	_lFNPPx

			_lbl:
			cmp	cx,			3d2dh		;; -=
			jne	_next
			mov	al,			2dh		;; -
			jmp	_lFNPPx

			_lbl:
			cmp	cx,			3d2ah		;; *=
			jne	_next
			mov	al,			2ah		;; *
			jmp	_lFNPPx

			_lbl:
			cmp	cx,			3d2fh		;; /=
			jne	_next
			mov	al,			2fh		;; /


				;;----------------
				;; += -= *= /=

					;;----------------
					;; scan
					_lFNPPx:
					cmp	byte ptr [edi-01h],	5dh		;; ]
					jne	_lFNPPxNorm

					mov	ecx,			edi
					xor	edx,			edx

					_lFNPPxSS:
					dec	ecx
					cmp	byte ptr [ecx],		5bh		;; [
					jne	_lFNPPxDD
					dec	ah
					jmp	_lFNPPxSS

					_lFNPPxDD:
					cmp	byte ptr [ecx],		5dh		;; ]
					jne	_lFNPPxFF
					inc	ah
					jmp	_lFNPPxSS

					_lFNPPxFF:
					cmp	word ptr [ecx],		0a0dh		;; nl
					je	_lFNPPxNorm
					cmp	byte ptr [ecx],		28h		;; (
					jne	_lFNPPxSS

					mov	dl,			byte ptr [ecx-01h]
					cmp	byte ptr [_bAscii_00+edx],		dh
					je	_lFNPPxSS

						;;----------------
						;; ex index
						mov	ebp,			esp
						push	ebx
						mov	ebx,			dword ptr [ebp]

						_lFNPPxII:
						dec	ecx
						cmp	byte ptr [ecx],		5dh		;; ]
						jne	_lFNPPxII_00
						inc	ah
						jmp	_lFNPPxII

						_lFNPPxII_00:
						cmp	byte ptr [ecx],		5bh		;; [
						jne	_lFNPPxII
						dec	ah
						jnz	_lFNPPxII

						mov	edx,			dword ptr [_dFCPL]
						inc	ecx

						inc	byte ptr [_bFCLL]
						mov	dword ptr [edx],	20746573h	;; set_
						mov	ah,			byte ptr [_bFCLL]
						mov	dword ptr [edx+04h],	3d207871h	;; qx_=
						mov	byte ptr [edx+06h],	ah
						add	edx,			08h

						_lFNPPxII_01:
						mov	ah,			byte ptr [ecx]
						mov	byte ptr [edx],		ah
						mov	byte ptr [ecx],		20h
						cmp	ebx,			ecx
						jne	_lFNPPxII_SC
						mov	dword ptr [ebp],	edx
						sub	ebp,			04h
						mov	ebx,			dword ptr [ebp]

						_lFNPPxII_SC:
						inc	edx
						inc	ecx
						cmp	ecx,			edi
						jne	_lFNPPxII_01

						mov	ah,			byte ptr [_bFCLL]
						mov	word ptr [edx-01h],	0a0dh
						mov	dword ptr [edi-04h],	5d207871h	;; qx*]
						inc	edx
						mov	byte ptr [edi-02h],	ah

						mov	dword ptr [_dFCPL],	edx
						pop	ebx
						;;----------------

					;;----------------

					;;----------------
					;; norm
					_lFNPPxNorm:
					mov	edx,			edi
					movsb					;; =
					mov	ecx,			esi
					_lFNPPxEX:
					inc	ecx
					cmp	word ptr [ecx],		0a0dh
					jne	_lFNPPxEX
					mov	word ptr [ecx],		3101h	;; #1
					lea	ecx,			[edi-01h]
					_lFNPPxSX:
					dec	edx
					cmp	dword ptr [edx-04h],	20746573h	;; set_
					jne	_lFNPPxSX
					_lFNPPxDX:
					mov	ah,			byte ptr [edx]
					mov	byte ptr [edi],		ah
					inc	edx
					inc	edi
					cmp	edx,			ecx
					jb	_lFNPPxDX
					mov	byte ptr [edi],		al
					mov	byte ptr [edi+01h],	28h
					add	edi,			02h
					xor	eax,			eax
					jmp	_lFNPCopyParseNext
					;;----------------
				;;----------------

				;;----------------
				_lbl:
				cmp	cl,				0eh		;; ;
				jne	_next

				mov	dword ptr [_dVarParams],	edi
				mov	dword ptr [edi],		0a0dh		;; nl
				dec	esi
				add	edi,				02h
				mov	byte ptr [esi],			0ch		;; ;
				test	ebx,			01b
				jp	_lFNPLine
				jmp	_lFNPIncDecSTX
				;;----------------

			_lbl:
			cmp	cx,			0a0dh		;; new line
			jne	_next
			mov	dword ptr [edi],	0a0dh		;; new line
			inc	esi
			add	edi,			02h
			test	ebx,			01b
			jp	_lFNPLine

				;;----------------
				;; ++ -- in stack
				_lFNPIncDecSTX:
				mov	dword ptr [_dBuffer],	esi
				mov	esi,			edi
				mov	edi,			dword ptr [_dFCB]

				_lFNPIncDecSTXNext:
				cmp	dword ptr [esp],	00h
				je	_lFNPIncDecSTXEnd

				pop	ebp
				xor	eax,			eax
				xor	edx,			edx
				mov	al,			byte ptr [ebp]
				mov	ecx,			ebp			;; ecx = operation position

				cmp	byte ptr [_bAscii_03+eax],		ah
				jz	_lFNPIDS_00p

					;;----------------
					;; pref

						;;----------------
						;; scan
						_lFNPIDS_00:
						inc	ecx
						mov	al,			byte ptr [ecx]

						cmp	al,			5bh		;; [
						jne	_lFNPIDS_01
						inc	edx
						jmp	_lFNPIDS_00

						_lFNPIDS_01:
						cmp	al,			28h		;; (
						jne	_lFNPIDS_02
						mov	al,			byte ptr [ecx-01h]
						cmp	byte ptr [_bAscii_00+eax],		ah
						jne	_lFNPIDEI
						jmp	_lFNPIDS_00

						_lFNPIDS_02:
						cmp	al,			5dh		;; ]
						jne	_lFNPIDS_03
						test	edx,			edx
						jz	_lFNPIDS_04
						dec	edx
						jnz	_lFNPIDS_00
						jmp	_lFNPIDS_04

						_lFNPIDS_03:
						cmp	byte ptr [_bAscii_03+eax],		ah
						jne	_lFNPIDS_00

						_lFNPIDS_04:
						mov	ecx,			ebp
						;;----------------

						;;----------------
						;; norm
						_lFNPIncDecSTX_Norm:
						mov	dword ptr [edi],	20746573h	;; set_
						add	edi,			04h
						jmp	_lFNPIncDecSTXPre

						_lFNPIncDecSTXPreNext:
						inc	ebp
						_lFNPIncDecSTXPre:
						mov	al,			byte ptr [ebp]
						cmp	al,			5bh		;; [
						jne	_lFNPIncDecSTXPre_00
						inc	edx
						jmp	_lFNPIncDecSTXPre_02
						_lFNPIncDecSTXPre_00:
						test	edx,			edx
						jnz	_lFNPIncDecSTXPre_03
						cmp	byte ptr [_bAscii_03+eax],		ah
						jne	_lFNPIncDecSTXPre_02
						_lFNPIncDecSTXPre_01:
						mov	edx,			ecx
						sub	ecx,			ebp
						neg	ecx

						mov	ebp,			edi
						mov	byte ptr [edi],		3dh		;; =
						inc	edi

						_lFNPIncDecSTXCopyNext:
						mov	al,			byte ptr [edx]
						inc	edx
						mov	byte ptr [edi],		al
						inc	edi
						dec	ecx
						jnz	_lFNPIncDecSTXCopyNext
						pop	eax
						or	eax,			0a0d3100h	;; x 1 new line
						mov	dword ptr [edi],	eax
						add	edi,			04h
						jmp	_lFNPIncDecSTXNext

						_lFNPIncDecSTXPre_02:
						mov	byte ptr [edi],		al
						inc	edi
						jmp	_lFNPIncDecSTXPreNext

						_lFNPIncDecSTXPre_03:
						cmp	al,			5dh		;; ]
						jne	_lFNPIncDecSTXPre_02
						dec	edx
						jmp	_lFNPIncDecSTXPre_02
						;;----------------

						;;----------------
						;; ex index
						_lFNPIDEI:
						inc	byte ptr [_bFCLL]
						mov	ecx,			ebp
						xor	edx,			edx
						mov	al,			byte ptr [_bFCLL]

						mov	dword ptr [edi],	20746573h	;; set_
						mov	dword ptr [edi+04h],	3d207871h	;; qx_=
						mov	byte ptr [edi+06h],	al
						add	edi,			08h

						_lFNPIDEIndexSet:
						inc	ecx
						cmp	byte ptr [ecx],		5bh		;; [
						jne	_lFNPIDEIndexSet
						inc	ecx

						_lFNPIDEIndexSetEX:
						mov	ah,			byte ptr [ecx]
						cmp	ah,			5bh		;; [
						jne	_lFNPIDEIndexSetFX
						inc	edx
						jmp	_lFNPIDEIndexSetCopy

						_lFNPIDEIndexSetFX:
						cmp	ah,			5dh		;; ]
						jne	_lFNPIDEIndexSetCopy
						dec	edx
						jns	_lFNPIDEIndexSetCopy

						mov	word ptr [edi],		0a0dh		;; nl
						mov	word ptr [ecx-03h],	7871h		;; qx
						mov	byte ptr [ecx-01h],	al
						add	edi,			02h
						xor	edx,			edx
						mov	ecx,			ebp
						xor	eax,			eax
						jmp	_lFNPIncDecSTX_Norm

						_lFNPIDEIndexSetCopy:
						mov	byte ptr [ecx],		20h		;; bs
						mov	byte ptr [edi],		ah
						inc	ecx
						inc	edi
						jmp	_lFNPIDEIndexSetEX
						;;----------------
					;;----------------

					;;----------------
					;; post

						;;----------------
						;; scan
						_lFNPIDS_00p:
						dec	ecx
						mov	al,			byte ptr [ecx]

						cmp	al,			5dh		;; ]
						jne	_lFNPIDS_01p
						inc	edx
						jmp	_lFNPIDS_00p

						_lFNPIDS_01p:
						cmp	al,			5bh		;; [
						jne	_lFNPIDS_02p
						test	edx,			edx
						jz	_lFNPIDS_04p
						dec	edx
						js	_lFNPIDS_04p
						jmp	_lFNPIDS_00p

						_lFNPIDS_02p:
						cmp	al,			28h		;; (
						jne	_lFNPIDS_03p
						test	edx,			edx
						jz	_lFNPIDS_04p
						mov	al,			byte ptr [ecx-01h]
						cmp	byte ptr [_bAscii_00+eax],		ah
						je	_lFNPIDS_00p
						jmp	_lFNPIDEIp

						_lFNPIDS_03p:
						cmp	byte ptr [_bAscii_03+eax],		ah
						jne	_lFNPIDS_00p
						test	edx,			edx
						jnz	_lFNPIDS_00p

						_lFNPIDS_04p:
						mov	ecx,			ebp
						;;----------------

						;;----------------
						;; norm
						_lFNPIncDecSTXPost:
						mov	byte ptr [_bCodePosOp],	01h		;; post operation = 1
						_lFNPIncDecSTXPostNext:
						dec	ebp
						mov	al,			byte ptr [ebp]
						cmp	al,			5dh		;; ]
						jne	_lFNPIncDecSTXPost_00
						inc	edx
						jmp	_lFNPIncDecSTXPostNext
						_lFNPIncDecSTXPost_00:
						cmp	byte ptr [_bAscii_03+eax],		ah
						jnz	_lFNPIncDecSTXPostNext
						test	edx,			edx
						jz	_lFNPIncDecSTXPost_01
						cmp	al,			5bh		;; [
						jne	_lFNPIncDecSTXPostNext
						dec	edx
						jmp	_lFNPIncDecSTXPostNext

						_lFNPIncDecSTXPost_01:
						inc	ebp
						sub	ecx,			ebp
						lea	edx,			[ecx+01h]
						mov	dword ptr [esi],	20746573h	;; set_
						add	esi,			04h

						_lFNPIncDecSTXCopyEX:
						mov	al,			byte ptr [ebp]
						inc	ebp
						mov	byte ptr [esi],		al
						mov	byte ptr [esi+edx],	al
						inc	esi
						dec	ecx
						jnz	_lFNPIncDecSTXCopyEX
						mov	byte ptr [esi],		3dh		;; =
						add	esi,			edx
						pop	eax
						or	eax,			0a0d3100h	;; x 1 new line
						mov	dword ptr [esi],	eax
						add	esi,			04h
						jmp	_lFNPIncDecSTXNext
						;;----------------

						;;----------------
						;; ex index
						_lFNPIDEIpEX:
						inc	edx
						_lFNPIDEIp:
						dec	ecx
						cmp	byte ptr [ecx],		5dh		;; ]
						je	_lFNPIDEIpEX
						cmp	byte ptr [ecx],		5bh		;; [
						jne	_lFNPIDEIp
						dec	edx
						jnz	_lFNPIDEIp

						inc	byte ptr [_bFCLL]
;						push	edi
						mov	al,			byte ptr [_bFCLL]
;						mov	edi,			dword ptr [_dFCPL]

						mov	dword ptr [edi],	20746573h	;; set_
						mov	dword ptr [edi+04h],	3d207871h	;; qx_=
						mov	byte ptr [edi+06h],	al
						add	edi,			08h
						lea	edx,			[ebp-01h]

						_lFNPIDEIndexSetp:
						inc	ecx
						mov	ah,			byte ptr [ecx]
						mov	byte ptr [edi],		ah
						inc	edi
						mov	byte ptr [ecx],		20h		;; bs
						cmp	ecx,			edx
						jne	_lFNPIDEIndexSetp

						inc	ecx
						mov	word ptr [edi-01h],	0a0dh
						mov	dword ptr [ebp-04h],	5d207871h	;; qx*]
						inc	edi
						mov	byte ptr [ebp-02h],	al
						xor	eax,			eax
;						mov	dword ptr [_dFCPL],	edi
;						pop	edi
						xor	edx,			edx
						jmp	_lFNPIncDecSTXPost
						;;----------------
					;;----------------

					;;----------------
					;; copy code
					_lFNPIncDecSTXEnd:

						;;----------------
						cmp	byte ptr [_bCodeSys],		00h
						je	_lFNPIncDecSTXEndDD

						cmp	byte ptr [_bCodePosOp],		00h
						je	_lFNPIncDecSTXEndFL

						mov	eax,				offset _bFuncCodeOneLine

						cmp	word ptr [eax],			6669h		;; if
						jne	_lFNPEXExit

							;;----------------
							;; if
							mov	byte ptr [_bTempBool],		01h
							mov	dword ptr [eax],		20746573h	;; set_
							mov	dword ptr [eax+04h],		765f6a63h	;; cj_v
							mov	dword ptr [eax+08h],		5f363636h	;; 666_
							mov	word ptr [eax+0ch],		3d62h		;; b=

							mov	dword ptr [esi],		63206669h	;; if_c
							mov	dword ptr [esi+04h],		36765f6ah	;; j_v6
							mov	dword ptr [esi+08h],		625f3636h	;; 66_b
							mov	dword ptr [esi+0ch],		65687420h	;; _the
							mov	dword ptr [esi+10h],		000a0d6eh	;; en__

							add	esi,				13h

							_lFNPIncDecSTXEndRemThen:
							inc	eax
							cmp	dword ptr [eax],		6e656874h	;; then
							jne	_lFNPIncDecSTXEndRemThen
							cmp	word ptr [eax+04h],		0a0dh		;; nl
							jne	_lFNPIncDecSTXEndRemThen
							mov	dword ptr [eax],		20202020h	;; bs
							jmp	_lFNPIncDecSTXEndFL
							;;----------------

							;;----------------
							;; exitwhen
							_lFNPEXExit:
							cmp	dword ptr [eax],		74697865h	;; exit
							jne	_lFNPEXRetn

							mov	dword ptr [eax],		20746573h	;; set_
							mov	dword ptr [eax+04h],		765f6a63h	;; cj_v
							mov	dword ptr [eax+08h],		5f363636h	;; 666_
							mov	word ptr [eax+0ch],		3d62h		;; b=

							mov	byte ptr [_bTempBool],		01h

							mov	dword ptr [esi],		74697865h	;; exit
							mov	dword ptr [esi+04h],		6e656877h	;; when
							mov	dword ptr [esi+08h],		5f6a6320h	;; _cj_
							mov	dword ptr [esi+0ch],		36363677h	;; v666
							mov	dword ptr [esi+10h],		0a0d625fh	;; _b__

							add	esi,				14h

							jmp	_lFNPIncDecSTXEndFL
							;;----------------

							;;----------------
							;; return
							_lFNPEXRetn:
							mov	byte ptr [_bTempType],		01h
							mov	dword ptr [eax],		20746573h	;; set_
							mov	dword ptr [eax+04h],		765f6a63h	;; cj_v
							mov	dword ptr [eax+08h],		5f363636h	;; 666_
							mov	word ptr [eax+0ch],		3d72h	;; r=

							mov	dword ptr [esi],		75746572h	;; retu
							mov	dword ptr [esi+04h],		63206e72h	;; rn_c
							mov	dword ptr [esi+08h],		36765f6ah	;; j_v6
							mov	dword ptr [esi+0ch],		725f3636h	;; 66_r
							mov	word ptr [esi+10h],		0a0dh		;; nl

							add	esi,				12h
							;;----------------

						_lFNPIncDecSTXEndFL:
						mov	byte ptr [_bCodeSys],	00h
						;;----------------

						;;----------------
						_lFNPIncDecSTXEndDD:
						mov	edx,			esi

						mov	ecx,			dword ptr [_dFCPL]
						mov	esi,			offset _bFuncPostEX
						sub	ecx,			esi
						rep	movsb
						mov	dword ptr [_dFCPL],	offset _bFuncPostEX

							;;----------------
							;; locals
							cmp	dword ptr [_bFuncCodeOneLine],		61636f6ch	;; loca
							jne	_lFNPIncDecSTXEndSFXD_Pre
							cmp	word ptr [_bFuncCodeOneLine+04h],	206ch		;; l_
							jne	_lFNPIncDecSTXEndSFXD_Pre

							mov	dword ptr [_dFCB],			edi
							mov	edi,					dword ptr [_dFCL]
							mov	esi,					offset _bFuncCodeOneLine
							cmp	dword ptr [_dFCB],			offset _bFuncCodeBase
							jne	_lFNPLoc_02

								;;----------------
								;; norm
								_lFNPLoc_00:
								cmp	word ptr [esi],			0a0dh		;; nl
								je	_lFNPLoc_01
								movsb
								jmp	_lFNPLoc_00						

								_lFNPLoc_01:
								movsw
								mov	dword ptr [_dFCL],		edi
								mov	edi,				dword ptr [_dFCB]
								jmp	_lFNPIncDecSTXEndSFXD
								;;----------------

								;;----------------
								;; ex
								_lFNPLoc_02:
								cmp	word ptr [esi],			0a0dh		;; nl
								je	_lFNPLoc_00
								cmp	byte ptr [esi],			3dh		;; =
								je	_lFNPLoc_03

								movsb
								jmp	_lFNPLoc_02

									;;----------------
									;; scan
									_lFNPLoc_03:
									mov	ecx,				esi
									xor	eax,				eax

									_lFNPLoc_03_AX:
									inc	ecx

									_lFNPLoc_03_AXX:
									cmp	word ptr [ecx],			0a0dh
									je	_lFNPLoc_00

									_lFNPLoc_03_BX:
									mov	al,				byte ptr [ecx]
									cmp	al,				22h	;; "
									je	_lFNPLoc_03_Str

									cmp	al,				30h
									jb	_lFNPLoc_03_AX
									cmp	al,				3ah
									jb	_lFNPLoc_03_Dgt
									cmp	byte ptr [_bAscii_00+eax],	ah
									je	_lFNPLoc_03_AX

										;;----------------
										;; check word
										mov	ebp,				offset _sCodeConst
										push	ecx

										_lFNPLoc_03_Word:
										mov	al,				byte ptr [ecx]
										cmp	byte ptr [ebp],			al
										jne	_lFNPLoc_03_WordNext
										inc	ecx
										inc	ebp
										jmp	_lFNPLoc_03_Word

										_lFNPLoc_03_WordNext:
										cmp	byte ptr [_bAscii_00+eax],	ah
										jne	_lFNPLoc_03_WordNextFX
										cmp	byte ptr [ebp],			ah
										jne	_lFNPLoc_03_WordNextFX

										add	esp,				04h
										jmp	_lFNPLoc_03_AXX

										_lFNPLoc_03_WordNextFX:
										mov	ecx,				dword ptr [esp]
										_lFNPLoc_03_WordNextEX:
										inc	ebp
										cmp	byte ptr [ebp],			ah
										jne	_lFNPLoc_03_WordNextEX

										inc	ebp
										cmp	byte ptr [ebp],			00h
										jne	_lFNPLoc_03_Word

										pop	ecx
										jmp	_lFNPLoc_05
										;;----------------

									_lFNPLoc_03_Dgt:
									inc	ecx
									mov	al,			byte ptr [ecx]
									cmp	byte ptr [_bAscii_00+eax],	ah
									jne	_lFNPLoc_03_Dgt
									jmp	_lFNPLoc_03_AXX

									_lFNPLoc_03_Str:
									inc	ecx
									cmp	byte ptr [ecx],		5ch	;; \ 
									jne	_lFNPLoc_03_StrEX
									add	ecx,			02h
									_lFNPLoc_03_StrEX:
									cmp	byte ptr [ecx],		22h	;; "
									jne	_lFNPLoc_03_Str
									jmp	_lFNPLoc_03_AX
									;;----------------

								_lFNPLoc_05:
								mov	word ptr [edi],			0a0dh
								add	edi,				02h
								mov	dword ptr [_dFCL],		edi
								mov	edi,				dword ptr [_dFCB]
								mov	dword ptr [edi],		20746573h	;; set_
								add	edi,				04h

								_lFNPLoc_04:
								dec	esi
								cmp	byte ptr [esi-01h],		20h		;; bs
								jne	_lFNPLoc_04
								jmp	_lFNPIncDecSTXEndSFXD
								;;----------------
							;;----------------

						_lFNPIncDecSTXEndSFXD_Pre:
						mov	esi,			offset _bFuncCodeOneLine

						_lFNPIncDecSTXEndSFXD:
						mov	ecx,			edx
						sub	ecx,			esi
						rep	movsb

						mov	al,			byte ptr [_bFCLL]
						cmp	byte ptr [_bFCLLMAX],	al
						jg	_lFNPIncDecSTXEndXX
						mov	byte ptr [_bFCLLMAX],	al
						_lFNPIncDecSTXEndXX:
						mov	byte ptr [_bFCLL],	40h
						;;----------------

					mov	esi,			dword ptr [_dBuffer]

					mov	dword ptr [_dFCB],	edi
					jmp	_lFNPLine
					;;----------------
				;;----------------

			_lbl:
			cmp	cl,			06h		;; ex bs
			je	_lFNPCopyParseNext

			_lbl:
			cmp	cl,			01h		;; #
			jne	_lFNPCopyParseCX

				;;----------------
				;; parse #
				cmp	cx,			3001h		;; #0
				jne	_next
				mov	word ptr [esi-01h],	0a0dh		;; new line
				inc	esi
				mov	dword ptr [edi],	65687420h	;; _the
				mov	dword ptr [edi+04h],	000a0d6eh	;; n new line
				add	edi,			07h		;; !
				jmp	_lFNPIncDecSTX

				_lbl:
				cmp	cx,			3101h		;; #1
				jne	_next
				mov	word ptr [esi-01h],	0a0dh		;; new line
				mov	dword ptr [edi],	000a0d29h	;; )__x
				inc	esi
				add	edi,			03h		;; !!!
				jmp	_lFNPIncDecSTX

				_lbl:
				cmp	cx,			6601h		;; #f
				jne	_next
				inc	esi
				mov	dword ptr [edi],	6e650a0dh	;; nl en
				mov	dword ptr [edi+04h],	6f6f6c64h	;; dloo
				mov	dword ptr [edi+08h],	000a0d70h	;; p nl
				add	edi,			0bh
				jmp	_lFNPIncDecSTX

					;;----------------
					;; func name
					_lbl:
					cmp	cx,			3801h		;; #8
					jne	_next

					inc	esi
					sub	edi,			02h
					xor	eax,			eax
					mov	ecx,			dword ptr [_dLastFuncName]

					_lFNPFuncNameCopy:
					mov	al,			byte ptr [ecx]
					cmp	byte ptr [_bAscii_00+eax],		ah
					je	_lFNPCopyParseNext
					stosb
					inc	ecx
					jmp	_lFNPFuncNameCopy
					;;----------------

				_lbl:
				cmp	cx,			3301h		;; #3
				jne	_next
				mov	word ptr [esi-01h],	0a0dh		;; new line
				inc	esi
				mov	dword ptr [edi],	6e650a0dh	;; __en
				mov	dword ptr [edi+04h],	6f6c6764h	;; dglo
				mov	dword ptr [edi+08h],	736c6162h	;; bals
				mov	word ptr [edi+0ch],	0a0dh		;; __
				add	edi,			0eh
				jmp	_lFNPLine

				_lbl:
				cmp	cx,			7801h		;; #x
				je	_lFNPUnBlockErr

				_lbl:
				cmp	cx,			7001h		;; #p
				jne	_next
				mov	al,			2bh		;; +

					;;----------------
					;; inc/dec
					_lFNPIncDec:

						;;----------------
						;; scan
						cmp	byte ptr [edi-01h],	5dh	;; ]
						jne	_lFNPIncDecNormJJ

						mov	edx,			edi
						xor	ecx,			ecx

						_lFNPIncDecFXScan:
						dec	edx

						cmp	byte ptr [edx],		28h	;; (
						jne	_lFNPIncDecFXScan_00
						mov	cl,			byte ptr [edx-01h]
						cmp	byte ptr [_bAscii_00+ecx],		ch
						je	_lFNPIncDecFXScan

							;;----------------
							;; ex index
							_lFNPIncDecEXI_00:
							dec	edx
							cmp	dword ptr [edx-04h],	20746573h	;; set_
							jne	_lFNPIncDecEXI_00

							_lFNPIncDecEXI_01:
							inc	edx
							cmp	byte ptr [edx],		5bh		;; [
							jne	_lFNPIncDecEXI_01

							mov	ebp,			esp
							push	ebx
							mov	ebx,			dword ptr [ebp]

							mov	ecx,			dword ptr [_dFCPL]
							inc	edx

							inc	byte ptr [_bFCLL]
							mov	dword ptr [ecx],	20746573h	;; set_
							mov	ah,			byte ptr [_bFCLL]
							mov	dword ptr [ecx+04h],	3d207871h	;; qx_=
							mov	byte ptr [ecx+06h],	ah
							add	ecx,			08h

							_lFNPIncDecEXI_02:
							mov	ah,			byte ptr [edx]
							mov	byte ptr [ecx],		ah
							mov	byte ptr [edx],		20h
							cmp	ebx,			edx
							jne	_lFNPIncDecEXI_03
							mov	dword ptr [ebp],	ecx
							sub	ebp,			04h
							mov	ebx,			dword ptr [ebp]

							_lFNPIncDecEXI_03:
							inc	ecx
							inc	edx
							cmp	edx,			edi
							jne	_lFNPIncDecEXI_02

							mov	ah,			byte ptr [_bFCLL]
							mov	word ptr [ecx-01h],	0a0dh		;; nl
							mov	dword ptr [edi-04h],	5d207871h	;; qx*]
							inc	ecx
							mov	byte ptr [edi-02h],	ah

							mov	dword ptr [_dFCPL],	ecx
							pop	ebx

							jmp	_lFNPIncDecNormJJ
							;;----------------

						_lFNPIncDecFXScan_00:
						cmp	word ptr [edx],		0a0dh	;; nl
						jne	_lFNPIncDecFXScan
						;;----------------

					_lFNPIncDecNormJJ:
					mov	edx,			edi
					mov	ecx,			edi
					_lFNPIncDecEX:
					dec	edx
					cmp	dword ptr [edx-04h],	20746573h	;; set_
					jne	_lFNPIncDecEX
					mov	byte ptr [edi],		3dh		;; =
					inc	edi
					_lFNPIncDecSX:
					mov	ah,			byte ptr [edx]
					mov	byte ptr [edi],		ah
					inc	edx
					inc	edi
					cmp	edx,			ecx
					jb	_lFNPIncDecSX
					mov	byte ptr [edi],		al
					mov	dword ptr [edi+01h],	000a0d31h	;; 1__x
					add	edi,			04h
					add	esi,			03h		;; !!!
					jmp	_lFNPIncDecSTX
					;;----------------

				_lbl:
				cmp	cx,			6d01h		;; #m
				;;jne	_next
				mov	al,			2dh		;; -
				jmp	_lFNPIncDec
				;;----------------

				;;----------------
				;; multidefined variables
				_lFNPVarX:
				inc	esi
				mov	edx,			esi		;; store esi
				mov	esi,			dword ptr [_dVarParams]
;				mov	word ptr [edi],		0a0dh
;				add	edi,			02h
				_lbl:						;; search begin of parsed line
				dec	esi
				cmp	word ptr [esi-02h],	0a0dh
				je	_lNPVarXKeyPreX
				cmp	byte ptr [esi-01h],	00h
				jne	_prew

				_lNPVarXKeyPreX:
				mov	eax,			dword ptr [esi]

				cmp	eax,			61636f6ch	;; loca
				jne	_next
				cmp	word ptr [esi+04h],	206ch		;; l_
				jne	_next
				movsd
				movsw
				jmp	_lNPVarXKeyPreX

				_lbl:
				cmp	eax,			76697270h	;; priv
				jne	_next
				cmp	dword ptr [esi+04h],	20657461h	;; ate_
				jne	_next
				movsd
				movsd
				jmp	_lNPVarXKeyPreX

				_lbl:
				cmp	eax,			6c627570h	;; publ
				jne	_next
				cmp	word ptr [esi+04h],	6369h		;; ic
				jne	_next
				cmp	byte ptr [esi+06h],	20h		;; _
				jne	_next
				movsd
				movsw
				movsb
				jmp	_lNPVarXKeyPreX

				_lbl:
				cmp	eax,			74617473h	;; stat
				jne	_next
				cmp	word ptr [esi+04h],	6369h		;; ic
				jne	_next
				cmp	byte ptr [esi+06h],	20h		;; _
				jne	_next
				movsd
				movsw
				movsb
				jmp	_lNPVarXKeyPreX

				_lbl:
				cmp	eax,			736e6f63h	;; cons
				jne	_next
				cmp	dword ptr [esi+04h],	746e6174h	;; tant
				jne	_next
				cmp	byte ptr [esi+08h],	20h		;; _
				jne	_next
				movsd
				movsd
				movsb
				jmp	_lNPVarXKeyPreX

				_lbl:
				cmp	eax,			75626564h	;; debu
				jne	_next
				cmp	word ptr [esi+04h],	2067h		;; g_
				jne	_next
				movsd
				movsw
				jmp	_lNPVarXKeyPreX

				_lbl:
				movsb						;; copy type
				cmp	byte ptr [esi-01h],	20h
				jne	_prew

				_lbl:						;; search next word
				cmp	dword ptr [esi],	61727261h	;; arra
				jne	_next
				cmp	word ptr [esi+04h],	2079h		;; y_	
				jne	_next
				mov	dword ptr [edi],	61727261h	;; arra
				mov	word ptr [edi+04h],	2079h		;; y_
				add	edi,			06h
				_lbl:	
				mov	esi,			edx		;; restore esi
				xor	eax,			eax
				jmp	_lFNPCopyParseNext
				;;----------------
			;;----------------
		;;----------------

	_lFNPEnd:
	cmp	word ptr [edi-02h],	0a0dh		;; new line
	jne	_next
	sub	edi,			02h
	_lbl:

	;;----------------
	add	esi,			04h
	mov	esp,			_dStackPos	;; restore esp
	sub	edi,			esi		;; edi = new script size
	add	esp,			04h

	;;----------------
	mov	_dCurrStr,		offset _sProg_05
	mov	eax,			64h
	call	_lSetProg
	;;----------------	
	
	retn	

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; parse endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;-------------------------------------------------------------------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc codep
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	_lOpenMapCode:

	push	THREAD_PRIORITY_TIME_CRITICAL
	push	dword ptr [_hTrd]
	call	_imp__SetThreadPriority@8

	mov	dword ptr [_dPreStackPos],	esp	;; for error

	push	0ffh
	push	04h
	push	offset _sMapPath
	call	_imp__MpqOpenArchiveForUpdate@12

	test	eax,	eax
	jnz	_lCodep_00

		;;----------------
		;; error
		push	MB_ICONERROR
		push	offset _sWinName
		push	offset _sErr_Arch
		push	dword ptr [_hWnd]
		call	_imp__MessageBoxA@16
		push	01h
		call	_imp__ExitProcess@4
		;;----------------

	_lCodep_00:
	push	offset _sAttr
	push	eax

	push	offset _fScr
	push	00h
	push	offset _sWJ
	push	eax
	call	_imp__SFileOpenFileEx@16

	test	eax,	eax
	jnz	_next

		;;----------------
		;; error
		push	MB_ICONERROR
		push	offset _sWinName
		push	offset _sErr_Code
		push	dword ptr [_hWnd]
		call	_imp__MessageBoxA@16
		push	01h
		call	_imp__ExitProcess@4
		;;----------------

	_lbl:
	push	00h
	push	_fScr
	call	_imp__SFileGetFileSize@8
	mov	ebx,	eax

	shl	eax,	06h
	add	eax,	00800000h	;; add 8 megabyte to include
	push	eax
	push	GMEM_ZEROINIT
	call	_imp__GlobalAlloc@8
	push	eax			;; SrcMem handle

	push	eax
	call	_imp__GlobalLock@4
	mov	esi,	eax		;; esi = SrcMem  address
	push	eax
	lea	edi,	[eax+ebx+04h]	;; edi = DestMem address

	push	edi

	push	00h
	push	00h
	push	edi
	push	esi
	push	_fScr
	call	_imp__SFileReadFile@20

	;;----------------
	;; start code processing
	call	dword ptr [_dMapProcCode]
	add	esp,	04h
	;;----------------

	push	dword ptr [_dfilename]
	call	_imp__DeleteFileA@4

	push	00h
	push	FILE_ATTRIBUTE_ARCHIVE
	push	CREATE_ALWAYS
	push	00h
	push	FILE_SHARE_WRITE
	push	GENERIC_WRITE
	push	dword ptr [_dfilename]
	call	_imp__CreateFileA@28

	push	eax			;; ---> _imp__CloseHandle@4

	push	00h
	push	offset _dBuffer
	push	edi			;; <--- !!! byter to write
	push	esi
	push	eax
	call	_imp__WriteFile@20

	call	_imp__CloseHandle@4

	call	_imp__GlobalUnlock@4
	call	_imp__GlobalFree@4

	mov	ebp,			dword ptr [esp]
	call	_imp__MpqDeleteFile@8

	push	09h
	push	02h			;; 08h
	push	0201h			;; MAFA_REPLACE_EXISTING
	push	offset _sWJ
	push	dword ptr [_dfilename];offset _sWJ
	push	ebp
	call	_imp__MpqAddFileToArchiveEx@24

	push	ebp
	call	_imp__MpqCompactArchive@4

	push	00h
	push	ebp
	call	_imp__MpqCloseUpdatedArchive@8

	_lOpenMapCodeEnd:
	push	00h
	push	00h
	push	WM_PROCEND
	push	_hWnd
	call	_imp__PostMessageA@16

	xor	eax,	eax
	retn

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; codep endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;-------------------------------------------------------------------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc main
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	start:	;; <---
	
	;;----------------
	;; command line
	call	_imp__GetCommandLineA@0
	mov	ebx,	eax
	dec	ebx
	
	_lCLScanStart:
	inc	ebx
	cmp	byte ptr [ebx],		00h
	je	_lCLAbout
	cmp	byte ptr [ebx],		2fh
	jne	_lCLScanStart

		;;----------------
		;; do not remove unused code
		cmp	dword ptr [ebx+01h],	20706f6eh	;; nop
		jne	_lCLScanVerEX

		mov	dword ptr [_dWarVerSL],	00h
		add	ebx,			05h

		jmp	_lCLScanMap
		;;----------------

		;;----------------
		;; version
		_lCLScanVerEX:
		cmp	dword ptr [ebx+01h],	20333276h	;; v23
		jne	_lCLScanVer

		add	ebx,			05h
		mov	dword ptr [_dWarVerSL],	offset _sVer23
		jmp	_lCLScanDbg

		_lCLScanVer:
		cmp	dword ptr [ebx+01h],	20343276h	;; v24
		jne	_lCLScanDbg

		add	ebx,			05h
		mov	dword ptr [_dWarVerSL],	offset _sVer24
		;;----------------

		;;----------------
		;; debug mode?
		_lCLScanDbg:
		cmp	dword ptr [ebx+01h],	20676264h	;; dbg_
		jne	_lCLScanMap
		add	ebx,			05h
		mov	dword ptr [_dDbgOff],	offset _lCRDebugAdd
		;;----------------
	
		;;----------------
		;; parse mappath
		_lCLScanMap:
		cmp	dword ptr [ebx+01h],	6f70616dh	;; mapo ;; mapoptz="..."
		jne	_lCLScanMapEX
		mov	dword ptr [_dMapProcCode],	offset _lMapOptimizeCode
		mov	dword ptr [_dfilename],		offset _sWJO
		jmp	_lCLMapScanSX
	
		_lCLScanMapEX:
		cmp	dword ptr [ebx+01h],	7070616dh	;; mapp ;; mappars="..."
		jne	_lCLAbout
		mov	dword ptr [_dMapProcCode],	offset _lMapParseCode
	
		_lCLMapScanSX:
		mov	edi,	offset _sMapPath
		lea	esi,	[ebx+0ah]
	
		_lCLScanMapNext:
		mov	al,	byte ptr [esi]
		cmp	al,	22h
		je	_lCLScanMapEnd
	
		mov	byte ptr [edi],			al
		mov	byte ptr [edi+_dMapPathToEX],	al
		inc	esi
		inc	edi
		jmp	_lCLScanMapNext
	
			;;----------------
			;; find map directory
			_lCLScanMapEnd:
			add	edi,	_dMapPathToEX
			_lCLFindMapDirr:
			dec	edi
			cmp	byte ptr [edi],			5ch
			jne	_lCLFindMapDirr
			mov	dword ptr [_dMapPathEnd],	edi
			jmp	_lCLSetCurrDir
			;;----------------
		;;----------------

		;;----------------
		;; about
		_lCLAbout:
		mov	dword ptr [_hWndCls],		CS_HREDRAW or CS_VREDRAW or CS_BYTEALIGNWINDOW
		mov	dword ptr [_hWndCls+04h],	offset _lInfoStart
		mov	dword ptr [_hWndCls+18h],	00h
		mov	dword ptr [_dBuffer],		00ff0000h
		mov	dword ptr [_dWndStlEx],		WS_VISIBLE or WS_CAPTION or WS_SYSMENU

		push	IDC_ARROW
		push	00h
		call	_imp__LoadCursorA@8
		mov	dword ptr [_dStdCursor],	eax

		push	40h
		push	00400000h
		call	_imp__LoadCursorA@8
		mov	dword ptr [_dExCursor],		eax

		push	offset _xWWWFont
		call	_imp__CreateFontIndirectA@4
		mov	dword ptr [_dWWWFont],		eax

		jmp	_lSrartEX
		;;----------------
	;;----------------	

	;;----------------
	;; set legal current directory
	_lCLSetCurrDir:
	mov	edi,				offset _sCurrDir
	push	edi
	push	edi
	push	0200h
	call	_imp__GetCurrentDirectoryA@8
	add	edi,				eax
	cmp	dword ptr [edi-04h],		7265706ch	;; lper
	je	_next
	mov	dword ptr [edi],		6964415ch	;; \Adi
	mov	dword ptr [edi+04h],		6c654863h	;; cHel
	mov	dword ptr [edi+08h],		00726570h	;; per_
	add	edi,				0bh
	call	_imp__SetCurrentDirectoryA@4
	_lbl:
	mov	dword ptr [edi],		62696c5ch	;; \lib
	mov	byte ptr [edi+04h],		5ch		;; \ 
	add	edi,				04h		;; !!!
	mov	_dCurrDirEnd,			edi
	;;----------------
	
	_lSrartEX:
	xor	ebx,	ebx
	;;push	ebx		;; ebx ---> ExitProcess

	;;----------------
	;; load gui font
	;;push	11h
	;;call	_imp__GetStockObject@4
	push	offset _xGuiFont
	call	_imp__CreateFontIndirectA@4
	mov	dword ptr [_dGuiFont],		eax
	;;----------------

	;;----------------
	;; create main window
	push	20h
	push	400000h
	call	_imp__LoadIconA@8
	mov	dword ptr [_hWndCls+14h],	eax
	mov	dword ptr [_hErrCls+14h],	eax
;;	mov	dword ptr [_hIconCJ],		eax

	push	offset _hWndCls
	call	_imp__RegisterClassA@4

	push	ebx
	push	400000h
	push	ebx
	push	ebx
	push	79h
	push	0192h
	push	SM_CYSCREEN
	call	_imp__GetSystemMetrics@4
	shr	eax,	01h
	sub	eax,	4bh
	push	eax
	push	SM_CXSCREEN
	call	_imp__GetSystemMetrics@4
	shr	eax,	01h
	sub	eax,	00c9h
	push	eax
	push	dword ptr [_dWndStlEx]
	push	offset _sWinName
	push	offset _sWJ
	push	ebx
	call	_imp__CreateWindowExA@48
	mov	_hWnd,	eax
	
	push	SW_SHOWNORMAL or SW_RESTORE
	push	eax
	call	_imp__ShowWindow@8
	;;----------------
	
	cmp	dword ptr [_dMapProcCode],	00h
	jz	_lExitButton

	;;----------------
	;; create progress bar
	push	ebx ;; 02h	;; control id
	push	dword ptr [_hWnd]
	push	12h
	push	016ah
	push	3eh
	push	10h
	push	WS_CHILD or WS_VISIBLE or PBS_SMOOTH
	push	ebx
	push	offset _sProgBar
	push	ebx
	call	_imp__CreateWindowExA@48
	mov	dword ptr [_hPrg],	eax
	;;----------------

	;;----------------
	;; create thread
	push	offset _hTrd
	push	ebx
	push	ebx
	push	offset _lOpenMapCode
	push	0800h
	push	ebx
	call	_imp__CreateThread@24

	push	eax
	call	_imp__CloseHandle@4
	;;----------------

	;;----------------
	;; message loop
	_lWndLS:
	push	ebx
	push	ebx
	push	ebx
	push	offset _hWndCls
	call	_imp__GetMessageA@16
	push	offset _hWndCls
	call	_imp__DispatchMessageA@4
	jmp	_lWndLS
	;;----------------

	;;----------------
	;; add exit button
	_lExitButton:
	push	ebx
	push	dword ptr [_hWnd]
	push	16h
	push	72h
	push	3ch
	push	90h	
	push	WS_CHILD or WS_VISIBLE
	push	offset _sExit
	push	offset _sButton
	push	ebx
	call	_imp__CreateWindowExA@48

	push	eax		;; ---> _imp__SetFocus

	push	ebx
	push	dword ptr [_dGuiFont]
	push	WM_SETFONT
	push	eax

	call	_imp__SendMessageA@16

	call	_imp__SetFocus@4

	jmp	_lWndLS
	;;----------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; main endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc wnd
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_hWndProc:
	mov	eax,			dword ptr [esp+08h]

	cmp	eax,			WM_PAINT
	je	_wmPaint

	cmp	eax,			WM_PROCEND
	je	_wmEnd

	cmp	eax,			WM_CJ_ERROR
	je	_wmErrS

	jmp	_imp__DefWindowProcA@16

	_wmEnd:
	push	00h
	call	_imp__ExitProcess@4
	ret	10h

	_wmPaint:
	push	offset _xPntStr
	push	dword ptr [esp+08h]

	push	dword ptr [esp+04h]
	push	dword ptr [esp+04h]

	call	_imp__BeginPaint@8
	mov	ebx,	eax

	push	dword ptr [_dGuiFont]
	push	ebx
	call	_imp__SelectObject@8

	push	TRANSPARENT
	push	ebx
	call	_imp__SetBkMode@8

	push	DT_LEFT or DT_SINGLELINE or DT_PATH_ELLIPSIS
	push	offset _xRect_01
	push	0ffffffffh
	push	_dCurrStr
	push	ebx
	call	_imp__DrawTextA@20

	push	DT_LEFT or DT_SINGLELINE or DT_PATH_ELLIPSIS
	push	offset _xRect_00
	push	0ffffffffh
	push	offset _sMapPath
	push	ebx
	call	_imp__DrawTextA@20

	call	_imp__EndPaint@8
	ret	10h

	_wmErrS:
	push	dword ptr [_hWnd]
	call	_imp__DestroyWindow@4
	ret	10h

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; wnd endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc infownd
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	_dSiteAdrXM	equ	0174h
	_dSiteArdXN	equ	0118h ;;0162h-50h
	_dSiteAdrYM	equ	1ch
	_dSiteAdrYN	equ	10h
	
	_lInfoStart:
	mov	eax,			dword ptr [esp+08h]

	cmp	eax,			WM_MOUSEMOVE
	je	_iwmMove

	cmp	eax,			WM_PAINT
	je	_iwmPaint

	cmp	eax,			WM_LBUTTONDOWN
	je	_iwmClick

	cmp	eax,			WM_COMMAND
	je	_iwmClose

;;	cmp	eax,			WM_KEYDOWN
;;	je	_iwmKey

	cmp	eax,			WM_DESTROY
	je	_iwmClose

	jmp	_imp__DefWindowProcA@16

	_iwmClose:
	push	00h
	call	_imp__ExitProcess@4

;;	_iwmKey:
;;	cmp	dword ptr [esp+0ch],	1bh	;; esc
;;	je	_iwmClose
;;	cmp	dword ptr [esp+0ch],	0dh	;; enter
;;	je	_iwmClose
;;	retn	10h

	_iwmMove:
	mov	eax,			dword ptr [esp+10h]

	cmp	ax,			_dSiteAdrXM
	jg	_next
	cmp	ax,			_dSiteArdXN
	jb	_next
	
	shr	eax,			10h
	
	cmp	al,			_dSiteAdrYN
	jb	_next
	cmp	al,			_dSiteAdrYM
	jg	_next
	
	cmp	dword ptr [_dBuffer],	00ff4040h
	je	_iwmRet
	mov	dword ptr [_dBuffer],	00ff4040h
	push	dword ptr [_dExCursor]
	jmp	_iwmReDraw
	_iwmRet:
	retn	10h
	
	_lbl:	
	cmp	dword ptr [_dBuffer],	00ff0000h
	je	_iwmRet
	mov	dword ptr [_dBuffer],	00ff0000h
	push	dword ptr [_dStdCursor]
	_iwmReDraw:
	call	_imp__SetCursor@4
	
	push	RDW_INVALIDATE or RDW_ERASE
	push	00h
	push	offset _xRect_toRedraw
	push	_hWnd
	call	_imp__RedrawWindow@16

	retn	10h
	
	_iwmPaint:
	push	offset _xPntStr
	push	dword ptr [esp+08h]

	push	dword ptr [esp+04h]
	push	dword ptr [esp+04h]

	call	_imp__BeginPaint@8
	mov	ebx,	eax
	
;;	push	dword ptr [_hIconCJ]
;;	push	0034h
;;	push	0138h
;;	push	ebx	
;;	call	_imp__DrawIcon@16

;;	push	dword ptr [_hIconCJ]
;;	push	34h
;;	push	40h
;;	push	ebx	
;;	call	_imp__DrawIcon@16

	push	dword ptr [_dGuiFont]
	push	ebx
	call	_imp__SelectObject@8

	push	TRANSPARENT
	push	ebx
	call	_imp__SetBkMode@8

	push	DT_LEFT
	push	offset _xRect_00
	push	0ffffffffh
	push	offset _sTollInfo
	push	ebx
	call	_imp__DrawTextA@20
	
	push	dword ptr [_dBuffer]
	push	ebx
	call	_imp__SetTextColor@8

	push	dword ptr [_dWWWFont]
	push	ebx
	call	_imp__SelectObject@8
	
	push	DT_RIGHT
	push	offset _xRect_00
	push	0ffffffffh
	push	offset _sSiteAdr
	push	ebx
	call	_imp__DrawTextA@20

	call	_imp__EndPaint@8
	retn	10h
	
	_iwmClick:
	mov	eax,			dword ptr [esp+10h]
	
	cmp	ax,			_dSiteAdrXM
	jg	_next
	cmp	ax,			_dSiteArdXN
	jb	_next
	
	shr	eax,			10h
	
	cmp	al,			_dSiteAdrYN
	jb	_next
	cmp	al,			_dSiteAdrYM
	jg	_next
	
	push	00h
	push	00h
	push	00h
	push	offset _sSiteAdr
	push	offset _sOpen
	push	_hWnd
	call	_imp__ShellExecuteA@24
	
	_lbl:
	retn	10h	
	
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; infownd endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; set progress proc
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_lSetProg:
	push	00h
	push	eax
	push	PBM_SETPOS
	push	_hPrg
	call	_imp__SendMessageA@16
	
	_lRedraw:	
	push	RDW_INVALIDATE or RDW_ERASE
	push	00h
	push	00h
	push	_hWnd
	call	_imp__RedrawWindow@16

	retn

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; set progress endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;-------------------------------------------------------------------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc error
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_lErrIn:
	xor	ebx,			ebx
	mov	esi,			dword ptr [_imp__SendMessageA@16]

		;;----------------
		;; destroy old window
		push	ebx
		push	ebx
		push	WM_CJ_ERROR
		push	dword ptr [_hWnd]
		call	esi			;; _imp__SendMessageA@16
		;;----------------

		;;----------------
		;; beep
		push	0ffffffffh
		call	_imp__MessageBeep@4
		;;----------------

		;;----------------
		;; create new window
		push	offset _hErrCls
		call	_imp__RegisterClassA@4

		push	ebx
		push	400000h
		push	ebx
		push	ebx
		push	0300h		;; height
		push	0400h		;; width
		push	SM_CYSCREEN
		call	_imp__GetSystemMetrics@4
		shr	eax,		01h
		sub	eax,		0180h
		push	eax
		push	SM_CXSCREEN
		call	_imp__GetSystemMetrics@4
		shr	eax,		01h
		sub	eax,		0200h
		push	eax
		push	WS_VISIBLE or WS_OVERLAPPEDWINDOW
		push	offset _sSynErr
		push	dword ptr [esp]
		push	ebx
		call	_imp__CreateWindowExA@48
		mov	_hWnd,		eax
		;;----------------

;;	push	SW_SHOWNORMAL or SW_RESTORE
;;	push	eax
;;	call	_imp__ShowWindow@8

		;;----------------
		;; edit
		push	ebx
		push	400000h
		push	ebx	;; control id
		push	eax	;; own window
		push	0210h
		push	03d5h
		push	20h
		push	10h
		push	WS_CHILD or WS_VISIBLE or ES_LEFT or ES_MULTILINE or ES_READONLY or WS_HSCROLL or WS_VSCROLL or ES_NOHIDESEL
		push	ebx
		push	offset _sEditWnd
		push	WS_EX_CLIENTEDGE
		call	_imp__CreateWindowExA@48
		mov	dword ptr [_hPrg],	eax

		push	ebx
		push	offset _xOutFont
		call	_imp__CreateFontIndirectA@4
		push	eax
		push	eax
		call	_imp__CloseHandle@4
		push	WM_SETFONT
		push	dword ptr [_hPrg]
		call	esi			;; _imp__SendMessageA@16
		;;----------------

		;;----------------
		;; list
		push	ebx
		push	400000h
		push	ebx
		push	dword ptr [_hWnd]
		push	90h
		push	0280h
		push	0240h
		push	10h
		push	WS_CHILD or WS_VISIBLE or WS_TABSTOP or LBS_NOINTEGRALHEIGHT or LBS_HASSTRINGS or LBS_NOTIFY
		push	ebx
		push	offset _sListWnd
		push	WS_EX_CLIENTEDGE
		call	_imp__CreateWindowExA@48
		mov	dword ptr [_hList],	eax

		push	ebx
		push	dword ptr [_dGuiFont]
		push	WM_SETFONT
		push	eax
		call	esi			;; _imp__SendMessageA@16
		;;----------------

		;;----------------
		;; exit button
		push	ebx
		push	dword ptr [_hWnd]
		push	16h
		push	72h
		push	02bah ;; - 46h
		push	0373h ;; - 62h
		push	WS_CHILD or WS_VISIBLE
		push	offset _sExit
		push	offset _sButton
		push	ebx
		call	_imp__CreateWindowExA@48
		mov	dword ptr [_hBtn],	eax

		push	ebx
		push	dword ptr [_dGuiFont]
		push	WM_SETFONT
		push	eax
		call	esi			;; _imp__SendMessageA@16
		;;----------------

		;;----------------
		;; veiw code
		mov	ecx,			dword ptr [_dErrorCodeStart]

		push	ecx
		push	dword ptr [_hPrg]
		jmp	_lErrCStart

			;;----------------
			;; process code
			_lErrCStartEX:
			inc	ecx

			_lErrCStart:
			mov	eax,			dword ptr [ecx]

				;;----------------
				;; add stdef
				cmp	al,			08h
				jne	_next

				mov	dword ptr [ecx],	64746573h	;; setd
				mov	word ptr [ecx+04h],	6665h		;; ef
				add	ecx,			06h
				jmp	_lErrCStart
				;;----------------

				;;----------------
				;; add backspaces
				_lbl:
				cmp	al,			06h
				jne	_next

				_lErrCAddBS:
				mov	byte ptr [ecx],		20h
				inc	ecx
				jmp	_lErrCStart

				_lbl:
				cmp	al,			07h
				je	_lErrCAddBS
				cmp	al,			05h
				je	_lErrCAddBS
				cmp	al,			04h
				je	_lErrCAddBS
				cmp	al,			03h
				je	_lSRNL
				cmp	al,			02h
				je	_lErrCAddBS
				;;----------------

				;;----------------
				;; blocks
				cmp	ax,			7801h		;; #x
				jne	_next
				mov	word ptr [ecx],		0a0dh		;; nl
				mov	dword ptr [ecx+02h],	0a0d207bh	;; {_nl
				add	ecx,			06h
				jmp	_lErrCStart

				_lbl:
				cmp	ax,			7901h		;; #x
				jne	_next
				_lSRNLEX:
				mov	word ptr [ecx],		207dh		;; }_
				mov	dword ptr [ecx+02h],	20202020h	;; ____
				add	ecx,			06h
				jmp	_lErrCStart
				;;----------------

				;;----------------
				;; add nl
				_lSRNL:
				cmp	word ptr [ecx-02h],	0a0dh
				je	_lSRNLEX
				mov	byte ptr [ecx],		0dh
				jmp	_lErrCStartEX
				;;----------------

				;;----------------
				;; error
				_lbl:
				cmp	ax,			6701h		;; #g
				jne	_next

				mov	dword ptr [ecx],	72726523h	;; #err
				add	ecx,			04h
				jmp	_lErrCStart
				;;----------------

				;;----------------
				;; #
				_lbl:
				cmp	al,			01h
				jne	_next

					;;----------------
					;; single lined definr
					cmp	ax,			6301h		;; #c
					jne	_lErrSSX_00

					cmp	eax,			6d756301h	;; #cum
					jne	_lErrDefEX

					mov	word ptr [ecx],		6e65h		;; en
					add	ecx,			02h
					jmp	_lErrCStart

					_lErrDefEX:
					mov	word ptr [ecx],		6564h		;; de
					add	ecx,			02h
					jmp	_lErrCStart
					;;----------------

					;;----------------
					;; multilined define
					_lErrSSX_00:
					cmp	eax,			6d756401h	;; #dum
					jne	_lErrSSX_02

					mov	word ptr [ecx],		6e65h		;; en
					add	ecx,			04h
					jmp	_lErrCStart

					_lErrSSX_02:
					cmp	ax,			6401h		;; #d
					jne	_lErrSSX_01

					cmp	dword ptr [ecx+02h],	656e6966h	;; fine
					jne	_lErrSSX_00_EX

					mov	word ptr [ecx],		6564h		;; de
					add	ecx,			02h
					jmp	_lErrCStart

					_lErrSSX_00_EX:
					mov	word ptr [ecx],		0a0dh		;; nl
					add	ecx,			02h
					jmp	_lErrCStart
					;;----------------

				_lErrSSX_01:
				mov	word ptr [ecx],		2020h		;; __
				add	ecx,			02h
				jmp	_lErrCStart
				;;----------------

				;;----------------
				;; null
				_lbl:
				cmp	al,			00h		;; 00h
				jne	_lErrCStartEX
				;;----------------

				;;----------------
				;; add info
				;;mov	dword ptr [ecx],	2a2a2a20h	;; _***
				;;----------------
			;;----------------
		call	_imp__SetWindowTextA@8
		;;----------------

		;;----------------
		;; select first error
		mov	eax,				_dErrorCodeStart
		sub	dword ptr [_xErrorTable+08h],	eax
		sub	dword ptr [_xErrorTable+04h],	eax
		sub	dword ptr [_xErrorTable+18h],	eax
		sub	dword ptr [_xErrorTable+14h],	eax

		push	dword ptr [_xErrorTable+08h]
		push	dword ptr [_xErrorTable+04h]
		push	EM_SETSEL
		push	dword ptr [_hPrg]
		call	esi			;; _imp__SendMessageA@16

		push	ebx
		push	ebx
		push	EM_SCROLLCARET
		push	dword ptr [_hPrg]
		call	esi			;; _imp__SendMessageA@16

		push	dword ptr [_xErrorTable]
		push	ebx
		push	LB_ADDSTRING
		push	dword ptr [_hList]
		call	esi			;; _imp__SendMessageA@16

		cmp	dword ptr [_xErrorTable+10h],	00h
		je	_next
		push	dword ptr [_xErrorTable+10h]
		push	ebx
		push	LB_ADDSTRING
		push	dword ptr [_hList]
		call	esi			;; _imp__SendMessageA@16
		_lbl:
		;;----------------

	_lErrLS:
	push	ebx
	push	ebx
	push	ebx
	push	offset _hErrCls
	call	_imp__GetMessageA@16
	push	offset _hErrCls
	call	_imp__DispatchMessageA@4
	jmp	_lErrLS

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; error endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; proc wnd_err
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	_hErrProc:
	mov	eax,			dword ptr [esp+08h]

	cmp	eax,			WM_SIZE
	je	_ewmSize

	cmp	eax,			WM_SIZING
	je	_ewmCSize

	cmp	eax,			WM_PAINT
	je	_ewmPaint

	cmp	eax,			WM_COMMAND
	je	_ewmCmd

	cmp	eax,			WM_DESTROY
	je	_ewmClose

	jmp	_imp__DefWindowProcA@16

		;;----------------
		;; close
		_ewmClose:
		push	01h
		call	_imp__ExitProcess@4
		;;----------------

		;;----------------
		;; command
		_ewmCmd:
		mov	ebx,			dword ptr [esp+0ch]
		shr	ebx,			10h
		cmp	ebx,			BN_CLICKED
		je	_ewmClose
		cmp	ebx,			LBN_SELCHANGE
		jne	_next

		mov	esi,			_imp__SendMessageA@16

		push	00h
		push	00h
		push	LB_GETCURSEL
		push	dword ptr [_hList]
		call	esi			;; _imp__SendMessageA@16

		shl	eax,			04h

		push	dword ptr [_xErrorTable+08h+eax]
		push	dword ptr [_xErrorTable+04h+eax]
		push	EM_SETSEL
		push	dword ptr [_hPrg]
		call	esi			;; _imp__SendMessageA@16

		push	ebx
		push	ebx
		push	EM_SCROLLCARET
		push	dword ptr [_hPrg]
		call	esi			;; _imp__SendMessageA@16

		_lbl:
		retn	10h
		;;----------------

		;;----------------
		;; sizing
		_ewmCSize:
		mov	ecx,			dword ptr [esp+10h]
		mov	ebx,			dword ptr [esp+0ch]

		dec	ebx			;; WMSZ_LEFT
		jnz	_next
		call	_ewmCSizeLeft
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_RIGHT
		jnz	_next
		call	_ewmCSizeRight
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_TOP
		jnz	_next
		call	_ewmCSizeTop
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_TOPLEFT
		jnz	_next
		call	_ewmCSizeTop
		call	_ewmCSizeLeft
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_TOPRIGHT
		jnz	_next
		call	_ewmCSizeTop
		call	_ewmCSizeRight
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_BOTTOM
		jnz	_next
		call	_ewmCSizeBottom
		retn	10h

		_lbl:
		dec	ebx			;; WMSZ_BOTTOMLEFT
		jnz	_next
		call	_ewmCSizeBottom
		call	_ewmCSizeLeft
		retn	10h

		_lbl:
;;		dec	edx			;; WMSZ_BOTTOMRIGHT
;;		jnz	_next
		call	_ewmCSizeBottom
		call	_ewmCSizeRight
		retn	10h

		_ewmCSizeBottom:
		mov	edi,			dword ptr [ecx+0ch]
		mov	esi,			dword ptr [ecx+04h]
		sub	edi,			esi
		cmp	edi,			0300h
		jg	_next
		add	esi,			0300h
		mov	dword ptr [ecx+0ch],	esi
		_lbl:
		retn

		_ewmCSizeRight:
		mov	edi,			dword ptr [ecx+08h]
		mov	esi,			dword ptr [ecx]
		sub	edi,			esi
		cmp	edi,			0400h
		jg	_next
		add	esi,			0400h
		mov	dword ptr [ecx+08h],	esi
		_lbl:
		retn

		_ewmCSizeTop:
		mov	edi,			dword ptr [ecx+0ch]
		mov	esi,			dword ptr [ecx+04h]
		sub	edi,			esi
		cmp	edi,			0300h
		jg	_next
		mov	edi,			dword ptr [ecx+0ch]
		sub	edi,			0300h
		mov	dword ptr [ecx+04h],	edi
		_lbl:
		retn

		_ewmCSizeLeft:
		mov	edi,			dword ptr [ecx+08h]
		mov	esi,			dword ptr [ecx]
		sub	edi,			esi
		cmp	edi,			0400h
		jg	_next
		mov	edi,			dword ptr [ecx+08h]
		sub	edi,			0400h
		mov	dword ptr [ecx],	edi
		_lbl:
		retn
		;;----------------

		;;----------------
		;; paint
		_ewmPaint:
		push	offset _xPntStr
		push	dword ptr [esp+08h]

		push	dword ptr [esp+04h]
		push	dword ptr [esp+04h]

		call	_imp__BeginPaint@8
		mov	ebx,			eax

		push	dword ptr [_dGuiFont]
		push	ebx
		call	_imp__SelectObject@8

		push	TRANSPARENT
		push	ebx
		call	_imp__SetBkMode@8

		push	DT_LEFT
		push	offset _xRect_02
		push	0ffffffffh
		push	offset _sErr_Title
		push	ebx
		call	_imp__DrawTextA@20

		call	_imp__EndPaint@8
		retn	10h
		;;----------------

		;;----------------
		;; size
		_ewmSize:
		cmp	dword ptr [esp+0ch],	SIZE_MINIMIZED	
		je	_ewmReSize

		mov	edi,			dword ptr [esp+10h]
		mov	esi,			edi
		and	edi,			0000ffffh	;; x
		shr	esi,			10h		;; y
		sub	edi,			20h
		sub	esi,			00d7h

		push	01h
		push	esi
		push	edi
		push	20h
		push	10h
		push	dword ptr [_hPrg]
		call	_imp__MoveWindow@24	;; move edit box

		push	01h
		push	90h
		push	0280h
		add	esi,				30h
		push	esi
		push	10h
		push	dword ptr [_hList]
		call	_imp__MoveWindow@24	;; move list box

		push	01h
		push	16h
		push	72h
		add	esi,				7ah
		push	esi
		sub	edi,				62h
		push	edi
		push	dword ptr [_hBtn]
		call	_imp__MoveWindow@24	;; move button

		_ewmReSize:
		retn	10h
		;;----------------

	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	;; wnd_err endp
	;;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	end start
;;-------------------------------------------------------------------------