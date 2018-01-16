local mpqloader = require 'mpqloader'
local w3xparser = require 'w3xparser'
local wes = w3xparser.ini(mpqloader:load(fs.ydwe_path() / "share" / "mpq", "ui/worldeditstrings.txt"))
local WESTRING_MODIFYTEXTURES = wes['WorldEditStrings']['WESTRING_MODIFYTEXTURES']
local tiledialog = 0

function event.EVENT_DIALOG_MESSAGE(event_data)
	if event_data.message == gui.WM_SETTEXT then
		if event_data.lparam  == WESTRING_MODIFYTEXTURES then
			tiledialog = event_data.handle
		end
	elseif event_data.message == gui.WM_COMMAND then
		if event_data.handle == tiledialog then
			-- Delete style
			gui.enable_dialog_item(event_data.handle, 0x10, true)
			-- Add style
			gui.enable_dialog_item(event_data.handle, 0x11, true)
			-- Replace style
			gui.enable_dialog_item(event_data.handle, 0x12, true)
		end
	end
	return 0
end
