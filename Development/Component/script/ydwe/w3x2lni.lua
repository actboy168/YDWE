local root = fs.ydwe_devpath() / 'plugin' / 'w3x2lni' / 'script' / 'core'
local sandbox = loadfile((root / 'backend' / 'sandbox.lua'):string())()
return sandbox(root:string() .. '\\', io.__open, {
    ['w3xparser'] = require 'w3xparser',
    ['lni']       = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml',
})
