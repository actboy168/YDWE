
#include <boost/filesystem.hpp>
#include <base/file/stream.h>
#include <base/path/service.h>
#include <base/win/env_variable.h>
#include <base/win/process.h>
#include <slk/reader/IniReader.hpp>
#include <base/warcraft3/directory.h>
#include <base/warcraft3/command_line.h>

bool launch_warcraft3(base::warcraft3::command_line& cmd)
{
	try {
		boost::filesystem::path ydwe_path = base::path::get(base::path::DIR_EXE).remove_filename().remove_filename();

		base::win::env_variable ev(L"PATH");
		std::wstring p;
		p += (ydwe_path / L"bin").c_str();    p += L";"; 
		ev.set(p + ev.get());

		boost::filesystem::path war3_path;
		if (!base::warcraft3::directory::get(nullptr, war3_path))
		{
			return false;
		}

		boost::filesystem::path inject_dll = ydwe_path / L"plugin" / L"warcraft3" / L"yd_loader.dll";

		slk::IniTable table;
		table["MapTest"]["LaunchOpenGL"]   = "0";
		table["MapTest"]["LaunchWindowed"] = "1";
		try {
			slk::buffer buf = base::file::read_stream(ydwe_path / L"bin" / L"EverConfig.cfg").read<slk::buffer>();
			slk::buffer_reader reader(buf);
			slk::IniReader::Read(reader, table);
		} 
		catch (...) {
		}

		if ("0" != table["MapTest"]["LaunchOpenGL"])
		{
			cmd.add(L"opengl");
		}

		if ("0" != table["MapTest"]["LaunchWindowed"])
		{
			cmd.add(L"window");
		}

		base::win::process warcraft3_process;

		if (boost::filesystem::exists(inject_dll))
		{
			cmd.add(L"ydwe", ydwe_path.wstring());
			warcraft3_process.inject(inject_dll);			
		}


		cmd.app(war3_path.wstring());
		cmd.del(L"launchwar3");
		return warcraft3_process.create(war3_path / L"war3.exe", cmd.str());

	}
	catch (...) {
	}

	return false;
}
