/***************************************************************************
      File Name:	TabCtrl.cpp
   File Created:	Thursday, February 20th 2003.
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "tabctrl.h"

TABCTRL_HEADER	TabCtrl_Header;

//===========================================================================
//
// OnSelChanged - processes the TCN_SELCHANGE notification. 
//
//	case WM_NOTIFY:
//		if(wParam==ID_TABCTRL)
//			if((LPNMHDR(lParam)->code)==TCN_SELCHANGE)
//				{
//				TabCtrl_OnSelChanged();
//				return TRUE;
//				}
//		break;
//
//===========================================================================
 
void WINAPI TabCtrl_OnSelChanged() 
{
int Selected = TabCtrl_GetCurSel(TabCtrl_Header.hwndTab); 
if(Selected>=TabCtrl_Header.TotalTabs) return;
if(TabCtrl_Header.hwndDisplay!= NULL) ShowWindow(TabCtrl_Header.hwndDisplay,FALSE); 
TabCtrl_Header.hwndDisplay = TabCtrl_Header.Tab[Selected].hW;
ShowWindow(TabCtrl_Header.hwndDisplay,TRUE);
}

//===========================================================================
//
// Reset: resets tabctrl header
//
//===========================================================================

void TabCtrl_Reset()
{
memset(&TabCtrl_Header,0,sizeof(TABCTRL_HEADER));
}

//===========================================================================
//
// Insert: adds new tab
//
//===========================================================================

void TabCtrl_Insert(char *Name,int ResourceID,DLGPROC DlgProc)
{
if(TabCtrl_Header.TotalTabs>=TABCTRL_MAXTABS) return;
strcpy(TabCtrl_Header.Tab[TabCtrl_Header.TotalTabs].Name,Name);
TabCtrl_Header.Tab[TabCtrl_Header.TotalTabs].DlgProc = DlgProc;
TabCtrl_Header.Tab[TabCtrl_Header.TotalTabs].ResourceID = ResourceID;
TabCtrl_Header.TotalTabs++;
}

//===========================================================================
//
// Init: initializes all inserted dialogs
//
//===========================================================================

void TabCtrl_Init(HINSTANCE hInstance,HWND hwndDialog,HWND hwndTab)
{
TCITEM tie;
RECT rect1,rect2;
int i;

TabCtrl_Header.hwndTab = hwndTab;
TabCtrl_Header.hInstance = hInstance;
//SetWindowLong(hwndTab,GWL_STYLE,GetWindowLong(hwndTab,GWL_STYLE)|TCS_HOTTRACK);

for(i=0;i<TabCtrl_Header.TotalTabs;i++)
	{
    tie.mask = TCIF_TEXT;
    tie.pszText = TabCtrl_Header.Tab[i].Name;
    TabCtrl_InsertItem(TabCtrl_Header.hwndTab,i,&tie);
    TabCtrl_Header.Tab[i].hW = CreateDialog(
		hInstance, MAKEINTRESOURCE(TabCtrl_Header.Tab[i].ResourceID),
		hwndDialog,TabCtrl_Header.Tab[i].DlgProc);
	}
GetWindowRect(hwndDialog,&rect1);
GetWindowRect(TabCtrl_Header.hwndTab,&rect2);

TabCtrl_Header.rcDisplay.left = rect2.left - rect1.left + GetSystemMetrics(SM_CXDLGFRAME);
TabCtrl_Header.rcDisplay.top = rect2.top - rect1.top + GetSystemMetrics(SM_CYDLGFRAME);

for(i=0;i<TabCtrl_Header.TotalTabs;i++)
	SetWindowPos(TabCtrl_Header.Tab[i].hW,HWND_TOP,TabCtrl_Header.rcDisplay.left,TabCtrl_Header.rcDisplay.top,0,0,SWP_NOSIZE);

TabCtrl_OnSelChanged();
}

//===========================================================================
//
// GetWindow: after it is created get window handle by it's index
//
//===========================================================================

HWND TabCtrl_GetWindow(int Index)
{
if(Index>=TabCtrl_Header.TotalTabs) return NULL;
return TabCtrl_Header.Tab[Index].hW;
}
