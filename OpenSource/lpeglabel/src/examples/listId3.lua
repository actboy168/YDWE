local m = require'lpeglabel'
local re = require'relabel'

local terror = {
  ErrId     =  "expecting an identifier",
  ErrComma  =  "expecting ','",
  fail      =  "undefined"
}

local id = m.R'az'^1

local g = m.P{
  'S',
  S      =  m.V'List',
  List   =  m.V'Id' * (#m.P(1) * m.V'Comma' * (m.V'Id' + m.T'ErrId'))^0,
  Id     =  m.V'Sp' * id,
  Comma  =  m.V'Sp' * ',' + m.T'ErrComma',
  Sp     =  m.S' \n\t'^0,
}


function mymatch (g, s)
  local r, e, pos = g:match(s)
  if not r then
    local line, col = re.calcline(s, pos)
    local msg = "Error at line " .. line .. " (col " .. col .. "): "
    return r, msg .. terror[e] .. " before '" .. s:sub(pos) .. "'"
  end
  return r
end
  
print(mymatch(g, "one,two"))
print(mymatch(g, "one two"))
print(mymatch(g, "one,\n two,\nthree,4"))
print(mymatch(g, " 1,2"))
