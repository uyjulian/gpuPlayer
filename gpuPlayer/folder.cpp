/***************************************************************************
      File Name:	Folder.cpp
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "folder.h"

//==========================================================================
// 
//==========================================================================

void GetSpecialFolder(LPSTR folder, INT size, INT FolderID)
{
	LPITEMIDLIST IDL;
	LPMALLOC Allocator;
	int i;
	memset(folder,0,size);

	if(SHGetSpecialFolderLocation(NULL,FolderID,&IDL)==NOERROR) {
        SHGetPathFromIDListA(IDL,folder);
        SHGetMalloc(&Allocator);
        Allocator->Free(IDL);
	} else
        switch(FolderID) {
			case CSIDL_WINDOWS:
				WinPath(folder,size);
                break;
			case CSIDL_SYSTEM:
				WinSysPath(folder,size);
                break;
			case CSIDL_PROGRAM_FILES_COMMON:
				WinCommonPath(folder,size);
                break;
        }
	i = strlen(folder);
	if(i > 0 && folder[i-1] != '\\') folder[i] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

void _SHFree(void *p)
{
	IMalloc *pm;
	SHGetMalloc(&pm);
	if(pm) {
		pm->Free(p);
		pm->Release();
	}
}

BOOL BrowseDirectory(LPCSTR szTitle, LPSTR buffer, INT size, HWND hwnd)
{
	BROWSEINFO BI;
	ITEMIDLIST *pidl = NULL;
	memset(buffer,0,size);
	memset(&BI,0,sizeof(BI));

	BI.pidlRoot = NULL;
	BI.hwndOwner = hwnd;
	BI.lpszTitle = szTitle;
	BI.ulFlags = BIF_RETURNONLYFSDIRS;
	pidl = SHBrowseForFolder(&BI);
	if(pidl == NULL) return FALSE;
	BOOL Result = SHGetPathFromIDList(pidl, buffer);
	_SHFree(pidl);
	if(Result == FALSE) return FALSE;
	int l = strlen(buffer);
	if(buffer[l-1] != '\\') { buffer[l] = '\\'; buffer[l+1] = 0; }
	return TRUE;
}

//==========================================================================
// 
//==========================================================================

void AppPath(LPSTR buffer, INT size)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	const char *path = _pgmptr ;
	_splitpath(_pgmptr, drive, dir, fname, ext );
	memset(buffer,0,size);
	strcpy(buffer, drive);
	strcat(buffer, dir);
	if(buffer[strlen(buffer)-1] != '\\') buffer[strlen(buffer)] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

void WinPath(LPSTR buffer, INT size)
{
	memset(buffer,0,size);
	GetWindowsDirectory(buffer,size);
	if(buffer[strlen(buffer)-1]!='\\') buffer[strlen(buffer)] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

void WinSysPath(LPSTR buffer, INT size)
{
	memset(buffer,0,size);
	GetSystemDirectory(buffer,size);
	if(buffer[strlen(buffer)-1] != '\\') buffer[strlen(buffer)] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

void WinTempPath(LPSTR buffer, INT size)
{
	memset(buffer,0,255);
	GetTempPath(255,buffer);
	if(buffer[strlen(buffer)-1] != '\\') buffer[strlen(buffer)] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

void WinCommonPath(LPSTR buffer, INT size)
{
	unsigned long DataType;
	memset(buffer,0,255);
	strcpy(buffer,(char*)REG_GetData(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion","CommonFilesDir",&DataType));
	if(DataType!=REG_SZ || strlen(buffer)==0) strcpy(buffer,"C:\\Program Files\\Common Files");
	if(buffer[strlen(buffer)-1] != '\\') buffer[strlen(buffer)] = '\\';
	return;
}

//==========================================================================
// 
//==========================================================================

BOOL DirExists(LPCSTR Dir)
{
	FILE* check;
	char FileName[MAX_PATH];
	sprintf(FileName,"%s~TMP.TMP",Dir);
	check = fopen(FileName,"w+");
	if(check==NULL) return FALSE;
	fclose(check);
	remove(FileName);
	return TRUE;
}

BOOL CreatePath(LPSTR NewPath)
{
	if(strlen(NewPath) < 3) return FALSE;
	if(NewPath[0] > 97) NewPath[0] -= 32;
	if(NewPath[0] < 67 || NewPath[0] > 90) return FALSE;
	if(NewPath[1] != ':' || NewPath[2] != '\\') return FALSE;
	char buffer[MAX_PATH];
	memset(buffer,0,sizeof(buffer));
	int i;
	for(i = 0; i < 4; i++) buffer[i] = NewPath[i];
	while(NewPath[i]) {
		buffer[i] = NewPath[i];
		if(buffer[i] == '\\' && !DirExists(buffer))
			if(_mkdir(buffer)) return FALSE;
		i++;
	}
	return TRUE;
}
