local ffi = require 'ffi'
ffi.cdef[[
    typedef void (*ffi_anyfunc)();
    int LoadLibraryA(const char* libname);
    int LoadLibraryW(const wchar_t* libname);
    int GetModuleHandleW(const wchar_t* libname);
    int FreeLibrary(int lib);
    ffi_anyfunc GetProcAddress(int lib, const char* name);
]]

local uni = require 'ffi.unicode'

local m = {}

function m.load_library(path)
    if type(path) ~= 'string' then
        path = path:string()
    end
	local wpath = uni.u2w(path)
	return ffi.C.LoadLibraryW(wpath)
end

function m.get_module_handle(path)
	local wpath = uni.u2w(path)
	return ffi.C.GetModuleHandleW(wpath)
end

function m.unload_library(module)
	return ffi.C.FreeLibrary(module)
end

function m.get_proc_address(lib, name, define)
    return ffi.cast(define, ffi.C.GetProcAddress(lib, name))
end

return m
