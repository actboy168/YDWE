#include <windows.h>
#include <base/path/self.h>
#include <base/warcraft3/virtual_mpq.h>

void Initialize()
{
	base::warcraft3::virtual_mpq::initialize(::GetModuleHandleW(L"Game.dll"));
	base::warcraft3::virtual_mpq::open_path(base::path::self().remove_filename().remove_filename().remove_filename() / L"share" / L"mpq" / L"war3", 15);
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
	return "yd_virtual_mpq";
}
