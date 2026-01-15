// SoundIn.h: interface for the CSoundIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDIN_H__DFF637AC_D133_4419_B4CA_241DFAC75789__INCLUDED_)
#define AFX_SOUNDIN_H__DFF637AC_D133_4419_B4CA_241DFAC75789__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SoundBase.h"
 
 
  //自定义消息
//#define WM_MY_DRAW  (WM_USER+100)
//#define WM_MYVAD_DRAW  (WM_USER+101)
//static UINT WM_MY_DRAWIN=::RegisterWindowMessage("User");//定义系统唯一消息
//static UINT WM_MY_DRAWOUT=::RegisterWindowMessage("User1");//定义系统唯一消息
static UINT WM_MY_DRAW=::RegisterWindowMessage("User");//定义系统唯一消息
static UINT WM_MYVAD_DRAW = ::RegisterWindowMessage("UserVAD");//定义系统唯一消息
 
				// CWinThread must be before CSoundBase -> and it's very difficult to debug!
				// Trust me!   Thomas.Holme@openmpeg4.org
class CSoundIn : public CWinThread, public CSoundBase  
{
	DECLARE_DYNCREATE(CSoundIn)
public:
	BOOL IsRecording();
	CSoundIn();
	virtual ~CSoundIn();
	
	CRITICAL_SECTION m_cs;
	// pointer to callback function
	void (*DataFromSoundIn)(CBuffer* buffer, void* Owner);
	void* m_pOwner;
	
	virtual void Stop();
	virtual bool Start(WAVEFORMATEX* format = NULL);
	static void waveInErrorMsg(MMRESULT result, LPCTSTR addstr);
  
	char * mPlayBuffer;
	int mFLen;
	int mFnowLen;
	afx_msg void OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2);
	DECLARE_MESSAGE_MAP()

protected:
	BOOL InitInstance();
	int AddInputBufferToQueue();

protected:
	BOOL m_bRecording;
	HWAVEIN m_hRecord;
	int m_QueuedBuffers;
	DWORD m_ThreadID;
 
	 
	
};

#endif // !defined(AFX_SOUNDIN_H__DFF637AC_D133_4419_B4CA_241DFAC75789__INCLUDED_)
