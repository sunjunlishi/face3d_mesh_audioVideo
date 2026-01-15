//------------------------------------------------------------------------------
// File: VMRUtil.h
//
// Desc: DirectShow sample code - header file for C++ VMR9 sample applications
//       that do not use MFC.  This header contains several methods useful
//       for creating filter graphs with the Video Mixing Renderer 9.
//
//       Because graph building with the VMR9 requires a few extra steps
//       in order to guarantee that the VMR9 is used instead of another
//       video renderer, these helper methods are implemented in a header file
//       so that they can be easily integrated into a non-MFC application.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#ifndef __INC_VMRUTIL_H__
#define __INC_VMRUTIL_H__

#pragma once
// VMR9 Headers
#include <dshow.h>
#include <d3d9.h>
#include <vmr9.h>


HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);

void SetColorRef(VMR9AlphaBitmap& bmpInfo);
extern IVMRMixerBitmap9 *pBMP;
//
// Global data
//
extern HWND      ghApp;
extern HMENU     ghMenu;
extern HINSTANCE ghInst;
extern TCHAR     g_szFileName[MAX_PATH];
extern DWORD     g_dwGraphRegister;

// DirectShow interfaces
extern IGraphBuilder *pGB;
extern IVMRWindowlessControl9 *pWC;
extern IMediaControl *gPMC;
extern IMediaEventEx *pME;
extern IMediaSeeking *pMS;
extern IAMStreamConfig *gPVSC;

HRESULT InitializeWindowlessVMR(IBaseFilter **ppVmr9);
#ifndef JIF
#define JIF(x) if (FAILED(hr=(x))) {return hr;}
#endif

//----------------------------------------------------------------------------
//  VerifyVMR9
// 
//  Verifies that VMR9 COM objects exist on the system and that the VMR9
//  can be instantiated.
//
//  Returns: FALSE if the VMR9 can't be created
//----------------------------------------------------------------------------

BOOL VerifyVMR9(void);
class CVdoFrameHandler;
extern CVdoFrameHandler * gFrameHandle;
#include <vector>


HRESULT GetUnconnectedPin(
	IBaseFilter *pFilter,   // Pointer to the filter.
	PIN_DIRECTION PinDir,   // Direction of the pin to find.
	IPin **ppPin);           // Receives a pointer to the pin.


bool BindFilter(int deviceId, IBaseFilter **pFilter);

bool GetTheRatioValueOk(std::vector<int>& wPixs, std::vector<int>& hPixs, int w, int h);

HRESULT GetCaptureRatio(IAMStreamConfig* pVSC, std::vector<int>& wPix, std::vector<int>& hPix);

void   ShowPinConfig(HWND * wnd, int& width, int& height);

void SetTheRatio(IAMStreamConfig* pVSC, std::vector<int> wPixs, std::vector<int> hPixs, int& outW, int& outH);

//自动调整分辨率和帧率
void AutoChangePinPixAndFrame( int& width, int& height);

HRESULT RenderFileToVMR9(IGraphBuilder *pGB, 
	IBaseFilter *pRenderer, BOOL bRenderAudio = TRUE);


#endif

