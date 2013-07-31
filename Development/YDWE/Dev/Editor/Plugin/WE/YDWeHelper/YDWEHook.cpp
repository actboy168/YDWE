#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/format.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/scope_exit.hpp>
#include <aero/aero.hpp>
#include <ydwe/util/unicode.h>
#include <ydwe/hook/inline.h>
#include <ydwe/hook/iat_manager.h>

#include <objbase.h>
#include <windows.h>
#include <Shlobj.h>

#include "PEMemoryFileInfo.h"
#include "MemoryPatternSearch.h"
#include "MemoryPatch.h"

#include "YDWELogger.h"
#include "YDWEHook.h"

namespace NYDWE
{
namespace NGameHook
{

/** \brief Install patch
 *
 * Install patch
 *
 * \param searchStart Search start address
 * \param searchLength Length of the search
 * \param patchPattern Pattern of the code to search
 * \param patternLength Length of the pattern in bytes
 * \param patch Code used to patch
 * \param patchLength Length of the patch in bytes
 * \return Whether the patch succeeds.
 */
static bool InstallPatch(void *searchStart, boost::uint32_t searchLength, boost::uint16_t *patchPattern, boost::uint32_t patternLength, boost::uint8_t *patch, boost::uint32_t patchLength)
{
	bool result = false;

	void *patchPoint = MemoryPatternSearch(searchStart, searchLength, patchPattern, patternLength);
	if (patchPoint)
	{
		LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found patch point: 0x%1$08X") % reinterpret_cast<uintptr_t>(patchPoint));

		// Patch memory
		bool patchResult = MemoryPatchAndVerify(patchPoint, patch, patchLength);
		if (patchResult)
		{
			LOG4CXX_TRACE(NYDWE::gInjectLogger, "Patch memory success");
			result = true;
		}
		else
		{
			LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot make memory patch.");
		}
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot find patch point.");
	}

	return result;
}

/// WE .text section start address
static void *pgWeTextSectionBase;
/// WE .text section length
static uintptr_t gWeTextSectionLength;
/// WE .data section start address
static void *pgWeDataSectionBase;
/// WE .text section length
static uintptr_t gWeDataSectionLength;

/// Storm.dll .text section start address
static void *pgStormDllTextSectionBase;
/// Storm.dll .text section length
static uintptr_t gStormDllTextSectionLength;
/// Storm.dll .data section start address
static void *pgStormDllDataSectionBase;
/// Storm.dll .text section length
static uintptr_t gStormDllDataSectionLength;

/**
 * Initialize section infomation
 */
static void InitSectionInfo()
{
	// Do WE
	CPEMemoryFileInfo pm;
	CPEMemoryFileInfo::TSectionInfoResult result = pm.querySection(".text");
	if (result)
	{
		pgWeTextSectionBase = (void *)result->get<0>();
		gWeTextSectionLength = result->get<1>();

		LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("WE .text section start address: 0x%1$08X. length 0x%2$08X") % pgWeTextSectionBase % gWeTextSectionLength);
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot get .text section info of WE.");
	}

	result = pm.querySection(".data");
	if (result)
	{
		pgWeDataSectionBase = (void *)result->get<0>();
		gWeDataSectionLength = result->get<1>();

		LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("WE .data section start address: 0x%1$08X. length 0x%2$08X") % pgWeDataSectionBase % gWeDataSectionLength);
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot get .data section info of WE.");
	}

	// Do storm.dll
	pm.swap(CPEMemoryFileInfo(reinterpret_cast<HMODULE>(pgStormDllBase)));
	result = pm.querySection(".text");
	if (result)
	{
		pgStormDllTextSectionBase = (void *)result->get<0>();
		gStormDllTextSectionLength = result->get<1>();

		LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Storm.dll .text section start address: 0x%1$08X. length 0x%2$08X") % pgStormDllTextSectionBase % gStormDllTextSectionLength);
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot get .text section info of Storm.dll.");
	}

	result = pm.querySection(".data");
	if (result)
	{
		pgStormDllDataSectionBase = (void *)result->get<0>();
		gStormDllDataSectionLength = result->get<1>();

		LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Storm.dll .data section start address: 0x%1$08X. length 0x%2$08X") % pgStormDllDataSectionBase % gStormDllDataSectionLength);
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot get .data section info of Storm.dll.");
	}
}

static uintptr_t pgTrueWeSetWindowCaption;
static bool isWeSetWindowCaptionHookInstalled;
static boost::uint16_t weSetWindowCaptionPattern[] =
{
	0x56FF,
	0x8BFF, 0xF1FF,
	0x8BFF, 0xCAFF,
	0xE8FF, 0xC600, 0x6200, 0xFF00, 0xFFFF,
	0xA1FF, 0x8C00, 0x3C00, 0x8000, 0x00FF,
	0x50FF,
	0x56FF,
	0xFFFF, 0x15FF, 0xD800, 0x9600, 0x7500, 0x0000,
	0x5EFF,
	0xC3FF
};

BOOL __fastcall DetourWeSetWindowCaption(HWND hWnd, LPCSTR lpString)
{
	if ((!lpString) || ydwe::util::is_utf8(lpString))
	{
		return aero::fast_call<BOOL>(pgTrueWeSetWindowCaption, hWnd, lpString);
	}
	else
	{
		return SetWindowTextA(hWnd, lpString);
	}
}

static uintptr_t pgTrueWeUtf8ToAnsi;
static bool isWeUtf8ToAnsiHookInstalled;
BOOL __fastcall DetourWeUtf8ToAnsi(LPCSTR lpString)
{
	try
	{
		if ((lpString) && !ydwe::util::is_utf8(lpString))
		{
			std::string tmp = ydwe::util::a2u(lpString);
			return aero::fast_call<BOOL>(pgTrueWeUtf8ToAnsi, tmp.c_str());
		}
	}
	catch (...)
	{
	}

	return aero::fast_call<BOOL>(pgTrueWeUtf8ToAnsi, lpString);
}


template<class CharT>
class locale_helper
{
public:
	typedef CharT                        char_type;
	typedef std::basic_string<char_type> string_type;

