require 'filesystem'

if _W2L_DIR then
    return fs.path(_W2L_DIR)
end

return fs.current_path():parent_path()
