#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

#ifdef DUI_USE_PROGRESS

namespace DuiLib
{
	class UILIB_API CProgressUI : public CLabelUI
	{
	public:
		CProgressUI();

		const wchar_t* GetClass() const;

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		bool IsStretchForeImage();
		void SetStretchForeImage(bool bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);

		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		bool m_bHorizontal;
		bool m_bStretchForeImage;
		int m_nMax;
		int m_nMin;
		int m_nValue;

#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<CImage> m_sForeImage;
		std::wstring m_sForeImageModify;
#pragma warning(pop)
	};

} // namespace DuiLib
#endif

#endif // __UIPROGRESS_H__
