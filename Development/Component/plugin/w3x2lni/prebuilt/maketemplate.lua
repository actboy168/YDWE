local config = require 'share.config'
local root = fs.current_path()

return function (w2l, version, slk)
    local template_path = root:parent_path() / 'template' / version
    w2l:set_config
    {
        data_war3 = config.global.data_war3,
        data_ui   = config.global.data_ui,
        data_meta = config.global.data_meta,
        data_wes  = config.global.data_wes,
        version   = version,
    }
    w2l.progress:start(0.3)
    slk = slk or w2l:get_default(true)
    w2l.progress:finish()
    fs.create_directories(template_path)
    w2l.progress:start(1.0)
    for i, ttype in ipairs {'ability', 'buff', 'unit', 'item', 'upgrade', 'doodad', 'destructable', 'misc'} do
        w2l.progress:start(i / 9)
        local data = w2l:frontend_merge(ttype, slk[ttype], {})
        io.save(template_path / (ttype .. '.ini'), w2l:backend_lni(ttype, data))
        w2l.progress:finish()
    end
    io.save(template_path / 'txt.ini', w2l:backend_txtlni(slk.txt))
    w2l.progress:finish()
end
