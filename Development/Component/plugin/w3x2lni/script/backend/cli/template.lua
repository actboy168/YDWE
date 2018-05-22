require 'filesystem'
local messager = require 'share.messager'
local maketemplate = require 'prebuilt.maketemplate'
local core = require 'backend.sandbox_core'
local lang = require 'share.lang'
local config = require 'share.config'
local w2l = core()
local root = fs.current_path()

w2l:set_messager(messager)

function w2l:mpq_load(filename)
    local mpq_path = root:parent_path() / 'data' / w2l.config.data_war3 / 'war3'
    return self.mpq_path:each_path(function(path)
        return io.load(mpq_path / path / filename)
    end)
end

function w2l:defined_load(filename)
    return io.load(root:parent_path() / 'data' / w2l.config.data_war3 / 'war3' / 'defined' / filename)
end

function w2l:wes_load(filename)
    return io.load(root:parent_path() / 'data' / w2l.config.data_wes / 'we' / filename)
end

return function ()
    local mpq = config.global.data_war3

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Melee'))
    w2l.progress:start(0.5)
    maketemplate(w2l, 'Melee')
    w2l.progress:finish()

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Custom'))
    w2l.progress:start(1.0)
    maketemplate(w2l, 'Custom')
    w2l.progress:finish()
end
