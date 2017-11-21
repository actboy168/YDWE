local ffi = require 'ffi'
local uni = require 'ffi.unicode'

ffi.cdef[[
    const wchar_t* __stdcall GetCommandLineW();
    wchar_t** __stdcall CommandLineToArgvW(const wchar_t* lpCmdLine, int* pNumArgs);
    int __stdcall LocalFree(wchar_t** hMem);
]]

local shell32 = ffi.load 'shell32'
local GetCommandLineW = ffi.C.GetCommandLineW
local CommandLineToArgvW = shell32.CommandLineToArgvW
local LocalFree = ffi.C.LocalFree

local function getCommandLine()
    local pargc = ffi.new('int[1]', 0)
    local pargv = CommandLineToArgvW(GetCommandLineW(), pargc)
    local args = {}
    for i = 0, pargc[0] - 1 do
        args[#args+1] = uni.w2u(pargv[i])
    end
    LocalFree(pargv)
    return args
end

local function convert(args)
    local res = {}
    local key, value
    for _, arg in ipairs(args) do
        if arg:sub(1, 1) == '-' then
            if key then
                res[key] = value or true
            end
            key = arg:sub(2)
            value = nil
        else
            if value then
                value = value .. ' ' .. arg
            else
                value = arg
            end
        end
    end
    if key then
        res[key] = value or true
    end
    return res
end

return function()
    return convert(getCommandLine())
end
