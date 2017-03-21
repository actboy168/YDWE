local w3xparser = require 'w3xparser'

local function search_staticfile(map)
    map:get '(listfile)'
    map:get '(signature)'
    map:get '(attributes)'
end

local function search_listfile(map)
    local buf = map:get '(listfile)'
    if buf then
        for name in buf:gmatch '[^\r\n]+' do
            map:get(name)
        end
    end
end

local function search_imp(map)
    local buf = map:get 'war3map.imp'
    if buf then
        local _, count, index = ('ll'):unpack(buf)
        local name
        for i = 1, count do
            _, name, index = ('c1z'):unpack(buf, index)
            if not map:get(name) then
                map:get('war3mapimported\\' .. name)
            end            
        end
    end
end

local searchers = {
    search_listfile,
    search_staticfile,
    search_imp,
}

local function search_mpq(map)
    local total = map:number_of_files()
    
    for i, searcher in ipairs(searchers) do
        pcall(searcher, map)
        if map.read_count == total then
            return true
        end
    end

    message('-report|1严重错误', ('还有%d个文件没有读取'):format(total - map.read_count))
    message('-tip', '这些文件被丢弃了,请包含完整(listfile)')
    message('-report|1严重错误', ('读取(%d/%d)个文件'):format(map.read_count, total))
end

local function scan_dir(dir, callback)
    for path in dir:list_directory() do
        if fs.is_directory(path) then
            scan_dir(path, callback)
        else
            callback(path)
        end
    end
end

local function search_dir(map)
    local len = #map.path:string()
    scan_dir(map.path, function(path)
        local name = path:string():sub(len+2):lower()
        map:get(name)
    end)
end

local function save_imp(w2l, output_ar, imp_buf)
    local impignore = {}
    for _, name in ipairs(w2l.info.pack.impignore) do
        impignore[name] = true
    end
    local imp = {}
    for name, buf in pairs(output_ar) do
        if buf and not impignore[name] then
            imp[#imp+1] = name
        end
    end
    if imp_buf then
        local imp_lni = w2l:parse_lni(imp_buf)
        for _, name in ipairs(imp_lni.import) do
            local name = name:lower()
            if impignore[name] then
                imp[#imp+1] = name
            end
        end
    end
    table.sort(imp)
    local hex = {}
    hex[1] = ('ll'):pack(1, #imp)
    for _, name in ipairs(imp) do
        hex[#hex+1] = ('z'):pack(name)
    end
    output_ar:set('war3map.imp', table.concat(hex, '\r'))
end

return function (w2l, output_ar, w3i, input_ar)
    if input_ar:get_type() == 'mpq' then
        search_mpq(input_ar)
    else
        search_dir(input_ar)
    end
    if w2l.config.remove_we_only then
        input_ar:set('war3map.wtg', false)
        input_ar:set('war3map.wct', false)
        input_ar:set('war3map.imp', false)
        input_ar:set('war3map.w3s', false)
        input_ar:set('war3map.w3r', false)
        input_ar:set('war3map.w3c', false)
        input_ar:set('war3mapunits.doo', false)
    else
        if not input_ar:get 'war3mapunits.doo' then
            input_ar:set('war3mapunits.doo', w2l:create_unitsdoo())
        end
    end
    for name, buf in pairs(input_ar) do
        if buf then
            if w2l.config.mdx_squf and name:sub(-4) == '.mdx' then
                buf = w3xparser.mdxopt(buf)
            end
            output_ar:set(name, buf)
        end
    end
    output_ar:set('(listfile)', false)
    output_ar:set('(signature)', false)
    output_ar:set('(attributes)', false)

    local imp = input_ar:get 'war3map.imp.ini'
    if w2l.config.target_format ~= 'lni' then
        output_ar:set('war3map.imp.ini', false)
    end

    if not w2l.config.remove_we_only and output_ar:get_type() == 'mpq' then
        save_imp(w2l, output_ar, imp)
    end

    if not output_ar:save(w3i, w2l.config.remove_we_only) then
        message('创建新地图失败,可能文件被占用了')
    end
end
