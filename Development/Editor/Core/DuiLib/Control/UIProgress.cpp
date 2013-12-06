#include "stdafx.h"
#include "UIProgress.h"
#include <base/util/format.h>

#ifdef DUI_USE_PROGRESS

namespace DuiLib
{
	CProgressUI::CProgressUI() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	const wchar_t* CProgressUI::GetClass() const
	{
		return DUI_CTR_PROGRESS;
	}

	bool CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		m_nValue = nValue;
		Invalidate();
	}

	void CProgressUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"foreimage") == 0 ) m_sForeImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"hor") == 0 ) SetHorizontal(wcscmp(pstrValue, L"true") == 0);
		else if( wcscmp(pstrName, L"min") == 0 ) SetMinValue(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"max") == 0 ) SetMaxValue(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"value") == 0 ) SetValue(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"isstretchfore")==0) SetStretchForeImage(wcscmp(pstrValue, L"true") == 0? true : false);
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}

		if( m_sForeImage ) {
			if (m_bStretchForeImage)
				m_sForeImageModify = base::format(L"dest='%d,%d,%d,%d'", rc.left, rc.top, rc.right, rc.bottom);
			else
				m_sForeImageModify = base::format(L"dest='%d,%d,%d,%d' source='%d,%d,%d,%d'", , rc.left, rc.top, rc.right, rc.bottom, rc.left, rc.top, rc.right, rc.bottom);

			if( !DrawImage(hDC, *m_sForeImage.get(), m_sForeImageModify.c_str()) ) m_sForeImage.reset();
			else return;
		}
	}

	bool CProgressUI::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void CProgressUI::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)		return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}
}
#endif
