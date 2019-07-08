#pragma once

#include <windows.h>

class DllModule
{
public:
	DllModule();

	void Attach();
	void Detach();
	void SetWindow(HWND hwnd);

	HMODULE  hGameDll;
	HWND     hWar3Wnd;
	bool     IsAuto;
};

extern DllModule g_DllMod;
