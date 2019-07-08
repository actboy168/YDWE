#include <windows.h>
#include <map>
#include <base/path/self.h>
#include <warcraft3/virtual_mpq.h>	 
#include <base/util/ini.h>
#include <base/file/stream.h>

void Initialize()
{
	warcraft3::virtual_mpq::initialize(::GetModuleHandleW(L"Game.dll"));
	fs::path ydwe_path = base::path::self().parent_path().parent_path().parent_path();
	base::ini::table table;
	table["MapTest"]["VirtualMpq"] = "share/mpq/war3";
	try {
		auto buf = base::file::read_stream(ydwe_path / L"bin" / L"EverConfig.cfg").read<std::string>();
		base::ini::read(table, buf.c_str());
	}
	catch (...) {
	}
	warcraft3::virtual_mpq::open_path(ydwe_path / table["MapTest"]["VirtualMpq"], 15);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
