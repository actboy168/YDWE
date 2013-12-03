#include "StdAfx.h"
#include "UILabel.h"

namespace DuiLib
{
	CLabelUI::CLabelUI() : m_uTextStyle(DT_VCENTER), m_dwTextColor(0), 
		m_dwDisabledTextColor(0), m_iFont(-1)
	{
		::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));
	}

	const wchar_t* CLabelUI::GetClass() const
	{
		return DUI_CTR_LABEL;
	}

	std::wstring const& CLabelUI::GetText() const
	{
		return m_sText;
	}

	void CLabelUI::SetText(const wchar_t* pstrText)
	{
		m_sText = pstrText;
		Invalidate();
	}

	void CLabelUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		Invalidate();
	}

	UINT CLabelUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void CLabelUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CLabelUI::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void CLabelUI::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CLabelUI::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	void CLabelUI::SetFont(int index)
	{
		m_iFont = index;
		Invalidate();
	}

	int CLabelUI::GetFont() const
	{
		return m_iFont;
	}

	RECT CLabelUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CLabelUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	SIZE CLabelUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 4);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CLabelUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			// return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			// return;
		}
		CControlUI::DoEvent(event);
	}

	void CLabelUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"text") == 0 ) SetText(pstrValue);
		else if( wcscmp(pstrName, L"align") == 0 ) {
			if( wcsstr(pstrValue, L"left") != NULL ) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if( wcsstr(pstrValue, L"center") != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if( wcsstr(pstrValue, L"right") != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
			if( wcsstr(pstrValue, L"top") != NULL ) {
				m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
				m_uTextStyle |= DT_TOP;
			}
			if( wcsstr(pstrValue, L"vcenter") != NULL ) {
				m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);			
				m_uTextStyle |= DT_VCENTER;
			}
			if( wcsstr(pstrValue, L"bottom") != NULL ) {
				m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
				m_uTextStyle |= DT_BOTTOM;
			}
		}
		else if( wcscmp(pstrName, L"endellipsis") == 0 ) {
			if( wcscmp(pstrValue, L"true") == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		else if( wcscmp(pstrName, L"font") == 0 ) SetFont(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"textcolor") == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if( wcscmp(pstrName, L"disabledtextcolor") == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			SetDisabledTextColor(clrColor);
		}
		else if( wcscmp(pstrName, L"textpadding") == 0 ) {
			RECT rcTextPadding = { 0 };
			wchar_t* pstr = NULL;
			rcTextPadding.left = wcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CLabelUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( GetText().empty() ) return;
		int nLinks = 0;
		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, GetText().c_str(), m_dwTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, GetText().c_str(), m_dwDisabledTextColor, m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
	}
}