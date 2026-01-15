#pragma once

#include <string>
#include <opencv2/core.hpp>
#include <jingmo\cpp\definition.h>
// 定义存储解析数据的结构体
struct PhonemeData {
	double start;
	double end;
	std::string phoneme;
	std::string newstr;
};

#ifdef _WIN32

std::wstring AnsiToUnicode2(const std::string& strAnsi);
#endif

extern char g_rootpath[256];

bool  ISFileExist(char * szFileName);
void ResizeImage(unsigned char *src, int nSrcWidth, int nSrcHeight, unsigned char *dst,
		int nDstWidth, int nDstHeight, int nChannel);


bool ResizeImageGray(const unsigned char *src_im, int src_width, int src_height,
	unsigned char* dst_im, int dst_width, int dst_height);

std::string getgModelpath();
std::vector<std::string> testSplit(std::string srcStr, const std::string& delim);
//记录日志
void cLogInfo(char * info);
void cLogOpen();

int GetStringAlNumCount(std::string strin);
int GetNoNumCount(std::string strin);

int GetStringNum(std::string strin);
int GetFileSize(char * filename);
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
cv::Rect CalculateBox(FaceBox &box, int w, int h, ModelConfig &config);
cv::Rect CalculateBox(cv::Rect inR, int w, int h, ModelConfig &config);

std::vector<PhonemeData> parsePhonemeData(std::vector<std::string> strs);