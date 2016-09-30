#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

class CComboWnd : public CWindowWnd
{
public:
    void Init(CComboUI* pOwner);
    const wchar_t* GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
	virtual UINT GetClassStyle() const;
#endif

public:
    CPaintManagerUI m_pm;
    CComboUI* m_pOwner;
    CVerticalLayoutUI* m_pLayout;
    int m_iOldSel;
};


void CComboWnd::Init(CComboUI* pOwner)
{
    m_pOwner = pOwner;
    m_pLayout = NULL;
    m_iOldSel = m_pOwner->GetCurSel();

    // Position the popup window in absolute space
    SIZE szDrop = m_pOwner->GetDropBoxSize();
    RECT rcOwner = pOwner->GetPos();
    RECT rc = rcOwner;
    rc.top = rc.bottom;		// 父窗口left、bottom位置作为弹出窗口起点
    rc.bottom = rc.top + szDrop.cy;	// 计算弹出窗口高度
    if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;	// 计算弹出窗口宽度

    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
    int cyFixed = 0;
    for( int it = 0; it < pOwner->GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
    }
    cyFixed += 4; // CVerticalLayoutUI 默认的Inset 调整
    rc.bottom = rc.top + MIN(cyFixed, szDrop.cy);

    ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
    if( rc.bottom > rcWork.bottom ) {
        rc.left = rcOwner.left;
        rc.right = rcOwner.right;
        if( szDrop.cx > 0 ) rc.right = rc.left + szDrop.cx;
        rc.top = rcOwner.top - MIN(cyFixed, szDrop.cy);
        rc.bottom = rcOwner.top;
        ::MapWindowRect(pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    }
    
    Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP, WS_EX_TOOLWINDOW, rc);
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);
    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessageW(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

const wchar_t* CComboWnd::GetWindowClassName() const
{
    return L"ComboWnd";
}

void CComboWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->m_pWindow = NULL;
    m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
    m_pOwner->Invalidate();
    delete this;
}

LRESULT CComboWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_CREATE ) {
        m_pm.Init(m_hWnd);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        m_pLayout = new CVerticalLayoutUI;
        m_pm.UseParentResource(m_pOwner->GetManager());
        m_pLayout->SetManager(&m_pm, NULL, true);
        CAttributesList* pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(L"VerticalLayout");
        if (pDefaultAttributes)
		{
            m_pLayout->ApplyAttributeTable(pDefaultAttributes);
		}
        m_pLayout->SetInset(CDuiRect(1, 1, 1, 1));
        m_pLayout->SetBkColor(0xFFFFFFFF);
        m_pLayout->SetBorderColor(0xFFC6C7D2);
        m_pLayout->SetBorderSize(1);
        m_pLayout->SetAutoDestroy(false);
        m_pLayout->EnableScrollBar();
        m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList().c_str());
		for (int i = 0; i < m_pOwner->GetCount(); i++) {
            m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
        }
		m_pm.AttachDialog(m_pLayout);
		PostMessage(WM_PAINT);
		PostMessage(WM_KEYDOWN, VK_SPACE);
        return 0;
    }
    else if( uMsg == WM_CLOSE ) {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos());
        m_pOwner->SetFocus();
    }
    else if( uMsg == WM_LBUTTONUP ) {
        POINT pt = { 0 };
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_pm.GetPaintWindow(), &pt);
        CControlUI* pControl = m_pm.FindControl(pt);
        if( pControl && wcscmp(pControl->GetClass(), DUI_CTR_SCROLLBAR) != 0 ) PostMessage(WM_KILLFOCUS);
    }
    else if( uMsg == WM_KEYDOWN ) {
        switch( wParam ) {
        case VK_ESCAPE:
            m_pOwner->SelectItem(m_iOldSel, true);
            EnsureVisible(m_iOldSel);
            // FALL THROUGH...
        case VK_RETURN:
            PostMessage(WM_KILLFOCUS);
            break;
        default:
            TEventUI event;
            event.Type = UIEVENT_KEYDOWN;
            event.chKey = (wchar_t)wParam;
            m_pOwner->DoEvent(event);
            EnsureVisible(m_pOwner->GetCurSel());
            return 0;
        }
    }
    else if( uMsg == WM_MOUSEWHEEL ) {
        int zDelta = (int) (short) HIWORD(wParam);
        TEventUI event = { 0 };
        event.Type = UIEVENT_SCROLLWHEEL;
        event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
        event.lParam = lParam;
        event.dwTimestamp = ::GetTickCount();
        m_pOwner->DoEvent(event);
        EnsureVisible(m_pOwner->GetCurSel());
        return 0;
    }
    else if( uMsg == WM_KILLFOCUS ) {
        if( m_hWnd != (HWND) wParam ) PostMessage(WM_CLOSE);
    }

    LRESULT lRes = 0;
    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CComboWnd::EnsureVisible(int iIndex)
{
    if( m_pOwner->GetCurSel() < 0 ) return;
    m_pLayout->FindSelectable(m_pOwner->GetCurSel(), false);
    RECT rcItem = m_pLayout->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pLayout->GetPos();
	CScrollBarUI* pVerticalScrollBar = m_pLayout->GetVerticalScrollBar();
	if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) rcList.bottom -= pVerticalScrollBar->GetFixedHeight();
    if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom) return;
    int dy = 0;
	if (rcItem.top < rcList.top) dy = rcItem.top - rcList.top;
	if (rcItem.bottom > rcList.bottom) dy = rcItem.bottom - rcList.bottom;
	Scroll(0, dy);
}

