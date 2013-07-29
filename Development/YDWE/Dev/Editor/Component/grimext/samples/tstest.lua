originaltileset = gettileset() -- get base tileset of the map
originaltilecount = tilecount()
originalcliffcount = cliffcount()
usedcliffs = {} -- empty table
unusedcliffs = {}
noncliffs = {}

-- this is just to show off the available functions
for i = tilecount() - 1, 0, -1 do
  tile = get(i) -- get tile with index i from the tile list
  if iscliff(tile) and (gettileset(tile) == originaltileset) and isusedascliff(tile) then
    table.insert(usedcliffs, tile)
  end
  if iscliff(tile) and not isusedascliff(tile) then
    table.insert(unusedcliffs, tile)
  end
  if isnoncliff(tile) then
    table.insert(noncliffs, tile)
  end
  if not iscliff(tile) and not isused(tile) and (gettileset(tile) == originaltileset) then
    remove(i) -- we can remove by index or by tileid
  end
end

log("Left should be Ldrt, Ldrg, Lgrs, Lrok, cCc1")
for i = 0, tilecount() - 1 do
  log(get(i))
end

log("Should contain Ldrt")
for i = 1, table.getn(usedcliffs) do
  log(usedcliffs[i])
end

log("Should contain Lgrs")
for i = 1, table.getn(unusedcliffs) do
  log(unusedcliffs[i])
end

log("Should contain cCc1")
for i = 1, table.getn(noncliffs) do
  log(noncliffs[i])
end

settileset("C") -- changing to felwood
replace(0, getcliffversion("cCc1")) -- we can also replace by index
replace("cCc1", getnoncliffversion("Ldrt"))
replace("Lgrs", "cWc1")
replace("Lrok", "Crck")
add("Cvin") -- max is 16 tiles
