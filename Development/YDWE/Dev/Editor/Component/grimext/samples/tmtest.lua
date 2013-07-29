mergefile("input.wtg")
mergefile("more.wct")
addvariable("IntVariable", "integer", true) -- merge set to true
addvariable("BooleanVariable", "boolean", false, 10) -- merge set to false, is array with 10 elements
addvariable("DialogVariable", "dialog", false, "DialogCreate()") --merge set to false, is initialized to DialogCreate()
addvariable("RealVariable", "real", true, 4, "5.99") -- merge set to true, is array with 4 elements, initialized to 5.99