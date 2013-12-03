#include "StdAfx.h"

namespace DuiLib {

CControlUI::CControlUI() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_bSetPos(false),
m_chShortcut('\0'),
m_dwBackColor(0),
m_dwBackColor2(0),
m_dwBackColor3(0),
m_dwBorderColor(0),
m_dwFocusBorderColor(0),
m_nBorderSize(0)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(m_rcPadding));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
    ::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));
}

CControlUI::~CControlUI()
{
    if( OnDestroy ) OnDestroy(this);
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

std::wstring const& CControlUI::GetName() const
{
	return m_sName;
}

void CControlUI::SetName(const wchar_t* pstrName)
{
    m_sName = pstrName;
}

const wchar_t* CControlUI::GetClass() const
{
    return DUI_CTR_CONTROL;
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

bool CControlUI::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManagerUI* CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = pParent;
    if( bInit && m_pParent ) Init();
}

CControlUI* CControlUI::GetParent() const
{
    return m_pParent;
}

DWORD CControlUI::GetBkColor() const
{
    return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
    if( m_dwBackColor == dwBackColor ) return;

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if( m_dwBackColor2 == dwBackColor ) return;

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if( m_dwBackColor3 == dwBackColor ) return;

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if( m_dwBorderColor == dwBorderColor ) return;

    m_dwBorderColor = dwBorderColor;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusBorderColor;
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
    if( m_dwFocusBorderColor == dwBorderColor ) return;

    m_dwFocusBorderColor = dwBorderColor;
    Invalidate();
}

void CControlUI::SetBkImage(const wchar_t* pStrImage)
{
	m_sBkImage.reset(new CImage(pStrImage));
	Invalidate();
}

int CControlUI::GetBorderSize() const
{
    return m_nBorderSize;
}

void CControlUI::SetBorderSize(int nSize)
{
    if( m_nBorderSize == nSize ) return;

    m_nBorderSize = nSize;
    Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, const wchar_t* pStrImage, const wchar_t* pStrModify)
{
    return CRenderEngine::DrawImageString(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

bool CControlUI::DrawImage(HDC hDC, CImage const& cImage, const wchar_t* pStrModify)
{
	return cImage.Draw(hDC, m_pManager, m_rcItem, m_rcPaint, pStrModify);
}

const RECT& CControlUI::GetPos() const
{
    return m_rcItem;
}

void CControlUI::SetPos(RECT rc)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect invalidateRc = m_rcItem;
    if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

    m_rcItem = rc;
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
        if( OnSize ) OnSize(this);
        m_bSetPos = false;
    }
    
    if( m_bFloat ) {
        CControlUI* pParent = GetParent();
        if( pParent != NULL ) {
            RECT rcParentPos = pParent->GetPos();
            if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
            else m_cXY.cx = m_rcItem.right - rcParentPos.right;
            if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
            else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
            m_cxyFixed.cx = m_rcItem.right - m_rcItem.left;
            m_cxyFixed.cy = m_rcItem.bottom - m_rcItem.top;
        }
    }

    m_bUpdateNeeded = false;
    invalidateRc.Join(m_rcItem);

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while (pParent = pParent->GetParent())
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }
    m_pManager->Invalidate(invalidateRc);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
    return m_rcPadding;
}

