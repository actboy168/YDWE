package = "LPegLabel"
version = "1.0.0-1"
source = {
   url = "https://github.com/sqmedeiros/lpeglabel/archive/v1.0.0-1.tar.gz",
   tag = "v1.0.0-1",
   dir = "lpeglabel-1.0.0-1",
}
description = {
   summary = "Parsing Expression Grammars For Lua with Labeled Failures",
   detailed = [[
     LPegLabel is a conservative extension of the LPeg library that provides
     an implementation of Parsing Expression Grammars (PEGs) with labeled failures. 
     Labels can be used to signal different kinds of erros and to specify which
     alternative in a labeled ordered choice should handle a given label.
     Labels can also be combined with the standard patterns of LPeg.  
   ]],
   homepage = "https://github.com/sqmedeiros/lpeglabel/",
   maintainer = "Sergio Medeiros <sqmedeiros@gmail.com>",
   license = "MIT/X11"
}
dependencies = {
   "lua >= 5.1",
}
build = {
   type = "builtin",
   modules = {
      lpeglabel = {
         "lpcap.c", "lpcode.c", "lpprint.c", "lptree.c", "lpvm.c"
      },
      relabel = "relabel.lua"
   }
}
