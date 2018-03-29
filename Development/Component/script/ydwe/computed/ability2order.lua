local order_list = require 'computed.order_list'
local order_id = require 'computed.order_id'

return function (skill, type)
    if skill:sub(1,1) == "'" and skill:sub(-1,-1) == "'" then
        skill = skill:sub(2, -2)
    else
        skill = tonumber(skill)
        if skill == nil then
            return 0
        end
    end
	local ability = require 'slk'.ability
    if not ability[skill] then
        return 0
    end
    local code = ability[skill].code
    if code == 'ANcl' then
        local order = ability[skill].DataF1
        if not order then
            return 0
        end
        return order_id[order] or 0
    end
    if not order_list[type] then
        return 0
    end
    return order_list[type][code] or 0
end
