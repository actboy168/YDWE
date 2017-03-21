require "util"

local file = fs.ydwe_path() / "bin" / "EverConfig.cfg"

local tbl_index = {
	MapSave = {
		Option = "0"
	},
	HostTest = {
		Option = "0"
	},
	War3Patch = {
		Option = "0"
	},
	MapTest = {
		EnableHost = "0",
		LaunchRenderingEngine = "Direct3D 8",
		LaunchWindowed = "1",
		LaunchFullWindowed = "0",
		LaunchWideScreenSupport = "1",
		LaunchLockingMouse = "0",
		LaunchFixedRatioWindowed = "0",
		LaunchDisableSecurityAccess = "0",
	},
	ScriptCompiler = {
		EnableJassHelper = "1",
		EnableJassHelperDebug = "0",
		EnableJassHelperScriptOnly = "0",
		EnableJassHelperOptimization = "1",
		EnableCJass = "0",
	},
	PJass = {
		Option = "0"
	},
	ScriptInjection = {
		Option = "0"
	},
	ThirdPartyPlugin = {
		EnableDotNetSupport = "0",
		EnableTesh = "1",
		EnableYDTrigger = "1",
	},
	FeatureToggle = {
		EnableManualNewId = "0",
		EnableTriggerCopyEncodingAutoConversion = "1",
		EnableShowInternalAttributeId = "0",
	},
	Font = {
		FontEnable = "0",
		FontName = "",
		FontSize = "12",
	},
}

function global_config_reload()
	local tbl = sys.ini_load(file)
	if not tbl then
		tbl = {}
	end
	tbl = setmetatable(tbl, {__index = tbl_index})
	global_config = tbl

	-- 根据ydweworldedit.exe的路径计算
	fs.__war3_path = fs.get(fs.DIR_EXE):remove_filename()
	log.debug('war3 path ' .. fs.__war3_path:string())
end
global_config_reload()
