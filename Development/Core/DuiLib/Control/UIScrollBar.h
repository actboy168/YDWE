#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CScrollBarUI : public CControlUI
	{
	public:
		CScrollBarUI();

		const wchar_t* GetClass() const;

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		bool GetShowButton2();
		void SetShowButton2(bool bShow);

		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sBkNormalImage;
		std::unique_ptr<CImage> m_sBkHotImage;
		std::unique_ptr<CImage> m_sBkPushedImage;
		std::unique_ptr<CImage> m_sBkDisabledImage;
#pragma warning(pop)

		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sButton1NormalImage;
		std::unique_ptr<CImage> m_sButton1HotImage;
		std::unique_ptr<CImage> m_sButton1PushedImage;
		std::unique_ptr<CImage> m_sButton1DisabledImage;
#pragma warning(pop)

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sButton2NormalImage;
		std::unique_ptr<CImage> m_sButton2HotImage;
		std::unique_ptr<CImage> m_sButton2PushedImage;
		std::unique_ptr<CImage> m_sButton2DisabledImage;
#pragma warning(pop)

		RECT m_rcThumb;
		UINT m_uThumbState;
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sThumbNormalImage;
		std::unique_ptr<CImage> m_sThumbHotImage;
		std::unique_ptr<CImage> m_sThumbPushedImage;
		std::unique_ptr<CImage> m_sThumbDisabledImage;
		std::wstring            m_sImageModify;
#pragma warning(pop)

	};
}

#endif // __UISCROLLBAR_H__