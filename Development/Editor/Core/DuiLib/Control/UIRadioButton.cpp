#include "stdafx.h"
#include "UIRadioButton.h"

namespace DuiLib
{
	const wchar_t* CRadioButtonUI::GetClass() const
	{
		return DUI_CTR_RADIOBUTTON;
	}

	void CRadioButtonUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CControlUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.empty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
		}
	}

	std::wstring const& CRadioButtonUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void CRadioButtonUI::SetGroup(const wchar_t* pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.empty() ) return;
			m_sGroupName.clear();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.empty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.empty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName.c_str(), this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName.c_str(), this);
		}

		Selected(m_bSelected);
	}

	void CRadioButtonUI::Selected(bool bSelected)
	{
		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;

		if (m_pManager != NULL) 
		{
			if (!m_sGroupName.empty()) 
			{
				if (m_bSelected)
				{
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName.c_str());
					for (int i = 0; i < aOptionGroup->GetSize(); i++)
					{
						CRadioButtonUI* pControl = static_cast<CRadioButtonUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) 
						{
							pControl->Selected(false);
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
			}
			else 
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool CRadioButtonUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.empty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void CRadioButtonUI::SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue)
	{
		if( wcscmp(pstrName, L"group") == 0 ) SetGroup(pstrValue);
		else CCheckBoxUI::SetAttribute(pstrName, pstrValue);
	}
}
