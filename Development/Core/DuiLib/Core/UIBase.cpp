#include "StdAfx.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

namespace ATL
{
	void * __stdcall __AllocStdCallThunk()
	{
		void* ptr = ::HeapAlloc(::GetProcessHeap(), 0, sizeof(_stdcallthunk));		
		if (!ptr)
			throw std::domain_error("HeapAlloc failed.");
		DWORD dwOldProtect;
		if (!::VirtualProtect(ptr, sizeof(_stdcallthunk), PAGE_EXECUTE_READWRITE, &dwOldProtect)) 
			throw std::domain_error("VirtualProtect failed.");
		return ptr;

	} 
	
	void __stdcall __FreeStdCallThunk(void *p)
	{
		//::VirtualFree(p, sizeof(_stdcallthunk), 0);
		::HeapFree(::GetProcessHeap(), 0, p);
	}
};

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

#ifdef _DEBUG
void UILIB_API DUI__Trace(const wchar_t* pstrFormat, ...)
{
    wchar_t szBuffer[300] = { 0 };
    va_list args;
    va_start(args, pstrFormat);
    ::wvnsprintfW(szBuffer, lengthof(szBuffer) - 2, pstrFormat, args);
    wcscat(szBuffer, L"\n");
    va_end(args);
    ::OutputDebugStringW(szBuffer);
}

const wchar_t* DUI__TraceMsg(UINT uMsg)
{
#define MSGDEF(x) if(uMsg==x) return L ## #x
    MSGDEF(WM_SETCURSOR);
    MSGDEF(WM_NCHITTEST);
    MSGDEF(WM_NCPAINT);
    MSGDEF(WM_PAINT);
    MSGDEF(WM_ERASEBKGND);
    MSGDEF(WM_NCMOUSEMOVE);  
    MSGDEF(WM_MOUSEMOVE);
    MSGDEF(WM_MOUSELEAVE);
    MSGDEF(WM_MOUSEHOVER);   
    MSGDEF(WM_NOTIFY);
    MSGDEF(WM_COMMAND);
    MSGDEF(WM_MEASUREITEM);
    MSGDEF(WM_DRAWITEM);   
    MSGDEF(WM_LBUTTONDOWN);
    MSGDEF(WM_LBUTTONUP);
    MSGDEF(WM_LBUTTONDBLCLK);
    MSGDEF(WM_RBUTTONDOWN);
    MSGDEF(WM_RBUTTONUP);
    MSGDEF(WM_RBUTTONDBLCLK);
    MSGDEF(WM_SETFOCUS);
    MSGDEF(WM_KILLFOCUS);  
    MSGDEF(WM_MOVE);
    MSGDEF(WM_SIZE);
    MSGDEF(WM_SIZING);
    MSGDEF(WM_MOVING);
    MSGDEF(WM_GETMINMAXINFO);
    MSGDEF(WM_CAPTURECHANGED);
    MSGDEF(WM_WINDOWPOSCHANGED);
    MSGDEF(WM_WINDOWPOSCHANGING);   
    MSGDEF(WM_NCCALCSIZE);
    MSGDEF(WM_NCCREATE);
    MSGDEF(WM_NCDESTROY);
    MSGDEF(WM_TIMER);
    MSGDEF(WM_KEYDOWN);
    MSGDEF(WM_KEYUP);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SYSKEYDOWN);
    MSGDEF(WM_SYSKEYUP);
    MSGDEF(WM_SYSCOMMAND);
    MSGDEF(WM_SYSCHAR);
    MSGDEF(WM_VSCROLL);
    MSGDEF(WM_HSCROLL);
    MSGDEF(WM_CHAR);
    MSGDEF(WM_SHOWWINDOW);
    MSGDEF(WM_PARENTNOTIFY);
    MSGDEF(WM_CREATE);
    MSGDEF(WM_NCACTIVATE);
    MSGDEF(WM_ACTIVATE);
    MSGDEF(WM_ACTIVATEAPP);   
    MSGDEF(WM_CLOSE);
    MSGDEF(WM_DESTROY);
    MSGDEF(WM_GETICON);   
    MSGDEF(WM_GETTEXT);
    MSGDEF(WM_GETTEXTLENGTH);   
    static wchar_t szMsg[10];
    ::wsprintfW(szMsg, L"0x%04X", uMsg);
    return szMsg;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
//
//

//////////////////////////////////////////////////////////////////////////
//
DUI_BASE_BEGIN_MESSAGE_MAP(CNotifyPump)
DUI_END_MESSAGE_MAP()

