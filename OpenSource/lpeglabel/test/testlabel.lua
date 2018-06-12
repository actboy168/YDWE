local m = require 'lpeglabel'

local p, r, l, s, poserr

local function checklabeq (x, ...)
  y = { ... }
  assert(type(x) == "table")
  assert(#x == #y)
  if x[2] == 0 then -- 0 -> 'fail'
    x[2] = 'fail'  
	end
  for i = 1, 3 do
    assert(x[i] == y[i])
  end
end

local function checkeq (x, y, p)
  if p then print(x,y) end
  if type(x) ~= "table" then assert(x == y)
  else
    for k,v in pairs(x) do checkeq(v, y[k], p) end
    for k,v in pairs(y) do checkeq(v, x[k], p) end
  end
end


-- tests related to reporting the farthest failure position
-- when a label is not thrown

p = m.P"a"^0 * m.P"b" + m.P"c"
checklabeq({4, nil, nil}, p:match("aabk"))
checklabeq({2, nil, nil}, p:match("ck"))
checklabeq({nil, 0, 1}, p:match("dk"))
checklabeq({nil, 0, 3}, p:match("aak"))

p = (m.P"a" + m.P"c")^0 * m.P"b" + m.P"c"
checklabeq({4, nil, nil}, p:match("aabk"))
checklabeq({2, nil, nil}, p:match("ck"))
checklabeq({nil, 0, 1}, p:match("dk"))
checklabeq({nil, 0, 3}, p:match("aak"))

p = m.P"a"^0 * m.P"b" + m.P(1)^0 * m.P(1)
checklabeq({4, nil, nil}, p:match("aabk"))
checklabeq({nil, 0, 3}, p:match("ck"))
checklabeq({nil, 0, 4}, p:match("aak"))

p = m.P(1) * m.P"a" + m.P"c" 
checklabeq({3, nil, nil}, p:match("bac"))
checklabeq({2, nil, nil}, p:match("c"))
checklabeq({nil, 0, 2}, p:match("x"))
checklabeq({nil, 0, 2}, p:match("kx"))

p = m.P"a"^0 * m.P(1) * m.P(1) + m.P"a"^0 * m.P"c"
checklabeq({5, nil, nil}, p:match("aabc"))
checklabeq({4, nil, nil}, p:match("aac"))
checklabeq({nil, 0, 4}, p:match("aak"))
checklabeq({nil, 0, 2}, p:match("x"))

p = m.P"a"^0 * m.P(1) * m.P(1) + m.P"a"^0 * m.P"c"
checklabeq({5, nil, nil}, p:match("aabc"))
checklabeq({4, nil, nil}, p:match("aac"))
checklabeq({nil, 0, 4}, p:match("aak"))
checklabeq({nil, 0, 2}, p:match("x"))

p = m.Cmt(m.P"a"^0, function() return nil end) + m.P"x"
checklabeq({2, nil, nil}, p:match("xabc"))
checklabeq({nil, 0, 3}, p:match("aac"))
checklabeq({nil, 0, 1}, p:match("kx"))

p = m.P"b" * -m.P"a" + m.P"c"
checklabeq({nil, 0, 2}, p:match("ba"))
checklabeq({nil, 0, 1}, p:match("kx"))

p = (m.P"c" + m.P"a") * m.P("b" + m.P"d") + m.P"xxx"
checklabeq({nil, 0, 1}, p:match("kk"))
checklabeq({nil, 0, 2}, p:match("ak"))
checklabeq({nil, 0, 3}, p:match("xxy"))
checklabeq({nil, 0, 2}, p:match("xyz"))

print"+"


-- throws a label 
p = m.T(1)
s = "abc"
r, l, poserr = p:match(s) 
assert(r == nil and l == 1 and poserr == 1)

-- throws a label, choice does not catch labels
p = m.T(1) + m.P"a"
r, l, poserr = p:match(s)
assert(r == nil and l == 1 and poserr == 1)

-- again throws a label that is not caught by choice
local g = m.P{
  "S",
  S = m.V"A" + m.V"B",
  A = m.T(1),
  B = m.P"a"
}
r, l, poserr = g:match(s)
assert(r == nil and l == 1 and poserr == 1)

-- throws a label in a position that is not the farthest one
-- but it is the position that should be reported
p = m.P(1) * m.P"a" + m.T(11) 
checklabeq({3, nil, nil}, p:match("bac"))
checklabeq({nil, 11, 1}, p:match("c"))
checklabeq({nil, 11, 1}, p:match("x"))
checklabeq({nil, 11, 1}, p:match("kx"))


-- throws a label without a corresponding recovery rule
p = m.P{
  "S",
  S = m.T("bola"),
  bolada = m.P"a"
} 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'bola' and poserr == 1)

