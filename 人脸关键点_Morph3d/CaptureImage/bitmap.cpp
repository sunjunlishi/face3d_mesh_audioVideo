//------------------------------------------------------------------------------
// File: Bitmap.cpp
//
// Desc: DirectShow sample code - Bitmap manipulation routines for 
//       VMR alpha-blended bitmap
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include <dshow.h>
#include "vmrutil.h"
#include "VideoMix.h"
#include "bitmap.h"
#include <opencv2\imgcodecs.hpp>
#include <opencv2\core.hpp>

//
// Constants
//
const float X_EDGE_BUFFER=0.05f; // Pixel buffer between bitmap and window edge
                                 // (represented in composition space [0 - 1.0f])
const float Y_EDGE_BUFFER=0.05f;

const int UPDATE_TIMER   = 50;
const int UPDATE_TIMEOUT = 50; // 2 seconds between ticker movements

int gVideoW = 640;
int gVideoH = 480;
//
// Global data
//
IVMRMixerBitmap9 *pBMP = NULL;
TCHAR g_szAppText[DYNAMIC_TEXT_SIZE]={0};
int gnTimer=0;
HPEN gBrush;
float g_fBitmapCompWidth=0;  // Width of bitmap in composition space units
int g_nImageWidth=0;         // Width of text bitmap

// Text font information
HFONT g_hFont=0;
LONG g_lFontPointSize   = DEFAULT_FONT_SIZE;
COLORREF g_rgbColors    = DEFAULT_FONT_COLOR;
TCHAR g_szFontName[100] = {DEFAULT_FONT_NAME};
TCHAR g_szFontStyle[32] = {DEFAULT_FONT_STYLE};
std::vector<S_SHOW> gShowinfos;
// Destination rectangle used for alpha-blended text
VMR9NormalizedRect  g_rDest={0};


