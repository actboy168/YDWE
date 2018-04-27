local command = require 'tool.command'
local messager = require 'tool.messager'
local lang = require 'tool.lang'
local config = require 'tool.config' ()
local act = command[1]

lang:set_lang(config.global.lang)

if act == 'slk' then
    local convert = require 'backend.convert'
    convert('slk')
elseif act == 'lni' then
    local convert = require 'backend.convert'
    convert('lni')
elseif act == 'obj' then
    local convert = require 'backend.convert'
    convert('obj')
elseif act == 'mpq' then
    local mpq = require 'backend.mpq'
    mpq()
elseif act == 'version' then
    local cl = require 'tool.changelog'
    messager.raw('w3x2lni version '..cl[1].version)
elseif act == 'config' then
    local cli_config = require 'backend.cli_config'
    cli_config(command)
elseif act == 'log' then
    require 'filesystem'
    require 'utility'
    local root = fs.current_path()
    local s = {}
    for l in io.lines((root:parent_path() / 'log' / 'report.log'):string()) do
        s[#s+1] = l
        if #s > 50 then
            messager.raw(table.concat(s, '\r\n') .. '\r\n')
            messager.wait()
            s = {}
        end
    end
    messager.raw(table.concat(s, '\r\n') .. '\r\n')
elseif act == 'template' then
    local template = require 'backend.template'
    template()
elseif not act or act == 'help' then
    require 'backend.help'
else
    messager.raw(lang.raw.INVALID:format(act))
end
