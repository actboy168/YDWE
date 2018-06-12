local re = require 'relabel'

local terror = {
  cmdSeq        =  "Missing ';' in CmdSeq",
  ifExp         =  "Error in expresion of 'if'",
  ifThen        =  "Error matching 'then' keyword",
  ifThenCmdSeq  =  "Error matching CmdSeq of 'then' branch",
  ifElseCmdSeq  =  "Error matching CmdSeq of 'else' branch",
  ifEnd         =  "Error matching 'end' keyword of 'if'",
  repeatCmdSeq  =  "Error matching CmdSeq of 'repeat'",
  repeatUntil   =  "Error matching 'until' keyword",
  repeatExp     =  "Error matching expression of 'until'",
  assignOp      =  "Error matching ':='",
  assignExp     =  "Error matching expression of assignment",
  readName      =  "Error matching 'NAME' after 'read'",
  writeExp      =  "Error matching expression after 'write'",
  simpleExp     =  "Error matching 'SimpleExp'",
  term          =  "Error matching 'Term'",
  factor        =  "Error matching 'Factor'",
  openParExp    =  "Error matching expression after '('",
  closePar      =  "Error matching ')'",
  eof           =  "Error, expecting EOF",
  undefined     =  "Undefined Error"
}

g = re.compile([[
  Tiny       <- CmdSeq (!. / %{eof})
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


local function mymatch(g, s)
	local r, e, pos = g:match(s)
  if not r then
    local line, col = re.calcline(s, pos)
    local msg = "Error at line " .. line .. " (col " .. col .. "): "
		return r, msg .. terror[e]
	end 
	return r
end

local s = [[
n := 5;
f := 1;
repeat
  f := f + n;
  n := n - 1
until (n < 1);
write f;]]
print(mymatch(g, s))

s = [[
n := 5;
f := 1;
repeat
  f := f + n;
  n := n - 1;
until (n < 1);
read ;]]
print(mymatch(g, s))

s = [[
if a < 1 then
  b := 2;
else
  b := 3;]]
print(mymatch(g, s))

s = [[
n := 5;
f := 1;
repeat
  f := f + n;
  n := n - 1;
untill (n < 1);
]]
print(mymatch(g, s))

s = [[
n := 5;
f := 1;
repeat
  f := f + n;
  n := n - 1;
3 (n < 1);
]]
print(mymatch(g, s))

print(mymatch(g, "a : 2"))
print(mymatch(g, "a := 2; 6"))

