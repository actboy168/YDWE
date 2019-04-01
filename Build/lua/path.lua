local fs = require 'bee.filesystem'

--和[MSBuild]::GetDirectoryNameOfFileAbove效果相同
local function GetDirectoryNameOfFileAbove(p, f)
    while true do
        if fs.exists(p / f) then
            return p
        end
        p = p:parent_path()
        if p:filename():string() == '' then
            error '无法定位根目录'
        end
    end
end

return GetDirectoryNameOfFileAbove(fs.current_path(), 'LICENSE')
