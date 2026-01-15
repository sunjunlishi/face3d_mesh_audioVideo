#ifndef _CFilter_H_
#define _CFilter_H_

#define SIZE_AUDIO_FRAME 2048  //==BUFFER_SIZE*采集声道数 BUFFER_SIZE=2048  单通道
class CFilter
{
private:
	unsigned short wBitsPerSample;
	HWND m_hWnd;
    
public:
	CFilter();
	~CFilter();

	void Init(HWND hWnd,unsigned short wBits);
	int DrawWave(char* pBuf);
	//int FilterOut(char* pBuf);
};

#endif