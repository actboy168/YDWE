#ifndef YDWEBaseIncluded
#define YDWEBaseIncluded

library YDWEBase initializer InitializeYD

#if WARCRAFT_VERSION >= 124
#  include "Base/YDWEBase_hashtable.j"
#else 
#  include "Base/YDWEBase_returnbug.j" 
#endif
#    
#  include "Base/YDWEBase_common.j"

endlibrary

#endif // YDWEBaseIncluded
