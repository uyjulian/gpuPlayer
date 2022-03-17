/***************************************************************************
      File Name:	StdAfx.h
   File Created:	Monday, September 22nd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _STDAFX_H_
#define _STDAFX_H_

/****** User disabled warning messages *****************************************/
// warning C4267: 'argument' : conversion from 'size_t' to 'int', possible loss of data
#pragma warning(disable: 4267)
// warning C4018: '<' : signed/unsigned mismatch
#pragma warning(disable: 4018)

/****** Standards Includes *****************************************/

#define WINVER 0x0500

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <shlobj.h>
#include <tchar.h>
#include <stdio.h>
#include <direct.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "resource.h"

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

/****** StdAfx functions *****************************************/

LPSTR FormatLong(DWORD Value);
LPSTR FormatDouble(DOUBLE Value, INT Decimals);
INT MessageBoxFormat(HWND hWnd, LPCSTR FormatString, LPCSTR lpCaption, UINT uType, ...);
void SetWindowTextFormat(HWND hWnd, LPCSTR FormatString, ...);

/****** User Defines *****************************************/

#define WM_RESIZE	    0x0401
#define WM_UPDATESTATE  0x0402

#define MAIN_CLASS_NAME     "gpuPlayerMain Class"
#define CONTROL_CLASS_NAME  "gpuPlayerControl Class"
#define SEEKBAR_CLASS_NAME  "gpuPlayerSeekBar Class"
#define INFOBAR_CLASS_NAME     "gpuPlayerInfo Class"

#define SEEKBAR_HEIGHT  20
#define TOOLBAR_HEIGHT  28
#define INFOBAR_HEIGHT     24

#define IDC_PLAY    2001
#define IDC_PAUSE   2002
#define IDC_STOP    2003
#define IDC_SOUND   2004

#define PSE_LT_GPU	2
#define PSE_LT_SPU	4

#define GPU_LIBRARY_NAME	"DarkMan's GPU Recorder"
#define SPU_LIBRARY_NAME	"DarkMan's SPU Recorder"

/****** Helper macros *****************************************/

#define IDC_HAND MAKEINTRESOURCE(32649)

#define randomize(x) if(x==0) srand(GetTickCount()); else srand(x);
#define rnd(x) rand()*(x)/RAND_MAX

#define Limit(val, min, max) if(val<min) val = min; if(val>max) val = max;

#define Del(hObj) if(hObj) { DeleteObject(hObj); hObj = NULL; }

#define SetFont(hW, hFont) SendMessage(hW,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE, 0))

/****** ProgressBar control message APIs *****************************************/

#define ProgressBar_GetPos(hwndCtl)                        (LONG)SendMessage(hwndCtl, PBM_GETPOS, 0, 0)
#define ProgressBar_SetPos(hwndCtl, nNewPos)               (LONG)SendMessage(hwndCtl, PBM_SETPOS, (WPARAM)nNewPos, 0);
#define ProgressBar_SetRange(hwndCtl, nMinRange, nMaxRange) SendMessage(hwndCtl, PBM_SETRANGE, 0, MAKELPARAM(nMinRange, nMaxRange))

/****** Slider control message APIs *****************************************/

#define Slider_GetPosition(hwndCtl)                           (LONG)SendMessage(hwndCtl, TBM_GETPOS, 0, 0)
#define Slider_SetPageSize(hwndCtl, lPageSize)                (LONG)SendMessage(hwndCtl, TBM_SETPAGESIZE, 0, (LONG)lPageSize);
#define Slider_SetPosition(hwndCtl, lPosition)                SendMessage(hwndCtl, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)lPosition)
#define Slider_SetRange(hwndCtl, fRedraw, lMinimum, lMaximum) SendMessage(hwndCtl, TBM_SETRANGE, (WPARAM)fRedraw, (LPARAM)MAKELONG(lMinimum, lMaximum)
#define Slider_SetTicFreq(hwndCtl, wFreq)                     SendMessage(hwndCtl, TBM_SETTICFREQ, (WPARAM)wFreq, 0) 
#define Slider_Init(hwndCtl, From, To, Freq, Page)            Slider_SetRange(hwndCtl, TRUE, From, To); Slider_SetTicFreq(hwndCtl, Freq); Slider_SetPageSize(hwndCtl, Page); Slider_SetPosition(hwndCtl, 0)

/****** ToolBox control message APIs *****************************************/

