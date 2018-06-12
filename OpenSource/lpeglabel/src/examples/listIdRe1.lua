local re = require 'relabel' 

local g = re.compile[[
  S      <- Id List
  List   <- !.  /  Comma Id List
  Id     <- Sp [a-z]+ / %{ErrId}
  Comma  <- Sp ',' / %{ErrComma}
  Sp     <- %s*
]]

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

