local wtg_checker = require 'w3x2lni.wtg_checker'
local wtg_reader = require 'w3x2lni.wtg_reader'
local load_triggerdata = require 'triggerdata'
local storm = require 'virtual_storm'
local ui = require 'ui-builder.init'

local function is_enable_unknownui()
    return true
end

return function (mappath)
    if not is_enable_unknownui() then
        return
    end
    local wtg = storm.load_file('war3map.wtg')
    if not wtg then
        return
    end
    local list = require 'ui'
    local _, _, state = load_triggerdata(list, true)
    if wtg_checker(wtg, state) then
        return
    end
    if not gui.yesno_message(nil, LNG.PARSE_UNKNOWN_UI) then
        return
    end
    local _, _, state = load_triggerdata(list, false)
    local _, fix = wtg_reader(wtg, state)
    local bufs = {ui.new_writer(fix)}
    local dir = fs.ydwe_path() / 'unknownui'
    fs.create_directories(dir)
    io.save(dir / 'define.txt',    bufs[1])
    io.save(dir / 'event.txt',     bufs[2])
    io.save(dir / 'condition.txt', bufs[3])
    io.save(dir / 'action.txt',    bufs[4])
    io.save(dir / 'call.txt',      bufs[5])
    sys.reboot(mappath)
end
