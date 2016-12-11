#include "stdafx.h"
#include "UIButton.h"

namespace DuiLib
{
	CButtonUI::CButtonUI()
		: m_uButtonState(0)
		, m_dwHotTextColor(0)
		, m_dwPushedTextColor(0)
		, m_dwFocusedTextColor(0)
		,m_dwHotBkColor(0)
	{
		m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
	}

	const wchar_t* CButtonUI::GetClass() const
	{
		return DUI_CTR_BUTTON;
	}

	UINT CButtonUI::GetControlFlags() const
	{
		return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
	}

	void CButtonUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
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
		if( event.Type == UIEVENT_KEYDOWN )
		{
			if (IsKeyboardEnabled()) {
				if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
					Activate();
					return;
				}
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
				m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
				else m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
				m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			if( IsContextMenuUsed() ) {
				m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			// return;
		}
		if( event.Type == UIEVENT_SETCURSOR ) {
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			return;
		}
		CLabelUI::DoEvent(event);
	}

	bool CButtonUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK);
		return true;
	}

	void CButtonUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	SIZE CButtonUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CButtonUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"normalimage") == 0 ) m_sNormalImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"hotimage") == 0 ) m_sHotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"pushedimage") == 0 ) m_sPushedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"focusedimage") == 0 ) m_sFocusedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"disabledimage") == 0 ) m_sDisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"foreimage") == 0 ) m_sFocusedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"hotforeimage") == 0 ) m_sHotForeImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"hotbkcolor") == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwHotBkColor = clrColor;
		}
		else if( wcscmp(pstrName, L"hottextcolor") == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwHotTextColor = clrColor;
		}
		else if( wcscmp(pstrName, L"pushedtextcolor") == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwPushedTextColor = clrColor;
		}
		else if( wcscmp(pstrName, L"focusedtextcolor") == 0 )
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwFocusedTextColor = clrColor;
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CButtonUI::PaintText(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( GetText().empty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		DWORD clrColor = IsEnabled()?m_dwTextColor:m_dwDisabledTextColor;

		if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (m_dwPushedTextColor != 0) )
			clrColor = m_dwPushedTextColor;
		else if( ((m_uButtonState & UISTATE_HOT) != 0) && (m_dwHotTextColor != 0) )
			clrColor = m_dwHotTextColor;
		else if( ((m_uButtonState & UISTATE_FOCUSED) != 0) && (m_dwFocusedTextColor != 0) )
			clrColor = m_dwFocusedTextColor;

		CRenderEngine::DrawText(hDC, m_pManager, rc, GetText().c_str(), clrColor, m_iFont, m_uTextStyle);
	}

	void CButtonUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if (m_sDisabledImage)
			{
				if (!DrawImage(hDC, *m_sDisabledImage.get())) m_sDisabledImage.reset();
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if (m_sPushedImage) {
				if( !DrawImage(hDC, *m_sPushedImage.get()) ){
					m_sPushedImage.reset();
				}
				if (m_sPushedForeImage)
				{
					if (!DrawImage(hDC, *m_sPushedForeImage.get()) )
						m_sPushedForeImage.reset();
					return;
				}
				else goto Label_ForeImage;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if (m_sHotImage) {
				if( !DrawImage(hDC, *m_sHotImage) ){
					m_sHotImage.reset();
				}
				if (m_sHotForeImage) {
					if( !DrawImage(hDC, *m_sHotForeImage.get()) )
						m_sHotForeImage.reset();
					return;
				}
				else goto Label_ForeImage;
			}
			else if(m_dwHotBkColor != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, m_dwHotBkColor);
				return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if (m_sFocusedImage) {
				if( !DrawImage(hDC, *m_sFocusedImage.get()) ) m_sFocusedImage.reset();
				else goto Label_ForeImage;
			}
		}

		if (m_sNormalImage) {
			if( !DrawImage(hDC, *m_sNormalImage.get()) ) m_sNormalImage.reset();
			else goto Label_ForeImage;
		}

		if (m_sForeImage)
			goto Label_ForeImage;

		return;

Label_ForeImage:
		if (m_sForeImage) {
			if (!DrawImage(hDC, *m_sForeImage.get()) ) m_sForeImage.reset();
		}
	}
}
