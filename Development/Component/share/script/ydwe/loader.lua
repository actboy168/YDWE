local mt = {}

function mt:mpq_load(filename)
    if self.on_mpq_load then
        return self:on_mpq_load(filename)
    end
    return nil
end

function mt:map_load(filename)
    if self.on_map_load then
        return self:on_map_load(filename)
    end
    return nil
end

function mt:map_save(filename, buf)
    if self.on_map_save then
        return self:on_map_save(filename, buf)
    end
    return false
end

function mt:map_remove(filename)
    if self.on_map_remove then
        return self:on_map_remove(filename)
    end
    return false
end

return mt
