#include "StdAfx.h"

#include "../zip/png_utils.h"

namespace DuiLib
{
	CImageInfo* CImageInfo::Load(STRINGorID bitmap, DWORD mask)
	{
		CUIBuffer buf;

		buf = CUIFile::LoadZip(bitmap.m_lpstr);

		if (!buf)
		{
			buf = CUIFile::LoadFile(bitmap.m_lpstr);
		}

		if (!buf)
		{
			//::MessageBox(0, L"读取图片数据失败！", L"抓BUG", MB_OK);
			return NULL;
		}

		try {
			png_utils::img_t img = png_utils::load((const char*)buf.ptrData.get(), buf.nSize);
			if (img.bmp) {
				return new CImageInfo(img.bmp, img.w, img.h, true);
			}
		}
		catch (...) {
		}

		return nullptr;
	}

	CImageInfo::~CImageInfo()
	{
		if (hBitmap) 
		{
			::DeleteObject(hBitmap); 
		}
	}
}
