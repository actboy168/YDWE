#pragma once

#include <windows.h>

#include <base/filesystem.h>
#include <base/thread/thread.h>
#include <atomic>
#include <map>	 

class DllModule
{
public:
	DllModule();

	void Attach();
	void Detach();
	void SetWindow(HWND hwnd);

	HMODULE  hGameDll;
	HWND     hWar3Wnd;
	bool     IsWindowMode;
	bool     IsAuto;
	bool     IsFullWindowedMode;
	bool     IsFixedRatioWindowed;
	bool     IsWideScreenSupport;
	fs::path ydwe_path;
};

extern DllModule g_DllMod;
