local parser    = require 'parser.init'
local optimizer = require 'optimizer.init'
local lang = require 'lang'

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
        w2l.messager.report(lang.report.OPTIMIZE_JASS, 8, lang.report.OPTIMIZE_JASS_RESULT:format(title, type, #msgs))
    end
    for i = 1, max do
        local msg = msgs[i+fix]
        if msg then
            w2l.messager.report(lang.report.OPTIMIZE_JASS, 8, msg[1], msg[2])
        end
    end
end

return function (w2l)
    local common   = w2l:file_load('map', 'common.j')   or w2l:file_load('map', 'scripts\\common.j')   or w2l:mpq_load('scripts\\common.j')
    local blizzard = w2l:file_load('map', 'blizzard.j') or w2l:file_load('map', 'scripts\\blizzard.j') or w2l:mpq_load('scripts\\blizzard.j')
    local war3map  = w2l:file_load('map', 'war3map.j')  or w2l:file_load('map', 'scripts\\war3map.j')
    if not war3map then
        w2l.messager.report(lang.report.ERROR, 1, lang.report.NO_JASS)
        return
    end
    local function messager(...)
        local t = table.pack(...)
        for i = 1, t.n do
            t[i] = tostring(t[i])
        end
        w2l.messager.report(lang.report.OPTIMIZE_JASS, 8, table.concat(t, '\t'))
    end
    local buf, report
    local suc, err = pcall(function ()
        local ast
        ast = parser(common,   'common.j',   ast, messager)
        ast = parser(blizzard, 'blizzard.j', ast, messager)
        ast = parser(war3map,  'war3map.j',  ast, messager)
        buf, report = optimizer(ast, w2l.config, messager)
    end)
    if not suc then
        w2l.messager.report(lang.report.ERROR, 1, lang.report.SYNTAX_ERROR, err:match '[\r\n]+(.+)$')
        return
    end

    if w2l:file_load('map', 'war3map.j') then
        w2l:file_save('map', 'war3map.j', buf)
    else
        w2l:file_save('map', 'scripts\\war3map.j', buf)
    end

    create_report(w2l, report, 1, lang.report.CONFUSE_JASS,         10)
    create_report(w2l, report, 2, lang.report.REFERENCE_FUNCTION,   5)
    create_report(w2l, report, 3, lang.report.UNREFERENCE_GLOBAL,   20)
    create_report(w2l, report, 4, lang.report.UNREFERENCE_FUNCTION, 20)
    create_report(w2l, report, 5, lang.report.UNREFERENCE_LOCAL,    20)
end
