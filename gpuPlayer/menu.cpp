/***************************************************************************
      File Name:	Menu.cpp
   File Created:	Tuesday, September 23rd 2003
      Copyright:	(C) 2003 by Darko Matesic
          Email:	darkman@eunet.yu
 ***************************************************************************/

#include "stdafx.h"

///////////////////////////////////////////////////////////////////////////////
typedef DWORD HLSCOLOR;
#define HLS(h,l,s) ((HLSCOLOR)(((BYTE)(h)|((WORD)((BYTE)(l))<<8))|(((DWORD)(BYTE)(s))<<16)))

///////////////////////////////////////////////////////////////////////////////
#define HLS_H(hls) ((BYTE)(hls))
#define HLS_L(hls) ((BYTE)(((WORD)(hls)) >> 8))
#define HLS_S(hls) ((BYTE)((hls)>>16))

///////////////////////////////////////////////////////////////////////////////
HLSCOLOR RGB2HLS (COLORREF rgb)
{
    unsigned char minval = min(GetRValue(rgb), min(GetGValue(rgb), GetBValue(rgb)));
    unsigned char maxval = max(GetRValue(rgb), max(GetGValue(rgb), GetBValue(rgb)));
    float mdiff  = float(maxval) - float(minval);
    float msum   = float(maxval) + float(minval);
   
    float luminance = msum / 510.0f;
    float saturation = 0.0f;
    float hue = 0.0f; 

    if ( maxval != minval )
    { 
        float rnorm = (maxval - GetRValue(rgb)  ) / mdiff;      
        float gnorm = (maxval - GetGValue(rgb)) / mdiff;
        float bnorm = (maxval - GetBValue(rgb) ) / mdiff;   

        saturation = (luminance <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

        if (GetRValue(rgb) == maxval) hue = 60.0f * (6.0f + bnorm - gnorm);
        if (GetGValue(rgb) == maxval) hue = 60.0f * (2.0f + rnorm - bnorm);
        if (GetBValue(rgb) == maxval) hue = 60.0f * (4.0f + gnorm - rnorm);
        if (hue > 360.0f) hue = hue - 360.0f;
    }
    return HLS ((hue*255)/360, luminance*255, saturation*255);
}

///////////////////////////////////////////////////////////////////////////////
static BYTE _ToRGB (float rm1, float rm2, float rh)
{
  if      (rh > 360.0f) rh -= 360.0f;
  else if (rh <   0.0f) rh += 360.0f;
 
  if      (rh <  60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;   
  else if (rh < 180.0f) rm1 = rm2;
  else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;      
                   
  return (BYTE)(rm1 * 255);
}

///////////////////////////////////////////////////////////////////////////////
COLORREF HLS2RGB (HLSCOLOR hls)
{
    float hue        = ((int)HLS_H(hls)*360)/255.0f;
    float luminance  = HLS_L(hls)/255.0f;
    float saturation = HLS_S(hls)/255.0f;

    if ( saturation == 0.0f )
    {
        return RGB (HLS_L(hls), HLS_L(hls), HLS_L(hls));
    }
    float rm1, rm2;
     
    if ( luminance <= 0.5f ) rm2 = luminance + luminance * saturation;  
    else                     rm2 = luminance + saturation - luminance * saturation;
    rm1 = 2.0f * luminance - rm2;   
    BYTE red   = _ToRGB (rm1, rm2, hue + 120.0f);   
    BYTE green = _ToRGB (rm1, rm2, hue);
    BYTE blue  = _ToRGB (rm1, rm2, hue - 120.0f);

    return RGB (red, green, blue);
}

///////////////////////////////////////////////////////////////////////////////
COLORREF HLS_TRANSFORM (COLORREF rgb, int percent_L, int percent_S)
{
    HLSCOLOR hls = RGB2HLS (rgb);
    BYTE h = HLS_H(hls);
    BYTE l = HLS_L(hls);
    BYTE s = HLS_S(hls);

    if ( percent_L > 0 )
    {
        l = BYTE(l + ((255 - l) * percent_L) / 100);
    }
    else if ( percent_L < 0 )
    {
        l = BYTE((l * (100+percent_L)) / 100);
    }
    if ( percent_S > 0 )
    {
        s = BYTE(s + ((255 - s) * percent_S) / 100);
    }
    else if ( percent_S < 0 )
    {
        s = BYTE((s * (100+percent_S)) / 100);
    }
    return HLS2RGB (HLS(h, l, s));
}
///////////////////////////////////////////////////////////////////////////////

#define IMGWIDTH 16
#define IMGHEIGHT 16
#define IMGPADDING 6
#define TEXTPADDING 8
#define TEXTPADDING_MNUBR 4
#define SM_CXSHADOW 4

typedef struct tagMenuItems {
	HMENU  hParentMenu;
	INT    ItemIndex;
	HMENU  hMenu;
	INT    ItemCount;
	struct tagMenuItems *lpItems;
	INT    ItemID;
	CHAR   szCaption[96], szShortcut[32];
	INT    CaptionLength, ShortcutLength;
	INT    ImageIndex;
	BOOL   IsMenuBar;
	BOOL   IsSeparator;
	BOOL   IsRadioCheck;
	struct tagMenuItems *Next;
} MENUITEMS, *LPMENUITEMS;

LPMENUITEMS Menu_lpItems = NULL;
HFONT		Menu_hFont = NULL;
HBITMAP		Menu_hbmCheck = NULL;
int			Menu_CheckImage = -1;
HIMAGELIST	Menu_hImages = NULL;


//==========================================================================
// Initialize menu structure
//==========================================================================

LPMENUITEMS Menu_GetMenuItems(HMENU hMenu, BOOL IsMenuBar)
{
	if(hMenu == NULL) return NULL;
	LPMENUITEMS lpItems = NULL, prev = NULL;
	char *tab;
	int Count = GetMenuItemCount(hMenu);
	for(int i=0; i<Count; i++) {
		LPMENUITEMS lpItem = (LPMENUITEMS)malloc(sizeof(MENUITEMS));
		if(lpItem == NULL) continue;
		memset(lpItem, 0, sizeof(MENUITEMS));
		lpItem->hParentMenu = hMenu;
		lpItem->ItemIndex = i;
		lpItem->hMenu = GetSubMenu(hMenu, i);
		if(lpItem->hMenu) {
			lpItem->ItemCount = GetMenuItemCount(lpItem->hMenu);
			lpItem->lpItems = Menu_GetMenuItems(lpItem->hMenu, FALSE);
		}
		MENUITEMINFO mii;
		memset(&mii, 0, sizeof(MENUITEMINFO));
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_SUBMENU;
		GetMenuItemInfo(hMenu, i, TRUE, &mii);
		lpItem->ItemID = mii.wID;
		GetMenuString(hMenu, i, lpItem->szCaption, sizeof(lpItem->szCaption), MF_BYPOSITION);
		tab = strchr(lpItem->szCaption,'\t');
		if(tab) {
			*(tab++) = 0;
			int i=0;
			while(*tab && i<30) lpItem->szShortcut[i++] = *(tab++);
			lpItem->ShortcutLength = strlen(lpItem->szShortcut);
		}
		lpItem->CaptionLength = strlen(lpItem->szCaption);
		lpItem->ImageIndex = -1;
		lpItem->IsMenuBar = IsMenuBar;
		lpItem->IsSeparator = mii.fType & MFT_SEPARATOR;
		lpItem->IsRadioCheck = mii.fType & MFT_RADIOCHECK;
		lpItem->Next = NULL;
		if(prev) prev->Next = lpItem;
		prev = lpItem;
		if(lpItems == NULL) lpItems = lpItem;

		memset(&mii,0,sizeof(MENUITEMINFO)); 
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_FTYPE | MIIM_DATA;
		mii.fType = MFT_OWNERDRAW;
		mii.dwItemData = (DWORD)lpItem;
		SetMenuItemInfo(hMenu, i, TRUE, &mii);
	}
	return lpItems;
}

BOOL Menu_Init(HMENU hMenu)
{
	if(hMenu == NULL) return FALSE;
	Menu_lpItems = (LPMENUITEMS)malloc(sizeof(MENUITEMS));
	if(Menu_lpItems == NULL) return FALSE;
	memset(Menu_lpItems, 0, sizeof(MENUITEMS));
	Menu_lpItems->hMenu = hMenu;
	if(Menu_lpItems->hMenu) Menu_lpItems->ItemCount = GetMenuItemCount(Menu_lpItems->hMenu);
	Menu_lpItems->lpItems = Menu_GetMenuItems(Menu_lpItems->hMenu, TRUE);
	Menu_lpItems->Next = NULL;

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	lf.lfHeight = 16;
	strcpy(lf.lfFaceName,"Tahoma");
	Menu_hFont = CreateFontIndirect(&lf);
	
	Menu_hbmCheck = LoadBitmap (NULL, MAKEINTRESOURCE(32760)); // OBM_CHECK

	return TRUE;
}

//==========================================================================
// Free allocated memory
//==========================================================================

void Menu_DeleteMenuItems(LPMENUITEMS lpItems)
{
	LPMENUITEMS next;
	while(lpItems) {
		if(lpItems->lpItems) Menu_DeleteMenuItems(lpItems->lpItems);
		next = lpItems->Next;
		free(lpItems);
		lpItems = next;
	}
}

void Menu_DeInit()
{
	Menu_DeleteMenuItems(Menu_lpItems);
	Menu_lpItems = NULL;
	DeleteFont(Menu_hFont); Menu_hFont = NULL;
	DeleteBitmap(Menu_hbmCheck); Menu_hbmCheck = NULL;
	if(Menu_hImages) {
		ImageList_Destroy(Menu_hImages);
		Menu_hImages = NULL;
	}
	Menu_CheckImage = -1;
}

//==========================================================================
// Associate menu item with image
//==========================================================================

BOOL Menu_SetImageList(HINSTANCE hInstance, INT BitmapID)
{
	if(Menu_hImages) {
		ImageList_Destroy(Menu_hImages);
		Menu_hImages = NULL;
	}

	HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(BitmapID), RT_BITMAP);
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(hInstance, hRes));
	int ImageCount = lpbi->biWidth / 16;
	HBITMAP hbm = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(BitmapID), IMAGE_BITMAP, 0, 0 , LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	Menu_hImages = ImageList_Create(IMGWIDTH, IMGHEIGHT, ILC_COLOR24 | ILC_MASK , ImageCount, 0);
	ImageList_AddMasked(Menu_hImages, hbm, CLR_DEFAULT);
	DeleteBitmap(hbm);

	return (Menu_hImages != NULL);
}

