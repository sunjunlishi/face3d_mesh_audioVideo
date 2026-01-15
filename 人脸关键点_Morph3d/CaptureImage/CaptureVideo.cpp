// CaptureVideo.cpp: implementation of the CCaptureVideo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "USBVideo.h"
#include "CaptureVideo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSampleGrabberCB mCB;
CCaptureVideo::CCaptureVideo():
m_nLevel(0)
{
	if(FAILED(CoInitialize(NULL)))
	{
		AfxMessageBox("CCaptureVideo CoInitialize Failed!\r\n"); 
		return;
	}
	m_hWnd = NULL;
	m_pVW = NULL;
	m_pMC = NULL;
	m_pGB = NULL;
	m_pBF = NULL; 
	m_pGrabber = NULL; 
	m_pCapture = NULL; 
}
CCaptureVideo::~CCaptureVideo()
{
	if(m_pMC)m_pMC->StopWhenReady();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW); 
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pGrabber); 
	SAFE_RELEASE(m_pCapture);
	CoUninitialize() ; 
}

/************************************************************************/
/* 枚举视频扑捉设备                                                     */
/************************************************************************/
int CCaptureVideo::EnumDevices(HWND hList)
{
	if (!hList)
		return -1;
	int id = 0;
	ICreateDevEnum *pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)return -1;
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR)return -1;
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK) {
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) {
			VARIANT var;
			var.vt = VT_BSTR;
			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR) {
				//TCHAR str[2048]; 
				//id++;
				//WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, str, 2048, NULL, NULL);
				::SendMessage(hList, CB_ADDSTRING, 0,(LPARAM)var.bstrVal);
				SysFreeString(var.bstrVal);
			}
			pBag->Release();
		}
		pM->Release();
	}
	return id;
}

int CCaptureVideo::GetDeviceCount()                                                                     
{                                                                                                
	int nCount = 0;                                                                               
	CoInitialize(NULL);                                                                         

	// enumerate all video capture devices                                                        
	CComPtr<ICreateDevEnum> pCreateDevEnum;                                                      
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,            
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);                

	CComPtr<IEnumMoniker> pEm;                                                                   
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,                   
		&pEm, 0);                                                                                
	if (hr != NOERROR)                                                                           
	{                                                                                            
		return nCount;                                                                            
	}                                                                                            

	pEm->Reset();                                                                                
	ULONG cFetched;                                                                              
	IMoniker *pM;                                                                                
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)                                           
	{                                                                                            
		nCount++;                                                                                 
	}                                                                                            

	pCreateDevEnum = NULL;                                                                       
	pEm = NULL;                                                                                  
	return nCount;                                                                                
}                                                                                                

CString CCaptureVideo::GetDeviceName(int nIndex)                              
{                                                                                                
	int nCount = 0;  
	CString strDeviceName;
	CoInitialize(NULL);                                                                         

	// enumerate all video capture devices                                                        
	CComPtr<ICreateDevEnum> pCreateDevEnum;                                                      
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,            
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);                

	CComPtr<IEnumMoniker> pEm;                                                                   
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,                   
		&pEm, 0);                                                                                
	if (hr != NOERROR) 
		return strDeviceName;                                                                 


	pEm->Reset();                                                                                
	ULONG cFetched;                                                                              
	IMoniker *pM;                                                                                
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)                                           
	{                                                                                            
		if (nCount == nIndex)                                                                     
		{                                                                                        
			IPropertyBag* pBag = 0;                                                                
			hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);                      
			if(SUCCEEDED(hr))                                                                    
			{                                                                                    
				VARIANT var;                                                                     
				var.vt = VT_BSTR;                                                                
				hr = pBag->Read(L"FriendlyName", &var, NULL); //还有其他属性,像描述信息等等...   
				if(hr == NOERROR)                                                                
				{                                                                                
					//获取设备名称                                                               
					strDeviceName = var.bstrVal;    
					SysFreeString(var.bstrVal);                                                  
				}                                                                                
				pBag->Release();                                                                 
			}                                                                                    
			pM->Release();                                                                       

			break;                                                                               
		}                                                                                        
		nCount++;                                                                                 
	}                                                                                            

	pCreateDevEnum = NULL;                                                                       
	pEm = NULL;                                                                                  

	return strDeviceName;                                                                                    
}

