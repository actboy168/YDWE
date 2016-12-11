#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CLabelUI : public CControlUI
	{
	public:
		CLabelUI();

		const wchar_t* GetClass() const;

		// 文本相关
		virtual std::wstring const& GetText() const;
		virtual void SetText(const wchar_t* pstrText);

		void SetTextStyle(UINT uStyle);
		UINT GetTextStyle() const;
		void SetTextColor(DWORD dwTextColor);
		DWORD GetTextColor() const;
		void SetDisabledTextColor(DWORD dwTextColor);
		DWORD GetDisabledTextColor() const;
		void SetFont(int index);
		int  GetFont() const;
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);

		SIZE EstimateSize(SIZE szAvailable);
		void DoEvent(TEventUI& event);
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

		void PaintText(HDC hDC);

	protected:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring m_sText;
#pragma warning(pop)
		DWORD m_dwTextColor;
		DWORD m_dwDisabledTextColor;
		int   m_iFont;
		UINT  m_uTextStyle;
		RECT  m_rcTextPadding;
	};
}

#endif // __UILABEL_H__