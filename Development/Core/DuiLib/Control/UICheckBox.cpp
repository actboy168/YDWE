#include "stdafx.h"
#include "UICheckBox.h"

namespace DuiLib
{
	CCheckBoxUI::CCheckBoxUI() : m_bSelected(false), m_dwSelectedTextColor(0), m_dwSelectedPushedTextColor(0)
	{
	}

	const wchar_t* CCheckBoxUI::GetClass() const
	{
		return DUI_CTR_CHECKBOX;
	}

	bool CCheckBoxUI::IsSelected() const
	{
		return m_bSelected;
	}

	void CCheckBoxUI::Selected(bool bSelected)
	{
		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;

		if (m_pManager != NULL) 
		{
			m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
		}

		Invalidate();
	}

	bool CCheckBoxUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		Selected(!m_bSelected);
		return true;
	}

	void CCheckBoxUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if (!IsEnabled()) 
		{
			m_uButtonState = 0;
		}
	}

	SIZE CCheckBoxUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CCheckBoxUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"selected") == 0 ) Selected(wcscmp(pstrValue, L"true") == 0);
		else if( wcscmp(pstrName, L"selectedimage") == 0 ) m_sSelectedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"selectedhotimage") == 0 ) m_sSelectedHotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"selectedpushedimage") == 0 ) m_sSelectedPushedImage.reset(new CImage(pstrValue));		
		else if( wcscmp(pstrName, L"selecteddisabledimage") == 0 ) m_sSelectedDisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"selectedbkcolor") == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwSelectedBkColor = clrColor;
		}
		else if( wcscmp(pstrName, L"selectedtextcolor") == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwSelectedTextColor = clrColor;
		}
		else if( wcscmp(pstrName, L"selectedpushedtextcolor") == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNextW(pstrValue);
			wchar_t* pstr = NULL;
			DWORD clrColor = wcstoul(pstrValue, &pstr, 16);
			m_dwSelectedPushedTextColor = clrColor;
		}
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void CCheckBoxUI::PaintStatusImage(HDC hDC)
	{
		//m_uButtonState &= ~UISTATE_PUSHED;

		if (IsSelected())
		{
			if((m_uButtonState & UISTATE_PUSHED) != 0 && m_sSelectedPushedImage) 
			{
				if (!DrawImage(hDC, *m_sSelectedPushedImage.get())) m_sSelectedPushedImage.reset();
				else goto Label_ForeImage;
			}
			else if ((m_uButtonState & UISTATE_HOT) != 0 && m_sSelectedHotImage) 
			{
				if (!DrawImage(hDC, *m_sSelectedHotImage.get())) m_sSelectedHotImage.reset();
				else goto Label_ForeImage;
			}
			else if (!IsEnabled() && m_sSelectedDisabledImage)
			{
				if (!DrawImage(hDC, *m_sSelectedDisabledImage.get())) m_sSelectedDisabledImage.reset();
				else goto Label_ForeImage;
			}
			else 
			{
				if (m_sSelectedImage) {
					if (!DrawImage(hDC, *m_sSelectedImage.get())) m_sSelectedImage.reset();
					else goto Label_ForeImage;
				}
				else if (m_dwSelectedBkColor != 0) {
					CRenderEngine::DrawColor(hDC, m_rcPaint, m_dwSelectedBkColor);
					return;
				}	
			}
		}

		CButtonUI::PaintStatusImage(hDC);

Label_ForeImage:
		if (m_sForeImage) 
		{
			if (!DrawImage(hDC, *m_sForeImage.get())) m_sForeImage.reset();
		}
	}

	void CCheckBoxUI::PaintText(HDC hDC)
	{
		if (IsSelected())
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if( GetText().empty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			DWORD clrColor = IsEnabled()? m_dwTextColor: m_dwDisabledTextColor;
			if( ((m_uButtonState & UISTATE_PUSHED) != 0) && (m_dwSelectedPushedTextColor != 0) )
				clrColor = m_dwSelectedPushedTextColor;

			CRenderEngine::DrawText(hDC, m_pManager, rc, GetText().c_str(), clrColor, m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
			CButtonUI::PaintText(hDC);
	}
}
