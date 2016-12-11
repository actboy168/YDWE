#pragma once
//////////////BEGIN消息映射宏定义////////////////////////////////////////////////////
///

namespace DuiLib
{

enum DuiSig
{
	DuiSig_end = 0, // [marks end of message map]
	DuiSig_lwl,     // LRESULT (WPARAM, LPARAM)
	DuiSig_vn,      // void (TNotifyUI)
};

class CControlUI;

// Structure for notifications to the outside world
typedef struct tagTNotifyUI 
{
	std::wstring sType;
	std::wstring sVirtualWnd;
	CControlUI* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	WPARAM wParam;
	LPARAM lParam;
} TNotifyUI;

class CNotifyPump;
typedef void (CNotifyPump::*DUI_PMSG)(TNotifyUI& msg);  //指针类型

union DuiMessageMapFunctions
{
	DUI_PMSG pfn;   // generic member function pointer
	LRESULT (CNotifyPump::*pfn_Notify_lwl)(WPARAM, LPARAM);
	void (CNotifyPump::*pfn_Notify_vn)(TNotifyUI&);
};

//定义所有消息类型
//////////////////////////////////////////////////////////////////////////

#define DUI_MSGTYPE_MENU                   (L"menu")
#define DUI_MSGTYPE_LINK                   (L"link")

#define DUI_MSGTYPE_TIMER                  (L"timer")
#define DUI_MSGTYPE_CLICK                  (L"click")

#define DUI_MSGTYPE_RETURN                 (L"return")
#define DUI_MSGTYPE_SCROLL                 (L"scroll")

#define DUI_MSGTYPE_DROPDOWN               (L"dropdown")
#define DUI_MSGTYPE_SETFOCUS               (L"setfocus")

#define DUI_MSGTYPE_KILLFOCUS              (L"killfocus")
#define DUI_MSGTYPE_ITEMCLICK 		   	   (L"itemclick")
#define DUI_MSGTYPE_TABSELECT              (L"tabselect")

#define DUI_MSGTYPE_ITEMSELECT 		   	   (L"itemselect")
#define DUI_MSGTYPE_ITEMEXPAND             (L"itemexpand")
#define DUI_MSGTYPE_WINDOWINIT             (L"windowinit")
#define DUI_MSGTYPE_BUTTONDOWN 		   	   (L"buttondown")
#define DUI_MSGTYPE_MOUSEENTER			   (L"mouseenter")
#define DUI_MSGTYPE_MOUSELEAVE			   (L"mouseleave")

#define DUI_MSGTYPE_TEXTCHANGED            (L"textchanged")
#define DUI_MSGTYPE_HEADERCLICK            (L"headerclick")
#define DUI_MSGTYPE_ITEMDBCLICK            (L"itemdbclick")
#define DUI_MSGTYPE_SHOWACTIVEX            (L"showactivex")

#define DUI_MSGTYPE_ITEMCOLLAPSE           (L"itemcollapse")
#define DUI_MSGTYPE_ITEMACTIVATE           (L"itemactivate")
#define DUI_MSGTYPE_VALUECHANGED           (L"valuechanged")

#define DUI_MSGTYPE_SELECTCHANGED 		   (L"selectchanged")


//////////////////////////////////////////////////////////////////////////



struct DUI_MSGMAP_ENTRY;
struct DUI_MSGMAP
{
#ifndef UILIB_STATIC
	const DUI_MSGMAP* (PASCAL* pfnGetBaseMap)();
#else
	const DUI_MSGMAP* pBaseMap;
#endif
	const DUI_MSGMAP_ENTRY* lpEntries;
};

//结构定义
struct DUI_MSGMAP_ENTRY //定义一个结构体，来存放消息信息
{
	std::wstring sMsgType;        // DUI消息类型
	std::wstring sCtrlName;       // 控件名称
	UINT       nSig;              // 标记函数指针类型
	DUI_PMSG   pfn;               // 指向函数的指针
};

//定义
#ifndef UILIB_STATIC
#define DUI_DECLARE_MESSAGE_MAP()                                         \
private:                                                                  \
	static const DUI_MSGMAP_ENTRY _messageEntries[];                      \
protected:                                                                \
	static const DUI_MSGMAP messageMap;                                   \
	static const DUI_MSGMAP* PASCAL _GetBaseMessageMap();                 \
	virtual const DUI_MSGMAP* GetMessageMap() const;                      \

#else
#define DUI_DECLARE_MESSAGE_MAP()                                         \
private:                                                                  \
	static const DUI_MSGMAP_ENTRY _messageEntries[];                      \
protected:                                                                \
	static  const DUI_MSGMAP messageMap;				                  \
	virtual const DUI_MSGMAP* GetMessageMap() const;                      \

#endif


//基类声明开始
#ifndef UILIB_STATIC
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                              \
	const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()               \
		{ return NULL; }                                                  \
	const DUI_MSGMAP* theClass::GetMessageMap() const                     \
		{ return &theClass::messageMap; }                                 \
	UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
		{  &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };\
	UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
	{                                                                     \

#else
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                              \
	const DUI_MSGMAP* theClass::GetMessageMap() const                     \
		{ return &theClass::messageMap; }                                 \
	UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
		{  NULL, &theClass::_messageEntries[0] };                         \
	UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
	{                                                                     \

#endif


//子类声明开始
#ifndef UILIB_STATIC
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
	const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()               \
		{ return &baseClass::messageMap; }                                \
	const DUI_MSGMAP* theClass::GetMessageMap() const                     \
		{ return &theClass::messageMap; }                                 \
	UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
		{ &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
	UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
	{                                                                     \

#else
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
	const DUI_MSGMAP* theClass::GetMessageMap() const                     \
		{ return &theClass::messageMap; }                                 \
	UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
		{ &baseClass::messageMap, &theClass::_messageEntries[0] };        \
	UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =     \
	{                                                                     \

#endif


//声明结束
#define DUI_END_MESSAGE_MAP()                                             \
	{ L"", L"", DuiSig_end, (DUI_PMSG)0 }                           \
};                                                                        \


//定义消息类型--执行函数宏
#define DUI_ON_MSGTYPE(msgtype, memberFxn)                                \
	{ msgtype, L"", DuiSig_vn, (DUI_PMSG)&memberFxn},                  \


//定义消息类型--控件名称--执行函数宏
#define DUI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                 \
	{ msgtype, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },                \


//定义click消息的控件名称--执行函数宏
#define DUI_ON_CLICK_CTRNAME(ctrname,memberFxn)                           \
	{ DUI_MSGTYPE_CLICK, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },      \


//定义selectchanged消息的控件名称--执行函数宏
#define DUI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                   \
    { DUI_MSGTYPE_SELECTCHANGED,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn }, \


//定义killfocus消息的控件名称--执行函数宏
#define DUI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                       \
	{ DUI_MSGTYPE_KILLFOCUS,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },     \


//定义menu消息的控件名称--执行函数宏
#define DUI_ON_MENU_CTRNAME(ctrname,memberFxn)                            \
	{ DUI_MSGTYPE_MENU,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },          \


//定义与控件名称无关的消息宏

  //定义timer消息--执行函数宏
#define DUI_ON_TIMER()                                                    \
	{ DUI_MSGTYPE_TIMER, L"", DuiSig_vn,(DUI_PMSG)&OnTimer },          \


///
//////////////END消息映射宏定义////////////////////////////////////////////////////


//////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
///

#define  DUI_CTR_EDIT                            (L"Edit")
#define  DUI_CTR_LIST                            (L"List")
#define  DUI_CTR_TEXT                            (L"Text")

#define  DUI_CTR_COMBO                           (L"Combo")
#define  DUI_CTR_LABEL                           (L"Label")

#define  DUI_CTR_BUTTON                          (L"Button")
#define  DUI_CTR_SLIDER                          (L"Slider")

#define  DUI_CTR_CONTROL                         (L"Control")
#define  DUI_CTR_ACTIVEX                         (L"ActiveX")
#define  DUI_CTR_CHECKBOX                        (L"CheckBox")

#define  DUI_CTR_LISTITEM                        (L"ListItem")
#define  DUI_CTR_PROGRESS                        (L"Progress")
#define  DUI_CTR_RICHEDIT                        (L"RichEdit")
#define  DUI_CTR_COMBOBOX                        (L"ComboBox")
#define  DUI_CTR_DATETIME                        (L"DateTime")
#define  DUI_CTR_TREEVIEW                        (L"TreeView")
#define  DUI_CTR_TREENODE                        (L"TreeNode")

#define  DUI_CTR_CONTAINER                       (L"Container")
#define  DUI_CTR_TABLAYOUT                       (L"TabLayout")
#define  DUI_CTR_SCROLLBAR                       (L"ScrollBar")

#define  DUI_CTR_LISTHEADER                      (L"ListHeader")
#define  DUI_CTR_TILELAYOUT                      (L"TileLayout")
#define  DUI_CTR_WEBBROWSER                      (L"WebBrowser")

#define  DUI_CTR_CHILDLAYOUT                     (L"ChildLayout")
#define  DUI_CTR_LISTELEMENT                     (L"ListElement")
#define  DUI_CTR_RADIOBUTTON                     (L"RadioButton")

#define  DUI_CTR_DIALOGLAYOUT                    (L"DialogLayout")

#define  DUI_CTR_VERTICALLAYOUT                  (L"VerticalLayout")
#define  DUI_CTR_LISTHEADERITEM                  (L"ListHeaderItem")

#define  DUI_CTR_LISTTEXTELEMENT                 (L"ListTextElement")

#define  DUI_CTR_HORIZONTALLAYOUT                (L"HorizontalLayout")
#define  DUI_CTR_LISTLABELELEMENT                (L"ListLabelElement")

#define  DUI_CTR_LISTCONTAINERELEMENT            (L"ListContainerElement")

#define DUI_USE_COMBOBOX
//#define DUI_USE_DATETIME
//#define DUI_USE_EDIT
//#define DUI_USE_PROGRESS
//#define DUI_USE_RICHEDIT
//#define DUI_USE_SLIDER
//#define DUI_USE_TEXT

#ifdef DUI_USE_SLIDER
#	ifndef DUI_USE_PROGRESS
#		define DUI_USE_PROGRESS
#	endif
#endif

///
//////////////END控件名称宏定义//////////////////////////////////////////////////


}// namespace DuiLib

