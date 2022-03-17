/***************************************************************************
      File Name:	Player.h
   File Created:	Sunday, September 28rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "stdafx.h"

//
// File structure: 4 bytes - IDTAG ('R','E','C',0) <=> 0x00434552;
//                 4 bytes - Version
//				   xxx     - Header
//                 xxx     - Data
//                 xxx     - Index Data

#define ID_TAG  0x00434552

// video recording data type
#define GPU_WRITEDATA		0x01
#define GPU_WRITEDATAMEM	0x02
#define GPU_READDATA		0x03
#define GPU_READDATAMEM		0x04
#define GPU_DMACHAIN		0x05
#define GPU_SETMODE			0x06
#define GPU_WRITESTATUS		0x07
#define GPU_UPDATELACE		0x08
#define GPU_FREEZE			0x09

// sound recording data type
#define SPU_READREGISTER		0x10
#define SPU_WRITEREGISTER		0x11
#define SPU_READDMA				0x12
#define SPU_WRITEDMA			0x13
#define SPU_READDMAMEM			0x14
#define SPU_WRITEDMAMEM			0x15
#define SPU_REGISTERCALLBACK	0x16
#define SPU_REGISTERCDDAVOLUME	0x17
#define SPU_PLAYADPCMCHANNEL	0x18
#define SPU_FREEZE				0x19
#define SPU_GETONE				0x1A
#define SPU_PUTONE				0x1B
#define SPU_PLAYSAMPLE			0x1C
#define SPU_SETADDR				0x1D
#define SPU_SETPITCH			0x1E
#define SPU_SETVOLUMEL			0x1F
#define SPU_SETVOLUMER			0x20
#define SPU_STARTCHANNELS1		0x21
#define SPU_STARTCHANNELS2		0x22
#define SPU_STOPCHANNELS1		0x23
#define SPU_STOPCHANNELS2		0x24
#define SPU_PLAYSECTOR			0x25

// additional data flag
#define RF_WRITENUMBER		0x40
#define RF_WRITEBUFFER		0x80

// Compression Type
#define COMPRESSION_NONE  0x00
#define COMPRESSION_ZLIB  0x01
#define COMPRESSION_BZIP  0x02

// Player State
#define STATE_ERROR    0xFF
#define STATE_STOPPED  0x00
#define STATE_PLAYING  0x01
#define STATE_PAUSED   0x02
#define STATE_SEEKING  0x03

typedef struct tagGPUFreeze_t {
	unsigned long ulFreezeVersion;      // should be always 1 for now (set by main emu)
	unsigned long ulStatus;             // current gpu status
	unsigned long ulControl[256];       // latest control register values
	unsigned char psxVRam[1024*512*2];  // current VRam image
	} GPUFreeze_t;

typedef struct tagHeader {
	DWORD Compression;
	DWORD Frames;
	DWORD FPS;
	DWORD VideoDataSize;
	DWORD SoundDataSize;
	DWORD CompressedDataSize;
	DWORD IndexOffset;
} HEADER, *LPHEADER;

extern DWORD Player_State;
extern DWORD Player_SeekingPossible;
extern DWORD Player_Frame;
extern DWORD Player_FileVersion;
extern DWORD Player_FileSize;
extern HEADER Player_Header;
extern FILE *Player_Stream;
extern DWORD Player_Position;
extern CHAR  Player_Error[MAX_PATH];

BOOL PLAYER_Open(char *FileName, HWND hwnd);
void PLAYER_Play(HWND hwnd);
void PLAYER_SeekFrame(DWORD newFrame);
void PLAYER_Stop(HWND hwnd);
void PLAYER_Close(HWND hwnd);

#endif