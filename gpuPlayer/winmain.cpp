/***************************************************************************
      File Name:	WinMain.cpp
   File Created:	Monday, September 22nd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "about.h"
#include "config.h"
#include "externals.h"
#include "menu.h"
#include "options.h"
#include "player.h"
#include "rebuild.h"

BOOL ActivateBreak = FALSE;

HINSTANCE hAppInstance = NULL;
BOOL AppActive = FALSE;
HMENU hMenu = NULL;
HACCEL hAccel = NULL;

HWND hMainWindow = NULL;
HWND hControlWindow = NULL;
HWND hSeekBar = NULL;
HWND hToolBar = NULL;
HWND hInfoBar = NULL;

BOOL Restart = FALSE;

LRESULT CALLBACK ToolBarWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==========================================================================
// Helper functions
//==========================================================================

void GetWindowBorderSize(HWND hwnd, LPSIZE lpsize)
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(wi);
	GetWindowInfo(hwnd, &wi);
	lpsize->cx = (wi.rcWindow.right - wi.rcWindow.left) - (wi.rcClient.right - wi.rcClient.left);
	lpsize->cy = (wi.rcWindow.bottom - wi.rcWindow.top) - (wi.rcClient.bottom - wi.rcClient.top);
}

//==========================================================================
// Load Image List
//==========================================================================

HIMAGELIST LoadImageList(HINSTANCE hInstance, int BitmapID)
{
	HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(BitmapID), RT_BITMAP);
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(hInstance, hRes));
	int ImageCount = lpbi->biWidth / 16;
	HBITMAP hbm = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(BitmapID), IMAGE_BITMAP, 0, 0 , LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	HIMAGELIST hImages = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK , ImageCount, 0);
	ImageList_AddMasked(hImages, hbm, CLR_DEFAULT);
	DeleteBitmap(hbm);
	return hImages;
}

//==========================================================================
// Main window procedure
//==========================================================================

void RefreshControls()
{
	static int LastImage = 0;
	TBBUTTONINFO bi;
	bi.cbSize = sizeof(bi);
	bi.dwMask = TBIF_IMAGE;
	if(Player_Stream) {
		ToolBar_EnableButton(hToolBar, IDC_PLAY,  TRUE);
		ToolBar_EnableButton(hToolBar, IDC_PAUSE, TRUE);
		ToolBar_EnableButton(hToolBar, IDC_STOP,  TRUE);
		if(Player_Header.SoundDataSize) {
			ToolBar_EnableButton(hToolBar, IDC_SOUND,  TRUE);
			bi.iImage = cfg.SoundEnabled ? 3 : 4;
		} else {
			ToolBar_EnableButton(hToolBar, IDC_SOUND,  FALSE);
			bi.iImage = 3;
		}
		ToolBar_CheckButton(hToolBar, IDC_PLAY,  (Player_State == STATE_PLAYING || Player_State == STATE_SEEKING));
		ToolBar_CheckButton(hToolBar, IDC_PAUSE, (Player_State == STATE_PAUSED));
		ToolBar_CheckButton(hToolBar, IDC_STOP,  (Player_State == STATE_STOPPED));
		ToolBar_CheckButton(hToolBar, IDC_SOUND,  !cfg.SoundEnabled);

	} else {
		ToolBar_EnableButton(hToolBar, IDC_PLAY,  FALSE);
		ToolBar_EnableButton(hToolBar, IDC_PAUSE, FALSE);
		ToolBar_EnableButton(hToolBar, IDC_STOP,  FALSE);
		ToolBar_EnableButton(hToolBar, IDC_SOUND,  TRUE);
		bi.iImage = cfg.SoundEnabled ? 3 : 4;
		ToolBar_CheckButton(hToolBar, IDC_PLAY,  FALSE);
		ToolBar_CheckButton(hToolBar, IDC_PAUSE, FALSE);
		ToolBar_CheckButton(hToolBar, IDC_STOP,  FALSE);
		ToolBar_CheckButton(hToolBar, IDC_SOUND,  !cfg.SoundEnabled);
	}
	if(bi.iImage != LastImage) {
		ToolBar_SetButtonInfo(hToolBar, IDC_SOUND, &bi);
		LastImage = bi.iImage;
	}
	InvalidateRect(hSeekBar, NULL, FALSE);
	InvalidateRect(hInfoBar, NULL, FALSE);
}

BOOL OnOpen(HWND hwnd)
{
	OPENFILENAME ofn;
	char buffer[MAX_PATH];
	memset(&ofn,0,sizeof(ofn));
	memset(buffer,0,sizeof(buffer));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "REC files\x00*.REC\x00\x00";
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	GetOpenFileName(&ofn);
	if(strlen(buffer)>0) PLAYER_Open(buffer, hwnd);
	RefreshControls();
	return TRUE;
}

//==========================================================================
// Main window procedure
//==========================================================================

LRESULT CALLBACK MainWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static HBITMAP hbmBackground = NULL, hbmPrev;
	static BITMAP bmpBackground;
	static HDC hdcBackground = NULL;
	static SIZE BorderSize;
	switch(uMsg) {
		case WM_CREATE: {
			GetWindowBorderSize(hwnd, &BorderSize);
			// Load bacground bitmap
			hbmBackground = (HBITMAP)LoadImage(hAppInstance, MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, 0, 0, 0);
			GetObject(hbmBackground, sizeof(BITMAP), &bmpBackground);
			hdcBackground = CreateCompatibleDC(NULL);
			hbmPrev = SelectBitmap(hdcBackground, hbmBackground);
			// Init Menu XP look
			Menu_Init(GetMenu(hwnd));
			Menu_SetImageList(hAppInstance, IDB_MENU);
			Menu_SetItemImage(IDM_FILE_OPEN, 0);
			Menu_SetItemImage(IDM_FILE_REBUILD, 1);
			Menu_SetItemImage(IDM_PLAYBACK_MAKESNAPSHOT, 2);
			Menu_SetItemImage(IDM_HELP_ABOUT, 3);
			Menu_SetCheckImage(4);
			break;
		}
		case WM_PAINT:
			if(Player_Stream == NULL) {
				HDC hdc = GetDC(hwnd);
				if(hbmBackground)
					BitBlt(hdc, 0, 0, bmpBackground.bmWidth, bmpBackground.bmHeight, hdcBackground, 0, 0, SRCCOPY);
				else {
					RECT rc;
					GetClientRect(hwnd, &rc);
					FillRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));
				}
				ReleaseDC(hwnd, hdc);
				ValidateRect(hwnd, NULL);
			}
			break;
		case WM_SIZING:
			if(Player_Stream == NULL) {
				LPRECT rc = (LPRECT)lParam;
				rc->right = rc->left + bmpBackground.bmWidth;
				rc->bottom = rc->top + bmpBackground.bmHeight;
			}
		case WM_SIZE:
			if(Player_Stream == NULL) {
				SetWindowPos(hwnd, NULL, 0, 0, bmpBackground.bmWidth, bmpBackground.bmHeight, SWP_NOZORDER | SWP_NOMOVE);
			}
		case WM_MOVE:
		case WM_MOVING:
			SendMessage(hControlWindow, WM_SIZE, 0, 0);
			break;
		case WM_ACTIVATE:
			if(wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
				SetWindowPos(hControlWindow, hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			return 0;
		case WM_INITMENU: {
			HMENU hMenu = (HMENU)wParam;			
			CheckMenuItem(hMenu, IDM_VIEW_CONTROLS, MF_BYCOMMAND | (IsWindowVisible(hControlWindow) ? MF_CHECKED : MF_UNCHECKED));
			CheckMenuItem(hMenu, IDM_VIEW_SHOWFRAMES, MF_BYCOMMAND | (cfg.ShowFrames ? MF_CHECKED : MF_UNCHECKED));
			CheckMenuItem(hMenu, IDM_PLAYBACK_SOUND, MF_BYCOMMAND | (cfg.SoundEnabled ? MF_CHECKED : MF_UNCHECKED));
			EnableMenuItem(hMenu, IDM_FILE_CLOSE,            MF_BYCOMMAND | ((Player_Stream != NULL) ? MF_ENABLED : MF_DISABLED));
			EnableMenuItem(hMenu, IDM_PLAYBACK_PLAY,         MF_BYCOMMAND | ((Player_Stream != NULL) ? MF_ENABLED : MF_DISABLED));
			EnableMenuItem(hMenu, IDM_PLAYBACK_STOP,         MF_BYCOMMAND | ((Player_Stream != NULL) ? MF_ENABLED : MF_DISABLED));
			EnableMenuItem(hMenu, IDM_PLAYBACK_SOUND,        MF_BYCOMMAND | ((Player_Stream == NULL || (Player_Stream != NULL && Player_Header.SoundDataSize)) ? MF_ENABLED : MF_DISABLED));
			EnableMenuItem(hMenu, IDM_PLAYBACK_MAKESNAPSHOT, MF_BYCOMMAND | ((Player_Stream != NULL && GPUmakeSnapshot != NULL) ? MF_ENABLED : MF_DISABLED));
			break;
		}
		case WM_MEASUREITEM:
			Menu_MeasureItem(hwnd, (LPMEASUREITEMSTRUCT)lParam);
			return 0;
		case WM_DRAWITEM:
			Menu_DrawItem((LPDRAWITEMSTRUCT)lParam);
			return 0;
		case WM_LBUTTONDOWN:
			uMsg = WM_COMMAND;
			wParam = IDM_PLAYBACK_PLAY;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDM_FILE_OPEN:
					OnOpen(hwnd);
					break;
				case IDM_FILE_CLOSE:
					PLAYER_Close(hMainWindow);
					SendMessage(hwnd, WM_SIZE, 0, 0);
					break;
				case IDM_FILE_REBUILD:
					DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_REBUILD), hwnd, Rebuild_DlgProc);
					break;
				case IDM_FILE_EXIT:
					DestroyWindow(hwnd);
					break;
				case IDM_VIEW_CONTROLS:
					ShowWindow(hControlWindow, !(GetMenuState(hMenu, IDM_VIEW_CONTROLS, MF_BYCOMMAND) & MF_CHECKED));
					SendMessage(hControlWindow, WM_SIZE, 0, 0);
					break;
				case IDM_VIEW_SHOWFRAMES:
					cfg.ShowFrames = !cfg.ShowFrames;
					break;
				case IDM_VIEW_OPTIONS: {
					char VideoPlugin[MAX_PATH];
					char SoundPlugin[MAX_PATH];
					strcpy(VideoPlugin, cfg.VideoPlugin);
					strcpy(SoundPlugin, cfg.SoundPlugin);
					if(DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwnd, Options_DlgProc)) {
						if(strcmp(VideoPlugin, cfg.VideoPlugin) || strcmp(SoundPlugin, cfg.SoundPlugin)) {
							if(PluginsLoaded) {
								MessageBox(hMainWindow, "Program must be restarted so changes can take effect !\nProgram will now exit.", "Information", MB_ICONINFORMATION);
								DestroyWindow(hMainWindow);
							}
							return 0;
						}
					}
					RefreshControls();
					break;
				}
				case IDM_PLAYBACK_PLAY:
					if(Player_Stream) {
						if(Player_State == STATE_PLAYING)
							Player_State = STATE_PAUSED;
						else
							Player_State = STATE_PLAYING;
						RefreshControls();
					}
					break;
				case IDM_PLAYBACK_STOP:
					if(Player_Stream) PLAYER_Stop(hMainWindow);
					break;
				case IDM_PLAYBACK_SOUND:
					cfg.SoundEnabled = !cfg.SoundEnabled;
					RefreshControls();
					break;
				case IDM_PLAYBACK_MAKESNAPSHOT:
					if(GPUmakeSnapshot) {
						mkdir("snap");
						GPUmakeSnapshot();
						MessageBox(hwnd, "Snapshot made !", "Info", MB_ICONINFORMATION);
					}
					break;
				case IDM_HELP_ABOUT:
					DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_ABOUT), hwnd, About_DlgProc);
					break;
			}
			break;
		case WM_UPDATESTATE: {
			RefreshControls();
			if(Player_State == STATE_STOPPED) InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		case WM_DESTROY: {
			DestroyWindow(hControlWindow);
			AppActive = FALSE;
			Menu_DeInit();
			DestroyMenu(hMenu);
			PostQuitMessage(0);
			return 0;
		}
	}
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}

//==========================================================================
// Control window procedure
//==========================================================================

DWORD PositionControls()
{
	WINDOWINFO wi; wi.cbSize = sizeof(wi);
	GetWindowInfo(hMainWindow, &wi);
	DWORD Width = wi.rcWindow.right - wi.rcWindow.left;
	DWORD Height = 0;
	
	GetWindowInfo(hSeekBar, &wi); 
	SetWindowPos(hSeekBar, NULL, 0, Height, Width, SEEKBAR_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
	Height += wi.rcWindow.bottom - wi.rcWindow.top;

	GetWindowInfo(hToolBar, &wi); 
	SetWindowPos(hToolBar, NULL, 0, Height, Width, TOOLBAR_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
	Height += wi.rcWindow.bottom - wi.rcWindow.top;

	GetWindowInfo(hInfoBar, &wi); 
	SetWindowPos(hInfoBar, NULL, 0, Height, Width, INFOBAR_HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE);
	Height += wi.rcWindow.bottom - wi.rcWindow.top;
	
	return Height;
}

LRESULT CALLBACK ControlWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HIMAGELIST hImages = NULL;

	switch(uMsg) {
		case WM_CREATE: {
			RECT rc; GetClientRect(hwnd, &rc);
			DWORD Width = rc.right - rc.left;
			// Create SeekBar
			hSeekBar = CreateWindowEx(0 , SEEKBAR_CLASS_NAME, "", 
							WS_CHILD | WS_VISIBLE, 
							CW_USEDEFAULT, CW_USEDEFAULT, Width, SEEKBAR_HEIGHT,
							hwnd, NULL, hAppInstance, NULL);
			// Create ToolBar
			hToolBar = CreateWindowEx(0 , TOOLBARCLASSNAME, "", 
							WS_CHILD | WS_VISIBLE, // | CCS_NOPARENTALIGN | CCS_NORESIZE,
							CW_USEDEFAULT, CW_USEDEFAULT, Width, TOOLBAR_HEIGHT,
							hwnd, NULL, hAppInstance, NULL);

			SetWindowLong(hToolBar, GWL_USERDATA, GetWindowLong(hToolBar, GWL_WNDPROC));
			SetWindowLong(hToolBar, GWL_WNDPROC, (LONG)ToolBarWindowProc);
			
			// Create info display
			hInfoBar = CreateWindowEx(0, INFOBAR_CLASS_NAME, "",
							WS_CHILD | WS_VISIBLE, 
							CW_USEDEFAULT, CW_USEDEFAULT, Width, INFOBAR_HEIGHT,
							hwnd, NULL, hAppInstance, NULL);
			
			// Init toolbar
			hImages = LoadImageList(hAppInstance, IDB_TOOLBAR);
			ToolBar_SetStyle(hToolBar, TBSTYLE_FLAT);
			ToolBar_SetImageList(hToolBar, hImages);
			TBBUTTON tbb1 = { 0, IDC_PLAY,  TBSTATE_ENABLED, TBSTYLE_CHECK, 0, NULL };
			TBBUTTON tbb2 = { 1, IDC_PAUSE, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, NULL };
			TBBUTTON tbb3 = { 2, IDC_STOP,  TBSTATE_ENABLED, TBSTYLE_CHECK, 0, NULL };
			TBBUTTON tbb4 = { 20, 0,  TBSTATE_ENABLED, TBSTYLE_SEP, 0, NULL };
			TBBUTTON tbb5 = { 3, IDC_SOUND, TBSTATE_ENABLED, TBSTYLE_CHECK, 0, NULL };
			ToolBar_SetIndent(hToolBar, 10);
			ToolBar_AddButtons(hToolBar, 1, &tbb1);
			ToolBar_AddButtons(hToolBar, 1, &tbb2);
			ToolBar_AddButtons(hToolBar, 1, &tbb3);
			ToolBar_AddButtons(hToolBar, 1, &tbb4);
			ToolBar_AddButtons(hToolBar, 1, &tbb5);
			RefreshControls();
			break;
		}
		case WM_SIZING: {
			WINDOWINFO wi;
			wi.cbSize = sizeof(wi);
			GetWindowInfo(hMainWindow, &wi);
			SIZE size;
			GetWindowBorderSize(hwnd, &size);			
			LPRECT rc = (LPRECT)lParam;			
			rc->left = wi.rcWindow.left;
			rc->right = wi.rcWindow.right;
			rc->top = wi.rcWindow.bottom;
			rc->bottom = rc->top + PositionControls() + size.cy;
			break;
		}
		case WM_SIZE:
		case WM_MOVE: {
			WINDOWINFO wi;
			wi.cbSize = sizeof(wi);
			GetWindowInfo(hMainWindow, &wi);
			SIZE size;
			GetWindowBorderSize(hwnd, &size);
			SetWindowPos(hwnd, hMainWindow, wi.rcWindow.left, wi.rcWindow.bottom,
					wi.rcWindow.right - wi.rcWindow.left, PositionControls() + size.cy, SWP_NOACTIVATE);
			return 0;
		}
		case WM_NOTIFY: {
			LPNMHDR hdr = (LPNMHDR)lParam;
//			if(hdr->hwndFrom == hToolBar && hdr->code == TBN_GETDISPINFO) {
			if(hdr->code == TBN_GETDISPINFO) {
				MessageBox(hMainWindow, "Get", "", 0);
				static int cnt = 0;
				char s[255];
				sprintf(s, "%d   %d", cnt++, hdr->code);
				SetWindowText(hMainWindow, s);
				
			}
			break;
		}
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_PLAY:
					if(Player_Stream) Player_State = STATE_PLAYING;
					RefreshControls();
					break;
				case IDC_PAUSE:
					if(Player_Stream) Player_State = STATE_PAUSED;
					RefreshControls();
					break;
				case IDC_STOP:
					if(Player_Stream) PLAYER_Stop(hMainWindow);
					break;
				case IDC_SOUND:
					cfg.SoundEnabled = !ToolBar_IsButtonChecked(hToolBar, IDC_SOUND);
					RefreshControls();
					break;
			}
			break;
		case WM_DESTROY:
			ImageList_Destroy(hImages);
			DestroyWindow(hMainWindow);
			break;
	}
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}

//==========================================================================
// SeekBar window procedure
//==========================================================================

DWORD GetTrackRect(LPRECT rcClient, LPRECT rcTrack)
{
	rcTrack->left   = rcClient->left   + 10; 
	rcTrack->right  = rcClient->right  - 15;
	rcTrack->top    = rcClient->top    + 5;
	rcTrack->bottom = rcClient->bottom - 5;
	return (rcTrack->right - rcTrack->left);
}

DWORD GetButtonRect(LPRECT rcClient, LPRECT rcButton)
{
	rcButton->left   = rcClient->left   + 5; 
	rcButton->right  = rcClient->left   + 15;
	rcButton->top    = rcClient->top    + 2;
	rcButton->bottom = rcClient->bottom - 2;
	DWORD TrackWidth = rcClient->right - rcClient->left - 25;
	DWORD Offset = 0;
	if(Player_Header.Frames > 0)
		Offset = (DWORD)((double)TrackWidth * (double)Player_Frame / (double)Player_Header.Frames);
	else if(Player_FileSize > 0)
		Offset = (DWORD)((double)TrackWidth * ((double)Player_Position / (double)Player_FileSize));
	OffsetRect(rcButton, Offset, 0);
	return Offset;
}

LRESULT CALLBACK SeekBarWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DWORD PrevState = 0;
	switch(uMsg) {
		case WM_LBUTTONDOWN:
			if(Player_SeekingPossible) {
				RECT rc, rcTrack, rcButton;
				GetClientRect(hwnd, &rc);			
				DWORD TrackWidth = GetTrackRect(&rc, &rcTrack);
				DWORD Offset = GetButtonRect(&rc, &rcButton);
				int X = GET_X_LPARAM(lParam);
				int Y = GET_Y_LPARAM(lParam);
				if(X >= rcTrack.left && X <= rcTrack.right && Y >= rcButton.top && Y <= rcButton.bottom) {
					PrevState = Player_State;
					Player_State = STATE_SEEKING;
					SetCapture(hSeekBar);
				}
			}
		case WM_MOUSEMOVE:
			if(Player_State == STATE_SEEKING) {
				RECT rc, rcTrack, rcButton;
				GetClientRect(hwnd, &rc);			
				DWORD TrackWidth = GetTrackRect(&rc, &rcTrack);
				DWORD Offset = GetButtonRect(&rc, &rcButton);
				int X = GET_X_LPARAM(lParam);
				int Y = GET_Y_LPARAM(lParam);
				if(X < rcTrack.left) X = rcTrack.left;
				if(X > rcTrack.right) X = rcTrack.right;
				X -= rcTrack.left;
				if(TrackWidth > 0) {
					PLAYER_SeekFrame((DWORD)((double)X * (double)Player_Header.Frames / (double)TrackWidth));
					InvalidateRect(hSeekBar, NULL, FALSE);
					InvalidateRect(hInfoBar, NULL, FALSE);
				}
			}
			break;
		case WM_LBUTTONUP:
			if(Player_SeekingPossible) {
				RECT rc, rcTrack, rcButton;
				GetClientRect(hwnd, &rc);			
				DWORD TrackWidth = GetTrackRect(&rc, &rcTrack);
				DWORD Offset = GetButtonRect(&rc, &rcButton);
				Player_State = PrevState;
				ReleaseCapture();
			}
			break;
		case WM_PAINT: {
			RECT rc, rcTrack, rcButton;
			GetClientRect(hwnd, &rc);			
			DWORD TrackWidth = GetTrackRect(&rc, &rcTrack);
			DWORD Offset = GetButtonRect(&rc, &rcButton);

			HDC hdcDisplay = GetDC(hwnd);
			HDC hdc = CreateCompatibleDC(hdcDisplay);
			HBITMAP hbm = CreateCompatibleBitmap(hdcDisplay, rc.right - rc.left, rc.bottom - rc.top);
			SelectBitmap(hdc, hbm);			
			FillRect(hdc, &rc, (HBRUSH)GetSysColorBrush(COLOR_3DFACE));
			
			if(Player_Stream) {
				// Draw filled track
				FillRect(hdc, &rcTrack, (HBRUSH)GetStockObject(WHITE_BRUSH));
				rcTrack.right = rcTrack.left + Offset;
				FillRect(hdc, &rcTrack, (HBRUSH)GetSysColorBrush(COLOR_HIGHLIGHT));
				rcTrack.right = rcTrack.left + TrackWidth;
				DrawEdge(hdc, &rcTrack, BDR_SUNKENOUTER, BF_RECT);
				// Draw button
				if(Player_Header.IndexOffset) {
					FillRect(hdc, &rcButton, (HBRUSH)GetSysColorBrush(COLOR_BTNFACE));
					DrawEdge(hdc, &rcButton, EDGE_RAISED, BF_RECT);
					InflateRect(&rcButton, -3, -2);
					DrawEdge(hdc, &rcButton, BDR_SUNKENOUTER, BF_RECT);
				}
			} else {
				// No seeking (file closed)
				DrawEdge(hdc, &rcTrack, BDR_SUNKENOUTER, BF_RECT);
			}
			
			BitBlt(hdcDisplay, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
			DeleteBitmap(hbm);
			DeleteDC(hdc);
			ReleaseDC(hwnd, hdcDisplay);
			ValidateRect(hwnd, NULL);
			return 0;
		}
		case WM_SIZE:
			return 0;
	}
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}

//==========================================================================
// ToolBar window procedure
//==========================================================================

LRESULT CALLBACK ToolBarWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg) {
		case WM_SIZE:
			return 0;
	}
	return CallWindowProc((WNDPROC)GetWindowLong(hToolBar, GWL_USERDATA), hwnd, uMsg, wParam, lParam);
}


//==========================================================================
// InfoBar window procedure
//==========================================================================

LRESULT CALLBACK InfoBarWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont = NULL;
	switch(uMsg) {
		case WM_CREATE:
			LOGFONT lf;
			memset(&lf,0,sizeof(LOGFONT));
			lf.lfHeight = 16;
			strcpy(lf.lfFaceName,"Tahoma");
			hFont = CreateFontIndirect(&lf);
			break;
		case WM_PAINT: {
			RECT rc; GetClientRect(hwnd, &rc);
			HDC hdcDisplay = GetDC(hwnd);
			HDC hdc = CreateCompatibleDC(hdcDisplay);
			HBITMAP hbm = CreateCompatibleBitmap(hdcDisplay, rc.right - rc.left, rc.bottom - rc.top);
			SelectBitmap(hdc, hbm);
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
			if(Player_Stream) {
				HFONT hPrevFont = SelectFont(hdc, hFont);
				SIZE size;
				char s[256];
				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, 0x00ffffff);

				// Display status
				if(Player_State == STATE_ERROR  ) strcpy(s, Player_Error);
				if(Player_State == STATE_STOPPED) strcpy(s, "Stopped");
				if(Player_State == STATE_PLAYING) strcpy(s, "Playing");
				if(Player_State == STATE_PAUSED ) strcpy(s, "Paused");
				if(Player_State == STATE_SEEKING) strcpy(s, "Seeking");
				GetTextExtentPoint32(hdc, s, strlen(s), &size);
				TextOut(hdc, 10, (INFOBAR_HEIGHT - size.cy) / 2, s, strlen(s));
				if(Player_Header.Frames > 0) {
					// Display frames
					if(cfg.ShowFrames || Player_Header.FPS == 0) {
						if(Player_Header.Frames > 0)
							sprintf(s, "%8d / %d", Player_Frame, Player_Header.Frames);
						else
							sprintf(s, "%8d", Player_Frame);
					} else {
						int current = Player_Frame / Player_Header.FPS;
						int total = Player_Header.Frames / Player_Header.FPS;
						sprintf(s, "%02d:%02d / %02d:%02d", current/60, current%60, total/60, total%60);
					}
				} else {
					// Frames not available (bad header)
					sprintf(s, "%d %%", 100 * Player_Position / Player_FileSize);
				}
				GetTextExtentPoint32(hdc, s, strlen(s), &size);
				TextOut(hdc, rc.right - size.cx - 10, (INFOBAR_HEIGHT - size.cy) / 2, s, strlen(s));

				SelectFont(hdc, hPrevFont);
			}
			GetClientRect(hwnd, &rc);
			BitBlt(hdcDisplay, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc, 0, 0, SRCCOPY);
			DeleteBitmap(hbm);
			DeleteDC(hdc);
			ReleaseDC(hwnd, hdcDisplay);
			ValidateRect(hwnd, NULL);
			return 0;
		}
		case WM_DESTROY:
			DeleteFont(hFont);
			break;
	}
//	return DefWindowProc(hwnd, uMsg, wParam, lParam);
	CallWindowProc(DefWindowProc, hwnd, uMsg, wParam, lParam);
}

//==========================================================================
// Procedure that creates main window & control window
//==========================================================================

BOOL CreateWindows(HINSTANCE hInstance)
{
	WNDCLASS wcMain, wcControl, wcSeekBar, wcInfoBar;
	
	memset(&wcMain, 0, sizeof(WNDCLASS));
	wcMain.style = CS_HREDRAW | CS_VREDRAW;
	wcMain.lpfnWndProc = MainWindowProc;
	wcMain.hInstance = hInstance;
	wcMain.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcMain.hCursor = LoadCursor(NULL, IDC_HAND);
	wcMain.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcMain.lpszClassName = MAIN_CLASS_NAME;
	RegisterClass(&wcMain);

	memset(&wcControl, 0, sizeof(WNDCLASS));
	wcControl.style = CS_HREDRAW | CS_VREDRAW;
	wcControl.lpfnWndProc = ControlWindowProc;
	wcControl.hInstance = hInstance;
	wcControl.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcControl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcControl.hbrBackground = NULL;
	wcControl.lpszClassName = CONTROL_CLASS_NAME;
	RegisterClass(&wcControl);

	memset(&wcSeekBar, 0, sizeof(WNDCLASS));
	wcSeekBar.style = CS_HREDRAW | CS_VREDRAW;
	wcSeekBar.lpfnWndProc = SeekBarWindowProc;
	wcSeekBar.hInstance = hInstance;
	wcSeekBar.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wcSeekBar.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);	
	wcSeekBar.lpszClassName = SEEKBAR_CLASS_NAME;
	RegisterClass(&wcSeekBar);

	memset(&wcInfoBar, 0, sizeof(WNDCLASS));
	wcInfoBar.style = CS_HREDRAW | CS_VREDRAW;
	wcInfoBar.lpfnWndProc = InfoBarWindowProc;
	wcInfoBar.hInstance = hInstance;
	wcInfoBar.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcInfoBar.hbrBackground = NULL;
	wcInfoBar.lpszClassName = INFOBAR_CLASS_NAME;
	RegisterClass(&wcInfoBar);

	// Create main window
	hMenu = LoadMenu(hAppInstance, MAKEINTRESOURCE(IDR_MENU));
	hAccel = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	hMainWindow = CreateWindowEx(WS_EX_ACCEPTFILES, // | WS_EX_TOPMOST, 
						MAIN_CLASS_NAME, "gpyPlayer", 
						WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // | WS_THICKFRAME,
						CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
						NULL, hMenu, hInstance, NULL);

	// Create control window
	hControlWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME, CONTROL_CLASS_NAME, "", 
						WS_VISIBLE | WS_POPUP,
						CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
						NULL, NULL, hInstance, NULL);

	return (hMainWindow != NULL && hControlWindow != NULL);
}

//==========================================================================
// Program entry
//==========================================================================

#include <process.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HANDLE  hMutex = NULL;
	MSG		msg;
	//
	// Check previous instance
	//
	hMutex = CreateMutex(NULL,TRUE,"gpuPlayer");
	if(GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL,"Already running!", "Error", MB_ICONERROR);
		if(hMutex) ReleaseMutex(hMutex);
		return 0;
	}
	hAppInstance = hInstance;
	InitCommonControls();
	LoadConfiguration();
	//
	// Create main window
	//
	if(!CreateWindows(hInstance)) {
		MessageBox(NULL, "Failed to create main window", "Error", MB_ICONERROR);
		if(hMutex) ReleaseMutex(hMutex);
		return 0;		
	}
	//
	// Begin message pump
	//
	AppActive = TRUE;
	while(AppActive) {
		if(PeekMessage(&msg, hMainWindow, 0, 0, PM_NOREMOVE)) {
			GetMessage(&msg, hMainWindow, 0, 0);
			if(msg.message == WM_COMMAND && LOWORD(msg.wParam) == IDM_FILE_CLOSE) {
				ActivateBreak = TRUE;
			}
			TranslateAccelerator(hMainWindow, hAccel, &msg);
			DispatchMessage(&msg);
		} else if(PeekMessage(&msg, hControlWindow, 0, 0, PM_NOREMOVE)) {
			GetMessage(&msg, hControlWindow, 0, 0);
			TranslateAccelerator(hMainWindow, hAccel, &msg);
			DispatchMessage(&msg);
		} else {
			if(Player_State == STATE_PLAYING) {
				static DWORD last_frame = 0;
				PLAYER_Play(hMainWindow);
				if(last_frame != Player_Frame) {
					RefreshControls();
					last_frame = Player_Frame;
				}
			} else {
				GetMessage(&msg, NULL, 0, 0);
				TranslateAccelerator(hMainWindow, hAccel, &msg);
				DispatchMessage(&msg);
			}
		}
	}
	//
	// Finish
	//
	if(hMutex) ReleaseMutex(hMutex);
	PLAYER_Close(NULL);
	FreePlugins();
	return 0;
}
