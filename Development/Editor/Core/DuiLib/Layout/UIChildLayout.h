#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CChildLayoutUI : public CContainerUI
	{
	public:
		CChildLayoutUI();

		void Init();
		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
		void SetChildLayoutXML(const std::wstring& pXML);
		const std::wstring& GetChildLayoutXML() const;
		virtual const wchar_t* GetClass() const;

	private:
#pragma warning(suppress:4251)
		std::wstring m_pstrXMLFile;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
