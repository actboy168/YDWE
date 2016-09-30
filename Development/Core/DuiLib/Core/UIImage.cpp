#include "StdAfx.h"

namespace DuiLib
{
	CImage::CImage()
	{
	}

	CImage::CImage(const wchar_t* pStrImage)
	{
		Parse(pStrImage);
	}

	bool CImage::Draw(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint) const
	{
		if (m_rcItem.right == m_rcItem.left && m_rcItem.top == m_rcItem.bottom)
		{
			CDuiRect rcDest = rc;

			return CRenderEngine::DrawImage(hDC, pManager, rc, rcPaint, m_sImageName,
				rcDest, m_rcBmpPart, m_rcCorner, m_dwMask, m_bFade, m_bHole, m_bTiledX, m_bTiledY);
		}
		else
		{
			CDuiRect rcDest(
				rc.left + m_rcItem.left, 
				rc.top + m_rcItem.top, 
				(rc.left + m_rcItem.right > rc.right)? rc.right: rc.left + m_rcItem.right,
				(rc.top + m_rcItem.bottom > rc.bottom)? rc.bottom: rc.top + m_rcItem.bottom
				);

			return CRenderEngine::DrawImage(hDC, pManager, rc, rcPaint, m_sImageName,
				rcDest, m_rcBmpPart, m_rcCorner, m_dwMask, m_bFade, m_bHole, m_bTiledX, m_bTiledY);
		}
	}

	bool CImage::Draw(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const wchar_t* pStrModify) const
	{
		if (!pStrModify)
		{
			return Draw(hDC, pManager, rc, rcPaint);
		}

		CImage temp = *this;
		temp.Parse(pStrModify);
		return temp.Draw(hDC, pManager, rc, rcPaint);
	}

	void CImage::Parse(const wchar_t* pStrImage)
	{
		if (!pStrImage) return;

		if (m_sImageName.empty())
		{
			m_sImageName = pStrImage;
			m_rcItem = CDuiRect(0,0,0,0);
			m_rcBmpPart = CDuiRect(0,0,0,0);
			m_rcCorner = CDuiRect(0,0,0,0);
			m_dwMask = 0;
			m_bFade = 0xFF;
			m_bHole = false;
			m_bTiledX = false;
			m_bTiledY = false;
		}

		std::wstring sItem;
		std::wstring sValue;
		wchar_t* pstr = NULL;

		while (*pStrImage != _T('\0'))
		{
			sItem.clear();
			sValue.clear();
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNextW(pStrImage);
			while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) 
			{
				const wchar_t* pstrTemp = ::CharNextW(pStrImage);
				while( pStrImage < pstrTemp) 
				{
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNextW(pStrImage);
			if( *pStrImage++ != _T('=') ) break;
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNextW(pStrImage);
			if( *pStrImage++ != _T('\'') ) break;
			while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) 
			{
				const wchar_t* pstrTemp = ::CharNextW(pStrImage);
				while( pStrImage < pstrTemp) 
				{
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != _T('\'') ) break;
			if( !sValue.empty() ) 
			{
				if( sItem == L"file" || sItem == L"res" )
				{
					m_sImageName = sValue;
				}
				else if( sItem == L"dest" ) 
				{
					m_rcItem.left = wcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcItem.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcItem.right =  wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcItem.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == L"source" ) 
				{
					m_rcBmpPart.left = wcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcBmpPart.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcBmpPart.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcBmpPart.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
				}
				else if( sItem == L"corner" )
				{
					m_rcCorner.left = wcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.top = wcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcCorner.right = wcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.bottom = wcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == L"mask" )
				{
					if( sValue[0] == _T('#')) m_dwMask = wcstoul(sValue.c_str() + 1, &pstr, 16);
					else m_dwMask = wcstoul(sValue.c_str(), &pstr, 16);
				}
				else if( sItem == L"fade" )
				{
					m_bFade = (BYTE)wcstoul(sValue.c_str(), &pstr, 10);
				}
				else if( sItem == L"hole" ) 
				{
					m_bHole = (wcscmp(sValue.c_str(), L"true") == 0);
				}
				else if( sItem == L"xtiled" ) 
				{
					m_bTiledX = (wcscmp(sValue.c_str(), L"true") == 0);
				}
				else if( sItem == L"ytiled" ) 
				{
					m_bTiledY = (wcscmp(sValue.c_str(), L"true") == 0);
				}
			}
			if( *pStrImage++ != _T(' ') ) break;
		}
	}
}
