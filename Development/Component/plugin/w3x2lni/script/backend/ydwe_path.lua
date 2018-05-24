return function()
    require 'registry'
    local commands = registry.current_user() / [[SOFTWARE\Classes\YDWEMap\shell\run_war3\command]]
    if not commands then
        return
    end
    local command = commands['']
    if not command then
        return
    end
    local path = command:match '^"([^"]*)"'
    local ydwe = fs.path(path):parent_path()
    if fs.exists(ydwe / 'YDWE.exe') then
        return ydwe
    end
    local ydwe = ydwe:parent_path()
    if fs.exists(ydwe / 'YDWE.exe') then
        return ydwe
    end
end
