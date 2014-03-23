#include "stdafx.h"

#ifdef DUI_USE_COMBOBOX

#include "UIComboBox.h"
#include <base/util/format.h>

namespace DuiLib
{
	CComboBoxUI::CComboBoxUI()
	{
		m_nArrowWidth = 0;
	}

	const wchar_t* CComboBoxUI::GetClass() const
	{
		return DUI_CTR_COMBOBOX;
	}

	void CComboBoxUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if (wcscmp(pstrName, L"arrowimage") == 0)
			m_sArrowImage.reset(new CImage(pstrValue));
		else
			CComboUI::SetAttribute(pstrName, pstrValue);
	}

	void CComboBoxUI::PaintStatusImage(HDC hDC)
	{
		if (!m_sArrowImage)
			CComboUI::PaintStatusImage(hDC);
		else
		{
			// get index
			if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
			else m_uButtonState &= ~ UISTATE_FOCUSED;
			if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
			else m_uButtonState &= ~ UISTATE_DISABLED;

			int nIndex = 0;
			if ((m_uButtonState & UISTATE_DISABLED) != 0)
				nIndex = 4;
			else if ((m_uButtonState & UISTATE_PUSHED) != 0)
				nIndex = 2;
			else if ((m_uButtonState & UISTATE_HOT) != 0)
				nIndex = 1;
			else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
				nIndex = 3;

			CDuiRect rcBmpPart = m_sArrowImage->m_rcBmpPart;

			m_nArrowWidth = rcBmpPart.GetWidth() / 5;
			rcBmpPart.left += nIndex * m_nArrowWidth;
			rcBmpPart.right = rcBmpPart.left + m_nArrowWidth;

			CDuiRect rcDest(0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
			rcDest.Deflate(GetBorderSize(), GetBorderSize());
			rcDest.left = rcDest.right - m_nArrowWidth;

			// draw image
			if (!DrawImage(hDC, *m_sArrowImage.get(), base::format(L"dest='%d,%d,%d,%d'", rcDest.left, rcDest.top, rcDest.right, rcDest.bottom).c_str()))
				m_sNormalImage.reset();
		}
	}

	void CComboBoxUI::PaintText(HDC hDC)
	{
		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.right -= m_rcTextPadding.right;
		rcText.top += m_rcTextPadding.top;
		rcText.bottom -= m_rcTextPadding.bottom;

		rcText.right -= m_nArrowWidth; // add this line than CComboUI::PaintText(HDC hDC)

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
}
#endif
