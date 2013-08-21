#include <windows.h>
#include <ctype.h>
#include "../Storm/StormAdapter.h"
#include "ConvertString.h"


int _fastcall 
	Utf8toAscii(char src[], char dst[], unsigned int limit)
{
	wchar_t* temp;
	unsigned int len;

	len = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);

	temp = (wchar_t*)malloc(len*sizeof(wchar_t));

	if (temp == NULL) 
		return 0;

	if (len > 0)
	{
		len = MultiByteToWideChar(CP_UTF8, 0, src, -1, temp, len*sizeof(wchar_t));
		len = WideCharToMultiByte(CP_ACP, 0, temp, -1, 0,  0, 0, 0);
		if (len > limit) len = limit;
		WideCharToMultiByte(CP_ACP, 0, temp, -1, dst, len*sizeof(char), 0, 0);

		dst[len] = 0;
		free(temp);
		return 1;
	}
	else
	{
		free(temp);
		return 0;
	}
}


#ifdef CONFIG_ConvertToPY

unsigned char* g_gbk2py_data   = NULL;
unsigned int   g_gbk2py_length = 0;

#define GBK2PY_HI 0x81
#define GBK2PY_LO 0x40

unsigned int _fastcall
gbk2py_one(char* buff, unsigned int limit, unsigned char hi, unsigned char lo)
{
  unsigned int phead, ptail, pos, len;
  
  if ((g_gbk2py_data == NULL) || (hi < GBK2PY_HI) || (lo < GBK2PY_LO)) 
  {
    if (limit > 2) 
      len = 2;
    memcpy(buff, "__", len);
  }
  else
  {
    pos = (0xFF-GBK2PY_LO)*(hi-GBK2PY_HI)+(lo-GBK2PY_LO);

    phead = *(unsigned int*)(g_gbk2py_data+4*pos);
    ptail = *(unsigned int*)(g_gbk2py_data+4*(pos+1));
    if (limit > ptail-phead-2) 
      len = ptail-phead-2;
    else
      len = limit;
    
    memcpy(buff, (char*)(g_gbk2py_data+phead+1), len);
    
    buff[0] = toupper(buff[0]);
  }
  return len;
}

int _fastcall
gbk2py_open()
{
  return BLZSFileLoadFile("UI\\gbk2py.dat", &g_gbk2py_data, &g_gbk2py_length, 1, 0);
}

int _fastcall
gbk2py_close()
{
  if (g_gbk2py_data != NULL) 
    return BLZSFileUnloadFile(g_gbk2py_data);
  return FALSE;
}

void _fastcall
ToPY(char* src, char* dst, unsigned int limit)
{
  unsigned int i, j, n;

  for (n = BLZSStrLen(src)-1; n >= 0; n--)
  {
    if (src[n] != ' ') break;
  }

  for (i = 0, j = 0; (i <= n) && (j < limit-1); i++)
  {
    if (isalnum(src[i]) || (src[i] == '_'))
    {
      dst[j++] = src[i];
      continue;
    }

    if (src[i] == ' ')
    {
      dst[j++] = '_';
      continue;
    }

    if (i == n)
    {
      dst[j++] = '_';
      continue;
    }

    j += gbk2py_one(dst+j, limit-j-1, ((unsigned char*)src)[i], ((unsigned char*)src)[i+1]);
    i++;
  }
  
  dst[j] = '\0';
}

void _fastcall
ToPY_utf8(char* src, char* dst, unsigned int limit)
{
  char* buffer = (char*)malloc(limit*sizeof(char));

  if (buffer == NULL)
    return;

  if (Utf8toAscii(src, buffer, limit))
  {
    ToPY(buffer, dst, limit);
  }
  else
  {
    ConvertString_Default(buffer, dst, limit);
  }
  free(buffer);
}

#endif