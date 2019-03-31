require 'filesystem'
local messager = require 'share.messager'
local maketemplate = require 'prebuilt.maketemplate'
local core = require 'backend.sandbox_core'
local lang = require 'share.lang'
local config = require 'share.config'
local w2l = core()

w2l:set_messager(messager)

return function ()
    local mpq = config.global.data

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Melee'))
    w2l.progress:start(0.5)
    maketemplate(w2l, 'Melee')
    w2l.progress:finish()

    w2l.messager.text(lang.report.CREATE_TEMPLATE:format(mpq, 'Custom'))
    w2l.progress:start(1.0)
    maketemplate(w2l, 'Custom')
    w2l.progress:finish()
end