	locale_helper()
		: locale_("")
		, collate_(std::use_facet<std::collate<char_type>>(locale_))
	{ }

	bool less(string_type const& a, string_type const& b) const
	{
		return compare(a, b) < 0;
	}

	int compare(string_type const& a, string_type const& b) const
	{
		return collate_.compare(
			a.data(), a.data() + a.size(), 
			b.data(), b.data() + b.size()
			);
	}

private:
	std::locale                    locale_;
	std::collate<char_type> const& collate_;
};

locale_helper<wchar_t> g_deflocale;

// 0x004D2D90
// 将WE的字符串比较改为根据locale来比较(中文环境下是根据拼音比较)
static uintptr_t pgTrueWeStringCompare;
static bool isWeStringCompareHookInstalled;
int __fastcall DetourWeStringCompare(const char* a, const char* b, bool ignore_case)
{
	if (!a || !b)
	{
		return aero::fast_call<bool>(pgTrueWeStringCompare, a, b, ignore_case);
	}

	try
	{
		std::string a_str = a;
		std::string b_str = b;

		if (ignore_case)
		{
			std::transform(a_str.begin(), a_str.end(), a_str.begin(), tolower);
			std::transform(b_str.begin(), b_str.end(), b_str.begin(), tolower);
		}

		return g_deflocale.compare(ydwe::util::u2w(a_str), ydwe::util::u2w(b_str));
	}
	catch (...)
	{
	}

	return aero::fast_call<bool>(pgTrueWeStringCompare, a, b, ignore_case);
}

template <typename charT>
HGLOBAL CreateGlobalData(const std::basic_string<charT>& str)
{
	HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, ((str.size() + 1) * sizeof(charT)));
	if (data) 
	{
		charT* raw_data = static_cast<charT*>(::GlobalLock(data));
		memcpy(raw_data, str.data(), str.size() * sizeof(charT));
		raw_data[str.size()] = '\0';
		::GlobalUnlock(data);
	}
	return data;
}

bool WriteText(std::wstring const& str) 
{
	bool result = false;
	HGLOBAL glob = CreateGlobalData(str);
	if (::OpenClipboard(::GetActiveWindow()))
	{
		if (::EmptyClipboard())
		{
			if (glob && !::SetClipboardData(CF_UNICODETEXT, glob)) 
			{
				result = (ERROR_CLIPBOARD_NOT_OPEN != ::GetLastError());
			}
		}
		::CloseClipboard();
	}
	::GlobalFree(glob);

	return result;
}

static uintptr_t pgTrueWeTriggerEditorEditboxCopy;
static bool isWeTriggerEditorEditboxCopyHookInstalled;
BOOL __fastcall DetourWeTriggerEditorEditboxCopy(const char *source)
{
	if (source)
	{
		try {
			return WriteText(ydwe::util::u2w(source));
		}
		catch (...) {			
		}
	}

	return aero::fast_call<BOOL>(pgTrueWeTriggerEditorEditboxCopy, source);
}


/// Original WeGetSystemParameter function pointer
static uintptr_t pgTrueWeGetSystemParameter;
/// Whether hook installed?
static bool isWeGetSystemParameterHookInstalled;
/*
.text:004D1DB0 55                                   push    ebp
.text:004D1DB1 8B EC                                mov     ebp, esp
.text:004D1DB3 51                                   push    ecx
.text:004D1DB4 8B C2                                mov     eax, edx
.text:004D1DB6 8B 55 08                             mov     edx, [ebp+flag]
.text:004D1DB9 52                                   push    edx
.text:004D1DBA 8D 55 FC                             lea     edx, [ebp+var_4]
.text:004D1DBD 52                                   push    edx
.text:004D1DBE 8B D1                                mov     edx, ecx
.text:004D1DC0 8B 0D 04 00 82 00                    mov     ecx, dword_820004
.text:004D1DC6 50                                   push    eax
.text:004D1DC7 C7 45 FC 00 00 00 00                 mov     [ebp+var_4], 0
.text:004D1DCE E8 7D 91 F3 FF                       call    sub_40AF50
.text:004D1DD3 8B 45 FC                             mov     eax, [ebp+var_4]
.text:004D1DD6 8B E5                                mov     esp, ebp
.text:004D1DD8 5D                                   pop     ebp
.text:004D1DD9 C2 04 00                             retn    4
.text:004D1DD9                      sub_4D1DB0      endp
.text:004D1DDC 90 90 90 90                          align 10h
.text:004D1DE0 55                                   push    ebp
.text:004D1DE1 8B EC                                mov     ebp, esp
.text:004D1DE3 83 EC 08                             sub     esp, 8
.text:004D1DE6 53                                   push    ebx
.text:004D1DE7 8B 5D 08                             mov     ebx, [ebp+childKey]
.text:004D1DEA 56                                   push    esi
.text:004D1DEB 57                                   push    edi
.text:004D1DEC 8B 7D 0C                             mov     edi, [ebp+flag]
 */
/// Pattern for finding WeGetSystemParameter
static boost::uint16_t weGetSystemParameterPattern[] =
{
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF51,
	0xFF8B, 0xFFC2,
	0xFF8B, 0xFF55, 0xFF08,
	0xFF52,
	0xFF8D, 0xFF55, 0xFFFC,
	0xFF52,
	0xFF8B, 0xFFD1,
	0xFF8B, 0xFF0D, 0x0004, 0x0000, 0x0082, 0x0000,
	0xFF50,
	0xFFC7, 0xFF45, 0xFFFC, 0xFF00, 0xFF00, 0xFF00, 0xFF00,
	0xFFE8, 0x007D, 0x0091, 0x00F3, 0x00FF,
	0xFF8B, 0xFF45, 0xFFFC,
	0xFF8B, 0xFFE5,
	0xFF5D,
	0xFFC2, 0xFF04, 0xFF00,
	0xFF90, 0xFF90, 0xFF90, 0xFF90,
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF83, 0xFFEC, 0xFF08,
	0xFF53
};