/************************************************************************/
/* 关闭视频设备                                                         */
/************************************************************************/
HRESULT CCaptureVideo::CloseDevices()
{
	if(m_pMC)m_pMC->StopWhenReady();
	if(m_pVW){
		m_pVW->put_Visible(OAFALSE);
		m_pVW->put_Owner(NULL);
	}
	SAFE_RELEASE(m_pMC);
	SAFE_RELEASE(m_pVW); 
	SAFE_RELEASE(m_pGB);
	SAFE_RELEASE(m_pBF);
	SAFE_RELEASE(m_pGrabber); 
	SAFE_RELEASE(m_pCapture);
	return S_OK ; 
}

//////////////////////////////////////////////////////////////////////////
void	CCaptureVideo::SetCaptureFormat(ICaptureGraphBuilder2* pCapture,IBaseFilter*   pBF,int nWidth,int nHeight,BOOL bMax)
{
    HRESULT hr = S_OK;
	int nMaxWidth = 640;
	int nMaxHeight = 480;
    IAMStreamConfig *pConfig = NULL;

    if(pCapture == NULL)  // no CaptureGraphBuilder initialised
    {  
		//DebugToLog(AGLOG_LEVEL_FATAL,("无效的对象"));
		return;
	}

    hr = pCapture->FindInterface(
        &PIN_CATEGORY_CAPTURE, // Preview pin.
        0,    // Any media type.
        pBF, // Pointer to the capture filter.
        IID_IAMStreamConfig, (void**)&pConfig);

    if(!SUCCEEDED(hr))
	{
		//DebugToLog(AGLOG_LEVEL_FATAL,("接口查找失败,错误描述:%s"),
	//		PrintError(hr));
        return;
	}

    int iCount = 0, iSize = 0;
    hr = pConfig->GetNumberOfCapabilities(&iCount, &iSize);

    // Check the size to make sure we pass in the correct structure.
    if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS)) 
	{
		BYTE *pSCC = NULL;
        // Use the video capabilities structure.
        for (int iFormat = 0; iFormat < iCount; iFormat++)
        {
			AM_MEDIA_TYPE *pmt;
			pSCC = new BYTE[iSize];
			if (pSCC == NULL)
			{
				AfxMessageBox(TEXT("内存不足"));
				exit(-2);
			}

			// Get the first format.
			hr = pConfig->GetStreamCaps(iFormat, &pmt, pSCC);
			if (SUCCEEDED(hr))
			{
				/* Examine the format, and possibly use it. */
				if (pmt->formattype == FORMAT_VideoInfo) 
				{
					// Check the buffer size.
					if (pmt->cbFormat >= sizeof(VIDEOINFOHEADER))
					{
						VIDEOINFOHEADER *pVih =  reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
						BITMAPINFOHEADER *bmiHeader = &pVih->bmiHeader;
						hr = pConfig->SetFormat(pmt);
						bmiHeader->biWidth = 640;
						bmiHeader->biHeight = 480;
						break;
						//DebugToLog(AGLOG_LEVEL_DEBUG,TEXT("[%d][%d][%d]"),
						//	bmiHeader->biWidth,bmiHeader->biHeight,bmiHeader->biBitCount);
						if(bMax)
						{

							/* Access VIDEOINFOHEADER members through pVih. */
							if( (bmiHeader->biWidth >= nMaxWidth) &&
								(bmiHeader->biWidth <= 640) &&
								(bmiHeader->biWidth * 3 == bmiHeader->biHeight * 4 ||
								bmiHeader->biWidth * 9 == bmiHeader->biHeight * 16)&&
								bmiHeader->biBitCount == 24)
							{
								hr = pConfig->SetFormat(pmt);
								if(SUCCEEDED(hr))
								{
									nMaxWidth = bmiHeader->biWidth ; 
									nMaxHeight =  bmiHeader->biHeight;
									//DebugToLog(AGLOG_LEVEL_DEBUG,TEXT("[[%d%d]]"),
									//	nMaxWidth,nMaxHeight);
								}
								else
								{
									//DebugToLog(AGLOG_LEVEL_FATAL,TEXT("设置格式失败,错误描述:%s"),
										//PrintError(hr));
								}
								
							}
						}
						else
						{
							if( bmiHeader->biWidth <= nMaxWidth && 
								(bmiHeader->biWidth >= 1140) &&
								(bmiHeader->biWidth * 3 == bmiHeader->biHeight * 4 ||
								bmiHeader->biWidth * 9 == bmiHeader->biHeight * 16)&&
								bmiHeader->biBitCount == 24)
							{
								hr = pConfig->SetFormat(pmt);
								if(SUCCEEDED(hr))
								{
									nMaxWidth = bmiHeader->biWidth ; 
									nMaxHeight =  bmiHeader->biHeight;
									//DebugToLog(AGLOG_LEVEL_DEBUG,TEXT("[[%d%d]]"),
									//	nMaxWidth,nMaxHeight);
								}
								else
								{
									//DebugToLog(AGLOG_LEVEL_FATAL,TEXT("设置格式失败,错误描述:%s"),
									//	PrintError(hr));
								}
							}
						}
					}
				}
			}
			else
			{
				AfxMessageBox(TEXT("获取格式失败"));
			}
			delete [] pSCC;
			FreeMediaType(*pmt);

        }
    }
}
//////////////////////////////////////////////////////////////////////////


