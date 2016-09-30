#include <Windows.h>	
#include <base/hook/fp_call.h>	
#include <base/hook/dyn_iat.h>	
#include "Direct3D8.h"

typedef IDirect3D8* (__stdcall* FnDirect3DCreate8)(UINT);

uintptr_t RealDirect3DCreate8()
{
	HMODULE lib = GetModuleHandleW(L"d3d8.dll");
	if (!lib)
	{
		lib = LoadLibraryW(L"d3d8.dll");
		if (!lib)
		{
			return NULL;
		}
	}
	return (uintptr_t)GetProcAddress(lib, "Direct3DCreate8");
}

IDirect3D8* callRealDirect3DCreate8(UINT sdk_version)
{
	uintptr_t fn = RealDirect3DCreate8();
	if (!fn) return 0;
	return base::std_call<IDirect3D8*>(fn, sdk_version);
}

IDirect3D8* callFakeDirect3DCreate8(UINT sdk_version)
{
	IDirect3D8* d3d = callRealDirect3DCreate8(sdk_version);
	if (!d3d) return 0;
	return new CDirect3D8(d3d);
}

extern "C"
IDirect3D8* __stdcall Direct3DCreate8(UINT sdk_version)
{
	return callFakeDirect3DCreate8(sdk_version);
}

void Initialize()
{
	HMODULE gamedll = ::GetModuleHandleW(L"Game.dll");
	base::hook::dyn_iat(gamedll, L"d3d8.dll", "Direct3DCreate8", 0, (uintptr_t)Direct3DCreate8);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_d3d8hook";
}
