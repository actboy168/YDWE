#include "stdafx.h"
#include "UIChildLayout.h"

namespace DuiLib
{
	CChildLayoutUI::CChildLayoutUI()
	{

	}

	void CChildLayoutUI::Init()
	{
		if (!m_pstrXMLFile.empty())
		{
			CDialogBuilder builder;
			CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(m_pstrXMLFile.c_str()));
			if (pChildWindow)
			{
				this->Add(pChildWindow);
			}
			else
			{
				this->RemoveAll();
			}
		}
	}

	void CChildLayoutUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else
			CContainerUI::SetAttribute(pstrName,pstrValue);
	}

	void CChildLayoutUI::SetChildLayoutXML(const std::wstring& pXML)
	{
		m_pstrXMLFile = pXML;
	}

	const std::wstring& CChildLayoutUI::GetChildLayoutXML() const
	{
		return m_pstrXMLFile;
	}

	LPCTSTR CChildLayoutUI::GetClass() const
	{
		return DUI_CTR_CHILDLAYOUT;
	}
} // namespace DuiLib
