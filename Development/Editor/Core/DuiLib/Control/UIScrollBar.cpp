#include "stdafx.h"
#include "UIScrollBar.h"
#include <base/util/format.h>

namespace DuiLib
{
	CScrollBarUI::CScrollBarUI() : m_bHorizontal(false), m_nRange(100), m_nScrollPos(0), m_nLineSize(8), 
		m_pOwner(NULL), m_nLastScrollPos(0), m_nLastScrollOffset(0), m_nScrollRepeatDelay(0), m_uButton1State(0), \
		m_uButton2State(0), m_uThumbState(0), m_bShowButton1(true), m_bShowButton2(true)
	{
		m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
		ptLastMouse.x = ptLastMouse.y = 0;
		::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
		::ZeroMemory(&m_rcButton1, sizeof(m_rcButton1));
		::ZeroMemory(&m_rcButton2, sizeof(m_rcButton2));
	}

	const wchar_t* CScrollBarUI::GetClass() const
	{
		return DUI_CTR_SCROLLBAR;
	}

	CContainerUI* CScrollBarUI::GetOwner() const
	{
		return m_pOwner;
	}

	void CScrollBarUI::SetOwner(CContainerUI* pOwner)
	{
		m_pOwner = pOwner;
	}

	void CScrollBarUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;
		m_bVisible = bVisible;
		if( m_bFocused ) m_bFocused = false;
	}

	void CScrollBarUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButton1State = 0;
			m_uButton2State = 0;
			m_uThumbState = 0;
		}
	}

	void CScrollBarUI::SetFocus()
	{
		if( m_pOwner != NULL ) m_pOwner->SetFocus();
		else CControlUI::SetFocus();
	}

	bool CScrollBarUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CScrollBarUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		if( m_bHorizontal ) {
			if( m_cxyFixed.cy == 0 ) {
				m_cxyFixed.cx = 0;
				m_cxyFixed.cy = DEFAULT_SCROLLBAR_SIZE;
			}
		}
		else {
			if( m_cxyFixed.cx == 0 ) {
				m_cxyFixed.cx = DEFAULT_SCROLLBAR_SIZE;
				m_cxyFixed.cy = 0;
			}
		}

		if( m_pOwner != NULL ) m_pOwner->NeedUpdate(); else NeedParentUpdate();
	}

	int CScrollBarUI::GetScrollRange() const
	{
		return m_nRange;
	}

	void CScrollBarUI::SetScrollRange(int nRange)
	{
		if( m_nRange == nRange ) return;

		m_nRange = nRange;
		if( m_nRange < 0 ) m_nRange = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(m_rcItem);
	}

	int CScrollBarUI::GetScrollPos() const
	{
		return m_nScrollPos;
	}

	void CScrollBarUI::SetScrollPos(int nPos)
	{
		if( m_nScrollPos == nPos ) return;

		m_nScrollPos = nPos;
		if( m_nScrollPos < 0 ) m_nScrollPos = 0;
		if( m_nScrollPos > m_nRange ) m_nScrollPos = m_nRange;
		SetPos(m_rcItem);
	}

	int CScrollBarUI::GetLineSize() const
	{
		return m_nLineSize;
	}

	void CScrollBarUI::SetLineSize(int nSize)
	{
		m_nLineSize = nSize;
	}

	bool CScrollBarUI::GetShowButton1()
	{
		return m_bShowButton1;
	}

	void CScrollBarUI::SetShowButton1(bool bShow)
	{
		m_bShowButton1 = bShow;
		SetPos(m_rcItem);
	}

	bool CScrollBarUI::GetShowButton2()
	{
		return m_bShowButton2;
	}

	void CScrollBarUI::SetShowButton2(bool bShow)
	{
		m_bShowButton2 = bShow;
		SetPos(m_rcItem);
	}

	void CScrollBarUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		if( m_bHorizontal ) {
			int cx = rc.right - rc.left;
			if( m_bShowButton1 ) cx -= m_cxyFixed.cy;
			if( m_bShowButton2 ) cx -= m_cxyFixed.cy;
			if( cx > m_cxyFixed.cy ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cy;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - m_cxyFixed.cy;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				m_rcThumb.top = rc.top;
				m_rcThumb.bottom = rc.top + m_cxyFixed.cy;
				if( m_nRange > 0 ) {
					int cxThumb = cx * (rc.right - rc.left) / (m_nRange + rc.right - rc.left);
					if (cxThumb < cx / 4) cxThumb = cx / 4;
					if (cxThumb < m_cxyFixed.cy) cxThumb = m_cxyFixed.cy;

					m_rcThumb.left = m_nScrollPos * (cx - cxThumb) / m_nRange + m_rcButton1.right;
					m_rcThumb.right = m_rcThumb.left + cxThumb;
					if( m_rcThumb.right > m_rcButton2.left ) {
						m_rcThumb.left = m_rcButton2.left - cxThumb;
						m_rcThumb.right = m_rcButton2.left;
					}
				}
				else {
					m_rcThumb.left = m_rcButton1.right;
					m_rcThumb.right = m_rcButton2.left;
				}
			}
			else {
				int cxButton = (rc.right - rc.left) / 2;
				if( cxButton > m_cxyFixed.cy ) cxButton = m_cxyFixed.cy;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + cxButton;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.top = rc.top;
				m_rcButton2.right = rc.right;
				if( m_bShowButton2 ) {
					m_rcButton2.left = rc.right - cxButton;
					m_rcButton2.bottom = rc.top + m_cxyFixed.cy;
				}
				else {
					m_rcButton2.left = m_rcButton2.right;
					m_rcButton2.bottom = m_rcButton2.top;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
		else {
			int cy = rc.bottom - rc.top;
			if( m_bShowButton1 ) cy -= m_cxyFixed.cx;
			if( m_bShowButton2 ) cy -= m_cxyFixed.cx;
			if( cy > m_cxyFixed.cx ) {
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + m_cxyFixed.cx;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - m_cxyFixed.cx;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				m_rcThumb.left = rc.left;
				m_rcThumb.right = rc.left + m_cxyFixed.cx;
				if( m_nRange > 0 ) {
					int cyThumb = cy * (rc.bottom - rc.top) / (m_nRange + rc.bottom - rc.top);
					if (cyThumb < cy / 4) cyThumb = cy / 4;
					if (cyThumb < m_cxyFixed.cx) cyThumb = m_cxyFixed.cx;

					m_rcThumb.top = m_nScrollPos * (cy - cyThumb) / m_nRange + m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcThumb.top + cyThumb;
					if( m_rcThumb.bottom > m_rcButton2.top ) {
						m_rcThumb.top = m_rcButton2.top - cyThumb;
						m_rcThumb.bottom = m_rcButton2.top;
					}
				}
				else {
					m_rcThumb.top = m_rcButton1.bottom;
					m_rcThumb.bottom = m_rcButton2.top;
				}
			}
			else {
				int cyButton = (rc.bottom - rc.top) / 2;
				if( cyButton > m_cxyFixed.cx ) cyButton = m_cxyFixed.cx;
				m_rcButton1.left = rc.left;
				m_rcButton1.top = rc.top;
				if( m_bShowButton1 ) {
					m_rcButton1.right = rc.left + m_cxyFixed.cx;
					m_rcButton1.bottom = rc.top + cyButton;
				}
				else {
					m_rcButton1.right = m_rcButton1.left;
					m_rcButton1.bottom = m_rcButton1.top;
				}

				m_rcButton2.left = rc.left;
				m_rcButton2.bottom = rc.bottom;
				if( m_bShowButton2 ) {
					m_rcButton2.top = rc.bottom - cyButton;
					m_rcButton2.right = rc.left + m_cxyFixed.cx;
				}
				else {
					m_rcButton2.top = m_rcButton2.bottom;
					m_rcButton2.right = m_rcButton2.left;
				}

				::ZeroMemory(&m_rcThumb, sizeof(m_rcThumb));
			}
		}
	}

	void CScrollBarUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( !IsEnabled() ) return;

			m_nLastScrollOffset = 0;
			m_nScrollRepeatDelay = 0;
			m_pManager->SetTimer(this, DEFAULT_TIMERID, 50U);

			if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
				m_uButton1State |= UISTATE_PUSHED;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
				m_uButton2State |= UISTATE_PUSHED;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else if( ::PtInRect(&m_rcThumb, event.ptMouse) ) {
				m_uThumbState |= UISTATE_CAPTURED | UISTATE_PUSHED;
				ptLastMouse = event.ptMouse;
				m_nLastScrollPos = m_nScrollPos;
			}
			else {
				if( !m_bHorizontal ) {
					if( event.ptMouse.y < m_rcThumb.top ) {
						if( m_pOwner != NULL ) m_pOwner->PageUp(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					}
					else if ( event.ptMouse.y > m_rcThumb.bottom ){
						if( m_pOwner != NULL ) m_pOwner->PageDown(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
					}
				}
				else {
					if( event.ptMouse.x < m_rcThumb.left ) {
						if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
					}
					else if ( event.ptMouse.x > m_rcThumb.right ){
						if( m_pOwner != NULL ) m_pOwner->PageRight(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
					}
				}
			}
			if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			m_nScrollRepeatDelay = 0;
			m_nLastScrollOffset = 0;
			m_pManager->KillTimer(this, DEFAULT_TIMERID);

			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				m_uThumbState &= ~( UISTATE_CAPTURED | UISTATE_PUSHED );
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
				m_uButton1State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
				m_uButton2State &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				if( !m_bHorizontal ) {
					m_nLastScrollOffset = (event.ptMouse.y - ptLastMouse.y) * m_nRange / \
						(m_rcItem.bottom - m_rcItem.top - m_rcThumb.bottom + m_rcThumb.top - 2 * m_cxyFixed.cx);
				}
				else {
					m_nLastScrollOffset = (event.ptMouse.x - ptLastMouse.x) * m_nRange / \
						(m_rcItem.right - m_rcItem.left - m_rcThumb.right + m_rcThumb.left - 2 * m_cxyFixed.cy);
				}
			}
			else {
				if( !IsEnabled() ) return;

				if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
					m_uButton1State |= UISTATE_HOT;
				} else {
					m_uButton1State &= ~UISTATE_HOT;
				}

				if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
					m_uButton2State |= UISTATE_HOT;
				} else {
					m_uButton2State &= ~UISTATE_HOT;
				}

				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_TIMER && event.wParam == DEFAULT_TIMERID )
		{
			++m_nScrollRepeatDelay;
			if( (m_uThumbState & UISTATE_CAPTURED) != 0 ) {
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->SetScrollPos(CSize(m_pOwner->GetScrollPos().cx, \
						m_nLastScrollPos + m_nLastScrollOffset)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->SetScrollPos(CSize(m_nLastScrollPos + m_nLastScrollOffset, \
						m_pOwner->GetScrollPos().cy)); 
					else SetScrollPos(m_nLastScrollPos + m_nLastScrollOffset);
				}
				Invalidate();
			}
			else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineUp(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineLeft(); 
					else SetScrollPos(m_nScrollPos - m_nLineSize);
				}
			}
			else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
				if( m_nScrollRepeatDelay <= 5 ) return;
				if( !m_bHorizontal ) {
					if( m_pOwner != NULL ) m_pOwner->LineDown(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
				else {
					if( m_pOwner != NULL ) m_pOwner->LineRight(); 
					else SetScrollPos(m_nScrollPos + m_nLineSize);
				}
			}
			else {
				if( m_nScrollRepeatDelay <= 5 ) return;
				POINT pt = { 0 };
				::GetCursorPos(&pt);
				::ScreenToClient(m_pManager->GetPaintWindow(), &pt);
				if( !m_bHorizontal ) {
					if( pt.y < m_rcThumb.top ) {
						if( m_pOwner != NULL ) m_pOwner->PageUp(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.top - m_rcItem.bottom);
					}
					else if ( pt.y > m_rcThumb.bottom ){
						if( m_pOwner != NULL ) m_pOwner->PageDown(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.top + m_rcItem.bottom);                    
					}
				}
				else {
					if( pt.x < m_rcThumb.left ) {
						if( m_pOwner != NULL ) m_pOwner->PageLeft(); 
						else SetScrollPos(m_nScrollPos + m_rcItem.left - m_rcItem.right);
					}
					else if ( pt.x > m_rcThumb.right ){
						if( m_pOwner != NULL ) m_pOwner->PageRight(); 
						else SetScrollPos(m_nScrollPos - m_rcItem.left + m_rcItem.right);                    
					}
				}
			}
			if( m_pManager != NULL && m_pOwner == NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_SCROLL);
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				if( ::PtInRect(&m_rcButton1, event.ptMouse) ) {
					m_uButton1State |= UISTATE_HOT;
				}
				else if( ::PtInRect(&m_rcButton2, event.ptMouse) ) {
					m_uButton2State |= UISTATE_HOT;
				}
					
				m_uThumbState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButton1State &= ~UISTATE_HOT;
				m_uButton2State &= ~UISTATE_HOT;
				m_uThumbState   &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}

		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CScrollBarUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"button1normalimage") == 0 )        m_sButton1NormalImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button1hotimage") == 0 )      m_sButton1HotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button1pushedimage") == 0 )   m_sButton1PushedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button1disabledimage") == 0 ) m_sButton1DisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button2normalimage") == 0 )   m_sButton2NormalImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button2hotimage") == 0 )      m_sButton2HotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button2pushedimage") == 0 )   m_sButton2PushedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"button2disabledimage") == 0 ) m_sButton2DisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"thumbhotimage") == 0 )        m_sThumbHotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"thumbpushedimage") == 0 )     m_sThumbPushedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"thumbnormalimage") == 0 )     m_sThumbNormalImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"thumbdisabledimage") == 0 )   m_sThumbDisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"bknormalimage") == 0 )        m_sBkNormalImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"bkhotimage") == 0 )           m_sBkHotImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"bkpushedimage") == 0 )        m_sBkPushedImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"bkdisabledimage") == 0 )      m_sBkDisabledImage.reset(new CImage(pstrValue));
		else if( wcscmp(pstrName, L"hor") == 0 ) SetHorizontal(wcscmp(pstrValue, L"true") == 0);
		else if( wcscmp(pstrName, L"linesize") == 0 ) SetLineSize(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"range") == 0 ) SetScrollRange(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"value") == 0 ) SetScrollPos(_wtoi(pstrValue));
		else if( wcscmp(pstrName, L"showbutton1") == 0 ) SetShowButton1(wcscmp(pstrValue, L"true") == 0);
		else if( wcscmp(pstrName, L"showbutton2") == 0 ) SetShowButton2(wcscmp(pstrValue, L"true") == 0);
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CScrollBarUI::DoPaint(HDC hDC, const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		PaintBk(hDC);
		PaintButton1(hDC);
		PaintButton2(hDC);
		PaintThumb(hDC);
	}

	void CScrollBarUI::PaintBk(HDC hDC)
	{
		if (!(m_uThumbState & UISTATE_HOT) && !(m_uThumbState & UISTATE_PUSHED)) return;

		if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~ UISTATE_DISABLED;

		if( !m_bHorizontal ) {
			m_sImageModify = base::format(L"corner='%d,%d,%d,%d' dest='%d,%d,%d,%d'", 
				0,
				m_cxyFixed.cx / 2, 
				0, 
				m_cxyFixed.cx / 2,
				0,
				m_cxyFixed.cx, 
				m_rcItem.right - m_rcItem.left, 
				m_rcItem.bottom - m_rcItem.top - m_cxyFixed.cx);
		}
		else {
			assert(false);
		}

		if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
			if( m_sBkDisabledImage ) {
				if( !DrawImage(hDC, *m_sBkDisabledImage.get(), m_sImageModify.c_str()) ) m_sBkDisabledImage.reset();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
			if( m_sBkPushedImage ) {
				if( !DrawImage(hDC, *m_sBkPushedImage.get(), m_sImageModify.c_str()) ) m_sBkPushedImage.reset();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
			if( m_sBkHotImage ) {
				if( !DrawImage(hDC, *m_sBkHotImage.get(), m_sImageModify.c_str()) ) m_sBkHotImage.reset();
				else return;
			}
		}

		if( m_sBkNormalImage) {
			if( !DrawImage(hDC, *m_sBkNormalImage.get(), m_sImageModify.c_str()) ) m_sBkNormalImage.reset();
			else return;
		}
	}

	void CScrollBarUI::PaintButton1(HDC hDC)
	{
		if( !m_bShowButton1 ) return;

		if (!(m_uThumbState & UISTATE_HOT) && !(m_uThumbState & UISTATE_PUSHED)) return;

		if( !IsEnabled() ) m_uButton1State |= UISTATE_DISABLED;
		else m_uButton1State &= ~ UISTATE_DISABLED;

		m_sImageModify = base::format(L"dest='%d,%d,%d,%d'", m_rcButton1.left - m_rcItem.left, m_rcButton1.top - m_rcItem.top, m_rcButton1.right - m_rcItem.left, m_rcButton1.bottom - m_rcItem.top);

		if( (m_uButton1State & UISTATE_DISABLED) != 0 ) {
			if( m_sButton1DisabledImage ) {
				if( !DrawImage(hDC, *m_sButton1DisabledImage.get(), m_sImageModify.c_str()) ) m_sButton1DisabledImage.reset();
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_PUSHED) != 0 ) {
			if( m_sButton1PushedImage ) {
				if( !DrawImage(hDC, *m_sButton1PushedImage.get(), m_sImageModify.c_str()) ) m_sButton1PushedImage.reset();
				else return;
			}
		}
		else if( (m_uButton1State & UISTATE_HOT) != 0 ) {
			if( m_sButton1HotImage ) {
				if( !DrawImage(hDC, *m_sButton1HotImage.get(), m_sImageModify.c_str()) ) m_sButton1HotImage.reset();
				else return;
			}
		}

		if( m_sButton1NormalImage ) {
			if( !DrawImage(hDC, *m_sButton1NormalImage.get(), m_sImageModify.c_str()) ) m_sButton1NormalImage.reset();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcButton1, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintButton2(HDC hDC)
	{
		if( !m_bShowButton2 ) return;

		if (!(m_uThumbState & UISTATE_HOT) && !(m_uThumbState & UISTATE_PUSHED)) return;

		if( !IsEnabled() ) m_uButton2State |= UISTATE_DISABLED;
		else m_uButton2State &= ~ UISTATE_DISABLED;

		m_sImageModify = base::format(L"dest='%d,%d,%d,%d'", m_rcButton2.left - m_rcItem.left, m_rcButton2.top - m_rcItem.top, m_rcButton2.right - m_rcItem.left, m_rcButton2.bottom - m_rcItem.top);

		if( (m_uButton2State & UISTATE_DISABLED) != 0 ) {
			if( m_sButton2DisabledImage ) {
				if( !DrawImage(hDC, *m_sButton2DisabledImage.get(), m_sImageModify.c_str()) ) m_sButton2DisabledImage.reset();
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_PUSHED) != 0 ) {
			if( m_sButton2PushedImage ) {
				if( !DrawImage(hDC, *m_sButton2PushedImage.get(), m_sImageModify.c_str()) ) m_sButton2PushedImage.reset();
				else return;
			}
		}
		else if( (m_uButton2State & UISTATE_HOT) != 0 ) {
			if( m_sButton2HotImage ) {
				if( !DrawImage(hDC, *m_sButton2HotImage.get(), m_sImageModify.c_str()) ) m_sButton2HotImage.reset();
				else return;
			}
		}

		if( m_sButton2NormalImage ) {
			if( !DrawImage(hDC, *m_sButton2NormalImage.get(), m_sImageModify.c_str()) ) m_sButton2NormalImage.reset();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcButton2, nBorderSize, dwBorderColor);
	}

	void CScrollBarUI::PaintThumb(HDC hDC)
	{
		if( m_rcThumb.left == 0 && m_rcThumb.top == 0 && m_rcThumb.right == 0 && m_rcThumb.bottom == 0 ) return;
		if( !IsEnabled() ) m_uThumbState |= UISTATE_DISABLED;
		else m_uThumbState &= ~ UISTATE_DISABLED;

		if( !m_bHorizontal ) {
			m_sImageModify = base::format(L"corner='%d,%d,%d,%d' dest='%d,%d,%d,%d'", 
				0,
				m_cxyFixed.cx / 2, 
				0, 
				m_cxyFixed.cx / 2,
				m_rcThumb.left - m_rcItem.left,
				m_rcThumb.top - m_rcItem.top, 
				m_rcThumb.right - m_rcItem.left, 
				m_rcThumb.bottom - m_rcItem.top);
		}
		else {
			assert(false);
		}

		if( (m_uThumbState & UISTATE_DISABLED) != 0 ) {
			if( m_sThumbDisabledImage ) {
				if( !DrawImage(hDC, *m_sThumbDisabledImage.get(), m_sImageModify.c_str()) ) m_sThumbDisabledImage.reset();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_PUSHED) != 0 ) {
			if( m_sThumbPushedImage ) {
				if( !DrawImage(hDC, *m_sThumbPushedImage.get(), m_sImageModify.c_str()) ) m_sThumbPushedImage.reset();
				else return;
			}
		}
		else if( (m_uThumbState & UISTATE_HOT) != 0 ) {
			if( m_sThumbHotImage ) {
				if( !DrawImage(hDC, *m_sThumbHotImage.get(), m_sImageModify.c_str()) ) m_sThumbHotImage.reset();
				else return;
			}
		}

		if( m_sThumbNormalImage ) {
			if( !DrawImage(hDC, *m_sThumbNormalImage.get(), m_sImageModify.c_str()) ) m_sThumbNormalImage.reset();
			else return;
		}

		DWORD dwBorderColor = 0xFF85E4FF;
		int nBorderSize = 2;
		CRenderEngine::DrawRect(hDC, m_rcThumb, nBorderSize, dwBorderColor);
	}
}