-- throws a label without a corresponding recovery rule
-- T(2) indexes key ["2"]
p = m.P{
  "S",
  S = m.T(2),
  [2] = m.P"a"
} 
r, l, poserr = p:match("abc")
assert(r == nil and l == 2 and poserr == 1)

-- throws a label with a corresponding recovery rule
p = m.P{
  "S",
  S = m.T("bola"),
  bola = m.P"a"
} 
r, l, poserr = p:match("abc")
assert(r == 2)

-- throws a label with a corresponding recovery rule
-- T(2) indexes key ["2"]
p = m.P{
  "S",
  S = m.T(2),
  ["2"] = m.P"a"^0
} 
r, l, poserr = p:match("aaabc")
assert(r == 4)

-- regular failure after the recovery
p = m.P{
  "S",
  S = m.T(2),
  ["2"] = m.P"a"^0 * m.P"c"
} 
r, l, poserr = p:match("aaabc")
assert(r == nil and l == 'fail' and poserr == 4)

s = "bola"
r, l, poserr = p:match("bola")
assert(r == nil and l == 'fail' and poserr == 1)

-- Recovery rules do not catch "fail" by default
p = m.P{
  "S",
  S = m.P"b" * m.T(2),
  ["2"] = m.P"a"^0
} 
r, l, poserr = p:match("c")
assert(r == nil and l == 'fail' and poserr == 1)

assert(p:match("baa") == 4)


-- recovery rules for "2" or "3"
-- when we use V instead of T, a recovery
-- rule becomes a regular grammar rule
p = m.P{
  "S",
  S = (m.P"a" + m.T(2)) * m.T(3),
  ["3"] = m.V"2",
  ["2"] = m.P"a" + m.P"b",
} 

assert(p:match("aac") == 3)
assert(p:match("abc") == 3)
r, l, poserr = p:match("acc")
assert(r == nil and l == 'fail' and poserr == 2)

--throws 2, recovery rule matches 'b', throw 3, and rec rule matches 'a'
assert(p:match("bac") == 3)

r, l, poserr = p:match("cab")
assert(r == nil and l == 'fail' and poserr == 1)

-- tests related to predicates
p = #m.T(1)
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 1)

p = #m.T(1) + m.P"a"
r, l, poserr = p:match("abc")
assert(r == 2)

p = #m.T(1) * m.P"a"
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 1)

p = ##m.T(1) + m.P"a"
r, l, poserr = p:match("abc")
assert(r == 2)

p = -#m.T(1) + m.P"a"
r, l, poserr = p:match("abc")
assert(r == 1)

p = -m.T(1) * m.P"a"
r, l, poserr = p:match("abc")
assert(r == 2)

p = -m.T(1)
r, l, poserr = p:match("bbc")
assert(r == 1)

p = -#m.T(1)
r, l, poserr = p:match("bbc")
assert(r == 1)

p = -(-m.P'a')
r, l, poserr = p:match("abc")
assert(r == 1)

p = -(-m.P'a')
r, l, poserr = p:match("bbc")
assert(r == nil and l == 'fail' and poserr == 1)

