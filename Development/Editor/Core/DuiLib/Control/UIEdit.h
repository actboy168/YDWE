#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#ifdef DUI_USE_EDIT

#pragma once

namespace DuiLib
{
	class CEditWnd;

	class UILIB_API CEditUI : public CLabelUI
	{
		friend class CEditWnd;
	public:
		CEditUI();

		const wchar_t* GetClass() const;
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(const wchar_t* pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(wchar_t cPasswordChar);
		wchar_t GetPasswordChar() const;
		void SetNumberOnly(bool bNumberOnly);
		bool IsNumberOnly() const;
		int GetWindowStyls() const;

		void SetNativeEditBkColor(DWORD dwBkColor);
		DWORD GetNativeEditBkColor() const;

		void SetSel(long nStartChar, long nEndChar);
		void SetSelAll();
		void SetReplaceSel(const wchar_t* lpszReplace);

		void SetPos(RECT rc);
		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		CEditWnd* m_pWindow;

		UINT m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		wchar_t m_cPasswordChar;
		UINT m_uButtonState;
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sNormalImage;
		std::unique_ptr<CImage> m_sHotImage;
		std::unique_ptr<CImage> m_sFocusedImage;
		std::unique_ptr<CImage> m_sDisabledImage;
#pragma warning(pop)
		DWORD m_dwEditbkColor;
		int m_iWindowStyls;
	};
}
#endif

#endif // __UIEDIT_H__