#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <boost/format.hpp>
#include <boost/signals2.hpp>
#include "../../Core/YDWECommon/InlineHook.h"
#include "../../Core/YDWECommon/PEMemoryFileInfo.h"
#include "../../Core/YDWECommon/MemoryPatternSearch.h"

#include "DllMain.h"
#include "BuiltinJAPI.h"
#include "YDJAPI.h"

static void (__fastcall *gpfRegisterNative)(void *pfNativeFunction, const char *nativeName, const char *nativeSignature);
static boost::signals2::signal<void ()> gCallbackSignal;

/*
.text:6F455C20                      ; =============== S U B R O U T I N E =======================================
.text:6F455C20
.text:6F455C20
.text:6F455C20                      sub_6F455C20    proc near               ; CODE XREF: sub_6F2E5270-9B1 p
.text:6F455C20                                                              ; sub_6F2E5270-99D p ...
.text:6F455C20
.text:6F455C20                      var_10C         = dword ptr -10Ch
.text:6F455C20                      var_108         = byte ptr -108h
.text:6F455C20                      var_4           = dword ptr -4
.text:6F455C20                      arg_0           = dword ptr  4
.text:6F455C20
.text:6F455C20 81 EC 0C 01 00 00                    sub     esp, 10Ch
.text:6F455C26 A1 60 41 AC 6F                       mov     eax, dword_6FAC4160
.text:6F455C2B 33 C4                                xor     eax, esp
.text:6F455C2D 89 84 24 08 01 00 00                 mov     [esp+10Ch+var_4], eax
.text:6F455C34 56                                   push    esi
.text:6F455C35 8B B4 24 14 01 00 00                 mov     esi, [esp+110h+arg_0]
.text:6F455C3C 57                                   push    edi
.text:6F455C3D 8B FA                                mov     edi, edx
.text:6F455C3F 56                                   push    esi
.text:6F455C40 51                                   push    ecx
.text:6F455C41 8D 54 24 10                          lea     edx, [esp+11Ch+var_10C]
.text:6F455C45 8B CF                                mov     ecx, edi
.text:6F455C47 C7 44 24 10 00 00 00+                mov     [esp+11Ch+var_10C], 0
.text:6F455C4F E8 7C FF FF FF                       call    sub_6F455BD0
.text:6F455C54 8B 54 24 08                          mov     edx, [esp+114h+var_10C]
.text:6F455C58 8D 46 01                             lea     eax, [esi+1]
.text:6F455C5B C7 42 20 00 00 00 00                 mov     dword ptr [edx+20h], 0
.text:6F455C62 8A 08                                mov     cl, [eax]
.text:6F455C64 80 F9 29                             cmp     cl, 29h
.text:6F455C67 74 32                                jz      short loc_6F455C9B
.text:6F455C69 BE 01 00 00 00                       mov     esi, 1
.text:6F455C6E 8B FF                                mov     edi, edi
*/
static uint16_t gRegisterNativePattern[] = 
{
	0xFF81, 0xFFEC, 0x000C, 0x0001, 0x0000, 0x0000, 
	0xFFA1, 0x0060, 0x0041, 0x00AC, 0x006F,
	0xFF33, 0xFFC4, 
	0xFF89, 0xFF84, 0xFF24, 0x0008, 0x0001, 0x0000, 0x0000,
	0xFF56,
	0xFF8B, 0xFFB4, 0xFF24, 0x0014, 0x0001, 0x0000, 0x0000,
	0xFF57, 
	0xFF8B, 0xFFFA,
	0xFF56,
	0xFF51,
	0xFF8D, 0xFF54, 0xFF24, 0x0010,
	0xFF8B, 0xFFCF,
	0xFFC7, 0xFF44, 0xFF24, 0x0010, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
	0xFFE8, 0x007C, 0x00FF, 0xFFFF, 0xFFFF
};

static void __fastcall MyRegisterNative(void *pfNativeFunction, const char *nativeName, const char *nativeSignature)
{
	if (nativeName && nativeSignature && strcmp(nativeName, "Preloader") == 0 && strcmp(nativeSignature, "(S)V") == 0)
	{
		LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, "Callbacks fireing!");
		gCallbackSignal();
	}

	return gpfRegisterNative(pfNativeFunction, nativeName, nativeSignature);
}

/*
.text:6F3BB560                   sub_6F3BB560    proc near               ; CODE XREF: sub_6F3BB5E0+36p
.text:6F3BB560                                                           ; sub_6F3BB630+2Bp ...
.text:6F3BB560 8B C1                             mov     eax, ecx
.text:6F3BB562 85 C0                             test    eax, eax
.text:6F3BB564 8B 0D 4C D4 AC 6F                 mov     ecx, dword_6FACD44C
.text:6F3BB56A 75 01                             jnz     short loc_6F3BB56D
.text:6F3BB56C C3                                retn
.text:6F3BB56D                   ; ---------------------------------------------------------------------------
.text:6F3BB56D
.text:6F3BB56D                   loc_6F3BB56D:                           ; CODE XREF: sub_6F3BB560+Aj
.text:6F3BB56D 50                                push    eax
.text:6F3BB56E E8 BD 4B FE FF                    call    sub_6F3A0130
.text:6F3BB573 C3                                retn
.text:6F3BB573                   sub_6F3BB560    endp
 */