HRESULT BlendText(HWND hwndApp)
{
	HRESULT hr = S_OK;
	
		LONG cx, cy;
	
		if (pWC == NULL) return hr;
		// Read the default video size
		hr = pWC->GetNativeVideoSize(&cx, &cy, NULL, NULL);
		if (FAILED(hr))
		{

			return hr;
		}

		// Create a device context compatible with the current window
		HDC hdc = GetDC(hwndApp);

		HDC hdcBmp = CreateCompatibleDC(hdc);


		// Determine the length of the string, then determine the
		// dimensions (in pixels) of the character string using the
		// currently selected font.  These dimensions are used to create
		// a bitmap below.
		int  nTextBmpWidth, nTextBmpHeight;

		nTextBmpHeight = gVideoH;
		nTextBmpWidth = gVideoW;

		// Create a new bitmap that is compatible with the current window
		HBITMAP hbm = CreateCompatibleBitmap(hdc, nTextBmpWidth, nTextBmpHeight);
		ReleaseDC(hwndApp, hdc);

		// Select our bitmap into the device context and save the old one
		BITMAP bm;
		HBITMAP hbmOld;
		GetObject(hbm, sizeof(bm), &bm);
		hbmOld = (HBITMAP)SelectObject(hdcBmp, hbm);

		// Set initial bitmap settings
		RECT rcText;
		SetRect(&rcText, 0, 0, nTextBmpWidth, nTextBmpHeight);


		HGDIOBJ oldBrush = SelectObject(hdcBmp, (HBRUSH)gBrush);

		
		Rectangle(hdcBmp, 0, 0, nTextBmpWidth, nTextBmpHeight);
		for (int k = 0; k < gShowinfos.size(); ++k)
		{
			S_SHOW item = gShowinfos[k];
			switch (item.type)
			{
			case SHOW_TYPE_LINE:
				MoveToEx(hdcBmp, item.p1.x, item.p1.y, NULL); //Cpoint m_ptOrigin;前面已经定义和赋值
				LineTo(hdcBmp, item.p2.x, item.p2.y);//point是函数本身参数，确定当前鼠标的位置
				break;
			case SHOW_TYPE_POINT:
				char szBuf[256];
				//sprintf(szBuf, "%d %d %d %d",item.p1.x,item.p1.y,item.p2.x,item.p2.y);

				Ellipse(hdcBmp, item.p1.x, item.p1.y, item.p2.x, item.p2.y);
				
				break;
			case SHOW_TYPE_STRING:
				// Draw the requested text string onto the bitmap
				TextOut(hdcBmp, item.p1.x, item.p1.y, item.str.c_str(),item.str.size());
				break;
			case SHOW_TYPE_RECT:
				//Polyline(hdcBmp, pts, 3);
				//Rectangle(hdcBmp, posX, posY, posRX, posBY);
				break;
			default:
				break;
			}
			
		}


		// Configure the VMR's bitmap structure
		VMR9AlphaBitmap bmpInfo;
		ZeroMemory(&bmpInfo, sizeof(bmpInfo));
		bmpInfo.dwFlags = VMRBITMAP_HDC;
		bmpInfo.hdc = hdcBmp;  // DC which has selected our bitmap

							   // Remember the width of this new bitmap
		g_nImageWidth = bm.bmWidth;

		// Save the ratio of the bitmap's width to the width of the video file.
		// This value is used to reposition the bitmap in composition space.
		g_fBitmapCompWidth = (float)g_nImageWidth / (float)cx;

		// Display the bitmap in the bottom right corner.
		// rSrc specifies the source rectangle in the GDI device context 
		// rDest specifies the destination rectangle in composition space (0.0f to 1.0f)
		bmpInfo.rDest.left = 0.0f;// +X_EDGE_BUFFER;
		bmpInfo.rDest.right = 1.0f;// -X_EDGE_BUFFER;
		bmpInfo.rDest.top = (float)(cy - bm.bmHeight) / (float)cy;// -Y_EDGE_BUFFER;
		bmpInfo.rDest.bottom = 1.0f;// -Y_EDGE_BUFFER;
		bmpInfo.rSrc = rcText;

		// Transparency value 1.0 is opaque, 0.0 is transparent.
		bmpInfo.fAlpha = TRANSPARENCY_VALUE;

		// Set the COLORREF so that the bitmap outline will be transparent
		SetColorRef(bmpInfo);

		// Give the bitmap to the VMR for display
		hr = pBMP->SetAlphaBitmap(&bmpInfo);
		if (FAILED(hr))
		{

		}
		// Select the initial objects back into our device context
		DeleteObject(SelectObject(hdcBmp, hbmOld));
		SelectObject(hdc, oldBrush);

		// Clean up resources
		DeleteObject(hbm);
		DeleteDC(hdcBmp);
	
   

    return hr;
}

void SetVideoSize(int w, int h)
{
	gVideoW = w;
	gVideoH = h;
}


void SetColorRef(VMR9AlphaBitmap& bmpInfo)
{
    // Set the COLORREF so that the bitmap outline will be transparent
    bmpInfo.clrSrcKey = RGB(255, 255, 255);  // Pure white
    bmpInfo.dwFlags |= VMRBITMAP_SRCCOLORKEY;
}




void StartTimer(void)
{
	gBrush = CreatePen(1, 2, RGB(255, 0, 0));//GetStockObject(WHITE_BRUSH);
    //gnTimer = (int) SetTimer(NULL, UPDATE_TIMER, UPDATE_TIMEOUT, TimerProc);
}

void StopTimer(void)
{
    if (gnTimer)
    {
        KillTimer(NULL, gnTimer);
        gnTimer = 0;
    }
}

VOID CALLBACK TimerProc(
  HWND hwnd,         // handle to window
  UINT uMsg,         // WM_TIMER message
  UINT_PTR idEvent,  // timer identifier
  DWORD dwTime       // current system time
)
{

}

