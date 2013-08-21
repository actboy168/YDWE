#include <cstdint>
#include <boost/format.hpp>
#include <boost/exception/all.hpp>
#include "../../Core/Include/YDWEPublic.h"
#include <ydwe/lua/luabind.h>
#include <ydwe/hook/inline.h>
#include "DllMain.h"
#include "CustomObjectIdHook.h"

namespace NYDWE
{

/// Original WeNewObjectId function pointer
static uintptr_t pgTrueWeNewObjectId;
/// Whether hook installed?
static bool isWeNewObjectIdHookInstalled;
/*
.text:005B645A 89 50 04                             mov     [eax+4], edx
.text:005B645D 89 08                                mov     [eax], ecx
.text:005B645F C7 43 24 01 00 00 00                 mov     dword ptr [ebx+24h], 1
.text:005B6466 8B C2                                mov     eax, edx
.text:005B6468 5B                                   pop     ebx
.text:005B6469 8B E5                                mov     esp, ebp
.text:005B646B 5D                                   pop     ebp
.text:005B646C C2 04 00                             retn    4
.text:005B646C                      sub_5B63A0      endp
*/
static uint16_t weNewObjectIdPattern[] = 
{
	0xFF89, 0xFF50, 0xFF04, 
	0xFF89, 0xFF08, 
	0xFFC7, 0xFF43, 0xFF24, 0xFF01, 0xFF00, 0xFF00, 0xFF00, 
	0xFF8B, 0xFFC2, 
	0xFF5B,
	0xFF8B, 0xFFE5,
	0xFF5D,
	0xFFC2, 0xFF04, 0xFF00
};

static int32_t __fastcall PromptUserForNewId(int32_t defaultId)
{
	int32_t result = defaultId;

	try
	{
		result = luabind::call_function<int32_t>((lua_State *)YDWEGetLuaState(), "on_prompt_for_new_id", defaultId);
	}
	catch (luabind::error &e)
	{
		const char *errorMessage = lua_tostring(e.state(), -1);
		LOG4CXX_ERROR(NYDWE::gInjectLogger, boost::format("Lua error: %1%.") % errorMessage);
		lua_pop(e.state(), 1);
	}
	catch (...)
	{
		LOG4CXX_ERROR(NYDWE::gInjectLogger, "Unknown exception: " << boost::current_exception_diagnostic_information());
	}

	return result;
}

static void __declspec(naked) DetourWeNewObjectId()
{
	__asm
	{
		push	eax;
		push	ecx;

		mov		ecx, edx
		call	PromptUserForNewId;
		mov		edx, eax

		pop		ecx;
		pop		eax;
		mov     [eax+4], edx;
		mov     [eax], ecx;
		mov     dword ptr [ebx+24h], 1;
		mov     eax, edx;
		pop     ebx;
		mov     esp, ebp;
		pop     ebp;
		retn    4;
	}
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

void InstallCustomObjectIdHook()
{
	pgTrueWeNewObjectId = (uintptr_t)0x005B645A;
	LOG4CXX_TRACE(NYDWE::gInjectLogger, boost::format("Found WeNewObjectId at 0x%1$08X.") % pgTrueWeNewObjectId);

	INSTALL_INLINE_HOOK(WeNewObjectId)
}

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

void UninstallCustomObjectIdHook()
{
	UNINSTALL_INLINE_HOOK(WeNewObjectId)
}

#undef UNINSTALL_INLINE_HOOK

} // namespace NYDWE
