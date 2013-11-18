#pragma once

#include <windows.h>

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <base/thread/thread.h>

namespace fs = boost::filesystem;

class DllModule
{
public:
	DllModule();

	void Attach();
	void Detach();
	void ThreadStart();
	void ThreadStop();
	void ThreadFunc();
	void LoadPlugins();
	bool SearchPatch(fs::path& result, std::wstring const& fv_str);

	HMODULE  hGameDll;
	HWND     hWar3Wnd;
	bool     IsWindowMode;
	bool     IsFullWindowedMode;
	bool     IsLockingMouse;
	bool     IsFixedRatioWindowed;
	bool     IsDisableSecurityAccess;
	fs::path patch_path;
	fs::path ydwe_path;

private:
	boost::shared_ptr<base::thread> daemon_thread_;
	boost::atomic<bool>             daemon_thread_exit_;
};

extern DllModule g_DllMod;
