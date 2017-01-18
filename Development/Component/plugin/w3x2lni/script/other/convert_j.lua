local function convert_j(self, file_name_in, file_name_out)
    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    io.save(file_name_out, content:convert_wts(false, true))

end

return convert_j
