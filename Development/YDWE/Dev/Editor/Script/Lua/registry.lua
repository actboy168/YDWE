local ffi = require("ffi")

ffi.cdef[[
	enum REGISTRY_PREDEFINED_KEYS {
	    HKEY_CURRENT_USER  = 0x80000001, 
	    HKEY_LOCAL_MACHINE = 0x80000002,
	};
	
	enum REGISTRY_VALUE_TYPE {
	    REG_DWORD = 4, 
	};
	
	enum REGISTRY_REGSAM {
	    KEY_WRITE = 0x20006, 
	    KEY_READ  = 0x20019,
	};
   
   uint32_t RegOpenKeyExA(uint32_t base_key, const char* sub_key, uint32_t options, enum REGSAM desired, uint32_t* result_key);
   uint32_t RegCreateKeyExA(uint32_t base_key, const char* sub_key, uint32_t reserved, const char* class_name, uint32_t options, enum REGSAM desired, void* security_attributes, uint32_t* result_key, uint32_t* disposition);
   uint32_t RegQueryValueExA(uint32_t key, const char* value_name, const uint32_t* reserved, enum REGISTRY_VALUE_TYPE* type, uint8_t* data_buffer, uint32_t* data_length);
   uint32_t RegSetValueExA(uint32_t key, const char* value_name, uint32_t reserved, enum REGISTRY_VALUE_TYPE type, const uint8_t* data_buffer, uint32_t data_length);    
   uint32_t RegFlushKey(uint32_t key);
   uint32_t RegCloseKey(uint32_t key);
]]

registry = {}

registry.lib = ffi.load("advapi32")

registry.HKEY_CURRENT_USER  = ffi.C.HKEY_CURRENT_USER
registry.HKEY_LOCAL_MACHINE = ffi.C.HKEY_LOCAL_MACHINE

function registry.LoadInt(base_key, sub_key, value_name)
	local result_key = ffi.new("uint32_t[1]", 0)
	local error_code = registry.lib.RegOpenKeyExA(base_key, sub_key, 0, ffi.C.KEY_READ, result_key)
	if 0 ~= error_code then
		return error_code, 0
	end

	local data_buffer = ffi.new("uint32_t[1]", 0)
	local data_length = ffi.new("uint32_t[1]", 4)
	error_code = registry.lib.RegQueryValueExA(result_key[0], value_name, nil, nil, ffi.cast('uint8_t*', data_buffer), data_length)
	registry.lib.RegCloseKey(result_key[0])	
	return error_code, data_buffer[0]
end

function registry.SaveInt(base_key, sub_key, value_name, data)
	local result_key  = ffi.new("uint32_t[1]", 0)
	local error_code = registry.lib.RegCreateKeyExA(base_key, sub_key, 0, nil, 0, ffi.C.KEY_WRITE, nil, result_key, nil)
	if 0 ~= error_code then
		return error_code
	end
	
	local data_buffer = ffi.new("uint32_t[1]", data)
	error_code = registry.lib.RegSetValueExA(result_key[0], value_name, 0, ffi.C.REG_DWORD, ffi.cast('uint8_t*', data_buffer), 4)

	if 0 == error_code then
		registry.lib.RegFlushKey(result_key[0])	
	end
	registry.lib.RegCloseKey(result_key[0])	
	return error_code
end
