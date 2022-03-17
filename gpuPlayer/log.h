/***************************************************************************
      File Name:	Log.h
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"

#ifndef _LOG_H_
#define _LOG_H_

// disable compiler message:
//          warning C4002: too many actual parameters for macro 'LogAppend'
#pragma warning(disable: 4002)

//#define GPU_LOG_ENABLED
//#define SPU_LOG_ENABLED

#ifdef _DEBUG
	#ifdef GPU_LOG_ENABLED
		void GPULogStart();
		void GPULogAppend(const char *, ...);
		void GPULogEnd();
	#else
		#define GPULogStart()
		#define GPULogAppend()
		#define GPULogEnd()
	#endif
	#ifdef SPU_LOG_ENABLED
		void SPULogStart();
		void SPULogAppend(const char *, ...);
		void SPULogEnd();
	#else
		#define SPULogStart()
		#define SPULogAppend()
		#define SPULogEnd()
	#endif
#else
	#define GPULogStart()
	#define GPULogAppend()
	#define GPULogEnd()
	#define SPULogStart()
	#define SPULogAppend()
	#define SPULogEnd()
#endif

#endif