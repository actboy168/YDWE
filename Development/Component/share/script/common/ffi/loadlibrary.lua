local ffi = require 'ffi'
ffi.cdef[[
    typedef void (*ffi_anyfunc)();
	void* LoadLibraryA(const char* libname);
	void* LoadLibraryW(const wchar_t* libname);
	void* GetModuleHandleW(const wchar_t* libname);
    int   FreeLibrary(void* lib);
    ffi_anyfunc GetProcAddress(void* lib, const char* name);
]]

local uni = require 'ffi.unicode'

function sys.load_library(path)
    if type(path) ~= 'string' then
        path = path:string()
    end
	local wpath = uni.u2w(path)
	return ffi.C.LoadLibraryW(wpath)
end

function sys.get_module_handle(path)
	local wpath = uni.u2w(path)
	return ffi.C.GetModuleHandleW(wpath)
end

function sys.unload_library(module)
	return ffi.C.FreeLibrary(module)
end

function sys.get_proc_address(lib, name, define)
    return ffi.cast(define, ffi.C.GetProcAddress(lib, name))
end
