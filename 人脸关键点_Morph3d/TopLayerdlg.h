#pragma once

#include <afxdialogex.h>
// TopLayerdlg dialog
#include "resource.h"
class TopLayerdlg : public CDialogEx
{
	DECLARE_DYNAMIC(TopLayerdlg)

public:
	int mWidth;
	int mHeight;
	TopLayerdlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TopLayerdlg();

// Dialog Data
	enum { IDD = IDD_DLG_TOP};
	void SetPoint(int x=100,int y=100);
	void SetPoint(CPoint pt = (100,100));
	void SetFaceR(float * fR,int iCount = 1);
	void SetFaceInfo(float * fR, float * marks, int iCount= 1);
	void SetWndPos(int x,int y,int w,int h);
private:
	CPoint m_pt;
protected:
	int mFaceCount;
	float m_faceR[5*50];
	float mMarks[10*50];
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
