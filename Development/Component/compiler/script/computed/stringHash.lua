local stormStringHash = (require 'ffi.storm').string_hash
return function (str)
	return ('0x%08X'):format(stormStringHash(str))
end