p = #(m.P"a" * m.T(1))
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)

p = #(m.P"a" * m.P'a')
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)

p = m.P'a' * #m.P('a') 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)

p = m.P'a' * #m.P('ba') 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 3)

p = m.P'a' * -m.P('b') 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)

p = m.P'a' * -m.P('bc')
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 4)

p = m.P'a' * #m.T('a') 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)

p = m.P'a' * #(m.P'b' * m.T('a')) 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 3)

p = m.P'a' * (m.P'b' * m.T('a')) 
r, l, poserr = p:match("abc")
assert(r == nil and l == 'a' and poserr == 3)

p = -(-m.T(1))
r, l, poserr = p:match("bbc")
assert(r == nil and l == 'fail' and poserr == 1)

p = m.P{
  "S",
  S = -m.T(22),
  ["22"] = m.P"a" 
} 
r, l, poserr = p:match("abc")
assert(r == 1)

assert(p:match("bbc") == 1)

p = m.P{
  "S",
  S = #m.T(22),
  ["22"] = m.P"a" 
}
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 1)

p = m.P{
  "S",
  S = m.T(22),
  ["22"] = #m.P"a" 
}
assert(p:match("abc") == 1)

p = m.P{
  "S",
  S = #m.T(22),
  ["22"] = m.P"a" 
}
r, l, poserr = p:match("bbc")
assert(r == nil and l == 'fail' and poserr == 1)

p = m.P{
  "S",
  S = #m.P"a" * m.T(22),
  ["22"] = m.T(15) 
}
r, l, poserr = p:match("abc")
assert(r == nil and l == 15 and poserr == 1)

p = m.P{
  "S",
  S = #(m.P"a" * m.T(22)),
  ["22"] = m.T(15) 
}
r, l, poserr = p:match("abc")
assert(r == nil and l == 'fail' and poserr == 2)


-- tests related to repetition
p = m.T(1)^0
r, l, poserr = p:match("ab")
assert(r == nil and l == 1 and poserr == 1)

p = (m.P"a" + m.T(1))^0
r, l, poserr = p:match("aa")
assert(r == nil and l == 1 and poserr == 3)


-- Bug reported by Matthew Allen
-- some optmizations performed by LPeg should not be
-- applied in case of labels

-- recovery operator
p = m.P{
  "S",
  S = m.P"A" + m.P"B",
  ["2"] = m.P(true) 
} 

assert(p:match("B") == 2)

p = m.P{
  "S",
  S = m.P"A" + m.P"B",
  ["2"] = m.P(false) 
} 
assert(p:match("B") == 2)


--[[
S -> A //{1} 'a'
A -> B
B -> %1
]]
g = m.P{
  "S",
  S = m.V"A",
  A = m.V"B",
  B = m.T(2),
  ["2"] = m.P'a'
}
assert(g:match("ab") == 2)
r, l, poserr = g:match("bc")
assert(r == nil and l == 'fail' and poserr == 1)


--[[
S -> A 
A -> (B (';' / %{2}))*
B -> 'a'
]]
g = m.P{
  "S",
  S = m.V"A",
  A = m.P(m.V"B" * (";" + m.T(2)))^0,
  B = m.P'a',
}
assert(g:match("a;a;") == 5)

r, l, poserr = g:match("a;a")
assert(r == nil and l == 2 and poserr == 4)


p = m.P{
  "S",
  S = m.T'A',
  A = m.T'B',
  B = m.P'a'
}
assert(p:match("abc") == 2)

r, l, poserr = p:match("")
assert(r == nil and l == 'fail' and poserr == 1)


-- Infinte Loop TODO: check the semantics
p = m.P{
  "S",
  S = m.T"A",
  A = m.T"S",
}
--r, l, poserr = p:match("ab")
--assert(r == nil and l == 1 and poserr == "ab")

p = m.P{
  "S",
  S = m.T"A",
  A = m.P'a' * (-m.P(1) + m.T"A"),
}
r, l, poserr = p:match("ab")
assert(r == nil and l == 'fail' and poserr == 2)

