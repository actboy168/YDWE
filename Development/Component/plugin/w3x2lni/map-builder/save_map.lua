local w3xparser = require 'w3xparser'
local lang = require 'share.lang'

local function build_imp(w2l, output_ar, imp_buf)
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
    return table.concat(hex, '\r')
end

return function (w2l, w3i, input_ar, output_ar)
    local files = {}
    if w2l.config.remove_we_only then
        w2l:file_remove('map', 'war3map.wtg')
        w2l:file_remove('map', 'war3map.wct')
        w2l:file_remove('map', 'war3map.imp')
        w2l:file_remove('map', 'war3map.w3s')
        w2l:file_remove('map', 'war3map.w3r')
        w2l:file_remove('map', 'war3map.w3c')
        w2l:file_remove('map', 'war3mapunits.doo')
    else
        if not w2l:file_load('map', 'war3mapunits.doo') then
            w2l:file_save('map', 'war3mapunits.doo', w2l:backend_unitsdoo())
        end
    end
    
    for _, name in pairs(w2l.info.pack.packignore) do
        w2l:file_remove('map', name)
    end
    w2l:map_remove('.w3x')
    local imp = w2l:file_load('table', 'imp')
    w2l:file_remove('table', 'imp')

    for type, name, buf in w2l:file_pairs() do
        if type == 'resource' and w2l.config.mdx_squf and name:sub(-4) == '.mdx' then
            buf = w3xparser.mdxopt(buf)
        end
        if type == 'w3x2lni' and w2l.config.remove_we_only then
        else
            w2l:file_save(type, name, buf)
        end
    end
    
    if w2l.config.mode == 'lni' then
        if w2l:file_load('map', 'war3map.imp') then
            w2l:file_save('table', 'imp', w2l:backend_imp(w2l:file_load('map', 'war3map.imp')))
            w2l:file_remove('map', 'war3map.imp')
        else
            w2l:file_save('table', 'imp', imp)
        end
    else
        if not w2l.config.remove_we_only and imp then
            w2l:file_save('map', 'war3map.imp', build_imp(w2l, output_ar, imp))
        end
    end

    input_ar:close()
    local suc, res = output_ar:save(w3i, w2l)
    if not suc then
        w2l:failed(res or lang.script.CREATE_FAILED)
    end
    output_ar:close()
end
