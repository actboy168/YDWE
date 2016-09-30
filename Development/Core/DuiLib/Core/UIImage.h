#ifndef __UIIMAGE_H__
#define __UIIMAGE_H__

#pragma once

// 1¡¢aaa.jpg
// 2¡¢file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' 
// mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false'

namespace DuiLib
{
	class UILIB_API CImage
	{
	public:
		CImage();
		CImage(const wchar_t* pStrImage);
		bool Draw(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint) const;
		bool Draw(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const wchar_t* pStrModify) const;
		void Parse(const wchar_t* pStrImage);

	public:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring m_sImageName;
#pragma warning(pop)
		RECT m_rcItem;
		RECT m_rcBmpPart;
		RECT m_rcCorner;
		DWORD m_dwMask;
		BYTE m_bFade;
		bool m_bHole;
		bool m_bTiledX;
		bool m_bTiledY;
	};
}

#endif

