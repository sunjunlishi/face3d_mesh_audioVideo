// USBVideoDlg.h : header file
//

#if !defined(AFX_USBVIDEODLG_H__5E8A068E_88EC_48CE_AEA4_914975035930__INCLUDED_)
#define AFX_USBVIDEODLG_H__5E8A068E_88EC_48CE_AEA4_914975035930__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUSBVideoDlg dialog


#include <afxwin.h>
#include <opencv2\core.hpp>
#include "TopLayerdlg.h"
#define WM_START_SELCAMERASHOW (WM_USER+6)
class CCaptureImage;
class CGraphic;
class CVdoFrameHandler;

 class  CUSBVideoDlg : public CDialog
{
// Construction
public:
	CUSBVideoDlg(CWnd* pParent = NULL);	
	// standard constructor
	void SetBFreeWork(bool bFreeWork);
// Dialog Data
	//{{AFX_DATA(CUSBVideoDlg)
	enum { IDD = 10001};

	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBVideoDlg)
	afx_msg void OnCloseDevices();	
	afx_msg void OnStartCapture();

	void OnOpenDevices();

	//}}AFX_VIRTUAL
	void ShowBufRgb32Data(cv::Mat img);
	void SetShowText(CString text,int posX,int posY);
	void SetTopDlgPos();
	void SetFaceR(float * fR, int iCount);
	void SetInfoAngle(CString angleInfo);
	HRESULT GetLBdata(unsigned char * * data);
	void ShowToFront();
	void SetVdoFrameHandler(CVdoFrameHandler * frameHandler);
	void CreateObj(CWnd* parent);
	afx_msg void OnBnClickedStopCapture();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	CString m_strInfoAnge;

	void ChangeSizeAndPos();

	
// Implementation
protected:
	TopLayerdlg m_topDlg;
	HBRUSH m_brush;
	CCaptureImage * m_CapImage;
	CGraphic * m_ddraw;
	// Generated message map functions
	//{{AFX_MSG(CUSBVideoDlg)
	virtual BOOL OnInitDialog();

	BOOL DrawFacePtsLimit(cv::Mat& src);
	afx_msg void OnPaint();


	LRESULT  OnselCameraShow(WPARAM wParam,LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CVdoFrameHandler * m_frameHandler;
	CComboBox	m_ListCtrl;

	int m_posX;
	int m_posY;

	int m_nShowW;//实际显示的宽
	int m_nShowH;//实际显示的高

	std::vector<std::vector<cv::Point> > m_psVecTraceFace;
	bool m_bFreeWork;

	CString m_infoText;
	int m_textPosX;
	int m_textPosY;

	CBitmap hBitmap; 
	CDC hdcMem;
	HBITMAP hOldBitmap2;
	CButton m_btnClose;
	CButton m_btnSelOk;
private:
	afx_msg void OnBnClickedButtonSelok();
	afx_msg void OnBnClickedBtnFilterpro();
	afx_msg void OnBnClickedBtnPinpro();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	
	
	

	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

public:
	afx_msg void OnClose();
 };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBVIDEODLG_H__5E8A068E_88EC_48CE_AEA4_914975035930__INCLUDED_)