static boost::int32_t __fastcall DetourWeGetSystemParameter(LPCSTR parentKey, LPCSTR childKey, boost::int32_t flag)
{
	boost::int32_t result = aero::fast_call<boost::int32_t>(pgTrueWeGetSystemParameter, parentKey, childKey, flag);

	if (strcmp(parentKey, "WorldEditMisc") == 0)
	{
		if (strcmp(childKey, "MinimumMapSize") == 0)
		{
			result = 32;	// Minimum 32
		}
		else if (strcmp(childKey, "MaximumMapSize") == 0)
		{
			result = 480;	// Maximum 480
		}
	}

	return result;
}

/// Original WeVerifyMapCellsLimit function pointer
static uintptr_t pgTrueWeVerifyMapCellsLimit;
/// Whether hook installed?
static bool isWeVerifyMapCellsLimitHookInstalled;
/// Get unknown flag?
static void *pgMapCellsGetUnknownGlobalFlag;
/*
.text:004E1EF0                      sub_4E1EF0      proc near
.text:004E1EF0 83 F9 FF                             cmp     ecx, -1
.text:004E1EF3 75 07                                jnz     short loc_4E1EFC
.text:004E1EF5 E8 C6 EB FE FF                       call    sub_4D0AC0
.text:004E1EFA 8B C8                                mov     ecx, eax
.text:004E1EFC
.text:004E1EFC                      loc_4E1EFC:
.text:004E1EFC 83 F9 01                             cmp     ecx, 1
.text:004E1EFF B8 C0 00 00 00                       mov     eax, 192
.text:004E1F04 7C 05                                jl      short loc_4E1F0B
.text:004E1F06 B8 00 01 00 00                       mov     eax, 256
.text:004E1F0B
.text:004E1F0B                      loc_4E1F0B:
.text:004E1F0B 0F AF C0                             imul    eax, eax
.text:004E1F0E C3                                   retn
.text:004E1F0E                      sub_4E1EF0      endp
*/
static boost::uint16_t weVerifyMapCellsLimitPattern[] = 
{
	0xFF83, 0xFFF9, 0xFFFF, 
	0xFF75, 0xFF07, 
	0xFFE8, 0x00C6, 0x00EB, 0x00FE, 0x00FF, 
	0xFF8B, 0xFFC8, 
	0xFF83, 0xFFF9, 0xFF01, 
	0xFFB8, 0xFFC0, 0xFF00, 0xFF00, 0xFF00, 
	0xFF7C, 0xFF05, 
	0xFFB8, 0xFF00, 0xFF01, 0xFF00, 0xFF00, 
	0xFF0F, 0xFFAF, 0xFFC0, 
	0xFFC3
};

boost::scoped_ptr<CMemoryPatch> pgWeVerifyMapCellsLimitPatcher;

static boost::int32_t AERO_FP_CALL_FASTCALL DetourWeVerifyMapCellsLimit(boost::int32_t flag)
{
	boost::int32_t cellSideLimit;

	if (flag == -1)
	{
		flag = aero::fast_call<boost::int32_t>(pgMapCellsGetUnknownGlobalFlag);
	}

	if (flag >= 1)
	{
		cellSideLimit = 480;
	}
	else
	{
		cellSideLimit = 480 - 64;
	}

	return cellSideLimit * cellSideLimit;
}