static const DUI_MSGMAP_ENTRY* DuiFindMessageEntry(const DUI_MSGMAP_ENTRY* lpEntry,TNotifyUI& msg )
{
	std::wstring const& sMsgType = msg.sType;
	std::wstring const& sCtrlName = msg.pSender->GetName();
	const DUI_MSGMAP_ENTRY* pMsgTypeEntry = NULL;
	while (lpEntry->nSig != DuiSig_end)
	{
		if (lpEntry->sMsgType==sMsgType)
		{
			if(!lpEntry->sCtrlName.empty())
			{
				if(lpEntry->sCtrlName == sCtrlName)
				{
					return lpEntry;
				}
			}
			else
			{
				pMsgTypeEntry = lpEntry;
			}
		}
		lpEntry++;
	}
	return pMsgTypeEntry;
}

bool CNotifyPump::AddVirtualWnd(const std::wstring& strName,CNotifyPump* pObject)
{
	return m_VirtualWndMap.Insert(strName.c_str(), pObject);
}

bool CNotifyPump::RemoveVirtualWnd(const std::wstring& strName)
{
	return m_VirtualWndMap.Remove(strName.c_str());
}

bool CNotifyPump::LoopDispatch(TNotifyUI& msg)
{
	const DUI_MSGMAP_ENTRY* lpEntry = NULL;
	const DUI_MSGMAP* pMessageMap = NULL;

#ifndef UILIB_STATIC
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)())
#else
	for(pMessageMap = GetMessageMap(); pMessageMap!=NULL; pMessageMap = pMessageMap->pBaseMap)
#endif
	{
#ifndef UILIB_STATIC
		ASSERT(pMessageMap != (*pMessageMap->pfnGetBaseMap)());
#else
		ASSERT(pMessageMap != pMessageMap->pBaseMap);
#endif
		if ((lpEntry = DuiFindMessageEntry(pMessageMap->lpEntries,msg)) != NULL)
		{
			goto LDispatch;
		}
	}
	return false;

LDispatch:
	union DuiMessageMapFunctions mmf;
	mmf.pfn = lpEntry->pfn;

	bool bRet = false;
	int nSig;
	nSig = lpEntry->nSig;
	switch (nSig)
	{
	default:
		ASSERT(FALSE);
		break;
	case DuiSig_lwl:
		(this->*mmf.pfn_Notify_lwl)(msg.wParam,msg.lParam);
		bRet = true;
		break;
	case DuiSig_vn:
		(this->*mmf.pfn_Notify_vn)(msg);
		bRet = true;
		break;
	}
	return bRet;
}

void CNotifyPump::NotifyPump(TNotifyUI& msg)
{
	///遍历虚拟窗口
	if( !msg.sVirtualWnd.empty() )
	{
		foreach (auto it, m_VirtualWndMap) 
		{
			std::wstring const& key = it.first;
			if(key == msg.sVirtualWnd)
			{
				CNotifyPump* pObject = it.second;
				if (pObject && pObject->LoopDispatch(msg))
					return;
			}
		}
	}

	///
	//遍历主窗口
	LoopDispatch( msg );
}

//////////////////////////////////////////////////////////////////////////
///
CWindowWnd::CWindowWnd() : m_hWnd(NULL)
{
}

HWND CWindowWnd::GetHWND() const 
{ 
    return m_hWnd; 
}

UINT CWindowWnd::GetClassStyle() const
{
    return 0;
}

CWindowWnd::operator HWND() const
{
    return m_hWnd;
}

HWND CWindowWnd::CreateDuiWindow( HWND hwndParent, const wchar_t* pstrWindowName,DWORD dwStyle /*=0*/, DWORD dwExStyle /*=0*/ )
{
	return Create(hwndParent,pstrWindowName,dwStyle,dwExStyle,0,0,0,0,NULL);
}

