#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CCheckBoxUI : public CButtonUI
	{
	public:
		CCheckBoxUI();

		LPCTSTR GetClass() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);

		bool IsSelected() const;
		virtual void Selected(bool bSelected);

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		bool			m_bSelected;

		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;
		DWORD           m_dwSelectedPushedTextColor;

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage>  		m_sSelectedImage;
		std::unique_ptr<CImage>		    m_sSelectedHotImage;
		std::unique_ptr<CImage>		    m_sSelectedPushedImage;		
		std::unique_ptr<CImage>		    m_sSelectedDisabledImage;
#pragma warning(pop)
	};

} // namespace DuiLib

#endif // __UIOPTION_H__
