#include <base/filesystem.h>
#include <base/file/stream.h>
#include <base/path/ydwe.h>
#include <bee/utility/path_helper.h>
#include <base/util/ini.h>
#include <fmt/format.h>
#include <bee/registry.h> 
#include <base/hook/fp_call.h>
#include <bee/subprocess.h>
#include <bee/utility/unicode_win.h>
#include <base/hook/injectdll.h>
#include <base/hook/replacedll.h>

#define YDWE_WAR3_INLINE
#include <warcraft3/directory.h>
#include <warcraft3/command_line.h>

static std::wstring get_test_map_path()
{
	std::wstring result = L"Maps\\Test\\WorldEditTestMap";
	try {
		result = bee::registry::key_w(L"HKEY_CURRENT_USER\\Software\\Blizzard Entertainment\\WorldEdit")[L"Test Map - Copy Location"].get_string();
	}
	catch (bee::registry::registry_exception const&) {}
	return std::move(result);
}

static bool launch_taskbar_support(const fs::path& ydwe_path)
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

static bool is_lnimap(const fs::path& from)
{
	bool res = false;
	FILE* f = 0;
	if (_wfopen_s(&f, from.c_str(), L"rb") == 0 && f) {
		fseek(f, 8, SEEK_SET);
		uint32_t mark = 0;
		if (1 == fread(&mark, sizeof mark, 1, f) && mark == '\1L2W') {
			res = true;
		}
		fclose(f);
	}
	return res;
}

static bool map_convert(const fs::path& ydwe, const fs::path& from, const fs::path& to, const char* mode)
{
	fs::path ydwedev = base::path::ydwe(true);
	fs::path app = ydwe / L"bin" / L"lua.exe";
	bee::subprocess::spawn spawn;
	spawn.set_console(bee::subprocess::console::eDisable);
	spawn.env_set(L"PATH", (ydwe / L"bin").wstring());
    bee::subprocess::args_t agrs;
	if (!spawn.exec(
		std::vector<std::wstring> {
			app.wstring(),
			L"-e",
			fmt::format(L"package.cpath = [[{}]]", (ydwe / L"bin" / L"?.dll").wstring()),
			L"gui\\mini.lua",
			bee::u2w(mode),
			from.wstring(),
			to.wstring(),
		},
		(ydwedev / L"plugin" / L"w3x2lni" / L"script").c_str()
	)) {
		return false;
	}
	return bee::subprocess::process(spawn).wait() == 0;
}

static void map_build(const fs::path& ydwe, const fs::path& from, const fs::path& to, bool slk)
{
	if (is_lnimap(from)) {
		if (slk) {
			if (!map_convert(ydwe, from, to, "slk")) {
				// TODO: ERROR
				fs::copy_file(from, to, fs::copy_options::overwrite_existing);
			}
		}
		else {
			if (!map_convert(ydwe, from, to, "obj")) {
				// TODO: ERROR
				fs::copy_file(from, to, fs::copy_options::overwrite_existing);
			}
		}
	}
	else {
		if (slk) {
			if (!map_convert(ydwe, from, to, "slk")) {
				fs::copy_file(from, to, fs::copy_options::overwrite_existing);
			}
		}
		else {
			fs::copy_file(from, to, fs::copy_options::overwrite_existing);
		}
	}
}

bool launch_warcraft3(warcraft3::command_line& cmd)
{
	try {
		fs::path ydwe = base::path::ydwe(false);
		launch_taskbar_support(ydwe);

		fs::path war3_path;
		if (!warcraft3::directory::get(nullptr, war3_path))
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
			if (!bee::path_helper::equal(loadfile.extension(), L".w3g"))
			{
				fs::path test_map_path = get_test_map_path() + L".w3x";
				try {
					cmd[L"loadfile"] = test_map_path.wstring();
					if (!loadfile.is_absolute()) {
						loadfile = war3_path / loadfile;
					}
					if (cmd.has(L"closew2l")) {
						fs::copy_file(loadfile, war3_path / test_map_path, fs::copy_options::overwrite_existing);
					}
					else {
						map_build(ydwe, loadfile, war3_path / test_map_path, "0" != table["MapTest"]["EnableMapSlk"]);
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
			bee::registry::key_a key("HKEY_CURRENT_USER\\Software\\Blizzard Entertainment\\Warcraft III\\String");
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

		bee::subprocess::spawn spawn;
		spawn.suspended();

		if (fs::exists(inject_dll))
		{
			cmd.add(L"ydwe", ydwe.wstring());
		}
		cmd.app(war3_path.wstring());
		cmd.del(L"war3");
		cmd.del(L"closew2l");
		if (!spawn.exec(cmd.args(), 0)) {
			return false;
		}

		bee::subprocess::process process(spawn);
		try {
			if (table["War3Patch"]["Option"] == "2")
			{
				fs::path stormdll = ydwe / L"share" / L"patch" / table["War3Patch"]["DirName"] / L"Storm.dll";
				if (fs::exists(stormdll))
				{
					base::hook::replacedll(process.info(), "Storm.dll", stormdll.string().c_str());
				}
			}
		}
		catch (...) {
		}
		if (fs::exists(inject_dll))
		{
			base::hook::injectdll(process.info(), inject_dll.wstring(), std::wstring());
		}
		process.resume();
		return true;
		
	}
	catch (...) {
	}

	return false;
}
