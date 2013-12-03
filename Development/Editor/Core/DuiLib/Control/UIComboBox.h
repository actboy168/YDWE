#ifndef __UICOMBOBOX_H__
#define __UICOMBOBOX_H__

#pragma once

#ifdef DUI_USE_COMBOBOX

namespace DuiLib
{
	/// 扩展下拉列表框
	/// 增加arrowimage属性,一张图片平均分成5份,Normal/Hot/Pushed/Focused/Disabled(必须有source属性)
	/// <Default name="ComboBox" value="arrowimage=&quot;file='sys_combo_btn.png' source='0,0,16,16'&quot; "/>
	class UILIB_API CComboBoxUI : public CComboUI
	{
	public:
		CComboBoxUI();
		const wchar_t* GetClass() const;

		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

		void PaintText(HDC hDC);
		void PaintStatusImage(HDC hDC);

	protected:
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sArrowImage;
#pragma warning(pop)
		int        m_nArrowWidth;
	};
}
#endif

#endif // __UICOMBOBOX_H__
