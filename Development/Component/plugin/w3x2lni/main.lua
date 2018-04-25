if _W2L_MODE == 'CLI' then
    require 'backend'
    return
elseif _W2L_MODE == 'GUI' then
    if arg[1] then
        require 'gui.mini'
        return
    end
    require 'gui.old.main'
end
