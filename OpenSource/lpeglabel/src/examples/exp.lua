-- Matthew's recovery example 
lpeg = require'lpeglabel'

local R, S, P, V = lpeg.R, lpeg.S, lpeg.P, lpeg.V
local C, Cc, Ct, Cmt = lpeg.C, lpeg.Cc, lpeg.Ct, lpeg.Cmt
local T = lpeg.T

local labels = {
  NoExp = "no expression found",
  Extra = "extra characters found after the expression",
  ExpTerm = "expected a term after the operator",
  ExpExp = "expected an expression after the parenthesis",
  MisClose = "missing a closing ')' after the expression",
}

local errors = {}

local function expect(patt, labname)
  function recorderror(input, pos)
    table.insert(errors, {labname, pos})
    return true
  end
  return patt + Cmt("", recorderror) * T(labname)
end

local num = R("09")^1 / tonumber
local op = S("+-*/")

local function compute(tokens)
  local result = tokens[1]
  for i = 2, #tokens, 2 do
    if tokens[i] == '+' then
      result = result + tokens[i+1]
    elseif tokens[i] == '-' then
      result = result - tokens[i+1]
    elseif tokens[i] == '*' then
      result = result * tokens[i+1]
    elseif tokens[i] == '/' then
      result = result / tokens[i+1]
    else
      error('unknown operation: ' .. tokens[i])
    end
  end
  return result
end

local g = P {
  "Exp",
  Exp = Ct(V"Term" * (C(op) * V"Operand")^0) / compute;
  Operand = expect(V"Term", "ExpTerm");
  Term = num + V"Group";
  Group = "(" * V"InnerExp" * expect(")", "MisClose");
  InnerExp = expect(V"Exp", "ExpExp");
  ExpTerm = Cc(0);
  MisClose = P"";
  ExpExp = (P(1) - ")")^0 * Cc(0);
}

g = expect(g, "NoExp") * expect(-P(1), "Extra")

local function eval(input)
  local result, label, suffix = g:match(input)
  if #errors == 0 then
    return result
  else
    local out = {}
    for i, err in ipairs(errors) do
      local pos = err[2]
      local msg = labels[err[1]]
      table.insert(out, "syntax error: " .. msg .. " (at index " .. pos .. ")")
    end
    errors = {}
    return nil, table.concat(out, "\n")
  end
end

assert(eval "98-76*(54/32)" == 37.125)
--> 37.125

local e, msg = eval "(1+1-1*2/2"
assert(e == nil and msg == "syntax error: missing a closing ')' after the expression (at index 11)")

e, msg = eval "(1+)-1*(2/2)"
assert(e == nil and msg == "syntax error: expected a term after the operator (at index 4)")

e, msg = eval "(1+1)-1*(/2)"
assert(e == nil and msg == "syntax error: expected an expression after the parenthesis (at index 10)")

e, msg = eval "1+(1-(1*2))/2x"
assert(e == nil and msg == "syntax error: extra characters found after the expression (at index 14)")

e, msg = eval "-1+(1-(1*2))/2"
assert(e == nil and msg == "syntax error: no expression found (at index 1)")

e, msg = eval "(1+1-1*(2/2+)-():"
assert(e == nil and msg == "syntax error: expected a term after the operator (at index 13)\n" ..
                           "syntax error: expected an expression after the parenthesis (at index 16)\n" ..
                           "syntax error: missing a closing ')' after the expression (at index 17)\n" ..
                           "syntax error: extra characters found after the expression (at index 17)")

