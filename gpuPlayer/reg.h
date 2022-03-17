/***************************************************************************
      File Name:	Reg.cpp
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

//
//  Suported registry data types: BINARY,DWORD,SZ(string)
//

#ifndef _REG_H_
#define _REG_H_
#include "stdafx.h"

void* REG_GetData(HKEY Root, LPCSTR KeyPath, LPCSTR Key, DWORD* DataType);
BOOL  REG_SetData(HKEY Root, LPCSTR KeyPath, LPCSTR Key, DWORD DataType, void* Data, DWORD Size);
BOOL  REG_DeleteKey(HKEY Root, LPCSTR KeyPath, LPCSTR SubKey);

//
// Reading: REG_GetDWORD
//			REG_GetBINARY
//			REG_GetSTRING
//
//			REG_GetSubDWORD
//			REG_GetSubBINARY
//			REG_GetSubSTRING
//
// Writing: REG_SetDWORD
//			REG_SetBINARY
//			REG_SetSTRING
//
//			REG_SetSubDWORD
//			REG_SetSubBINARY
//			REG_SetSubSTRING
//

#define REG_KEY "Software\\DarkMan\\gpuPlayer"

#define REG_GetDWORD(szKey,Buffer) \
	{ HKEY hKey;DWORD temp;DWORD type;DWORD size; \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_KEY,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = 4; \
		if(RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&temp,&size)==ERROR_SUCCESS) Buffer = (unsigned long)temp; \
		RegCloseKey(hKey); }}
#define REG_GetBINARY(szKey,Buffer) \
	{ HKEY hKey;DWORD type;DWORD size; \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_KEY,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = sizeof(Buffer); RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&Buffer,&size); \
		RegCloseKey(hKey); }}
#define REG_GetSTRING(szKey,Buffer) \
	{ HKEY hKey;DWORD type;DWORD size; \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,REG_KEY,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = sizeof(Buffer); RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&Buffer,&size); \
		RegCloseKey(hKey); }}

#define REG_GetSubDWORD(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD temp;DWORD type;DWORD size; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = 4; \
		if(RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&temp,&size)==ERROR_SUCCESS) Buffer = (unsigned long)temp; \
		RegCloseKey(hKey); }}
#define REG_GetSubBINARY(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD type;DWORD size; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = sizeof(Buffer); RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&Buffer,&size); \
		RegCloseKey(hKey); }}
#define REG_GetSubSTRING(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD type;DWORD size; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegOpenKeyEx(HKEY_CURRENT_USER,KeyPath,0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS) \
		{ size = sizeof(Buffer); RegQueryValueEx(hKey,szKey,0,&type,(LPBYTE)&Buffer,&size); \
		RegCloseKey(hKey); }}


#define REG_SetDWORD(szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,REG_KEY,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_DWORD,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}
#define REG_SetBINARY(szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,REG_KEY,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_BINARY,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}
#define REG_SetSTRING(szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,REG_KEY,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_SZ,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}

#define REG_SetSubDWORD(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,KeyPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_DWORD,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}
#define REG_SetSubBINARY(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,KeyPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_BINARY,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}
#define REG_SetSubSTRING(szSubKey,szKey,Buffer) \
	{ HKEY hKey;DWORD Disp; char KeyPath[256]; \
	sprintf(KeyPath,"%s\\%s",REG_KEY,szSubKey); \
	if(RegCreateKeyEx(HKEY_CURRENT_USER,KeyPath,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&Disp)==ERROR_SUCCESS) \
		{ RegSetValueEx(hKey,szKey,0,REG_SZ,(LPBYTE)&Buffer,sizeof(Buffer)); \
		RegCloseKey(hKey);}}

#endif
