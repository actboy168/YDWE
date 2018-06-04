#include "CC_Include.h"

void _fastcall 
    CC_Put_globals_Hook(DWORD OutClass)
{
	CC_PutString(OutClass, 0, "#define USE_BJ_ANTI_LEAK", 1);
	//CC_PutString(OutClass, 0, "#define USE_BJ_OPTIMIZATION", 1);
	//CC_PutString(OutClass, 0, "#define USE_BJ_OPTIMIZATION_PRO", 1);
	//CC_PutString(OutClass, 0, "#define USE_BJ_INLINE", 1);
    CC_PutString(OutClass, 0, "#include <YDTrigger/Import.h>", 1);
    CC_PutString(OutClass, 0, "#include <YDTrigger/YDTrigger.h>", 1);
    CC_PutString(OutClass, 0, "globals", 1);
    ((DWORD*)OutClass)[3]++;
}
