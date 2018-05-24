return (require 'backend.sandbox')('.\\core\\', io.open, { 
    ['w3xparser'] = require 'w3xparser',
    ['lni']       = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml',
    ['lang']      = require 'share.lang',
    ['data_load'] = require 'backend.data_load'
})
