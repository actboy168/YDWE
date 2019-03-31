local ffi = require 'ffi'
local uni = require 'ffi.unicode'
local pe = require 'ffi.pe'
local getmodule = require 'ffi.getmodule'

ffi.cdef[[
    typedef struct __sFILE FILE;
    int _fileno(FILE* _Stream);
    int _setmode(int _FileHandle, int _Mode);
]]

local _O_BINARY = 0x8000
local _O_TEXT   = 0x4000

local function hasmodule(m, name)
    local obj = pe.parse_memory(m)
    if not obj then
        return false
    end
    for _, dll in ipairs(obj.DataDirectory.ImportTable) do
        if dll.Name:upper() == name:upper() then
            return true
        end
    end
    return false
end

return function (f, mode)
    local ucrt
    if hasmodule('lua54.dll', 'UCRTBASED.DLL') then
        ucrt = ffi.load('UCRTBASED.DLL')
    else
        ucrt = ffi.load('API-MS-WIN-CRT-STDIO-L1-1-0.DLL')
    end
    if mode == 'b' then
        ucrt._setmode(ucrt._fileno(f), _O_BINARY)
    else
        ucrt._setmode(ucrt._fileno(f), _O_TEXT)
    end
end
