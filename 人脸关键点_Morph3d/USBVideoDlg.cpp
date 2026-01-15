// USBVideoDlg.cpp : implementation file
//

#include <afxwin.h>
#include <atlimage.h>
#include "USBVideoDlg.h"
#include <CaptureImage\CaptureImage.h>
#include <dshow.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Common/CommonWin.h"
#include "resource.h"

using namespace cv;
/////////////////////////////////////////////////////////////////////////////
// CUSBVideoDlg dialog

CUSBVideoDlg::CUSBVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBVideoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUSBVideoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	

	m_nShowW = 640;//实际显示的宽限制在900
	m_nShowH = 480;

	m_posX = 5;
	m_posY = 10;

	m_bFreeWork = false;

	//m_ddraw = new CGraphic;

}

void CUSBVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUSBVideoDlg)
	DDX_Control(pDX, IDC_COMBO1, m_ListCtrl);
	//}}AFX_DATA_MAP


}

BEGIN_MESSAGE_MAP(CUSBVideoDlg, CDialog)
	//{{AFX_MSG_MAP(CUSBVideoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_CLOSE_DEVICES, OnCloseDevices)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_FILTERPRO, &CUSBVideoDlg::OnBnClickedBtnFilterpro)
	ON_BN_CLICKED(IDC_BTN_PINPRO, &CUSBVideoDlg::OnBnClickedBtnPinpro)
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	
	ON_WM_DESTROY()
	ON_MESSAGE(WM_START_SELCAMERASHOW,&CUSBVideoDlg::OnselCameraShow)
	ON_WM_SYSCOMMAND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUSBVideoDlg message handlers

BOOL CUSBVideoDlg::OnInitDialog()
{

	CDialog::OnInitDialog();

	

	m_CapImage = new CCaptureImage;

	m_CapImage->EnumDevlist(m_ListCtrl);
	
	m_ListCtrl.SetCurSel(m_ListCtrl.GetCount() - 1);//m_ListCtrl.GetCount()-1


	m_topDlg.Create(IDD_DLG_TOP, this);


	CRect rt;
	GetClientRect(rt);
	GetDlgItem(IDC_STATIC_FU)->MoveWindow(0, 0, 310, 230);
	m_topDlg.MoveWindow(0, 0, 310, 230);
	m_ListCtrl.MoveWindow(315,5,65,100);

	return TRUE;
}

LRESULT  CUSBVideoDlg::OnselCameraShow(WPARAM wParam,LPARAM lParam)
{
	ShowToFront();
	return true;
}
void CUSBVideoDlg::ChangeSizeAndPos()
{
	// int nPicWidth = m_CapImage->
	// int nPicHeight = m_CapImage->GetH();
	////重新更新显示宽高
	//if(nPicWidth  > 640)
	//{
	//	float ratio =nPicWidth/ 640.0;
	//	m_nShowH = nPicHeight/ratio;
	//	m_nShowW = 640;
	//}
	//else
	//{
	//	m_nShowW = nPicWidth ;
	//	m_nShowH = nPicHeight;
	//}
	//int cx = GetSystemMetrics(SM_CXSCREEN);

	//MoveWindow(360,m_posY, m_nShowW+100, m_nShowH,SWP_SHOWWINDOW);
	//GetDlgItem(IDC_STATIC_FU)->MoveWindow(0,0,m_nShowW,m_nShowH);

	//GetDlgItem(IDC_STATIC_FU)->MoveWindow(15,nTopSpan,10,10);
	//GetDlgItem(IDC_STATIC_FU)->ShowWindow(SW_HIDE);
	

	//GetDlgItem(IDC_COMBO1)->MoveWindow(40,height-50-m_capInfoHeight,138,30);
	//GetDlgItem(IDC_CLOSE_DEVICES)->MoveWindow(80+138,height-50-m_capInfoHeight,80,35);
	//GetDlgItem(IDC_BTN_FILTERPRO)->MoveWindow(120+132+80,height-50-m_capInfoHeight,80,35);
	//GetDlgItem(IDC_BTN_PINPRO)->MoveWindow(120+132*2+80,height-50-m_capInfoHeight,80,35);

	/*GetDlgItem(IDC_COMBO1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CLOSE_DEVICES)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_FILTERPRO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BTN_PINPRO)->ShowWindow(SW_HIDE);*/

	Invalidate();
}
void CUSBVideoDlg::ShowToFront()
{

	Invalidate();
	ShowWindow(SW_SHOW);
}

void CUSBVideoDlg::SetFaceR(float * fR, int iCount)
{
	m_topDlg.SetFaceR(fR, iCount);
}

