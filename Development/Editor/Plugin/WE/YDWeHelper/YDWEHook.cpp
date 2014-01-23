#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <functional>
#include <regex>
#include <aero/aero.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/scope_exit.hpp>
#include <base/util/unicode.h>
#include <base/hook/inline.h>
#include <base/hook/iat_manager.h>
#include <base/hook/fp_call.h>
#include <base/util/format.h>

#include <objbase.h>
#include <windows.h>
#include <Shlobj.h>

#include "PEMemoryFileInfo.h"
#include "MemoryPatternSearch.h"
#include "MemoryPatch.h"

#include "YDWEHookPattern.h"
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
		LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found patch point: 0x%08X", reinterpret_cast<uintptr_t>(patchPoint)));

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

static void *pgWeTextSectionBase;
static uintptr_t gWeTextSectionLength;
static void *pgWeDataSectionBase;
static uintptr_t gWeDataSectionLength;

static void InitSectionInfo()
{
	CPEMemoryFileInfo pm;
	CPEMemoryFileInfo::TSectionInfoResult result = pm.querySection(".text");
	if (result)
	{
		pgWeTextSectionBase = (void *)result->get<0>();
		gWeTextSectionLength = result->get<1>();

		LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("WE .text section start address: 0x%08X. length 0x%08X", pgWeTextSectionBase, gWeTextSectionLength));
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

		LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("WE .data section start address: 0x%08X. length 0x%08X", pgWeDataSectionBase, gWeDataSectionLength));
	}
	else
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Cannot get .data section info of WE.");
	}
}

static uintptr_t pgTrueWeSetWindowCaption;
static bool isWeSetWindowCaptionHookInstalled;

BOOL __fastcall DetourWeSetWindowCaption(HWND hWnd, LPCSTR lpString)
{
	if ((!lpString) || base::util::is_utf8(lpString))
	{
		return base::fast_call<BOOL>(pgTrueWeSetWindowCaption, hWnd, lpString);
	}
	else
	{
		return SetWindowTextA(hWnd, lpString);
	}
}

static uintptr_t pgTrueWeSetMenuItem;
static bool isWeSetMenuItemHookInstalled;
uint32_t __fastcall DetourWeSetMenuItem(uint32_t this_, uint32_t edx_, uint32_t item, const char* str, uint32_t hotkey[2])
{
	try
	{
		if ((str) && !base::util::is_utf8(str))
		{
			std::string tmp = base::util::a2u(str);
			return base::this_call<uint32_t>(pgTrueWeSetMenuItem, this_, item, tmp.c_str(), hotkey);
		}
	}
	catch (...)
	{
	}

	return base::this_call<uint32_t>(pgTrueWeSetMenuItem, this_, item, str, hotkey);
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
		return base::fast_call<bool>(pgTrueWeStringCompare, a, b, ignore_case);
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

		return g_deflocale.compare(base::util::u2w(a_str), base::util::u2w(b_str));
	}
	catch (...)
	{
	}

	return base::fast_call<bool>(pgTrueWeStringCompare, a, b, ignore_case);
}

template <typename charT>
HGLOBAL CreateGlobalData(const std::basic_string<charT>& str)
{
	HGLOBAL data = ::GlobalAlloc(GMEM_MOVEABLE, ((str.size() + 1) * sizeof(charT)));
	if (data) 
	{
		charT* raw_data = static_cast<charT*>(::GlobalLock(data));
		if (!raw_data)
		{
			::GlobalUnlock(data);
			return nullptr;
		}
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
			return WriteText(base::util::u2w(source));
		}
		catch (...) {			
		}
	}

	return base::fast_call<BOOL>(pgTrueWeTriggerEditorEditboxCopy, source);
}

static uintptr_t pgTrueWeGetSystemParameter;
static bool isWeGetSystemParameterHookInstalled;

