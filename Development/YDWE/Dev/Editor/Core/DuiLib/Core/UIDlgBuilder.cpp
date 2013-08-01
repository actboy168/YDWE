#include "StdAfx.h"

namespace DuiLib {

CDialogBuilder::CDialogBuilder() : m_pCallback(NULL)
{

}

CControlUI* CDialogBuilder::Create(STRINGorID xml, IDialogBuilderCallback* pCallback, 
                                   CPaintManagerUI* pManager, CControlUI* pParent)
{
	if( *(xml.m_lpstr) == _T('<') ) 
	{
		if( !m_xml.Load(xml.m_lpstr) ) return NULL;
	}
	else
	{
		if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
	}

    return Create(pCallback, pManager, pParent);
}

CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
{
    m_pCallback = pCallback;
    CMarkupNode root = m_xml.GetRoot();
    if( !root.IsValid() ) return NULL;

    if (pManager) 
	{
        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;
        for (CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
            pstrClass = node.GetName();
            if (_tcscmp(pstrClass, _T("Image")) == 0)
			{
                nAttributes = node.GetAttributeCount();
                LPCTSTR pImageName = NULL;
                DWORD mask = 0;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcscmp(pstrName, _T("name")) == 0 ) {
                        pImageName = pstrValue;
					}
                    else if( _tcscmp(pstrName, _T("mask")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        mask = _tcstoul(pstrValue, &pstr, 16);
                    }
                }
                if( pImageName ) pManager->AddImage(pImageName, mask);
            }
            else if( _tcscmp(pstrClass, _T("Font")) == 0 ) 
			{
                nAttributes = node.GetAttributeCount();
                LPCTSTR pFontName = NULL;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);
                    if( _tcscmp(pstrName, _T("name")) == 0 ) {
                        pFontName = pstrValue;
                    }
                    else if( _tcscmp(pstrName, _T("size")) == 0 ) {
                        size = _tcstol(pstrValue, &pstr, 10);
                    }
                    else if( _tcscmp(pstrName, _T("bold")) == 0 ) {
                        bold = (_tcscmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcscmp(pstrName, _T("underline")) == 0 ) {
                        underline = (_tcscmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcscmp(pstrName, _T("italic")) == 0 ) {
                        italic = (_tcscmp(pstrValue, _T("true")) == 0);
                    }
                    else if( _tcscmp(pstrName, _T("default")) == 0 ) {
                        defaultfont = (_tcscmp(pstrValue, _T("true")) == 0);
                    }
                }
                if( pFontName ) {
                    pManager->AddFont(pFontName, size, bold, underline, italic);
                    if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
                }
            }
        }

        pstrClass = root.GetName();
        if( _tcscmp(pstrClass, _T("Window")) == 0 ) {
            if( pManager->GetPaintWindow() ) {
                int nAttributes = root.GetAttributeCount();
                for( int i = 0; i < nAttributes; i++ ) {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    if( _tcscmp(pstrName, _T("size")) == 0 ) {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
                        pManager->SetInitSize(cx, cy);
                    } 
                    else if( _tcscmp(pstrName, _T("sizebox")) == 0 ) {
                        RECT rcSizeBox = { 0 };
                        LPTSTR pstr = NULL;
                        rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
                        rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
                        rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
                        pManager->SetSizeBox(rcSizeBox);
                    }
                    else if( _tcscmp(pstrName, _T("caption")) == 0 ) {
                        RECT rcCaption = { 0 };
                        LPTSTR pstr = NULL;
                        rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
                        rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
                        rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
                        pManager->SetCaptionRect(rcCaption);
                    }
                    else if( _tcscmp(pstrName, _T("roundcorner")) == 0 ) {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
                        pManager->SetRoundCorner(cx, cy);
                    } 
                    else if( _tcscmp(pstrName, _T("mininfo")) == 0 ) {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
                        pManager->SetMinInfo(cx, cy);
                    }
                    else if( _tcscmp(pstrName, _T("maxinfo")) == 0 ) {
                        LPTSTR pstr = NULL;
                        int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
                        int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
                        pManager->SetMaxInfo(cx, cy);
                    }
                    else if( _tcscmp(pstrName, _T("showdirty")) == 0 ) {
                        pManager->SetShowUpdateRect(_tcscmp(pstrValue, _T("true")) == 0);
                    } 
                    else if( _tcscmp(pstrName, _T("alpha")) == 0 ) {
                        pManager->SetTransparent(_ttoi(pstrValue));
                    } 
                    else if( _tcscmp(pstrName, _T("bktrans")) == 0 ) {
                        pManager->SetBackgroundTransparent(_tcscmp(pstrValue, _T("true")) == 0);
                    } 
                    else if( _tcscmp(pstrName, _T("disabledfontcolor")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultDisabledColor(clrColor);
                    } 
                    else if( _tcscmp(pstrName, _T("defaultfontcolor")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultFontColor(clrColor);
                    }
                    else if( _tcscmp(pstrName, _T("linkfontcolor")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultLinkFontColor(clrColor);
                    } 
                    else if( _tcscmp(pstrName, _T("linkhoverfontcolor")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultLinkHoverFontColor(clrColor);
                    } 
                    else if( _tcscmp(pstrName, _T("selectedcolor")) == 0 ) {
                        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
                        LPTSTR pstr = NULL;
                        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
                        pManager->SetDefaultSelectedBkColor(clrColor);
                    } 
                }
            }
        }
    }
    return _Parse(&root, pParent, pManager);
}

CMarkup* CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
{
    IContainerUI* pContainer = NULL;
    CControlUI* pReturn = NULL;
    for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() ) {
        LPCTSTR pstrClass = node.GetName();
        if( _tcscmp(pstrClass, _T("Image")) == 0 || _tcscmp(pstrClass, _T("Font")) == 0 \
             ) continue;

        CControlUI* pControl = NULL;

		if (_tcscmp(pstrClass, _T("Class")) == 0) 
		{
			LPCTSTR pControlName = node.GetAttributeValue(_T("name"));

			if (pControlName && pControlName[0]) 
			{
				int nAttributes = node.GetAttributeCount();
				for (int i = 0; i < nAttributes; ++i) 
				{
					wchar_t const* pstrName = node.GetAttributeName(i);
					wchar_t const* pstrValue = node.GetAttributeValue(i);
					if (_tcscmp(pstrName, _T("class")) == 0)
					{
						CAttributesList* pParent = pManager->GetDefaultAttributeList(pstrValue);
						if (pParent)
						{
							foreach(auto it, *pParent)
							{
								pManager->AddDefaultAttribute(pControlName, it.first.c_str(), it.second.c_str());
							}
						}
					}
					else if (_tcscmp(pstrName, _T("name")) != 0)
					{
						pManager->AddDefaultAttribute(pControlName, pstrName, pstrValue);
					}
				}
			}
		}
        else if (_tcscmp(pstrClass, _T("Include")) == 0)
		{
			LPCTSTR szValue = node.GetAttributeValue(_T("source"));
			if (szValue && szValue[0])
			{
				CDialogBuilder builder;
				pControl = builder.Create((LPCTSTR)szValue, m_pCallback, pManager, pParent);
			}
            continue;
        }
		//树控件XML解析
		else if( _tcscmp(pstrClass, _T("TreeNode")) == 0 ) {
			CTreeNodeUI* pParentNode	= dynamic_cast<CTreeNodeUI*>(pParent);
			CTreeNodeUI* pNode			= new CTreeNodeUI();
			if(pParentNode){
				if(!pParentNode->Add(pNode)){
					delete pNode;
					continue;
				}
			}

			// 若有控件默认配置先初始化默认属性
			if (pManager) 
			{
				pNode->SetManager(pManager, NULL, false);
				CAttributesList* pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if (pDefaultAttributes) 
				{
					pNode->ApplyAttributeTable(pDefaultAttributes);
				}
			}

			// 解析所有属性并覆盖默认属性
			if( node.HasAttributes() ) {
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) {
					pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
				}
			}

			//检索子节点及附加控件
			if(node.HasChildren()){
				CControlUI* pSubControl = _Parse(&node,pNode,pManager);
				if(pSubControl && _tcscmp(pSubControl->GetClass(),DUI_CTR_TREENODE) != 0)
				{
					// 					pSubControl->SetFixedWidth(30);
					// 					CHorizontalLayoutUI* pHorz = pNode->GetTreeNodeHoriznotal();
					// 					pHorz->Add(new CEditUI());
					// 					continue;
				}
			}

			if(!pParentNode){
				CTreeViewUI* pTreeView = dynamic_cast<CTreeViewUI*>(pParent);
				ASSERT(pTreeView);
				if( pTreeView == NULL ) return NULL;
				if( !pTreeView->Add(pNode) ) {
					delete pNode;
					continue;
				}
			}
			continue;
		}
        else {
            SIZE_T cchLen = _tcslen(pstrClass);
            switch( cchLen ) {
            case 4:
#ifdef DUI_USE_EDIT
				if( _tcscmp(pstrClass, DUI_CTR_EDIT) == 0 )                   { pControl = new CEditUI; break; }
#endif
                if( _tcscmp(pstrClass, DUI_CTR_LIST) == 0 )                   { pControl = new CListUI; break; }
#ifdef DUI_USE_TEXT
                if( _tcscmp(pstrClass, DUI_CTR_TEXT) == 0 )                   { pControl = new CTextUI; break; }
#endif
                break;
            case 5:		
                if( _tcscmp(pstrClass, DUI_CTR_COMBO) == 0 )                  { pControl = new CComboUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_LABEL) == 0 )                  { pControl = new CLabelUI; break; }
                break;
            case 6:		
				if( _tcscmp(pstrClass, DUI_CTR_BUTTON) == 0 )                 { pControl = new CButtonUI; break; }
#ifdef DUI_USE_SLIDER
                if( _tcscmp(pstrClass, DUI_CTR_SLIDER) == 0 )                 { pControl = new CSliderUI; break; }
#endif
                break;
            case 7:		
				if( _tcscmp(pstrClass, DUI_CTR_CONTROL) == 0 )                { pControl = new CControlUI; break; }
#ifdef DUI_USE_ACTIVEX
				if( _tcscmp(pstrClass, DUI_CTR_ACTIVEX) == 0 )                { pControl = new CActiveXUI; break; }
#endif
				if( _tcscmp(pstrClass, DUI_CTR_CHECKBOX) == 0 )               { pControl = new CCheckBoxUI; break; }
                break;
			case 8:		
#ifdef DUI_USE_PROGRESS
                if( _tcscmp(pstrClass, DUI_CTR_PROGRESS) == 0 )               { pControl = new CProgressUI; break; }
#endif
#ifdef DUI_USE_RICHEDIT
                if( _tcscmp(pstrClass, DUI_CTR_RICHEDIT) == 0 )               { pControl = new CRichEditUI; break; }
#endif
				if( _tcscmp(pstrClass, DUI_CTR_CHECKBOX) == 0 )		          { pControl = new CCheckBoxUI; break; }
#ifdef DUI_USE_COMBOBOX
				if( _tcscmp(pstrClass, DUI_CTR_COMBOBOX) == 0 )		          { pControl = new CComboBoxUI; break; }
#endif
#ifdef DUI_USE_DATETIME				
				if( _tcscmp(pstrClass, DUI_CTR_DATETIME) == 0 )		          { pControl = new CDateTimeUI; break; }
#endif
				if( _tcscmp(pstrClass, DUI_CTR_TREEVIEW) == 0 )		          { pControl = new CTreeViewUI; break; }
                break;
            case 9:		
                if( _tcscmp(pstrClass, DUI_CTR_CONTAINER) == 0 )              { pControl = new CContainerUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_TABLAYOUT) == 0 )              { pControl = new CTabLayoutUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_SCROLLBAR) == 0 )              { pControl = new CScrollBarUI;  break; }
                break;
            case 10:
                if( _tcscmp(pstrClass, DUI_CTR_LISTHEADER) == 0 )             { pControl = new CListHeaderUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_TILELAYOUT) == 0 )             { pControl = new CTileLayoutUI; break; }
#ifdef DUI_USE_WEBBROWSER
				if( _tcscmp(pstrClass, DUI_CTR_WEBBROWSER) == 0 )             { pControl = new CWebBrowserUI; break; }
#endif
                break;	
			case 11:	
				if (_tcscmp(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)			  { pControl = new CChildLayoutUI; break; }
				if( _tcscmp(pstrClass, DUI_CTR_RADIOBUTTON) == 0 )            { pControl = new CRadioButtonUI; break; }
				break;													      
            case 14:													      
                if( _tcscmp(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0 )         { pControl = new CVerticalLayoutUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0 )         { pControl = new CListHeaderItemUI; break; }
                break;													      
            case 15:													      
                if( _tcscmp(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0 )        { pControl = new CListTextElementUI; break; }
                break;													      
            case 16:													      
                if( _tcscmp(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0 )       { pControl = new CHorizontalLayoutUI; break; }
                if( _tcscmp(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0 )       { pControl = new CListLabelElementUI; break; }
                break;
            case 20:
                if( _tcscmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0 )   { pControl = new CListContainerElementUI; break; }
                break;
            }
            if( pControl == NULL && m_pCallback != NULL ) {
                pControl = m_pCallback->CreateControl(pstrClass);
            }
        }

#ifndef _DEBUG
        ASSERT(pControl);
#endif // _DEBUG
			if( pControl == NULL )
			{
#ifdef _DEBUG
				DUITRACE(_T("未知控件:%s"),pstrClass);
#endif
				continue;
			}

        // Add children
        if( node.HasChildren() ) {
            _Parse(&node, pControl, pManager);
        }
        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
		if( pParent != NULL ) {
			CTreeNodeUI* pContainerNode = dynamic_cast<CTreeNodeUI*>(pParent);
			if(pContainerNode)
				pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
			else
			{
				if( pContainer == NULL ) pContainer = dynamic_cast<IContainerUI*>(pParent);
				ASSERT(pContainer);
				if( pContainer == NULL ) return NULL;
				if( !pContainer->Add(pControl) ) {
					delete pControl;
					continue;
				}
			}
		}
        // Init default attributes
        if (pManager)
		{
            pControl->SetManager(pManager, NULL, false);
            CAttributesList* pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
            if (pDefaultAttributes) 
			{
                pControl->ApplyAttributeTable(pDefaultAttributes);
            }
        }
        // Process attributes
        if( node.HasAttributes() ) {
            // Set ordinary attributes
            int nAttributes = node.GetAttributeCount();
            for( int i = 0; i < nAttributes; i++ ) {
                pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
            }
			pControl->Invalidate();
        }
        if( pManager ) {
            pControl->SetManager(NULL, NULL, false);
        }
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

} // namespace DuiLib
