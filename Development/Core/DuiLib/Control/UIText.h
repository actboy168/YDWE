#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

#ifdef DUI_USE_TEXT

namespace DuiLib
{
	class UILIB_API CTextUI : public CLabelUI
	{
	public:
		CTextUI();

		const wchar_t* GetClass() const;
		UINT GetControlFlags() const;

		std::wstring* GetLinkContent(int iIndex);

		void DoEvent(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
#pragma warning(suppress:4251)
		std::wstring m_sLinks[MAX_LINK];
		int m_nHoverLink;
	};

} // namespace DuiLib

#endif

#endif //__UITEXT_H__