void CControlUI::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
    return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{
    if( cx < 0 ) return; 
    m_cxyFixed.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
    if( cy < 0 ) return; 
    m_cxyFixed.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxWidth() const
{
    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxHeight() const
{
    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

void CControlUI::SetRelativePos(SIZE szMove,SIZE szZoom)
{
    m_tRelativePos.bRelative = TRUE;
    m_tRelativePos.nMoveXPercent = szMove.cx;
    m_tRelativePos.nMoveYPercent = szMove.cy;
    m_tRelativePos.nZoomXPercent = szZoom.cx;
    m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void CControlUI::SetRelativeParentSize(SIZE sz)
{
    m_tRelativePos.szParent = sz;
}

TRelativePosUI CControlUI::GetRelativePos() const
{
    return m_tRelativePos;
}

bool CControlUI::IsRelativePos() const
{
    return m_tRelativePos.bRelative;
}

std::wstring const& CControlUI::GetToolTip() const
{
	return m_sToolTip;
}

void CControlUI::SetToolTip(const wchar_t* pstrText)
{
    m_sToolTip = pstrText;
}

wchar_t CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(wchar_t ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

bool CControlUI::IsVisible() const
{

    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

bool CControlUI::ForeachControl(std::function<bool(CControlUI*)> Proc)
{
	return true;
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

void CControlUI::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::Init()
{
    DoInit();
    if( OnInit ) OnInit(this);
}

void CControlUI::DoInit()
{

}

void CControlUI::Event(TEventUI& event)
{
    if( OnEvent(&event) ) DoEvent(event);
}

void CControlUI::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void CControlUI::SetVirtualWnd(const wchar_t* pstrValue)
{
	m_sVirtualWnd = pstrValue;
	m_pManager->UsedVirtualWnd(true);
}

std::wstring const& CControlUI::GetVirtualWnd() const
{
	if (!m_sVirtualWnd.empty())
	{
		return m_sVirtualWnd;
	}
	else
	{
		CControlUI* pParent = GetParent();
		if (pParent != NULL)
		{
			return pParent->GetVirtualWnd();
		}
		else
		{
			static std::wstring s_dummy = L"";
			return s_dummy;
		}
	}
}

void CControlUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
{
	if (wcscmp(pstrName, L"class") == 0)
	{
		ApplyAttributeTable(m_pManager->GetDefaultAttributeList(pstrValue));
	}
	else if( wcscmp(pstrName, L"pos") == 0 ) {
        RECT rcPos = { 0 };
        wchar_t* pstr = NULL;
        rcPos.left = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPos.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPos.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPos.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
        SetFixedXY(szXY);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( wcscmp(pstrName, L"relativepos") == 0 ) {
        SIZE szMove,szZoom;
        wchar_t* pstr = NULL;
        szMove.cx = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        szMove.cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        szZoom.cx = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        szZoom.cy = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr); 
        SetRelativePos(szMove,szZoom);
    }
    else if( wcscmp(pstrName, L"padding") == 0 ) {
        RECT rcPadding = { 0 };
        wchar_t* pstr = NULL;
        rcPadding.left = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcPadding.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcPadding.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcPadding.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetPadding(rcPadding);
    }
    else if( wcscmp(pstrName, L"bkcolor") == 0 || wcscmp(pstrName, L"bkcolor1") == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNextW(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( wcscmp(pstrName, L"bkcolor2") == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNextW(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if( wcscmp(pstrName, L"bkcolor3") == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNextW(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
    else if( wcscmp(pstrName, L"bordercolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if( wcscmp(pstrName, L"focusbordercolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        SetFocusBorderColor(clrColor);
    }
    else if( wcscmp(pstrName, L"bordersize") == 0 ) SetBorderSize(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"borderround") == 0 ) {
        SIZE cxyRound = { 0 };
        wchar_t* pstr = NULL;
        cxyRound.cx = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
    else if( wcscmp(pstrName, L"bkimage") == 0 ) m_sBkImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"width") == 0 ) SetFixedWidth(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"height") == 0 ) SetFixedHeight(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"minwidth") == 0 ) SetMinWidth(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"minheight") == 0 ) SetMinHeight(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"maxwidth") == 0 ) SetMaxWidth(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"maxheight") == 0 ) SetMaxHeight(_wtoi(pstrValue));
    else if( wcscmp(pstrName, L"name") == 0 ) SetName(pstrValue);
    else if( wcscmp(pstrName, L"tooltip") == 0 ) SetToolTip(pstrValue);
    else if( wcscmp(pstrName, L"enabled") == 0 ) SetEnabled(wcscmp(pstrValue, L"true") == 0);
    else if( wcscmp(pstrName, L"mouse") == 0 ) SetMouseEnabled(wcscmp(pstrValue, L"true") == 0);
	else if( wcscmp(pstrName, L"keyboard") == 0 ) SetKeyboardEnabled(wcscmp(pstrValue, L"true") == 0);
    else if( wcscmp(pstrName, L"visible") == 0 ) SetVisible(wcscmp(pstrValue, L"true") == 0);
    else if( wcscmp(pstrName, L"float") == 0 ) SetFloat(wcscmp(pstrValue, L"true") == 0);
    else if( wcscmp(pstrName, L"shortcut") == 0 ) SetShortcut(pstrValue[0]);
    else if( wcscmp(pstrName, L"menu") == 0 ) SetContextMenuUsed(wcscmp(pstrValue, L"true") == 0);
	else if( wcscmp(pstrName, L"virtualwnd") == 0 ) SetVirtualWnd(pstrValue);
}

CControlUI* CControlUI::ApplyAttributeList(const wchar_t* pstrList)
{
	std::wstring sItem;
	std::wstring sValue;
	while( *pstrList != _T('\0') ) {
		sItem.clear();
		sValue.clear();
		while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
			const wchar_t* pstrTemp = ::CharNextW(pstrList);
			while( pstrList < pstrTemp) {
				sItem.push_back(*pstrList++);
			}
		}
		ASSERT( *pstrList == _T('=') );
		if( *pstrList++ != _T('=') ) return this;
		ASSERT( *pstrList == _T('\"') );
		if( *pstrList++ != _T('\"') ) return this;
		while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
			const wchar_t* pstrTemp = ::CharNextW(pstrList);
			while( pstrList < pstrTemp) {
				sValue.push_back(*pstrList++);
			}
		}
		ASSERT( *pstrList == _T('\"') );
		if( *pstrList++ != _T('\"') ) return this;
		SetAttribute(sItem.c_str(), sValue.c_str());
		if( *pstrList++ != _T(' ') ) return this;
	}
	Invalidate();
	return this;
}

CControlUI* CControlUI::ApplyAttributeTable(CAttributesList* pTable)
{
	if (pTable)
	{
		foreach(auto it, *pTable)
		{
			SetAttribute(it.first.c_str(), it.second.c_str());
		}
		Invalidate();
	}
    return this;
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    // »æÖÆÑ­Ðò£º±³¾°ÑÕÉ«->±³¾°Í¼->×´Ì¬Í¼->ÎÄ±¾->±ß¿ò
    if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
        CRenderClip roundClip;
        CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if ( m_dwBackColor != 0 )
	{
        if ( m_dwBackColor2 != 0 ) 
		{
            if ( m_dwBackColor3 != 0 )
			{
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / 2;
                CRenderEngine::DrawGradient(hDC, rc, m_dwBackColor, m_dwBackColor2, true, 8);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, m_dwBackColor2, m_dwBackColor3, true, 8);
            }
            else 
                CRenderEngine::DrawGradient(hDC, m_rcItem, m_dwBackColor, m_dwBackColor2, true, 16);
        }
        else if( m_dwBackColor >= 0xFF000000 )
			CRenderEngine::DrawColor(hDC, m_rcPaint, m_dwBackColor);
        else 
			CRenderEngine::DrawColor(hDC, m_rcItem, m_dwBackColor);
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
    if (!m_sBkImage) return;
    if (!DrawImage(hDC, *m_sBkImage.get())) m_sBkImage.reset();
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
	if( m_nBorderSize > 0 )
	{
        if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 )//»­Ô²½Ç±ß¿ò
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, m_dwFocusBorderColor);
			else
				CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, m_dwBorderColor);
		}
		else
		{
			if (IsFocused() && m_dwFocusBorderColor != 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, m_dwFocusBorderColor);
			else
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, m_dwBorderColor);
		}
	}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
    return;
}

} // namespace DuiLib
