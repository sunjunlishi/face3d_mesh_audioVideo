//------------------------------------------------------------------------------
// File: Text.cpp
//
// Desc: DirectShow sample code - a simple text-over-video app
//       Using the DirectX 9 Video Mixing Renderer, a generated bitmap
//       containing app-specified text is blended with a running video.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include <dshow.h>
#include <stdio.h>
//#include <tchar.h>
#include "vmrutil.h"

#include "videoMix.h"
#include "bitmap.h"
#include <opencv2\imgcodecs.hpp>
#include <opencv2\core.hpp>




// An application can advertise the existence of its filter graph
// by registering the graph with a global Running Object Table (ROT).
// The GraphEdit application can detect and remotely view the running
// filter graph, allowing you to 'spy' on the graph with GraphEdit.
//
// To enable registration in this sample, define REGISTER_FILTERGRAPH.
//
#define REGISTER_FILTERGRAPH

//
// Global data
//
HWND      ghApp=0;
HMENU     ghMenu=0;
HINSTANCE ghInst=0;
TCHAR     g_szFileName[MAX_PATH]={0};
DWORD     g_dwGraphRegister=0;
RECT      g_rcDest={0};

// DirectShow interfaces
IGraphBuilder *pGB = NULL;

IMediaEventEx *pME = NULL;
IMediaSeeking *pMS = NULL;
IVMRWindowlessControl9 *pWC = NULL;


HRESULT PlayMovieInWindow(HWND wnd)
{
	ghApp = wnd;

    HRESULT hr;

    // Clear open dialog remnants before calling RenderFile()
    UpdateWindow(ghApp);

    // Get the interface for DirectShow's GraphBuilder
    JIF(CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                         IID_IGraphBuilder, (void **)&pGB));

    if(SUCCEEDED(hr))
    {
		IBaseFilter* pVmr = NULL;

        // Create the Video Mixing Renderer and add it to the graph
        JIF(InitializeWindowlessVMR(&pVmr));


        // Render the file programmatically to use the VMR9 as renderer.
        // We pass a pointer to the VMR9 so that it will be used as the 
        // video renderer.  Pass TRUE to create an audio renderer also.
        if (FAILED(hr = RenderFileToVMR9(pGB, pVmr, FALSE)))
            return hr;

        // QueryInterface for DirectShow interfaces
        JIF(pGB->QueryInterface(IID_IMediaControl, (void **)&gPMC));
        JIF(pGB->QueryInterface(IID_IMediaEventEx, (void **)&pME));
        JIF(pGB->QueryInterface(IID_IMediaSeeking, (void **)&pMS));

        // Is this an audio-only file (no video component)?
        if (CheckVideoVisibility())
        {
            JIF(InitVideoWindow(1, 1));
        }
        else
        {
            return E_FAIL;
        }

        // Have the graph signal event via window callbacks for performance
        JIF(pME->SetNotifyWindow((OAHWND)ghApp, WM_GRAPHNOTIFY, 0));



        // Complete the window setup
        ShowWindow(ghApp, SW_SHOWNORMAL);
        UpdateWindow(ghApp);
        SetForegroundWindow(ghApp);
        SetFocus(ghApp);

#ifdef REGISTER_FILTERGRAPH
        if (FAILED(AddGraphToRot(pGB, &g_dwGraphRegister)))
        {
            g_dwGraphRegister = 0;
        }
#endif

        // Run the graph to play the media file
        JIF(gPMC->Run());

        // Start the text update timer
       StartTimer();
    }


	if (!FAILED(hr))
	{
		int w,  h;
		AutoChangePinPixAndFrame(w, h);
	}
    return hr;
}


HRESULT InitVideoWindow(int nMultiplier, int nDivider)
{
    LONG lHeight, lWidth;
    HRESULT hr = S_OK;

    if (!pWC)
        return S_OK;

    // Read the default video size
    hr = pWC->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
    if (hr == E_NOINTERFACE)
        return S_OK;

    // Account for requests of normal, half, or double size
    lWidth  = lWidth  * nMultiplier / nDivider;
    lHeight = lHeight * nMultiplier / nDivider;

    int nTitleHeight  = GetSystemMetrics(SM_CYCAPTION);
    int nBorderWidth  = GetSystemMetrics(SM_CXBORDER);
    int nBorderHeight = GetSystemMetrics(SM_CYBORDER);

    // Account for size of title bar and borders for exact match
    // of window client area to default video size
    SetWindowPos(ghApp, NULL, 0, 0, lWidth + 2*nBorderWidth,
                 lHeight + nTitleHeight + 2*nBorderHeight,
                 SWP_NOMOVE | SWP_NOOWNERZORDER);

    GetClientRect(ghApp, &g_rcDest);
    hr = pWC->SetVideoPosition(NULL, &g_rcDest);
	if (FAILED(hr))
	{

	}
       

    return hr;
}

void FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

HRESULT mix_getLpdata(unsigned char ** data)
{
	return pWC->GetCurrentImage(data);
}


void DoShowClearinfo()
{
	gShowinfos.clear();
}

void DoShowAddInfo(S_SHOW sshow)
{
	gShowinfos.push_back(sshow);
}


void DoShowFaceRect()
{
	BlendText(ghApp);
}

void mix_startCap(CVdoFrameHandler * FrameHandler)
{
	gFrameHandle = FrameHandler;
	return;
}

void MoveVideoWindow(void)
{
    HRESULT hr;

    // Track the movement of the container window and resize as needed
    if(pWC)
    {
        GetClientRect(ghApp, &g_rcDest);

        hr = pWC->SetVideoPosition(NULL, &g_rcDest);
		if (FAILED(hr))
		{

		}
	}
}


BOOL CheckVideoVisibility(void)
{
    HRESULT hr;
    LONG lWidth=0, lHeight=0;

    //
    // Because this sample explicitly loads the VMR9 into the filter graph
    // before rendering a file, the IVMRWindowlessControl interface will exist
    // for all properly rendered files.  As a result, we can't depend on the
    // existence of the pWC interface to determine whether the media file has
    // a video component.  Instead, check the width and height values.
    //
    if (!pWC)
    {
        // Audio-only files have no video interfaces.  This might also
        // be a file whose video component uses an unknown video codec.
        return FALSE;
    }

    hr = pWC->GetNativeVideoSize(&lWidth, &lHeight, 0, 0);
    if (FAILED(hr))
    {
        // If this video is encoded with an unsupported codec,
        // we won't see any video, although the audio will work if it is
        // of a supported format.
        return FALSE;
    }

    // If this is an audio-only clip, width and height will be 0.
    if ((lWidth == 0) && (lHeight == 0))
        return FALSE;

    // Assume that this media file contains a video component
    return TRUE;
}





void CloseClip()
{
    HRESULT hr;

    // Stop the text update timer
    StopTimer();

    // Stop media playback
    if(gPMC)
        hr = gPMC->Stop();

    // Free DirectShow interfaces
    CloseInterfaces();

    // Clear file name to allow selection of new file with open dialog
    g_szFileName[0] = L'\0';

    // Reset the player window
    RECT rect;
    GetClientRect(ghApp, &rect);
    InvalidateRect(ghApp, &rect, TRUE);

}


void CloseInterfaces(void)
{
#ifdef REGISTER_FILTERGRAPH
    if (g_dwGraphRegister)
    {
        RemoveGraphFromRot(g_dwGraphRegister);
        g_dwGraphRegister = 0;
    }
#endif

    // Release and zero DirectShow interfaces
    SAFE_RELEASE(pME);
    SAFE_RELEASE(pMS);
    SAFE_RELEASE(gPMC);
    SAFE_RELEASE(pWC);
    SAFE_RELEASE(pBMP);
    SAFE_RELEASE(pGB);
}


HRESULT HandleGraphEvent(void)
{
    LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    // Make sure that we don't access the media event interface
    // after it has already been released.
    if (!pME)
        return S_OK;

    // Process all queued events
    while(SUCCEEDED(pME->GetEvent(&evCode, (LONG_PTR *) &evParam1,
                   (LONG_PTR *) &evParam2, 0)))
    {
        // Free memory associated with callback, since we're not using it
        hr = pME->FreeEventParams(evCode, evParam1, evParam2);

        // If this is the end of the clip, reset to beginning
        if(EC_COMPLETE == evCode)
        {
            LONGLONG pos=0;

            // Reset to first frame of movie
            hr = pMS->SetPositions(&pos, AM_SEEKING_AbsolutePositioning ,
                                   NULL, AM_SEEKING_NoPositioning);
            if (FAILED(hr))
            {
                // If seeking failed, just stop and restart playback
                StopTimer();
                hr = gPMC->Stop();

                // Wait for the state to propagate to all filters
                OAFilterState fs;
                hr = gPMC->GetState(500, &fs);

                hr = gPMC->Run();
                StartTimer();
            }
        }
    }

    return hr;
}


