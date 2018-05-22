local lang = require 'share.lang'

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

return function (w2l, report, config, clock, err, warn)
    local lines = {}
    lines[#lines+1] = lang.report.INPUT_PATH .. config.input:string()
    lines[#lines+1] = lang.report.OUTPUT_PATH .. config.output:string()
    if w2l.input_mode == 'lni' then
        lines[#lines+1] = lang.report.INPUT_FORMAT .. 'Lni'
    elseif w2l.input_ar:get_type() == 'mpq' then
        lines[#lines+1] = lang.report.INPUT_FORMAT .. 'Mpq'
    elseif w2l.input_ar:get_type() == 'dir' then
        lines[#lines+1] = lang.report.INPUT_FORMAT .. 'Dir'
    end
    lines[#lines+1] = lang.report.OUTPUT_MODE .. config.mode
    lines[#lines+1] = lang.report.RESULT .. lang.script.ERROR_COUNT:format(err, warn)
    lines[#lines+1] = lang.report.TAKES_TIME:format(clock)
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
    return table.concat(lines, '\r\n')
end
