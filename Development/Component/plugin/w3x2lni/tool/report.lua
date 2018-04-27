local lang = require 'tool.lang'

local function sortpairs(t)
    local sort = {}
    for k, v in pairs(t) do
        sort[#sort+1] = {k, v}
    end
    table.sort(sort, function (a, b)
        return a[1] < b[1]
    end)
    local n = 1
    return function()
        local v = sort[n]
        if not v then
            return
        end
        n = n + 1
        return v[1], v[2]
    end
end

return function (report)
    local lines = {}
    for type, report in sortpairs(report) do
        if type ~= '' then
            
            local total = report[1][1]:match('TOTAL:(%d+)')
            local title = ('%s (%d)'):format(type:sub(2), total or #report)
            lines[#lines+1] = '=========================='
            lines[#lines+1] = title
            lines[#lines+1] = '=========================='
            for i, s in ipairs(report) do
                if total and i == 1 then
                elseif s[2] then
                    lines[#lines+1] = ('%s - %s'):format(s[1], s[2])
                else
                    lines[#lines+1] = s[1]
                end
            end
            lines[#lines+1] = ''
        end
    end
    local report = report['']
    if report then
        for _, s in ipairs(report) do
            if s[2] then
                lines[#lines+1] = ('%s - %s'):format(s[1], s[2])
            else
                lines[#lines+1] = s[1]
            end
        end
    end
    return table.concat(lines, '\n')
end
