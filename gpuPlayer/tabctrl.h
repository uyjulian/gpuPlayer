/***************************************************************************
      File Name:	TabCtrl.h
   File Created:	Thursday, February 20th 2003.
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _TABCTRL_H_
#define _TABCTRL_H_

#include "stdafx.h"

#define TABCTRL_MAXTABS 16

typedef struct tag_tabctrl_item {
	char	Name[32];
	int		ResourceID;
	DLGPROC	DlgProc;
	HWND	hW;
	} TABCTRL_ITEM;

typedef struct tag_tabctrl_header {
	HINSTANCE	hInstance;
	HWND		hwndTab;
	HWND		hwndDisplay;
	RECT		rcDisplay;
	LONG		TotalTabs;
	TABCTRL_ITEM Tab[TABCTRL_MAXTABS];
	} TABCTRL_HEADER;

// OnSelChanged - processes the TCN_SELCHANGE notification. 
//
//	case WM_NOTIFY:
//		if(wParam==ID_TABCTRL)
//			if((LPNMHDR(lParam)->code)==TCN_SELCHANGE)
//				{
//				OnSelChanged(hwndDlg);
//				return TRUE;
//				}
//		break;
//
 
void WINAPI TabCtrl_OnSelChanged();
void TabCtrl_Reset();
void TabCtrl_Insert(char *Name,int ResourceID,DLGPROC DlgProc);
void TabCtrl_Init(HINSTANCE hInstance,HWND hwndDialog,HWND hwndTab);
HWND TabCtrl_GetWindow(int Index);

#endif