void CUSBVideoDlg::SetTopDlgPos()
{
	CPoint pt(0, 0);
	ClientToScreen(&pt);
	::SetWindowPos(m_topDlg.m_hWnd, HWND_TOP, pt.x, pt.y, 640, 480, SWP_NOSIZE);
	m_topDlg.ShowWindow(SW_SHOW);



}
	

void CUSBVideoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

	}
	else
	{
		CDialog::OnPaint();
	}
}

HRESULT CUSBVideoDlg::GetLBdata(unsigned char * * data)
{
	return S_OK;
	//return mix_getLpdata(data);
}
/************************************************************************/
/* 打开设备                                                             */
/************************************************************************/
void CUSBVideoDlg::OnOpenDevices() 
{
	GetDlgItem(IDC_CLOSE_DEVICES)->EnableWindow(TRUE);
	HWND hWnd = GetDlgItem(IDC_STATIC_FU)->m_hWnd;
	HRESULT hr = m_CapImage->OpenDevice(0,hWnd);
	if(hr == S_OK)
	{
		SetDlgItemText(IDC_CLOSE_DEVICES,"关闭设备");
		ChangeSizeAndPos(); 
	}

}
	
/************************************************************************/
/* 关闭设备                                                             */
/************************************************************************/
void CUSBVideoDlg::OnCloseDevices() 
{
	CString text;
	GetDlgItemText(IDC_CLOSE_DEVICES,text);
	if(text == "打开设备")
	{
		OnOpenDevices();
		OnStartCapture();
	}

}


void CUSBVideoDlg::SetInfoAngle(CString angleInfo)
{
	m_strInfoAnge = angleInfo;
}

void CUSBVideoDlg::SetShowText(CString text, int posX, int posY)
{
	m_infoText = text;
	m_textPosX = posX;
	m_textPosY = posY;


}



void CUSBVideoDlg::ShowBufRgb32Data(cv::Mat img)
{
	//int iWidth = tmpImg.cols;
	//int iHeight = tmpImg.rows;

	//PAINTSTRUCT ps;
	//::BeginPaint( m_StaticScreen.m_hWnd, &ps );
	//CDC * pDC = m_StaticScreen.GetDC();
	//if(pDC)
	//{	
	//	//创建兼容DC
	//	if(!hdcMem.GetSafeHdc())
	//	{
	//		hdcMem.CreateCompatibleDC(pDC);
	//		hBitmap.CreateCompatibleBitmap(pDC,iWidth,iHeight);
	//		hOldBitmap2 = (HBITMAP)hdcMem.SelectObject(hBitmap);
	//	}
	//

	//	 CBitmap hbmp;
	//	 bool bRes = hbmp.CreateBitmap(iWidth,iHeight,1,32,(const void*)tmpImg.data);
	//	 CDC dcBmp;
	//	 dcBmp.CreateCompatibleDC(pDC);
	//	 HBITMAP hOldBitmap = (HBITMAP)dcBmp.SelectObject(hbmp);


	//	hdcMem.BitBlt(0,0,iWidth,iHeight,&dcBmp,0,0,SRCCOPY);

	//
	//	 // 写文字提示
	//	 DrawMyText(m_infoText,m_textPosX,m_textPosY, &hdcMem,40,RGB(128,255,0));
	//	 if(!m_strInfoAnge.IsEmpty())
	//	 DrawMyText(m_strInfoAnge, (iWidth-190)/2,47, &hdcMem, 23, RGB(0, 255, 0));
	//	 // 将后备缓冲区中的图形拷贝到前端缓冲区
	//	 pDC->BitBlt(0, 0, iWidth, iHeight, &hdcMem, 0, 0, SRCCOPY);

	//	

	//	 dcBmp.SelectObject( hOldBitmap );
	//	 dcBmp.DeleteDC();
	//	 DeleteObject(hbmp);
	//	 ReleaseDC(pDC);
	//}


	//::EndPaint(m_StaticScreen.m_hWnd, &ps );
	////::ReleaseDC( m_StaticScreen.m_hWnd, hdcStill );
	
}


void CUSBVideoDlg::SetBFreeWork(bool bFreeWork)
{
	m_bFreeWork = bFreeWork;
}

BOOL CUSBVideoDlg::DrawFacePtsLimit(cv::Mat& src)
{
	int depth = 1;
	if(src.type() == CV_8UC3)
	{
		depth = 3;
	}

	return true;
	
}





void CUSBVideoDlg::CreateObj(CWnd* parent)
{
	BOOL bRes = Create(IDD_USBVIDEO_DIALOG,parent);

	return;
}

void CUSBVideoDlg::SetVdoFrameHandler(CVdoFrameHandler * frameHandler)
{
	m_frameHandler = frameHandler;
}

