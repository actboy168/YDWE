local ffi = require 'ffi'
local uni = require 'ffi.unicode'
local sleep = require 'ffi.sleep'

ffi.cdef[[
    typedef int HWND;
    typedef int HINSTANCE;
    typedef int HICON;
    typedef int HCURSOR;
    typedef int HBRUSH;
    typedef int HMENU;
    typedef int WPARAM;
    typedef int LPARAM;
    typedef int (__stdcall* WNDPROC) (HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam);
    struct RECT
    {
        long left;
        long top;
        long right;
        long bottom;
    };
    struct POINT
    {
        long  x;
        long  y;
    };
    struct WINDOWPLACEMENT {
        unsigned int  length;
        unsigned int  flags;
        unsigned int  showCmd;
        POINT         ptMinPosition;
        POINT         ptMaxPosition;
        RECT          rcNormalPosition;
    };

    struct WNDCLASSEXW {
        unsigned int   cbSize;
        unsigned int   style;
        WNDPROC        lpfnWndProc;
        int            cbClsExtra;
        int            cbWndExtra;
        HINSTANCE      hInstance;
        HICON          hIcon;
        HCURSOR        hCursor;
        HBRUSH         hbrBackground;
        const wchar_t* lpszMenuName;
        const wchar_t* lpszClassName;
        HICON          hIconSm;
    };

    int  __stdcall GetWindowPlacement(HWND hWnd, struct WINDOWPLACEMENT *lpwndpl);
    long __stdcall GetWindowLongW(HWND hWnd, int nIndex);
    long __stdcall SetWindowLongW(HWND hWnd, int nIndex, long dwNewLong);
    HWND __stdcall SetParent(HWND hWndChild, HWND hWndNewParent);
    HWND __stdcall SetFocus(HWND hWnd);
    int  __stdcall SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, unsigned int uFlags);
    unsigned long __stdcall GetClassLongW(HWND hWnd, int nIndex);
    int  __stdcall RegisterClassExW(const struct WNDCLASSEXW *);
    int  __stdcall GetWindowRect(HWND hWnd, struct RECT* lpRect);
    HWND __stdcall GetDesktopWindow();
    HWND __stdcall CreateWindowExW(unsigned long dwExStyle, const wchar_t* lpClassName, const wchar_t* lpWindowName, unsigned long dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, int lpParam);
    int  __stdcall ShowWindow(HWND hWnd, int nCmdShow);
    int  __stdcall UpdateWindow(HWND hWnd);
    int  __stdcall DefWindowProcW(HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam);
    int  __stdcall PostMessageW(HWND hWnd, unsigned int Msg, WPARAM wParam,  LPARAM lParam);
    int  __stdcall SendMessageW(HWND hWnd, unsigned int Msg, WPARAM wParam, LPARAM lParam);
    int  __stdcall SetForegroundWindow(HWND hWnd);
    int  __stdcall BringWindowToTop(HWND hWnd);
    int  __stdcall GetSystemMetrics(int nIndex);
]]

local CS_VREDRAW = 0x0001
local CS_HREDRAW = 0x0002
local GCL_MENUNAME      = -8
local GCL_HBRBACKGROUND = -10
local GCL_HCURSOR       = -12
local GCL_HICON         = -14
local GCL_HMODULE       = -16
local GCL_CBWNDEXTRA    = -18
local GCL_CBCLSEXTRA    = -20
local GCL_WNDPROC       = -24
local GCL_STYLE         = -26
local GCW_ATOM          = -32
local COLOR_INACTIVECAPTIONTEXT = 19
local WS_POPUP          = 0x80000000
local WS_SYSMENU        = 0x00080000
local SW_SHOWNORMAL     = 1
local WM_DESTROY        = 0x0002
local WM_SETFOCUS       = 0x0007
local WM_CLOSE          = 0x0010
local WM_KEYDOWN        = 0x0100
local WM_KEYUP          = 0x0101
local VK_MENU           = 0x12
local VK_F10            = 0x79
local GWL_STYLE         = -16
local WS_CAPTION        = 0x00C00000 
local WS_SIZEBOX        = 0x00040000
local WS_CHILD          = 0x40000000
local WS_CLIPCHILDREN   = 0x02000000
local WS_CLIPSIBLINGS   = 0x04000000
local HWND_TOP          = 0
local SM_CXSCREEN       = 0
local SM_CYSCREEN       = 1
local SWP_SHOWWINDOW    = 0x0040
local SWP_DRAWFRAME     = 0x0020

