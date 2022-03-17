/***************************************************************************
      File Name:	Player.cpp
   File Created:	Sunday, September 28rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "config.h"
#include "externals.h"
#include "player.h"
#include "log.h"

#define BZ_NO_STDIO
#include "zlib.h"
#include "bzlib.h"

DWORD Player_State = STATE_STOPPED;
DWORD Player_SeekingPossible = 0;
DWORD Player_Frame = 0;
DWORD Player_FileVersion = 0;
DWORD Player_FileSize = 0;
HEADER Player_Header;
FILE *Player_Stream = NULL;
DWORD Player_Position = 0;
CHAR  Player_Error[MAX_PATH];

DWORD Player_DataOffset = 0;

DWORD index_data[0x80000];
DWORD compressed[0x80000];
DWORD buffer[0x80000];
DWORD temp[0x80000];
DWORD buffer_pos = 0;
DWORD buffer_size = 0;
GPUFreeze_t freeze;


//==========================================================================
// Dummy Callback Functions (for Sound Playback)
//==========================================================================

void CALLBACK DUMMY_callback(void) { };
void CALLBACK DUMMY_CDDAVcallback(unsigned short arg1, unsigned short arg2) { }

//==========================================================================
// Open recorded file for playback
//==========================================================================

BOOL PLAYER_GetVersion(LPDWORD dwVersion)
{
	if(Player_Stream == NULL) return 0;
	DWORD dwTAG;
	fseek(Player_Stream, 0, SEEK_SET);
	fread(&dwTAG, 4, 1, Player_Stream);
	fread(dwVersion, 4, 1, Player_Stream);
	if(dwTAG != ID_TAG) return FALSE;
	return TRUE;
}

BOOL PLAYER_Open(char *FileName, HWND hwnd)
{
	// Init playback
	PLAYER_Close(hwnd);

	// Open REC file
	Player_Stream = fopen(FileName, "rb");
	if(Player_Stream == NULL) return FALSE;
	fseek(Player_Stream, 0, SEEK_END);
	Player_FileSize = ftell(Player_Stream);
	fseek(Player_Stream, 0, SEEK_SET);

	// Chech version
	if(!PLAYER_GetVersion(&Player_FileVersion)) {
		fclose(Player_Stream);
		Player_Stream = NULL;
		MessageBox(hwnd, "Unsupported file type !", "Error", MB_ICONERROR);
		return FALSE;
	}
	
	// Read header
	if(Player_FileVersion == 0x0100) {
		int file_size;
		fread(&Player_Header.Frames, 4, 1, Player_Stream);
		fread(&Player_Header.FPS, 4, 1, Player_Stream);
		fread(&file_size, 4, 1, Player_Stream);
		Player_DataOffset = ftell(Player_Stream);

	} else if(Player_FileVersion == 0x0200) {

		fread(&Player_Header, sizeof(HEADER), 1, Player_Stream);
		Player_DataOffset = ftell(Player_Stream);
		if(Player_Header.Frames > 0 && Player_Header.IndexOffset > 0 && Player_Header.IndexOffset < Player_FileSize) {		
			fseek(Player_Stream, Player_Header.IndexOffset, SEEK_SET);
			fread(index_data, Player_Header.Frames * 4, 1, Player_Stream);
			fseek(Player_Stream, Player_DataOffset, SEEK_SET);
			Player_SeekingPossible = 1;
		}

	} else {
		fclose(Player_Stream);
		Player_Stream = NULL;
		MessageBoxFormat(hwnd, "Unsupported file version %d.%d !\n\nPlayer supports versions 1.0 and 2.0", "Error", MB_ICONERROR, HIBYTE(Player_FileVersion), LOBYTE(Player_FileVersion));
		return FALSE;
	}
	
	//Check compression type
	if(!(Player_Header.Compression == COMPRESSION_NONE ||
		 Player_Header.Compression == COMPRESSION_ZLIB ||
		 Player_Header.Compression == COMPRESSION_BZIP)) {
		fclose(Player_Stream);
		Player_Stream = NULL;
		MessageBox(hwnd, "Unsupported compression type !", "Error", MB_ICONERROR);
		return FALSE;
	}
	
	// Initialize	
	Player_State = STATE_PLAYING;
	Player_Frame = 0;
	Player_Position = ftell(Player_Stream);
	buffer_pos = 0;
	buffer_size = 0;
	if(!LoadPlugins()) {
		PLAYER_Close(hwnd);
		MessageBox(hwnd, "Couldn't Load Plugin !", "Error", MB_ICONERROR);
		return FALSE;
	}
	if(GPUinit) GPUinit();
	if(GPUopen) GPUopen(hwnd);
	if(SPUinit) SPUinit();
	if(SPUopen) SPUopen(hwnd);

	GPULogStart();

	if(Player_Header.Frames == 0) {
		MessageBox(hwnd, "Corrupted header !\nProbably recording incorrectly finished.\nThere may be problems during playback.", "Warning", MB_ICONINFORMATION);
	}
	
	return TRUE;
}

//==========================================================================
// Playback
//==========================================================================

void PLAYER_Play(HWND hwnd)
{
	if(Player_State != STATE_PLAYING || Player_Stream == NULL) return;

	Player_Position = ftell(Player_Stream);
	if(Player_Header.Frames > 0) { 
		if(Player_Frame >= Player_Header.Frames) PLAYER_Stop(hwnd);
	} else {
		if(Player_Position >= Player_FileSize) PLAYER_Stop(hwnd);
	}
	if(Player_State == STATE_STOPPED && cfg.Replay) {
		Player_State = STATE_PLAYING;
		SendMessage(hwnd, WM_UPDATESTATE, 0, 0);
	}

	if(Player_FileVersion == 0x0100) {	
		//
		// Play Version 1.0
		//
		unsigned char id;
		unsigned long gdata;
		unsigned long size;
		long i;
	
		fread(&id, 1, 1, Player_Stream);
		switch((int)id) {
			case GPU_WRITEDATA:
				fread(&gdata, 4, 1, Player_Stream);
				if(GPUwriteData) GPUwriteData(gdata);
				GPULogAppend("GPUwriteData: 0x%08lX", gdata);
				break;
	
			case GPU_WRITEDATAMEM:
				fread(&size, 4, 1, Player_Stream);
				fread(buffer, size, 1, Player_Stream);
				if(GPUwriteDataMem)
					GPUwriteDataMem(buffer, size>>2);
				else {
					size = size>>2;
					if(GPUwriteData) for(i=0; i<size; i++) GPUwriteData(buffer[i]);
				}
				GPULogAppend("GPUwriteDataMem: 0x%08lX", (size>>2));
				break;
	
			case GPU_READDATA:
				if(GPUreadData) GPUreadData();
				GPULogAppend("GPUreadData");
				break;
	
			case GPU_READDATAMEM:
				fread(&size, 4, 1, Player_Stream);
				if(GPUreadDataMem)
					GPUreadDataMem(temp, size);
				else
					if(GPUreadData) for(i=0; i<size; i++) GPUreadData();
				GPULogAppend("GPUreadDataMem: 0x%08lX", size);
				break;
	
			case GPU_DMACHAIN:
				fread(&size, 4, 1, Player_Stream);
				fread(buffer, size, 1, Player_Stream);
				if(GPUdmaChain) GPUdmaChain(buffer, 0);
				GPULogAppend("GPUdmaChain: 0x%08lX", size);
				break;

			case GPU_SETMODE:
				fread(&gdata, 4, 1, Player_Stream);
				if(GPUsetMode) GPUsetMode(gdata);
				GPULogAppend("GPUsetMode: 0x%08lX", gdata);
				break;

			case GPU_WRITESTATUS:
				fread(&gdata, 4, 1, Player_Stream);
				if(GPUwriteStatus) GPUwriteStatus(gdata);
				GPULogAppend("GPUwriteStatus: 0x%08lX", gdata);
				break;

			case GPU_UPDATELACE:
				if(GPUupdateLace) GPUupdateLace();
				GPULogAppend("GPUupdateLace (frame %d)", Player_Frame);
				Player_Frame++;
				break;

			case GPU_FREEZE:
				fread(&freeze, sizeof(GPUFreeze_t), 1, Player_Stream);
				if(GPUfreeze) GPUfreeze(0, &freeze);
				GPULogAppend("GPUfreeze");	
				break;
		}

	} else if(Player_FileVersion == 0x0200) {
		//
		// Play Version 2.0
		//
		BYTE  flags = 0;
		DWORD iSize = 0;
		LPDWORD data = NULL;
		int i;
		if(Player_Header.Compression) {
			// read compressed data			
			if(buffer_pos >= buffer_size) {
				fread(&iSize, 4, 1, Player_Stream);
				fread(compressed, iSize, 1, Player_Stream);
				buffer_size = sizeof(buffer);
				buffer_pos = 0;
				int result = 0;
				if(Player_Header.Compression == COMPRESSION_ZLIB)
					result = uncompress((Bytef *)buffer, &buffer_size, (Bytef *)compressed, iSize);
				else if(Player_Header.Compression == COMPRESSION_BZIP)
					result = BZ2_bzBuffToBuffDecompress((char*)buffer, (unsigned int*)&buffer_size, (char*)compressed, iSize, 0, 0);
				else
					result = -1;
				if(result != Z_OK) {
					Player_State = STATE_ERROR;
					strcpy(Player_Error, "Error while decompressing...");
					buffer_size = 0;
					return;
				}
			}
			LPBYTE start = &((LPBYTE)buffer)[buffer_pos];
			flags = *start; 
			buffer_pos++; start++;
			if(flags & RF_WRITENUMBER) {
				iSize = *((LPDWORD)start);
				buffer_pos += 4; start += 4;
			} else if(flags & RF_WRITEBUFFER) {
				iSize = *((LPDWORD)start);
				buffer_pos += 4; start += 4;
				data = (LPDWORD)start;
				buffer_pos += iSize;
			}			

		} else {
			// read uncompressed data
			fread(&flags, 1, 1, Player_Stream);
			if(flags & RF_WRITENUMBER) {
				fread(&iSize, 4, 1, Player_Stream);
			} else if(flags & RF_WRITEBUFFER) {
				fread(&iSize, 4, 1, Player_Stream);
				fread(buffer, iSize, 1, Player_Stream);
			}
			data = buffer;
		}

		switch(flags & 0x3f) {
			case GPU_WRITEDATA:
				if(GPUwriteData) GPUwriteData(iSize);
				GPULogAppend("GPUwriteData: 0x%08lX", iSize);
				break;	
			case GPU_WRITEDATAMEM:
				if(GPUwriteDataMem)
					GPUwriteDataMem(data, iSize>>2);
				else {
					iSize = iSize>>2;
					if(GPUwriteData) for(i=0; i<iSize; i++) GPUwriteData(data[i]);
				}
				GPULogAppend("GPUwriteDataMem: 0x%08lX", (iSize>>2));
				break;	
			case GPU_READDATA:
				if(GPUreadData) GPUreadData();
				GPULogAppend("GPUreadData");
				break;
			case GPU_READDATAMEM:
				if(GPUreadDataMem)
					GPUreadDataMem(temp, iSize);
				else
					if(GPUreadData) for(i=0; i<iSize; i++) GPUreadData();
				GPULogAppend("GPUreadDataMem: 0x%08lX", iSize);
				break;
			case GPU_DMACHAIN:
				if(GPUdmaChain) GPUdmaChain(data, 0);
				GPULogAppend("GPUdmaChain: 0x%08lX", iSize);
				break;
			case GPU_SETMODE:
				if(GPUsetMode) GPUsetMode(iSize);
				GPULogAppend("GPUsetMode: 0x%08lX", iSize);
				break;
			case GPU_WRITESTATUS:
				if(GPUwriteStatus) GPUwriteStatus(iSize);
				GPULogAppend("GPUwriteStatus: 0x%08lX", iSize);
				break;
			case GPU_UPDATELACE:
				if(GPUupdateLace) GPUupdateLace();
				GPULogAppend("GPUupdateLace (frame %d)", Player_Frame);
				Player_Frame++;
				break;
			case GPU_FREEZE:
				if(GPUfreeze) GPUfreeze(0, data);
				GPULogAppend("GPUfreeze");	
				break;
		}
		
		if(cfg.SoundEnabled) {
			switch(flags & 0x3f) {
				case SPU_READREGISTER:
					if(SPUreadRegister) SPUreadRegister(iSize);
					SPULogAppend("SPUreadRegister: 0x%08lX", iSize);
					break;
				case SPU_WRITEREGISTER:
					if(SPUwriteRegister) SPUwriteRegister(data[0], (USHORT)data[1]);
					SPULogAppend("SPUwriteRegister: 0x%08lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_READDMA:
					if(SPUreadDMA) SPUreadDMA();
					SPULogAppend("SPUreadDMA");
					break;
				case SPU_WRITEDMA:
					if(SPUwriteDMA) SPUwriteDMA((USHORT)iSize);
					SPULogAppend("SPUwriteDMA: 0x%04lX", iSize);
					break;
				case SPU_READDMAMEM:
					if(SPUreadDMAMem) SPUreadDMAMem((unsigned short*)temp, (iSize>>1));
					SPULogAppend("SPUreadDMAMem: 0x%08lX", (iSize<<1));
					break;
				case SPU_WRITEDMAMEM:
					if(SPUwriteDMAMem) SPUwriteDMAMem((unsigned short*)data, (iSize>>1));
					SPULogAppend("SPUwriteDMAMem: 0x%08lX", (iSize<<1));
					break;
				case SPU_REGISTERCALLBACK:
					if(SPUregisterCallback) SPUregisterCallback(DUMMY_callback);
					SPULogAppend("SPUregisterCallback");
					break;
				case SPU_REGISTERCDDAVOLUME:
					if(SPUregisterCDDAVolume) SPUregisterCDDAVolume(DUMMY_CDDAVcallback);
					SPULogAppend("SPUregisterCDDAVolume");
					break;
				case SPU_PLAYADPCMCHANNEL:
					if(SPUplayADPCMchannel) SPUplayADPCMchannel(data);
					SPULogAppend("SPUplayADPCMchannel");
					break;
				case SPU_FREEZE:
					if(SPUfreeze) SPUfreeze(0, data);
					SPULogAppend("SPUfreeze");
					break;
				case SPU_GETONE:
					if(SPUgetOne) SPUgetOne(iSize);
					SPULogAppend("SPUgetOne: 0x%08lX", iSize);
					break;
				case SPU_PUTONE:
					if(SPUputOne) SPUputOne(data[0], (USHORT)data[1]);
					SPULogAppend("SPUputOne: 0x%08lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_PLAYSAMPLE:
					if(SPUplaySample) SPUplaySample((UCHAR)iSize);
					SPULogAppend("SPUplaySample: 0x%02lX", iSize);
					break;
				case SPU_SETADDR:
					if(SPUsetAddr) SPUsetAddr((UCHAR)data[0], (USHORT)data[1]);
					SPULogAppend("SPUsetAddr: 0x%02lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_SETPITCH:
					if(SPUsetPitch) SPUsetPitch((UCHAR)data[0], (USHORT)data[1]);
					SPULogAppend("SPUsetPitch: 0x%02lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_SETVOLUMEL:
					if(SPUsetVolumeL) SPUsetVolumeL((UCHAR)data[0], (SHORT)data[1]);
					SPULogAppend("SPUsetVolumeL: 0x%02lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_SETVOLUMER:
					if(SPUsetVolumeR) SPUsetVolumeR((UCHAR)data[0], (SHORT)data[1]);
					SPULogAppend("SPUsetVolumeR: 0x%02lX, 0x%04lX", data[0], data[1]);
					break;
				case SPU_STARTCHANNELS1:
					if(SPUstartChannels1) SPUstartChannels1((USHORT)iSize);
					SPULogAppend("SPUstartChanells1: 0x%04lX", iSize);
					break;
				case SPU_STARTCHANNELS2:
					if(SPUstartChannels2) SPUstartChannels2((USHORT)iSize);
					SPULogAppend("SPUstartChannels2: 0x%04lX", iSize);
					break;
				case SPU_STOPCHANNELS1:
					if(SPUstopChannels1) SPUstopChannels1((USHORT)iSize);
					SPULogAppend("SPUstopChannels1: 0x%04lX", iSize);
					break;
				case SPU_STOPCHANNELS2:
					if(SPUstopChannels2) SPUstopChannels2((USHORT)iSize);
					SPULogAppend("SPUstopChannels2: 0x%04lX", iSize);
					break;
			}
		}
		


	}

	//
	// For protection
	//
	Player_Position = ftell(Player_Stream);
	if(Player_Position >= Player_FileSize) PLAYER_Stop(hwnd);
}

//==========================================================================
// Seeking
//==========================================================================

void PLAYER_SeekFrame(DWORD newFrame)
{
	if(!Player_SeekingPossible) return;
	if(newFrame >= Player_Header.Frames) return;
	Player_Frame = newFrame;
	fseek(Player_Stream, index_data[Player_Frame], SEEK_SET);
}

//==========================================================================
// Stop playback / Rewind
//==========================================================================

void PLAYER_Stop(HWND hwnd)
{
	if(Player_Stream == NULL) return;
	Player_State = STATE_STOPPED;
	Player_Frame = 0;
	fseek(Player_Stream, Player_DataOffset, SEEK_SET);
	Player_Position = ftell(Player_Stream);

	if(GPUclose) GPUclose();
	if(GPUshutdown) GPUshutdown();
	if(SPUclose) SPUclose();
	if(SPUshutdown) SPUshutdown();

	if(GPUinit) GPUinit();
	if(GPUopen) GPUopen(hwnd);
	if(SPUinit) SPUinit();
	if(SPUopen) SPUopen(hwnd);

	SendMessage(hwnd, WM_UPDATESTATE, 0, 0);
}

//==========================================================================
// Close opened file & free plugin
//==========================================================================

void PLAYER_Close(HWND hwnd)
{
	#ifdef LOG_ENABLED
		if(Player_Stream) LogEnd();
	#endif

	if(Player_Stream) {
		fclose(Player_Stream);
		Player_Stream = NULL;
	}
	
	Player_State = STATE_STOPPED;
	Player_SeekingPossible = 0;
	Player_Frame = 0;
	Player_FileVersion = 0;
	Player_FileSize = 0;
	memset(&Player_Header, 0, sizeof(Player_Header));
	Player_Position = 0;
	Player_DataOffset = 0;

	if(GPUclose) GPUclose();
	if(GPUshutdown) GPUshutdown();
	if(SPUclose) SPUclose();
	if(SPUshutdown) SPUshutdown();
	if(hwnd) SendMessage(hwnd, WM_UPDATESTATE, 0, 0);
}