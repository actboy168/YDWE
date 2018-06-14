local w2l
local content
local unpack_pos
local wts
local w3f

function set_index(...)
    unpack_pos = select(-1, ...)
    return ...
end

local function unpack(str)
    return set_index(str:unpack(content, unpack_pos))
end

local function add_head()
    w3f.campaign_version     = unpack 'l'
    w3f.editor_version       = unpack 'l'
    w3f.campaign_name        = w2l:load_wts(wts, unpack 'z')
    w3f.campaign_difficulty  = w2l:load_wts(wts, unpack 'z')
    w3f.author_name          = w2l:load_wts(wts, unpack 'z')
    w3f.campaign_description = w2l:load_wts(wts, unpack 'z')
end

return function (w2l_, content_, wts_)
    if not content_ then
        return nil
    end
    w2l = w2l_
    content = content_
    wts = wts_
    unpack_pos = 1
    w3f = {}

    local version = unpack 'l'
    add_head()
    
    return w3f
end