/************************************************************************/
/* 打开视频设备                                                         */
/************************************************************************/
HRESULT CCaptureVideo::OpenDevices(int iDeviceID, HWND hWnd, int nWidth, int nHeight)
{
	HRESULT hr;
	hr = InitCaptureGraphBuilder();
	if (FAILED(hr)){
		AfxMessageBox("Failed to get video interfaces!");
		return hr;
	}
	if(!BindFilter(iDeviceID, &m_pBF))
	{
		//DebugToLog(AGLOG_LEVEL_FATAL, _T("BindFilter fail"));
		return S_FALSE;
	}
	hr = m_pGB->AddFilter(m_pBF, L"Capture Filter");
	hr = CoCreateInstance( CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_ISampleGrabber, (void**)&m_pGrabber );
	if(FAILED(hr))
	{
		AfxMessageBox("Fail to create SampleGrabber, maybe qedit.dll is not registered?");
		return hr;
	}
	///
	CComQIPtr< IBaseFilter, &IID_IBaseFilter > pGrabBase(m_pGrabber);
	//设置视频格式
	AM_MEDIA_TYPE mt; 
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24; 
	hr = m_pGrabber->SetMediaType(&mt);
	if(FAILED(hr))
	{
		AfxMessageBox("Fail to set media type!");
		return hr;
	}
	
	SetCaptureFormat(m_pCapture,m_pBF,2592,1944,true);
	hr = m_pGB->AddFilter( pGrabBase, L"Grabber" );
	if(FAILED(hr))
	{
		AfxMessageBox("Fail to put sample grabber in graph");
		return hr;
	}
	hr = m_pCapture->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	if( FAILED(hr))
	{
		//DebugToLog(AGLOG_LEVEL_FATAL,TEXT("RenderStream Failed [%x],错误描述:%s"),
		//	hr,PrintError(hr));
		hr = m_pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,m_pBF,pGrabBase,NULL);
	}
    if( FAILED(hr))
	{
		//DebugToLog(AGLOG_LEVEL_FATAL,TEXT("RenderStream Failed [%x],错误描述:%s"),
		//	hr,PrintError(hr));
		AfxMessageBox("Can't build the graph");
		return hr;
    }
	//////////////////////////////////////////////////////////////////////////
	AM_MEDIA_TYPE	amTemp;
	hr =  m_pGrabber->GetConnectedMediaType(&amTemp);
	if(FAILED(hr))
	{
		{
			AfxMessageBox("Can't get the type");
			return hr;
		}
		return hr;
	}
	else
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) amTemp.pbFormat;
		nWidth =vih->bmiHeader.biWidth;
		nHeight = vih->bmiHeader.biHeight;
		int n3 =vih->bmiHeader.biSizeImage;
	}

	//////////////////////////////////////////////////////////////////////////
	//VIDEOINFOHEADER * vih = (VIDEOINFOHEADER*) mt.pbFormat;
	//mCB.lWidth = vih->bmiHeader.biWidth;
	//mCB.lHeight = vih->bmiHeader.biHeight;
	mCB.lWidth = nWidth;
	mCB.lHeight = nHeight;
	mCB.bGrabVideo = FALSE ; 
	mCB.frame_handler = NULL ; 
	hr = m_pGrabber->SetBufferSamples(FALSE);
	hr = m_pGrabber->SetOneShot(FALSE);
	hr = m_pGrabber->SetCallback(&mCB, 1);
	//设置视频捕捉窗口
	m_hWnd = hWnd ; 
	SetupVideoWindow();
	hr = m_pMC->Run();//开始视频捕捉
	if(FAILED(hr))
	{
		AfxMessageBox("Couldn't run the graph!");
		//DebugToLog(AGLOG_LEVEL_FATAL, _T("Couldn't run the graph! ErrorCode: %ld"), hr);
		return hr;
	}
	return S_OK;
}

