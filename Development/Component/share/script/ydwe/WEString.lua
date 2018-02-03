local mpqloader = require 'mpqloader'
local w3xparser = require 'w3xparser'
local wes = w3xparser.ini(mpqloader:load(fs.ydwe_devpath() / "share" / "mpq", "ui/worldeditstrings.txt"))
return wes['WorldEditStrings']
