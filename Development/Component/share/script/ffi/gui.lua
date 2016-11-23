local ffi = require 'ffi'
ffi.cdef[[
	int MessageBoxW(unsigned int hWnd, const wchar_t* lpText, const wchar_t* lpCaption, unsigned int uType);
]]

local uni = require 'ffi.unicode'
local generate_id = 48886
local message_map = {}

gui.MB_ICONQUESTION = 0x00000020
gui.MB_OK = 0x00000000
gui.MB_YESNO = 0x00000004
gui.IDOK = 1
gui.MF_STRING = 0x00000000
gui.MF_POPUP = 0x00000010
gui.WM_COMMAND = 0x0111
gui.WM_SETTEXT = 0x000C

local mt = {}
mt.__index = mt
function mt:add(name, callback)
	message_map[generate_id] = callback
	gui.append_menu(self.handle, gui.MF_STRING, generate_id, name)
	generate_id = generate_id + 1
end

function gui.menu(main_menu, name)
	local handle = gui.create_menu()
	gui.append_menu(main_menu, gui.MF_STRING | gui.MF_POPUP, mem.pointer_to_number(handle), name)
	return setmetatable({handle = handle}, mt)
end

-- 当WE的窗口过程收到消息时执行
-- event_data - 事件参数，table，包含以下值
--	handle, message, wparam, lparam，具体不解释
-- 返回非负数会调用原窗口函数。返回负数则直接吃掉消息
function event.EVENT_WINDOW_MESSAGE(event_data)
	-- 只处理菜单信息
	if event_data.message == gui.WM_COMMAND then
		-- 菜单ID（低16位）
		local menu_id = (event_data.wparam & 0xFFFF)
		if message_map[menu_id] then
			message_map[menu_id]()
		end
	end
	return 0
end

local function messagebox(hwnd, text, caption, type)
	local wtext = uni.u2w(text)
	local wcaption = uni.u2w(caption)
	-- todo: remove mem.pointer_to_number
	return ffi.C.MessageBoxW(mem.pointer_to_number(hwnd), wtext, wcaption, type)
end

function gui.error_message(hwnd, fmt, ...)
    return messagebox(hwnd, fmt:format(...), _("Error") ,gui.MB_ICONQUESTION | gui.MB_OK)
end

function gui.yesno_message(hwnd, fmt, ...)
    return messagebox(hwnd, fmt:format(...), _("YDWE"), gui.MB_ICONQUESTION | gui.MB_YESNO) == gui.IDYES
end
