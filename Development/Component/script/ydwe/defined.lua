require 'filesystem'
local w2l_path = fs.ydwe(true) / 'plugin' / 'w3x2lni' / 'script'
local yd_path = fs.ydwe(false)
local defined_path = yd_path / 'share' / 'mpq' / 'defined'

return function (w2l)
    package.loaded['share.messager'] = setmetatable({}, {
        __index = function ()
            return function ()
            end
        end,
    })
    package.loaded['share.lang'] = setmetatable({}, {
        __index = function ()
            return setmetatable({}, {
                __index = function (_, str)
                    return str
                end,
            })
        end,
    })
    local prebuilt_keydata = loadfile((w2l_path / 'prebuilt' / 'keydata.lua'):string())()
    local prebuilt_search = loadfile((w2l_path / 'prebuilt' / 'search.lua'):string())()
    package.loaded['share.messager'] = nil
    package.loaded['share.lang'] = nil

    local function loader(name)
        return w2l:mpq_load(name)
    end

    local keydata = prebuilt_keydata(w2l, loader)
    local search = prebuilt_search(w2l, loader)
    fs.create_directories(defined_path)
    io.save(defined_path / 'keydata.ini', keydata)
    io.save(defined_path / 'search.ini', search)
end
