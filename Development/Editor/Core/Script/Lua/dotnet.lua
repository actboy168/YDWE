require "registry"

local function is_dotnet_installed()
	return fs.exists(fs.get(fs.DIR_SYSTEM) / "MSCorEE.dll")
end

local function is_dotnet_version_20_installed()
	local err, value = registry.LoadInt(registry.HKEY_LOCAL_MACHINE, "Software\\Microsoft\\NET Framework Setup\\NDP\\v2.0.50727", "Install")
	return (err == 0) and (value ~= 0)
end

local function is_dotnet_version_35_installed()
	local err, value = registry.LoadInt(registry.HKEY_LOCAL_MACHINE, "Software\\Microsoft\\NET Framework Setup\\NDP\\v3.5", "Install")
	return (err == 0) and (value ~= 0)
end

dotnet = {}

function dotnet:load (path, entry)
    local suc, res = pcall(
		function()
			return clr.object(self.default_appdomain, path, entry)
		end
	)
    if suc then
        return res
    else
        return nil, res
    end
end

function dotnet:call (obj, name)
    local suc, res = pcall(
		function()
			return obj:call(name)
		end
	)
    if suc then
        return res
    else
        return nil, res
    end
end

function dotnet:initialize ()
	local dotnet_support = is_dotnet_installed()
		--and is_dotnet_version_35_installed()
		--and is_dotnet_version_20_installed()
		
	if not dotnet_support then
		log.error("Current platform does not support .net")
		return false
	end
	
	local success, err = pcall(require, "clr")
	if not success then
		log.error("Failed to load .net components")
		return false
	end
	log.debug(".net component loaded successfully.")

	self.default_appdomain = clr.appdomain()
	if not self.default_appdomain:vaild() then
		log.error("Failed to create appdomain")
		return false
	end

	return true
end

dotnet:initialize()
