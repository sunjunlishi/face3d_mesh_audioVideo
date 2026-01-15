// SoundBase.h: interface for the CSoundBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
#define AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define HERTZ_POOR		8000
#define HERTZ_LOW		11025
#define HERTZ_NORMAL	22050
#define HERTZ_HIGH		16000

#include <mmsystem.h>
#include "Buffer.h"

class CSoundBase  
{
public:
	WAVEFORMATEX* GetFormat();
	int GetBufferSize();
	void SetBufferSize(int NumberOfSamples);
	int GetNumberOfChannels();
	void SetNumberOfChannels(int nchan);
	int GetSamplesPerSecond();
	void SetSamplesPerSecond(int sps);
	int GetBitsPerSample();
	void SetBitsPerSample(int bps);
	CSoundBase();
	virtual ~CSoundBase();
public:
	WAVEFORMATEX m_Format;
	int			 m_BufferSize;	// number of samples

private:
	void Update();
};

#endif // !defined(AFX_SOUNDBASE_H__041D24CC_3A2F_4E33_AAB9_6C98297DAC59__INCLUDED_)