void CComboWnd::Scroll(int dx, int dy)
{
    if( dx == 0 && dy == 0 ) return;
    SIZE sz = m_pLayout->GetScrollPos();
    m_pLayout->SetScrollPos(CSize(sz.cx + dx, sz.cy + dy));
}

#if(_WIN32_WINNT >= 0x0501)
UINT CComboWnd::GetClassStyle() const
{
	return __super::GetClassStyle() | CS_DROPSHADOW;
}
#endif
////////////////////////////////////////////////////////


CComboUI::CComboUI() : m_pWindow(NULL), m_iCurSel(-1), m_uButtonState(0)
{
    m_szDropBox = CSize(0, 150);
    ::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));

    m_ListInfo.nColumns = 0;
    m_ListInfo.nFont = -1;
	m_ListInfo.uTextStyle = DT_VCENTER;
	m_ListInfo.SetAlternateBk(false);
    m_ListInfo.SetTextColor(0xFF000000);
    m_ListInfo.SetBkColor(0);
    m_ListInfo.SetSelectedTextColor(0xFF000000);
    m_ListInfo.SetSelectedBkColor(0xFFC1E3FF);
    m_ListInfo.SetHotTextColor(0xFF000000);
    m_ListInfo.SetHotBkColor(0xFFE9F5FF);
    m_ListInfo.SetDisabledTextColor(0xFFCCCCCC);
    m_ListInfo.SetDisabledBkColor(0xFFFFFFFF);
    m_ListInfo.SetLineColor(0);
    m_ListInfo.bMultiExpandable = false;
    ::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
}

const wchar_t* CComboUI::GetClass() const
{
    return DUI_CTR_COMBO;
}

UINT CComboUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

void CComboUI::DoInit()
{
}

int CComboUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CComboUI::SelectItem(int iIndex, bool bTakeFocus)
{
    if( m_pWindow != NULL ) m_pWindow->Close();
    if( iIndex == m_iCurSel ) return true;
    int iOldSel = m_iCurSel;
    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        if( !pControl ) return false;
        IListItemUI* pListItem = dynamic_cast<IListItemUI*>(pControl);
        if( pListItem != NULL ) pListItem->Select(false);
        m_iCurSel = -1;
    }
    if( iIndex < 0 ) return false;
    if( m_items.GetSize() == 0 ) return false;
    if( iIndex >= m_items.GetSize() ) iIndex = m_items.GetSize() - 1;
    CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);
    if( !pControl ) return false;
    IListItemUI* pListItem = dynamic_cast<IListItemUI*>(pControl);
    if( pListItem == NULL ) return false;
    m_iCurSel = iIndex;
    if( m_pWindow != NULL || bTakeFocus ) pControl->SetFocus();
    pListItem->Select(true);
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
    Invalidate();

    return true;
}

