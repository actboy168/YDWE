log = require 'log'
local fs = require 'bee.filesystem'
local ydwe = fs.dll_path():parent_path():parent_path()
log.init(ydwe / "logs", "lock_mouse")

local ffi = require 'ffi'
ffi.cdef[[
    void __stdcall Sleep(unsigned long dwMilliseconds);

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
    int __stdcall GetParent(int hWnd);
    int __stdcall GetForegroundWindow();
    int __stdcall GetCursorPos(struct POINT* lpPoint);
    int __stdcall GetClientRect(int hWnd, struct RECT* lpRect);
    int __stdcall ClientToScreen(int hWnd, struct POINT* lpPoint);
    int __stdcall ClipCursor(const struct RECT* lpRect);
]]

local thread = require 'bee.thread'
local channel = thread.channel 'window'
local war3window = channel:bpop(window)
local enable = false

local function IsForegroundWindow()
    local window = ffi.C.GetParent(war3window)
    if window == 0 then
        return ffi.C.GetForegroundWindow() == war3window
    else
        return ffi.C.GetForegroundWindow() == window
    end
end

local function PtInRect(rect, pt)
    return (pt.x >= rect.left) and (pt.x < rect.right) and (pt.y >= rect.top) and (pt.y < rect.bottom)
end

local function lock_mouse()
    if IsForegroundWindow() then
        local rect = ffi.new('struct RECT')
        local pt = ffi.new('struct POINT')
        ffi.C.GetClientRect(war3window, rect)

        pt.x = rect.left
        pt.y = rect.top
        ffi.C.ClientToScreen(war3window, pt)
        rect.left = pt.x
        rect.top = pt.y
        
        pt.x = rect.right
        pt.y = rect.bottom
        ffi.C.ClientToScreen(war3window, pt)
        rect.right = pt.x
        rect.bottom = pt.y

        ffi.C.GetCursorPos(pt)
        if PtInRect(rect, pt) then
            ffi.C.ClipCursor(rect)
            enable = true
        end
    elseif enable then
        ffi.C.ClipCursor(nil)
        enable = false
    end
end

while true do
    local ok, msg = channel:pop()
    if ok and msg == 'exit' then
        return
    end
    lock_mouse()
    thread.sleep(0.01)
end
