#include <base/util/console.h>
#include <windows.h>
#include <cstdio>

namespace base { namespace console {

	void enable()
	{
		FILE* new_file;
		::AllocConsole(); 
		freopen_s(&new_file, "CONIN$",  "r", stdin);
		freopen_s(&new_file, "CONOUT$", "w", stdout);
		freopen_s(&new_file, "CONOUT$", "w", stderr);
	}

	bool disable_close_button()
	{
		wchar_t buffer[MAX_PATH];
		
		if (!::GetConsoleTitleW(buffer, MAX_PATH))
		{
			return false;
		}

		HWND hwnd = ::FindWindowW(NULL, buffer);
		if (!hwnd)
		{
			return false;
		}

		HMENU hmenu = ::GetSystemMenu(hwnd, FALSE);   
		return !!::RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	}

	void pause()
	{
		system("pause");
	}
}}
