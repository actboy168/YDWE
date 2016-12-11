#include <windows.h>
#include <base/path/self.h>
#include <base/warcraft3/virtual_mpq.h>	
#include <slk/reader/IniReader.hpp>
#include <slk/reader/IniReader.cpp>
#include <slk/reader/CommonReader.cpp> 
#include <base/file/stream.h>

void Initialize()
{
	base::warcraft3::virtual_mpq::initialize(::GetModuleHandleW(L"Game.dll"));
	fs::path ydwe_path = base::path::self().remove_filename().remove_filename().remove_filename();
	slk::IniTable table;
	table["MapTest"]["VirtualMpq"] = "share/mpq/war3";
	try {
		base::buffer buf = base::file::read_stream(ydwe_path / L"bin" / L"EverConfig.cfg").read<base::buffer>();
		base::buffer_reader reader(buf);
		slk::IniReader::Read(reader, table);
	}
	catch (...) {
	}
	base::warcraft3::virtual_mpq::open_path(ydwe_path / table["MapTest"]["VirtualMpq"], 15);
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
