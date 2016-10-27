require 'luabind'
require 'filesystem'

--和[MSBuild]::GetDirectoryNameOfFileAbove效果相同
local function GetDirectoryNameOfFileAbove(p, f)
    while true do
        if fs.exists(p / f) then
            return p
        end
        p:remove_filename()
        if p:filename():string() == '' then
            return nil
        end
    end
end

return GetDirectoryNameOfFileAbove(fs.current_path(), 'build.root')
