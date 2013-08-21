#ifndef __UIIMAGEINFO_H__
#define __UIIMAGEINFO_H__

#pragma once

namespace DuiLib
{
	class CImageInfo
	{
	public:
		CImageInfo(HBITMAP hBitmap_, int nX_, int nY_, bool alphaChannel_)
			: hBitmap(hBitmap_)
			, nX(nX_)
			, nY(nY_)
			, alphaChannel(alphaChannel_)
			, dwMask(0)
		{ }

		~CImageInfo();

		static CImageInfo* Load(STRINGorID bitmap, DWORD mask);

	public:
		HBITMAP hBitmap;
		int nX;
		int nY;
		bool alphaChannel;
		DWORD dwMask;
	};
}

#endif
