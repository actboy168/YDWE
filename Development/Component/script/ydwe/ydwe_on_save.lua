local compiler = require "compiler"
local map_packer = require 'w3x2lni.map_packer'
local dev = fs.ydwe_devpath()

local function backup_map(map_path)
    local ydwe_path = fs.ydwe_path()
    fs.create_directories(ydwe_path / 'backups')
    local buf = io.load(ydwe_path / 'backups' / 'backupsdata.txt')
    local char
    if buf then
        char = buf:match '(.)[\r\n]*$'
    else
        char = '0'
    end
    local filename = char .. map_path:extension():string()
    local target_path = ydwe_path / 'backups' / filename
    log.info('Backup map at ' .. target_path:string())
    fs.copy_file(map_path, target_path, true)
end

local function saveW3x(source_path, target_path, temp_path, save_version)
    fs.remove(target_path)
    local result = compiler:compile(temp_path, global_config, save_version)
    log.debug("Compiler Result " .. tostring(result))
    
    local result
    if target_path:filename():string() == '.w3x' then
        result = map_packer('lni', temp_path, source_path:parent_path())
        fs.copy_file(dev / 'plugin' / 'w3x2lni' / 'script' / 'core' / '.w3x', target_path, true)
    else
        result = map_packer('pack', temp_path, target_path)
        backup_map(target_path)
    end
    log.debug("Packer Result " .. tostring(result))
    return result
end

local function saveW3m(source_path, target_path, temp_path, save_version)
    fs.remove(target_path)
    local result = compiler:compile(temp_path, global_config, save_version)
    log.debug("Compiler Result " .. tostring(result))
    
    local result
    result = map_packer('pack', temp_path, target_path)
    backup_map(target_path)
    log.debug("Packer Result " .. tostring(result))
    return result
end

function event.EVENT_NEW_SAVE_MAP(event_data)
	log.debug("********************* on new save start *********************")

	-- 刷新配置数据
	global_config_reload()

    local target_path = fs.path(event_data.map_path)
    local temp_path = target_path:parent_path()
    local source_path = temp_path:parent_path() / target_path:filename()
    
    log.info("Saving " .. source_path:string())
    local save_type = temp_path:extension():string():sub(2, 4)
    local save_version = war3_version:is_new() and 24 or 20
    log.info("Type:", save_type, "Version:", save_version)

	-- 如果地图文件带有只读属性，则先询问是否去掉只读属性
	-- 128 == 0200 S_IWUSR
	if fs.exists(source_path) and 0 == (source_path:permissions() & 128) then
		if gui.yesno_message(nil, LNG.REMOVE_MAP_READONLY, source_path:string()) then
			log.trace("Remove the read-only attribute.")
			source_path:add_permissions(128)
		else
            log.trace("Don't remove the read-only attribute.")
            log.debug("********************* on new save end *********************")
            return -1
        end
    end

    local result = false
    if save_type == 'w3x' then
        result = saveW3x(source_path, target_path, temp_path, save_version)
    elseif save_type == 'w3m' then
        result = saveW3m(source_path, target_path, temp_path, save_version)
    else
        log.error('Unsupport save to ' .. save_type)
        gui.error_message(nil, LNG.UNSUPORTED_SAVE_TYPE, save_type)
    end

	log.debug("********************* on new save end *********************")
	if result then return 0 else return -1 end
end
