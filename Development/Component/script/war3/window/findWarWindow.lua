local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef[[
    int __stdcall GetCurrentProcessId();
    int __stdcall FindWindowExW(int hWndParent, int hWndChildAfter, const wchar_t* lpszClass, const wchar_t* lpszWindow);
    int __stdcall GetWindowThreadProcessId(int hWnd, int* lpdwProcessId);
]]

local GetCurrentProcessId = ffi.C.GetCurrentProcessId
local FindWindowExW = ffi.C.FindWindowExW
local GetWindowThreadProcessId = ffi.C.GetWindowThreadProcessId

return function()
    local current_pid = GetCurrentProcessId()
    local war3name = uni.u2w('Warcraft III')
    local pid = ffi.new('int[1]', 0)
    local hwnd = 0
    while true do
        hwnd = FindWindowExW(0, hwnd, war3name, war3name)
        if hwnd == 0 then
            return
        end
        GetWindowThreadProcessId(hwnd, pid)
        if pid[0] == current_pid then
            return hwnd
        end
    end
end
