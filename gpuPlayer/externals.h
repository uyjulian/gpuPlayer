/***************************************************************************
      File Name:	Externals.h
   File Created:	Sunday, September 28rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#ifndef _EXTERNALS_H_
#define _EXTERNALS_H_
#include "stdafx.h"

extern char*			(CALLBACK *PSEgetLibName)(void);
extern unsigned long	(CALLBACK *PSEgetLibType)(void);
extern unsigned long	(CALLBACK *PSEgetLibVersion)(void);

extern long				(CALLBACK *GPUinit)(void);
extern long				(CALLBACK *GPUshutdown)(void);
extern void				(CALLBACK *GPUmakeSnapshot)(void);
extern long				(CALLBACK *GPUopen)(HWND);
extern long				(CALLBACK *GPUclose)(void);
extern void				(CALLBACK *GPUupdateLace)(void);
extern unsigned long	(CALLBACK *GPUreadStatus)(void);
extern void				(CALLBACK *GPUwriteStatus)(unsigned long);
extern unsigned long	(CALLBACK *GPUreadData)(void);
extern void				(CALLBACK *GPUreadDataMem)(unsigned long*,int);
extern void				(CALLBACK *GPUwriteData)(unsigned long);
extern void				(CALLBACK *GPUwriteDataMem)(unsigned long*,int);
extern void				(CALLBACK *GPUsetMode)(unsigned long);
extern long				(CALLBACK *GPUgetMode)(void);
extern long				(CALLBACK *GPUdmaChain)(unsigned long*,unsigned long);
extern long				(CALLBACK *GPUconfigure)(void);
extern void				(CALLBACK *GPUabout)(void);
extern long				(CALLBACK *GPUtest)(void);
extern long				(CALLBACK *GPUfreeze)(unsigned long,void*);
extern void				(CALLBACK *GPUdisplayText)(char * pText);
extern void				(CALLBACK *GPUdisplayFlags)(unsigned long dwFlags);
extern void				(CALLBACK *GPUgetScreenPic)(unsigned char*);
extern void				(CALLBACK *GPUshowScreenPic)(unsigned char*);

extern long				(CALLBACK *SPUtest)(void);
extern long				(CALLBACK *SPUconfigure)(void);
extern void				(CALLBACK *SPUabout)(void);
extern long				(CALLBACK *SPUinit)(void);
extern long				(CALLBACK *SPUshutdown)(void);
extern long				(CALLBACK *SPUopen)(HWND);
extern long				(CALLBACK *SPUclose)(void);
extern unsigned short	(CALLBACK *SPUreadRegister)(unsigned long);
extern void				(CALLBACK *SPUwriteRegister)(unsigned long, unsigned short);
extern unsigned short	(CALLBACK *SPUreadDMA)(void);
extern void				(CALLBACK *SPUwriteDMA)(unsigned short);
extern void				(CALLBACK *SPUreadDMAMem)(unsigned short*, int);
extern void				(CALLBACK *SPUwriteDMAMem)(unsigned short*, int);
extern void				(CALLBACK *SPUregisterCallback)(void (CALLBACK *callback)(void));
extern void				(CALLBACK *SPUregisterCDDAVolume)(void (CALLBACK *CDDAVcallback)(unsigned short, unsigned short));
extern long				(CALLBACK *SPUfreeze)(unsigned long, void*);
extern void				(CALLBACK *SPUplayADPCMchannel)(void*);
extern unsigned short	(CALLBACK *SPUgetOne)(unsigned long);
extern void				(CALLBACK *SPUputOne)(unsigned long, unsigned short);
extern void				(CALLBACK *SPUplaySample)(unsigned char);
extern void				(CALLBACK *SPUsetAddr)(unsigned char, unsigned short);
extern void				(CALLBACK *SPUsetPitch)(unsigned char, unsigned short);
extern void				(CALLBACK *SPUsetVolumeL)(unsigned char, short);
extern void				(CALLBACK *SPUsetVolumeR)(unsigned char, short);
extern void				(CALLBACK *SPUstartChannels1)(unsigned short);
extern void				(CALLBACK *SPUstartChannels2)(unsigned short);
extern void				(CALLBACK *SPUstopChannels1)(unsigned short);
extern void				(CALLBACK *SPUstopChannels2)(unsigned short);
extern void				(CALLBACK *SPUplaySector)(unsigned long, unsigned char*);

extern BOOL PluginsLoaded;

BOOL LoadPlugins();
void FreePlugins();

#endif