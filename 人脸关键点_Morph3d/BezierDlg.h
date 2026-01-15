// BezierDlg.h : header file
//

#if !defined(AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_)
#define AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
/*
	 
	  
*/
// CBezierDlg dialog
#include <opencv2\core.hpp>
#include "afxwin.h"
#include <mfc3dshow\MFCDlgDlg.h>
#include "sherpa-onnx/c-api/c-api.h"
using namespace std;
#include "USBVideoDlg.h"
#include "CaptureVideo.h"
#include <mutex>
#include <sapi.h>           // SAPI includes
//#include "face_swap\basel_3dmm.h"
#include "soundplay\Pipe.h"

class CBezierDlg : public CDialog, public CVdoFrameHandler, public CPipe
{
// Construction
public:
	int m_nPlayLen;
	CString m_characerID;
	int m_nCur[15];
	void DoFrame();

	void DoInitObj();
	BOOL m_bWorking;
	CBezierDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void VdoFrameData(long lw, long lh, unsigned char * pBuffer, long lBufferSize);
// Dialog Data
	//{{AFX_DATA(CBezierDlg)
	enum { IDD = IDD_BEZIER_DIALOG };
	//}}AFX_DATA
	eos::core::Mesh  m_obj;

	eos::core::Mesh  m_obj1;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBezierDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	int m_nLeftPos;
// Implementation
private:

	const SherpaOnnxOfflineTts* tts_ = nullptr;
	float * m_3ddout;
	std::mutex mMutex;
	std::mutex mMutexBk;

	std::vector<float> mBlends;
	CPaintDC * pDC;
	int  m_currentMark;
	vector<cv::Point2f> m_points;
	cv::Mat m_srcImg;

	vector<cv::Point2f> m_pointsBk;
	cv::Mat m_srcImgBk;



	CString m_fileName;
	void DrawThePicPoint(cv::Mat clmResult, int posX, int posY, int dstW);
	bool m_bDeleteFile;
	bool m_bMouseMove;
	bool m_leftBtnDown;
	bool m_bKeyOper;
	int m_nowMulti;


	cv::Mat m_nowImg;
private:
	afx_msg void OnBnClickedBtnImport();
	afx_msg void OnBnClickedBtnSave();
	void do3dWork(std::vector<float> blends);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);


	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnBig();
	afx_msg void OnBnClickedBtnOrigin();
	CUSBVideoDlg m_dlgVideo;
	CMFCDlgDlg m_dlgMfc;
	CMFCDlgDlg m_dlgMfc1;
	void DrawPoint(int start,int end,CDC * pDc);

	void CLMGetCNN106(cv::Mat dst, cv::Mat showMat);

	void DoGetPtsAndDraw(CString fileName);


	// Generated message map functions
	//{{AFX_MSG(CBezierDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnMyMessage(WPARAM wParam, LPARAM lParam);
	LRESULT OnMyMessage2(WPARAM wParam, LPARAM lParam);
	LRESULT OnMyMessageWave(WPARAM wParam, LPARAM lParam);
public:
	
	CComPtr<ISpAudio>   m_cpOutAudio;
	int m_DefaultFormatIndex;
	HRESULT VoiceChange();
	afx_msg LRESULT  MainHandleSynthEvent(WPARAM wparam, LPARAM lParam);
	

	afx_msg void OnBnClickedButton3d();
	void Init();
	void GenerateAudio(std::string text);
	 void OnBnClickedButton3d(cv::Mat img, vector<cv::Point2f> pts);
	afx_msg void OnBnClickedButtonIn();
	void OnRecord(char *filename, char* szContent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSlider9(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDrive3d();
	afx_msg void OnBnClickedSavev();
	CComboBox m_comboIdens; 
	CComboBox m_combox2;
	CComboBox m_comboTietu;

	CComboBox m_comboToushi;

	afx_msg void OnCbnSelchangeCombo1();

	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo4();

	CSliderCtrl m_slider;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedReloadpic();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEZIERDLG_H__78532247_8543_44EB_AE32_706C9CD0D646__INCLUDED_)
