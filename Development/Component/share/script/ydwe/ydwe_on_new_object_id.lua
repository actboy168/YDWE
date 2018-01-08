require "filesystem"
require "localization"
local ffi = require "ffi"
local lni = require 'lni-c'

local path = fs.ydwe_path() / 'share' / 'mpq' / 'units' / 'prebuilt' / 'Custom'
local default = {}
local function get_default(type)
	if not default[type] then
		default[type] = lni(io.load(path / (type .. '.ini')))
	end
	return default[type]
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
	
	-- 获取当前窗口
	local foregroundWindow = gui.get_foreground_window()

	-- 循环直到输入合法或者放弃
	while true do
		-- 打开对话框让用户输入
		local ok, id_string = gui.prompt_for_input(
			foregroundWindow, 														-- 父窗口句柄
			_("NEW_OBJECT_ID"),														-- 标题栏
			_("INPUT_OBJECT_ID"),		-- 提示语句
			from_objectid(default_id),							                	-- 文本编辑区初始文字
			_("OK"),																-- “确定”按钮文本
			_("CANCEL")																-- “取消"按钮文本
		)
		
		-- 用户点了确定，验证输入是否合法。如果点了取消，使用系统默认
		if not ok then
			log.trace("User cancel.")
			return default_id
		end
		
		-- 检查输入是否合法（字符串长度是否为4）
		if #id_string ~= 4 then
			log.trace("User input error(" .. tostring(id_string) .. ").")	
			-- 提示错误
			gui.error_message(foregroundWindow, _("ERROR_OBJECT_ID"))
		elseif object:custom_has(object_type, id_string) or object:original_has(event_data.class, id_string) then
			log.trace("User input error(" .. tostring(id_string) .. ").")	
			-- 提示错误
			gui.error_message(foregroundWindow, _("EXISTS_OBJECT_ID"))
		else
			-- 合法，转换为整数返回	
			log.trace("Result " .. tostring(id_string))	
			return to_objectid(id_string)
		end
				
	end
	return 0
end
