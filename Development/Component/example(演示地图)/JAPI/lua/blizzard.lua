-- 简易的blizzard.lua，若需要完整的blizzard.lua，可以使用jass2lua工具，自行转换。

local CJ = require "jass.common"
local BJ = {}

BJ.bj_MAX_PLAYER_SLOTS = 16
function BJ.TriggerRegisterAnyUnitEventBJ(trig, event)
	for i = 0, BJ.bj_MAX_PLAYER_SLOTS-1 do
		CJ.TriggerRegisterPlayerUnitEvent(trig, CJ.Player(i), event, nil)
	end
end

return BJ