static boost::int32_t __fastcall DetourWeGetSystemParameter(LPCSTR parentKey, LPCSTR childKey, boost::int32_t flag)
{
	boost::int32_t result = base::fast_call<boost::int32_t>(pgTrueWeGetSystemParameter, parentKey, childKey, flag);

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

static uintptr_t pgTrueWeVerifyMapCellsLimit;
static bool isWeVerifyMapCellsLimitHookInstalled;
static void *pgMapCellsGetUnknownGlobalFlag;

boost::scoped_ptr<CMemoryPatch> pgWeVerifyMapCellsLimitPatcher;

static boost::int32_t __fastcall DetourWeVerifyMapCellsLimit(boost::int32_t flag)
{
	boost::int32_t cellSideLimit;

	if (flag == -1)
	{
		flag = base::fast_call<boost::int32_t>(pgMapCellsGetUnknownGlobalFlag);
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

static uintptr_t pgTrueWeTriggerNameCheck;
static bool isWeTriggerNameCheckHookInstalled;

static BOOL __fastcall DetourWeTriggerNameCheck(const char *triggerName, BOOL allowSpaces)
{
	//BOOL result = base::fast_call<BOOL>(pgTrueWeTriggerNameCheck, triggerName, allowSpaces);
	//return result;
	return triggerName && (*triggerName != 0);
}

static uintptr_t pgTrueWeTriggerNameInputCharCheck;
static bool isWeTriggerNameInputCharCheckHookInstalled;

boost::scoped_ptr<CMemoryPatch> pgWeTriggerNameInputCharCheckPatcher;

static boost::uint32_t __fastcall DetourWeTriggerNameInputCharCheck(boost::uint32_t c, boost::uint32_t flag)
{
	//boost::uint32_t result = base::fast_call<boost::uint32_t>(pgTrueWeTriggerNameInputCharCheck, c, flag);
	//return result;
	return 1;
}

#define INSTALL_INLINE_HOOK(name) \
	if (!is##name##HookInstalled) \
	{ \
		if (pgTrue##name##) \
		{ \
			is##name##HookInstalled = base::hook::inline_install(&pgTrue##name##, (uintptr_t)Detour##name##); \
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

bool isWeUtf8ToAnsiHookInstalled;
uintptr_t pgTrueWeUtf8ToAnsi;
void __fastcall DetourWeUtf8ToAnsi(const char* str)
{
	if (str)
	{
		base::fast_call<void>(pgTrueWeUtf8ToAnsi, str);
	}
	else
	{
		base::fast_call<void>(pgTrueWeUtf8ToAnsi, "");
	}
}

static void InitInlineHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Start installing inline hooks.");

	pgTrueWeGetSystemParameter = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, 
		&weGetSystemParameterPattern[0], sizeof(weGetSystemParameterPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeGetSystemParameter at 0x%08X.", pgTrueWeGetSystemParameter));
	INSTALL_INLINE_HOOK(WeGetSystemParameter)

	pgTrueWeVerifyMapCellsLimit = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, 
		gWeTextSectionLength, &weVerifyMapCellsLimitPattern[0], sizeof(weVerifyMapCellsLimitPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeVerifyMapCellsLimit at 0x%08X.", pgTrueWeVerifyMapCellsLimit));
	boost::uint32_t callOffset = aero::offset_element_sum<boost::uint32_t>(pgTrueWeVerifyMapCellsLimit, 6);
	pgMapCellsGetUnknownGlobalFlag = aero::p_sum<void *>(pgTrueWeVerifyMapCellsLimit, callOffset + 10);
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found GetUnkownFlag at 0x%08X.", pgMapCellsGetUnknownGlobalFlag));
	pgWeVerifyMapCellsLimitPatcher.reset(new CMemoryPatch(aero::p_sum<void *>(pgTrueWeVerifyMapCellsLimit, 3), "\x90\x90", 2));
	pgWeVerifyMapCellsLimitPatcher->patch();
	INSTALL_INLINE_HOOK(WeVerifyMapCellsLimit)

	pgTrueWeTriggerNameCheck = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, 
		&weTriggerNameCheckPattern[0], sizeof(weTriggerNameCheckPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeTriggerNameCheck at 0x%08X.", pgTrueWeTriggerNameCheck));
	INSTALL_INLINE_HOOK(WeTriggerNameCheck)

	pgTrueWeTriggerNameInputCharCheck = (uintptr_t)MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, &weTriggerNameInputCharCheckPattern[0], sizeof(weTriggerNameInputCharCheckPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeTriggerNameInputCharCheck at 0x%08X.", pgTrueWeTriggerNameInputCharCheck));
	pgWeTriggerNameInputCharCheckPatcher.reset(new CMemoryPatch(
		aero::pointer_sum<aero::pointer_type>(pgTrueWeTriggerNameInputCharCheck, 3),
		"\x90\x90", 2)
	);
	pgWeTriggerNameInputCharCheckPatcher->patch();
	INSTALL_INLINE_HOOK(WeTriggerNameInputCharCheck)

	pgTrueWeSetWindowCaption = (uintptr_t)0x00433A00;//MemoryPatternSearch(pgWeTextSectionBase, gWeTextSectionLength, &weSetWindowCaptionPattern[0], sizeof(weSetWindowCaptionPattern));
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeSetWindowCaption at 0x%08X.", pgTrueWeSetWindowCaption));
	INSTALL_INLINE_HOOK(WeSetWindowCaption)

	pgTrueWeSetMenuItem = (uintptr_t)0x0042AA10;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeSetMenuItem at 0x%08X.", pgTrueWeSetMenuItem));
	INSTALL_INLINE_HOOK(WeSetMenuItem)

	pgTrueWeStringCompare = (uintptr_t)0x004D2D90;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeStringCompare at 0x%08X.", pgTrueWeStringCompare));
	INSTALL_INLINE_HOOK(WeStringCompare)

	pgTrueWeTriggerEditorEditboxCopy = (uintptr_t)0x0071FE90;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found TriggerEditorEditboxCopy at 0x%08X.", pgTrueWeTriggerEditorEditboxCopy));
	INSTALL_INLINE_HOOK(WeTriggerEditorEditboxCopy)

	pgTrueWeUtf8ToAnsi = (uintptr_t)0x00429CD0;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, base::format("Found WeUtf8ToAnsi at 0x%08X.", pgTrueWeUtf8ToAnsi));
	INSTALL_INLINE_HOOK(WeUtf8ToAnsi)

	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "Installing inline hooks complete.");
}

#undef INSTALL_INLINE_HOOK

#define UNINSTALL_INLINE_HOOK(name) \
	if (is##name##HookInstalled) \
	{ \
		is##name##HookInstalled = \
			!base::hook::inline_uninstall(&pgTrue##name, (uintptr_t)Detour##name); \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook uninstallation succeeded."); \
	} \
	else \
	{ \
		LOG4CXX_TRACE(NYDWE::gInjectLogger, #name " hook has not been installed. No need to uninstall."); \
	}

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

static base::hook::iat_manager pgStormIatHooker;
static base::hook::iat_manager pgWeIatHooker;

static uintptr_t pgTrueSHBrowseForFolderA;
PIDLIST_ABSOLUTE WINAPI DetourWeSHBrowseForFolderA(LPBROWSEINFOA lpbi)
{
	if (lpbi->lpszTitle && base::util::is_utf8(lpbi->lpszTitle))
	{
		std::string tmp =  base::util::u2a(lpbi->lpszTitle);
		lpbi->lpszTitle = tmp.c_str();
		return base::std_call<PIDLIST_ABSOLUTE>(pgTrueSHBrowseForFolderA, lpbi);
	}

	return base::std_call<PIDLIST_ABSOLUTE>(pgTrueSHBrowseForFolderA, lpbi);
}

	
static uintptr_t pgTrueGetOpenFileNameA;
BOOL WINAPI DetourWeGetOpenFileNameA(LPOPENFILENAMEA lpofn)
{
	if (lpofn->lpstrTitle && base::util::is_utf8(lpofn->lpstrTitle))
	{
		std::string tmp =  base::util::u2a(lpofn->lpstrTitle);
		lpofn->lpstrTitle = tmp.c_str();
		return base::std_call<BOOL>(pgTrueGetOpenFileNameA, lpofn);
	}

	return base::std_call<BOOL>(pgTrueGetOpenFileNameA, lpofn);
}

static uintptr_t pgTrueGetSaveFileNameA;
BOOL WINAPI DetourWeGetSaveFileNameA(LPOPENFILENAMEA lpofn)
{
	if (lpofn->lpstrTitle && base::util::is_utf8(lpofn->lpstrTitle))
	{
		std::string tmp =  base::util::u2a(lpofn->lpstrTitle);
		lpofn->lpstrTitle = tmp.c_str();
		return base::std_call<BOOL>(pgTrueGetSaveFileNameA, lpofn);
	}

	return base::std_call<BOOL>(pgTrueGetSaveFileNameA, lpofn);
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

static void InitIATHook()
{
	LOG4CXX_DEBUG(NYDWE::gInjectLogger, "IAT hook initialization started.");
	
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
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_NORMAL]); ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_PIERCE]); ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_SIEGE]);  ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_MAGIC]);  ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_CHAOS]);  ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_SPELLS]); ptr += 4;
		base::hook::replace_pointer(ptr, attack_table_string[WESTRING_UE_ATTACKTYPE_HERO]);   ptr += 4;

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
} // namespace NYDWE
