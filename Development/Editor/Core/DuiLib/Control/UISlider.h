#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

#ifdef DUI_USE_SLIDER

namespace DuiLib
{
	class UILIB_API CSliderUI : public CProgressUI
	{
	public:
		CSliderUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);

		int GetChangeStep();
		void SetChangeStep(int step);
		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;

		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;
		int m_nStep;

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sThumbImage;
		std::unique_ptr<CImage> m_sThumbHotImage;
		std::unique_ptr<CImage> m_sThumbPushedImage;
		CDuiString m_sImageModify;
#pragma warning(pop)
	};
}
#endif

#endif // __UISLIDER_H__