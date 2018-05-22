local lang = require 'share.lang'
local config = require 'share.config'
local root = fs.current_path()

return function (w2l, version)
    w2l.messager.text(lang.script.CONVERT_ONE .. version)

    w2l:set_config
    {
        data_war3 = config.global.data_war3,
        data_ui   = config.global.data_ui,
        data_meta = config.global.data_meta,
        data_wes  = config.global.data_wes,
        version   = version,
    }
    local prebuilt_path = root:parent_path() / 'data' / config.global.data_war3 / 'prebuilt' / w2l.mpq_path:first_path()
    fs.create_directories(prebuilt_path)

    function w2l:prebuilt_save(filename, buf)
        io.save(prebuilt_path / filename, buf)
    end

    local slk = w2l:frontend_buildslk()

    function w2l:prebuilt_save()
    end

    return slk
end
