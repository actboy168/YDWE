require "westrings"

local WESTRING_MODIFYTEXTURES = westrings['WESTRING_MODIFYTEXTURES']
local tiledialog = 0

event.register(event.EVENT_DIALOG_MESSAGE, false, function (event_data)
	if event_data.message == gui.WM_SETTEXT then
		if event_data.lparam  == WESTRING_MODIFYTEXTURES then
			tiledialog = mem.pointer_to_number(event_data.handle)
		end
	elseif event_data.message == gui.WM_COMMAND then
		if mem.pointer_to_number(event_data.handle) == tiledialog then
			-- Delete style
			gui.enable_dialog_item(event_data.handle, 0x10, true)
			-- Add style
			gui.enable_dialog_item(event_data.handle, 0x11, true)
			-- Replace style
			gui.enable_dialog_item(event_data.handle, 0x12, true)
		end
	end
	return 0
end)
