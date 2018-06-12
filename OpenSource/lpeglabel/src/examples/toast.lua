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
print(g:match("oast."))   --> nil  0  oast.
print(g:match("toward."))   --> nil  0  ward.

