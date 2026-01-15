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

#include "vmrutil.h"

#include <streams.h>    // Active Movie (includes windows.h)
#include <initguid.h>    // declares DEFINE_GUID to declare an EXTERN_C const.
#include "grabber.h"
#include <stdio.h>
#include "bitmap.h"

#include <opencv2\core.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\imgcodecs.hpp>

#include <CaptureImage\VdoFrameHandler.h>

CVdoFrameHandler * gFrameHandle;
IMediaControl *gPMC = NULL;
IAMStreamConfig *gPVSC;
//----------------------------------------------------------------------------
//  VerifyVMR9
// 
//  Verifies that VMR9 COM objects exist on the system and that the VMR9
//  can be instantiated.
//
//  Returns: FALSE if the VMR9 can't be created
//----------------------------------------------------------------------------

BOOL VerifyVMR9(void)
{
    HRESULT hr;

    // Verify that the VMR exists on this system
    IBaseFilter* pBF = NULL;
    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL,
                          CLSCTX_INPROC,
                          IID_IBaseFilter,
                          (LPVOID *)&pBF);
    if(SUCCEEDED(hr))
    {
        pBF->Release();
        return TRUE;
    }
    else
    {
        MessageBox(NULL, 
            TEXT("This application requires the Video Mixing Renderer, which is present\r\n")
            TEXT("only on DirectX 9 systems with hardware video acceleration enabled.\r\n\r\n")

            TEXT("The Video Mixing Renderer (VMR9) is not enabled when viewing a \r\n")
            TEXT("remote Windows XP machine through a Remote Desktop session.\r\n")
            TEXT("You can run VMR-enabled applications only on your local machine.\r\n\r\n")

            TEXT("To verify that hardware acceleration is enabled on a Windows XP\r\n")
            TEXT("system, follow these steps:\r\n")
            TEXT("-----------------------------------------------------------------------\r\n")
            TEXT(" - Open 'Display Properties' in the Control Panel\r\n")
            TEXT(" - Click the 'Settings' tab\r\n")
            TEXT(" - Click on the 'Advanced' button at the bottom of the page\r\n")
            TEXT(" - Click on the 'Troubleshooting' tab in the window that appears\r\n")
            TEXT(" - Verify that the 'Hardware Acceleration' slider is at the rightmost position\r\n")

            TEXT("\r\nThis sample will now exit."),

            TEXT("Video Mixing Renderer (VMR9) capabilities are required"), MB_OK);

        return FALSE;
    }
}




HRESULT GetUnconnectedPin(
    IBaseFilter *pFilter,   // Pointer to the filter.
    PIN_DIRECTION PinDir,   // Direction of the pin to find.
    IPin **ppPin)           // Receives a pointer to the pin.
{
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;

    if (!ppPin)
        return E_POINTER;
    *ppPin = 0;

    // Get a pin enumerator
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
        return hr;

    // Look for the first unconnected pin
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;

        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;

            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }

    // Release the enumerator
    pEnum->Release();

    // Did not find a matching pin
    return E_FAIL;
}

bool GetTheRatioValueOk(std::vector<int>& wPixs, std::vector<int>& hPixs, int w, int h)
{
	for (int i = 0; i<wPixs.size(); ++i)
	{
		if (wPixs[i] == w && hPixs[i] == h)
		{
			return true;
		}
	}
	return false;
}

HRESULT GetCaptureRatio(IAMStreamConfig* pVSC, std::vector<int>& wPix, std::vector<int>& hPix)
{

	int nCount = 0;
	int nSize = 0;
	HRESULT hr = pVSC->GetNumberOfCapabilities(&nCount, &nSize);

	// 判断是否为视频信息
	if (sizeof(VIDEO_STREAM_CONFIG_CAPS) == nSize) {
		for (int i = 0; i<nCount; i++) 
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE* pmmt;
			hr = pVSC->GetStreamCaps(i, &pmmt, reinterpret_cast<unsigned char*>(&scc));
			if (pmmt->formattype == FORMAT_VideoInfo) {

				VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(pmmt->pbFormat);
				int nFrame = pvih->AvgTimePerFrame; // 得到采集的帧率
				int nwid = pvih->bmiHeader.biWidth; // 得到采集的宽
				int nhei = pvih->bmiHeader.biHeight; //　得到采集的高
				wPix.push_back(nwid);
				hPix.push_back(nhei);
			}
		}
	}
	return 1;
}


