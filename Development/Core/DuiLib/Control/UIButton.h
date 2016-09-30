#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CButtonUI : public CLabelUI
	{
	public:
		CButtonUI();

		const wchar_t* GetClass() const;
		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
		UINT m_uButtonState;

		DWORD m_dwHotBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sForeImage;
		std::unique_ptr<CImage> m_sNormalImage;
		std::unique_ptr<CImage> m_sHotImage;
		std::unique_ptr<CImage> m_sHotForeImage;
		std::unique_ptr<CImage> m_sPushedImage;
		std::unique_ptr<CImage> m_sPushedForeImage;
		std::unique_ptr<CImage> m_sFocusedImage;
		std::unique_ptr<CImage> m_sDisabledImage;
#pragma warning(pop)
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__