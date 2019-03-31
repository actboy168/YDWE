local ORDER = require 'defined.order_id'
local lang = require 'lang'

local w2l, slk

local function check_skill_order(skill, order_list, tip)
    for _, order in ipairs(order_list) do
        if order == nil or order == '' then
            return
        end
        if not ORDER[order] then
            w2l.messager.report(lang.report.WARN, 2, tip:format(skill._id, skill.name), lang.report.ERROR_ORDER:format(order))
        end
    end
end

local function check_order_id()
    -- 只需要检查通魔的DataF与魔法书的DataE，因为技能的Order是无法修改的
    for id, skill in pairs(slk.ability) do
        if skill._code == 'ANcl' then
            check_skill_order(skill, skill.dataf, lang.report.ERROR_ORDER_IN_ANcl)
        elseif skill._code == 'Aspb' then
            check_skill_order(skill, skill.datae, lang.report.ERROR_ORDER_IN_Aspb)
        end
    end
end

return function (w2l_, slk_)
    w2l = w2l_
    slk = slk_

    check_order_id()
end