void AutoChangePinPixAndFrame(int& width, int& height)
{
	HRESULT hr;
	if (gPVSC)
	{
		gPMC->Stop();
		std::vector<int> wPixs;
		std::vector<int> hPixs;
		//获得分辨率
		GetCaptureRatio(gPVSC, wPixs, hPixs);

		//设置分辨率
		SetTheRatio(gPVSC, wPixs, hPixs, width, height);

		//mCB.lWidth = width;
		//mCB.lHeight = height;

		gPMC->Run();

	}
}

void SetTheRatio(IAMStreamConfig* pVSC, std::vector<int> wPixs, std::vector<int> hPixs,int& outW,int& outH)
{
	AM_MEDIA_TYPE * mmt = NULL;
	pVSC->GetFormat(&mmt);    //取得默认参数
	VIDEOINFOHEADER * pvih = (VIDEOINFOHEADER*)mmt->pbFormat;
	if (GetTheRatioValueOk(wPixs, hPixs, 960, 720))
	{
		//960,720的分辨率优先
		pvih->bmiHeader.biWidth = 960;
		pvih->bmiHeader.biHeight = 720;
	}
	else if (GetTheRatioValueOk(wPixs, hPixs, 800, 600))
	{
		//960,720的分辨率优先
		pvih->bmiHeader.biWidth = 800;
		pvih->bmiHeader.biHeight = 600;
	}
	else if (GetTheRatioValueOk(wPixs, hPixs, 1280, 800))
	{
		//960,720的分辨率优先
		pvih->bmiHeader.biWidth = 1280;
		pvih->bmiHeader.biHeight = 800;
	}
	else if (GetTheRatioValueOk(wPixs, hPixs, 1280, 720))
	{
		//960,720的分辨率优先
		pvih->bmiHeader.biWidth = 1280;
		pvih->bmiHeader.biHeight = 720;
	}
	else //if(GetTheRatioValueOk(wPixs,hPixs,640,480))
	{
		//960,720的分辨率优先
		pvih->bmiHeader.biWidth = 640;
		pvih->bmiHeader.biHeight = 480;
	}
	outW = pvih->bmiHeader.biWidth;
	outH = pvih->bmiHeader.biHeight;

	SetVideoSize(outW,outH);

	pvih->AvgTimePerFrame = 500000;
	mmt->pbFormat = (unsigned char *)pvih;
	HRESULT hr = pVSC->SetFormat(mmt);   //重新设置参数
	int k = 0;
}

bool BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
		return false;
	ICreateDevEnum * pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		return false;
	}
	IEnumMoniker * pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEm, 0);
	if (hr != NOERROR)
	{
		return false;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while (hr = pEm->Next(1, &pM, &cFetched), hr == S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				if (index == deviceId)
				{
					pM->BindToObject(0, 0, IID_IBaseFilter, (void**)pFilter);
				}
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
		index++;
	}
	return true;
}

cv::Mat matYuv;
cv::Mat matBGR;
 HRESULT  __stdcall CallBackSample (
	 IMediaSample * pSample)
{
	
	int dataLen = pSample->GetActualDataLength();
	BYTE *pSourceBuffer;
	pSample->GetPointer(&pSourceBuffer);

	if (matYuv.empty())
	{
		matYuv = cv::Mat(gVideoW, gVideoH,CV_8UC2);
	}
	memcpy(matYuv.data, pSourceBuffer, gVideoW * gVideoH * 2);
	cvtColor(matYuv, matBGR, CV_YUV2BGR_YUY2);

	char szBuf[128];
	sprintf(szBuf, "%d", dataLen);
	gFrameHandle->VdoFrameData(gVideoW, gVideoH, matBGR.data, gVideoW * gVideoH);

	 //if(dataLen > 0 )
	// pSample->GetPointer(&pSourceBuffer);
	 return S_OK;
}

 IGrabberSample*   g_pGrabber;
 ICaptureGraphBuilder2* g_pCapBuild;