bool CComboUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
    int iOrginIndex = GetItemIndex(pControl);
    if( iOrginIndex == -1 ) return false;
    if( iOrginIndex == iIndex ) return true;

    IListItemUI* pSelectedListItem = NULL;
    if( m_iCurSel >= 0 ) 
		pSelectedListItem = dynamic_cast<IListItemUI*>(GetItemAt(m_iCurSel));
    if( !CContainerUI::SetItemIndex(pControl, iIndex) ) return false;
    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);
    for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = dynamic_cast<IListItemUI*>(p);
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= 0 && pSelectedListItem != NULL ) m_iCurSel = pSelectedListItem->GetIndex();
    return true;
}

bool CComboUI::Add(CControlUI* pControl)
{
    IListItemUI* pListItem = dynamic_cast<IListItemUI*>(pControl);
    if( pListItem != NULL ) 
	{
        pListItem->SetOwner(this);
        pListItem->SetIndex(m_items.GetSize());
    }
    return CContainerUI::Add(pControl);
}

bool CComboUI::AddAt(CControlUI* pControl, int iIndex)
{
    if (!CContainerUI::AddAt(pControl, iIndex)) return false;

    // The list items should know about us
    IListItemUI* pListItem = dynamic_cast<IListItemUI*>(pControl);
	if (pListItem != NULL) 
	{
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for(int i = iIndex + 1; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        pListItem = dynamic_cast<IListItemUI*>(p);
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }
    if( m_iCurSel >= iIndex ) m_iCurSel += 1;
    return true;
}

bool CComboUI::Remove(CControlUI* pControl)
{
    int iIndex = GetItemIndex(pControl);
    if (iIndex == -1) return false;

    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = dynamic_cast<IListItemUI*>(p);
        if( pListItem != NULL ) {
            pListItem->SetIndex(i);
        }
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

bool CComboUI::RemoveAt(int iIndex)
{
    if (!CContainerUI::RemoveAt(iIndex)) return false;

    for(int i = iIndex; i < GetCount(); ++i) {
        CControlUI* p = GetItemAt(i);
        IListItemUI* pListItem = dynamic_cast<IListItemUI*>(p);
        if( pListItem != NULL ) pListItem->SetIndex(i);
    }

    if( iIndex == m_iCurSel && m_iCurSel >= 0 ) {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if( iIndex < m_iCurSel ) m_iCurSel -= 1;
    return true;
}

void CComboUI::RemoveAll()
{
    m_iCurSel = -1;
    CContainerUI::RemoveAll();
}

void CComboUI::DoEvent(TEventUI& event)
{
    if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
        if( m_pParent != NULL ) m_pParent->DoEvent(event);
        else CContainerUI::DoEvent(event);
        return;
    }

    if( event.Type == UIEVENT_SETFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        Invalidate();
    }
    if( event.Type == UIEVENT_BUTTONDOWN )
    {
        if( IsEnabled() ) {
            Activate();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
        }
        return;
    }
    if( event.Type == UIEVENT_BUTTONUP )
    {
        if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
            m_uButtonState &= ~ UISTATE_CAPTURED;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSEMOVE )
    {
        return;
    }
    if( event.Type == UIEVENT_KEYDOWN )
    {
        switch( event.chKey ) {
        case VK_F4:
            Activate();
            return;
        case VK_UP:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_DOWN:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_PRIOR:
            SelectItem(FindSelectable(m_iCurSel - 1, false));
            return;
        case VK_NEXT:
            SelectItem(FindSelectable(m_iCurSel + 1, true));
            return;
        case VK_HOME:
            SelectItem(FindSelectable(0, false));
            return;
        case VK_END:
            SelectItem(FindSelectable(GetCount() - 1, true));
            return;
        }
    }
    if( event.Type == UIEVENT_SCROLLWHEEL )
    {
        bool bDownward = LOWORD(event.wParam) == SB_LINEDOWN;
        SelectItem(FindSelectable(m_iCurSel + (bDownward ? 1 : -1), bDownward));
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        return;
    }
    if( event.Type == UIEVENT_MOUSEENTER )
    {
        if( ::PtInRect(&m_rcItem, event.ptMouse ) ) {
            if( (m_uButtonState & UISTATE_HOT) == 0  )
                m_uButtonState |= UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    if( event.Type == UIEVENT_MOUSELEAVE )
    {
        if( (m_uButtonState & UISTATE_HOT) != 0 ) {
            m_uButtonState &= ~UISTATE_HOT;
            Invalidate();
        }
        return;
    }
    CControlUI::DoEvent(event);
}

SIZE CComboUI::EstimateSize(SIZE szAvailable)
{
    if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 12);
    return CControlUI::EstimateSize(szAvailable);
}

bool CComboUI::Activate()
{
    if( !CControlUI::Activate() ) return false;
    if( m_pWindow ) return true;
    m_pWindow = new CComboWnd();
    ASSERT(m_pWindow);
    m_pWindow->Init(this);
    if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_DROPDOWN);
    Invalidate();
    return true;
}

std::wstring const& CComboUI::GetText() const
{
	static std::wstring s_dummy = L"";
    if( m_iCurSel < 0 ) return s_dummy;
    CLabelUI* pControl = dynamic_cast<CLabelUI*>(static_cast<CControlUI*>(m_items[m_iCurSel]));
	if (!pControl) return s_dummy;
    return pControl->GetText();
}

void CComboUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);
    if( !IsEnabled() ) m_uButtonState = 0;
}

