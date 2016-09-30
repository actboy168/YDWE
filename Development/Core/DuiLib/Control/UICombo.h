#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();

    const wchar_t* GetClass() const;

    void DoInit();
    UINT GetControlFlags() const;

    std::wstring const& GetText() const;
    void SetEnabled(bool bEnable = true);

    std::wstring GetDropBoxAttributeList();
    void SetDropBoxAttributeList(const wchar_t* pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);

    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    bool Activate();

    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);

    TListInfoUI* GetListInfo();
    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
    
    void DoPaint(HDC hDC, const RECT& rcPaint);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

protected:
    CComboWnd* m_pWindow;

    int m_iCurSel;
	RECT m_rcTextPadding;
#pragma warning(suppress:4251)
    std::wstring m_sDropBoxAttributes;
    SIZE m_szDropBox;
    UINT m_uButtonState;

#pragma warning(push)
#pragma warning(disable:4251)
    std::unique_ptr<CImage> m_sNormalImage;
    std::unique_ptr<CImage> m_sHotImage;
    std::unique_ptr<CImage> m_sPushedImage;
    std::unique_ptr<CImage> m_sFocusedImage;
	std::unique_ptr<CImage> m_sDisabledImage;
#pragma warning(pop)

    TListInfoUI m_ListInfo;
};

} // namespace DuiLib

#endif // __UICOMBO_H__
