// CaptureImage.h: interface for the CCaptureImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTUREIMAGE_H__D0CFF37F_7B95_431C_967B_D118D33D7FAA__INCLUDED_)
#define AFX_CAPTUREIMAGE_H__D0CFF37F_7B95_431C_967B_D118D33D7FAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CaptureVideo.h"

class CCaptureImage : public CVdoFrameHandler  
{
public:
	CCaptureImage();
	virtual ~CCaptureImage();
public:
	void VdoFrameData(long lw,long lh,BYTE * pBuffer, long lBufferSize);
	void StartCapture();
	HRESULT OpenDevice(int index, HWND wnd);
	void StartCapture(CVdoFrameHandler * frameHandler);
	void StopCapture();
	void EnumDevlist(HWND list);
	CImage* GetImage() { return &m_image; }
public:
	CCaptureVideo m_CaptureVideo;
	CImage m_image;
	void	SetLevel(int nLevel);
};

#endif // !defined(AFX_CAPTUREIMAGE_H__D0CFF37F_7B95_431C_967B_D118D33D7FAA__INCLUDED_)
