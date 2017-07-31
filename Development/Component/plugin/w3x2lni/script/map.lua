(function()
    local exepath = package.cpath:sub(1, (package.cpath:find(';') or 0)-6)
    package.path = package.path .. ';' .. exepath .. '..\\script\\?.lua'
end)()

require 'filesystem'
require 'utility'
local uni = require 'ffi.unicode'
local w2l = require 'w3x2lni'
local progress = require 'progress'
local archive = require 'archive'
local save_map = require 'save_map'
w2l:initialize()

function message(...)
    print(...)
end

local input = fs.path(uni.a2u(arg[1]))

message('正在打开地图...')
local slk = {}
local input_ar = archive(input)
if not input_ar then
    return
end
local output
if w2l.config.target_storage == 'dir' then
    if fs.is_directory(input) then
        output = input:parent_path() / (input:filename():string() .. '_' .. w2l.config.target_format)
    else
        output = input:parent_path() / input:stem():string()
    end
    fs.create_directory(output)
elseif w2l.config.target_storage == 'mpq' then
    if fs.is_directory(input) then
        output = input:parent_path() / (input:filename():string() .. '.w3x')
    else
        output = input:parent_path() / (input:stem():string() .. '_' .. w2l.config.target_format .. '.w3x')
    end
end
local output_ar = archive(output, 'w')
if not output_ar then
    return
end

message('正在读取物编...')
progress:start(0.4)
w2l:frontend(input_ar, slk)
progress:finish()

message('正在转换...')
progress:start(0.8)
w2l:backend(input_ar, slk)
progress:finish()

message('正在生成文件...')
progress:start(1)
save_map(w2l, output_ar, slk.w3i, input_ar)
progress:finish()
output_ar:close()
input_ar:close()
message('转换完毕,用时 ' .. os.clock() .. ' 秒') 
