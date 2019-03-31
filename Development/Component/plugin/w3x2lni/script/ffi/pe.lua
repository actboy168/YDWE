local parse = require 'ffi.pe-parse'

local file = {}
file.__index = file

function file:seek(n)
    self.h:seek('set', n)
end

function file:read(n)
    return self.h:read(n)
end

function file:close()
    return self.h:close()
end

local function parse_file(filename)
    local f, err = io.open(filename, 'rb')
    if not f then
        return nil, err
    end
    local f = setmetatable({
        h = f,
        type = 'file',
    }, file)
    return parse(f)
end

local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef[[
    int   __stdcall GetModuleHandleW(const wchar_t* libname);
    int   __stdcall ReadProcessMemory(void* hProcess, const void* lpBaseAddress, void* lpBuffer, unsigned int nSize, unsigned int* lpNumberOfBytesRead);
    void* __stdcall GetCurrentProcess();
]]

local memory = {}
memory.__index = memory

function memory:seek(n)
    self.n = self.h + n
end

function memory:read(n)
    local tmp = ffi.new('char[?]', n)
    local rn = ffi.new('unsigned int[1]')
    if 0 == ffi.C.ReadProcessMemory(ffi.C.GetCurrentProcess(), ffi.cast('const void*', self.n), ffi.cast('void*', tmp), n, rn) or rn[0] ~= n then
        error('Invalid data.')
    end
    self.n = self.n + n
    return ffi.string(tmp, n)
end

function memory:close()
end

local function parse_memory(modulename)
    local wpath = uni.u2w(modulename)
    local h = ffi.C.GetModuleHandleW(wpath)
    if not h then
        return nil, 'Open PE failed.'
    end
    local f = setmetatable({
        h = h,
        n = h,
        type = 'memory',
    }, memory)
    return parse(f)
end

return {
    parse_file = parse_file,
    parse_memory = parse_memory,
}
