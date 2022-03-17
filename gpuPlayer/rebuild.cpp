/***************************************************************************
      File Name:	Rebuild.cpp
   File Created:	Sunday, October 12th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "config.h"
#include "externals.h"
#include "folder.h"
#include "winmain.h"
#include "player.h"

#define BZ_NO_STDIO
#include "zlib/zlib.h"
#include "bzip/bzlib.h"

typedef struct tagCompressionType {
	CHAR Name[MAX_PATH];
	INT  Type;
	INT  Level;
	} COMPRESSIONTYPE, *LPCOMPRESSIONTYPE;
	
COMPRESSIONTYPE Rebuild_CompressionTypes[] = {
	{ "None",           COMPRESSION_NONE, 0                     },
	{ "ZLIB (default)", COMPRESSION_ZLIB, Z_DEFAULT_COMPRESSION },
	{ "ZLIB (best)",    COMPRESSION_ZLIB, Z_BEST_COMPRESSION    },
	{ "BZIP",           COMPRESSION_BZIP, 0                     },
	{ "", 0, 0 }
	};

INT Rebuild_BlockSizes[] = { 0, 1024, 4096, 16384, 65536, 262144, -1 };

BOOL Rebuild_OverwriteWarning = TRUE;
CHAR Rebuild_SourceFileName[MAX_PATH];
CHAR Rebuild_DestinationFileName[MAX_PATH];
CHAR Rebuild_CompressionName[MAX_PATH];
INT  Rebuild_CompressionType;
INT  Rebuild_CompressionLevel;
INT  Rebuild_BlockSize;
INT  Rebuild_Sounds;

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK Rebuild_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Rebuild_OnInit(HWND hW);
void Rebuild_OnSourceBrowse(HWND hW);
void Rebuild_OnDestinationBrowse(HWND hW);
void Rebuild_OnOK(HWND hW);

BOOL CALLBACK Rebuild_ProgressDlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam);

//==========================================================================
// Helper functions
//==========================================================================

void Rebuild_ClearDestinationInfo(HWND hW)
{
	SetWindowText(GetDlgItem(hW, IDC_INFO_VERSION),     "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_COMPRESSION), "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_FRAMES),      "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_FPS),         "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_VIDEO),       "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_SOUND),       "-");
	SetWindowText(GetDlgItem(hW, IDC_INFO_COMPRESSED),  "-");
}

LPSTR Rebuild_GetCompressionType(DWORD Compression)
{
	if(Compression == COMPRESSION_NONE) return "None";
	if(Compression == COMPRESSION_ZLIB) return "ZLIB";
	if(Compression == COMPRESSION_BZIP) return "BZIP";
	return "Unknown";
}

void Rebuild_EnableControls(HWND hW)
{
	CHAR buffer[MAX_PATH];
	EnableWindow(GetDlgItem(hW, IDOK),
		(GetWindowText(GetDlgItem(hW, IDC_SOURCE_FILE), buffer, MAX_PATH) > 0 && 
		GetWindowText(GetDlgItem(hW, IDC_DESTINATION_FILE), buffer, MAX_PATH) > 0 ));
	DWORD Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_COMPRESSION));
	EnableWindow(GetDlgItem(hW, IDC_BLOCK_SIZE), (Rebuild_CompressionTypes[Index].Type != 0));
}

//==========================================================================
// Main options dialog
//==========================================================================

BOOL CALLBACK Rebuild_DlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG:   Rebuild_OnInit(hW); return TRUE;
		case WM_COMMAND: {
			switch(LOWORD(wParam)) {
				case IDC_SOURCE_BROWSE:			Rebuild_OnSourceBrowse(hW); return TRUE;
				case IDC_DESTINATION_BROWSE:	Rebuild_OnDestinationBrowse(hW); return TRUE;
				case IDC_COMPRESSION:
					Rebuild_EnableControls(hW);
					return TRUE;
				case IDCANCEL:		EndDialog(hW, TRUE);	return TRUE;
				case IDOK:			Rebuild_OnOK(hW);		return TRUE;
//				case IDC_POMOC:
//					switch (TabCtrl_GetCurSel(GetDlgItem(hW,IDC_TAB))) {
//						case 0: HtmlHelpExecute(hW, HELP_FILE, HH_DISPLAY_TOPIC, (DWORD)"html\\opcije-generalno.htm");	break;
//				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_OnInit(HWND hW)
{
//	SetWindowText(GetDlgItem(hW, IDC_SOURCE_FILE), "D:\\Test\\rec_trex_v20_compr.rec");
//	SetWindowText(GetDlgItem(hW, IDC_DESTINATION_FILE), "D:\\Test\\test.rec");

	Rebuild_ClearDestinationInfo(hW);

	// Fill Compression Types
	HWND hWC = GetDlgItem(hW, IDC_COMPRESSION);
	ComboBox_ResetContent(hWC);
	int Index = 0;
	while(strlen(Rebuild_CompressionTypes[Index].Name))
		ComboBox_AddString(hWC, Rebuild_CompressionTypes[Index++].Name);
	ComboBox_SetCurSel(hWC, 0);

	// Fill Block Sizes
	hWC = GetDlgItem(hW, IDC_BLOCK_SIZE);
	ComboBox_ResetContent(hWC);
	Index = 0;
	while(Rebuild_BlockSizes[Index] >= 0)
		ComboBox_AddString(hWC, FormatLong(Rebuild_BlockSizes[Index++]));
	ComboBox_SetCurSel(hWC, 1);
	
	Button_SetCheck(GetDlgItem(hW, IDC_REBUILD_SOUND), TRUE);
	Rebuild_EnableControls(hW);
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_OnSourceBrowse(HWND hW)
{
	OPENFILENAME ofn;
	char buffer[MAX_PATH];
	memset(&ofn,0,sizeof(ofn));
	memset(buffer,0,sizeof(buffer));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hW;
	ofn.lpstrFilter = "REC files\x00*.REC\x00\x00";
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select Source File";
	ofn.Flags = OFN_FILEMUSTEXIST;
	GetOpenFileName(&ofn);
	if(strlen(buffer) > 0) {
		FILE* fp = fopen(buffer, "rb");
		if(fp == NULL) { MessageBox(hW, "Could not open file!", "Error", MB_ICONERROR); return; }
		DWORD dwTAG, dwVersion;
		fread(&dwTAG, 4, 1, fp);
		if(dwTAG != ID_TAG) { fclose(fp); MessageBox(hW, "Incorrect file format!", "Error", MB_ICONERROR); return; }
		fread(&dwVersion, 4, 1, fp);
		if(dwVersion == 0x0100) {
			DWORD dwFrames, dwFPS;
			fread(&dwFrames, 4, 1, fp);
			fread(&dwFPS, 4, 1, fp);
			Rebuild_ClearDestinationInfo(hW);
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_VERSION), "%d.%d", HIBYTE(dwVersion), LOBYTE(dwVersion));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FRAMES),  "%d"   , dwFrames);
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FPS),     "%d"   , dwFPS);
			
		} else if(dwVersion == 0x0200) {
			HEADER hdr;
			fread(&hdr, sizeof(HEADER), 1, fp);
			Rebuild_ClearDestinationInfo(hW);
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_VERSION),     "%d.%d", HIBYTE(dwVersion), LOBYTE(dwVersion));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_COMPRESSION), "%s", Rebuild_GetCompressionType(hdr.Compression));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FRAMES),      "%s", FormatLong(hdr.Frames));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FPS),         "%d", hdr.FPS);
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_VIDEO),       "%s kB", FormatDouble((double)hdr.VideoDataSize / 1000, 2));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_SOUND),       "%s kB", FormatDouble((double)hdr.SoundDataSize / 1000, 2));
			SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_COMPRESSED),  "%s kB (%d%%)", FormatDouble((double)hdr.CompressedDataSize / 1000, 2),
				100 - (int)((float)100 * (float)hdr.CompressedDataSize / (float)(hdr.VideoDataSize + hdr.SoundDataSize)));

		} else {
			fclose(fp);
			MessageBox(hW, "Incorrect file format!", "Error", MB_ICONERROR);
			return;
		}
		SetWindowText(GetDlgItem(hW, IDC_SOURCE_FILE), buffer);
		fclose(fp);
	}
	Rebuild_EnableControls(hW);
	return;
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_OnDestinationBrowse(HWND hW)
{
	OPENFILENAME ofn;
	char buffer[MAX_PATH];
	memset(&ofn,0,sizeof(ofn));
	memset(buffer,0,sizeof(buffer));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hW;
	ofn.lpstrFilter = "REC files\x00*.REC\x00\x00";
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select Destination File";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOREADONLYRETURN;
	GetSaveFileName(&ofn);
	if(strlen(buffer) > 0) {
		int len = strlen(buffer);
		if(len < 3) strcat(buffer, ".rec");
		else if(buffer[len-4] != '.' && 
				(buffer[len-3] != 'r' || buffer[len-3] != 'R') &&
				(buffer[len-2] != 'e' || buffer[len-2] != 'E') &&
				(buffer[len-1] != 'c' || buffer[len-1] != 'C')) 
			strcat(buffer, ".rec");
		SetWindowText(GetDlgItem(hW, IDC_DESTINATION_FILE), buffer);
		Rebuild_OverwriteWarning = FALSE;
	}
	Rebuild_EnableControls(hW);
	return ;
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_OnOK(HWND hW)
{
	GetWindowText(GetDlgItem(hW, IDC_SOURCE_FILE), Rebuild_SourceFileName, MAX_PATH);
	GetWindowText(GetDlgItem(hW, IDC_DESTINATION_FILE), Rebuild_DestinationFileName, MAX_PATH);
	INT Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_COMPRESSION));
	strcpy(Rebuild_CompressionName, Rebuild_CompressionTypes[Index].Name);
	Rebuild_CompressionType = Rebuild_CompressionTypes[Index].Type;
	Rebuild_CompressionLevel = Rebuild_CompressionTypes[Index].Level;
	Index = ComboBox_GetCurSel(GetDlgItem(hW, IDC_BLOCK_SIZE));
	Rebuild_BlockSize = Rebuild_BlockSizes[Index];
	if(Rebuild_BlockSize == 0) Rebuild_BlockSize = 1;
	Rebuild_Sounds = Button_GetCheck(GetDlgItem(hW, IDC_REBUILD_SOUND));
	
	if(stricmp(Rebuild_SourceFileName, Rebuild_DestinationFileName) == 0) {
		MessageBox(hW, "Must select different file names for source and destination !", "Error", MB_ICONERROR);
		return;
	}
	
	if(Rebuild_OverwriteWarning) {
		FILE* fp = fopen(Rebuild_DestinationFileName, "rb");
		if(fp != NULL) {
			fclose(fp);
			if(MessageBoxFormat(hW, "Destination file name\n%s\nalready exists ! Overwrite ?", "Warning", MB_ICONWARNING | MB_YESNO, Rebuild_DestinationFileName) != IDYES) return;
		}
	}
	Rebuild_OverwriteWarning = TRUE;
	DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_REBUILD_PROGRESS), hW, Rebuild_ProgressDlgProc);
}

//==========================================================================
//
// File rebuilding functions
//
//==========================================================================

DWORD  Rebuild_ThreadID = 0;
BOOL   Rebuild_ThreadRunning = FALSE;
BOOL   Rebuild_Abort = FALSE;
DWORD  Rebuild_SourceVersion;
HEADER Rebuild_SourceHeader;
FILE*  Rebuild_SourceStream = NULL;
DWORD  Rebuild_SourceFileSize;
DWORD  Rebuild_SourcePosition;
HEADER Rebuild_Header;
DWORD  Rebuild_Position;
DWORD  Rebuild_IndexOffset;
FILE*  Rebuild_Stream = NULL;
DWORD  Rebuild_Finished;

BYTE  Rebuild_CompressBuffer[0x200000];
DWORD Rebuild_ReadBuffer[0x80000];
DWORD Rebuild_ReadBufferSize;
DWORD Rebuild_ReadBufferPos;
BYTE  Rebuild_WriteBuffer[0x200000];
DWORD Rebuild_WriteBufferPos;
DWORD Rebuild_WriteBufferMaxSize;
DWORD Rebuild_IndexData[0x80000];

DWORD WINAPI Rebuild_ThreadProc(HWND hW);
BOOL Rebuild_Open(HWND hW);
void Rebuild_Close(HWND hW);
BOOL Rebuild_ReadData(HWND hW);
void Rebuild_WriteData(HWND hW, BYTE flags, DWORD iSize, LPVOID pMem);

BOOL CALLBACK Rebuild_ProgressDlgProc(HWND hW, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		case WM_INITDIALOG: {
			Rebuild_Abort = FALSE;
			Rebuild_ThreadRunning = FALSE;
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Rebuild_ThreadProc, hW, 0, &Rebuild_ThreadID);
			return TRUE;
		}
		case WM_COMMAND:
			case IDCANCEL:
				if(Rebuild_ThreadRunning) 
					Rebuild_Abort = TRUE;
				else
					EndDialog(hW, !Rebuild_Abort);
				return TRUE;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////

DWORD WINAPI Rebuild_ThreadProc(HWND hW)
{
	Rebuild_ThreadRunning = TRUE;
	SetWindowText(GetDlgItem(hW, IDCANCEL), "Abort");
	if(Rebuild_Open(hW)) {
		HWND hWC = GetDlgItem(hW, IDC_PROGRESS);
		DWORD StartTick = GetTickCount();
		DWORD LastTick = 0, ElapsedTick = 0, Sec = 0;
		double Percent = 0;
		ProgressBar_SetRange(hWC, 0, 100);	
		ProgressBar_SetPos(hWC, 0)
		Rebuild_Finished = FALSE;
		while(Rebuild_Abort == FALSE && Rebuild_Finished == FALSE) {
			Rebuild_ReadData(hW);
			if((GetTickCount() - LastTick) >= 500) {
				LastTick = GetTickCount();
				SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FRAMES),      "%s", FormatLong(Rebuild_Header.Frames));
				SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_FPS),         "%d", Rebuild_Header.FPS);
				if(Rebuild_Header.VideoDataSize > 0)
					SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_VIDEO),       "%s kB", FormatDouble((double)Rebuild_Header.VideoDataSize / 1000, 2));
				if(Rebuild_Header.SoundDataSize > 0)
					SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_SOUND),       "%s kB", FormatDouble((double)Rebuild_Header.SoundDataSize / 1000, 2));
				if(Rebuild_Header.CompressedDataSize > 0)
					SetWindowTextFormat(GetDlgItem(hW, IDC_INFO_COMPRESSED),  "%s kB (%d%%)", FormatDouble((double)Rebuild_Header.CompressedDataSize / 1000, 2),
						100 - (int)((float)100 * (float)Rebuild_Header.CompressedDataSize / (float)(Rebuild_Header.VideoDataSize + Rebuild_Header.SoundDataSize)));			
				Percent = (double)Rebuild_SourcePosition / (double)Rebuild_SourceFileSize;
				ProgressBar_SetPos(hWC, (DWORD)(100 * Percent));
				ElapsedTick = LastTick - StartTick; Sec = ElapsedTick / 1000;
				SetWindowTextFormat(GetDlgItem(hW, IDC_ELAPSED_TIME), "%02d:%02d:%02d", Sec / 3600, (Sec / 60) % 60, Sec % 60);
				Sec = (DWORD)((double)Sec / Percent) - Sec;
				SetWindowTextFormat(GetDlgItem(hW, IDC_REMAINING_TIME), "%02d:%02d:%02d", Sec / 3600, (Sec / 60) % 60, Sec % 60);
			}
			Sleep(0);
		}
	} else {
		MessageBox(hW, "Error opening file !", "Error", MB_ICONERROR);
	}
	Rebuild_Close(hW);
	Beep(0, 0);
	Rebuild_ThreadRunning = FALSE;
	SetWindowText(GetDlgItem(hW, IDCANCEL), "Exit");
	return 0;
}

////////////////////////////////////////////////////////////////////////////

BOOL Rebuild_GetVersion(LPDWORD dwVersion)
{
	if(Rebuild_SourceStream == NULL) return 0;
	DWORD dwTAG;
	fseek(Rebuild_SourceStream, 0, SEEK_SET);
	fread(&dwTAG, 4, 1, Rebuild_SourceStream);
	fread(dwVersion, 4, 1, Rebuild_SourceStream);
	if(dwTAG != ID_TAG) return FALSE;
	return TRUE;
}

BOOL Rebuild_Open(HWND hW)
{
	memset(&Rebuild_SourceHeader, 0, sizeof(Rebuild_SourceHeader));

	// Open REC file
	Rebuild_SourceStream = fopen(Rebuild_SourceFileName, "rb");
	if(Rebuild_SourceStream == NULL) return FALSE;
	fseek(Rebuild_SourceStream, 0, SEEK_END);
	Rebuild_SourceFileSize = ftell(Rebuild_SourceStream);
	fseek(Rebuild_SourceStream, 0, SEEK_SET);

	// Check version
	if(!Rebuild_GetVersion(&Rebuild_SourceVersion)) {
		fclose(Rebuild_SourceStream);
		Rebuild_SourceStream = NULL;
		MessageBox(hW, "Unsupported file type !", "Error", MB_ICONERROR);
		return FALSE;
	}
	
	// Read header
	if(Rebuild_SourceVersion == 0x0100) {

		int file_size;
		fread(&Rebuild_SourceHeader.Frames, 4, 1, Rebuild_SourceStream);
		fread(&Rebuild_SourceHeader.FPS, 4, 1, Rebuild_SourceStream);
		fread(&file_size, 4, 1, Rebuild_SourceStream);
		Rebuild_IndexOffset = 0;

	} else if(Rebuild_SourceVersion == 0x0200) {

		fread(&Rebuild_SourceHeader, sizeof(HEADER), 1, Rebuild_SourceStream);
		Rebuild_IndexOffset = Rebuild_SourceHeader.IndexOffset;

	} else {
		fclose(Rebuild_SourceStream);
		Rebuild_SourceStream = NULL;
		MessageBoxFormat(hW, "Unsupported file version %d.%d !\n\nPlayer supports versions 1.0 and 2.0", "Error", MB_ICONERROR, HIBYTE(Rebuild_SourceVersion), LOBYTE(Rebuild_SourceVersion));
		return FALSE;
	}

	// Initialize reading
	memset(&Rebuild_Header, 0, sizeof(Rebuild_Header));
	Rebuild_IndexOffset = 0;
	Rebuild_SourcePosition = ftell(Rebuild_SourceStream);
	SetWindowText(GetDlgItem(hW, IDC_INFO_COMPRESSION), Rebuild_CompressionName);

	Rebuild_ReadBufferSize = 0;
	Rebuild_ReadBufferPos = 0;
	
	// Initialize writting
	BYTE FileID[8] = {'R', 'E', 'C',  0 ,  0 ,  2 ,  0 ,  0 };

	Rebuild_Stream = fopen(Rebuild_DestinationFileName, "wb");
	if(Rebuild_Stream == NULL) {
		MessageBox(hW, "Error creating output file!", "Error", MB_ICONERROR);
		return FALSE;
	}
	fwrite(&FileID, 8, 1, Rebuild_Stream);
	fwrite(&Rebuild_Header, sizeof(HEADER), 1, Rebuild_Stream);	
	Rebuild_Header.Compression = Rebuild_CompressionType;
	Rebuild_Header.FPS = Rebuild_SourceHeader.FPS;
	Rebuild_Position = ftell(Rebuild_Stream);
	Rebuild_WriteBufferPos = 0;
	Rebuild_WriteBufferMaxSize = sizeof(Rebuild_WriteBuffer) - 1;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_Close(HWND hW)
{
	if(Rebuild_SourceStream) {
		fclose(Rebuild_SourceStream);
		Rebuild_SourceStream = NULL;
	}
	if(Rebuild_Stream) {
		Rebuild_Header.IndexOffset = Rebuild_Position;
		fwrite(Rebuild_IndexData, 4*Rebuild_Header.Frames, 1, Rebuild_Stream);
		fseek(Rebuild_Stream, 8, SEEK_SET);
		fwrite(&Rebuild_Header, sizeof(HEADER), 1, Rebuild_Stream);
		fclose(Rebuild_Stream);
		Rebuild_Stream = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////

BOOL Rebuild_ReadData(HWND hW)
{
	BOOL WriteData = TRUE;
	BYTE  flags = 0;
	DWORD iSize = 0;
	LPDWORD data = (LPDWORD)Rebuild_ReadBuffer;

	if(Rebuild_SourceVersion == 0x0100) {	
		//
		// Play Version 1.0
		//
	
		if(Rebuild_SourcePosition >= Rebuild_SourceFileSize) {
			Rebuild_Finished = TRUE;
			return FALSE;
		}
		
		fread(&flags, 1, 1, Rebuild_SourceStream);
		switch((int)flags) {
			case GPU_WRITEDATA:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				flags = GPU_WRITEDATA | RF_WRITENUMBER;
				break;	
			case GPU_WRITEDATAMEM:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				fread(data, iSize, 1, Rebuild_SourceStream);
				flags = GPU_WRITEDATAMEM | RF_WRITEBUFFER;
				break;
			case GPU_READDATA:
				break;	
			case GPU_READDATAMEM:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				flags = GPU_READDATAMEM | RF_WRITENUMBER;
				break;
			case GPU_DMACHAIN:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				fread(data, iSize, 1, Rebuild_SourceStream);
				flags = GPU_DMACHAIN | RF_WRITEBUFFER;
				break;
			case GPU_SETMODE:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				flags = GPU_SETMODE | RF_WRITENUMBER;
				break;
			case GPU_WRITESTATUS:
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				flags = GPU_WRITESTATUS | RF_WRITENUMBER;
				break;
			case GPU_UPDATELACE:
				flags = GPU_UPDATELACE;
				break;
			case GPU_FREEZE:
				iSize = sizeof(GPUFreeze_t);
				fread(data, iSize, 1, Rebuild_SourceStream);
				flags = GPU_FREEZE | RF_WRITEBUFFER;
				break;
			default:
				WriteData = FALSE;
		}

	} else if(Rebuild_SourceVersion == 0x0200) {
		//
		// Play Version 2.0
		//
		if(Rebuild_SourcePosition >= Rebuild_SourceFileSize) { 
			Rebuild_Finished = TRUE; 
			return FALSE; 
		}
		if(Rebuild_SourceHeader.IndexOffset > 0 && Rebuild_SourcePosition >= Rebuild_SourceHeader.IndexOffset) {
			Rebuild_Finished = TRUE;
			return FALSE;
		}


		if(Rebuild_SourceHeader.Compression) {
			// read compressed data			
			if(Rebuild_ReadBufferPos >= Rebuild_ReadBufferSize) {
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				fread(Rebuild_CompressBuffer, iSize, 1, Rebuild_SourceStream);
				Rebuild_ReadBufferSize = sizeof(Rebuild_ReadBuffer);
				Rebuild_ReadBufferPos = 0;
				int result = 0;
				if(Rebuild_SourceHeader.Compression == COMPRESSION_ZLIB)
					result = uncompress((Bytef *)Rebuild_ReadBuffer, &Rebuild_ReadBufferSize, (Bytef *)Rebuild_CompressBuffer, iSize);
				else
					result = BZ2_bzBuffToBuffDecompress((char*)Rebuild_ReadBuffer, (unsigned int*)&Rebuild_ReadBufferSize, (char*)Rebuild_CompressBuffer, iSize, 0, 0);
				if(result != Z_OK) {
					MessageBox(hW, "Error while decompressing...", "Error", MB_ICONERROR);
					Rebuild_ReadBufferSize = 0;
					return FALSE;
				}
			}
			LPBYTE start = &((LPBYTE)Rebuild_ReadBuffer)[Rebuild_ReadBufferPos];
			flags = *start; 
			Rebuild_ReadBufferPos++; start++;
			if(flags & RF_WRITENUMBER) {
				iSize = *((LPDWORD)start);
				Rebuild_ReadBufferPos += 4; start += 4;
			} else if(flags & RF_WRITEBUFFER) {
				iSize = *((LPDWORD)start);
				Rebuild_ReadBufferPos += 4; start += 4;
				data = (LPDWORD)start;
				Rebuild_ReadBufferPos += iSize;
			}

		} else {
			// read uncompressed data
			fread(&flags, 1, 1, Rebuild_SourceStream);
			if(flags & RF_WRITENUMBER) {
				fread(&iSize, 4, 1, Rebuild_SourceStream);
			} else if(flags & RF_WRITEBUFFER) {
				fread(&iSize, 4, 1, Rebuild_SourceStream);
				fread(Rebuild_ReadBuffer, iSize, 1, Rebuild_SourceStream);
			}
			data = Rebuild_ReadBuffer;
		}

		switch(flags & 0x3f) {
			case GPU_WRITEDATA:
			case GPU_WRITEDATAMEM:
			case GPU_READDATA:
			case GPU_READDATAMEM:
			case GPU_DMACHAIN:
			case GPU_SETMODE:
			case GPU_WRITESTATUS:
			case GPU_UPDATELACE:
			case GPU_FREEZE:
				break;
			case SPU_READREGISTER:
			case SPU_WRITEREGISTER:
			case SPU_READDMA:
			case SPU_WRITEDMA:
			case SPU_READDMAMEM:
			case SPU_WRITEDMAMEM:
			case SPU_REGISTERCALLBACK:
			case SPU_REGISTERCDDAVOLUME:
			case SPU_PLAYADPCMCHANNEL:
			case SPU_FREEZE:
			case SPU_GETONE:
			case SPU_PUTONE:
			case SPU_PLAYSAMPLE:
			case SPU_SETADDR:
			case SPU_SETPITCH:
			case SPU_SETVOLUMEL:
			case SPU_SETVOLUMER:
			case SPU_STARTCHANNELS1:
			case SPU_STARTCHANNELS2:
			case SPU_STOPCHANNELS1:
			case SPU_STOPCHANNELS2:
				if(!Rebuild_Sounds) WriteData = FALSE;
				break;
			default:
				WriteData = FALSE;
		}
	} else {
		WriteData = FALSE;
		Rebuild_Finished = TRUE;
	}
	
	if(WriteData) Rebuild_WriteData(hW, flags, iSize, data);
	Rebuild_SourcePosition = ftell(Rebuild_SourceStream);
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

void Rebuild_WriteCompressedBuffer(HWND hW)
{
	if(Rebuild_WriteBufferPos >= Rebuild_BlockSize) {
		DWORD CompressedSize = sizeof(Rebuild_CompressBuffer);
		int result;
		if(Rebuild_CompressionType == COMPRESSION_ZLIB)
			result = compress2((Bytef*)Rebuild_CompressBuffer, &CompressedSize, (Bytef*)Rebuild_WriteBuffer, Rebuild_WriteBufferPos, Rebuild_CompressionLevel);
		else
			result = BZ2_bzBuffToBuffCompress((char*)Rebuild_CompressBuffer, (unsigned int*)&CompressedSize, (char*)Rebuild_WriteBuffer, Rebuild_WriteBufferPos, 9, 0, 30);
		if(result != Z_OK) {
			MessageBox(hW, "Error compressing data", "Error", MB_ICONERROR);
			Rebuild_WriteBufferPos = 0;
			Rebuild_Finished = TRUE;
			return;
		}
		fwrite(&CompressedSize, 4, 1, Rebuild_Stream);
		fwrite(Rebuild_CompressBuffer, CompressedSize, 1, Rebuild_Stream);
		Rebuild_Header.CompressedDataSize += CompressedSize;
		Rebuild_WriteBufferPos = 0;
		Rebuild_Position = ftell(Rebuild_Stream);
	}
}

void Rebuild_WriteData(HWND hW, BYTE flags, DWORD iSize, LPVOID pMem)
{
	DWORD Total = 0;
	if(Rebuild_Header.Compression) {
	//
	// write compressed data
	//
		if((flags & 0x0f)== GPU_UPDATELACE) {
			Rebuild_WriteCompressedBuffer(hW);
			Rebuild_IndexData[Rebuild_Header.Frames++] = Rebuild_Position;
		}
		
		// calculate total size to write
		Total = 1;
		if(flags & RF_WRITENUMBER) 
			Total += 4;
		else if(flags & RF_WRITEBUFFER) 
			Total += 4 + iSize;

		if((Rebuild_WriteBufferPos + Total) > Rebuild_WriteBufferMaxSize)
			Rebuild_WriteCompressedBuffer(hW);
		
		if(Total > Rebuild_WriteBufferMaxSize) {
			MessageBox(hW, "Data size larger than buffer size", "Error", MB_ICONERROR);
			return;
		}

		Rebuild_WriteBuffer[Rebuild_WriteBufferPos++] = flags;
		if(flags & RF_WRITENUMBER) {
			memcpy(&Rebuild_WriteBuffer[Rebuild_WriteBufferPos], &iSize, 4);
			Rebuild_WriteBufferPos += 4;
		} else if(flags & RF_WRITEBUFFER) {
			memcpy(&Rebuild_WriteBuffer[Rebuild_WriteBufferPos], &iSize, 4); Rebuild_WriteBufferPos += 4;
			memcpy(&Rebuild_WriteBuffer[Rebuild_WriteBufferPos], pMem, iSize); Rebuild_WriteBufferPos += iSize;
		}

	} else {
	//
	// Write uncompressed data
	//		

		if((flags & 0x0f)== GPU_UPDATELACE) {
			Rebuild_IndexData[Rebuild_Header.Frames++] = Rebuild_Position;
		}

		// calculate total size to write
		Total = 1;
		if(flags & RF_WRITENUMBER) 
			Total += 4;
		else if(flags & RF_WRITEBUFFER) 
			Total += 4 + iSize;
				
		fwrite(&flags, 1, 1, Rebuild_Stream);
		if(flags & RF_WRITENUMBER) {
			fwrite(&iSize, 4, 1, Rebuild_Stream);
		} else if(flags & RF_WRITEBUFFER) {
			fwrite(&iSize, 4, 1, Rebuild_Stream);
			fwrite(pMem, iSize, 1, Rebuild_Stream);
		}
		
		Rebuild_Position = ftell(Rebuild_Stream);
	}

	switch(flags & 0x3f) {
		case SPU_READREGISTER:
		case SPU_WRITEREGISTER:
		case SPU_READDMA:
		case SPU_WRITEDMA:
		case SPU_READDMAMEM:
		case SPU_WRITEDMAMEM:
		case SPU_REGISTERCALLBACK:
		case SPU_REGISTERCDDAVOLUME:
		case SPU_PLAYADPCMCHANNEL:
		case SPU_FREEZE:
		case SPU_GETONE:
		case SPU_PUTONE:
		case SPU_PLAYSAMPLE:
		case SPU_SETADDR:
		case SPU_SETPITCH:
		case SPU_SETVOLUMEL:
		case SPU_SETVOLUMER:
		case SPU_STARTCHANNELS1:
		case SPU_STARTCHANNELS2:
		case SPU_STOPCHANNELS1:
		case SPU_STOPCHANNELS2:
			Rebuild_Header.SoundDataSize += Total;
			break;
		default:
			Rebuild_Header.VideoDataSize += Total;
		}
}
