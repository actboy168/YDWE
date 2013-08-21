#pragma once

#include <Windows.h>
#include <cstdint>

class FullWindowedMode
{
public:
    FullWindowedMode();
    ~FullWindowedMode();

public:
    void Start();
	void Stop();
	static void FixedRatio(HWND hwnd, uint32_t max_x, uint32_t max_y);

private:
    DWORD           m_nStyle;
    WINDOWPLACEMENT m_Wndpl;
    HWND            m_hBlackWindow;
    static BOOL     m_bKillWar3;

private:
    HWND CreateBlackWindow();
    static LRESULT CALLBACK BlackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void KillWar3();
};
