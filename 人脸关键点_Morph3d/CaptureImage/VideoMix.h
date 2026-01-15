//------------------------------------------------------------------------------
// File: Text.h
//
// Desc: DirectShow sample code - header file for VMR Text sample
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//
// Function prototypes

#ifdef DXUSBVIDEO_EXPORTS
#define DLL_SAMPLE_API __declspec(dllexport)
#else
#define DLL_SAMPLE_API __declspec(dllimport)
#endif

#include <opencv2\core.hpp>
#include  "bitmap.h"
DLL_SAMPLE_API HRESULT InitVideoWindow(int nMultiplier, int nDivider);

DLL_SAMPLE_API void DoShowClearinfo();
DLL_SAMPLE_API void DoShowAddInfo(S_SHOW sshow);
DLL_SAMPLE_API void DoShowFaceRect();

DLL_SAMPLE_API void DoResetShow(HWND wnd, CVdoFrameHandler * FrameHandler);

HRESULT HandleGraphEvent(void);


BOOL CheckVideoVisibility(void);

void MoveVideoWindow(void);
void CloseInterfaces(void);

void CloseClip(void);


class CVdoFrameHandler;
DLL_SAMPLE_API HRESULT PlayMovieInWindow(HWND wnd);


void OnPaint(HWND hwnd);



DLL_SAMPLE_API void mix_startCap(CVdoFrameHandler * FrameHandler);
DLL_SAMPLE_API HRESULT mix_getLpdata(unsigned char ** data);

//
// Constants
//

// File filter for OpenFile dialog
#define FILE_FILTER_TEXT \
    TEXT("Video Files (*.asf; *.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv)\0*.asf; *.avi; *.qt; *.mov; *.mpg; *.mpeg; *.m1v; *.wmv\0\0")

// Begin default media search at root directory
#define DEFAULT_MEDIA_PATH  TEXT("\\\0")

// Defaults used with audio-only files
#define DEFAULT_PLAYER_WIDTH    240
#define DEFAULT_PLAYER_HEIGHT   120
#define DEFAULT_VIDEO_WIDTH     320
#define DEFAULT_VIDEO_HEIGHT    240
#define MINIMUM_VIDEO_WIDTH     200
#define MINIMUM_VIDEO_HEIGHT    120

#define APPLICATIONNAME TEXT("VMR9 Text\0")
#define CLASSNAME       TEXT("VMR9Text\0")

#define WM_GRAPHNOTIFY  WM_USER+13


//
// Macros
//
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define JIF(x) if (FAILED(hr=(x))) \
    {return hr;}

#define LIF(x) if (FAILED(hr=(x))) \
    {;}

//
// Resource constants
//
