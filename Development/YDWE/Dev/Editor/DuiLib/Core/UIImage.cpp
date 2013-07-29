#include "StdAfx.h"

namespace DuiLib
{
	CImage::CImage()
	{
	}

	CImage::CImage(LPCTSTR pStrImage)
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

	bool CImage::Draw(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, LPCTSTR pStrModify) const
	{
		if (!pStrModify)
		{
			return Draw(hDC, pManager, rc, rcPaint);
		}

		CImage temp = *this;
		temp.Parse(pStrModify);
		return temp.Draw(hDC, pManager, rc, rcPaint);
	}

	void CImage::Parse(LPCTSTR pStrImage)
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
		LPTSTR pstr = NULL;

		while (*pStrImage != _T('\0'))
		{
			sItem.clear();
			sValue.clear();
			while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) pStrImage = ::CharNext(pStrImage);
			while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ')) 
			{
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) 
				{
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('=') ) break;
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('\'') ) break;
			while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) 
			{
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) 
				{
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != _T('\'') ) break;
			if( !sValue.empty() ) 
			{
				if( sItem == _T("file") || sItem == _T("res") )
				{
					m_sImageName = sValue;
				}
				else if( sItem == _T("dest") ) 
				{
					m_rcItem.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcItem.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcItem.right =  _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcItem.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == _T("source") ) 
				{
					m_rcBmpPart.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcBmpPart.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcBmpPart.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcBmpPart.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
				}
				else if( sItem == _T("corner") )
				{
					m_rcCorner.left = _tcstol(sValue.c_str(), &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if( sItem == _T("mask") )
				{
					if( sValue[0] == _T('#')) m_dwMask = _tcstoul(sValue.c_str() + 1, &pstr, 16);
					else m_dwMask = _tcstoul(sValue.c_str(), &pstr, 16);
				}
				else if( sItem == _T("fade") )
				{
					m_bFade = (BYTE)_tcstoul(sValue.c_str(), &pstr, 10);
				}
				else if( sItem == _T("hole") ) 
				{
					m_bHole = (_tcscmp(sValue.c_str(), _T("true")) == 0);
				}
				else if( sItem == _T("xtiled") ) 
				{
					m_bTiledX = (_tcscmp(sValue.c_str(), _T("true")) == 0);
				}
				else if( sItem == _T("ytiled") ) 
				{
					m_bTiledY = (_tcscmp(sValue.c_str(), _T("true")) == 0);
				}
			}
			if( *pStrImage++ != _T(' ') ) break;
		}
	}
}
