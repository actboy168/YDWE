require 'filesystem'
local w2l_path = fs.ydwe_devpath() / 'plugin' / 'w3x2lni'
local yd_path = fs.ydwe_path()
local defined_path = yd_path / 'share' / 'mpq' / 'defined'

return function (w2l)
    package.loaded['tool.messager'] = {
        text = function ()
        end,
    }
    local prebuilt_keydata = loadfile((w2l_path / 'prebuilt' / 'keydata.lua'):string())()
    local prebuilt_search = loadfile((w2l_path / 'prebuilt' / 'search.lua'):string())()
    package.loaded['tool.messager'] = nil

    local function loader(name)
        return w2l:mpq_load(name)
    end

    local keydata = prebuilt_keydata(w2l, loader)
    local search = prebuilt_search(w2l, loader)
    fs.create_directories(defined_path)
    io.save(defined_path / 'keydata.ini', keydata)
    io.save(defined_path / 'search.ini', search)
end
