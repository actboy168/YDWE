local function txt2wct(self, file_name_in, file_name_out)
    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    local chunk    = {}
    local index    = 1
    local max    = #content
    --文件头
    chunk.file_ver    = content:match 'VERSION%=(%d+)'

    --遍历文本
    for chars in content:gmatch '########%c*(.-)%c*########' do
        table.insert(chunk, chars)
    end

    --生成二进制文件
    local pack    = {}

    --文件头
    table.insert(pack, ('l'):pack(chunk.file_ver))
    --自定义区域注释
    table.insert(pack, ('z'):pack(chunk[1]))
    --自定义区域文本大小与文本
    if chunk[2] == '' then
        table.insert(pack, ('l'):pack(0))
    else
        table.insert(pack, ('lz'):pack(#chunk[2] + 1, chunk[2]))
    end
    --触发器数量
    table.insert(pack, ('l'):pack(#chunk - 2))
    --触发器文本大小与文本
    for i = 3, #chunk do
        if chunk[i] == '' then
            table.insert(pack, ('l'):pack(0))
        else
            table.insert(pack, ('lz'):pack(#chunk[i] + 1, chunk[i]))
        end
    end

    io.save(file_name_out, table.concat(pack))
    
end

return txt2wct