HWND CWindowWnd::Create(HWND hwndParent, const wchar_t* pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
{
    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CWindowWnd::Create(HWND hwndParent, const wchar_t* pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
{
    if (!RegisterWindowClass()) return NULL;
	m_hWnd = ::CreateWindowExW(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, CPaintManagerUI::GetInstance(), this);
	ASSERT(m_hWnd!=NULL);
    return m_hWnd;
}

void CWindowWnd::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= false*/)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    ::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

UINT CWindowWnd::ShowModal()
{
    ASSERT(::IsWindow(m_hWnd));
    UINT nRet = 0;
    HWND hWndParent = GetWindowOwner(m_hWnd);
    ::ShowWindow(m_hWnd, SW_SHOWNORMAL);
    ::EnableWindow(hWndParent, FALSE);
    MSG msg = { 0 };
    while( ::IsWindow(m_hWnd) && ::GetMessageW(&msg, NULL, 0, 0) ) {
        if( msg.message == WM_CLOSE && msg.hwnd == m_hWnd ) {
            nRet = msg.wParam;
            ::EnableWindow(hWndParent, TRUE);
            ::SetFocus(hWndParent);
        }
        if( !CPaintManagerUI::TranslateMessage(&msg) ) {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        if( msg.message == WM_QUIT ) break;
    }
    ::EnableWindow(hWndParent, TRUE);
    ::SetFocus(hWndParent);
    if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
    return nRet;
}

void CWindowWnd::Close(UINT nRet)
{
    ASSERT(::IsWindow(m_hWnd));
    if( !::IsWindow(m_hWnd) ) return;
    PostMessage(WM_CLOSE, (WPARAM)nRet, 0L);
}

void CWindowWnd::CenterWindow()
{
    ASSERT(::IsWindow(m_hWnd));
    ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
    RECT rcDlg = { 0 };
    ::GetWindowRect(m_hWnd, &rcDlg);
    RECT rcArea = { 0 };
    RECT rcCenter = { 0 };
    HWND hWndCenter = ::GetWindowOwner(m_hWnd);
    ::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
    if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

    int DlgWidth = rcDlg.right - rcDlg.left;
    int DlgHeight = rcDlg.bottom - rcDlg.top;

    // Find dialog's upper left based on rcCenter
    int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
    int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

    // The dialog is outside the screen, move it inside
    if( xLeft < rcArea.left ) xLeft = rcArea.left;
    else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
    if( yTop < rcArea.top ) yTop = rcArea.top;
    else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
    ::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CWindowWnd::SetIcon(UINT nRes)
{
    HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
    ASSERT(hIcon);
    ::SendMessageW(m_hWnd, WM_SETICON, (WPARAM) TRUE, (LPARAM) hIcon);
    hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nRes), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    ASSERT(hIcon);
    ::SendMessageW(m_hWnd, WM_SETICON, (WPARAM) FALSE, (LPARAM) hIcon);
}

bool CWindowWnd::RegisterWindowClass()
{
    WNDCLASSW wc = { 0 };
    wc.style = GetClassStyle();
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.lpfnWndProc = CWindowWnd::StartWndProc;
    wc.hInstance = CPaintManagerUI::GetInstance();
    wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = GetWindowClassName();
    ATOM ret = ::RegisterClassW(&wc);
    ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
    return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CWindowWnd::WindowProc(HWND hWnd, UINT uMsg,  WPARAM wParam, LPARAM lParam)
{
	CWindowWnd* pThis = reinterpret_cast<CWindowWnd*>(hWnd);

	ASSERT(pThis);
	if (!pThis) { return 0; }

	ASSERT(pThis->m_hWnd != NULL);
	if (!pThis->m_hWnd) { return 0; }

	if (uMsg == WM_NCDESTROY)
	{
		LRESULT lRes = ::DefWindowProcW(pThis->m_hWnd, uMsg, wParam, lParam);
		pThis->m_hWnd = NULL;
		pThis->OnFinalMessage(hWnd);
		return lRes;
	}
	else
	{
		return pThis->HandleMessage(uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK CWindowWnd::StartWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT (uMsg == WM_NCCREATE);
	LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
	CWindowWnd* pThis = static_cast<CWindowWnd*>(lpcs->lpCreateParams);
	pThis->m_hWnd = hWnd;
	pThis->m_thunk.Init((DWORD_PTR)CWindowWnd::WindowProc, pThis);
	WNDPROC pProc = (WNDPROC)pThis->m_thunk.GetCodeAddress();
	::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)pProc);
	return pProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CWindowWnd::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::SendMessageW(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT CWindowWnd::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
    ASSERT(::IsWindow(m_hWnd));
    return ::PostMessageW(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
    ASSERT(::IsWindow(m_hWnd));
    RECT rc = { 0 };
    if( !::GetClientRect(m_hWnd, &rc) ) return;
    if( cx != -1 ) rc.right = cx;
    if( cy != -1 ) rc.bottom = cy;
    if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
    ::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

LRESULT CWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}

void CWindowWnd::OnFinalMessage(HWND /*hWnd*/)
{
}

} // namespace DuiLib
