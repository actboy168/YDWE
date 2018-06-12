local m = require'lpeglabel'

function matchPrint(p, s)
	local r, lab, errpos = p:match(s)
	print("r: ", r, "lab: ", lab, "errpos: ", errpos)
end

function getSub (s, i)
	if i then return s:sub(i) else return nil end
end

function matchPrint2(p, s)
	local r, lab, ifail = p:match(s)
	print("r: ", r, "lab: ", lab, "sfail: ", getSub(s, ifail))
end

local p = m.P"a"^0 * m.P"b" + m.P"c"
matchPrint(p, "abc")  --> r: 3	  lab: nil   errpos: nil
matchPrint(p, "c")    --> r: 2	  lab: nil   errpos: nil
matchPrint(p, "aac")  --> r: nil	lab: fail  errpos: 3
matchPrint(p, "xxc")  --> r: nil	lab: fail  errpos: 1


matchPrint2(p, "abc")  --> r: 3	   lab: nil   sfail: nil
matchPrint2(p, "c")    --> r: 2	   lab: nil   sfail: nil
matchPrint2(p, "aac")  --> r: nil	 lab: fail  sfail: c
matchPrint2(p, "xxc")  --> r: nil	 lab: fail  sfail: xxc