/// Original WeTriggerNameCheck function pointer
static uintptr_t pgTrueWeTriggerNameCheck;
/// Whether hook installed?
static bool isWeTriggerNameCheckHookInstalled;
/*
.text:005A4B40                ; BOOL __fastcall trigger_name_validate(const char *trigger_name, BOOL allow_spaces)
.text:005A4B40                trigger_name_validate proc near         ; CODE XREF: sub_51EA10+18p
.text:005A4B40                                                        ; sub_51F970+14p ...
.text:005A4B40
.text:005A4B40                var_4           = dword ptr -4
.text:005A4B40
.text:005A4B40 55                             push    ebp
.text:005A4B41 8B EC                          mov     ebp, esp
.text:005A4B43 51                             push    ecx
.text:005A4B44 53                             push    ebx
.text:005A4B45 56                             push    esi
.text:005A4B46 57                             push    edi
.text:005A4B47 8B F9                          mov     edi, ecx
.text:005A4B49 8B F2                          mov     esi, edx
.text:005A4B4B 57                             push    edi
.text:005A4B4C 89 75 FC                       mov     [ebp+var_4], esi
.text:005A4B4F E8 94 04 12 00                 call    Storm_506
.text:005A4B54 8B D8                          mov     ebx, eax
.text:005A4B56 85 DB                          test    ebx, ebx
.text:005A4B58 75 07                          jnz     short loc_5A4B61
.text:005A4B5A 5F                             pop     edi
.text:005A4B5B 5E                             pop     esi
.text:005A4B5C 5B                             pop     ebx
.text:005A4B5D 8B E5                          mov     esp, ebp
.text:005A4B5F 5D                             pop     ebp
.text:005A4B60 C3                             retn
.text:005A4B61                ; ---------------------------------------------------------------------------
.text:005A4B61
.text:005A4B61                loc_5A4B61:                             ; CODE XREF: trigger_name_validate+18j
.text:005A4B61 8A 44 3B FF                    mov     al, [ebx+edi-1]
.text:005A4B65 3C 5F                          cmp     al, 5Fh
.text:005A4B67 75 09                          jnz     short loc_5A4B72
.text:005A4B69 5F                             pop     edi
.text:005A4B6A 5E                             pop     esi
.text:005A4B6B 33 C0                          xor     eax, eax
.text:005A4B6D 5B                             pop     ebx
.text:005A4B6E 8B E5                          mov     esp, ebp
.text:005A4B70 5D                             pop     ebp
.text:005A4B71 C3                             retn
.text:005A4B72                ; ---------------------------------------------------------------------------
.text:005A4B72
.text:005A4B72                loc_5A4B72:                             ; CODE XREF: trigger_name_validate+27j
.text:005A4B72 85 F6                          test    esi, esi
.text:005A4B74 74 0D                          jz      short loc_5A4B83
.text:005A4B76 3C 20                          cmp     al, 20h
.text:005A4B78 75 09                          jnz     short loc_5A4B83
.text:005A4B7A 5F                             pop     edi
.text:005A4B7B 5E                             pop     esi
.text:005A4B7C 33 C0                          xor     eax, eax
.text:005A4B7E 5B                             pop     ebx
.text:005A4B7F 8B E5                          mov     esp, ebp
.text:005A4B81 5D                             pop     ebp
.text:005A4B82 C3                             retn
.text:005A4B83                ; ---------------------------------------------------------------------------
.text:005A4B83
.text:005A4B83                loc_5A4B83:                             ; CODE XREF: trigger_name_validate+34j
.text:005A4B83                                                        ; trigger_name_validate+38j
.text:005A4B83 8A 07                          mov     al, [edi]
.text:005A4B85 3C 30                          cmp     al, 30h
.text:005A4B87 7C 0D                          jl      short loc_5A4B96
.text:005A4B89 3C 39                          cmp     al, 39h
.text:005A4B8B 7F 09                          jg      short loc_5A4B96
.text:005A4B8D
.text:005A4B8D                loc_5A4B8D:                             ; CODE XREF: trigger_name_validate+6Dj
.text:005A4B8D 5F                             pop     edi
.text:005A4B8E 5E                             pop     esi
.text:005A4B8F 33 C0                          xor     eax, eax
.text:005A4B91 5B                             pop     ebx
.text:005A4B92 8B E5                          mov     esp, ebp
.text:005A4B94 5D                             pop     ebp
.text:005A4B95 C3                             retn
.text:005A4B96                ; ---------------------------------------------------------------------------
.text:005A4B96
.text:005A4B96                loc_5A4B96:                             ; CODE XREF: trigger_name_validate+47j
.text:005A4B96                                                        ; trigger_name_validate+4Bj
.text:005A4B96 33 F6                          xor     esi, esi
.text:005A4B98 85 DB                          test    ebx, ebx
.text:005A4B9A 7E 18                          jle     short loc_5A4BB4
.text:005A4B9C 8D 64 24 00                    lea     esp, [esp+0]
.text:005A4BA0
.text:005A4BA0                loc_5A4BA0:                             ; CODE XREF: trigger_name_validate+72j
.text:005A4BA0 8B 55 FC                       mov     edx, [ebp+var_4]
.text:005A4BA3 8A 0C 3E                       mov     cl, [esi+edi]
.text:005A4BA6 E8 55 FF FF FF                 call    trigger_name_char_validate
.text:005A4BAB 85 C0                          test    eax, eax
.text:005A4BAD 74 DE                          jz      short loc_5A4B8D
.text:005A4BAF 46                             inc     esi
.text:005A4BB0 3B F3                          cmp     esi, ebx
.text:005A4BB2 7C EC                          jl      short loc_5A4BA0
.text:005A4BB4
.text:005A4BB4                loc_5A4BB4:                             ; CODE XREF: trigger_name_validate+5Aj
.text:005A4BB4 5F                             pop     edi
.text:005A4BB5 5E                             pop     esi
.text:005A4BB6 B8 01 00 00 00                 mov     eax, 1
.text:005A4BBB 5B                             pop     ebx
.text:005A4BBC 8B E5                          mov     esp, ebp
.text:005A4BBE 5D                             pop     ebp
.text:005A4BBF C3                             retn
.text:005A4BBF                trigger_name_validate endp
*/
/// Pattern for finding WeTriggerNameCheck
static boost::uint16_t weTriggerNameCheckPattern[] =
{
	0xFF55,
	0xFF8B, 0xFFEC,
	0xFF51, 0xFF53,	0xFF56, 0xFF57,
	0xFF8B, 0xFFF9,
	0xFF8B, 0xFFF2,
	0xFF57,
	0xFF89, 0xFF75, 0xFFFC,
	0xFFE8, 0x0094, 0x0004, 0x0012, 0x0000,
	0xFF8B, 0xFFD8,
	0xFF85, 0xFFDB,
	0xFF75, 0x0007,
	0xFF5F, 0xFF5E, 0xFF5B,
	0xFF8B, 0xFFE5,
	0xFF5D,
	0xFFC3
};

static BOOL AERO_FP_CALL_FASTCALL DetourWeTriggerNameCheck(const char *triggerName, BOOL allowSpaces)
{
	//BOOL result = aero::fast_call<BOOL>(pgTrueWeTriggerNameCheck, triggerName, allowSpaces);
	//return result;
	return triggerName && (*triggerName != 0);
}

