#ifndef __UIRADIOBUTTON_H__
#define __UIRADIOBUTTON_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CRadioButtonUI : public CCheckBoxUI
	{
	public:
		const wchar_t* GetClass() const;

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit);
		std::wstring const& GetGroup() const;
		void SetGroup(const wchar_t* pStrGroupName);

		void Selected(bool bSelected);
		bool Activate();
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

	protected:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring		m_sGroupName;
#pragma warning(pop)
	};
}

#endif // __UICHECKBOX_H__
