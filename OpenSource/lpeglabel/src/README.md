<p align="center"><img src="https://github.com/sqmedeiros/lpeglabel/raw/master/lpeglabel-logo.png" alt="LPegLabel" width="150px"></p>

## LPegLabel - Parsing Expression Grammars (with Labels) for Lua 

---

### Introduction

LPegLabel is a conservative extension of the
[LPeg](http://www.inf.puc-rio.br/~roberto/lpeg)
library that provides an implementation of Parsing
Expression Grammars (PEGs) with labeled failures.
Labels can be used to signal different kinds of errors
and to specify which recovery pattern should handle a
given label. Labels can also be combined with the standard
patterns of LPeg. 

Besides that, LPegLabel also reports the farthest
failure position in case of an ordinary failure
(which is represented by label **fail**).

This document describes the new functions available
in LpegLabel and presents some examples of usage.

With labeled failures it is possible to distinguish
between an ordinary failure and an error. Usually, an
ordinary failure is produced when the matching of a
character fails, and this failure is caught by ordered choice.
An error (a non-ordinary failure), by its turn, is produced
by the throw operator and may be caught by a recovery rule. 

In LPegLabel, the result of an unsuccessful matching
is a triple **nil, lab, errpos**, where **lab**
is the label associated with the failure (a string or
an integer), and **errpos** is the input position being
matched when **lab** was thrown. 

When **lab** is an ordinary failure and no error was thrown before,
**errpos** is the farthest position where an ordinary failure occurred. 
In case **lab** is an ordinary failure and an error
was thrown before, **errpos** is the farthest input position
where an ordinary failure occurred after the last error. 
 
Below there is a brief summary of the new functions provided by LpegLabel: 

<table border="1">
<tbody><tr><td><b>Function</b></td><td><b>Description</b></td></tr>
<tr><td><a href="#f-t"><code>lpeglabel.T (l)</code></a></td>
  <td>Throws a label <code>l</code> to signal an error</td></tr>
<tr><td><a href="#re-t"><code>%{l}</code></a></td>
  <td>Syntax of <em>relabel</em> module. Equivalent to <code>lpeglabel.T(l)</code>
      </td></tr>
<tr><td><a href="#re-pow"><code>p^l</code></a></td>
  <td>Syntax sugar available at <em>relabel</em> for <code>p / %{l}</code>
      </td></tr>
<tr><td><a href="#re-line"><code>relabel.calcline(subject, i)</code></a></td>
  <td>Calculates line and column information regarding position <i>i</i> of the subject</code>
      </td></tr>
</tbody></table>


### Functions


#### <a name="f-t"></a><code>lpeglabel.T(l)</code>

Returns a pattern that throws the label `l`, which
can be an integer or a string.

When a label is thrown, the current subject position 
is used to set **errpos**, no matter whether it is the
fartherst failure position or not.

In case the PEG grammar has a rule `l`, after a label is thrown
this rule will be used as a recovery rule, otherwise the whole
matching fails.
 
The recovery rule will try to match the input from the subject
position where `l` was thrown. In case the matching of the recovery
rule succeeds, the regular matching is resumed. Otherwise, the
result of the recovery rule is the matching result.
 
When we have a predicate such as `-p` or `#p` and a label `l` is thrown
during the matching of `p`, this causes the failure of `p`, but does
not propagate `l`, or calls its associated recovery rule.


#### <a name="re-t"></a><code>%{l}</code>

Syntax of *relabel* module. Equivalent to `lpeg.T(l)`.

Label `l` must be a valid identifier name.

#### <a name="re-pow"></a><code>p^l</code>

Syntax of *relabel* module. The pattern `p^l` is equivalent
to `p / lpeglabel.T(l)`.

Label `l` must be a valid identifier name.


#### <a name="re-line"></a><code>relabel.calcline (subject, i)</code>

Returns line and column information regarding position <i>i</i> of the subject.



### Examples

Below there a few examples of usage of LPegLabel.
The code of these and of other examples is available
in the *examples* directory. 


#### Reporting the farthest failure

This example illustrates the new values returned
by the *match* function in case of an unsuccessful
matching. As no error is thrown in this example,
when the matching fails *errpos* represents the
farthest suffix where an ordinary failure occurred.

```lua
local m = require'lpeglabel'

function matchPrint(p, s)
  local r, lab, errpos = p:match(s)
  print("r: ", r, "lab: ", lab, "errpos: ", errpos)
end

local p = m.P"a"^0 * m.P"b" + m.P"c"
matchPrint(p, "abc")  --> r: 3    lab: nil   errpos: nil
matchPrint(p, "c")    --> r: 2    lab: nil   errpos: nil
matchPrint(p, "aac")  --> r: nil  lab: fail  errpos: 3
matchPrint(p, "xxc")  --> r: nil  lab: fail  errpos: 1
```


#### Matching a list of identifiers separated by commas

The following example defines a grammar that matches
a (possibly empty) list of identifiers separated by commas.
A label is thrown when there is no identifier after a comma,
or when the whole input is not matched.


```lua
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
  
print(mymatch(g, "one,two"))              --> 8
print(mymatch(g, "one two"))              --> nil  Error at line 1 (col 4): expecting EOF before ' two'
print(mymatch(g, "one,\n two,\nthree,4")) --> nil  Error at line 3 (col 7): expecting an identifier before '4'
```

In this example we could think about writing rule <em>List</em> as follows:
```lua
List = m.V'Id' * ((m.V'Comma' + m.T'ErrComma') * (m.V'Id' + m.T'ErrId'))^0,
```

but when matching <code>m.V'Comma' + m.T'ErrComma'</code> against the end of input
we would get a failure whose associated label would be **errComma**,
and this would cause the failure of the *whole* repetition.

Below we rewrite the previous grammar to indicate an error when there is no
comma after an identifer. Before tyring to match a comma, we check if
we have reached the end of input:

```lua
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
  
print(mymatch(g, "one,two"))               --> 8
print(mymatch(g, "one two"))               --> nil  Error at line 1 (col 4): expecting ',' before ' two'
print(mymatch(g, "one,\n two,\nthree,4"))  --> nil  Error at line 3 (col 7): expecting an identifier before '4'
print(mymatch(g, " 1,2"))                  --> nil  Error at line 1 (col 2): undefined before '1,2'
```


#### Error Recovery

We can specify a recovery rule that should
be matched when a label is thrown. After matching
the recovery rule, and possibly recording the error,
the parser will resume the <em>regular</em> matching.
The recovery rule must have the same name (or number)
of the label that was thrown. 


For example, in the example below we expect to match rule *A*,
but when a failure occur the label `Err` is thrown and then we
will try to match rule *Err*:
```lua
local m = require'lpeglabel'

local recp = m.P"oast"

local g = m.P{
  'S',
  S    =  m.V'A' * '.',
  A    =  m.P't' * (m.P'est' + m.T'Err'),
  Err  =  m.P'oast'
}

print(g:match("test."))   --> 6
print(g:match("toast."))  --> 7
print(g:match("oast."))   --> nil  fail  oast.
print(g:match("toward.")) --> nil  fail  ward.
```
When trying to match subject 'toast.', in rule *A* the first
't' is matched, then the matching of `m.P"est"` fails and label
`Err` is thrown, with the associated inpux suffix 'oast.'. 
The recovery rule *Err* successfully matches 'oast', so
the regular matching continues, and pattern `'.'` matches
the rest of the input.

When matching subject 'oast.', pattern `m.P"t"` fails, and
the result of the matching is <b>nil,  fail, 1</b>.

When matching 'toward.', label `Err` is thrown after matching 't',
with the associated input suffix 'oward.'. As the matching of the
recovery pattern fails, the result is <b>nil, fail, 3</b>.

Usually, the recovery pattern is an expression that does not fail.
In the previous example, we could have used `(m.P(1) - m.P".")^0`
as the recovery pattern.

Below we rewrite the grammar that describes a list of identifiers
to use a recovery strategy, with the help of some auxiliary functions.
Function `record`, plus function `recorderror`, will help
us to save the input position where a label was thrown,
while function `sync` will give us a synchronization pattern,
that consumes the input while it is not possible to match a given
pattern `p`.   

When the matching of an identifier fails, a defaul value ('NONE')
is provided.

```lua
local m = require'lpeglabel'
local re = require'relabel'

local terror = {
  ErrId     =  "expecting an identifier",
  ErrComma  =  "expecting ','",
  ErrList   =  "expecting a list of identifiers",
  fail      =  "undefined"
}

local subject, errors

function recorderror(pos, lab)
  local line, col = re.calcline(subject, pos)
  table.insert(errors, { line = line, col = col, msg = terror[lab] })
end

function record (lab)
  return (m.Cp() * m.Cc(lab)) / recorderror
end

function sync (p)
  return (-p * m.P(1))^0
end

function defaultValue ()
  return m.Cc"NONE" 
end

local id = m.R'az'^1

local g = m.P{
  "S",
  S         =  m.V"List" + (m.P(1) * m.T'ErrList'),
  List      =  m.V'Id' * (#m.P(1) * m.V'Comma' * (m.V'Id' + m.T'ErrId'))^0,
  Id        =  m.V'Sp' * m.C(id),
  Comma     =  m.V'Sp' * ',' + m.T'ErrComma',
  Sp        =  m.S' \n\t'^0,
  ErrId     =  record'ErrId' * sync(m.P",") * defaultValue(), 
  ErrComma  =  record'ErrComma' * sync(id),
  ErrList   =  record'ErrList' * sync(m.P(-1)) * defaultValue()  
}

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
--> Captures (separated by ';'): one; two;
--> Syntactic errors found: 0

mymatch(g, "one two three")           
--> Captures (separated by ';'): one; two; three;
--> Syntactic errors found: 2
--> Error at line 1 (col 4): expecting ','
--> Error at line 1 (col 8): expecting ','

mymatch(g, "1,\n two, \n3,")          
--> Captures (separated by ';'): NONE; 
--> Syntactic errors found: 1
--> Error at line 1 (col 2): expecting a list of identifiers

mymatch(g, "one\n two123, \nthree,")
--> Captures (separated by ';'): one; two; three; NONE; 
--> Syntactic errors found: 3
--> Error at line 2 (col 1): expecting ','
--> Error at line 2 (col 5): expecting ','
--> Error at line 3 (col 6): expecting an identifier

```

##### *relabel* syntax

Below we write a grammar for a simple programming language 
using the syntax supported by *relabel*, where `%{}` is the throw
operator, and the syntax `p^l` is syntatic sugar for
`p / %{l}` (given that *l* is a valid identifier name):

```lua
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
print(mymatch(g, s))            --> nil  Error at line 6 (col 1): Missing ';' in CmdSeq

print(mymatch(g, "a : 2"))      --> nil  Error at line 1 (col 2): Error matching ':='
print(mymatch(g, "a := 2; 6"))  --> nil  Error at line 1 (col 8): Error, expecting EOF
```

### Caveats

Does not use the number **1** to specify a recovery rule,
since that the entry with index 1 in the grammar table defines
its initial rule.

In case your grammar has many regular and recovery rules,
you may get an error message such as grammar: <em>has too many rules</em>.
In this case, we need to change *MAXRULES* in `lptypes.h`.
  
