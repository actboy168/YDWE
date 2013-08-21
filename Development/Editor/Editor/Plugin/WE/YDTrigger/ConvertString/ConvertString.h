#ifndef INCLUDE_CONVERTSTRING_H
#define INCLUDE_CONVERTSTRING_H

#ifdef CONFIG_ConvertToPY
int  _fastcall gbk2py_open();
int  _fastcall gbk2py_close();
void _fastcall ToPY_utf8(char* src, char* dst, unsigned int limit);
#endif
void _fastcall ConvertString_Default(char* src, char* dst, unsigned int limit);
void _fastcall ConvertString(char* src, char* dst, unsigned int limit);

#endif
