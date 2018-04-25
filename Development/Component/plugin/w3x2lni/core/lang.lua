return setmetatable({}, {
    __index = function (self, k)
        self[k] = setmetatable({}, {
            __index = function (self, v)
                self[v] = v
                return v
            end
        })
        return self[k]
    end
})
