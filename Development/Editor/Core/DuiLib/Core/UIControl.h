#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);

class UILIB_API CControlUI
{
public:
    CControlUI();
    virtual ~CControlUI();

public:
    virtual std::wstring const& GetName() const;
    virtual void SetName(const wchar_t* pstrName);
    virtual const wchar_t* GetClass() const;
    virtual UINT GetControlFlags() const;

    virtual bool Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
    virtual CControlUI* GetParent() const;


    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
	void SetBkColor3(DWORD dwBackColor);
	void SetBkImage(const wchar_t* pStrImage);
    DWORD GetBorderColor() const;
    void SetBorderColor(DWORD dwBorderColor);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
    int GetBorderSize() const;
    void SetBorderSize(int nSize);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
	bool DrawImage(HDC hDC, const wchar_t* pStrImage, const wchar_t* pStrModify = NULL);
	bool DrawImage(HDC hDC, CImage const& cImage, const wchar_t* pStrModify = NULL);

    // 位置相关
    virtual const RECT& GetPos() const;
    virtual void SetPos(RECT rc);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // 设置外边距，由上层窗口绘制
    virtual SIZE GetFixedXY() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);      // 仅float为true时有效
    virtual int GetFixedWidth() const;       // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);      // 预设的参考值
    virtual int GetFixedHeight() const;      // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);     // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
    virtual void SetRelativePos(SIZE szMove,SIZE szZoom);
    virtual void SetRelativeParentSize(SIZE sz);
    virtual TRelativePosUI GetRelativePos() const;
    virtual bool IsRelativePos() const;

    // 鼠标提示
    virtual std::wstring const& GetToolTip() const;
    virtual void SetToolTip(const wchar_t* pstrText);

    // 快捷键
    virtual wchar_t GetShortcut() const;
    virtual void SetShortcut(wchar_t ch);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // 仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

	virtual bool ForeachControl(std::function<bool(CControlUI*)> Proc);
    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

    virtual void SetAttribute(const wchar_t* pstrName, const wchar_t* pstrValue);
	CControlUI* ApplyAttributeList(const wchar_t* pstrList);
	CControlUI* ApplyAttributeTable(CAttributesList* pTable);
	

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//虚拟窗口参数
	void SetVirtualWnd(const wchar_t* pstrValue);
	std::wstring const& GetVirtualWnd() const;

public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;

protected:
    CPaintManagerUI* m_pManager;
	CControlUI* m_pParent;
#pragma warning(push)
#pragma warning(disable:4251)
	std::wstring m_sName;
	std::wstring m_sToolTip;
	std::wstring m_sVirtualWnd;
	std::unique_ptr<CImage>     m_sBkImage;
#pragma warning(pop)
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
    RECT m_rcItem;
    RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    bool m_bFocused;
    bool m_bFloat;
    bool m_bSetPos; // 防止SetPos循环调用
    TRelativePosUI m_tRelativePos;
    wchar_t m_chShortcut;
    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    DWORD m_dwBorderColor;
	DWORD m_dwFocusBorderColor;
    int m_nBorderSize;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
};

} // namespace DuiLib

#endif // __UICONTROL_H__
