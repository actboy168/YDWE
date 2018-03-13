#include <base/filesystem.h>
#include <base/file/stream.h>
#include <base/path/service.h>
#include <base/path/helper.h>
#include <base/win/env_variable.h>
#include <base/win/process.h>
#include <base/util/ini.h>
#include <base/util/format.h>
#include <base/warcraft3/directory.h>
#include <base/warcraft3/command_line.h>
#include <base/win/registry/key.h> 
#include <base/hook/fp_call.h>

std::wstring get_test_map_path()
{
	std::wstring result = L"Maps\\Test\\WorldEditTestMap";
	try {
		result = (base::registry::current_user() / L"Software\\Blizzard Entertainment\\WorldEdit")[L"Test Map - Copy Location"].get_string();
	}
	catch (base::registry::registry_exception const&) {}
	return std::move(result);
}

bool launch_taskbar_support(const fs::path& ydwe_path)
{
	HMODULE hdll = LoadLibraryW((ydwe_path / L"plugin" / L"YDTaskbarSupport.dll").c_str());
	if (hdll)
	{
		uintptr_t initialize = (uintptr_t)GetProcAddress(hdll, "Initialize");
		if (initialize)
		{
			base::std_call<void>(initialize);
			return true;
		}
	}
	return false;
}

bool map_slk(const fs::path& ydwe, const fs::path& from, const fs::path& to)
{
	fs::path ydwedev = base::path::ydwe(true);
	fs::path app = ydwe / L"bin" / L"lua.exe";
	base::win::process process;
	process.set_console(base::win::process::CONSOLE_DISABLE);
	process.set_env(L"PATH", (ydwe / L"bin").wstring());
	if (!process.create(
		app,
		base::format(LR"("%s" -e "package.cpath = [[%s]]" gui\mini.lua -slk -config="%s" "%s" "%s")", 
			app.wstring(), (ydwe / L"bin" / L"modules" / L"?.dll").wstring(), 
			(ydwedev / L"share"/ L"script"/ L"war3"/ L"w3x2lni.ini").wstring(),
			from.wstring(),
			to.wstring()
		),
		ydwedev / L"plugin" / L"w3x2lni"
	)) {
		return false;
	}
	return process.wait() == 0;
}

bool launch_warcraft3(base::warcraft3::command_line& cmd)
{
	try {
		fs::path ydwe = base::path::ydwe(false);
		launch_taskbar_support(ydwe);

		fs::path war3_path;
		if (!base::warcraft3::directory::get(nullptr, war3_path))
		{
			return false;
		}

		base::ini::table table;
		table["MapTest"]["LaunchRenderingEngine"] = "Direct3D 8";
		table["MapTest"]["LaunchWindowed"] = "1";
		table["MapTest"]["UserName"] = "";
		table["MapTest"]["EnableMapSlk"] = "0";
		try {
			auto buf = base::file::read_stream(ydwe / L"bin" / L"EverConfig.cfg").read<std::string>();
			base::ini::read(table, buf.c_str());
		}
		catch (...) {
		}

		//
		//  war3.exe -loadfile "xxx.w3x"
		//  据测试，当xxx.w3x的长度>=54时，war3会无法识别(原因不明)
		//  所以这里模仿we测试地图的行为，将地图复制到war3目录下，并使用相对路径(相对war3根目录的目录)
		//
		if (cmd.has(L"loadfile"))
		{
			fs::path loadfile = cmd[L"loadfile"];

			// war3将非.w3g后缀名的文件当地图处理
			if (!base::path::equal(loadfile.extension(), L".w3g"))
			{
				fs::path test_map_path = get_test_map_path() + loadfile.extension().wstring();
				try {
					cmd[L"loadfile"] = test_map_path.wstring();
					if (!loadfile.is_absolute()) {
						loadfile = war3_path / loadfile;
					}
					if ("0" != table["MapTest"]["EnableMapSlk"]) {
						if (!map_slk(ydwe, loadfile, war3_path / test_map_path)) {
							fs::copy_file(loadfile, war3_path / test_map_path, fs::copy_options::overwrite_existing);
						}
					}
					else {
						fs::copy_file(loadfile, war3_path / test_map_path, fs::copy_options::overwrite_existing);
					}
				}
				catch (...) {
				}
			}
		}

		war3_path = war3_path / L"war3.exe";
		fs::path inject_dll = ydwe / L"bin" / L"LuaEngine.dll";

		std::string name = table["MapTest"]["UserName"];
		if (name != "")
		{
			base::registry::key_a key(HKEY_CURRENT_USER, "Software\\Blizzard Entertainment\\Warcraft III", "String");
			key["userlocal"].set((const void*)name.c_str(), name.size());
		}

		if ("OpenGL" == table["MapTest"]["LaunchRenderingEngine"])
		{
			cmd.add(L"opengl");
		}

		if ("0" != table["MapTest"]["LaunchWindowed"])
		{
			cmd.add(L"window");
		}


		SetEnvironmentVariableW(L"ydwe-process-name", L"war3");

		base::win::process warcraft3_process;

		try {
			if (table["War3Patch"]["Option"] == "2")
			{
				fs::path stormdll = ydwe / L"share" / L"patch" / table["War3Patch"]["DirName"] / L"Storm.dll";
				if (fs::exists(stormdll))
				{
					warcraft3_process.replace(stormdll, "Storm.dll");
				}
			}
		}
		catch (...) {
		}

		if (fs::exists(inject_dll))
		{
			cmd.add(L"ydwe", ydwe.wstring());
			warcraft3_process.inject(inject_dll);			
		}

		cmd.app(war3_path.wstring());
		cmd.del(L"war3");
		return warcraft3_process.create(war3_path, cmd.str());
	}
	catch (...) {
	}

	return false;
}
