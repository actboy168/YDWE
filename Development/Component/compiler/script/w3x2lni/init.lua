local root = fs.ydwe_devpath() / 'plugin' / 'w3x2lni' / 'script' / 'core'
local sandbox = loadfile((fs.ydwe_devpath() / 'plugin' / 'w3x2lni' / 'script' / 'backend' / 'sandbox.lua'):string())()
return sandbox(root:string() .. '\\', io.__open, {
    ['w3xparser'] = require 'w3xparser',
    ['lni']       = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml',
    ['data_load'] = require 'compiler.w3x2lni.data_load',
})