r, l, poserr = p:match("cd")
assert(r == nil and l == 'fail' and poserr == 1)

p = m.P{
  "S",
  S = m.T"A",
  A = m.P(1) * (-m.P(1) + m.T"A"),
}
assert(p:match("abc") == 4)


print("+")

--[[ grammar based on Figure 8 of paper submitted to SCP
S  -> S0 //{1} ID //{2} ID '=' Exp //{3} 'unsigned'* 'int' ID //{4} 'unsigned'* ID ID / %error
S0 -> S1 / S2 / &'int' %3
S1 -> &(ID '=') %2  /  &(ID !.) %1  /  &ID %4
S2 -> &('unsigned'+ ID) %4  /  & ('unsigned'+ 'int') %3 
]]
local sp = m.S" \t\n"^0
local eq = sp * m.P"="

g = m.P{
  "S",
  S = m.V"S0" + m.T'L5', -- error
  S0 = m.V"S1"  +  m.V"S2"  +  #m.V"I" * m.T'L3',
  S1 = #(m.V"ID" * eq) * m.T'L2' + sp * #(m.V"ID" * -m.P(1)) * m.T'L1' + #m.V"ID" * m.T'L4',
  S2 = #(m.V"U"^1 * m.V"ID") * m.T'L4'  +  #(m.V"U"^1 * m.V"I") * m.T'L3',
  ID = sp * m.P"a",
  U = sp * m.P"unsigned",
  I = sp * m.P"int",
  Exp = sp * m.P"E",
  L1 = m.V"ID",
  L2 = m.V"ID" * eq * m.V"Exp",
  L3 = m.V"U"^0 * m.V"I" * m.V"ID",
  L4 = m.V"U"^0 * m.V"ID" * m.V"ID", 
}

