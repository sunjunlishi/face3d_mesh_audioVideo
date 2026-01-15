// SoundOut.cpp: implementation of the CSoundOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdAfx.h"
#include "SoundOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// IMPLEMENT_DYNCREATE(CSoundOut, CWinThread)

CSoundOut::CSoundOut()
{
	m_QueuedBuffers = 0;
	m_hPlay = NULL;
	m_bPlaying = FALSE;
	GetDataToSoundOut = NULL;
	m_pOwner = NULL;
	CreateThread();
	m_bAutoDelete = FALSE;
	InitializeCriticalSection(&m_cs);
	m_TimeDelay=0;
}

CSoundOut::~CSoundOut()
{
	if(IsPlaying())
		Stop();
	::PostQuitMessage(0);
	DeleteCriticalSection(&m_cs);
}

BOOL CSoundOut::InitInstance()
{
	m_ThreadID = ::GetCurrentThreadId();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSoundOut, CWinThread)
	//{{AFX_MSG_MAP(CSoundOut)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(MM_WOM_DONE, OnMM_WOM_DONE)
END_MESSAGE_MAP()

bool CSoundOut::Start(WAVEFORMATEX* format)
{
	MMRESULT mmReturn = 0;
	m_start = clock();
	if(m_bPlaying || GetDataToSoundOut == NULL || m_pOwner == NULL)
	{
		// already recording!
		return FALSE;
	}
	else
	{
		if(format != NULL)		// was this function called with at specific format
			m_Format = *format;
		
		if(m_TimeDelay>0)
			Sleep(m_TimeDelay);
		// open wavein device
		EnterCriticalSection(&m_cs);
		mmReturn = ::waveOutOpen( &m_hPlay, WAVE_MAPPER, &m_Format, m_ThreadID, NULL, CALLBACK_THREAD);
		if(mmReturn)
		{
			waveOutErrorMsg(mmReturn, "in start()");
			LeaveCriticalSection(&m_cs);
			return FALSE;
		}
		else
		{
			m_bPlaying = TRUE;
			
			// we need at least 2 -> one for playing, one for refilling (3 is better)
			for(int i=0; i<3; i++)
			{
				CBuffer buf(m_Format.nBlockAlign*m_BufferSize, false);
				GetDataToSoundOut(&buf, m_pOwner);//调用ReadSoundDataFromFile(buffer)完成从m_pPlayBuffer[m_nOutPos]读取数据并将数据放入buffer中
				AddOutputBufferToQueue(&buf);
			}
			LeaveCriticalSection(&m_cs);
		}
	}
	return TRUE;

}

void CSoundOut::Stop()
{
	MMRESULT mmReturn = 0;
	if(IsPlaying())
	{
		EnterCriticalSection(&m_cs);
		m_bPlaying = FALSE;
		mmReturn = ::waveOutReset(m_hPlay);
		Sleep(500);
		if(mmReturn) 
		{
			ErrorMsg("CSoundOut waveinReset failed!");
		}
		TRACE("waveOutReset()\n");
		mmReturn=::waveOutClose(m_hPlay);
		Sleep(30);
		if(mmReturn)
		{
			ErrorMsg("CSoundOut waveoutClose failed!");
		}
		LeaveCriticalSection(&m_cs);
	}
}


void CSoundOut::OnMM_WOM_DONE(WPARAM parm1, LPARAM parm2)
{
 
		
	MMRESULT mmReturn = 0;
	
	LPWAVEHDR pHdr = (LPWAVEHDR) parm2;


	if(IsPlaying())
	{	//释放内存要在m_bPlaying为假之后进行否则有可能引起死锁
		mmReturn = ::waveOutUnprepareHeader(m_hPlay, pHdr, sizeof(WAVEHDR));
		if(mmReturn)
		{
			waveOutErrorMsg(mmReturn, "in OnWOM_DONE()");
			return;
		}
		m_QueuedBuffers--;
		CBuffer buf(pHdr->lpData, pHdr->dwBufferLength);
		// virtual function supplyed by user
		if (!GetDataToSoundOut(&buf, m_pOwner))
		{
			OutputDebugString("WOM_DONE : GetDataToSoundOut  NULL *********************************************************\n");

			
			return;
		}
		
		
		if(buf.ByteLen > 0)
		{
			if ((clock() - m_start) < m_longtime)
			{
				AddOutputBufferToQueue(&buf);
				TRACE("WOM_DONE : refill buffer\n");
				OutputDebugString("WOM_DONE : refill buffer\n");
				// delete old header
			}
			
			delete pHdr;
			return;			
		}
		else
		{
			Stop();
		}
	}

	// we are closing the waveOut handle, 
	// all data must be deleted
	// this buffer was allocated in Start()
	delete []pHdr->lpData;
	delete pHdr;
	TRACE("WOM_DONE : remove buffer\n");

	if(m_QueuedBuffers == 0 && m_bPlaying == false)
	{
		mmReturn = ::waveOutClose(m_hPlay);
		if(mmReturn) waveOutErrorMsg(mmReturn, "in stop()");
		TRACE("waveOutClose()\n");
	}
}

int CSoundOut::AddOutputBufferToQueue(CBuffer *buffer)
{
	MMRESULT mmReturn = 0;
	
	// create the header
	LPWAVEHDR pHdr = new WAVEHDR;
	if(pHdr == NULL) return NULL;

	/*波形取反  
			int tempint=m_SoundOut.m_BufferSize;
			short tempshort[tempint];
			for(int i=0;i<tempint;i++)
			tempshort[i]   =-(*((short*)buffer->m_chData+i));
			if(!buffer->m_chData)
			delete []buffer->m_chData;
			memcpy(buffer->m_chData,tempshort,tempint);
	*/
	//波形取反
	for(int i=0;i<buffer->ByteLen/2;i++)
		(*((short*)buffer->m_chData+i))=-(*((short*)buffer->m_chData+i));
	// new a buffer
	pHdr->lpData = buffer->m_chData;
	pHdr->dwBufferLength = buffer->ByteLen;
	pHdr->dwFlags = 0;
	
	// prepare it
	mmReturn = ::waveOutPrepareHeader(m_hPlay,pHdr, sizeof(WAVEHDR));
	if(mmReturn)
	{
		waveOutErrorMsg(mmReturn, "in AddOutputBufferToQueue()");
		return m_QueuedBuffers;
	}
	// write the buffer to output queue
	mmReturn = ::waveOutWrite(m_hPlay, pHdr, sizeof(WAVEHDR));
	if(mmReturn) 
		waveOutErrorMsg(mmReturn, "in AddOutputBufferToQueue()");
	// increment the number of waiting buffers
	return m_QueuedBuffers++;
}

void CSoundOut::waveOutErrorMsg(MMRESULT result, LPCTSTR addstr)
{
	// say error message
	char errorbuffer[100];
	waveOutGetErrorText(result, errorbuffer,100);
	ErrorMsg("WAVEOUT:%x:%s %s", result, errorbuffer, addstr);
}

BOOL CSoundOut::IsPlaying()
{
	BOOL bPlaying = FALSE;
	EnterCriticalSection( &m_cs );
	bPlaying = m_bPlaying;
	LeaveCriticalSection( &m_cs );
	return bPlaying;
}
