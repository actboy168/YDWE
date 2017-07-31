local parser    = require 'parser.init'
local optimizer = require 'optimizer.init'

local function create_report(report, title, type, max)
    local msgs = report[type]
    if not msgs then
        return
    end
    local fix = 0
    if #msgs > max then
        fix = math.random(0, #msgs - max)
    end
    if title then
        message('-report|8脚本优化', ('%d.%s    总计：%d'):format(title, type, #msgs))
    end
    for i = 1, max do
        local msg = msgs[i+fix]
        if msg then
            message('-report|8脚本优化', msg[1])
            message('-tip', msg[2])
        end
    end
end

return function (w2l, archive)
    local common   = archive:get 'common.j'   or archive:get 'scripts\\common.j'   or io.load(w2l.mpq / 'scripts' / 'common.j')
    local blizzard = archive:get 'blizzard.j' or archive:get 'scripts\\blizzard.j' or io.load(w2l.mpq / 'scripts' / 'blizzard.j')
    local war3map  = archive:get 'war3map.j'  or archive:get 'scripts\\war3map.j'
    local ast
    ast = parser(common,   'common.j',   ast)
    ast = parser(blizzard, 'blizzard.j', ast)
    ast = parser(war3map,  'war3map.j',  ast)
    
    local buf, report = optimizer(ast, w2l.config)

    if archive:get 'war3map.j' then
        archive:set('war3map.j', buf)
    else
        archive:set('scripts\\war3map.j', buf)
    end

    create_report(report, nil, '脚本混淆失败',     1)
    create_report(report, nil, '没有混淆函数名',   1)
    create_report(report, nil, '强制引用全部函数', 1)
    create_report(report, 1,   '引用函数',        5)
    create_report(report, 2,   '未引用的全局变量', 20)
    create_report(report, 3,   '未引用的函数',     20)
    create_report(report, 4,   '未引用的局部变量', 20)
end