LPMENUITEMS Menu_FindItem(LPMENUITEMS lpItems, int ItemID)
{
	while(lpItems) {
		if(lpItems->ItemID == ItemID) return lpItems;
		if(lpItems->lpItems) {
			LPMENUITEMS lpItem = Menu_FindItem(lpItems->lpItems, ItemID);
			if(lpItem) return lpItem;
		}
		lpItems = lpItems->Next;
	}
	return NULL;
}

BOOL Menu_SetItemImage(INT ItemID, INT ImageIndex)
{
	LPMENUITEMS lpItem = Menu_FindItem(Menu_lpItems, ItemID);
	if(lpItem) {
		lpItem->ImageIndex = ImageIndex;
		return TRUE;
	}
	return FALSE;
}

void Menu_SetCheckImage(INT ImageIndex)
{
	Menu_CheckImage = ImageIndex;
}


//==========================================================================
// Process message WM_MEASUREITEM
//==========================================================================

void Menu_MeasureItem(HWND hwnd, LPMEASUREITEMSTRUCT lpmis)
{
	LPMENUITEMS lpItem = (LPMENUITEMS)lpmis->itemData;
    HDC hdc = GetDC(hwnd); 
	HFONT hPrevFont = SelectFont(hdc, Menu_hFont);
    char szText[256];
    SIZE size = { 0, 0};
    int i = 0, j = 0, k = 0;
    while(lpItem->szCaption[j] != 0 && i < 255) {
		if(lpItem->szCaption[j] != '&') szText[i++] = lpItem->szCaption[j];
		j++;
	}
    while(lpItem->szShortcut[k] != 0 && i < 255) {
		if(lpItem->szShortcut[k] != '&') szText[i++] = lpItem->szShortcut[k];
		k++;
	}
	szText[i++] = 0;
    GetTextExtentPoint32(hdc, szText, strlen(szText), &size);
    if(lpItem->IsMenuBar) {
		lpmis->itemWidth = size.cx;
		lpmis->itemHeight = size.cy; 
	} else if(lpItem->IsSeparator) {
		lpmis->itemWidth = 0;
		lpmis->itemHeight = 6;
	} else {
		lpmis->itemWidth = size.cx + IMGWIDTH + IMGPADDING + TEXTPADDING + 4;
		lpmis->itemHeight = IMGHEIGHT + 4; 
	}
	SelectFont(hdc, hPrevFont);
    ReleaseDC(hwnd, hdc); }

