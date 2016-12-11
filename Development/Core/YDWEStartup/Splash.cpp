#include "splash.h"
#include <string>

static LRESULT CALLBACK ExtWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CSplash * spl = NULL;
	if (uMsg == WM_CREATE)
	{
		spl = (CSplash*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}
	if (spl)
		return spl->WindowProc(hwnd, uMsg, wParam, lParam);
	else
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static std::wstring s_SplashClassName = L"SPLASH" + std::to_wstring((long long)ExtWndProc);

LRESULT CSplash::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT: 
		OnPaint(hwnd); 
		return 0;
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void CSplash::OnPaint(HWND hwnd)
{
	if (!m_hBitmap)
		return;

	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hwnd, &ps);

	RECT rect;
	::GetClientRect(m_hwnd, &rect);

	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hCloneDC, m_hBitmap);
	::BitBlt(hDC, 0, 0, m_dwWidth, m_dwHeight, hCloneDC, 0, 0, SRCCOPY);
	::SelectObject(hCloneDC, hOldBitmap);
	::DeleteDC(hCloneDC);

	EndPaint(hwnd, &ps);
}

CSplash::CSplash()
	: m_hwnd(NULL)
	, m_colTrans(0)
	, m_dwWidth(0)
	, m_dwHeight(0)
	, m_hBitmap(NULL)
{ }

CSplash::~CSplash()
{
	Close();
	FreeResources();
}

HWND CSplash::RegAndCreateWindow()
{
	WNDCLASSEXW wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	wndclass.lpfnWndProc = ExtWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = ::GetModuleHandleW(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = ::LoadCursorW(NULL, MAKEINTRESOURCEW(32514)/*IDC_WAIT*/);
	wndclass.hbrBackground = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = s_SplashClassName.c_str();
	wndclass.hIconSm = NULL;

	if (!RegisterClassExW(&wndclass))
		return NULL;

	DWORD nScrWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	DWORD nScrHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);

	int x = (nScrWidth - m_dwWidth) / 2;
	int y = (nScrHeight - m_dwHeight) / 2;
	m_hwnd = ::CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, s_SplashClassName.c_str(), L"Banner", WS_POPUP, x, y, m_dwWidth, m_dwHeight, NULL, NULL, NULL, this);

	if (m_hwnd)
	{
		MakeTransparent();
		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);
	}
	return m_hwnd;
}

int CSplash::DoLoop()
{
	if (!m_hwnd)
		Show();
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return msg.wParam;
}

void CSplash::Show()
{
	Close();
	RegAndCreateWindow();
}

HBITMAP LoadBlp(HDC hdc, const wchar_t* filename);

bool CSplash::SetBitmap(const wchar_t* filename)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImageW(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  

	FreeResources();

	if (hBitmap)
	{
		BITMAP  csBitmapSize;
		m_hBitmap = hBitmap;
		int nRetValue = ::GetObjectW(hBitmap, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return false;
		}
		m_dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_dwHeight = (DWORD)csBitmapSize.bmHeight;
	}
	return true;
}

bool CSplash::SetText(const wchar_t* text, int x, int y, int w, int h)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = m_dwWidth;
	rect.bottom = m_dwHeight;

	HDC hDC = ::CreateCompatibleDC(0);

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDC, m_hBitmap);

	::SetBkMode(hDC, TRANSPARENT);
	::SetTextColor(hDC, RGB(255, 255, 255));
	LOGFONTW lf = { 0 };
	::GetObjectW(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	lf.lfHeight = h;
	lf.lfWidth = w;
	HFONT hOldFont = (HFONT)::SelectObject(hDC, CreateFontIndirectW(&lf));
	::DrawTextW(hDC, text, -1, &rect, DT_LEFT);
	::SelectObject(hDC, hOldFont);

	::SelectObject(hDC, hOldBitmap);
	::DeleteDC(hDC);

	return true;
}

void CSplash::FreeResources()
{
	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
	m_hBitmap = NULL;
}

bool CSplash::Close()
{
	if (m_hwnd)
	{
		::DestroyWindow(m_hwnd);
		m_hwnd = 0;
		::UnregisterClassW(s_SplashClassName.c_str(), ::GetModuleHandleW(NULL));
		return true;
	}
	return false;
}

bool CSplash::SetTransparentColor(COLORREF col)
{
	m_colTrans = col;
	return MakeTransparent();
}

bool CSplash::MakeTransparent()
{
	if (m_hwnd && m_colTrans)
	{
		::SetWindowLongW(m_hwnd, GWL_EXSTYLE, ::GetWindowLongW(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		::SetLayeredWindowAttributes(m_hwnd, m_colTrans, 0, LWA_COLORKEY);
	}
	return TRUE;
}
