#include "DllModule.h"
#include <Windows.h>

static BOOL IsForegroundWindow(HWND hWar3Wnd)
{
	HWND hWnd = ::GetParent(hWar3Wnd);
	if (NULL == hWnd)
	{
		return (::GetForegroundWindow() == hWar3Wnd);
	}
	else
	{
		return (::GetForegroundWindow() == hWnd);
	}
}

void LockingMouse()
{
	static BOOL s_bClipped = FALSE;

	if ((g_DllMod.IsWindowMode)                     // 窗口模式
		&& (g_DllMod.IsLockingMouse)                // 开启窗口魔兽锁鼠标
		&& (g_DllMod.hWar3Wnd != NULL)              // 魔兽窗口存在
		&& IsForegroundWindow(g_DllMod.hWar3Wnd))   // 魔兽窗口处于激活状态
	{
		RECT rcClient;
		POINT Point;
		::GetCursorPos(&Point);
		::GetClientRect(g_DllMod.hWar3Wnd, &rcClient);
		::ClientToScreen(g_DllMod.hWar3Wnd, (LPPOINT)&rcClient); 
		::ClientToScreen(g_DllMod.hWar3Wnd, (LPPOINT)&rcClient+1);
		if(::PtInRect(&rcClient, Point))
		{
			::ClipCursor(&rcClient);
			s_bClipped = TRUE;
		}
	}
	else if(s_bClipped)
	{
		::ClipCursor(NULL);
		s_bClipped = FALSE;
	}
}
