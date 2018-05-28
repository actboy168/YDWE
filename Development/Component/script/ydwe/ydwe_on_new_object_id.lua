require "filesystem"
require "localization"
local ffi = require "ffi"
local root = fs.ydwe_devpath()
local ydpath = fs.ydwe_path()
local w3x2lni = require 'w3x2lni'

local function get_default(type)
	local w2l = w3x2lni()
	return w2l:get_default()[type]
end

local function from_objectid (id)
	return string.pack('<I4', id)
end

local function to_objectid (str)
	return string.unpack('<I4', str)
end

local object = {}

local object_type = {
	[0] = 'unit',
	[1] = 'item',
	[2] = 'destructable',
	[3] = 'doodad',
	[4] = 'ability',
	[5] = 'buff',
	[6] = 'upgrade',
}

function object:original_has (this_, id_string_)
	local this_ptr_ = ffi.cast('uint32_t*', this_)
	local ptr  = this_ptr_[7] + 4
	local size = this_ptr_[6]

	for i = 0, size-2 do
		local id = ffi.cast('uint32_t*', ptr)[0]
		if from_objectid(id) == id_string_ then
			return true
		end
		ptr = ptr + 24
	end

	return false
end

function object:custom_has (type_, id_string_)
	if not object_type[type_] then
		return false
	end
	if not get_default(object_type[type_])[id_string_] then
		return false
	end
	return true
end


-- 在新建物体的时候调用，本函数根据用户的操作决定新ID值
-- object_type - 整数类型，物体的类型
-- default_id - 整数类型，系统生成的ID
-- 返回值：新建物体的最终ID，必须是整数类型
function event.EVENT_NEW_OBJECT_ID(event_data)
	log.debug("**************** on new object id start ****************")	
		
	local object_type = event_data.object_type
	local default_id = event_data.default_id
	-- 刷新配置数据
	global_config_reload()

	-- 如果没有选择手动输入则直接返回
	if tonumber(global_config["FeatureToggle"]["EnableManualNewId"]) == 0 then
		log.trace("Disable.")
		return default_id
	end
	local dialog = require 'gui_newobjectid'
    local res = dialog(from_objectid(default_id), function(r)
        if #r ~= 4 then
            return LNG.ERROR_OBJECT_ID
        end
        if object:custom_has(object_type, r) or object:original_has(event_data.class, r) then
            return LNG.EXISTS_OBJECT_ID
        end
    end)
    return to_objectid(res)
end