HRESULT RenderFileToVMR9(IGraphBuilder *pGB,
                         IBaseFilter *pRenderer, BOOL bRenderAudio)
{
    HRESULT hr=S_OK;
	IPin * pOutputPin = NULL;
	IBaseFilter* pSource;
	IBaseFilter* pAudioRenderer = NULL;
	IFilterGraph2 * pFG;


	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&g_pCapBuild);

	if (FAILED(hr)) 	return hr;

	g_pCapBuild->SetFiltergraph(pGB);


		if (!BindFilter(0, &pSource))
			return S_FALSE;

		//Generic USB Controller Interface (AVS)
		hr = pGB->AddFilter(pSource, L"USB Camera");
        if (FAILED(hr ))
        {
            return hr;
        }

		hr = g_pCapBuild->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pSource,
			IID_IAMStreamConfig, (void **)&gPVSC);

		if (hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			MessageBox(NULL, "aa", TEXT("Error %x: Cannot find VCapture:IAMStreamConfig"), 0);
			return hr;
		}

        // Get the interface for the first unconnected output pin
        JIF(GetUnconnectedPin(pSource, PINDIR_OUTPUT, &pOutputPin));

		hr = CoCreateInstance(CLSID_GrabberSample, NULL, CLSCTX_INPROC_SERVER, IID_IGrabberSample, (void**)&g_pGrabber);
		if (FAILED(hr))
		{
			return hr;
		}
		IBaseFilter * pGrabBase = NULL;
		hr = g_pGrabber->QueryInterface(IID_IBaseFilter,(void**)&pGrabBase);
		if (FAILED(hr))
		{
			return hr;
		}
		hr = pGB->AddFilter(pGrabBase, L"Grabber");
		if (FAILED(hr))
		{
			return hr;
		}
		g_pGrabber->SetCallback(CallBackSample);

		
    // Render audio if requested (defaults to TRUE)
    if (bRenderAudio)
    {
        // Because we will be rendering with the RENDERTOEXISTINGRENDERERS flag,
        // we need to create an audio renderer and add it to the graph.  
        // Create an instance of the DirectSound renderer (for each media file).
        //
        // Note that if the system has no sound card (or if the card is disabled),
        // then creating the DirectShow renderer will fail.  In that case,
        // handle the failure quietly.
        if (SUCCEEDED(CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC_SERVER, 
                                       IID_IBaseFilter, (void **)&pAudioRenderer)))
        {
            // The audio renderer was successfully created, so add it to the graph
            JIF(pGB->AddFilter(pAudioRenderer, L"Audio Renderer"));
        }
    }

    //// Get an IFilterGraph2 interface to assist in building the
    //// multifile graph with the non-default VMR9 renderer
    JIF(pGB->QueryInterface(IID_IFilterGraph2, (void **)&pFG));

    //// Render the output pin, using the VMR9 as the specified renderer.  This is 
    //// necessary in case the GraphBuilder needs to insert a Color Space convertor,
    //// or if multiple filters insist on using multiple allocators.
    //// The audio renderer will also be used, if the media file contains audio.
    JIF(pFG->RenderEx(pOutputPin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, NULL));

    // If this media file does not contain an audio stream, then the 
    // audio renderer that we created will be unconnected.  If left in the 
    // graph, it could interfere with rate changes and timing.
    // Therefore, if the audio renderer is unconnected, remove it from the graph.
    if (pAudioRenderer != NULL)
    {
        IPin *pUnconnectedPin=0;

        // Is the audio renderer's input pin connected?
        HRESULT hrPin = GetUnconnectedPin(pAudioRenderer, PINDIR_INPUT, &pUnconnectedPin);

        // If there is an unconnected pin, then remove the unused filter
        if (SUCCEEDED(hrPin) && (pUnconnectedPin != NULL))
        {
            // Release the returned IPin interface
            pUnconnectedPin->Release();

            // Remove the audio renderer from the graph
            hrPin = pGB->RemoveFilter(pAudioRenderer);
        }
    }

    return hr;
}


void   ShowPinConfig(HWND * wnd, int& width, int& height)
{
	HRESULT hr;

	if (gPVSC)
	{
		gPMC->Stop();
		ISpecifyPropertyPages *pSpec = NULL;
		CAUUID cauuid;
		hr = gPVSC->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pSpec);

		if (hr == S_OK)
		{
			hr = pSpec->GetPages(&cauuid);
			hr = OleCreatePropertyFrame(*wnd, 30, 30, NULL, 1,
				(IUnknown **)&gPVSC, cauuid.cElems,
				(GUID *)cauuid.pElems, 0, 0, NULL);
			AM_MEDIA_TYPE *pmt;
			// get format being used NOW
			hr = gPVSC->GetFormat(&pmt);
			// DV capture does not use a VIDEOINFOHEADER
			if (hr == NOERROR)
			{
				if (pmt->formattype == FORMAT_VideoInfo)
				{
					//ResizeVideoWindow();
				}
				//DeleteMediaType(pmt);
			}

			//分辨率更改要通知其余进行 设置
			VIDEOINFOHEADER * pvih = (VIDEOINFOHEADER*)pmt->pbFormat;
			SetVideoSize(pvih->bmiHeader.biWidth, pvih->bmiHeader.biHeight);
			CoTaskMemFree(cauuid.pElems);
			pSpec->Release();
			gPMC->Run();

		}
	}

}


