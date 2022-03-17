/***************************************************************************
      File Name:	Dialog.cpp
   File Created:	Sunday, September 28th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

/***********************************************************************/
// History of changes:
//
// 28/09/2003
// - initial release
//
/***********************************************************************/

#include "stdafx.h"
#include "winmain.h"

HFONT hFont = NULL;

BOOL CALLBACK About_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG: {
            hFont = CreateFont(22, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, VARIABLE_PITCH | FF_SWISS, "Tahoma");
            if(hFont) hFont = CreateFont(22, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, VARIABLE_PITCH | FF_SWISS, "");
			if(hFont) 
				SetFont(GetDlgItem(hW,IDC_TITLE),hFont);
			SetDlgItemText(hW, IDC_BUILD, APP_BUILD);
			SetDlgItemText(hW, IDC_VERSION, APP_VERSION);
			return TRUE;
		}
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDCANCEL:
				case IDOK:			EndDialog(hW,TRUE);		return TRUE;
			}
		}
		case WM_DESTROY:
			if(hFont) DeleteObject(hFont);
			break;
	}
	return FALSE;
} 