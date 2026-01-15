// CaptureVideo.h: interface for the CCaptureVideo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTUREVIDEO_H__BDEEEAAF_A7FC_4D27_A890_BC2C6DF7DEF0__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__BDEEEAAF_A7FC_4D27_A890_BC2C6DF7DEF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <windows.h>
#include <dshow.h>
#include <Qedit.h>
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
	if ( NULL != x ) \
	{ \
		x->Release(); \
		x = NULL; \
	}
#endif

class CVdoFrameHandler {
public:
	virtual void VdoFrameData(long lw,long lh,BYTE * pBuffer, long lBufferSize) = 0; 
};

class CSampleGrabberCB : public ISampleGrabberCB 
{
public:
	long	lWidth ; 
	long	lHeight ; 
	CVdoFrameHandler *  frame_handler ; 
	BOOL	bGrabVideo ; 
public:
	CSampleGrabberCB()
	{ 
		lWidth = 0 ; 
		lHeight = 0 ; 
		bGrabVideo = FALSE ; 
		frame_handler = NULL ; 
	} 
	STDMETHODIMP_(ULONG) AddRef() 
	{ 
		return 2; 
	}
	STDMETHODIMP_(ULONG) Release() 
	{ 
		return 1; 
	}
	STDMETHODIMP QueryInterface(REFIID riid, void ** ppv)
	{
		if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown )
		{ 
			*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
			return NOERROR;
		} 
		return E_NOINTERFACE;
	}
	STDMETHODIMP SampleCB( double SampleTime, IMediaSample * pSample )
	{
		return 0;
	}
	STDMETHODIMP BufferCB( double dblSampleTime, BYTE * pBuffer, long lBufferSize )
	{
		if (!pBuffer) 
			return E_POINTER;
		if(bGrabVideo && frame_handler) 
			frame_handler->VdoFrameData(lWidth,lHeight,pBuffer,lBufferSize); 
		return 0;
	}
};
class CCaptureVideo : public CWnd  
{
	friend class CSampleGrabberCB;
public:
	HRESULT CloseDevices();
	void GrabVideoFrames(BOOL bGrabVideoFrames, CVdoFrameHandler * frame_handler);
	HRESULT OpenDevices(int iDeviceID, HWND hWnd, int nWidth, int nHeight);
	int EnumDevices(HWND hList);
	int GetDeviceCount();
	CString GetDeviceName(int nIndex);
	CCaptureVideo();
	virtual ~CCaptureVideo();
	HRESULT ClearVideoWindow();
	void SetVideoWnd(HWND hWnd) { m_hWnd = hWnd; }
	HRESULT SetupVideoWindow();
	int				m_nLevel;
private:
    HWND     m_hWnd;
    IGraphBuilder *   m_pGB;
    ICaptureGraphBuilder2* m_pCapture;
    IBaseFilter*   m_pBF;
    IMediaControl*   m_pMC;
    IVideoWindow*   m_pVW;
    ISampleGrabber*   m_pGrabber;
protected:
	void ResizeVideoWindow();
	HRESULT InitCaptureGraphBuilder();
	BOOL BindFilter(int deviceId, IBaseFilter **pFilter);
    void FreeMediaType(AM_MEDIA_TYPE& mt);
	void SetCaptureFormat(ICaptureGraphBuilder2* pCapture,IBaseFilter*   pBF,int nWidth,int nHeight,BOOL bMax);

};

#endif // !defined(AFX_CAPTUREVIDEO_H__BDEEEAAF_A7FC_4D27_A890_BC2C6DF7DEF0__INCLUDED_)
