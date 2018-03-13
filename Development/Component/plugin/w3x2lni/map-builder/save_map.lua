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

    print('-report|1严重错误', ('还有%d个文件没有读取'):format(total - map.read_count))
    print('-tip', '这些文件被丢弃了,请包含完整(listfile)')
    print('-report|1严重错误', ('读取(%d/%d)个文件'):format(map.read_count, total))
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

local function save_imp(w2l, output_ar, imp_buf, filename)
    local impignore = {}
    for _, name in ipairs(w2l.info.pack.impignore) do
        impignore[name] = true
    end
    for _, name in pairs(w2l.info.obj) do
        impignore[name] = true
    end
    for _, name in pairs(w2l.info.lni) do
        impignore[name] = true
    end
    for _, slks in pairs(w2l.info.slk) do
        for _, name in ipairs(slks) do
            impignore[name] = true
        end
    end
    for _, name in ipairs(w2l.info.txt) do
        impignore[name] = true
    end
    local imp = {}
    for name, buf in pairs(output_ar) do
        if buf and not impignore[name] then
            imp[#imp+1] = name
        end
    end
    if imp_buf then
        local imp_lni = w2l:parse_lni(imp_buf, filename)
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
        input_ar:remove('war3map.wtg')
        input_ar:remove('war3map.wct')
        input_ar:remove('war3map.imp')
        input_ar:remove('war3map.w3s')
        input_ar:remove('war3map.w3r')
        input_ar:remove('war3map.w3c')
        input_ar:remove('war3mapunits.doo')
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
    output_ar:remove('(listfile)')
    output_ar:remove('(signature)')
    output_ar:remove('(attributes)')

    if w2l.config.mode ~= 'lni' then
        local imp = input_ar:get 'war3map.imp.ini'
        output_ar:remove('war3map.imp.ini')
        if not w2l.config.remove_we_only then
            save_imp(w2l, output_ar, imp, 'war3map.imp.ini')
        end
    end

    input_ar:close()
    if not output_ar:save(w3i, w2l.progress, w2l.config.remove_we_only) then
        print('创建新地图失败,可能文件被占用了')
    end
    output_ar:close()
end
