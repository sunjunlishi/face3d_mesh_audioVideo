// SoundIn.cpp: implementation of the CSoundIn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdAfx.h"
#include "SoundIn.h"
// include callback class



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSoundIn, CWinThread)

CSoundIn::CSoundIn()
{
	m_QueuedBuffers = 0;
	m_hRecord = NULL;
	m_bRecording = FALSE;
	DataFromSoundIn = NULL;
	m_pOwner = NULL;
	CreateThread();
	m_bAutoDelete = FALSE;
	InitializeCriticalSection(&m_cs);//初始化临界资源对象

	 
	 
//	TRACE("%d",m_Format.nSamplesPerSec);
//	TRACE("%d",m_Format.wBitsPerSample);
}

CSoundIn::~CSoundIn()
{
 
	if(IsRecording())
		Stop();
	::PostQuitMessage(0);
	DeleteCriticalSection(&m_cs);
}
extern CStatic	m_Show; //定义静态控件变量
BOOL CSoundIn::InitInstance()
{
	m_ThreadID = ::GetCurrentThreadId();

	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSoundIn, CWinThread)
	//{{AFX_MSG_MAP(CSoundIn)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(MM_WIM_DATA, OnMM_WIM_DATA)
END_MESSAGE_MAP()

bool CSoundIn::Start(WAVEFORMATEX* format)
{
	MMRESULT mmReturn = 0;
	
	if(IsRecording() || DataFromSoundIn == NULL || m_pOwner == NULL)
	{
		// already recording!
		return FALSE;
	}
	else
	{
		if(format != NULL)
			m_Format = *format;

		// open wavein device
		mmReturn = ::waveInOpen( &m_hRecord, WAVE_MAPPER, &m_Format, m_ThreadID, NULL, CALLBACK_THREAD);
		FILE * fp = fopen("./毛阿敏16.pcm", "rb");
		if (fp)
		{
			fseek(fp, 0, 2);
			mFLen = ftell(fp);
			mPlayBuffer = new char[mFLen];
			fseek(fp, 0, 0);
			int readLen = fread(mPlayBuffer, 1, mFLen, fp);
			mFnowLen = 0;
		}
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "in Start()");
			return FALSE;
		}
		else
		{
			// make several input buffers and add them to the input queue
			for(int i=0; i<3; i++)
			{
				AddInputBufferToQueue();
			}
			
			// start recording
			EnterCriticalSection(&m_cs);
			mmReturn = ::waveInStart(m_hRecord);
			if(mmReturn )
			{
				waveInErrorMsg(mmReturn, "in Start()");
				LeaveCriticalSection(&m_cs);
				return FALSE;
			}
			m_bRecording = TRUE;
			LeaveCriticalSection(&m_cs);
		}
	}
	return TRUE;
}

void CSoundIn::Stop()
{
	MMRESULT mmReturn = MMSYSERR_NOERROR;
	if(!IsRecording())
	{
		return;
	}
	else
	{
		EnterCriticalSection( &m_cs );
		m_bRecording = FALSE;
		mmReturn = ::waveInReset(m_hRecord);
		if(mmReturn)
		{			 
			ErrorMsg("CSoundIn waveInReset failed!");
			 
			LeaveCriticalSection( &m_cs );
			return;
		}
		else
		{
			m_bRecording = FALSE;
			LeaveCriticalSection( &m_cs );
			Sleep(500);
			mmReturn = ::waveInClose(m_hRecord);
			if(mmReturn) 
			{
				ErrorMsg("CSoundIn waveinClose failed!");
				 
			}
			Sleep(10);
		}
		if(m_QueuedBuffers != 0) 
			ErrorMsg("Still %d buffers in waveIn queue!", m_QueuedBuffers);
	}
}
 
void CSoundIn::OnMM_WIM_DATA(WPARAM parm1, LPARAM parm2)
{
	MMRESULT mmReturn = 0;
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;

	if(IsRecording())
	{
		//释放内存要在m_bRecording 为假之后进行否则可能引起死锁
		mmReturn = ::waveInUnprepareHeader(m_hRecord, pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
			return;
		}
		CBuffer buf(mPlayBuffer+mFnowLen, pHdr->dwBufferLength);
		mFnowLen += pHdr->dwBufferLength;
		char szBuf[256];
		sprintf(szBuf," %d ", pHdr->dwBufferLength);
		TRACE0(szBuf);

	
		::PostMessage(::FindWindow(NULL,"降噪处理软件"),WM_MY_DRAW,0,parm2);
	

		// virtual processing function supplyed by user
		DataFromSoundIn(&buf, m_pOwner);//调用WriteSoundDataToFile(buffer)完成对m_pPlayBuffer[5]数据填充

		// reuse the buffer:
		// prepare it again
		mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
		}
		else // no error
		{
			// add the input buffer to the queue again
			mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
			if(mmReturn) waveInErrorMsg(mmReturn, "in OnWIM_DATA()");
			else return;  // no error
		}	
	}
	delete []pHdr->lpData;
	delete pHdr;
	m_QueuedBuffers--;
}

int CSoundIn::AddInputBufferToQueue()
{
	MMRESULT mmReturn = 0;
	
	// create the header
	LPWAVEHDR pHdr = new WAVEHDR;
	if(pHdr == NULL) return NULL;
	ZeroMemory(pHdr, sizeof(WAVEHDR));

	// new a buffer
	CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
	pHdr->lpData = buf.m_chData;
	pHdr->dwBufferLength = buf.ByteLen;
	
	// prepare it
	mmReturn = ::waveInPrepareHeader(m_hRecord,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		return m_QueuedBuffers;
	}

	// add the input buffer to the queue
	mmReturn = ::waveInAddBuffer(m_hRecord, pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveInErrorMsg(mmReturn, "in AddInputBufferToQueue()");
		return m_QueuedBuffers;
	}

	// no error
	// increment the number of waiting buffers
	return m_QueuedBuffers++;
}

void CSoundIn::waveInErrorMsg(MMRESULT result, LPCTSTR addstr)
{
	// say error message
	char errorbuffer[100];
	waveInGetErrorText(result, errorbuffer,100);
	ErrorMsg("WAVEIN:%x:%s %s", result, errorbuffer, addstr);
}

BOOL CSoundIn::IsRecording()
{
	BOOL bRecding;
	EnterCriticalSection( &m_cs );
	bRecding = m_bRecording;
	LeaveCriticalSection( &m_cs );
	return bRecding;
}
