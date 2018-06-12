local m = require'lpeglabel'
local re = require'relabel'

local terror = {
  ErrId     =  "expecting an identifier",
  ErrComma  =  "expecting ','"
}

local subject, errors

local function recorderror(pos, lab)
	local line, col = re.calcline(subject, pos)
	table.insert(errors, { line = line, col = col, msg = terror[lab] })
end

local function record (lab)
	return (m.Cp() * m.Cc(lab)) / recorderror
end

local function sync (p)
	return (-p * m.P(1))^0
end

local id = m.R'az'^1

local g = m.P{
  "S",
  S = m.V"Id" * m.V"List",
  List = -m.P(1) + m.V"Comma" * m.V"Id" * m.V"List",
  Id = m.V"Sp" * id + m.T'ErrId',
  Comma = m.V"Sp" * "," + m.T'ErrComma',
  Sp = m.S" \n\t"^0,
  ErrId = record('ErrId') * sync(m.P","),
  ErrComma = record('ErrComma') * sync(id),
}

function mymatch (g, s)
	errors = {}
	subject = s	
	local r, e, sfail = g:match(s)
	if #errors > 0 then
		local out = {}
    for i, err in ipairs(errors) do
    	local msg = "Error at line " .. err.line .. " (col " .. err.col .. "): " .. err.msg
      table.insert(out,  msg)
    end
    return nil, table.concat(out, "\n") .. "\n"
  end
  return r
end
  
print(mymatch(g, "one,two"))
print(mymatch(g, "one two three"))
print(mymatch(g, "1,\n two, \n3,"))
print(mymatch(g, "one\n two123, \nthree,"))