BOOL CCaptureVideo::BindFilter(int deviceId, IBaseFilter **pFilter)
{
	if (deviceId < 0)
	{
		//DebugToLog(AGLOG_LEVEL_FATAL, _T("deviceID < 0"));
		return false;
	}
	CComPtr<ICreateDevEnum> pCreateDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	if (hr != NOERROR)
	{
		//DebugToLog(AGLOG_LEVEL_FATAL, _T("CoCreateInstance fail: %d"), hr);
		return false;
	}
	CComPtr<IEnumMoniker> pEm;
	hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,&pEm, 0);
	if (hr != NOERROR) 
	{
		//DebugToLog(AGLOG_LEVEL_FATAL, _T("CreateClassEnumerator fail: %d"), hr);
		return false;
	}
	pEm->Reset();
	ULONG cFetched;
	IMoniker *pM;
	int index = 0;
	while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK, index <= deviceId)
	{
		IPropertyBag *pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
		if(SUCCEEDED(hr)) 
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

HRESULT CCaptureVideo::InitCaptureGraphBuilder()
{
	HRESULT hr;
	// 创建IGraphBuilder接口
	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGB);
	// 创建ICaptureGraphBuilder2接口
	hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
	if (FAILED(hr))
		return hr;
	m_pCapture->SetFiltergraph(m_pGB);
	hr = m_pGB->QueryInterface(IID_IMediaControl, (void **)&m_pMC);
	if (FAILED(hr))
		return hr;
	hr = m_pGB->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
	if (FAILED(hr))
		return hr;
	return hr;
}

HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;
	hr = m_pVW->put_Owner((OAHWND)m_hWnd);
	if (FAILED(hr))
		return hr;
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;
	ResizeVideoWindow();
	hr = m_pVW->put_Visible(OATRUE);
	return hr;
}

/************************************************************************/
/* 让图像充满整个窗口                                                   */
/************************************************************************/
void CCaptureVideo::ResizeVideoWindow()
{
	if (m_pVW){
		
		CRect rc;
		::GetClientRect(m_hWnd,&rc);
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	} 
}

void CCaptureVideo::FreeMediaType(AM_MEDIA_TYPE& mt)
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

void CCaptureVideo::GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler *frame_handler)
{
	mCB.frame_handler = frame_handler ; 
	mCB.bGrabVideo = bGrabVideoFrames ; 
}

HRESULT CCaptureVideo::ClearVideoWindow()
{
	HRESULT hr;
	hr = m_pVW->put_Visible(OAFALSE);
	if (FAILED(hr))
		return hr;
	hr = m_pVW->put_Owner(NULL);
	return hr;
}