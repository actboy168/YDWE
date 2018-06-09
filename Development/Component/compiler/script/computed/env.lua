return setmetatable({
	import = require 'compiler.computed.import', 
	StringHash = require 'compiler.computed.stringHash',
	abilityOrder = require 'compiler.computed.abilityOrder',
}, {__index = _G})
