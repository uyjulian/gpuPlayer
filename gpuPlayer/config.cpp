/***************************************************************************
      File Name:	Config.cpp
   File Created:	Friday, October 10th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "config.h"
#include "reg.h"

CFG cfg;

//==========================================================================
// Load Configuration
//==========================================================================

void LoadConfiguration()
{
	memset(&cfg, 0, sizeof(cfg));
	REG_GetSTRING(REG_GPU_PLUGIN,   cfg.VideoPlugin);
	REG_GetSTRING(REG_SPU_PLUGIN,   cfg.SoundPlugin);
	REG_GetDWORD(REG_SOUND_ENABLED, cfg.SoundEnabled);
	REG_GetDWORD(REG_SHOW_FRAMES,   cfg.ShowFrames);
	REG_GetDWORD(REG_REPLAY,        cfg.Replay);
}

//==========================================================================
// Save Configuration
//==========================================================================

void SaveConfiguration()
{
	REG_SetSTRING(REG_GPU_PLUGIN,   cfg.VideoPlugin);
	REG_SetSTRING(REG_SPU_PLUGIN,   cfg.SoundPlugin);
	REG_SetDWORD(REG_SOUND_ENABLED, cfg.SoundEnabled);
	REG_SetDWORD(REG_SHOW_FRAMES,   cfg.ShowFrames);
	REG_SetDWORD(REG_REPLAY,        cfg.Replay);
}
