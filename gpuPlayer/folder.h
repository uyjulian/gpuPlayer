/***************************************************************************
      File Name:	Folder.h
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _FOLDER_H_
#define _FOLDER_H_
#include "stdafx.h"
#include "reg.h"

void GetSpecialFolder(LPSTR folder, INT size, INT FolderID);
BOOL BrowseDirectory(LPCSTR szTitle, LPSTR buffer, INT size, HWND hwnd);
void AppPath(LPSTR buffer, INT size);
void WinPath(LPSTR buffer, INT size);
void WinSysPath(LPSTR buffer, INT size);
void WinTempPath(LPSTR buffer, INT size);
void WinCommonPath(LPSTR buffer, INT size);
BOOL CreatePath(LPSTR NewPath);

#endif