
local function table_remove(a, k)
	for i, v in ipairs(a) do
		if v == k then
			table.remove(a, i)
			break
		end
	end
end

local function table_append(a, b)
	for _, bv in ipairs(b) do
		for i, av in ipairs(a) do
			if av[1] == bv[1] then
				table.remove(a, i)
				break
			end
		end
	end
	local pos = 1
	for _, v in ipairs(b) do
		table.insert(a, pos, v)
		pos = pos + 1
	end
end

local function merge_define(a, b)
	for section, keyvalue in pairs(b) do
		if not a[section] then
			a[section] = {}
		end
		table_append(a[section], keyvalue)
	end
end

local function merge_ui(type, a, b)
    local aca = a.categories[type]
    local bca = b.categories[type]
    local aui = a.ui[type]
    local bui = b.ui[type]
	for k, bv in pairs(bui) do
		if aui[k] then
			table_remove(aca[aui[k].category], aui[k])
		end
        aui[k] = bv
	end
    local pos = 1
    for _, category in ipairs(bca) do
        if not aca[category] then
            table.insert(aca, pos, category)
            pos = pos + 1
            aca[category] = bca[category]
        else
			for i, v in ipairs(aca) do
				if v == category then
					pos = i + 1
					break
				end
			end
			local a, b = aca[category], bca[category]
			local pos = 1
			for _, v in ipairs(b) do
				table.insert(a, pos, v)
				pos = pos + 1
			end
        end
    end
end

return function (a, b)
	if not a then
		return b
	end
    merge_define(a.ui.define, b.ui.define)
    merge_ui('event', a, b)
    merge_ui('condition', a, b)
    merge_ui('action', a, b)
    merge_ui('call', a, b)
	return a
end
