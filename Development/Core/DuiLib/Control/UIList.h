#ifndef __UILIST_H__
#define __UILIST_H__

#pragma once
#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CListHeaderUI;

#define UILIST_MAX_COLUMNS 32

class UILIB_API TListInfoUI
{
public:
    int nColumns;
    RECT rcColumn[UILIST_MAX_COLUMNS];
	int nFont;
    UINT uTextStyle;
    RECT rcTextPadding;
	bool bMultiExpandable;

#pragma warning(push)
#pragma warning(disable:4251)
	std::unique_ptr<CImage> sBkImage;
	std::unique_ptr<CImage> sSelectedImage;
	std::unique_ptr<CImage> sHotImage;
	std::unique_ptr<CImage> sDisabledImage;
#pragma warning(pop)

private:
	bool bAlternateBk;
	DWORD dwLineColor;

	DWORD dwTextColor;
	DWORD dwBkColor;
	DWORD dwSelectedTextColor;
	DWORD dwSelectedBkColor;
	DWORD dwHotTextColor;
	DWORD dwHotBkColor;
	DWORD dwDisabledTextColor;
	DWORD dwDisabledBkColor;

public:
	void  SetAlternateBk(bool bAlternateBk);
	bool  IsAlternateBk() const;
	DWORD GetLineColor() const;
	void  SetLineColor(DWORD dwLineColor);

	void  SetBkImage(const wchar_t* pStrImage);
	void  SetSelectedImage(const wchar_t* pStrImage);
	void  SetHotImage(const wchar_t* pStrImage);
	void  SetDisabledImage(const wchar_t* pStrImage);

	DWORD GetTextColor() const;
	DWORD GetBkColor() const;
	DWORD GetSelectedTextColor() const;
	DWORD GetSelectedBkColor() const;
	DWORD GetHotTextColor() const;
	DWORD GetHotBkColor() const;
	DWORD GetDisabledTextColor() const;
	DWORD GetDisabledBkColor() const;

	void  SetTextColor(DWORD dwTextColor);
	void  SetBkColor(DWORD dwBkColor);
	void  SetSelectedTextColor(DWORD dwTextColor);
	void  SetSelectedBkColor(DWORD dwBkColor);
	void  SetHotTextColor(DWORD dwTextColor);
	void  SetHotBkColor(DWORD dwBkColor);
	void  SetDisabledTextColor(DWORD dwTextColor);
	void  SetDisabledBkColor(DWORD dwBkColor);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class IListCallbackUI
{
public:
    virtual const wchar_t* GetItemText(CControlUI* pList, int iItem, int iSubItem) = 0;
};

class IListOwnerUI
{
public:
    virtual TListInfoUI* GetListInfo() = 0;
    virtual int GetCurSel() const = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false) = 0;
    virtual void DoEvent(TEventUI& event) = 0;
};

class IListUI : public IListOwnerUI
{
public:
    virtual CListHeaderUI* GetHeader() const = 0;
    virtual CContainerUI* GetList() const = 0;
    virtual IListCallbackUI* GetTextCallback() const = 0;
    virtual void SetTextCallback(IListCallbackUI* pCallback) = 0;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
    virtual int GetExpandedItem() const = 0;
};

class IListItemUI
{
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual IListOwnerUI* GetOwner() = 0;
    virtual void SetOwner(CControlUI* pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true) = 0;
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(HDC hDC, const RECT& rcItem) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI;
class CListHeaderUI;

class UILIB_API CListUI : public CVerticalLayoutUI, public IListUI
{
public:
    CListUI();

    const wchar_t* GetClass() const;
    UINT GetControlFlags() const;

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false);

    CListHeaderUI* GetHeader() const;  
    CContainerUI* GetList() const;
    TListInfoUI* GetListInfo();

    CControlUI* GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI* pControl) const;
    bool SetItemIndex(CControlUI* pControl, int iIndex);
    int GetCount() const;
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    int GetChildPadding() const;
    void SetChildPadding(int iPadding);

    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
    void SetItemTextPadding(RECT rc);
	RECT GetItemTextPadding() const;

    void SetMultiExpanding(bool bMultiExpandable); 
    int GetExpandedItem() const;
    bool ExpandItem(int iIndex, bool bExpand = true);

    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

    IListCallbackUI* GetTextCallback() const;
    void SetTextCallback(IListCallbackUI* pCallback);

    SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI* GetVerticalScrollBar() const;
    virtual CScrollBarUI* GetHorizontalScrollBar() const;

