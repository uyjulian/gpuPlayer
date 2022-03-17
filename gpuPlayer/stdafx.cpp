/***************************************************************************
      File Name:	StdAfx.h
   File Created:	Sunday, October 12th 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////

LPSTR FormatLong(DWORD Value)
{
	static char buff[MAX_PATH];
	char temp[MAX_PATH];
	int i, j, c = 0;
	sprintf(temp, "%lu", Value);
	i = strlen(temp); 
	j = i + (i - 1) / 3;
	buff[j--] = temp[i--];
	while(i >= 0) {
		buff[j--]=temp[i--];
		if((c++%3)==2) buff[j--]=',';
	}
	return buff;
}

////////////////////////////////////////////////////////////////////////////

LPSTR FormatDouble(DOUBLE Value, INT Decimals)
{
	static char buff[MAX_PATH];
	strcpy(buff, FormatLong((DWORD)Value));
	if(Decimals > 0) {
		char temp[MAX_PATH];
		int i = 0;
		sprintf(temp, "%f", Value);
		while(temp[i] != 0 && temp[i] != '.' ) i++;
		if(temp[i] == '.') {
			int l = strlen(buff);
			for(int j = 0; j <= Decimals; j++) buff[l++] = temp[i++];
			buff[l] = 0;
		}
	}
	return buff;
}

////////////////////////////////////////////////////////////////////////////

INT MessageBoxFormat(HWND hWnd, LPCSTR FormatString, LPCSTR lpCaption, UINT uType, ...)
{
	char buff[1024];
	va_list args;
	va_start(args, FormatString);
	_vsnprintf(buff, sizeof(buff), FormatString, args);
	va_end(args);
	return MessageBox(hWnd, buff, lpCaption, uType);
}

////////////////////////////////////////////////////////////////////////////

void SetWindowTextFormat(HWND hWnd, LPCSTR FormatString, ...)
{
	char buff[1024];
	va_list args;
	va_start(args, FormatString);
	_vsnprintf(buff, sizeof(buff), FormatString, args);
	va_end(args);
	SetWindowText(hWnd, buff);
}

////////////////////////////////////////////////////////////////////////////
