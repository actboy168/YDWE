#include "StdAfx.h"

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);
};

namespace DuiLib
{
	CImageInfo* CImageInfo::Load(STRINGorID bitmap, DWORD mask)
	{
		CUIBuffer buf;

		if (CPaintManagerUI::GetResourceZip().empty()) 
		{
			buf = CUIFile::LoadFile(CPaintManagerUI::GetResourcePath() / bitmap.m_lpstr);
		}
		else 
		{
			buf = CUIFile::LoadZip(bitmap.m_lpstr);
		}
		

		if (!buf)
		{
			buf = CUIFile::LoadFile(bitmap.m_lpstr);
		}

		if (!buf)
		{
			//::MessageBox(0, _T("¶ÁÈ¡Í¼Æ¬Êý¾ÝÊ§°Ü£¡"), _T("×¥BUG"), MB_OK);
			return NULL;
		}

		LPBYTE pImage = NULL;
		int x,y,n;
		pImage = stbi_load_from_memory(buf.ptrData.get(), buf.nSize, &x, &y, &n, 4);

		if (!pImage) 
		{
			//::MessageBox(0, _T("½âÎöÍ¼Æ¬Ê§°Ü"), _T("×¥BUG"), MB_OK);
			return NULL;
		}

		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = x;
		bmi.bmiHeader.biHeight = -y;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = x * y * 4;

		bool bAlphaChannel = false;
		LPBYTE pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if (!hBitmap) 
		{
			//::MessageBox(0, _T("CreateDIBSectionÊ§°Ü"), _T("×¥BUG"), MB_OK);
			return NULL;
		}

		for( int i = 0; i < x * y; i++ ) 
		{
			pDest[i*4 + 3] = pImage[i*4 + 3];
			if( pDest[i*4 + 3] < 255 )
			{
				pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
				pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
				pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
				bAlphaChannel = true;
			}
			else
			{
				pDest[i*4] = pImage[i*4 + 2];
				pDest[i*4 + 1] = pImage[i*4 + 1];
				pDest[i*4 + 2] = pImage[i*4]; 
			}

			if( *(DWORD*)(&pDest[i*4]) == mask ) {
				pDest[i*4] = (BYTE)0;
				pDest[i*4 + 1] = (BYTE)0;
				pDest[i*4 + 2] = (BYTE)0; 
				pDest[i*4 + 3] = (BYTE)0;
				bAlphaChannel = true;
			}
		}

		stbi_image_free(pImage);

		return new CImageInfo(hBitmap, x, y, bAlphaChannel);
	}

	CImageInfo::~CImageInfo()
	{
		if (hBitmap) 
		{
			::DeleteObject(hBitmap); 
		}
	}
}