#define ToolBar_AddBitmap(hwndCtl, nButtons, lptbab)            (int)SNDMSG(hwndCtl, TB_ADDBITMAP, (WPARAM)nButtons, (LPARAM)(LPTBADDBITMAP)lptbab)
#define ToolBar_AddButtons(hwndCtl, uNumButtons, lpButtons)     (BOOL)SNDMSG(hwndCtl, TB_ADDBUTTONS, (WPARAM)(UINT)uNumButtons, (LPARAM)(LPTBBUTTON)lpButtons)
#define ToolBar_AddString(hwndCtl, hinst, idString)             (int)SNDMSG(hwndCtl, TB_ADDSTRING, (WPARAM) (HINSTANCE) hinst, ((LPARAM) MAKELONG(idString, 0))
#define ToolBar_AutoSize(hwndCtl)                               (void)SNDMSG(hwndCtl, TB_AUTOSIZE, 0L, 0L)
#define ToolBar_ButtonCount(hwndCtl)                            (int)SNDMSG(hwndCtl, TB_BUTTONCOUNT, 0L, 0L)
#define ToolBar_ButtonStructSize(hwndCtl, cb)                   (void)SNDMSG(hwndCtl, TB_BUTTONSTRUCTSIZE, (WPARAM) cb, 0L)
#define ToolBar_ChangeBitmap(hwndCtl, idButton, iBitmap)        (BOOL)SNDMSG(hwndCtl, TB_CHANGEBITMAP, (WPARAM)idButton, (LPARAM)MAKELPARAM(iBitmap, 0))
#define ToolBar_CheckButton(hwndCtl, idButton, fCheck)          (BOOL)SNDMSG(hwndCtl, TB_CHECKBUTTON, (WPARAM)idButton, (LPARAM)MAKELONG(fCheck, 0))
#define ToolBar_CommandToIndex(hwndCtl, idButton)               (int)SNDMSG(hwndCtl, TB_COMMANDTOINDEX, (WPARAM)idButton, 0L)
#define ToolBar_Customize(hwndCtl)                              (void)SNDMSG(hwndCtl, TB_CUSTOMIZE, 0L, 0L)
#define ToolBar_DeleteButton(hwndCtl, iButton)                  (BOOL)SNDMSG(hwndCtl, TB_DELETEBUTTON, (WPARAM)iButton, 0L)
#define ToolBar_EnableButton(hwndCtl, idButton, fEnable)        (BOOL)SNDMSG(hwndCtl, TB_ENABLEBUTTON, (WPARAM)idButton, (LPARAM)MAKELONG(fEnable, 0))
#define ToolBar_InsertButton(hwndCtl, iButton, lpButton)        (BOOL)SNDMSG(hwndCtl, TB_INSERTBUTTON, (WPARAM) iButton, (LPARAM) (LPTBBUTTON) lpButton)
#define ToolBar_IsButtonChecked(hwndCtl, idButton)              (int)SNDMSG(hwndCtl, TB_ISBUTTONCHECKED, (WPARAM) idButton, 0L)
#define ToolBar_IsButtonEnabled(hwndCtl, idButton)              (int)SNDMSG(hwndCtl, TB_ISBUTTONENABLED, (WPARAM) idButton, 0L)
#define ToolBar_IsButtonHidden(hwndCtl, idButton)               (int)SNDMSG(hwndCtl, TB_ISBUTTONHIDDEN, (WPARAM) idButton, 0L)
#define ToolBar_IsButtonHighlited(hwndCtl, idButton)            (int)SNDMSG(hwndCtl, TB_ISBUTTONHIGHLIGHTED, (WPARAM) idButton, 0L)
#define ToolBar_IsButtonIndeterminate(hwndCtl, idButton)        (int)SNDMSG(hwndCtl, TB_ISBUTTONINDETERMINATE, (WPARAM) idButton, 0L)
#define ToolBar_IsButtonPressed(hwndCtl, idButton)              (int)SNDMSG(hwndCtl, TB_ISBUTTONPRESSED, (WPARAM) idButton, 0L)
#define ToolBar_LoadImages(hwndCtl, iBitmapID, hinst)           (int)SNDMSG(hwndCtl, TB_LOADIMAGES, (WPARAM)(INT) iBitmapID, (LPARAM)(HINSTANCE) hinst)
#define ToolBar_SetBitmapSize(hwndCtl, dxBitmap, dyBitmap)      (BOOL)SNDMSG(hwndCtl, TB_SETBITMAPSIZE, 0L, (LPARAM) MAKELONG(dxBitmap, dyBitmap))
#define ToolBar_SetButtonInfo(hwndCtl, iID, lptbbi)             (int)SNDMSG(hwndCtl, TB_SETBUTTONINFO, (WPARAM)(INT) iID, (LPARAM)(LPTBBUTTONINFO) lptbbi)
#define ToolBar_SetButtonSize(hwndCtl, dxButton, dyButton)      (BOOL)SNDMSG(hwndCtl, TB_SETBUTTONSIZE, 0L, (LPARAM) MAKELONG(dxButton, dyButton))
#define ToolBar_SetButtonWidth(hwndCtl, cxMin, cxMax)           (int)SNDMSG(hwndCtl, TB_SETBUTTONWIDTH, 0L, (LPARAM)(DWORD) MAKELONG(cxMin,cxMax))
#define ToolBar_SetColorScheme(hwndCtl, lpcs)                   (void)SNDMSG(hwndCtl, TB_SETCOLORSCHEME, 0L, (LPARAM)(LPCOLORSCHEME) lpcs)
#define ToolBar_SetDisabledImageList(hwndCtl, himlNewDisabled)  (HIMAGELIST)SNDMSG(hwndCtl, TB_SETDISABLEDIMAGELIST, 0L, (LPARAM)(HIMAGELIST) himlNewDisabled)
#define ToolBar_SetHotImageList(hwndCtl, himlNewHot)            (HIMAGELIST)SNDMSG(hwndCtl, TB_SETHOTIMAGELIST, 0L, (LPARAM)(HIMAGELIST) himlNewHot)
#define ToolBar_SetImageList(hwndCtl, himlNew)                  (HIMAGELIST)SNDMSG(hwndCtl, TB_SETIMAGELIST, 0L, (LPARAM)(HIMAGELIST) himlNew)
#define ToolBar_SetIndent(hwndCtl, iIndent)                     (int)SNDMSG(hwndCtl, TB_SETINDENT, (WPARAM)(INT) iIndent, 0L)
#define ToolBar_SetPadding(hwndCtl, cx, cy)                     (DWORD)SNDMSG(hwndCtl, TB_SETPADDING, 0L, lParam = MAKELPARAM(cx, cy))
#define ToolBar_SetParent(hwndCtl, hwndParent)                  (HWND)SNDMSG(hwndCtl, TB_SETPARENT, (WPARAM) (HWND) hwndParent, 0L)
#define ToolBar_SetRows(hwndCtl, cRows, fLarger, lprc)          (void)SNDMSG(hwndCtl, TB_SETROWS, (WPARAM) MAKEWPARAM(cRows, fLarger), (LPARAM) (LPRECT) lprc)
#define ToolBar_SetState(hwndCtl, idButton, fState)             (BOOL)SNDMSG(hwndCtl, TB_SETSTATE, (WPARAM) idButton, (LPARAM) MAKELONG(fState, 0))
#define ToolBar_SetStyle(hwndCtl, dwStyle)                      (void)SNDMSG(hwndCtl, TB_SETSTYLE, 0L,(LPARAM)(DWORD) dwStyle)
#define ToolBar_SetUnicodeFormat(hwndCtl, fUnicode)             (DWORD)SNDMSG(hwndCtl, TB_SETUNICODEFORMAT, (WPARAM)(BOOL)fUnicode, 0L)
#define ToolBar_SetWindowTheme(hwndCtl, pwStr)                  (LRESULT)SNDMSG(hwndCtl, TB_SETWINDOWTHEME, 0L, (LPARAM) (LPWSTR) pwStr)

