local m = require'lpeglabel'
local re = require'relabel'

local id = m.R'az'^1

local g = m.P{
  "S",
  S = m.V"Id" * m.V"List",
  List = -m.P(1) + m.V"Comma" * m.V"Id" * m.V"List",
  Id = m.V"Sp" * id + m.T'ErrId',
  Comma = m.V"Sp" * "," + m.T'ErrComma',
  Sp = m.S" \n\t"^0,
}

function mymatch (g, s)
  local r, e, pos = g:match(s)
  if not r then
    local line, col = re.calcline(s, pos)
    local msg = "Error at line " .. line .. " (col " .. col .. ")"
    if e == 'ErrId' then
      return r, msg .. ": expecting an identifier before '" .. s:sub(pos) .. "'"
    elseif e == 'ErrComma' then
      return r, msg .. ": expecting ',' before '" .. s:sub(pos) .. "'"
    else
      return r, msg
    end
  end
  return r
end
  
print(mymatch(g, "one,two"))
print(mymatch(g, "one two"))
print(mymatch(g, "one,\n two,\nthree,"))
