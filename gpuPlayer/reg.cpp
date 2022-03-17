/***************************************************************************
      File Name:	Reg.cpp
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "reg.h"

//==========================================================================
// Read registry data
//==========================================================================

void* REG_GetData(HKEY Root, LPCSTR KeyPath, LPCSTR Key, DWORD* DataType)
{
	static char ReturnString[32768];
	DWORD DataSize = 0, ReturnDword = 0;
	void* ReturnData;
	HKEY hKey;

	memset(ReturnString,0,sizeof(ReturnString));
	if(RegOpenKey(Root,KeyPath,&hKey)==ERROR_SUCCESS) {
		if(RegQueryValueEx(hKey,Key,NULL,DataType,NULL,&DataSize)==ERROR_SUCCESS) {
			if(*DataType==REG_BINARY || *DataType==REG_SZ) {
				if (RegQueryValueEx(hKey,Key,NULL,DataType,(LPBYTE)ReturnString,&DataSize)!=ERROR_SUCCESS) ReturnString[0] = 0;
					ReturnData = (void*)ReturnString;
				} else if(*DataType==REG_DWORD) {
					if (RegQueryValueEx(hKey,Key,NULL,DataType,(unsigned char*)&ReturnDword,&DataSize)!=ERROR_SUCCESS) ReturnDword=0;
					ReturnData = (void*)ReturnDword;
				} else
					ReturnData=NULL;
			}
	}
	RegCloseKey(hKey);
	return ReturnData;
}

//==========================================================================
// Set registry data
//==========================================================================
				
BOOL REG_SetData(HKEY Root, LPCSTR KeyPath, LPCSTR Key, DWORD DataType, void* Data, DWORD Size)
{
	BOOL Result = FALSE;
	HKEY hKey;

	if(RegCreateKey(Root,KeyPath,&hKey)==ERROR_SUCCESS) {
		if(DataType==REG_BINARY || DataType==REG_SZ) {
			if(RegSetValueEx(hKey,Key,NULL,DataType,(BYTE*)Data,Size)==ERROR_SUCCESS) Result = TRUE;
		} else if (DataType==REG_DWORD) {
			if(RegSetValueEx(hKey,Key,NULL,DataType,(BYTE*)Data,4)==ERROR_SUCCESS) Result = TRUE;
		}
	}
	RegCloseKey(hKey);
	return Result;
}

//==========================================================================
// Delete registry key
//==========================================================================

BOOL REG_DeleteKey(HKEY Root, LPCSTR KeyPath, LPCSTR SubKey)
{
	BOOL Result = FALSE;
	HKEY hKey;
	if(RegOpenKey(Root,KeyPath,&hKey)==ERROR_SUCCESS) {
        if(RegDeleteKey(hKey,SubKey)==ERROR_SUCCESS) Result = TRUE;
        RegCloseKey(hKey);
	}
	return Result;
}
