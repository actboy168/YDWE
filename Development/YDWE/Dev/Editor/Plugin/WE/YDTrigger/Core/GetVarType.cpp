#include "CC_Include.h"

DWORD GetVarType(DWORD This, DWORD index)
{
  unsigned char* type = (unsigned char*)&GetGUIVar_Value(This, index);
  return (type[8]-'0')*10+(type[9]-'0');
}
