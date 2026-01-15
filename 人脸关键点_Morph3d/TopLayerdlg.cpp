// TopLayerdlg.cpp : implementation file
//

#include "stdafx.h"
//#include "LayeredWindow.h"
#include "TopLayerdlg.h"
#include "afxdialogex.h"


// TopLayerdlg dialog

IMPLEMENT_DYNAMIC(TopLayerdlg, CDialogEx)

TopLayerdlg::TopLayerdlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(TopLayerdlg::IDD, pParent),m_pt(100,100)
{
	mWidth = 640;
	mHeight = 480;
}

TopLayerdlg::~TopLayerdlg()
{
}

void TopLayerdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TopLayerdlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// TopLayerdlg message handlers


BOOL TopLayerdlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE,	GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE)^WS_EX_LAYERED);
	//::SetWindowPos(this->m_hWnd,HWND_TOPMOST,10,10,10,10,SWP_NOMOVE|SWP_NOSIZE);
	COLORREF clr = RGB(255,255,255);
	SetLayeredWindowAttributes(  clr, 128, LWA_COLORKEY);//LWA_COLORKEY LWA_ALPHA     //通过第三个参数来设置窗体透明程度
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void TopLayerdlg::SetPoint(int x,int y)
{
	m_pt.x = x;
	m_pt.y = y;
	Invalidate();


}
void TopLayerdlg::SetFaceInfo(float * marks, float * fR,int iCount)
{
	mFaceCount = iCount;
	memcpy(m_faceR, fR, 4 * iCount * 4);
	memcpy(mMarks, marks, 10 * iCount * 4);
	Invalidate();
}

void TopLayerdlg::SetFaceR(float * fR,int iCount)
{
	
	mFaceCount = iCount;
	if (iCount > 0)
	{
		memcpy(m_faceR, fR, 4 * iCount * 4);
	}
	
	Invalidate();
}
void TopLayerdlg::SetWndPos(int x, int y, int w, int h)
{
	mWidth = w;
	mHeight = h;
	MoveWindow(0, 0, mWidth, mHeight);
}

void TopLayerdlg::SetPoint(CPoint pt)
{
	m_pt = pt;
	Invalidate();
}

void TopLayerdlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	//SetBackgroundColor(RGB(255,255,255));
	CRect rect;
	GetClientRect(&rect);
	CBrush bs;
	
	bs.CreateSolidBrush(RGB(255,255,255));
	dc.FillRect(rect,&bs);
	CPen pen;
	if (m_faceR[0] > 0.67)
	{
		pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	}
	else
	pen.CreatePen(PS_SOLID,3,RGB(255,0,0));

	dc.SelectObject(&pen); // 100,100
	mWidth = rect.Width();
	mHeight = rect.Height();
//	dc.MoveTo(m_pt.x-300,m_pt.y);
//	dc.LineTo(m_pt.x+200,m_pt.y);

	for (int k = 0; k < mFaceCount; ++k)
	{
	/*	float x1 = m_faceR[k * 4 + 0];
		float y1 = m_faceR[k * 4 + 1];
		float x2 = m_faceR[k * 4 + 2];
		float y2 = m_faceR[k * 4 + 3];
*/
		//dc.Rectangle(x1, y1,x2, y2);

		char szId[256];
		//sprintf(szId, "%.2f", m_faceR[k * 5 ]);
		//dc.DrawText(szId, CRect(x1,y1,x2,y2), 0);
	}
	





	/*for (int k = 0; k < mFaceCount; ++k)
	{
		int span = 3;
		for (int m = 0; m < 5; ++m)
			dc.Ellipse(mMarks[k * 10 + m] * mWidth - span, mMarks[k * 10 + m + 5] * mHeight - span,
				mMarks[k * 10 + m] * mWidth + span, mMarks[k * 10 + m + 5] * mHeight + span);
	}*/
}
