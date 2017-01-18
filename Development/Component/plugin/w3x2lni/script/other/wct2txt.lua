local function wct2txt(self, file_name_in, file_name_out)
    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    local index = 1
    local max    = #content
    local chunk    = {}

    --文件版本
    chunk.file_ver, index    = ('l'):unpack(content, index)

    chunk.triggers    = {}

    --自定义代码区的注释
    chunk.comment, index        = ('z'):unpack(content, index)

    --自定义代码区的文本
    local trigger    = {}
    table.insert(chunk.triggers, trigger)

    trigger.size, index    = ('l'):unpack(content, index)
    if trigger.size ~= '0' then
        trigger.content, index    = ('z'):unpack(content, index)
    end

    --触发器数量
    chunk.trigger_count, index    = ('l'):unpack(content, index)
    
    for i = 1, chunk.trigger_count do
        local trigger    = {}
        table.insert(chunk.triggers, trigger)

        --文本长度
        trigger.size, index    = ('l'):unpack(content, index)

        --如果文本长度为0,无文本
        if trigger.size == 0 then
            trigger.content    = ''
        else
            trigger.content, index    = ('z'):unpack(content, index)
        end
    end

    --转换文本
    local lines    = {}

    --文件版本
    table.insert(lines, ('VERSION=%s'):format(chunk.file_ver))
    table.insert(lines, ('########\r\n%s\r\n########'):format(chunk.comment))

    --文本
    for _, trigger in ipairs(chunk.triggers) do
        table.insert(lines, ('########\r\n%s\r\n########'):format(trigger.content))
    end

    io.save(file_name_out, table.concat(lines, '\r\n'):convert_wts() .. '\r\n')
    
end

return wct2txt
