local mpqloader = require 'mpqloader'
local w3xparser = require 'w3xparser'
local wes = w3xparser.ini(mpqloader:load("ui/worldeditstrings.txt"))
return wes['WorldEditStrings']
