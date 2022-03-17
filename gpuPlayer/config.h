/***************************************************************************
      File Name:	Config.h
   File Created:	Friday, October 10th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "stdafx.h"

#define REG_GPU_PLUGIN      "Video Plugin" 
#define REG_SPU_PLUGIN      "Sound Plugin"
#define REG_SOUND_ENABLED   "Sound Enabled"
#define REG_SHOW_FRAMES     "Show Frames"
#define REG_REPLAY          "Replay"

typedef struct tagCfg {
	CHAR  VideoPlugin[MAX_PATH];
	CHAR  SoundPlugin[MAX_PATH];
	DWORD SoundEnabled;
	DWORD ShowFrames;
	DWORD Replay;
	} CFG, *LPCFG;
	
extern CFG cfg;

void LoadConfiguration();
void SaveConfiguration();


#endif