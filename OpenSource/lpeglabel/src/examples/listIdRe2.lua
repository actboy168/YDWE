local re = require 'relabel' 

local errinfo = {
  ErrId     =  "expecting an identifier",
  ErrComma  =  "expecting ','",
  fail      =  "undefined",
}

local function sync (p)
	return '( !(' .. p .. ') .)*'
end

local errors

local function recorderror (subject, pos, label)
	local line, col = re.calcline(subject, pos)
	table.insert(errors, { line = line, col = col, msg = errinfo[label] })
	return true 
end

local g = re.compile([[
  S         <- Id List
  List      <- !.  /  Comma Id List
  Id        <- (Sp {[a-z]+})^ErrId
  Comma     <- (Sp ',')^ErrComma
  Sp        <- %s*]] ..
  "ErrId    <- ('' -> 'ErrId' => recorderror) " .. sync('","') .. "-> default\n" ..
  "ErrComma <-  ('' -> 'ErrComma' => recorderror) " .. sync('[a-z]+'),
  {default = "NONE", recorderror = recorderror})


function mymatch (g, s)
	errors = {}
	subject = s	
	io.write("Input: ", s, "\n")
	local r = { g:match(s) }
	io.write("Captures (separated by ';'): ")
	for k, v in pairs(r) do
		io.write(v .. "; ")
	end
	io.write("\nSyntactic errors found: " .. #errors)
	if #errors > 0 then
		io.write("\n")
		local out = {}
    for i, err in ipairs(errors) do
    	local msg = "Error at line " .. err.line .. " (col " .. err.col .. "): " .. err.msg
      table.insert(out,  msg)
    end
    io.write(table.concat(out, "\n"))
  end
	print("\n")
	return r
end
  
mymatch(g, "one,two")
mymatch(g, "one two three")
mymatch(g, "1,\n two, \n3,")
mymatch(g, "one\n two123, \nthree,")
