//------------------------------------------------------------------------------
// File: Bitmap.h
//
// Desc: DirectShow sample code - header file for VMR bitmap manipulation
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

//
// Constants
//
#pragma once
extern int gVideoW;
extern int gVideoH;

enum SHOW_TYPE
{
	SHOW_TYPE_POINT,
	SHOW_TYPE_LINE,
	SHOW_TYPE_STRING,
	SHOW_TYPE_RECT
};


struct S_SHOW
{
	SHOW_TYPE type;
	std::string str;
	POINT p1;
	POINT p2;
	COLORREF color;
};



extern std::vector<S_SHOW> gShowinfos;

#define TRANSPARENCY_VALUE   (0.5f)

#define PURE_WHITE          RGB(255,255,255)
#define ALMOST_WHITE        RGB(250,250,250)

#define BLEND_TEXT          TEXT("This is a demonstration of alpha-blended dynamic text.\0")
#define DYNAMIC_TEXT_SIZE   255

#define DEFAULT_FONT_NAME   TEXT("Impact\0")
#define DEFAULT_FONT_STYLE  TEXT("Regular\0")
#define DEFAULT_FONT_SIZE   12
#define DEFAULT_FONT_COLOR  RGB(255,0,0)
#define MAX_FONT_SIZE		25

#define STR_VMR_DISPLAY_WARNING  \
    TEXT("The VMR9 requires Direct3D9 in order to perform alpha blending.  ") \
    TEXT("Therefore, this sample requires that your display be set to a mode ") \
    TEXT("which is compatible with your computer's video card.  ") \
    TEXT("Most video cards support Direct3D in 16-bit and 32-bit RGB modes, ") \
    TEXT("and some newer cards support 16, 24 and 32-bit display modes.\r\n\r\n") \
    TEXT("To correct this problem, try changing your display to use ") \
    TEXT("16-bit or 32-bit color depth in the Display Control Panel applet.\0")

//
// Function prototypes
//
HRESULT BlendText(HWND hwndAp);

void StartTimer(void);
void StopTimer(void);

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

void SetVideoSize(int w,int h);
//
// Global data
//

extern TCHAR g_szAppText[DYNAMIC_TEXT_SIZE];
