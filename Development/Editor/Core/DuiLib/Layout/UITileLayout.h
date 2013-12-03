#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CTileLayoutUI : public CContainerUI
	{
	public:
		CTileLayoutUI();

		const wchar_t* GetClass() const;

		void SetPos(RECT rc);

		SIZE GetItemSize() const;
		void SetItemSize(SIZE szItem);
		int GetColumns() const;
		void SetColumns(int nCols);

		void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

	protected:
		SIZE m_szItem;
		int m_nColumns;
	};
}
#endif // __UITILELAYOUT_H__