local function createBlackWindow(window, onDestroy)
    local className = uni.u2w('Black Warcraft III')
    local windowName = uni.u2w('Warcraft III')
    local wc = ffi.new('struct WNDCLASSEXW')
    wc.cbSize = ffi.sizeof(wc)
    wc.style  = CS_HREDRAW  | CS_VREDRAW
    wc.lpfnWndProc = ffi.new('WNDPROC', function(hwnd, uMsg, wParam, lParam)
        if uMsg == WM_SETFOCUS then
            ffi.C.SetFocus(window)
            -- win7下 alt+tab切换到桌面，会导致子窗口收不到[WM_KEYUP, VK_MENU]消息
            ffi.C.PostMessageW(window, WM_KEYDOWN, VK_MENU, 1)
            ffi.C.PostMessageW(window, WM_KEYUP,   VK_MENU, 1)
            return 1
        elseif uMsg == WM_DESTROY then
            onDestroy()
        end
        return ffi.C.DefWindowProcW(hwnd, uMsg, wParam, lParam)
    end)
    wc.cbClsExtra = 0
    wc.cbWndExtra = 0
    wc.hInstance = ffi.C.GetClassLongW(window, GCL_HMODULE)
    wc.hbrBackground = COLOR_INACTIVECAPTIONTEXT
    wc.lpszMenuName = nil
    wc.lpszClassName = className
    wc.hIcon   = ffi.C.GetClassLongW(window, GCL_HICON)
    wc.hIconSm = ffi.C.GetClassLongW(window, GCL_HICONSM)
    wc.hCursor = ffi.C.GetClassLongW(window, GCL_HCURSOR)

    local desktopRect = ffi.new('struct RECT')
    ffi.C.RegisterClassExW(wc)
    ffi.C.GetWindowRect(ffi.C.GetDesktopWindow(), desktopRect)
    local blackWindow = ffi.C.CreateWindowExW(0, className, windowName, WS_POPUP | WS_SYSMENU, 0, 0, desktopRect.right, desktopRect.bottom, 0, 0, 0, 0)
    ffi.C.ShowWindow(blackWindow, SW_SHOWNORMAL)
    ffi.C.UpdateWindow(blackWindow)
    return blackWindow
end

local war3window = nil
local blackWindow = nil
local needKillWar3 = true
local windowPlacement = ffi.new('struct WINDOWPLACEMENT')
local windowStyle = 0

local function set_ratio(hwnd, x, y)
    if x/4 > y/3 then
		local fixed_x = y*4/3
		local fixed_y = y
		ffi.C.SetWindowPos(hwnd, HWND_TOP, (x-fixed_x)/2, 0, fixed_x, fixed_y, SWP_SHOWWINDOW|SWP_DRAWFRAME)
    else
		local fixed_x = x
		local fixed_y = x*3/4
		ffi.C.SetWindowPos(hwnd, HWND_TOP, 0, (y-fixed_y)/2, fixed_x, fixed_y, SWP_SHOWWINDOW|SWP_DRAWFRAME)
    end
end

local function start(window, fixedRatio)
    if blackWindow then
        return
    end
    war3window = window
    ffi.C.GetWindowPlacement(war3window, windowPlacement)
    windowStyle = ffi.C.GetWindowLongW(war3window, GWL_STYLE)
    local dwStyle = windowStyle & (~(WS_CAPTION | WS_SIZEBOX))
    if fixedRatio then
        dwStyle = dwStyle | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    end
    ffi.C.SetWindowLongW(war3window, GWL_STYLE, dwStyle)
    if fixedRatio then
        blackWindow = createBlackWindow(window, function()
            if needKillWar3 then
                ffi.C.SetForegroundWindow(window)
                ffi.C.BringWindowToTop(window)
                ffi.C.SendMessageW(window, WM_KEYDOWN, VK_F10, 0)
                ffi.C.SendMessageW(window, WM_KEYUP,   VK_F10, 0)
                sleep(500)
                ffi.C.SendMessageW(window, WM_KEYDOWN, string.byte('E'), 0)
                ffi.C.SendMessageW(window, WM_KEYUP,   string.byte('E'), 0)
                sleep(500)
                ffi.C.SendMessageW(window, WM_KEYDOWN, string.byte('Q'), 0)
                ffi.C.SendMessageW(window, WM_KEYUP,   string.byte('Q'), 0)
                sleep(1000)
                ffi.C.SendMessageW(window, WM_CLOSE,     0, 0)
                ffi.C.SendMessageW(window, WM_KEYDOWN, string.byte('X'), 0)
            end
        end)
        ffi.C.SetParent(war3window, blackWindow)
        ffi.C.SetFocus(war3window);
        set_ratio(war3window, ffi.C.GetSystemMetrics(SM_CXSCREEN), ffi.C.GetSystemMetrics(SM_CYSCREEN))
        needKillWar3 = true
    else
        ffi.C.SetWindowPos(war3window, HWND_TOP, 0, 0, ffi.C.GetSystemMetrics(SM_CXSCREEN), ffi.C.GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW|SWP_DRAWFRAME)
    end
end

local function stop()
    if not blackWindow then
        return
    end
    needKillWar3 = false
    ffi.C.SetParent(war3window, 0);
    ffi.C.SetWindowLongW(war3window, GWL_STYLE, windowStyle)
    ffi.C.SetWindowPlacement(war3window, windowPlacement)
    ffi.C.PostMessageW(blackWindow, WM_CLOSE, 0, 0)
    blackWindow = nil
end

local event = require 'ev'

local LaunchFullWindowed = '0' ~= global_config.MapTest.LaunchFullWindowed
local LaunchFixedRatioWindowed = '0' ~= global_config.MapTest.LaunchFixedRatioWindowed

if LaunchFullWindowed then
    event.on('窗口初始化', function(window)
        start(window, LaunchFixedRatioWindowed)
    end)
elseif LaunchFixedRatioWindowed then
    event.on('窗口初始化', function(window)
        local r = ffi.new('struct RECT')
        if ffi.C.GetWindowRect(window, r) ~= 0 then
            set_ratio(window, r.right - r.left, r.bottom - r.top)
        end
    end)
end
