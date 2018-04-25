require 'filesystem'
local command = require 'tool.command'
local messager = require 'tool.messager'
local maketemplate = require 'prebuilt.maketemplate'
local core = require 'backend.sandbox_core'
local lang = require 'tool.lang'
local w2l = core()
local root = fs.current_path()

w2l:set_messager(messager)

function w2l:mpq_load(filename)
    local mpq_path = root:parent_path() / 'data' / 'war3'
    return self.mpq_path:each_path(function(path)
        return io.load(mpq_path / path / filename)
    end)
end

return function ()
    local mpq = command[2]

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Melee'))
    maketemplate(w2l, mpq, 'Melee')

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Custom'))
    maketemplate(w2l, mpq, 'Custom')
end
