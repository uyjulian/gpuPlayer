/***************************************************************************
      File Name:	Options.cpp
   File Created:	Wednesday, October 8th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "config.h"
#include "externals.h"
#include "folder.h"
#include "tabctrl.h"
#include "winmain.h"

BOOL CALLBACK Options_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL Options_OnInit(HWND hW);
void Options_OnOK(HWND hW);
void Options_OnCancel(HWND hW);

BOOL CALLBACK Playback_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==========================================================================
// Main options dialog
//==========================================================================

BOOL CALLBACK Options_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG:
			return Options_OnInit(hW);
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDCANCEL:		Options_OnCancel(hW);	return TRUE;
				case IDOK:			Options_OnOK(hW);		return TRUE;
//				case IDC_POMOC:
//					switch (TabCtrl_GetCurSel(GetDlgItem(hW,IDC_TAB))) {
//						case 0: HtmlHelpExecute(hW, HELP_FILE, HH_DISPLAY_TOPIC, (DWORD)"html\\opcije-generalno.htm");	break;
//				}
				return TRUE;
			}
		}
		case WM_NOTIFY: {
			switch(LOWORD(wParam)) {
				case IDC_TAB:
					if((LPNMHDR(lParam)->code)==TCN_SELCHANGE)
						TabCtrl_OnSelChanged(); return TRUE;
					break;
			}
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

BOOL Options_OnInit(HWND hW)
{
	LoadConfiguration();
	TabCtrl_Reset();
	TabCtrl_Insert("Playback", IDD_OPTIONS_PLAYBACK, Playback_DlgProc);
	TabCtrl_Init(hAppInstance, hW, GetDlgItem(hW, IDC_TAB));
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

void Options_OnOK(HWND hW)
{
	SaveConfiguration();
	EndDialog(hW, TRUE);
}

////////////////////////////////////////////////////////////////////////////

void Options_OnCancel(HWND hW)
{
	LoadConfiguration();
	EndDialog(hW, FALSE);
}

//==========================================================================
// Playback options
//==========================================================================

#define MAX_PLUGINS 100

char VideoPlugins[MAX_PATH][MAX_PLUGINS];
char SoundPlugins[MAX_PATH][MAX_PLUGINS];

void Playback_SearchPlugins(HWND hW, char *SearchPath, int &SelectedVideoPlugin, int &SelectedSoundPlugin)
{
	WIN32_FIND_DATA FindData;
	HANDLE hFindFile;
	HMODULE hModule;
	char SearchFile[255];
	memset(&FindData, 0, sizeof(FindData));
	sprintf(SearchFile, "%s*.dll", SearchPath);
	if((hFindFile = FindFirstFile(SearchFile, &FindData)) != NULL) {
		do {
			char FileName[MAX_PATH];
			sprintf(FileName, "%s%s", SearchPath, FindData.cFileName);
			if((hModule = LoadLibrary(FileName)) != NULL) {
				if(((FARPROC&)PSEgetLibType = GetProcAddress(hModule, "PSEgetLibType")) != NULL)
					if(((FARPROC&)PSEgetLibName = GetProcAddress(hModule, "PSEgetLibName")) != NULL)
						if(((FARPROC&)PSEgetLibVersion = GetProcAddress(hModule, "PSEgetLibVersion")) != NULL) {

							if(PSEgetLibType() == PSE_LT_GPU) {
								if(strcmp(PSEgetLibName(), GPU_LIBRARY_NAME)) {
									unsigned long version = PSEgetLibVersion();
									HWND hWC = GetDlgItem(hW, IDC_GPU_PLUGIN);
									char buffer[MAX_PATH];
									int Index;
									sprintf(buffer,"%s %d.%d", PSEgetLibName(),(version>>8)&0xff,version&0xff);
									if(ComboBox_GetCount(hWC) < MAX_PLUGINS) {
										ComboBox_AddString(hWC, buffer);
										Index = ComboBox_GetCount(hWC) - 1;
										sprintf(VideoPlugins[Index], "%s%s", SearchPath, FindData.cFileName);
										if(stricmp(VideoPlugins[Index], cfg.VideoPlugin)==0) SelectedVideoPlugin = Index;
									}
								}

							} else if(PSEgetLibType() == PSE_LT_SPU) {
								if(strcmp(PSEgetLibName(), SPU_LIBRARY_NAME)) {
									unsigned long version = PSEgetLibVersion();
									HWND hWC = GetDlgItem(hW, IDC_SPU_PLUGIN);
									char buffer[MAX_PATH];
									int Index;
									sprintf(buffer,"%s %d.%d", PSEgetLibName(),(version>>8)&0xff,version&0xff);
									if(ComboBox_GetCount(hWC) < MAX_PLUGINS) {
										ComboBox_AddString(hWC, buffer);
										Index = ComboBox_GetCount(hWC) - 1;
										sprintf(SoundPlugins[Index], "%s%s", SearchPath, FindData.cFileName);
										if(stricmp(SoundPlugins[Index], cfg.SoundPlugin)==0) SelectedSoundPlugin = Index;
									}
								}
							}
							
						}
				FreeLibrary(hModule);
			}
		} while(FindNextFile(hFindFile, &FindData));
		FindClose(hFindFile);
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnGPUTest(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_GPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(VideoPlugins[Index])) != NULL) {
			if(((FARPROC&)GPUtest = GetProcAddress(hModule, "GPUtest")) != NULL) {
				long ret = GPUtest();
				if(ret==0)
					MessageBox(hW, "Plugin should be working fine !", "Test", MB_ICONINFORMATION);
				else if(ret<0)
					MessageBox(hW, "Plugin returned an error !", "Test", MB_ICONINFORMATION);
				else
					MessageBox(hW, "Plugin returned a warning !", "Test", MB_ICONINFORMATION);
			}
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnGPUConfig(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_GPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(VideoPlugins[Index])) != NULL) {
			if(((FARPROC&)GPUconfigure = GetProcAddress(hModule, "GPUconfigure")) != NULL)
				GPUconfigure();
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnGPUAbout(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_GPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(VideoPlugins[Index])) != NULL) {
			if(((FARPROC&)GPUabout = GetProcAddress(hModule, "GPUabout")) != NULL)
				GPUabout();
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnSPUTest(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_SPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(SoundPlugins[Index])) != NULL) {
			if(((FARPROC&)SPUtest = GetProcAddress(hModule, "SPUtest")) != NULL) {
				long ret = SPUtest();
				if(ret==0)
					MessageBox(hW, "Plugin should be working fine !", "Test", MB_ICONINFORMATION);
				else if(ret<0)
					MessageBox(hW, "Plugin returned an error !", "Test", MB_ICONINFORMATION);
				else
					MessageBox(hW, "Plugin returned a warning !", "Test", MB_ICONINFORMATION);
			}
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnSPUConfig(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_SPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(SoundPlugins[Index])) != NULL) {
			if(((FARPROC&)SPUconfigure = GetProcAddress(hModule, "SPUconfigure")) != NULL)
				SPUconfigure();
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void Playback_OnSPUAbout(HWND hW) 
{
	int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_SPU_PLUGIN));
	if(Index >= 0) {
		HMODULE hModule;
		if((hModule = LoadLibrary(SoundPlugins[Index])) != NULL) {
			if(((FARPROC&)SPUabout = GetProcAddress(hModule, "SPUabout")) != NULL)
				SPUabout();
			FreeLibrary(hModule);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK Playback_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG: {
			int SelectedVideoPlugin = -1, SelectedSoundPlugin = -1;
			char SearchPath[MAX_PATH];
			HWND hWC_GPU = GetDlgItem(hW, IDC_GPU_PLUGIN);
			HWND hWC_SPU = GetDlgItem(hW, IDC_SPU_PLUGIN);
			AppPath(SearchPath, sizeof(SearchPath));
			ComboBox_ResetContent(hWC_GPU);
			ComboBox_ResetContent(hWC_SPU);
			Playback_SearchPlugins(hW, SearchPath, SelectedVideoPlugin, SelectedSoundPlugin);
			strcat(SearchPath, "plugins\\");
			Playback_SearchPlugins(hW, SearchPath, SelectedVideoPlugin, SelectedSoundPlugin);

			if(ComboBox_GetCount(hWC_GPU) > 0) {
				if(SelectedVideoPlugin >= 0 && SelectedVideoPlugin < ComboBox_GetCount(hWC_GPU))
					ComboBox_SetCurSel(hWC_GPU, SelectedVideoPlugin);
				else
					ComboBox_SetCurSel(hWC_GPU, 0);
			} else {
				EnableWindow(GetDlgItem(hW, IDC_GPU_TEST), FALSE);
				EnableWindow(GetDlgItem(hW, IDC_GPU_CONFIG), FALSE);
				EnableWindow(GetDlgItem(hW, IDC_GPU_ABOUT), FALSE);
				EnableWindow(GetDlgItem(GetParent(GetParent(hW)), IDOK), FALSE);
			}

			if(ComboBox_GetCount(hWC_SPU) > 0) {
				if(SelectedSoundPlugin >= 0 && SelectedSoundPlugin < ComboBox_GetCount(hWC_SPU))
					ComboBox_SetCurSel(hWC_SPU, SelectedSoundPlugin);
				else
					ComboBox_SetCurSel(hWC_SPU, 0);
			} else {
				EnableWindow(GetDlgItem(hW, IDC_SOUND_ENABLED), FALSE);
				EnableWindow(GetDlgItem(hW, IDC_SPU_TEST), FALSE);
				EnableWindow(GetDlgItem(hW, IDC_SPU_CONFIG), FALSE);
				EnableWindow(GetDlgItem(hW, IDC_SPU_ABOUT), FALSE);
				EnableWindow(GetDlgItem(GetParent(GetParent(hW)), IDOK), FALSE);
			}
			Button_SetCheck(GetDlgItem(hW, IDC_SOUND_ENABLED), cfg.SoundEnabled);
			
			Button_SetCheck(GetDlgItem(hW, IDC_SHOW_FRAMES), cfg.ShowFrames);
			Button_SetCheck(GetDlgItem(hW, IDC_SHOW_TIME),   !cfg.ShowFrames);

			Button_SetCheck(GetDlgItem(hW, IDC_REPLAY), cfg.Replay);

			return TRUE;
		}
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_GPU_PLUGIN: {
					int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_GPU_PLUGIN));
					if(Index >= 0) strcpy(cfg.VideoPlugin, VideoPlugins[Index]);
					return TRUE;
				}
				case IDC_GPU_TEST:		Playback_OnGPUTest(hW);		return TRUE;
				case IDC_GPU_CONFIG:	Playback_OnGPUConfig(hW);	return TRUE;
				case IDC_GPU_ABOUT:		Playback_OnGPUAbout(hW);	return TRUE;
				case IDC_SPU_PLUGIN: {
					int Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_SPU_PLUGIN));
					if(Index >= 0) strcpy(cfg.SoundPlugin, SoundPlugins[Index]);
					return TRUE;
				}
				case IDC_SPU_TEST:		Playback_OnSPUTest(hW);		return TRUE;
				case IDC_SPU_CONFIG:	Playback_OnSPUConfig(hW);	return TRUE;
				case IDC_SPU_ABOUT:		Playback_OnSPUAbout(hW);	return TRUE;
				case IDC_SOUND_ENABLED:
					cfg.SoundEnabled = Button_GetCheck(GetDlgItem(hW, IDC_SOUND_ENABLED));
					return TRUE;
				case IDC_SHOW_FRAMES:
					cfg.ShowFrames = Button_GetCheck(GetDlgItem(hW, IDC_SHOW_FRAMES));
				case IDC_SHOW_TIME:
					cfg.ShowFrames = !Button_GetCheck(GetDlgItem(hW, IDC_SHOW_TIME));
					return TRUE;
				case IDC_REPLAY:
					cfg.Replay = Button_GetCheck(GetDlgItem(hW, IDC_REPLAY));
					return TRUE;				
			}
			return TRUE;
	}
	return FALSE;
}
