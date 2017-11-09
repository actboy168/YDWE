local ffi = require 'ffi'
ffi.cdef[[
	void* LoadLibraryW(const wchar_t* lpLibFileName);
	int FreeLibrary(void* hLibModule);
]]

local uni = require 'ffi.unicode'

function sys.load_library(path)
	local wpath = uni.u2w(path:string())
	return ffi.C.LoadLibraryW(wpath)
end

function sys.unload_library(module)
	return ffi.C.FreeLibrary(module)
end