local s = "a"
assert(g:match(s) == #s + 1) --1
s = "a = E"
assert(g:match(s) == #s + 1) --2
s = "int a"
assert(g:match(s) == #s + 1) --3
s = "unsigned int a"
assert(g:match(s) == #s + 1) --3
s = "unsigned a a"
assert(g:match(s) == #s + 1) --4

s = "b" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)

s = "unsigned" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)

s = "unsigned a" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)

s = "unsigned int" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)


print("+")


local re = require 'relabel'

g = re.compile[['a' / %{l1}
]]
assert(g:match("a") == 2)
r, l, poserr = g:match("b")
assert(r == nil and l == 'l1' and poserr == 1)

g = re.compile[['a'^l1
]]
assert(g:match("a") == 2)
r, l, poserr = g:match("b")
assert(r == nil and l == 'l1' and poserr == 1)

g = re.compile[[A <- 'a'^B
                B <- [a-f]^C
                C <- [a-z]
]]
assert(g:match("a") == 2)
assert(g:match("a") == 2)
assert(g:match("f") == 2)
assert(g:match("g") == 2)
assert(g:match("z") == 2)
r, l, poserr = g:match("A")
assert(r == nil and l == 'fail' and poserr == 1)

g = re.compile[[A <- %{C}
                B <- [a-z]
]]
r, l, poserr = g:match("a")
assert(r == nil and l == 'C' and poserr == 1)

g = re.compile[[A <- %{B}
                B <- [a-z]
]]
r, l, poserr = g:match("a")
assert(g:match("a") == 2)
r, l, poserr = g:match("U")
assert(r == nil and l == 'fail' and poserr == 1)


g = re.compile[[A <- [a-f] %{B}
                B <- [a-c] %{C}
                C <- [a-z]
]]

r, l, poserr = g:match("a")
assert(r == nil and l == 'fail' and poserr == 2)
r, l, poserr = g:match("aa")
assert(r == nil and l == 'fail' and poserr == 3)
assert(g:match("aaa") == 4)

r, l, poserr = g:match("ad")
assert(r == nil and l == 'fail' and poserr == 2)

r, l, poserr = g:match("g")
assert(r == nil and l == 'fail' and poserr == 1)


--[[ grammar based on Figure 8 of paper submitted to SCP (using the recovery operator)
S  -> S0 //{1} ID //{2} ID '=' Exp //{3} 'unsigned'* 'int' ID //{4} 'unsigned'* ID ID / %error
S0 -> S1 / S2 / &'int' %3
S1 -> &(ID '=') %2  /  &(ID !.) %1  /  &ID %4
S2 -> &('unsigned'+ ID) %4  /  & ('unsigned'+ 'int') %3 
]]

g = re.compile([[
  S <- S0 / %{L5}
  S0 <- S1 / S2 / &Int %{L3}
  S1 <- &(ID %s* '=') %{L2} / &(ID !.) %{L1} / &ID %{L4}
  S2 <- &(U+ ID) %{L4} / &(U+ Int) %{L3}
  ID <- %s* 'a' 
  U <- %s* 'unsigned'
  Int <- %s* 'int'
  Exp <- %s* 'E'
  L1 <- ID
  L2 <- ID %s* '=' Exp
  L3 <- U* Int ID
  L4 <- U ID ID
]])

local s = "a"
assert(g:match(s) == #s + 1) --1
s = "a = E"
assert(g:match(s) == #s + 1) --2
s = "int a"
assert(g:match(s) == #s + 1) --3
s = "unsigned int a"
assert(g:match(s) == #s + 1) --3
s = "unsigned a a"
assert(g:match(s) == #s + 1) --4
s = "b" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)
s = "unsigned" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)
s = "unsigned a" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)
s = "unsigned int" 
r, l, poserr = g:match(s)
assert(r == nil and l == 'L5' and poserr == 1)



local terror = { cmdSeq = "Missing ';' in CmdSeq",
                 ifExp = "Error in expresion of 'if'",
                 ifThen = "Error matching 'then' keyword",
                 ifThenCmdSeq = "Error matching CmdSeq of 'then' branch",
                 ifElseCmdSeq = "Error matching CmdSeq of 'else' branch",
                 ifEnd = "Error matching 'end' keyword of 'if'",
                 repeatCmdSeq = "Error matching CmdSeq of 'repeat'",
                 repeatUntil = "Error matching 'until' keyword",
                 repeatExp = "Error matching expression of 'until'",
                 assignOp = "Error matching ':='",
                 assignExp = "Error matching expression of assignment",
                 readName = "Error matching 'NAME' after 'read'",
                 writeExp = "Error matching expression after 'write'",
                 simpleExp = "Error matching 'SimpleExp'",
                 term = "Error matching 'Term'",
                 factor = "Error matching 'Factor'",
                 openParExp = "Error matching expression after '('",
                 closePar = "Error matching ')'",
                 undefined = "Undefined Error"}

g = re.compile([[
  Tiny       <- CmdSeq 
  CmdSeq     <- (Cmd SEMICOLON^cmdSeq) (Cmd SEMICOLON^cmdSeq)*
  Cmd        <- IfCmd / RepeatCmd / ReadCmd / WriteCmd  / AssignCmd 
  IfCmd      <- IF  Exp^ifExp  THEN^ifThen  CmdSeq^ifThenCmdSeq  (ELSE CmdSeq^ifElseCmdSeq / '')  END^ifEnd
  RepeatCmd  <- REPEAT  CmdSeq^repeatCmdSeq  UNTIL^repeatUntil  Exp^repeatExp
  AssignCmd  <- NAME  ASSIGNMENT^assignOp  Exp^assignExp
  ReadCmd    <- READ  NAME^readName
  WriteCmd   <- WRITE  Exp^writeExp
  Exp        <- SimpleExp  ((LESS / EQUAL)  SimpleExp^simpleExp / '')
  SimpleExp  <- Term  ((ADD / SUB)  Term^term)*
  Term       <- Factor ((MUL / DIV)  Factor^factor)*
  Factor     <- OPENPAR  Exp^openParExp  CLOSEPAR^closePar  / NUMBER  / NAME
  ADD        <- Sp '+'
  ASSIGNMENT <- Sp ':='
  CLOSEPAR   <- Sp ')'
  DIV        <- Sp '/'
  IF         <- Sp 'if'
  ELSE       <- Sp 'else'
  END        <- Sp 'end'
  EQUAL      <- Sp '='
  LESS       <- Sp '<'
  MUL        <- Sp '*'
  NAME       <- !RESERVED Sp [a-z]+
  NUMBER     <- Sp [0-9]+
  OPENPAR    <- Sp '('
  READ       <- Sp 'read'
  REPEAT     <- Sp 'repeat'
  SEMICOLON  <- Sp ';'
  SUB        <- Sp '-'
  THEN       <- Sp 'then'
  UNTIL      <- Sp 'until'
  WRITE      <- Sp 'write'
  RESERVED   <- (IF / ELSE / END / READ / REPEAT / THEN / UNTIL / WRITE) ![a-z]+
  Sp         <- (%s / %nl)*  
]], terror)

s = [[
n := 5;]]
assert(g:match(s) == #s + 1) 

s = [[
n := 5;
f := 1;
repeat
  f := f * n;
  n := n - 1;
until (n < 1);
write f;]]
assert(g:match(s) == #s + 1) 

-- a ';' is missing in 'read a' 
s = [[
read a]]
r, l, poserr = g:match(s)
assert(l == 'cmdSeq') 


-- a ';' is missing in 'n := n - 1' 
s = [[
n := 5;
f := 1;
repeat
  f := f * n;
  n := n - 1
until (n < 1);
write f;]]
r, l, poserr = g:match(s)
assert(l == 'cmdSeq') 


-- IF expression 
s = [[
if a then a := a + 1; end;]]
assert(g:match(s) == #s + 1) 


-- IF expression 
s = [[
if a then a := a + 1; else write 2; end;]]
assert(g:match(s) == #s + 1) 

-- Error in expression of 'if'. 'A' is not a valida name
s = [[
if A then a := a + 1; else write 2; end;]]
r, l, poserr = g:match(s)
assert(l == 'ifExp') 

-- Error matching the 'then' keyword
s = [[
if a a := a + 1; else write 2; end;]]
r, l, poserr = g:match(s)
assert(l == 'ifThen') 

-- Error matching the CmdSeq inside of 'then' branch 
s = [[
if a then 3 := 2; else write 2; end;]]
r, l, poserr = g:match(s)
assert(l == 'ifThenCmdSeq') 

-- Error matching the CmdSeq inside of 'else' branch 
s = [[
if a then b := 2; else A := 2; end;]]
r, l, poserr = g:match(s)
assert(l == 'ifElseCmdSeq') 

-- Error matching 'end' of 'if' 
s = [[
if a then b := 2; else a := 2; 77;]]
r, l, poserr = g:match(s)
assert(l == 'ifEnd') 

-- Error matching the CmdSeq of 'repeat'
s = [[repeat
  F := f * n;
  n := n - 1;
until (n < 1);]]
r, l, poserr = g:match(s)
assert(l == 'repeatCmdSeq') 

-- Error matching 'until'
s = [[repeat
  f := f * n;
  n := n - 1;
88 (n < 1);]]
r, l, poserr = g:match(s)
assert(l == 'repeatUntil') 

-- Error matching expression of 'until'
s = [[repeat
  f := f * n;
  n := n - 1;
until ; (n < 1);]]
r, l, poserr = g:match(s)
assert(l == 'repeatExp') 

-- Error matching ':='
s = [[
f = f * n;]]
r, l, poserr = g:match(s)
assert(l == 'assignOp') 

-- Error matching expression of assignment
s = [[
f := A * n;]]
r, l, poserr = g:match(s)
assert(l == 'assignExp') 

-- Error matching 'name'
s = [[
read 2;]]
r, l, poserr = g:match(s)
assert(l == 'readName') 

-- Error matching expression after 'write'
s = [[
write [a] := 2;]]
r, l, poserr = g:match(s)
assert(l == 'writeExp') 

-- Error matching 'SimpleExp'
s = [[
a := a < A;]]
r, l, poserr = g:match(s)
assert(l == 'simpleExp') 

-- Error matching 'Term'
s = [[
a := a + A;]]
r, l, poserr = g:match(s)
assert(l == 'term') 

-- Error matching 'Factor'
s = [[
a := a * A;]]
r, l, poserr = g:match(s)
assert(l == 'factor') 

-- Error matching expression after '('
s = [[
a := (A);]]
r, l, poserr = g:match(s)
assert(l == 'openParExp') 

-- Error matching ')'
s = [[
a := (a];]]
r, l, poserr = g:match(s)
assert(l == 'closePar') 

-- Error undefined
s = [[
A := a;]]
r, l, poserr = g:match(s)
assert(l == 'fail') 


print("+")


p = m.P{
  "A",
  A = m.P"a",
  B = m.P"b" 
}
assert(p:match("a") == 2)
checklabeq({nil, 0, 1}, p:match("b"))
checklabeq({nil, 0, 1}, p:match("c"))

p = m.P{
  "A",
  A = m.T"B",
  B = m.P"b" 
}
checklabeq({nil, 'fail', 1}, p:match("a"))
assert(p:match("b") == 2)

--[[
S -> (A //{128} (!c .)*) C
A -> a*b / %128
C -> c+
]]
g = m.P{
  "S",
  S = m.V"A" * m.V"C",
  A = m.P"a"^0 * "b" + m.T'Rec',
  C = m.P"c"^1,
  Rec = (-m.P"c" * m.P(1))^0
}

assert(g:match("abc") == 4)
assert(g:match("aabc") == 5)
assert(g:match("aadc") == 5)  
assert(g:match("dc") == 3)
checklabeq({nil, 0, 2}, g:match("bbc"))
assert(g:match("xxc") == 4) 
assert(g:match("c") == 2)
checklabeq({nil, 0, 5}, g:match("fail"))
checklabeq({nil, 0, 5}, g:match("aaxx"))


--[[
S -> (A //{99} (!c .)*) C
A -> a+ (b / ^99) 
C -> c+
]]
g = m.P{
  "S",
  S = m.V"A" * m.V"C",
  A = m.P"a"^1 * ("b" + m.T'Rec'),
  C = m.P"c"^1,
  Rec = (-m.P"c" * m.P(1))^0
}

assert(g:match("abc") == 4)
assert(g:match("aabc") == 5)
assert(g:match("aadc") == 5)
checklabeq({nil, 0, 1}, g:match("bc"))
checklabeq({nil, 0, 1}, g:match("bbc"))
checklabeq({nil, 0, 3}, g:match("abb"))
checklabeq({nil, 0, 4}, g:match("axx"))
assert(g:match("accc") == 5)
assert(g:match("axxc") == 5)
checklabeq({nil, 0, 1}, g:match("c"))
checklabeq({nil, 0, 1}, g:match("fail"))



-- Matthew's recovery example 
lpeg = m

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


print("+")

local g1 = {
  "S1",
  S1 = V"End" + V'A' * V'S1',
  A = P'a' + T'Ea',
  End = P"." * (-P(1) + T'Eend'),
}

local p1 = m.P(g1)

assert(p1:match("a.") == 3)
assert(p1:match("aa.") == 4)
assert(p1:match(".") == 2)
checklabeq({nil, 'Ea', 1}, p1:match("ba."))
checklabeq({nil, 'Ea', 2}, p1:match("aba."))
checklabeq({nil, 'Ea', 1}, p1:match("cba."))
checklabeq({nil, 'Eend', 3}, p1:match("a.a"))

local g2 = {
  "S2",
  S2 = m.V"S1",
  Ea = P'b'^1 + T'Eb',
}

g1[1] = nil -- start rule
for k, v in pairs(g1) do
  g2[k] = v
end

local p2 = m.P(g2)

assert(p2:match("a.") == 3)
assert(p2:match("aa.") == 4)
assert(p2:match(".") == 2)
assert(p2:match("ba.") == 4)
assert(p2:match("aba.") == 5)
checklabeq({nil, 'Eb', 1}, p2:match("cba."))
checklabeq({nil, 'Eend', 3}, p2:match("a.a"))

local g3 = {
  "S3",
  S3 = V'S2',
  Eb = P'c'^1 + T'Ec',
  Eend = V'Eb',
}
g2[1] = nil -- start rule
for k, v in pairs(g2) do
  g3[k] = v
end

local p3 = m.P(g3)

assert(p3:match("a.") == 3)
assert(p3:match("aa.") == 4)
assert(p3:match(".") == 2)
assert(p3:match("ba.") == 4)
assert(p3:match("aba.") == 5)
assert(p3:match("cba.") == 5)
checklabeq({nil, 'Ec', 3}, p3:match("a.a"))
checklabeq({nil, 'Ec', 1}, p3:match("dc"))
checklabeq({nil, 'Ec', 2}, p3:match(".d"))


-- testing more captures
local g = re.compile[[
  S <- ( %s* &. {A} )* 
  A <- [0-9]+ / %{5}
]]

checkeq({"523", "624", "346", "888"} , {g:match("523 624  346\n888")}) 
checkeq({nil, 5, 4}, {g:match("44 a 123")})

local g2 = re.compile[[
  S <- ( %s* &. {A} )*  
  A <- [0-9]+ / %{Rec}
  Rec <- ((![0-9] .)*) -> "58"
]]

checkeq({"523", "624", "346", "888"} , {g2:match("523 624  346\n888")}) 
checkeq({"44", "a ", "58", "123"}, {g2:match("44 a 123")})


local g = re.compile[[
  S <- ( %s* &. A )* 
  A <- {[0-9]+} / %{5}
]]

checkeq({"523", "624", "346", "888"} , {g:match("523 624  346\n888")}) 
checkeq({nil, 5, 4}, {g:match("44 a 123")})

local g2 = re.compile[[
  S <- ( %s* &. A )* 
  A <- {[0-9]+} / %{Rec}
  Rec <- ((![0-9] .)*) -> "58"
]] 

checkeq({"523", "624", "346", "888"} , {g2:match("523 624  346\n888")}) 
checkeq({"44", "58", "123"}, {g2:match("44 a 123")})


local R, S, P, V = lpeg.R, lpeg.S, lpeg.P, lpeg.V
local C, Cc, Ct, Cmt = lpeg.C, lpeg.Cc, lpeg.Ct, lpeg.Cmt
local T = lpeg.T 

local labels = {
  {"NoExp",     "no expression found"},
  {"Extra",     "extra characters found after the expression"},
  {"ExpTerm",   "expected a term after the operator"},
  {"ExpExp",    "expected an expression after the parenthesis"},
  {"MisClose",  "missing a closing ')' after the expression"},
}

local errors = {}

local function expect(patt, labname, recpatt)
  function recorderror(input, pos)
    table.insert(errors, {labname, pos})
    return true
  end
  if not recpatt then recpatt = P"" end
  --return Rec(patt, Cmt("", recorderror) * recpatt)
  return patt + T(labname)
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
  Exp = Ct(V"Term" * (C(op) * V"Operand")^0) / compute,
  Operand = expect(V"Term", "ExpTerm"),
  Term = num,
  ExpTerm = Cc(3)
}
                 
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

assert(eval("98-76*54/32") == 37.125)
--> 37.125

assert(eval("1+") == 4)
--> syntax error: expected a term after the operator (at index 3)

print("OK")
