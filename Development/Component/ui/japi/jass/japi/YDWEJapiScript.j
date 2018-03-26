#ifndef YDWEYDWEJapiScriptIncluded 
#define YDWEYDWEJapiScriptIncluded

library YDWEYDWEJapiScript
	globals
    	constant integer YDWE_OBJECT_TYPE_ABILITY      = 0
    	constant integer YDWE_OBJECT_TYPE_BUFF         = 1
    	constant integer YDWE_OBJECT_TYPE_UNIT         = 2
    	constant integer YDWE_OBJECT_TYPE_ITEM         = 3
    	constant integer YDWE_OBJECT_TYPE_UPGRADE      = 4
    	constant integer YDWE_OBJECT_TYPE_DOODAD       = 5
    	constant integer YDWE_OBJECT_TYPE_DESTRUCTABLE = 6
	endglobals

	native EXExecuteScript     takes string script returns string

endlibrary

#endif  /// YDWEYDWEJapiScriptIncluded
