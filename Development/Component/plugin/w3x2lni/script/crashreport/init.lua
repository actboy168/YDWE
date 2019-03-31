local messagebox = require 'ffi.messagebox'
local lang = require 'share.lang'
local cl = require 'share.changelog'

local root = fs.absolute(fs.path '..')
local errmessage = io.stdin:read 'a'

local ok, gl = pcall(require, 'share.gitlog')
if ok then
    gl = gl.commit
else
    gl = 'unknown'
end

local log = {}
log[#log+1] = os.date('Date: %Y-%m-%d %H:%M:%S')
log[#log+1] = string.format('Version: %s', cl[1].version)
log[#log+1] = string.format('Commit: %s', gl)
log[#log+1] = 'Stack:'
log[#log+1] = '----------------------------------------'
log[#log+1] = errmessage
log[#log+1] = '----------------------------------------'

local errorlog = table.concat(log, '\n')

fs.create_directories(root / 'log' / 'error')
local logfile = root / 'log' / 'error' / (os.date('%Y-%m-%d %H-%M-%S') .. '.log')
local f = io.open(logfile:string(), 'w')
if f then
    f:write(errorlog)
    f:close()
end
messagebox(lang.ui.ERROR, '%s', errorlog)