/// Original WeTriggerNameInputCharCheck function pointer
static uintptr_t pgTrueWeTriggerNameInputCharCheck;
/// Whether hook installed?
static bool isWeTriggerNameInputCharCheckHookInstalled;
/*
.text:0042E390                ; BOOL __fastcall char_verify(unsigned __int32 c, unsigned __int32 flag)
.text:0042E390                char_verify     proc near               ; CODE XREF: sub_42E360+19 p
.text:0042E390                                                        ; char_message_verify+36p
.text:0042E390 83 F9 20                       cmp     ecx, 20h
.text:0042E393 73 06                          jnb     short loc_42E39B
.text:0042E395 B8 01 00 00 00                 mov     eax, 1
.text:0042E39A C3                             retn
.text:0042E39B                ; ---------------------------------------------------------------------------
.text:0042E39B
.text:0042E39B                loc_42E39B:                             ; CODE XREF: char_verify+3 j
.text:0042E39B F6 C2 01                       test    dl, 1
.text:0042E39E 74 20                          jz      short loc_42E3C0
.text:0042E3A0 83 F9 61                       cmp     ecx, 61h
.text:0042E3A3 72 0B                          jb      short loc_42E3B0
.text:0042E3A5 83 F9 7A                       cmp     ecx, 7Ah
.text:0042E3A8 77 06                          ja      short loc_42E3B0
.text:0042E3AA B8 01 00 00 00                 mov     eax, 1
.text:0042E3AF C3                             retn
.text:0042E3B0                ; ---------------------------------------------------------------------------
.text:0042E3B0
.text:0042E3B0                loc_42E3B0:                             ; CODE XREF: char_verify+13 j
.text:0042E3B0                                                        ; char_verify+18 j
.text:0042E3B0 83 F9 41                       cmp     ecx, 41h
.text:0042E3B3 72 0B                          jb      short loc_42E3C0
.text:0042E3B5 83 F9 5A                       cmp     ecx, 5Ah
.text:0042E3B8 77 06                          ja      short loc_42E3C0
.text:0042E3BA B8 01 00 00 00                 mov     eax, 1
.text:0042E3BF C3                             retn
.text:0042E3C0                ; ---------------------------------------------------------------------------
.text:0042E3C0
.text:0042E3C0                loc_42E3C0:                             ; CODE XREF: char_verify+E j
.text:0042E3C0                                                        ; char_verify+23 j ...
.text:0042E3C0 F6 C2 02                       test    dl, 2
.text:0042E3C3 74 10                          jz      short loc_42E3D5
.text:0042E3C5 83 F9 30                       cmp     ecx, 30h
.text:0042E3C8 72 0B                          jb      short loc_42E3D5
.text:0042E3CA 83 F9 39                       cmp     ecx, 39h
.text:0042E3CD 77 06                          ja      short loc_42E3D5
.text:0042E3CF B8 01 00 00 00                 mov     eax, 1
.text:0042E3D4 C3                             retn
.text:0042E3D5                ; ---------------------------------------------------------------------------
.text:0042E3D5
.text:0042E3D5                loc_42E3D5:                             ; CODE XREF: char_verify+33 j
.text:0042E3D5                                                        ; char_verify+38 j ...
.text:0042E3D5 F6 C2 04                       test    dl, 4
.text:0042E3D8 74 10                          jz      short loc_42E3EA
.text:0042E3DA 83 F9 2B                       cmp     ecx, 2Bh
.text:0042E3DD 74 05                          jz      short loc_42E3E4
.text:0042E3DF 83 F9 2D                       cmp     ecx, 2Dh
.text:0042E3E2 75 06                          jnz     short loc_42E3EA
.text:0042E3E4
.text:0042E3E4                loc_42E3E4:                             ; CODE XREF: char_verify+4D j
.text:0042E3E4 B8 01 00 00 00                 mov     eax, 1
.text:0042E3E9 C3                             retn
.text:0042E3EA                ; ---------------------------------------------------------------------------
.text:0042E3EA
.text:0042E3EA                loc_42E3EA:                             ; CODE XREF: char_verify+48 j
.text:0042E3EA                                                        ; char_verify+52 j
.text:0042E3EA F6 C2 08                       test    dl, 8
.text:0042E3ED 74 0B                          jz      short loc_42E3FA
.text:0042E3EF 83 F9 2E                       cmp     ecx, 2Eh
.text:0042E3F2 75 06                          jnz     short loc_42E3FA
.text:0042E3F4 B8 01 00 00 00                 mov     eax, 1
.text:0042E3F9 C3                             retn
.text:0042E3FA                ; ---------------------------------------------------------------------------
.text:0042E3FA
.text:0042E3FA                loc_42E3FA:                             ; CODE XREF: char_verify+5D j
.text:0042E3FA                                                        ; char_verify+62 j
.text:0042E3FA F6 C2 10                       test    dl, 10h
.text:0042E3FD 74 0B                          jz      short loc_42E40A
.text:0042E3FF 83 F9 20                       cmp     ecx, 20h
.text:0042E402 75 06                          jnz     short loc_42E40A
.text:0042E404 B8 01 00 00 00                 mov     eax, 1
.text:0042E409 C3                             retn
.text:0042E40A                ; ---------------------------------------------------------------------------
.text:0042E40A
.text:0042E40A                loc_42E40A:                             ; CODE XREF: char_verify+6D j
.text:0042E40A                                                        ; char_verify+72 j
.text:0042E40A F6 C2 20                       test    dl, 20h
.text:0042E40D 74 0B                          jz      short loc_42E41A
.text:0042E40F 83 F9 5F                       cmp     ecx, 5Fh
.text:0042E412 75 06                          jnz     short loc_42E41A
.text:0042E414 B8 01 00 00 00                 mov     eax, 1
.text:0042E419 C3                             retn
.text:0042E41A                ; ---------------------------------------------------------------------------
.text:0042E41A
.text:0042E41A                loc_42E41A:                             ; CODE XREF: char_verify+7D j
.text:0042E41A                                                        ; char_verify+82 j
.text:0042E41A F6 C2 40                       test    dl, 40h
.text:0042E41D 74 0B                          jz      short loc_42E42A
.text:0042E41F 83 F9 7F                       cmp     ecx, 7Fh
.text:0042E422 73 06                          jnb     short loc_42E42A
.text:0042E424 B8 01 00 00 00                 mov     eax, 1
.text:0042E429 C3                             retn
.text:0042E42A                ; ---------------------------------------------------------------------------
.text:0042E42A
.text:0042E42A                loc_42E42A:                             ; CODE XREF: char_verify+8D j
.text:0042E42A                                                        ; char_verify+92 j
.text:0042E42A 33 C0                          xor     eax, eax
.text:0042E42C C3                             retn
.text:0042E42C                char_verify     endp
 */
/// Pattern for finding WeTriggerNameInputCharCheck
static boost::uint16_t weTriggerNameInputCharCheckPattern[] =
{
	0xFF83, 0xFFF9, 0xFF20,
	0xFF73, 0x0006,
	0xFFB8, 0xFF01, 0xFF00, 0xFF00, 0xFF00,
	0xFFC3,
	0xFFF6, 0xFFC2, 0xFF01,
	0xFF74, 0x0020,
	0xFF83, 0xFFF9, 0xFF61,
	0xFF72, 0x000B,
	0xFF83, 0xFFF9, 0xFF7A,
	0xFF77, 0x0006,
	0xFFB8, 0xFF01, 0xFF00, 0xFF00, 0xFF00,
	0xFFC3
};

boost::scoped_ptr<CMemoryPatch> pgWeTriggerNameInputCharCheckPatcher;

static boost::uint32_t AERO_FP_CALL_FASTCALL DetourWeTriggerNameInputCharCheck(boost::uint32_t c, boost::uint32_t flag)
{
	//boost::uint32_t result = aero::fast_call<boost::uint32_t>(pgTrueWeTriggerNameInputCharCheck, c, flag);
	//return result;
	return 1;
}

