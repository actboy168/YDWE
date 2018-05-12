local mt = {}

mt.info = {
    name = 'YDWE定制',
    version = 1.0,
    author = '最萌小汐',
    description = '修改mpq与prebuilt的读写路径。',
}

function mt:on_convert(w2l)
    require 'filesystem'
    local root = fs.ydwe(true)
    local ydpath = fs.ydwe(false)
    local lang = require 'ffi.language' ()
    local mpq_path = require 'core.mpq_path' ()
    local defined = loadfile((root / 'script' / 'ydwe' / 'defined.lua'):string())()

    mpq_path:lang(lang)

    local updated
    local function update_version()
        if updated then
            return
        end
        updated = true
        if w2l.config.version == 'Melee' then
            mpq_path:open 'Melee_V1'
        else
            mpq_path:open 'Custom_V1'
        end
    end
    
    local war3_path = root / 'share' / 'mpq'
    function w2l:mpq_load(filename)
        return mpq_path:each_path(function (path)
            return io.load(war3_path / path / filename)
        end)
    end

    local defined_path = ydpath / 'share' / 'mpq' / 'defined'
    function w2l:defined_load(filename)
        return io.load(defined_path / filename)
    end

    defined(w2l)

    local prebuilt_path = ydpath / 'script' / 'ydwe' / 'prebuilt'
    function w2l:prebuilt_load(filename)
        return mpq_path:each_path(function (path)
            return io.load(prebuilt_path / path / filename)
        end)
    end
    function w2l:prebuilt_save(filename, buf)
        local path = prebuilt_path / mpq_path:first_path() / filename
        fs.create_directories(path:parent_path())
        io.save(path, buf)
	end

    w2l.config.optimize_jass = false
    w2l.config.mdx_squf = false

    w2l:set_config(w2l.config)
end

return mt