static uint16_t gMapStringToInternalObjectPattern[] = 
{
	0xFF8B, 0xFFC1,
	0xFF85, 0xFFC0,
	0xFF8B, 0xFF0D, 0x004C, 0x00D4, 0x00AC, 0x006F,
	0xFF75, 0xFF01,
	0xFFC3,
	0xFF50,
	0xFFE8, 0x00BD, 0x004B, 0xF0FE, 0xFFFF,
	0xFFC3
};

static int32_t (__fastcall *gpfMapStringToInternalObject)(const char *string);

int32_t MapStringToInternalObject(const char *string)
{
	return gpfMapStringToInternalObject(string);
}

BOOL InitJAPIFramework(HMODULE gameDllModule)
{
	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, "Begin initialize JAPI framework.");

	NYDWE::CPEMemoryFileInfo info(gameDllModule);
	auto result = info.querySection(".text");
	if (!result)
	{
		LOG4CXX_ERROR(NYDWE::gJAPIFrameworkLogger, "Cannot get .text section information from game.dll");
		return FALSE;
	}

	void *pGameDllTextBase = (void *)result->get<0>();
	size_t gameDllTextLength = result->get<1>();

	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger,
		boost::format("Game.dll .text section base: %1%, length: %2%") % pGameDllTextBase % gameDllTextLength);

	gpfRegisterNative = (decltype(gpfRegisterNative))
		NYDWE::MemoryPatternSearch(pGameDllTextBase, gameDllTextLength, &gRegisterNativePattern[0], sizeof(gRegisterNativePattern));
	if (!gpfRegisterNative)
	{
		LOG4CXX_ERROR(NYDWE::gJAPIFrameworkLogger, "Cannot find RegisterNative!");
		return FALSE;
	}

	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, boost::format("RegisterNative found at %1%.") % gpfRegisterNative);

	gpfMapStringToInternalObject = (decltype(gpfMapStringToInternalObject))
		NYDWE::MemoryPatternSearch(pGameDllTextBase, gameDllTextLength,
			&gMapStringToInternalObjectPattern[0], sizeof(gMapStringToInternalObjectPattern));
	if (!gpfMapStringToInternalObject)
	{
		LOG4CXX_ERROR(NYDWE::gJAPIFrameworkLogger, "Cannot find MapStringToInternalObject!");
		return FALSE;
	}

	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, boost::format("MapStringToInternalObject found at %1%.") % gpfMapStringToInternalObject);

	if (!NYDWE::InlineHookInstall((void **)&gpfRegisterNative, MyRegisterNative))
	{
		LOG4CXX_ERROR(NYDWE::gJAPIFrameworkLogger, "Cannot hook RegisterNative!");
		return FALSE;
	}

	RegisterJAPICallback(NYDWE::RegisterBuiltinJAPI);

	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, "Initialize JAPI framework completed.");
	return TRUE;
}

void RegisterJAPICallback(void (*pfCallback)())
{
	gCallbackSignal.connect(pfCallback);
	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, boost::format("Callback %1% registered. Total slots: %2%.")
		% pfCallback % gCallbackSignal.num_slots());
}

void RegisterJAPINative(void *pfNativeFunction, const char *nativeName, const char *nativeSignature)
{
	gpfRegisterNative(pfNativeFunction, nativeName, nativeSignature);
	LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, boost::format("Registered native: %2%, signature: %3%, address: %1%")
		% pfNativeFunction % nativeName % nativeSignature);
}

const char *GetStringFromNativeParameter(int32_t stringObject)
{
	const char *result;

	if (!stringObject)
		return 0;

	int32_t v1 = *(uint32_t *)(stringObject + 8);
	if (v1)
		result = *(const char **)(v1 + 28);
	else
		result = 0;

	return result;
}

int JAPILog(int level, const char *format, ...)
{
	char logBuffer[8192];

	va_list vl;
	va_start(vl, format);
	int result = _vsnprintf_s(logBuffer, sizeof(logBuffer) / sizeof(logBuffer[0]), _TRUNCATE, format, vl);
	va_end(vl);

	switch (level)
	{
	case YDJAPI_LOG_TRACE:
		LOG4CXX_TRACE(NYDWE::gJAPIFrameworkLogger, logBuffer);
		break;
	case YDJAPI_LOG_DEBUG:
		LOG4CXX_DEBUG(NYDWE::gJAPIFrameworkLogger, logBuffer);
		break;
	case YDJAPI_LOG_INFO:
		LOG4CXX_INFO(NYDWE::gJAPIFrameworkLogger, logBuffer);
		break;
	case YDJAPI_LOG_ERROR:
		LOG4CXX_ERROR(NYDWE::gJAPIFrameworkLogger, logBuffer);
		break;
	case YDJAPI_LOG_FATAL:
		LOG4CXX_FATAL(NYDWE::gJAPIFrameworkLogger, logBuffer);
		break;
	default:
		OutputDebugStringA(logBuffer);
		break;
	}

	return result;
}

BOOL InstallInlineHook(void **ppOriginalFunction, void *pSubstitutionFunction)
{
	return NYDWE::InlineHookInstall(ppOriginalFunction, pSubstitutionFunction);
}

BOOL UninstallInlineHook(void **ppOriginalFunction, void *pSubstitutionFunction)
{
	return NYDWE::InlineHookUninstall(ppOriginalFunction, pSubstitutionFunction);
}
