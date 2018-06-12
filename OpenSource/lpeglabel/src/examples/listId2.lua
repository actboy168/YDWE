local m = require'lpeglabel'
local re = require'relabel'

local terror = {
  ErrId   =  "expecting an identifier",
  ErrEnd  =  "expecting EOF",
  fail    =  "undefined"
}

local id = m.R'az'^1

local g = m.P{
  'S',
  S      =  m.V'List' * (-m.P(1) + m.T'ErrEnd'),
  List   =  m.V'Id' * (m.V'Comma' * (m.V'Id' + m.T'ErrId'))^0,
  Id     =  m.V'Sp' * id,
  Comma  =  m.V'Sp' * ',',
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
