local sandbox = require 'sandbox'
local root = fs.ydwe_devpath() / 'plugin' / 'w3x2lni' / 'core'
return sandbox(root:string() .. '\\', io.__open, {
    ['w3xparser'] = require 'w3xparser',
    ['lni']       = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml',
})
