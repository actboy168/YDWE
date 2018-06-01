local event = require 'ev'
local lniloader = require 'w3x2lni.lniloader'
local wtgloader = require 'w3x2lni.wtgloader'

local ignore_once = nil
event.on('virtual_mpq: open map', function(mappath)
    if ignore_once == mappath then
        ignore_once = nil
        return
    end
    ignore_once = mappath
    log.info('OpenMap', mappath)
    lniloader(mappath)
    wtgloader(mappath)
end)
