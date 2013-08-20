setobjecttype("units") -- units, items, doodads, destructables, abilities, buffs, upgrades
mergefile("input.w3u", import) -- merge, replace, import

setobjecttype("doodads")
mergefile("input.w3d", replace)

setobjecttype("abilities")
mergefile("input.w3a", merge)
mergefile("more.w3a", merge)

if objectexists("AHbz") then
  modifyobject("AHbz")
  if (currentobject() == "AHbz") then
    logf(currentobjecttype(), currentobject()) -- write debug information to log
    makechange(current, "Tip", 3, "Lame Level 3 Tooltip")
    makechange(current, "Cool", 2, 99.5)
    makechange(current, "achd", 1)
    makechange(current, "alev", 4)
  end 
end

if objectexists("S000") then
  createobject("S000", "Sxyz") -- new id will be Sxyz
  if (currentobject() == "Sxyz") then
    logf(currentobjecttype(), currentobject()) -- write debug information to log
    makechange(current, "Hotkey", "Q")
    makechange(current, "Name", "My Chaos")
    makechange(current, "EditorSuffix", "(Peasant)")
    makechange(current, "CheckDep", 0)
    makechange(current, "Cha1", 1, "hpea")
  end
end

if objectexists("AHbz") then
  createobject("AHbz", generateid("AHbz")) -- new id will be generated as substitute for the passed AHbz id
  if (currentobject() ~= "") then -- "" would indicate that we do not have a current object
    logf(currentobjecttype(), currentobject()) -- write debug information to log
    makechange(current, "Levels", 10)
    makechange(current, "Tip", 6, "Blizzard Level 6")
    makechange(current, "ResearchTip", "Learn Blixxard")
    makechange(current, "Tip", 5, "fuckfuckfuckfuckfuck")
    resetchange(current, "Tip", 5) -- due to bad language
  end
  createobject("AHbz", generateid("AHbz"))
  if (currentobject() ~= "") then
    logf(currentobjecttype(), currentobject()) -- write debug information to log
    makechange(current, "Name", "LOL")
    resetobject(current)
  end
end

setobjecttype("doodads")
createobject("ZOss", generateid("ZOss"))
createobject("ZOss", generateid("ZOss"))
createdid = currentobject()

-- we set this again because the modded, unmodded and custom groups refer to the state of the last time we called setobjecttype
setobjecttype("doodads")
makechange(custom, "dmis", 0.55) -- min scale for all custom doodads
makechange(original, "dmas", 300) -- max scale for all original doodads
makechange(unmodded, "dmis", 0.99) -- min scale for all unmodded original doodads
resetchange(modded, "dmas") -- max scale for all modded original doodads
modifyobject(createdid)
resetobject(current) -- resetobject also works with custom, original, unmodded, modded groups