/************************************************************************/
/* 开始采集                                                             */
/************************************************************************/
void CUSBVideoDlg::OnStartCapture() 
{
	//DoResetShow(GetDlgItem(IDC_STATIC_FU)->m_hWnd, m_frameHandler);
	m_CapImage->StartCapture(m_frameHandler);
}
/************************************************************************/


void CUSBVideoDlg::OnBnClickedStopCapture()
{
	m_CapImage->StopCapture();
}


void CUSBVideoDlg::OnBnClickedButtonSelok()
{
	
}


void CUSBVideoDlg::OnBnClickedBtnFilterpro()
{
	 ISpecifyPropertyPages *pSpec;
      CAUUID cauuid;
	  HRESULT hr;
	/*IBaseFilter * videoCapFilter = m_CapImage->GetCaptureFilter();
	if(!videoCapFilter)
	{
		MessageBox("打开摄像头属性设置失败","警告",NULL);
		return;
	}
	  hr = videoCapFilter->QueryInterface(IID_ISpecifyPropertyPages,
                    (void **)&pSpec);
       if(hr == S_OK)
      {
            hr = pSpec->GetPages(&cauuid);

			hr = OleCreatePropertyFrame(m_hWnd, 30, 30, NULL, 1,
                        (IUnknown **)(&videoCapFilter), cauuid.cElems,
                        (GUID *)cauuid.pElems, 0, 0, NULL);

             CoTaskMemFree(cauuid.pElems);
             pSpec->Release();
      }*/
}


void CUSBVideoDlg::OnBnClickedBtnPinpro()
{
	//设置分辨率
	//m_CapImage->ShowPinConfigMix(&(GetDlgItem(IDC_STATIC_FU)->m_hWnd));

	//重新更显显示位置
	ChangeSizeAndPos();  

	Invalidate();
}



BOOL CUSBVideoDlg::OnEraseBkgnd(CDC* pDC)
{

	return CDialog::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	if(pDC)
	{
		pDC->FillSolidRect(rect,RGB(102,102,102));

		int span = 8;
		int colorValue = 128;
		CRect shortRect;		
		
		shortRect.SetRect(0,0,rect.Width(),75);
		pDC->FillSolidRect(shortRect,RGB(255,255,255));
	}
	return TRUE;
}


LRESULT CUSBVideoDlg::OnNcHitTest(CPoint point)
{
	//// TODO: Add your message handler code here and/or call default
	UINT nHitTest = CDialog::OnNcHitTest(point);
	//if(nHitTest == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) < 0)
	//	nHitTest = HTCAPTION;
	return nHitTest;
}





void CUSBVideoDlg::OnDestroy()
{
	//m_CapImage->CloseDevice();
	//m_CapImage->StopCapture();

	if(hdcMem.GetSafeHdc())
	{
		 hdcMem.SelectObject( hOldBitmap2 );
		 hdcMem.DeleteDC();
	}

	__super::OnDestroy();
	// TODO: 在此处添加消息处理程序代码
}





void CUSBVideoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nID == SC_SCREENSAVE || nID == SC_MONITORPOWER) 
    {
		//屏保消息返回，禁止系统的屏幕保护
           return;
    }

	__super::OnSysCommand(nID, lParam);
}


LRESULT CUSBVideoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(  WM_POWERBROADCAST == message)
	{
		if(wParam == PBT_APMQUERYSUSPEND)
		 {
			 //如果是屏保消息，则直接返回
			   return BROADCAST_QUERY_DENY;
		 }
	}
	else if(WM_LBUTTONDOWN == message)
	{
		
	}


	return __super::WindowProc(message, wParam, lParam);
}

void CUSBVideoDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
		CRect rect;
		GetClientRect(rect);
		int nTopSpan = 80;
		int width = rect.Width();

		CRect videoR;
		videoR.SetRect((width-m_nShowW)/2,nTopSpan,
			m_nShowW+(width-m_nShowW)/2,m_nShowH+nTopSpan);
		if(videoR.PtInRect(point))
		{
			
		}
	__super::OnLButtonDblClk(nFlags, point);
}



void CUSBVideoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

		ScreenToClient(&point);
		CRect rect;
		GetClientRect(rect);
		int nTopSpan = 80;
		int width = rect.Width();

		CRect videoR;
		videoR.SetRect((width-m_nShowW)/2,nTopSpan,
			m_nShowW+(width-m_nShowW)/2,m_nShowH+nTopSpan);
		if(videoR.PtInRect(point))
		{
			
		}
	__super::OnLButtonDown(nFlags, point);
}







void CUSBVideoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return ;
	CDialog::OnClose();
}