std::wstring CComboUI::GetDropBoxAttributeList()
{
    return m_sDropBoxAttributes;
}

void CComboUI::SetDropBoxAttributeList(const wchar_t* pstrList)
{
    m_sDropBoxAttributes = pstrList;
}

SIZE CComboUI::GetDropBoxSize() const
{
    return m_szDropBox;
}

void CComboUI::SetDropBoxSize(SIZE szDropBox)
{
    m_szDropBox = szDropBox;
}

RECT CComboUI::GetTextPadding() const
{
    return m_rcTextPadding;
}

void CComboUI::SetTextPadding(RECT rc)
{
    m_rcTextPadding = rc;
    Invalidate();
}

TListInfoUI* CComboUI::GetListInfo()
{
    return &m_ListInfo;
}

void CComboUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    Invalidate();
}

void CComboUI::SetItemTextStyle(UINT uStyle)
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

RECT CComboUI::GetItemTextPadding() const
{
	return m_ListInfo.rcTextPadding;
}

void CComboUI::SetItemTextPadding(RECT rc)
{
    m_ListInfo.rcTextPadding = rc;
    Invalidate();
}

void CComboUI::SetPos(RECT rc)
{
    // Put all elements out of sight
    RECT rcNull = { 0 };
    for( int i = 0; i < m_items.GetSize(); i++ ) static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
    // Position this control
    CControlUI::SetPos(rc);
}

void CComboUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
{
    if( wcscmp(pstrName, L"textpadding") == 0 ) {
        RECT rcTextPadding = { 0 };
        wchar_t* pstr = NULL;
        rcTextPadding.left = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetTextPadding(rcTextPadding);
    }
    else if( wcscmp(pstrName, L"normalimage") == 0 ) m_sNormalImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"hotimage") == 0 ) m_sHotImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"pushedimage") == 0 ) m_sPushedImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"focusedimage") == 0 ) m_sFocusedImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"disabledimage") == 0 ) m_sDisabledImage.reset(new CImage(pstrValue));
    else if( wcscmp(pstrName, L"dropbox") == 0 ) SetDropBoxAttributeList(pstrValue);
	else if( wcscmp(pstrName, L"dropboxsize") == 0)
	{
		SIZE szDropBoxSize = { 0 };
		wchar_t* pstr = NULL;
		szDropBoxSize.cx = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		szDropBoxSize.cy = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		SetDropBoxSize(szDropBoxSize);
	}
    else if( wcscmp(pstrName, L"itemfont") == 0 ) m_ListInfo.nFont = _wtoi(pstrValue);
    else if( wcscmp(pstrName, L"itemalign") == 0 ) {
        if( wcsstr(pstrValue, L"left") != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        if( wcsstr(pstrValue, L"center") != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        if( wcsstr(pstrValue, L"right") != NULL ) {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    if( wcscmp(pstrName, L"itemtextpadding") == 0 ) {
        RECT rcTextPadding = { 0 };
        wchar_t* pstr = NULL;
        rcTextPadding.left = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
        rcTextPadding.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
        rcTextPadding.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
        SetItemTextPadding(rcTextPadding);
    }
    else if( wcscmp(pstrName, L"itemtextcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetTextColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itembkcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetBkColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itembkimage") == 0 ) m_ListInfo.SetBkImage(pstrValue);
    else if( wcscmp(pstrName, L"itemaltbk") == 0 ) m_ListInfo.SetAlternateBk(wcscmp(pstrValue, L"true") == 0);
    else if( wcscmp(pstrName, L"itemselectedtextcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetSelectedTextColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemselectedbkcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetSelectedBkColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemselectedimage") == 0 ) m_ListInfo.SetSelectedImage(pstrValue);
    else if( wcscmp(pstrName, L"itemhottextcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetHotTextColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemhotbkcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetHotBkColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemhotimage") == 0 ) m_ListInfo.SetHotImage(pstrValue);
    else if( wcscmp(pstrName, L"itemdisabledtextcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetDisabledTextColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemdisabledbkcolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetDisabledBkColor(clrColor);
    }
    else if( wcscmp(pstrName, L"itemdisabledimage") == 0 ) m_ListInfo.SetDisabledImage(pstrValue);
    else if( wcscmp(pstrName, L"itemlinecolor") == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
        wchar_t* pstr = NULL;
        DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
        m_ListInfo.SetLineColor(clrColor);
    }
    else CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CComboUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    CControlUI::DoPaint(hDC, rcPaint);
}

void CComboUI::PaintStatusImage(HDC hDC)
{
    if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
    else m_uButtonState &= ~ UISTATE_FOCUSED;
    if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
    else m_uButtonState &= ~ UISTATE_DISABLED;

    if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
        if(m_sDisabledImage) {
            if( !DrawImage(hDC, *m_sDisabledImage.get()) ) m_sDisabledImage.reset();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
        if(m_sPushedImage) {
            if( !DrawImage(hDC, *m_sPushedImage.get()) ) m_sPushedImage.reset();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        if(m_sHotImage) {
            if( !DrawImage(hDC, *m_sHotImage.get()) ) m_sHotImage.reset();
            else return;
        }
    }
    else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
        if(m_sFocusedImage) {
            if( !DrawImage(hDC, *m_sFocusedImage.get()) ) m_sFocusedImage.reset();
            else return;
        }
    }

    if(m_sNormalImage) {
        if( !DrawImage(hDC, *m_sNormalImage.get()) ) m_sNormalImage.reset();
        else return;
    }
}

void CComboUI::PaintText(HDC hDC)
{
    RECT rcText = m_rcItem;
    rcText.left += m_rcTextPadding.left;
    rcText.right -= m_rcTextPadding.right;
    rcText.top += m_rcTextPadding.top;
    rcText.bottom -= m_rcTextPadding.bottom;

    if( m_iCurSel >= 0 ) {
        CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
        IListItemUI* pElement = dynamic_cast<IListItemUI*>(pControl);
        if( pElement != NULL ) {
            pElement->DrawItemText(hDC, rcText);
        }
        else {
            RECT rcOldPos = pControl->GetPos();
            pControl->SetPos(rcText);
            pControl->DoPaint(hDC, rcText);
            pControl->SetPos(rcOldPos);
        }
    }
}

} // namespace DuiLib
