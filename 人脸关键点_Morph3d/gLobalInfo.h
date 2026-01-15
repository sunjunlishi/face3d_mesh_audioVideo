#pragma once

#include <string>
#include <opencv2/core.hpp>
extern char g_rootpath[256];

bool  ISFileExist(char * szFileName);
void ResizeImage(unsigned char *src, int nSrcWidth, int nSrcHeight, unsigned char *dst,
		int nDstWidth, int nDstHeight, int nChannel);


bool ResizeImageGray(const unsigned char *src_im, int src_width, int src_height,
	unsigned char* dst_im, int dst_width, int dst_height);

std::string getgModelpath();

//记录日志
void cLogInfo(char * info);
void cLogOpen();

int GetStringAlNumCount(std::string strin);
int GetNoNumCount(std::string strin);

int GetStringNum(std::string strin);

int GetHanziLen();
//设置模型和so所在的文件路径
void SetModelPath(char * modelpath);


//Buf2img  in memory
bool  CvBufMem2img(char * in, int len, cv::Mat& outimg);

//gen random num by timenow
int GetRandMy();
int GetRandT();
void SetGpuID(int gpuID);

void SetGpuIDstr(char * ids);
