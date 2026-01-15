// CaptureImage.cpp: implementation of the CCaptureImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "USBVideo.h"
#include "CaptureImage.h"
#include "PublicFun.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaptureImage::CCaptureImage()
{

}

CCaptureImage::~CCaptureImage()
{

}

void CCaptureImage::VdoFrameData(long lw, long lh, BYTE* pBuffer, long lBufferSize)
{
 //	SYSTEMTIME tm;
 //	GetLocalTime(&tm);
 //	CString strFile;
	///*strFile.Format("C:\\%04d-%02d-%02d %02d:%02d:%02d.bmp",int(tm.wYear),int(tm.wMonth),int(tm.wDay),
	//	int(tm.wHour),int(tm.wMinute),int(tm.wSecond));*/
	//strFile.Format(L"C:\\%04d%02d%02d%02d%02d%02d.bmp",int(tm.wYear),int(tm.wMonth),int(tm.wDay),
	//	int(tm.wHour),int(tm.wMinute),int(tm.wSecond));

	//HANDLE hf = CreateFile(
	//	strFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
	//	CREATE_ALWAYS, NULL, NULL );
	//if( hf == INVALID_HANDLE_VALUE )
	//	return;
	//// 写文件头 
	//BITMAPFILEHEADER bfh;
	//memset( &bfh, 0, sizeof( bfh ) );
	//bfh.bfType = 0x4D42;//'BM'
	//bfh.bfSize = sizeof( bfh ) + lBufferSize + sizeof(BITMAPINFOHEADER);
	//bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof(BITMAPFILEHEADER);
	//DWORD dwWritten = 0;
	//WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );
	//// 写位图格式
	//BITMAPINFOHEADER bih;
	//memset( &bih, 0, sizeof( bih ) );
	//bih.biSize = sizeof( bih );
	//bih.biWidth = lw;
	//bih.biHeight = lh;
	//bih.biPlanes = 1;
	//bih.biBitCount = 24;
	//WriteFile(hf, &bih, sizeof(bih), &dwWritten, NULL);
	//// 写位图数据
	//WriteFile(hf,pBuffer, lBufferSize, &dwWritten, NULL);
	//CloseHandle(hf);
	//StopCapture();//存完一张照片后停止采集
	if(!m_image.IsNull())                                                                  
	{                                                                                            
		m_image.Destroy();                                                                 
	}
	m_image.Create(lw, lh, 24); 
	if(m_image.IsNull())                                                                         
	{      
		return;                                                                                
	}
	byte* q = NULL;                                                                                     
	//byte* p = new byte[lw * lh * 3];                                                                                          

	for(int y = 0, z = lh - 1; y < lh, z >= 0; y++, z--)                                       
	{                                                                                            
		q = (byte*)m_image.GetPixelAddress(0, z);                                                 
		memcpy(q, &pBuffer[lw * 3 * y], lw * 3);                                            
	}                                                                                            
	//delete []p;      
	StopCapture();//存完一张照片后停止采集
}
/************************************************************************/
/* 开始采集                                                             */
/************************************************************************/
void CCaptureImage::StartCapture()
{
	m_CaptureVideo.GrabVideoFrames(TRUE,this);
}

HRESULT CCaptureImage::OpenDevice(int index, HWND wnd)
{
	return m_CaptureVideo.OpenDevices(index, wnd, 640, 480);
}

void CCaptureImage::StartCapture(CVdoFrameHandler * frameHandler)
{
	m_CaptureVideo.GrabVideoFrames(TRUE, frameHandler);
}
/************************************************************************/
/* 停止采集                                                             */
/************************************************************************/
void CCaptureImage::StopCapture()
{
	m_CaptureVideo.GrabVideoFrames(FALSE,NULL);
	//m_CaptureVideo.ClearVideoWindow();
}
void CCaptureImage::EnumDevlist(HWND list)
{
	m_CaptureVideo.EnumDevices(list);
}
void	CCaptureImage::SetLevel(int nLevel)
{
	m_CaptureVideo.m_nLevel = nLevel;
}