/****** Last error description *****************************************/

#define DescribeLastError { \
	LPVOID lpMsgBuf; \
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
		(LPTSTR)&lpMsgBuf, 0, NULL ); \
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION ); \
	LocalFree( lpMsgBuf ); }

#define DescribeSystemError(ErrorCode) { \
	LPVOID lpMsgBuf; \
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, \
		NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
		(LPTSTR)&lpMsgBuf, 0, NULL ); \
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION ); \
	LocalFree( lpMsgBuf ); }

/****** Window information *****************************************/

#define DisplayWindowInfo(hwnd) { \
	WINDOWINFO wi; char s[1024]; \
	memset(&wi,0,sizeof(WINDOWINFO)); wi.cbSize = sizeof(wi); \
	GetWindowInfo(hwnd,&wi); sprintf(s, \
	"cbSize          : %d\nrcWindow        : (%d,%d)(%d,%d)\nrcClient        : (%d,%d)(%d,%d)\n" \
	"dwStyle         : %d\ndwExStyle       : %d\ndwWindowStatus  : %d\ncxWindowBorders : %d\n" \
	"cyWindowBorders : %d\natomWindowType  : %d\nwCreatorVersion : %d",(int)wi.cbSize, \
	(int)wi.rcWindow.left,(int)wi.rcWindow.top,(int)wi.rcWindow.right,(int)wi.rcWindow.bottom, \
	(int)wi.rcClient.left,(int)wi.rcClient.top,(int)wi.rcClient.right,(int)wi.rcClient.bottom, \
	(int)wi.dwStyle,(int)wi.dwExStyle,(int)wi.dwWindowStatus,(int)wi.cxWindowBorders, \
	(int)wi.cyWindowBorders,(int)wi.atomWindowType,(int)wi.wCreatorVersion); \
	MessageBox(hwnd,s,"WindiowInfo",0); }

/****** Draw value on window *****************************************/

#define OutInt(hW,Value) { \
		HDC hdc = GetDC(hW); char s[255]; \
		SetBkMode(hdc,OPAQUE); SetBkColor(hdc,0xffffff); SetTextColor(hdc,0); \
		sprintf(s,"   0x%lX - %d   ",Value,Value); TextOut(hdc,0,0,s,strlen(s)); \
		ReleaseDC(hW,hdc); }

#endif
