#include <base/warcraft3/directory.h>	  	  		
#include <base/filesystem.h>
#include <base/win/registry/key.h>
#include <base/win/registry/value.h> 
#include <base/util/foreach.h>

namespace base { namespace warcraft3 { namespace directory {

	bool read_current_user(fs::path& result)
	{
		try {
			registry::key_w warkey = registry::current_user() / L"Software\\Blizzard Entertainment\\Warcraft III";
			result = warkey[L"InstallPath"].get_string();
			return true;
		}
		catch (...) {
		}

		return false;
	}

	// War3 1.26会自动将自身路径写入注册表，但写入的位置是HKEY_LOCAL_MACHINE
	bool read_all_users(fs::path& result)
	{
		try {
			
			registry::key_w warkey = registry::local_machine() / L"Software\\Blizzard Entertainment\\Warcraft III";
			result = warkey[L"InstallPath"].get_string();
			return true;
		}
		catch (...) {
		}

		return false;
	}

	bool validate(fs::path const& p)
	{
		if (!fs::exists(p / L"war3.exe")) return false;
		if (!fs::exists(p / L"game.dll")) return false;
		if (!fs::exists(p / L"war3.mpq")) return false;
		if (!fs::exists(p / L"war3patch.mpq")) return false;
		if (!fs::exists(p / L"storm.dll")) return false;
		return true;
	}

	bool write(fs::path const& p)
	{
		if (!fs::exists(p))
		{
			return false;
		}

		try {
			registry::key_w warkey = registry::current_user() / L"Software\\Blizzard Entertainment\\Warcraft III";
			warkey[L"InstallPath"] = p.wstring();
			return true;
		}
		catch (...) {
		}

		return false;
	}

	bool read(fs::path& result)
	{
		fs::path retval;
		if (read_current_user(retval) && validate(retval))
		{
			result = retval;
			return true;
		}

		if (read_all_users(retval) && validate(retval))
		{
			write(retval);
			result = retval;
			return true;
		}

		return false;
	}

	bool open_file_dialog(wchar_t const* title, wchar_t const* filter, HWND owner, std::wstring* path) 
	{
		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = owner;
		wchar_t filename[MAX_PATH];
		filename[0] = 0;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = _countof(filename);
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
		if (title) ofn.lpstrTitle = title;
		if (filter) ofn.lpstrFilter = filter;

		if (!::GetOpenFileNameW(&ofn))
		{
			return false;
		}

		*path = std::wstring(filename);
		return true;
	}

	bool choose(const wchar_t* title, fs::path& result)
	{
		std::wstring retval;
		if (!open_file_dialog(title, L"war3.exe\0war3.exe\0", NULL, &retval))
		{
			return false;
		}

		result = fs::path(retval).parent_path();
		return true;
	}

	bool choose(const wchar_t* title)
	{
		fs::path retval;
		if (choose(title, retval) && validate(retval))
		{
			return write(retval);
		}

		return false;
	}


	bool get(const wchar_t* title, fs::path& war3_path)
	{
		while (!read(war3_path))
		{
			fs::path result;
			if (!choose(title, result))
			{
				return false;
			}

			if (validate(result))
			{
				write(result);
			}
		}

		return true;
	}
}}}