#define INSTALL_INLINE_HOOK(name) \
	if (!is##name##HookInstalled) \
	{ \
		if (pgTrue##name##) \
		{ \
			is##name##HookInstalled = ydwe::hook::inline_install(&pgTrue##name##, (uintptr_t)Detour##name##); \
			if (is##name##HookInstalled) \
			{ \
				LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook installation succeeded."); \
			} \
			else \
			{ \
				LOG4CXX_ERROR(NYDWE::gInjectLogger, #name " hook installation failed: " << GetLastError()); \
			} \
		} \
		else \
		{ \
			LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot find " #name "!"); \
		} \
	} \
	else \
	{ \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook already installed."); \
	}

/** \brief Install inline hook
 * 
 * Install inline hook
 */
static void InitInlineHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Start installing inline hooks.");

	pgTrueWeGetSystemParameter = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, 
		&weGetSystemParameterPattern[0], sizeof(weGetSystemParameterPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeGetSystemParameter at 0x%1$08X.") % pgTrueWeGetSystemParameter);
	INSTALL_INLINE_HOOK(WeGetSystemParameter)

	pgTrueWeVerifyMapCellsLimit = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, 
		gWeTextSectionLength, &weVerifyMapCellsLimitPattern[0], sizeof(weVerifyMapCellsLimitPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeVerifyMapCellsLimit at 0x%1$08X.") % pgTrueWeVerifyMapCellsLimit);
	boost::uint32_t callOffset = aero::offset_element_sum<boost::uint32_t>(pgTrueWeVerifyMapCellsLimit, 6);
	pgMapCellsGetUnknownGlobalFlag = aero::p_sum<void *>(pgTrueWeVerifyMapCellsLimit, callOffset + 10);
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found GetUnkownFlag at 0x%1$08X.") % pgMapCellsGetUnknownGlobalFlag);
	pgWeVerifyMapCellsLimitPatcher.reset(new CMemoryPatch(aero::p_sum<void *>(pgTrueWeVerifyMapCellsLimit, 3), "\x90\x90", 2));
	pgWeVerifyMapCellsLimitPatcher->patch();
	INSTALL_INLINE_HOOK(WeVerifyMapCellsLimit)

	pgTrueWeTriggerNameCheck = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, 
		&weTriggerNameCheckPattern[0], sizeof(weTriggerNameCheckPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeTriggerNameCheck at 0x%1$08X.") % pgTrueWeTriggerNameCheck);
	INSTALL_INLINE_HOOK(WeTriggerNameCheck)

	pgTrueWeTriggerNameInputCharCheck = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, &weTriggerNameInputCharCheckPattern[0], sizeof(weTriggerNameInputCharCheckPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeTriggerNameInputCharCheck at 0x%1$08X.") % pgTrueWeTriggerNameInputCharCheck);
	pgWeTriggerNameInputCharCheckPatcher.reset(new CMemoryPatch(
		aero::pointer_sum<aero::pointer_type>(pgTrueWeTriggerNameInputCharCheck, 3),
		"\x90\x90", 2)
	);
	pgWeTriggerNameInputCharCheckPatcher->patch();
	INSTALL_INLINE_HOOK(WeTriggerNameInputCharCheck)

	//pgTrueWeSetWindowCaption = (uintptr_t)0x00433A00;//MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, &weSetWindowCaptionPattern[0], sizeof(weSetWindowCaptionPattern));
	//LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeSetWindowCaption at 0x%1$08X.") % pgTrueWeSetWindowCaption);
	//INSTALL_INLINE_HOOK(WeSetWindowCaption)

	pgTrueWeUtf8ToAnsi = (uintptr_t)0x00429CD0;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeUtf8ToAnsi at 0x%1$08X.") % pgTrueWeUtf8ToAnsi);
	INSTALL_INLINE_HOOK(WeUtf8ToAnsi)

	pgTrueWeStringCompare = (uintptr_t)0x004D2D90;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeStringCompare at 0x%1$08X.") % pgTrueWeStringCompare);
	INSTALL_INLINE_HOOK(WeStringCompare)

	pgTrueWeTriggerEditorEditboxCopy = (uintptr_t)0x0071FE90;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found TriggerEditorEditboxCopy at 0x%1$08X.") % pgTrueWeTriggerEditorEditboxCopy);
	INSTALL_INLINE_HOOK(WeTriggerEditorEditboxCopy)

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Installing inline hooks complete.");
}

#undef INSTALL_INLINE_HOOK

#define UNINSTALL_INLINE_HOOK(name) \
	if (is##name##HookInstalled) \
	{ \
		is##name##HookInstalled = \
			!ydwe::hook::inline_uninstall(&pgTrue##name, (uintptr_t)Detour##name); \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook uninstallation succeeded."); \
	} \
	else \
	{ \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook has not been installed. No need to uninstall."); \
	}

/** \brief Uninstall storm hook
 *
 * Uninstall storm hook
 */
static void UninstallInlineHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Inline hook uninstallation begins!");

	UNINSTALL_INLINE_HOOK(WeSetWindowCaption)

	UNINSTALL_INLINE_HOOK(WeTriggerNameInputCharCheck)
	pgWeTriggerNameInputCharCheckPatcher->restore();

	UNINSTALL_INLINE_HOOK(WeTriggerNameCheck)

	UNINSTALL_INLINE_HOOK(WeVerifyMapCellsLimit)
	pgWeVerifyMapCellsLimitPatcher->restore();

	UNINSTALL_INLINE_HOOK(WeGetSystemParameter)

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Inline hook uninstallation finished!");
}

#undef UNINSTALL_INLINE_HOOK

/// Storm IAT hooker
static ydwe::hook::iat_manager pgStormIatHooker;
/// We IAT hooker
static ydwe::hook::iat_manager pgWeIatHooker;

static uintptr_t pgTrueSHBrowseForFolderA;
PIDLIST_ABSOLUTE WINAPI DetourWeSHBrowseForFolderA(LPBROWSEINFOA lpbi)
{
	if (lpbi->lpszTitle && ydwe::util::is_utf8(lpbi->lpszTitle))
	{
		std::string tmp =  ydwe::util::u2a(lpbi->lpszTitle);
		lpbi->lpszTitle = tmp.c_str();
		return aero::std_call<PIDLIST_ABSOLUTE>(pgTrueSHBrowseForFolderA, lpbi);
	}

	return aero::std_call<PIDLIST_ABSOLUTE>(pgTrueSHBrowseForFolderA, lpbi);
}

	
static uintptr_t pgTrueGetOpenFileNameA;
BOOL WINAPI DetourWeGetOpenFileNameA(LPOPENFILENAMEA lpofn)
{
	if (lpofn->lpstrTitle && ydwe::util::is_utf8(lpofn->lpstrTitle))
	{
		std::string tmp =  ydwe::util::u2a(lpofn->lpstrTitle);
		lpofn->lpstrTitle = tmp.c_str();
		return aero::std_call<BOOL>(pgTrueGetOpenFileNameA, lpofn);
	}

	return aero::std_call<BOOL>(pgTrueGetOpenFileNameA, lpofn);
}

static uintptr_t pgTrueGetSaveFileNameA;
BOOL WINAPI DetourWeGetSaveFileNameA(LPOPENFILENAMEA lpofn)
{
	if (lpofn->lpstrTitle && ydwe::util::is_utf8(lpofn->lpstrTitle))
	{
		std::string tmp =  ydwe::util::u2a(lpofn->lpstrTitle);
		lpofn->lpstrTitle = tmp.c_str();
		return aero::std_call<BOOL>(pgTrueGetSaveFileNameA, lpofn);
	}

	return aero::std_call<BOOL>(pgTrueGetSaveFileNameA, lpofn);
}

#define INSTALL_STORM_IAT_HOOK(apiName) \
	if (pgStormIatHooker.hook(#apiName, &pgTrue##apiName##, (uintptr_t)DetourStorm##apiName##)) \
	{ \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Storm " #apiName " IAT hook succeeded."); \
	} \
	else \
	{ \
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Storm " #apiName " IAT hook failed."); \
	}

#define INSTALL_WE_IAT_HOOK(apiName) \
	if (pgWeIatHooker.hook(#apiName, &pgTrue##apiName##, (uintptr_t)DetourWe##apiName##)) \
	{ \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "WE " #apiName " IAT hook succeeded."); \
	} \
	else \
	{ \
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "WE " #apiName " IAT hook failed."); \
	}

/** \brief Initialize IAT hook
 *
 * Initialize IAT hook targeted at modules
 */
static void InitIATHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "IAT hook initialization started.");

	// Init Iat hooker
	
	if (pgWeIatHooker.open_module(::GetModuleHandleW(NULL)))
	{
		if (pgWeIatHooker.open_dll("shell32.dll"))
		{
			INSTALL_WE_IAT_HOOK(SHBrowseForFolderA);
		}
		else
		{
			LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot find shell32.dll in WE.");
		}

		if (pgWeIatHooker.open_dll("comdlg32.dll"))
		{
			INSTALL_WE_IAT_HOOK(GetSaveFileNameA);
			INSTALL_WE_IAT_HOOK(GetOpenFileNameA);
		}
		else
		{
			LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot find comdlg32.dll in WE.");
		}
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "WE initialize IAT hook failed.");
	}
	
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "IAT hook initialization completed.");
}

#undef INSTALL_WE_IAT_HOOK
#undef INSTALL_STORM_IAT_HOOK

static void UninstallIATHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "IAT hook cleanup started.");
	pgWeIatHooker.release();
	LOG4CXX_TRACE(NYDWE::gInjectLogger, "WE IAT hook cleaned.");
	pgStormIatHooker.release();
	LOG4CXX_TRACE(NYDWE::gInjectLogger, "Storm.dll IAT hook cleaned.");
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "IAT hook cleanup completed.");
}

static boost::uint8_t syntaxCheckPatch[] = 
{
	0x31, 0xC0,	//xor eax,eax (return 0)
	0x90,
	0x90,
	0x90,
	0x90,
	0x90,
	0x90,
	0x90
};

/*
.text:005BC089 50                       push    eax
.text:005BC08A 8B CF                    mov     ecx, edi
.text:005BC08C 89 5F 04                 mov     [edi+4], ebx
.text:005BC08F FF 52 0C                 call    dword ptr [edx+0Ch]
*/
static boost::uint16_t syntaxCheckPattern[] =
{
	0xFF50,
	0xFF8B, 0xFFCF,
	0xFF89, 0xFF5F, 0xFF04,
	0xFFFF, 0xFF52, 0xFF0C
};

/*
.text:005CEFF3 E8 68 FD F4 FF                 call    sub_51ED60
.text:005CEFF8 85 C0                          test    eax, eax
.text:005CEFFA 75 09                          jnz     short loc_5CF005
.text:005CEFFC 8B 4D FC                       mov     ecx, [ebp+var_4]
.text:005CEFFF 53                             push    ebx
*/
static boost::uint16_t autoDisablePattern[] = 
{
	0xFFE8, 0x0068, 0x00FD, 0x00F4, 0x00FF,
	0xFF85, 0xFFC0,
	0xFF75, 0xFF09,
	0xFF8B, 0xFF4D, 0xFFFC,
	0xFF53
};

static boost::uint8_t autoDisablePatch[] = 
{
	0x90, 0x90, 0x90, 0x90, 0x90
};

/*
.text:005C88F5 75 08                          jnz     short loc_5C88FF
.text:005C88F7 5F                             pop     edi
.text:005C88F8 5E                             pop     esi
.text:005C88F9 8B E5                          mov     esp, ebp
.text:005C88FB 5D                             pop     ebp
.text:005C88FC C2 0C 00                       retn    0Ch
.text:005C88FF 68 04 01 00 00                 push    104h
*/
static boost::uint16_t enableTriggerCheck1Pattern[] = 
{
	0xFF75, 0xFF08, 
	0xFF5F, 
	0xFF5E, 
	0xFF8B, 0xFFE5, 
	0xFF5D, 
	0xFFC2, 0xFF0C, 0xFF00, 
	0xFF68, 0xFF04, 0xFF01, 0xFF00, 0xFF00
};

static boost::uint8_t enableTriggerCheck1Patch[] = 
{
	0xEB		// jnz -> jmp
};

/*
.text:005C88DA 56                             push    esi
.text:005C88DB E8 B0 62 F5 FF                 call    sub_51EB90
.text:005C88E0 85 C0                          test    eax, eax
.text:005C88E2 75 08                          jnz     short loc_5C88EC
.text:005C88E4 5F                             pop     edi
.text:005C88E5 5E                             pop     esi
.text:005C88E6 8B E5                          mov     esp, ebp
.text:005C88E8 5D                             pop     ebp
*/
static boost::uint16_t enableTriggerCheck2Pattern[] = 
{
	0xFF56, 
	0xFFE8, 0x00B0, 0x0062, 0x00F5, 0x00FF, 
	0xFF85, 0xFFC0, 
	0xFF75, 0xFF08, 
	0xFF5F, 
	0xFF5E, 
	0xFF8B, 0xFFE5,
	0xFF5D
};

static boost::uint8_t enableTriggerCheck2Patch[] = 
{
	0x90, 
	0x90, 0x90, 0x90, 0x90, 0x90,	// remove push && call
	0x90, 0x90, 
	0xEB							// jnz -> jmp
};

static boost::uint16_t doodadLimitPattern[] = 
{
	0xFF7E, 0xFF5C, 
	0xFF53, 
	0xFF57, 
	0xFF33, 0xFFFF, 
	0xFF8B, 0x0000, 
	0xFF8B, 0xFF46, 0x0000, 
	0xFF8B, 0xFF8C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0xFF03, 0xFFC7
};

static boost::uint8_t doodadLimitPatch[] = 
{
	0xEB
};

static boost::uint16_t unitItemLimitPattern[] = 
{
	0xFF0F, 0xFF8E, 0x0000, 0xFF00, 0xFF00, 0xFF00, 
	0xFF89, 0x0000, 0x0000, 
	0xFF89, 0x0000, 0x0000, 
	0xFF8B, 0xFF43, 0x0000, 
	0xFF8B, 0xFF8C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
	0xFF03, 0xFFC7, 
	0xFFF6, 0xFFC5, 0xFF04
};

static boost::uint8_t unitItemLimitPatch[] = 
{
	0xE9, 0xA0, 0x00, 0x00, 0x00, 0x90
};

static boost::uint16_t editorInstanceCheckPattern[] = 
{
	0xFF57, 0xFF51, 0xFF6A, 0xFF00, 0xFF6A, 0xFF01, 
	0xFF6A, 0xFF00, 
	0xFFFF, 0xFF15, 0x0000, 0x0000, 0x0000, 0x0000, 
	0xFF8B, 0xFFF8, 
	0xFF85, 0xFFFF, 
	0xFF75, 0x0000, 
	0xFF5F, 0xFFC3, 0xFF56
};

static boost::uint8_t editorInstanceCheckPatch[] = 
{
	0x33, 0xC0, 0xC3, 0x90
};

#define INSTALL_PATCH_CODEGEN(name) InstallPatch(pgWeTextSectionBase, gWeTextSectionLength, name## Pattern, sizeof(name## Pattern), name## Patch, sizeof(name## Patch))

/// Patches
static void InitPatches()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Patches initialization started.");

	if (pgWeTextSectionBase && (gWeTextSectionLength > 0))
	{
		// Syntax check patch
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing syntax check patch");
		INSTALL_PATCH_CODEGEN(syntaxCheck);

		// Auto disable trigger patch
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing auto disable patch");
		INSTALL_PATCH_CODEGEN(autoDisable);

		// Enable trigger check patch
		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing enable trigger check patch");
		INSTALL_PATCH_CODEGEN(enableTriggerCheck1);
		INSTALL_PATCH_CODEGEN(enableTriggerCheck2);

		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing doodad limit patch");
		INSTALL_PATCH_CODEGEN(doodadLimit);

		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing unit/item limit patch");
		INSTALL_PATCH_CODEGEN(unitItemLimit);

		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing editor multi-instance patch");
		INSTALL_PATCH_CODEGEN(editorInstanceCheck);

		LOG4CXX_TRACE(NYDWE::gInjectLogger, "Installing attack table patch");


		CPEMemoryFileInfo pm;
#define WE_ADDRESS(ADDR) ((uintptr_t)(ADDR) - 0x00400000 + pm.getBase())
		enum ATTACK_TABLE
		{
			WESTRING_UE_ATTACKTYPE_SPELLS = 0,
			WESTRING_UE_ATTACKTYPE_NORMAL,
			WESTRING_UE_ATTACKTYPE_PIERCE,
			WESTRING_UE_ATTACKTYPE_SIEGE,
			WESTRING_UE_ATTACKTYPE_MAGIC,
			WESTRING_UE_ATTACKTYPE_CHAOS,
			WESTRING_UE_ATTACKTYPE_HERO,
		};
		uintptr_t attack_table_string[] = {
			WE_ADDRESS(0x007DF394),
			WE_ADDRESS(0x007DF374),
			WE_ADDRESS(0x007DF354),
			WE_ADDRESS(0x007DF334),
			WE_ADDRESS(0x007DF314),
			WE_ADDRESS(0x007DF2F4),
			WE_ADDRESS(0x007DF2D8),
		};

		uintptr_t ptr = WE_ADDRESS(0x00784488);
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_NORMAL]); ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_PIERCE]); ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_SIEGE]);  ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_MAGIC]);  ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_CHAOS]);  ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_SPELLS]); ptr += 4;
		ydwe::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_HERO]);   ptr += 4;

#undef WE_ADDRESS
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Section info not correct. Cannot make patches");
	}

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Patches initialization completed.");
}

} // namespace NGameHook

#undef INSTALL_PATCH_CODEGEN

void InstallHooks()
{
	pgStormDllBase = GetModuleHandleW(L"storm.dll");

	// Init section information
	NGameHook::InitSectionInfo();

	// Install hooks
	NGameHook::InitInlineHook();
	NGameHook::InitIATHook();

	// Initialize patches
	NGameHook::InitPatches();
}

void UninstallHooks()
{
	// Uninstall hooks
	NGameHook::UninstallIATHook();
	NGameHook::UninstallInlineHook();
}

/// Storm.dll base address
HMODULE pgStormDllBase;

} // namespace NYDWE
