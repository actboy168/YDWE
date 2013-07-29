#ifndef __UIRADIOBUTTON_H__
#define __UIRADIOBUTTON_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CRadioButtonUI : public CCheckBoxUI
	{
	public:
		LPCTSTR GetClass() const;

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit);
		std::wstring const& GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName);

		void Selected(bool bSelected);
		bool Activate();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring		m_sGroupName;
#pragma warning(pop)
	};
}

#endif // __UICHECKBOX_H__
