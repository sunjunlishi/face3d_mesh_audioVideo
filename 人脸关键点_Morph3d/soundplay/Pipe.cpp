// Pipe.cpp: implementation of the CPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StdAfx.h"
#include "Pipe.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPipe::CPipe()
{
	m_SoundIn.DataFromSoundIn = DataFromSoundIn;	// assign pointer to callback function
	m_SoundIn.m_pOwner = this;
	m_SoundOut.GetDataToSoundOut = GetDataToSoundOut;  // assign pointer to callback function
	m_SoundOut.m_pOwner = this;
	m_nInPos = 0;
	m_nOutPos = 0;
	for(int i=0; i<10; i++)
		m_pPlayBuffer[i] = NULL;
	InitializeCriticalSection(&m_cs);
	m_pFile = NULL;
	m_bRealTime = FALSE;
	//m_strFilePath = "sound1.wav";
	m_strFilePath = "./out.wav";
}

CPipe::~CPipe()
{
	for(int i=0; i<10; i++)
	{
		if(m_pPlayBuffer[i] != NULL)
			delete m_pPlayBuffer[i];
	}
	DeleteCriticalSection(&m_cs);
}

void CPipe::DataFromSoundIn(CBuffer* buffer, void* Owner)
{
	((CPipe*) Owner)->WriteSoundDataToFile(buffer);
}

void CPipe::WriteSoundDataToFile(CBuffer* buffer)
{
	if(m_bRealTime)//实时录播
	{
		EnterCriticalSection(&m_cs);
		//TRACE("写入一个录音块.序号:%d\n",m_nInPos);
		m_pPlayBuffer[m_nInPos]->ByteLen = buffer->ByteLen;
		memcpy(m_pPlayBuffer[m_nInPos]->m_chData,buffer->m_chData,buffer->ByteLen);
		m_pPlayBuffer[m_nInPos]->m_bHaveData = TRUE;
		m_nInPos++;
		if(m_nInPos > 9)
			m_nInPos = 0;
		LeaveCriticalSection(&m_cs);
	}
	else//写入文件
	{
		if(m_pFile)
		{
			if(!m_pFile->Write(buffer))
			{
				m_SoundIn.Stop();
				AfxMessageBox("写入文件失败");
			}
		}
	}
	
}

BOOL CPipe::GetDataToSoundOut(CBuffer* buffer, void* Owner)
{
	return ((CPipe*) Owner)->ReadSoundDataFromFile(buffer);
}


BOOL CPipe::ReadSoundDataFromFile(CBuffer* buffer)
{ 
	if(m_bRealTime)//实时录播
	{
		EnterCriticalSection(&m_cs);
		//TRACE("读取一个块%d\n",m_nOutPos);
		if(m_pPlayBuffer[m_nOutPos]->m_bHaveData)
		{
			m_pPlayBuffer[m_nOutPos]->m_bHaveData = FALSE;
			buffer->ByteLen = m_pPlayBuffer[m_nOutPos]->ByteLen;
			memcpy(buffer->m_chData,m_pPlayBuffer[m_nOutPos]->m_chData,buffer->ByteLen);			 
			m_nOutPos++;
			if(m_nOutPos > 9)
				m_nOutPos = 0;        
			
			LeaveCriticalSection(&m_cs);
			return TRUE;
		}
		LeaveCriticalSection(&m_cs);
		TRACE("读取失败\n");
	}
	else//从文件中播放
	{
		if(m_pFile)
		{
			if(!m_pFile->Read(buffer))
			{
				// enf of file -> tell the GUI
				OnEndOfPlayingFile();
				return FALSE;
			}
		}
		return TRUE;
	}
	
	return FALSE;
}

void CPipe::StartRecordingToFile()
{
	m_pFile = new CSoundFile(m_strFilePath, m_SoundIn.GetFormat());
	if(m_pFile && m_pFile->IsOK())
		m_SoundIn.Start();
}

void CPipe::StopRecordingToFile()
{
	m_SoundIn.Stop();
	// close output file
	if(m_pFile != NULL)
		delete m_pFile;
}

void CPipe::StartPlayingFromFile(int longtime)
{
	m_pFile = new CSoundFile(m_strFilePath);

	m_SoundOut.m_longtime = longtime;
	if(m_pFile && m_pFile->IsOK())
		m_SoundOut.Start(m_pFile->GetFormat());
	else
	{
		TRACE("Unable to open file");
	}
}

void CPipe::StopPlayingFromFile(char *filename)
{
	m_strFilePath = filename;
	m_SoundOut.Stop();
	// close output file
	if(m_pFile)
	{ 
		delete m_pFile;
		m_pFile = NULL;
	}

	
}

void CPipe::OnEndOfPlayingFile()
{
	// implement this function in the GUI to change things when EOF is reached
}

void CPipe::StartPlayingFromBuffer()
{
	m_nOutPos = 0;
	m_SoundOut.Start(m_SoundOut.GetFormat());
}

void CPipe::StartRecordToBuffer()
{
	m_nInPos = 0;
	for( int  i=0; i<10; i++)
	{
		if(m_pPlayBuffer[i] != NULL)
			delete m_pPlayBuffer[i];
	}
	for(int i=0; i<10; i++)
	{
		m_pPlayBuffer[i] = new CBuffer(m_SoundOut.m_Format.nBlockAlign*m_SoundOut.m_BufferSize);
	}
	m_SoundIn.Start();
}
