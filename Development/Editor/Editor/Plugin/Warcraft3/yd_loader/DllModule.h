#pragma once

#include <windows.h>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
namespace fs = boost::filesystem;

class DllModule
{
public:
	DllModule();

	void Attach();
	void Detach();
	void ThreadStart();
	void ThreadStop();
	void LoadPlugins();
	bool SearchPatch(fs::path& result, std::wstring const& fv_str);

	HMODULE  hGameDll;
	HWND     hWar3Wnd;
	bool     IsWindowMode;
	bool     IsFullWindowedMode;
	bool     IsLockingMouse;
	bool     IsFixedRatioWindowed;
	fs::path patch_path;
	fs::path ydwe_path;

private:
	boost::shared_ptr<boost::thread> daemon_thread_;
};

extern DllModule g_DllMod;
