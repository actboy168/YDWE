local ffi = require 'ffi'
local uni = require 'ffi.unicode'
local getmodule = require 'ffi.getmodule'

ffi.cdef[[
    typedef struct __sFILE FILE;
    int _fileno(FILE* _Stream);
    int _setmode(int _FileHandle, int _Mode);
    int GetModuleHandleW(const wchar_t* libname);
]]

local _O_BINARY = 0x8000
local _O_TEXT   = 0x4000

return function (f, mode)
    local ucrt
    if getmodule('UCRTBASED.DLL') then
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