//==========================================================================
// Process message WM_DRAWITEM
//==========================================================================

void Menu_DrawItem(LPDRAWITEMSTRUCT lpdis)
{
	LPMENUITEMS lpItem = (LPMENUITEMS)lpdis->itemData;
	RECT rc = lpdis->rcItem;
	
	BOOL IsMenuBar = lpItem->IsMenuBar;
	BOOL IsSeparator = lpItem->IsSeparator;
	BOOL IsSelected = lpdis->itemState & ODS_SELECTED;
	BOOL IsChecked  = lpdis->itemState & ODS_CHECKED;
	BOOL IsHotLight = lpdis->itemState & ODS_HOTLIGHT;
	BOOL IsInactive = lpdis->itemState & ODS_INACTIVE;
	BOOL IsDisabled = IsSeparator ? TRUE : lpdis->itemState & ODS_DISABLED;
	
	HBRUSH hbrPrev;
	HPEN hpenPrev;
	
	//
	// Draw Item Background
	//
		
	if(IsMenuBar && IsSelected) {

		HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
		HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), 20, 0));
        rc.right -= TEXTPADDING_MNUBR;
        hbrPrev = SelectBrush(lpdis->hDC, hBrush);
        hpenPrev = SelectPen(lpdis->hDC, hPen);
        Rectangle(lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom);
        SelectBrush(lpdis->hDC, hbrPrev);
        SelectPen(lpdis->hDC, hpenPrev);
		DeletePen(hPen);
		DeleteBrush(hBrush);

		HBRUSH hShadow = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -10, 0));
		RECT rcShadow = lpdis->rcItem;
		rcShadow.left = rcShadow.right - TEXTPADDING_MNUBR;
		rcShadow.top += TEXTPADDING_MNUBR;
		FillRect(lpdis->hDC, &rcShadow, hShadow);
		DeleteBrush(hShadow);
	} else if(!IsSeparator && (IsSelected || (IsHotLight && !IsInactive))) {

		HBRUSH hBrush = CreateSolidBrush(IsDisabled ? HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), +75, 0) : HLS_TRANSFORM(GetSysColor(COLOR_HIGHLIGHT), +70, -57));
		if(IsMenuBar) {
			rc.right -= TEXTPADDING_MNUBR;
		}
		HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
		hbrPrev = SelectBrush(lpdis->hDC, hBrush);
		hpenPrev = SelectPen(lpdis->hDC, hPen);
		Rectangle(lpdis->hDC, rc.left, rc.top, rc.right, rc.bottom);
        SelectBrush(lpdis->hDC, hbrPrev);
		SelectPen(lpdis->hDC, hpenPrev);
		DeletePen(hPen);
		DeleteBrush(hBrush);

	} else if(!IsMenuBar) {

		HBRUSH hBrush1 = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), +20, 0));
		HBRUSH hBrush2 = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), +75, 0));
		rc.right = IMGWIDTH+IMGPADDING;
		FillRect(lpdis->hDC, &rc, hBrush1);
        rc.left = rc.right;
        rc.right = lpdis->rcItem.right;
		FillRect(lpdis->hDC, &rc, hBrush2);
		DeleteBrush(hBrush1);
		DeleteBrush(hBrush2);

	} else {
		FillRect(lpdis->hDC, &rc, GetSysColorBrush(COLOR_3DFACE));
	}

	//
	// Draw Item Foreground
	//
	
	rc = lpdis->rcItem;
	if(IsSeparator) {

		HPEN hPen = CreatePen(PS_SOLID, 1, HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -18, 0));
