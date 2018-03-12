local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef[[
	int MessageBoxW(unsigned int hWnd, const wchar_t* lpText, const wchar_t* lpCaption, unsigned int uType);
]]

local MB_ICONQUESTION = 0x00000020
local MB_OK = 0x00000000

local function messagebox(hwnd, text, caption, type)
	local wtext = uni.u2w(text)
	local wcaption = uni.u2w(caption)
	return ffi.C.MessageBoxW(hwnd, wtext, wcaption, type)
end

return function(caption, fmt, ...)
    return messagebox(0, fmt:format(...), caption, MB_ICONQUESTION | MB_OK)
end
