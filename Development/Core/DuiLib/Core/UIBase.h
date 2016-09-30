#ifndef __UIBASE_H__
#define __UIBASE_H__


#pragma once


#include <atlstdthunk.h>

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#ifndef DUITRACE
#define DUITRACE DUI__Trace
#endif
#define DUITRACEMSG DUI__TraceMsg
#else
#ifndef DUITRACE
#define DUITRACE
#endif
#define DUITRACEMSG L""
#endif

#ifdef _DEBUG
UILIB_API void  DUI__Trace(const wchar_t* pstrFormat, ...);
UILIB_API const wchar_t*  DUI__TraceMsg(UINT uMsg);
#endif

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CNotifyPump
{
public:
	bool AddVirtualWnd(const std::wstring& strName,CNotifyPump* pObject);
	bool RemoveVirtualWnd(const std::wstring& strName);
	void NotifyPump(TNotifyUI& msg);
	bool LoopDispatch(TNotifyUI& msg);
	DUI_DECLARE_MESSAGE_MAP()
private:
	CStdStringPtrMap<CNotifyPump> m_VirtualWndMap;
};

class UILIB_API CWindowWnd
{
public:
    CWindowWnd();

    HWND GetHWND() const;
    operator HWND() const;

    bool RegisterWindowClass();

    HWND Create(HWND hwndParent, const wchar_t* pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
    HWND Create(HWND hwndParent, const wchar_t* pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
    HWND CreateDuiWindow(HWND hwndParent, const wchar_t* pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);
    void ShowWindow(bool bShow = true, bool bTakeFocus = true);
    UINT ShowModal();
    void Close(UINT nRet = IDOK);
    void CenterWindow();
    void SetIcon(UINT nRes);

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    void ResizeClient(int cx = -1, int cy = -1);

protected:
    virtual const wchar_t* GetWindowClassName() const = 0;
    virtual UINT GetClassStyle() const;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnFinalMessage(HWND hWnd);

    static LRESULT CALLBACK StartWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg,  WPARAM wParam, LPARAM lParam);
protected:
	HWND m_hWnd;
#pragma warning(push)
#pragma warning(disable:4251)
	ATL::CStdCallThunk m_thunk;
#pragma warning(pop)
};

} // namespace DuiLib

#endif // __UIBASE_H__
