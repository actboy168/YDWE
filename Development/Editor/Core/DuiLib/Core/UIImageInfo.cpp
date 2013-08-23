#include "StdAfx.h"

#include <ydwe/graph/graph.h>

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
			//::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
			return NULL;
		}

		try {
			using namespace ydwe;
			graph::image_t img = graph::from_png((char*)buf.ptrData.get(), buf.nSize);
			return new CImageInfo(graph::to_bitmap(img, NULL), img.width(), img.height(), true);
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
