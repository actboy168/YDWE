local simplify = require 'optimizer.simplify'
local converter = require 'optimizer.converter'

return function (ast, setting, messager)
    local report = {}

    local function reporter(type, msg, tip)
        if not report[type] then
            report[type] = {}
        end
        report[type][#report[type]+1] = {msg, tip}
    end
    
    simplify(ast, setting, reporter)
    return converter(ast, reporter, messager or print), report
end
