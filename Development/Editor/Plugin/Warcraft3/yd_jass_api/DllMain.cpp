#include <windows.h>


namespace ydwe { namespace warcraft3 { namespace japi {
void InitializeUnitState();
void InitializeAbilityState();
void InitializeEventDamageData();
void InitializeDisplayChat();
}}}

void Initialize()
{
	ydwe::warcraft3::japi::InitializeUnitState();
	ydwe::warcraft3::japi::InitializeAbilityState();
	ydwe::warcraft3::japi::InitializeEventDamageData();
	ydwe::warcraft3::japi::InitializeDisplayChat();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "yd_jass_api";
}

// 6F2978FC
