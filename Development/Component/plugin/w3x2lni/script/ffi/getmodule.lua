local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef[[
    int GetModuleHandleW(const wchar_t* libname);
]]

return function(name)
    local wpath = uni.u2w(name)
    return ffi.C.GetModuleHandleW(wpath)
end
