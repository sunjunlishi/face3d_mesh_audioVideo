#include "stdafx.h"
#include "Filter.h"
#include  <math.h>

CFilter::CFilter()
{
	m_hWnd = NULL;
	wBitsPerSample = 0;
}

CFilter::~CFilter()
{
	
 
	
	
}

void CFilter::Init(HWND hWnd,unsigned short wBits)
{
	m_hWnd = hWnd;
	wBitsPerSample = wBits;
	
	 
}

int CFilter::DrawWave(char* pBuf)
{
	if( m_hWnd == NULL || wBitsPerSample == 0 )
	{
		MessageBox(m_hWnd,"m_hWnd==NULL","error info",MB_OK);
		return 0;
	}
	//CDC m_MDC;
	HDC m_myHDC=::GetDC(m_hWnd);
	CRect m_rectSrc,m_rectDest;
	CPen pen,*oldpen;
	CBrush brush;
	CBitmap m_bitmap, *m_pOldBitmap;
	 
	::CreatePen(PS_SOLID,0,RGB(255,0,0));
	
	 brush.CreateSolidBrush(RGB(0,0,0));
 
 	CWnd::FromHandle(m_hWnd)->GetWindowRect(&m_rectSrc);
	CWnd::FromHandle(m_hWnd)->ScreenToClient(&m_rectSrc);
	CWnd::FromHandle(m_hWnd)->GetWindowRect(&m_rectDest);
	CWnd::FromHandle(m_hWnd)->ScreenToClient(&m_rectDest);
	 
	 ::CreateCompatibleBitmap(m_myHDC,m_rectSrc.Width(),m_rectSrc.Height());
	int x,y;    
	m_pOldBitmap =(CBitmap*)SelectObject(m_myHDC,m_bitmap);
	::Rectangle(m_myHDC,0,0,m_rectSrc.Width (),m_rectSrc.Height ());	
	 
	oldpen =(CPen *)SelectObject (m_myHDC,&pen);
	 ::FillRect(m_myHDC,&m_rectSrc,brush);
	 
	DWORD size;
	size = wBitsPerSample == 16 ? SIZE_AUDIO_FRAME/2 : SIZE_AUDIO_FRAME;
	int yy = m_rectSrc.Height()/2;
	::MoveToEx(m_myHDC,0,yy,NULL);
	 
	int h=yy;
	short sample;
	for(int register i = 0 ; i <(long)size ; i++)	//to draw first channel
	{
		sample = wBitsPerSample == 16 ? ((*((short*)pBuf+i))*h)/(65535/2) : ( (*((BYTE*)pBuf+i)-128)*h)/128;
		x = int(((float)i/size)*(m_rectSrc.Width()));
		y = yy-sample;
		::LineTo(m_myHDC,x,y);
	}
	
	StretchBlt(::GetDC(m_hWnd),0,0,m_rectDest.Width (),m_rectDest.Height (),m_myHDC,0,0,m_rectSrc.Width (),m_rectSrc.Height(),SRCCOPY);
	::SelectObject(m_myHDC,oldpen);
	 SelectObject(m_myHDC,m_pOldBitmap);
	 ::DeleteObject(m_bitmap);
	::ReleaseDC(m_hWnd,m_myHDC);
	
	CWnd::FromHandle(m_hWnd)->UpdateData(false);
 
 	return 1;
}
 