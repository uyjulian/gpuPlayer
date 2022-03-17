/***************************************************************************
      File Name:	Menu.h
   File Created:	Tuesday, September 23rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _MENU_H_
#define _MENU_H_
#include "stdafx.h"

/************************************************************
Add following lines to MainWindowProc

		case WM_CREATE:
			Menu_Init(hwnd);
			break;
		case WM_MEASUREITEM:
			Menu_MeasureItem(hwnd, (LPMEASUREITEMSTRUCT)lParam);
			return TRUE;
		case WM_DRAWITEM:
			Menu_DrawItem((LPDRAWITEMSTRUCT)lParam);
			return TRUE;
		case WM_DESTROY:
			Menu_DeInit();
			break;

************************************************************/

BOOL Menu_Init(HMENU hMenu);
void Menu_DeInit();

BOOL Menu_SetImageList(HINSTANCE hInstance, INT BitmapID);
BOOL Menu_SetItemImage(INT ItemID, INT ImageIndex);
void Menu_SetCheckImage(INT ImageIndex);

void Menu_MeasureItem(HWND hwnd, LPMEASUREITEMSTRUCT lpmis);
void Menu_DrawItem(LPDRAWITEMSTRUCT lpdis);


#endif