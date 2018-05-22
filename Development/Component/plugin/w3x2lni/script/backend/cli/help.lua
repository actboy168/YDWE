local messager = require 'share.messager'
local lang = require 'share.lang'

local list = {
    mpq = lang.raw.HELP_MPQ .. '\r\n\r\n' .. lang.raw.HELP_MPQ_DESC,
    lni = lang.raw.HELP_LNI .. '\r\n\r\n' .. lang.raw.HELP_LNI_DESC,
    slk = lang.raw.HELP_SLK .. '\r\n\r\n' .. lang.raw.HELP_SLK_DESC,
    obj = lang.raw.HELP_OBJ .. '\r\n\r\n' .. lang.raw.HELP_OBJ_DESC,
    config = lang.raw.HELP_CONFIG .. '\r\n\r\n' .. lang.raw.HELP_CONFIG_DESC,
    version = lang.raw.HELP_VERSION,
    log = lang.raw.HELP_LOG,
}

if arg[2] and list[arg[2]] then
    messager.raw(list[arg[2]])
    return
end

local cmd = [[
    mpq       {HELP_MPQ}
    lni       {HELP_LNI}
    obj       {HELP_OBJ}
    slk       {HELP_SLK}

    log       {HELP_LOG}
    config    {HELP_CONFIG}

    help      {HELP_HELP}
    version   {HELP_VERSION}
]]
cmd = cmd:gsub('%{(.-)%}', function (str)
    return lang.raw[str]
end)

messager.raw((lang.raw.HELP):format(cmd))
