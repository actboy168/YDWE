require "mapanalyzer"
require "filesystem"
require "interface_storm"

local mapanalyzer = mapanalyzer
local setmetatable = setmetatable
local slk_mgr = {}

function slk_mgr.initialize (self)
	local mpq_path = fs.ydwe_path() / "share" / "mpq"
	self.interface = interface_stormlib()
	self.interface:open_path(mpq_path / "units")
	self.interface:open_path(mpq_path / "ydwe")
	self.interface:attach_archive(__map_handle__)	
	self.manager = mapanalyzer.manager2(self.interface)

	self.table_type = {}
	self.table_type.ability      = mapanalyzer.OBJECT_TYPE.ABILITY
	self.table_type.buff         = mapanalyzer.OBJECT_TYPE.BUFF
	self.table_type.destructable = mapanalyzer.OBJECT_TYPE.DESTRUCTABLE
	self.table_type.doodad       = mapanalyzer.OBJECT_TYPE.DOODAD
	self.table_type.item         = mapanalyzer.OBJECT_TYPE.ITEM
	self.table_type.unit         = mapanalyzer.OBJECT_TYPE.UNIT
	self.table_type.upgrade      = mapanalyzer.OBJECT_TYPE.UPGRADE
	self.table_type.misc         = mapanalyzer.OBJECT_TYPE.MISC

	self.cache = setmetatable({}, {
		__index = function (table_, key_)
			local v = self.manager:load(key_)
			table_[key_] = v
			return v
		end
	})
end
			
slk_mgr:initialize()

local _slk_object = {}

function _slk_object.factory (self)
	local it = self._object:begin()
	return function ()
		if it:valid() then
			local k, v = it:next()
			return k, slk_mgr.manager:convert(v)
		end
		return nil
	end
end

function _slk_object._index (self, key)
	local str = self._object:get(key)
	return str and slk_mgr.manager:convert(str) or ''
end
            
function _slk_object.create (object)
	return setmetatable(
		{
			_object = object,
			factory = _slk_object.factory, 
		}, 
		{ 
			__index = _slk_object._index 
		})
end

local _slk_table = {}

function _slk_table.cache (self)
	return slk_mgr.cache[self._type]
end

function _slk_table.factory (self)	
	local it = self:cache():begin()
	return function ()
		if it:valid() then
			local k, v = it:next()
			return k, _slk_object.create(v)
		end
		return nil
	end
end

function _slk_table._index (self, key)
	return _slk_object.create(self:cache():get(key) or mapanalyzer.slk_object())
end

function _slk_table.create (table_type)
	return setmetatable(
		{
			_type   = table_type,
			factory = _slk_table.factory,
			cache   = _slk_table.cache,
		}, 
		{ 
			__index = _slk_table._index,
		})
end

local slk_interface = {}

function slk_interface.initialize (self)
	self.ability      = _slk_table.create(slk_mgr.table_type.ability)
	self.buff         = _slk_table.create(slk_mgr.table_type.buff)
	self.destructable = _slk_table.create(slk_mgr.table_type.destructable)
	self.doodad       = _slk_table.create(slk_mgr.table_type.doodad)
	self.item         = _slk_table.create(slk_mgr.table_type.item)
	self.unit         = _slk_table.create(slk_mgr.table_type.unit)
	self.upgrade      = _slk_table.create(slk_mgr.table_type.upgrade)
	self.misc         = _slk_table.create(slk_mgr.table_type.misc)
		
	-- deprecated
	self.Ability      = self.ability     
	self.Buff         = self.buff        
	self.Destructable = self.destructable
	self.Doodad       = self.doodad      
	self.Item         = self.item        
	self.Unit         = self.unit        
	self.Upgrade      = self.upgrade     
	self.Misc         = self.misc 
end

-- deprecated functions
function slk_interface.Create (self)	
	return self
end

-- deprecated functions
function slk_interface.Foreach (t)
	return t:factory()
end

slk_interface:initialize()

return slk_interface
