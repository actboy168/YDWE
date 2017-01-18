if arg[1] == '-backend' then
    table.remove(arg, 1)
    require 'map'
elseif arg[1] == '-prebuilt' then
    local nk = require 'nuklear'
    nk:console()
    table.remove(arg, 1)
    require 'prebuilt.prebuilt'
    os.execute('pause')
elseif arg[1] == '-slk' then
    local nk = require 'nuklear'
    nk:console()
    table.remove(arg, 1)
    require 'slk'
    os.execute('pause')
else
    require 'gui.main'
end
