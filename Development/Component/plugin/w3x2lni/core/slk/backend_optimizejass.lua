local parser    = require 'parser.init'
local optimizer = require 'optimizer.init'

local function create_report(w2l, report, title, type, max)
    local msgs = report[type]
    if not msgs then
        return
    end
    local fix = 0
    if #msgs > max then
        fix = math.random(0, #msgs - max)
    end
    if title then
        w2l.message('-report|8脚本优化', ('%d.%s    总计：%d'):format(title, type, #msgs))
    end
    for i = 1, max do
        local msg = msgs[i+fix]
        if msg then
            w2l.message('-report|8脚本优化', msg[1])
            w2l.message('-tip', msg[2])
        end
    end
end

return function (w2l)
    local common   = w2l:map_load 'common.j'   or w2l:map_load 'scripts\\common.j'   or w2l:mpq_load 'scripts\\common.j'
    local blizzard = w2l:map_load 'blizzard.j' or w2l:map_load 'scripts\\blizzard.j' or w2l:mpq_load 'scripts\\blizzard.j'
    local war3map  = w2l:map_load 'war3map.j'  or w2l:map_load 'scripts\\war3map.j'
    local ast
    ast = parser(common,   'common.j',   ast)
    ast = parser(blizzard, 'blizzard.j', ast)
    ast = parser(war3map,  'war3map.j',  ast)
    
    local buf, report = optimizer(ast, w2l.config)

    if w2l:map_load 'war3map.j' then
        w2l:map_save('war3map.j', buf)
    else
        w2l:map_save('scripts\\war3map.j', buf)
    end

    create_report(w2l, report, 1,   '混淆脚本',        10)
    create_report(w2l, report, 2,   '引用函数',        5)
    create_report(w2l, report, 3,   '未引用的全局变量', 20)
    create_report(w2l, report, 4,   '未引用的函数',     20)
    create_report(w2l, report, 5,   '未引用的局部变量', 20)
end
