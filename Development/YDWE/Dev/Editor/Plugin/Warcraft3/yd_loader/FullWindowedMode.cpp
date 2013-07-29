#include "FullWindowedMode.h"
#include "DllModule.h"

BOOL FullWindowedMode::m_bKillWar3 = TRUE;

FullWindowedMode::FullWindowedMode() 
	: m_hBlackWindow(NULL)
	, m_nStyle(0)
{ }

FullWindowedMode::~FullWindowedMode()
{ }


void FullWindowedMode::Start()
{
	if (m_hBlackWindow != NULL)
		return ;

	if (g_DllMod.hWar3Wnd != NULL)
	{
		::GetWindowPlacement(g_DllMod.hWar3Wnd, &m_Wndpl);

		m_nStyle = ::GetWindowLong(g_DllMod.hWar3Wnd, GWL_STYLE);
		DWORD dwStyle = m_nStyle;
		dwStyle &= ~(WS_CAPTION | WS_SIZEBOX);
		if (g_DllMod.IsFixedRatioWindowed)
		{
			dwStyle |= (WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		}
		::SetWindowLong(g_DllMod.hWar3Wnd, GWL_STYLE, dwStyle);

		if (g_DllMod.IsFixedRatioWindowed)
		{
			m_hBlackWindow = CreateBlackWindow();
			::SetParent(g_DllMod.hWar3Wnd, m_hBlackWindow);
			::SetFocus(g_DllMod.hWar3Wnd);
			FixedRatio(g_DllMod.hWar3Wnd, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
			m_bKillWar3 = TRUE;
		}
		else
		{
			::SetWindowPos(g_DllMod.hWar3Wnd, HWND_TOP, 0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW|SWP_DRAWFRAME);
		}
	}
}

void FullWindowedMode::Stop()
{
	if (m_hBlackWindow == NULL)
		return ;

	if (g_DllMod.hWar3Wnd != NULL)
	{
		m_bKillWar3 = FALSE;
		::SetParent(g_DllMod.hWar3Wnd, NULL);
		::SetWindowLong(g_DllMod.hWar3Wnd, GWL_STYLE,  m_nStyle);
		::SetWindowPlacement(g_DllMod.hWar3Wnd, &m_Wndpl);
		::PostMessage(m_hBlackWindow, WM_CLOSE, 0, 0);
		m_hBlackWindow = NULL;
	}
}

void FullWindowedMode::FixedRatio(HWND hwnd, uint32_t max_x, uint32_t max_y)
{
#define FIXED_RATIO_WINDOW_X2Y(x) ((x)*3/4)
#define FIXED_RATIO_WINDOW_Y2X(y) ((y)*4/3)

	if (max_x/4 > max_y/3)
	{
		uint32_t fixed_ratio_x = FIXED_RATIO_WINDOW_Y2X(max_y);
		uint32_t fixed_ratio_y = max_y;

		::SetWindowPos(hwnd, HWND_TOP, (max_x-fixed_ratio_x)/2, 0, fixed_ratio_x, fixed_ratio_y, SWP_SHOWWINDOW|SWP_DRAWFRAME);
	}
	else
	{
		uint32_t fixed_ratio_x = max_x;
		uint32_t fixed_ratio_y = FIXED_RATIO_WINDOW_X2Y(max_x);

		::SetWindowPos(hwnd, HWND_TOP, 0, (max_y-fixed_ratio_y)/2, fixed_ratio_x, fixed_ratio_y, SWP_SHOWWINDOW|SWP_DRAWFRAME);
	}
#undef FIXED_RATIO_WINDOW_X2Y
#undef FIXED_RATIO_WINDOW_Y2X
}

HWND FullWindowedMode::CreateBlackWindow()
{
	WNDCLASSEX wc;
	HWND hwBG;
	RECT stRectDeskTop;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style  = CS_HREDRAW  | CS_VREDRAW;
	wc.lpfnWndProc = BlackWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HMODULE)::GetClassLong(g_DllMod.hWar3Wnd, GCL_HMODULE);
	wc.hbrBackground = (HBRUSH)(COLOR_BTNTEXT+1); 
	wc.lpszMenuName = NULL;
	wc.lpszClassName = TEXT("Black Warcraft III");
	wc.hIcon   = (HICON)::GetClassLong(g_DllMod.hWar3Wnd, GCL_HICON);
	wc.hIconSm = (HICON)::GetClassLong(g_DllMod.hWar3Wnd, GCL_HICONSM);
	wc.hCursor = (HCURSOR)::GetClassLong(g_DllMod.hWar3Wnd, GCL_HCURSOR);

	::RegisterClassEx(&wc);
	::GetWindowRect(::GetDesktopWindow(), &stRectDeskTop);
	hwBG = ::CreateWindowEx(0, TEXT("Black Warcraft III"), TEXT("Warcraft III"),
		WS_POPUP | WS_SYSMENU,
		0, 0, stRectDeskTop.right, stRectDeskTop.bottom,
		NULL, NULL,  NULL, NULL);

	::ShowWindow(hwBG, SW_SHOWNORMAL);
	::UpdateWindow(hwBG);

	return hwBG;
}

LRESULT CALLBACK FullWindowedMode::BlackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SETFOCUS:
		{
			::SetFocus(g_DllMod.hWar3Wnd);
			// win7下 alt+tab切换到桌面，会导致子窗口收不到[WM_KEYUP, VK_MENU]消息
			::PostMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_MENU, 1);
			::PostMessage(g_DllMod.hWar3Wnd, WM_KEYUP,   VK_MENU, 1);
		}
		return TRUE;
	case WM_DESTROY:
		if (FullWindowedMode::m_bKillWar3)
		{
			KillWar3();
		}
		break;
	}
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void FullWindowedMode::KillWar3()
{
	if (g_DllMod.hWar3Wnd != NULL)
	{
		//窗口置顶
		::SetForegroundWindow(g_DllMod.hWar3Wnd);
		::BringWindowToTop(g_DllMod.hWar3Wnd);

		//F10
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_F10, 0);
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP,   VK_F10, 0);
		::Sleep(500);
		//E
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, 'E', 0);
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP,   'E', 0);
		::Sleep(500);
		//Q
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, 'Q', 0);
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP,   'Q', 0);
		::Sleep(1000);

		::SendMessage(g_DllMod.hWar3Wnd, WM_CLOSE,     0, 0);
		::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, 'X', 0);
	}
}
