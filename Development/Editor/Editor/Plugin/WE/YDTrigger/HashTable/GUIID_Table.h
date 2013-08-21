#ifndef INCLUDE_GUIID_TABLE_H
#define INCLUDE_GUIID_TABLE_H

void GUIID_Table_Create();
void GUIID_Table_Destory();

void GUIID_Table_Init();

void GUIID_Table_PutID(const char* gui_name, unsigned int  id);
void GUIID_Table_GetID(const char* gui_name, unsigned int* pid);

#endif
