// SoundOut.h: interface for the CSoundOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_)
#define AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SoundBase.h"

				// CWinThread must be before CSoundBase -> and it's very difficult to debug!
				// Trust me!   Thomas.Holme@openmpeg4.org
class CSoundOut : public CWinThread, public CSoundBase
{
//	DECLARE_DYNCREATE(CSoundOut)

public:
	int m_longtime;
	clock_t m_start;
	BOOL IsPlaying();
	CSoundOut();
	virtual ~CSoundOut();
	CRITICAL_SECTION m_cs;

	// pointer to function
	BOOL (*GetDataToSoundOut)(CBuffer* buffer, void* Owner);
	void* m_pOwner;

	virtual void Stop();
	virtual bool Start(WAVEFORMATEX* format = NULL);
	static void waveOutErrorMsg(MMRESULT result, LPCTSTR addstr);

	afx_msg void OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2);
	DECLARE_MESSAGE_MAP()

	int AddOutputBufferToQueue(CBuffer* buffer);
protected:
	BOOL m_bPlaying;
	HWAVEOUT m_hPlay;
	DWORD m_ThreadID;
	int m_QueuedBuffers;

	BOOL InitInstance();
public:
	UINT m_TimeDelay;
 
	
};

#endif // !defined(AFX_SOUNDOUT_H__F926DE0E_4F1A_4A8C_B189_5E4D63446A95__INCLUDED_)