#ifdef REGISTER_FILTERGRAPH

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) 
    {
        return E_FAIL;
    }

    WCHAR wsz[128];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
              GetCurrentProcessId());

    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) 
    {
        // Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
        // to the object.  Using this flag will cause the object to remain
        // registered until it is explicitly revoked with the Revoke() method.
        //
        // Not using this flag means that if GraphEdit remotely connects
        // to this graph and then GraphEdit exits, this object registration 
        // will be deleted, causing future attempts by GraphEdit to fail until
        // this application is restarted or until the graph is registered again.
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
                            pMoniker, pdwRegister);
        pMoniker->Release();
    }

    pROT->Release();
    return hr;
}

void RemoveGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

#endif

void DoResetShow(HWND wnd, CVdoFrameHandler * FrameHandler)
{
	if (gPMC)
	{
		gPMC->Stop();
		ghApp = wnd;
		gFrameHandle = FrameHandler;
		HRESULT hr = pWC->SetVideoClippingWindow(ghApp);
		hr = pWC->SetBorderColor(RGB(0, 0, 0));

		gPMC->Run();
	}
	
}



HRESULT InitializeWindowlessVMR(IBaseFilter **ppVmr9)
{
    IBaseFilter* pVmr = NULL;

    if (!ppVmr9)
        return E_POINTER;
    *ppVmr9 = NULL;

    // Create the VMR and add it to the filter graph.
    HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL,
                     CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);
    if (SUCCEEDED(hr)) 
    {
        hr = pGB->AddFilter(pVmr, L"Video Mixing Renderer 9");
        if (SUCCEEDED(hr)) 
        {
            // Set the rendering mode and number of streams
			IVMRFilterConfig9 *pConfig;

            JIF(pVmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&pConfig));
            JIF(pConfig->SetRenderingMode(VMR9Mode_Windowless));

            hr = pVmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pWC);
            if( SUCCEEDED(hr)) 
            {
                hr = pWC->SetVideoClippingWindow(ghApp);
                hr = pWC->SetBorderColor(RGB(0,0,0));
            }

#ifndef BILINEAR_FILTERING
            // Request point filtering (instead of bilinear filtering)
            // to improve the text quality.  In general, if you are 
            // not scaling the app image, you should use point filtering.
            // This is very important if you are doing source color keying.
            IVMRMixerControl9 *pMix;

            hr = pVmr->QueryInterface(IID_IVMRMixerControl9, (void**)&pMix);
            if( SUCCEEDED(hr)) 
            {
                DWORD dwPrefs=0;
                hr = pMix->GetMixingPrefs(&dwPrefs);

                if (SUCCEEDED(hr))
                {
                    dwPrefs |= MixerPref_PointFiltering;
                    dwPrefs &= ~(MixerPref_BiLinearFiltering);

                    hr = pMix->SetMixingPrefs(dwPrefs);
                }
                pMix->Release();
            }
#endif

            // Get alpha-blended bitmap interface
            hr = pVmr->QueryInterface(IID_IVMRMixerBitmap9, (void**)&pBMP);
        }
		else
		{

		}

        // Don't release the pVmr interface because we are copying it into
        // the caller's ppVmr9 pointer
        *ppVmr9 = pVmr;
    }
	else
	{
	}

    return hr;
}


void OnPaint(HWND hwnd) 
{
    HRESULT hr;
    PAINTSTRUCT ps; 
    HDC         hdc; 
    RECT        rcClient; 

    GetClientRect(hwnd, &rcClient); 
    hdc = BeginPaint(hwnd, &ps); 

    if(pWC) 
    { 
        hr = pWC->RepaintVideo(hwnd, hdc);  
    } 
    else  // No video image. Just paint the whole client area. 
    { 
        FillRect(hdc, &rcClient, (HBRUSH)(COLOR_BTNFACE + 1)); 
    } 

    EndPaint(hwnd, &ps); 
} 


