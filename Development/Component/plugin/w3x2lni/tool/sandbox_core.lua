return (require 'tool.sandbox')('.\\core\\', io.open, { 
    ['w3xparser'] = require 'w3xparser',
    ['lni']       = require 'lni',
    ['lpeg']      = require 'lpeg',
    ['lml']       = require 'lml'
})