//		HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_GRAYTEXT));	
		hpenPrev = SelectPen(lpdis->hDC, hPen);
        MoveToEx(lpdis->hDC, rc.left+IMGWIDTH+IMGPADDING+TEXTPADDING,  (rc.top+rc.bottom)/2, NULL);
        LineTo(lpdis->hDC, rc.right-1, (rc.top+rc.bottom)/2);
        SelectPen(lpdis->hDC, hpenPrev);

	} else {

		//
		// Draw Caption & Shortcut
		//
		
		COLORREF clrPrev = SetTextColor(lpdis->hDC, IsDisabled ? GetSysColor(COLOR_GRAYTEXT) : GetSysColor(COLOR_MENUTEXT));
		SetBkMode(lpdis->hDC, TRANSPARENT);
		rc.top += 2;

		if(IsMenuBar) {
			DrawText(lpdis->hDC, lpItem->szCaption, lpItem->CaptionLength, &rc, DT_VCENTER | DT_CENTER);
		} else {

			rc.left += IMGWIDTH + IMGPADDING + TEXTPADDING;
			DrawText(lpdis->hDC, lpItem->szCaption, lpItem->CaptionLength, &rc, DT_VCENTER);
			if(lpItem->ShortcutLength > 0) {
				SIZE size;
			    GetTextExtentPoint32(lpdis->hDC, lpItem->szShortcut, lpItem->ShortcutLength, &size);
			    rc.left = rc.right - size.cx - TEXTPADDING;
				DrawText(lpdis->hDC, lpItem->szShortcut, lpItem->ShortcutLength, &rc, DT_VCENTER);		    			
			}
			
			//
			// Draw checkmark
			//
			rc = lpdis->rcItem;
			
			if(IsChecked) {
				
				if(Menu_CheckImage >= 0) {
					rc.left += 3;
					rc.right = rc.left + IMGWIDTH - 1;
					rc.top += 3;
					rc.bottom = rc.top + IMGHEIGHT - 1;
					if(IsDisabled) {
						HICON hIcon = ImageList_ExtractIcon (NULL, Menu_hImages, Menu_CheckImage);
						HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -27, 0));
						DrawState(lpdis->hDC, hBrush, NULL,(LPARAM)hIcon, 0, rc.left, rc.top, IMGWIDTH, IMGHEIGHT, DST_ICON | DSS_MONO);
						DeleteBrush(hBrush);
						DestroyIcon (hIcon);
					} else if(IsSelected) {
						HICON hIcon = ImageList_ExtractIcon (NULL, Menu_hImages, Menu_CheckImage);
						HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -27, 0));
						DrawState(lpdis->hDC, hBrush, NULL,(LPARAM)hIcon, 0, rc.left+1, rc.top+1, IMGWIDTH, IMGHEIGHT, DST_ICON | DSS_MONO);
						DeleteBrush(hBrush);
						DestroyIcon (hIcon);
						ImageList_Draw(Menu_hImages, Menu_CheckImage, lpdis->hDC, rc.left-1, rc.top-1, ILD_TRANSPARENT);
					} else
						ImageList_Draw(Menu_hImages, Menu_CheckImage, lpdis->hDC, rc.left, rc.top, ILD_TRANSPARENT);

				} else {
					if(IsDisabled) {
						HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -27, 0));
						hbrPrev = SelectBrush(lpdis->hDC, hBrush);
						rc.left = rc.left + IMGWIDTH/4 + 1;
						rc.right = rc.left + IMGWIDTH - 1;
						DrawState(lpdis->hDC, hBrush, NULL,(LPARAM)Menu_hbmCheck, 0, rc.left, rc.top+3, IMGWIDTH, IMGHEIGHT, DST_BITMAP | DSS_MONO);
						SelectBrush(lpdis->hDC, hbrPrev);
						DeleteBrush(hBrush);
					} else {
						COLORREF clrBrush = HLS_TRANSFORM(GetSysColor(COLOR_HIGHLIGHT), +70, -57);
						HBRUSH hBrush = CreateSolidBrush(clrBrush);
						HPEN hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
						hbrPrev = SelectBrush(lpdis->hDC, hBrush);
						hpenPrev = SelectPen(lpdis->hDC, hPen);
						Rectangle(lpdis->hDC, rc.left+1, rc.top+1, rc.left+IMGWIDTH+4, rc.bottom-1);
	
						rc.left = rc.left + IMGWIDTH/4 + 1;
						rc.right = rc.left + IMGWIDTH - 1;
						SetBkColor(lpdis->hDC, clrBrush);
						DrawState(lpdis->hDC, NULL, NULL,(LPARAM)Menu_hbmCheck, 0, rc.left, rc.top+3, IMGWIDTH, IMGHEIGHT, DST_BITMAP | DSS_NORMAL);
	
						SelectBrush(lpdis->hDC, hbrPrev);
						SelectPen(lpdis->hDC, hpenPrev);
						DeletePen(hPen);
						DeleteBrush(hBrush);
					}
				}

			} else if(lpItem->ImageIndex >= 0) {

				rc.left += 3;
				rc.right = rc.left + IMGWIDTH - 1;
				rc.top += 3;
				rc.bottom = rc.top + IMGHEIGHT - 1;
				if(IsDisabled) {
					HICON hIcon = ImageList_ExtractIcon (NULL, Menu_hImages, lpItem->ImageIndex);
					HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -27, 0));
					DrawState(lpdis->hDC, hBrush, NULL,(LPARAM)hIcon, 0, rc.left, rc.top, IMGWIDTH, IMGHEIGHT, DST_ICON | DSS_MONO);
					DeleteBrush(hBrush);
					DestroyIcon (hIcon);
				} else if(IsSelected) {
					HICON hIcon = ImageList_ExtractIcon (NULL, Menu_hImages, lpItem->ImageIndex);
					HBRUSH hBrush = CreateSolidBrush(HLS_TRANSFORM(GetSysColor(COLOR_3DFACE), -27, 0));
					DrawState(lpdis->hDC, hBrush, NULL,(LPARAM)hIcon, 0, rc.left+1, rc.top+1, IMGWIDTH, IMGHEIGHT, DST_ICON | DSS_MONO);
					DeleteBrush(hBrush);
					DestroyIcon (hIcon);
					ImageList_Draw(Menu_hImages, lpItem->ImageIndex, lpdis->hDC, rc.left-1, rc.top-1, ILD_TRANSPARENT);
				} else
					ImageList_Draw(Menu_hImages, lpItem->ImageIndex, lpdis->hDC, rc.left, rc.top, ILD_TRANSPARENT);
			}
		}
	}
}