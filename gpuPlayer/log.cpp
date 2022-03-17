/***************************************************************************
      File Name:	Log.cpp
   File Created:	Monday, October 6th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"
#include "log.h"
#ifdef _DEBUG

#ifdef GPU_LOG_ENABLED
#define GPU_LOG_FILE "gpuRecorder.log"

////////////////////////////////////////////////////////////////////////////

void GPULogStart()
{
	FILE *fp = fopen(GPU_LOG_FILE,"a");	
	if(fp) {
		char text[255];
		sprintf(text, "------------------------------------------------------------\r\n");
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}
	GPULogAppend("Begin log...");

}
////////////////////////////////////////////////////////////////////////////

void GPULogAppend(const char *formatstring, ...)
{
	FILE *fp = fopen(GPU_LOG_FILE,"a");
	if(fp) {
		SYSTEMTIME time;
		char buff[255], text[511];
		GetLocalTime(&time);
		va_list args;
		va_start(args, formatstring);
		_vsnprintf(buff, sizeof(buff), formatstring, args);
		va_end(args);
		sprintf(text, "%02d-%02d-%04d , %02d:%02d:%02d.%03d : %s\n",time.wDay,time.wMonth,time.wYear,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds, buff);
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}

}

////////////////////////////////////////////////////////////////////////////

void GPULogEnd()
{
	GPULogAppend("End log...");
	FILE *fp = fopen(GPU_LOG_FILE,"a");
	if(fp) {
		char text[255];
		sprintf(text, "------------------------------------------------------------\r\n");
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}
}

#endif

#ifdef SPU_LOG_ENABLED
#define SPU_LOG_FILE "spuRecorder.log"

////////////////////////////////////////////////////////////////////////////

void SPULogStart()
{
	FILE *fp = fopen(SPU_LOG_FILE,"a");	
	if(fp) {
		char text[255];
		sprintf(text, "------------------------------------------------------------\r\n");
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}
	SPULogAppend("Begin log...");

}
////////////////////////////////////////////////////////////////////////////

void SPULogAppend(const char *formatstring, ...)
{
	FILE *fp = fopen(SPU_LOG_FILE,"a");
	if(fp) {
		SYSTEMTIME time;
		char buff[255], text[511];
		GetLocalTime(&time);
		va_list args;
		va_start(args, formatstring);
		_vsnprintf(buff, sizeof(buff), formatstring, args);
		va_end(args);
		sprintf(text, "%02d-%02d-%04d , %02d:%02d:%02d.%03d : %s\n",time.wDay,time.wMonth,time.wYear,time.wHour,time.wMinute,time.wSecond,time.wMilliseconds, buff);
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}

}

////////////////////////////////////////////////////////////////////////////

void SPULogEnd()
{
	SPULogAppend("End log...");
	FILE *fp = fopen(SPU_LOG_FILE,"a");
	if(fp) {
		char text[255];
		sprintf(text, "------------------------------------------------------------\r\n");
		fwrite(text, strlen(text), 1, fp);
		fclose(fp);
	}
}

#endif

#endif