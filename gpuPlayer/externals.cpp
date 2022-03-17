/***************************************************************************
      File Name:	Externals.cpp
   File Created:	Sunday, September 28rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "config.h"
#include "externals.h"

char*			(CALLBACK *PSEgetLibName)(void)							= NULL;
unsigned long	(CALLBACK *PSEgetLibType)(void)							= NULL;
unsigned long	(CALLBACK *PSEgetLibVersion)(void)						= NULL;

long			(CALLBACK *GPUinit)(void)								= NULL;
long			(CALLBACK *GPUshutdown)(void)							= NULL;
void			(CALLBACK *GPUmakeSnapshot)(void)						= NULL;
long			(CALLBACK *GPUopen)(HWND)								= NULL;
long			(CALLBACK *GPUclose)(void)								= NULL;
void			(CALLBACK *GPUupdateLace)(void)							= NULL;
unsigned long	(CALLBACK *GPUreadStatus)(void)							= NULL;
void			(CALLBACK *GPUwriteStatus)(unsigned long)				= NULL;
unsigned long	(CALLBACK *GPUreadData)(void)							= NULL;
void			(CALLBACK *GPUreadDataMem)(unsigned long*,int)			= NULL;
void			(CALLBACK *GPUwriteData)(unsigned long)					= NULL;
void			(CALLBACK *GPUwriteDataMem)(unsigned long*,int)			= NULL;
void			(CALLBACK *GPUsetMode)(unsigned long)					= NULL;
long			(CALLBACK *GPUgetMode)(void)							= NULL;
long			(CALLBACK *GPUdmaChain)(unsigned long*,unsigned long) = NULL;
long			(CALLBACK *GPUconfigure)(void)							= NULL;
void			(CALLBACK *GPUabout)(void)								= NULL;
long			(CALLBACK *GPUtest)(void)								= NULL;
long			(CALLBACK *GPUfreeze)(unsigned long,void*)				= NULL;
void			(CALLBACK *GPUdisplayText)(char * pText)				= NULL;
void			(CALLBACK *GPUdisplayFlags)(unsigned long dwFlags)		= NULL;
void			(CALLBACK *GPUgetScreenPic)(unsigned char*)				= NULL;
void			(CALLBACK *GPUshowScreenPic)(unsigned char*)			= NULL;

long			(CALLBACK *SPUtest)(void)								= NULL;
long			(CALLBACK *SPUconfigure)(void)							= NULL;
void			(CALLBACK *SPUabout)(void)								= NULL;
long			(CALLBACK *SPUinit)(void)								= NULL;
long			(CALLBACK *SPUshutdown)(void)							= NULL;
long			(CALLBACK *SPUopen)(HWND)								= NULL;
long			(CALLBACK *SPUclose)(void)								= NULL;
unsigned short	(CALLBACK *SPUreadRegister)(unsigned long) 				= NULL;
void			(CALLBACK *SPUwriteRegister)(unsigned long, unsigned short)			= NULL;
unsigned short	(CALLBACK *SPUreadDMA)(void)										= NULL;
void			(CALLBACK *SPUwriteDMA)(unsigned short)								= NULL;
void			(CALLBACK *SPUreadDMAMem)(unsigned short*, int)						= NULL;
void			(CALLBACK *SPUwriteDMAMem)(unsigned short*, int)					= NULL;
void			(CALLBACK *SPUregisterCallback)(void (CALLBACK *callback)(void))	= NULL;
void			(CALLBACK *SPUregisterCDDAVolume)(void (CALLBACK *CDDAVcallback)(unsigned short, unsigned short)) = NULL;
long			(CALLBACK *SPUfreeze)(unsigned long, void*)					= NULL;
void			(CALLBACK *SPUplayADPCMchannel)(void*)						= NULL;
unsigned short	(CALLBACK *SPUgetOne)(unsigned long)						= NULL;
void			(CALLBACK *SPUputOne)(unsigned long, unsigned short)		= NULL;
void			(CALLBACK *SPUplaySample)(unsigned char)					= NULL;
void			(CALLBACK *SPUsetAddr)(unsigned char, unsigned short)		= NULL;
void			(CALLBACK *SPUsetPitch)(unsigned char, unsigned short)		= NULL;
void			(CALLBACK *SPUsetVolumeL)(unsigned char, short)				= NULL;
void			(CALLBACK *SPUsetVolumeR)(unsigned char, short)				= NULL;
void			(CALLBACK *SPUstartChannels1)(unsigned short)				= NULL;
void			(CALLBACK *SPUstartChannels2)(unsigned short)				= NULL;
void			(CALLBACK *SPUstopChannels1)(unsigned short)				= NULL;
void			(CALLBACK *SPUstopChannels2)(unsigned short)				= NULL;
void			(CALLBACK *SPUplaySector)(unsigned long, unsigned char*)	= NULL;

BOOL PluginsLoaded = FALSE;

HMODULE Externals_hGPUModule = NULL;
HMODULE Externals_hSPUModule = NULL;

//==========================================================================
// Load Plugins
//==========================================================================

BOOL LoadPlugins()
{
	FreePlugins();

	if(strlen(cfg.VideoPlugin) == 0) return FALSE;
	Externals_hGPUModule = LoadLibrary(cfg.VideoPlugin);
	if(Externals_hGPUModule == NULL) return FALSE;
	
	(FARPROC&)GPUinit			= GetProcAddress(Externals_hGPUModule, "GPUinit");
	(FARPROC&)GPUshutdown		= GetProcAddress(Externals_hGPUModule, "GPUshutdown");
	(FARPROC&)GPUmakeSnapshot	= GetProcAddress(Externals_hGPUModule, "GPUmakeSnapshot");
	(FARPROC&)GPUopen			= GetProcAddress(Externals_hGPUModule, "GPUopen");
	(FARPROC&)GPUclose			= GetProcAddress(Externals_hGPUModule, "GPUclose");
	(FARPROC&)GPUupdateLace		= GetProcAddress(Externals_hGPUModule, "GPUupdateLace");
	(FARPROC&)GPUreadStatus		= GetProcAddress(Externals_hGPUModule, "GPUreadStatus");
	(FARPROC&)GPUwriteStatus	= GetProcAddress(Externals_hGPUModule, "GPUwriteStatus");
	(FARPROC&)GPUreadData		= GetProcAddress(Externals_hGPUModule, "GPUreadData");
	(FARPROC&)GPUreadDataMem	= GetProcAddress(Externals_hGPUModule, "GPUreadDataMem");
	(FARPROC&)GPUwriteData		= GetProcAddress(Externals_hGPUModule, "GPUwriteData");
	(FARPROC&)GPUwriteDataMem	= GetProcAddress(Externals_hGPUModule, "GPUwriteDataMem");
	(FARPROC&)GPUsetMode		= GetProcAddress(Externals_hGPUModule, "GPUsetMode");
	(FARPROC&)GPUgetMode		= GetProcAddress(Externals_hGPUModule, "GPUgetMode");
	(FARPROC&)GPUdmaChain		= GetProcAddress(Externals_hGPUModule, "GPUdmaChain");
	(FARPROC&)GPUconfigure		= GetProcAddress(Externals_hGPUModule, "GPUconfigure");
	(FARPROC&)GPUabout			= GetProcAddress(Externals_hGPUModule, "GPUabout");
	(FARPROC&)GPUtest			= GetProcAddress(Externals_hGPUModule, "GPUtest");
	(FARPROC&)GPUfreeze			= GetProcAddress(Externals_hGPUModule, "GPUfreeze");
	(FARPROC&)GPUdisplayText	= GetProcAddress(Externals_hGPUModule, "GPUdisplayText");
	(FARPROC&)GPUdisplayFlags	= GetProcAddress(Externals_hGPUModule, "GPUdisplayFlags");
	(FARPROC&)GPUgetScreenPic	= GetProcAddress(Externals_hGPUModule, "GPUgetScreenPic");
	(FARPROC&)GPUshowScreenPic	= GetProcAddress(Externals_hGPUModule, "GPUshowScreenPic");

	if(strlen(cfg.SoundPlugin) == 0) { FreePlugins(); return FALSE; }
	Externals_hSPUModule = LoadLibrary(cfg.SoundPlugin);
	if(Externals_hSPUModule == NULL) { FreePlugins(); return FALSE; }

	(FARPROC&)SPUtest				= GetProcAddress(Externals_hSPUModule, "SPUtest");
	(FARPROC&)SPUconfigure			= GetProcAddress(Externals_hSPUModule, "SPUconfigure");
	(FARPROC&)SPUabout				= GetProcAddress(Externals_hSPUModule, "SPUabout");
	(FARPROC&)SPUinit				= GetProcAddress(Externals_hSPUModule, "SPUinit");
	(FARPROC&)SPUshutdown			= GetProcAddress(Externals_hSPUModule, "SPUshutdown");
	(FARPROC&)SPUopen				= GetProcAddress(Externals_hSPUModule, "SPUopen");
	(FARPROC&)SPUclose				= GetProcAddress(Externals_hSPUModule, "SPUclose");
	(FARPROC&)SPUreadRegister		= GetProcAddress(Externals_hSPUModule, "SPUreadRegister");
	(FARPROC&)SPUwriteRegister		= GetProcAddress(Externals_hSPUModule, "SPUwriteRegister");
	(FARPROC&)SPUreadDMA			= GetProcAddress(Externals_hSPUModule, "SPUreadDMA");
	(FARPROC&)SPUwriteDMA			= GetProcAddress(Externals_hSPUModule, "SPUwriteDMA");
	(FARPROC&)SPUreadDMAMem			= GetProcAddress(Externals_hSPUModule, "SPUreadDMAMem");
	(FARPROC&)SPUwriteDMAMem		= GetProcAddress(Externals_hSPUModule, "SPUwriteDMAMem");
	(FARPROC&)SPUregisterCallback	= GetProcAddress(Externals_hSPUModule, "SPUregisterCallback");
	(FARPROC&)SPUregisterCDDAVolume	= GetProcAddress(Externals_hSPUModule, "SPUregisterCDDAVolume");
	(FARPROC&)SPUfreeze				= GetProcAddress(Externals_hSPUModule, "SPUfreeze");
	(FARPROC&)SPUplayADPCMchannel	= GetProcAddress(Externals_hSPUModule, "SPUplayADPCMchannel");
	(FARPROC&)SPUgetOne				= GetProcAddress(Externals_hSPUModule, "SPUgetOne");
	(FARPROC&)SPUputOne				= GetProcAddress(Externals_hSPUModule, "SPUputOne");
	(FARPROC&)SPUplaySample			= GetProcAddress(Externals_hSPUModule, "SPUplaySample");
	(FARPROC&)SPUsetAddr			= GetProcAddress(Externals_hSPUModule, "SPUsetAddr");
	(FARPROC&)SPUsetPitch			= GetProcAddress(Externals_hSPUModule, "SPUsetPitch");
	(FARPROC&)SPUsetVolumeL			= GetProcAddress(Externals_hSPUModule, "SPUsetVolumeL");
	(FARPROC&)SPUsetVolumeR			= GetProcAddress(Externals_hSPUModule, "SPUsetVolumeR");
	(FARPROC&)SPUstartChannels1		= GetProcAddress(Externals_hSPUModule, "SPUstartChannels1");
	(FARPROC&)SPUstartChannels2		= GetProcAddress(Externals_hSPUModule, "SPUstartChannels2");
	(FARPROC&)SPUstopChannels1		= GetProcAddress(Externals_hSPUModule, "SPUstopChannels1");
	(FARPROC&)SPUstopChannels2		= GetProcAddress(Externals_hSPUModule, "SPUstopChannels2");
	(FARPROC&)SPUplaySector			= GetProcAddress(Externals_hSPUModule, "SPUplaySector"); 
	
	PluginsLoaded = TRUE;	
	return TRUE;
}

//==========================================================================
// Free Plugins
//==========================================================================

void FreePlugins()
{
	PSEgetLibName		= NULL;
	PSEgetLibType		= NULL;
	PSEgetLibVersion	= NULL;

	if(Externals_hGPUModule != NULL) {
		FreeLibrary(Externals_hGPUModule);
		Externals_hGPUModule = NULL;
	}
	
	GPUinit				= NULL;
	GPUshutdown			= NULL;
	GPUmakeSnapshot		= NULL;
	GPUopen				= NULL;
	GPUclose			= NULL;
	GPUupdateLace		= NULL;
	GPUreadStatus		= NULL;
	GPUwriteStatus		= NULL;
	GPUreadData			= NULL;
	GPUreadDataMem		= NULL;
	GPUwriteData		= NULL;
	GPUwriteDataMem		= NULL;

	GPUsetMode			= NULL;
	GPUgetMode			= NULL;
	GPUdmaChain			= NULL;

	GPUconfigure		= NULL;
	GPUabout			= NULL;
	GPUtest				= NULL;

	GPUfreeze			= NULL;
	GPUdisplayText		= NULL;
	GPUdisplayFlags		= NULL;
	GPUgetScreenPic		= NULL;
	GPUshowScreenPic	= NULL;

	if(Externals_hSPUModule != NULL) {
		FreeLibrary(Externals_hSPUModule);
		Externals_hSPUModule = NULL;
	}

	SPUtest					= NULL;
	SPUconfigure			= NULL;
	SPUabout				= NULL;
	SPUinit					= NULL;
	SPUshutdown				= NULL;
	SPUopen					= NULL;
	SPUclose				= NULL;
	SPUreadRegister			= NULL;
	SPUwriteRegister		= NULL;
	SPUreadDMA				= NULL;
	SPUwriteDMA				= NULL;
	SPUreadDMAMem			= NULL;
	SPUwriteDMAMem			= NULL;
	SPUregisterCallback		= NULL;
	SPUregisterCDDAVolume	= NULL;
	SPUfreeze				= NULL;
	SPUplayADPCMchannel		= NULL;
	SPUgetOne				= NULL;
	SPUputOne				= NULL;
	SPUplaySample			= NULL;
	SPUsetAddr				= NULL;
	SPUsetPitch				= NULL;
	SPUsetVolumeL			= NULL;
	SPUsetVolumeR			= NULL;
	SPUstartChannels1		= NULL;
	SPUstartChannels2		= NULL;
	SPUstopChannels1		= NULL;
	SPUstopChannels2		= NULL;
	SPUplaySector			= NULL;

	PluginsLoaded = FALSE;
}

//-----------------------------------------------------------------------------------------------
