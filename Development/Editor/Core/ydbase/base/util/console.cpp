#include <base/util/console.h>
#include <windows.h>
#include <cstdio>

namespace base { namespace console {

	void enable()
	{
		HWND h = ::GetConsoleWindow();

		if (h)
		{
			::ShowWindow(h, SW_SHOW);
		}
		else
		{
			FILE* new_file;
			::AllocConsole();
			freopen_s(&new_file, "CONIN$", "r", stdin);
			freopen_s(&new_file, "CONOUT$", "w", stdout);
			freopen_s(&new_file, "CONOUT$", "w", stderr);
		}
	}

	void disable()
	{
		::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	}

	bool disable_close_button()
	{
		HWND h = ::GetConsoleWindow();
		if (!h)
		{
			return false;
		}

		HMENU hmenu = ::GetSystemMenu(h, FALSE);   
		return !!::RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	}

	void pause()
	{
		system("pause");
	}
}}
