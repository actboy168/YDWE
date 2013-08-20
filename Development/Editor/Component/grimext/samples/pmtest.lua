-- conditions
function dirtonlayertwo(tileid, cliff, layer)
  return tileid == "Odrt" and layer == 2
end

function abyssonlayertwo(tileid, cliff, layer)
  return tileid == "Oaby" and layer == 2
end

function roughdirt(tileid, cliff, layer)
  return tileid == "Osmb"
end

function dirtcliff(tileid, cliff, layer)
  return tileid == "Odrt" and cliff
end

function allclifftiles(tileid, cliff, layer)
  return iscliff(tileid)
end

function allnonclifftiles(tileid, cliff, layer)
  return isnoncliff(tileid)
end

function all(tileid, cliff, layer)
  return true
end

-- actions
function setunwalkable()
  addflags(nowalk) -- adds flags
  -- removeflags(nobuild) -- removes flags
  -- setflags(nofly, obstacle) -- replaces with the sum of all passed flags
  -- nowalk, nofly, nobuild, blight, nowater, obstacle
end

function setwalkable()
  if (not isflagset(obstacle)) then
    removeflags(nowalk) -- only if it is not the map border
  end
end

function setnoobstacle()
  removeflags(obstacle, nowalk)
end

function setnofly()
  if (isflagset(obstacle)) then
    addflags(nofly)
  end
end

function setspecialbuildable()
  if (isflagset(nowalk) and isflagset(nofly) and not isflagset(nobuild)) then
    removeflags(nowalk)
  end
end

function logcliff()
  log("cliff")
end

function lognoncliff()
  log("noncliff")
end

-- order matters
registercallback(dirtonlayertwo, setunwalkable)
registercallback(abyssonlayertwo, setwalkable)
registercallback(roughdirt, setnoobstacle)
registercallback(dirtcliff, setnofly)
registercallback(all, setspecialbuildable)
registercallback(allclifftiles, logcliff)
registercallback(allnonclifftiles, lognoncliff)