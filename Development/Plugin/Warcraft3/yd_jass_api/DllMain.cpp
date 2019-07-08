#include <windows.h>

namespace warcraft3::japi {
	void initialize();
}

void Initialize()
{
	warcraft3::japi::initialize();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID /*pReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

// 6F2978FC
