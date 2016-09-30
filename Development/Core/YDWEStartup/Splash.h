#pragma once

#include <Windows.h>

class CSplash
{
public:
    CSplash();
    virtual ~CSplash();
	void     Show();
	bool     Close();
    int      DoLoop();
	LRESULT  WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			 
	bool     SetBitmap(const wchar_t* filename);
	bool     SetTransparentColor(COLORREF col);
	bool     SetText(const wchar_t* text, int x, int y, int w, int h);
			 
private:	 
    void     OnPaint(HWND hwnd);
    bool     MakeTransparent();
	HWND     RegAndCreateWindow();
	void     FreeResources();

private:
	HWND     m_hwnd;
	COLORREF m_colTrans;
	DWORD    m_dwWidth;
	DWORD    m_dwHeight;
    HBITMAP  m_hBitmap;
};
