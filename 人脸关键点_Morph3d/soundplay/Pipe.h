// Pipe.h: interface for the CPipe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPE_H__71D36BFB_9EC3_4FEB_91BB_0178A52B8B7B__INCLUDED_)
#define AFX_PIPE_H__71D36BFB_9EC3_4FEB_91BB_0178A52B8B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SoundIn.h"
#include "SoundOut.h"
#include "soundfile.h"
#include "ptrFIFO.h"
#define MAX_BUFCOUNT 20

class CPipe
{
public:
	CPipe();
	virtual ~CPipe();

	virtual void OnEndOfPlayingFile();
	
public:
	void StartRecordToBuffer();
	void StartPlayingFromBuffer();
	void StopPlayingFromFile(char *filename);
	void StartPlayingFromFile(int longtime);
	void StopRecordingToFile();
	void StartRecordingToFile();
	void WriteSoundDataToFile(CBuffer* buffer);
	BOOL ReadSoundDataFromFile(CBuffer* buffer);
	static void DataFromSoundIn(CBuffer* buffer, void* Owner);
	static BOOL GetDataToSoundOut(CBuffer* buffer, void* Owner);
public:
	CPtrFIFO m_FIFOFull;	// FIFO holding pointers to buffers with sound
	CPtrFIFO m_FIFOEmpty;	// FIFO holding pointers to empty buffers that can be reused

	CSoundFile* m_pFile;
	CSoundOut	m_SoundOut;
	CSoundIn	m_SoundIn;
	//////////////////////////////////////////////////////////////////////////
	//
	CRITICAL_SECTION m_cs;
	CBuffer* m_pPlayBuffer[10];//fuffer array used by play
	int      m_nOutPos;
	int      m_nInPos;
	//-------------------------------------------------------------//
	BOOL m_bRealTime;  //实时播放标志
	CString m_strFilePath;
};

#endif // !defined(AFX_PIPE_H__71D36BFB_9EC3_4FEB_91BB_0178A52B8B7B__INCLUDED_)