protected:
    bool m_bScrollSelect;
    int m_iCurSel;
    int m_iExpandedItem;
    IListCallbackUI* m_pCallback;
    CListBodyUI* m_pList;
    CListHeaderUI* m_pHeader;
    TListInfoUI m_ListInfo;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListBodyUI : public CVerticalLayoutUI
{
public:
    CListBodyUI(CListUI* pOwner);

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);

protected:
    CListUI* m_pOwner;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListHeaderUI : public CHorizontalLayoutUI
{
public:
    CListHeaderUI();

    const wchar_t* GetClass() const;

    SIZE EstimateSize(SIZE szAvailable);
};


/////////////////////////////////////////////////////////////////////////////////////
//
/*
FIXME: 继承于CControlUI而不是CLabelUI，应该更好
*/
class UILIB_API CListHeaderItemUI : public CLabelUI
{
public:
    CListHeaderItemUI();

    const wchar_t* GetClass() const;
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

	bool IsDragable() const;
    void SetDragable(bool bDragable);
	DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
	DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
	DWORD GetTextColor() const;
    void SetTextColor(DWORD dwTextColor);
	void SetTextPadding(RECT rc);
	RECT GetTextPadding() const;
    void SetFont(int index);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
    RECT GetThumbRect() const;

    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    POINT ptLastMouse;
    bool m_bDragable;
    UINT m_uButtonState;
    int m_iSepWidth;
    DWORD m_dwTextColor;
    int m_iFont;
    UINT m_uTextStyle;
	RECT m_rcTextPadding;

#pragma warning(push)
#pragma warning(disable:4251)
    std::unique_ptr<CImage> m_sNormalImage;
    std::unique_ptr<CImage> m_sHotImage;
    std::unique_ptr<CImage> m_sPushedImage;
    std::unique_ptr<CImage> m_sFocusedImage;
    std::unique_ptr<CImage> m_sSepImage;
#pragma warning(pop)
};

/////////////////////////////////////////////////////////////////////////////////////
//

/*
FIXME: 继承于CControlUI而不是CLabelUI，应该更好
*/
class UILIB_API CListElementUI : public CLabelUI, public IListItemUI
{
public:
    CListElementUI();

    const wchar_t* GetClass() const;
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

    int GetIndex() const;
    void SetIndex(int iIndex);

    IListOwnerUI* GetOwner();
    void SetOwner(CControlUI* pOwner);
    void SetVisible(bool bVisible = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    bool Activate();

    void DoEvent(TEventUI& event);
    void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);

    void DrawItemBk(HDC hDC, const RECT& rcItem);

protected:
    int m_iIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    IListOwnerUI* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListLabelElementUI : public CListElementUI
{
public:
    CListLabelElementUI();

    const wchar_t* GetClass() const;

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void DrawItemText(HDC hDC, const RECT& rcItem);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListTextElementUI : public CListLabelElementUI
{
public:
    CListTextElementUI();
    ~CListTextElementUI();

    const wchar_t* GetClass() const;
    UINT GetControlFlags() const;

    const wchar_t* GetText(int iIndex) const;
    void SetText(int iIndex, const wchar_t* pstrText);

    void SetOwner(CControlUI* pOwner);
    std::wstring* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);

    void DrawItemText(HDC hDC, const RECT& rcItem);

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
	RECT m_rcLinks[MAX_LINK];
#pragma warning(suppress:4251)
    std::wstring m_sLinks[MAX_LINK];
    int m_nHoverLink;
	IListUI* m_pOwner;
#pragma warning(suppress:4251)
    std::vector<std::wstring> m_aTexts;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListContainerElementUI : public CContainerUI, public IListItemUI
{
public:
    CListContainerElementUI();

    const wchar_t* GetClass() const;
    UINT GetControlFlags() const;

    int GetIndex() const;
    void SetIndex(int iIndex);

    IListOwnerUI* GetOwner();
    void SetOwner(CControlUI* pOwner);
    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    bool Activate();

    void DoEvent(TEventUI& event);
    void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
    void DoPaint(HDC hDC, const RECT& rcPaint);

    void DrawItemText(HDC hDC, const RECT& rcItem);    
    void DrawItemBk(HDC hDC, const RECT& rcItem);

protected:
    int m_iIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    IListOwnerUI* m_pOwner;
};

} // namespace DuiLib

#endif // __UILIST_H__
