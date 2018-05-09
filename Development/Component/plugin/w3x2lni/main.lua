if _W2L_MODE == 'CLI' then
    require 'backend'
    return
elseif _W2L_MODE == 'GUI' then
    require 'gui.old.main'
end
