
#include <stdio.h>
#include "gLobalInfo.h"
#include <vector>
 #include <string.h> 
#include <opencv2/highgui.hpp>
#include <time.h>
#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include<locale>

#ifdef _WIN32
char g_rootpath[256] = "./";
//#include <debugapi.h>
#else 
#ifdef ANDROID
char  g_rootpath[256] = "./";
#else
char g_rootpath[256]="./";
#endif
#endif




#define __assert2(_Expression)     ((void)0)


std::string gcascadeModePath = "./";
std::string getgModelpath() {

	return gcascadeModePath;

}

int gNeedClog =0;
int gGpuCardID = -1;
void cLogOpen()
{
	gNeedClog = 1;
}
void cLogInfo(char * info)
{
	if (gNeedClog)
	{
		printf("log path %s len %d\n", (getgModelpath() + "/aglog.txt").c_str(), strlen(info));
		FILE * fp = fopen((getgModelpath()+"/aglog.txt").c_str(), "ab+");
		if (fp)
		{
			fwrite(info, 1, strlen(info), fp);
			fwrite("\n", 1, strlen("\n"), fp);
			fclose(fp);
		}
	}
}

 int GetHanziLen() { 
#ifdef _WIN32 
	 return 2;
#else
	 return 3;
#endif

 }



#ifdef _WIN32


 std::wstring AnsiToUnicode2(const std::string& strAnsi)
 {



	 return L"";

 }
#endif
 std::vector<std::string> testSplit(std::string srcStr, const std::string& delim)
 {
	 int nPos = 0;
	 std::vector<std::string> vec;
	 nPos = srcStr.find(delim.c_str());
	 while (-1 != nPos)
	 {
		 std::string temp = srcStr.substr(0, nPos);
		 vec.push_back(temp);
		 srcStr = srcStr.substr(nPos + 1);
		 nPos = srcStr.find(delim.c_str());
	 }
	 vec.push_back(srcStr);
	 return vec;
 }

int GetNoNumCount(std::string strin)
{
	int iCount = 0;
	if (strin.empty()) return iCount;
	for (int k = 0; k < strlen(strin.c_str()); ++k)
	{
		if (!isalnum(strin.c_str()[k]))
		{
			iCount++;
		}
	}
	return iCount;
}

int GetFileSize(char * filename)
{
	FILE *file;
	long size;

	file = fopen(filename, "rb"); // 替换为所需文件路径
	if (file == NULL) {
		// 处理文件打开失败的情况
	}

	fseek(file, 0L, SEEK_END);
	size = ftell(file);

	fclose(file);
	return size;
}

int GetStringAlNumCount(std::string strin)
{
	int iCount = 0;
	if (strin.empty()) return iCount;
	for (int k = 0; k < strlen(strin.c_str()); ++k)
	{
		if (isalnum(strin.c_str()[k]))
		{
			iCount++;
		}
	}
	return iCount;
}

int GetStringNum(std::string strin)
{
	int iCount = 0;
	if (strin.empty()) return iCount;
	for (int k = 0; k < strlen(strin.c_str()); ++k)
	{
		if (strin[k] >= '0' && strin[k] <= '9')
		{
			iCount++;
		}
	}
	return iCount;
}




//模型和so所在的文件路径
void SetModelPath(char * modelpath)
{
#ifdef _WINJAVA
	// std::wstring wpath = Utf8ToWString(modelpath);
	// gcascadeModePath = wstringToString(wpath);
	gcascadeModePath = modelpath;
	printf("pathname*******************  %s\n", modelpath);
#else 
	gcascadeModePath = modelpath;
	strcpy(g_rootpath, modelpath);
#endif


}


bool  CvBufMem2img(char * in, int len, cv::Mat& outimg)
{
	CvMat mat = cvMat(5000, 4000, CV_8UC3, in);

	if (len > 1024 * 1024 *7)
	{
		mat = cvMat(12000, 12000, CV_8UC3, in);
	}
	IplImage *pIplImage = cvDecodeImage(&mat, 1);
	if (pIplImage != NULL)
	{
		cv::Mat mtx(pIplImage);
		outimg = mtx.clone();
		cvReleaseImage(&pIplImage);
	}
	
	
	if (outimg.empty())
	{
		return false;
	}
	return true;
}
bool  ISFileExist(char * szFileName)
{
	FILE * pfile = fopen(szFileName,"r");
	if(pfile != NULL)
	{
		fclose(pfile);
		return true;
	}
	return false;
}




cv::Rect CalculateBox(FaceBox &box, int w, int h, ModelConfig &config)
{
	int x = static_cast<int>(box.x1);
	int y = static_cast<int>(box.y1);
	int box_width = static_cast<int>(box.x2 - box.x1 + 1);
	int box_height = static_cast<int>(box.y2 - box.y1 + 1);

	int shift_x = static_cast<int>(box_width * config.shift_x);
	int shift_y = static_cast<int>(box_height * config.shift_y);

	float scale = (std::min)(
		config.scale,
		(std::min)((w - 1) / (float)box_width, (h - 1) / (float)box_height)
		);

	int box_center_x = box_width / 2 + x;
	int box_center_y = box_height / 2 + y;

	int new_width = static_cast<int>(box_width * scale);
	int new_height = static_cast<int>(box_height * scale);

	int left_top_x = box_center_x - new_width / 2 + shift_x;
	int left_top_y = box_center_y - new_height / 2 + shift_y;
	int right_bottom_x = box_center_x + new_width / 2 + shift_x;
	int right_bottom_y = box_center_y + new_height / 2 + shift_y;

	if (left_top_x < 0) {
		right_bottom_x -= left_top_x;
		left_top_x = 0;
	}

	if (left_top_y < 0) {
		right_bottom_y -= left_top_y;
		left_top_y = 0;
	}

	if (right_bottom_x >= w) {
		int s = right_bottom_x - w + 1;
		left_top_x -= s;
		right_bottom_x -= s;
	}

	if (right_bottom_y >= h) {
		int s = right_bottom_y - h + 1;
		left_top_y -= s;
		right_bottom_y -= s;
	}

	return cv::Rect(left_top_x, left_top_y, new_width, new_height);
}

cv::Rect CalculateBox(cv::Rect inR, int w, int h, ModelConfig &config)
{

	FaceBox box;
	box.x1 = inR.x;
	box.x2 = inR.x + inR.width;
	box.y1 = inR.y;
	box.y2 = inR.y + inR.height;

	return CalculateBox(box, w, h, config);
}


bool ResizeImageGray(const unsigned char *src_im, int src_width, int src_height,
	unsigned char* dst_im, int dst_width, int dst_height)
{

	float	lfx_scl, lfy_scl;
	if (src_width == dst_width && src_height == dst_height) {
		memcpy(dst_im, src_im, src_width * src_height * sizeof(unsigned char));
		return true;
	}

	lfx_scl = float(src_width + 0.0) / dst_width;
	lfy_scl = float(src_height + 0.0) / dst_height;

	for (int n_y_d = 0; n_y_d < dst_height; ++n_y_d) {
		for (int n_x_d = 0; n_x_d < dst_width; ++n_x_d) {
			float lf_x_s = lfx_scl * n_x_d;
			float lf_y_s = lfy_scl * n_y_d;

			int n_x_s = int(lf_x_s);
			n_x_s = (n_x_s <= (src_width - 2) ? n_x_s : (src_width - 2));
			int n_y_s = int(lf_y_s);
			n_y_s = (n_y_s <= (src_height - 2) ? n_y_s : (src_height - 2));

			float lf_weight_x = lf_x_s - n_x_s;
			float lf_weight_y = lf_y_s - n_y_s;

			double lf_new_gray = (1 - lf_weight_y) * ((1 - lf_weight_x) * src_im[n_y_s * src_width + n_x_s] +
				lf_weight_x * src_im[n_y_s * src_width + n_x_s + 1]) +
				lf_weight_y * ((1 - lf_weight_x) * src_im[(n_y_s + 1) * src_width + n_x_s] +
					lf_weight_x * src_im[(n_y_s + 1) * src_width + n_x_s + 1]);

			dst_im[n_y_d * dst_width + n_x_d] = (unsigned char)(lf_new_gray);
		}
	}
	return true;
}
int min_int(int x, int y) { return (x <= y ? x : y); }
int max_int(int x, int y) { return (x <= y ? y : x); }
void ResizeImage(unsigned char *src, int nSrcWidth, int nSrcHeight, unsigned char *dst, int nDstWidth, int nDstHeight, int nChannel)
{

	int step1 = nSrcWidth*nChannel;
	int step2 = nDstWidth*nChannel;
	int j;
	int i;
	int k;

	unsigned char* data1 = (unsigned char*)src;
	unsigned char* data2 = (unsigned char*)dst;

	long xrIntFloat_16 = ((nSrcWidth) << 16) / nDstWidth + 1;
	long yrIntFloat_16 = ((nSrcHeight) << 16) / nDstHeight + 1;
	const long csDErrorX = -(1 << 15) + (xrIntFloat_16 >> 1);
	const long csDErrorY = -(1 << 15) + (yrIntFloat_16 >> 1);

	long srcy_16 = csDErrorY;

	for (j = 0; j < nDstHeight; j++)
	{

		long srcx_16 = csDErrorX;
		unsigned long v_8 = (srcy_16 & 0xFFFF) >> 8;
		long kq = srcy_16 >> 16;
		kq = max_int(0, kq);
		kq = min_int(kq, nSrcHeight - 2);

		for (i = 0; i < nDstWidth; i++)
		{

			unsigned long u_8 = (srcx_16 & 0xFFFF) >> 8;
			unsigned long pm3_16 = (u_8*v_8);
			unsigned long pm2_16 = (u_8*(unsigned long)(256 - v_8));
			unsigned long pm1_16 = (v_8*(unsigned long)(256 - u_8));
			unsigned long pm0_16 = ((256 - u_8)*(256 - v_8));

			long kp = srcx_16 >> 16;
			kp = max_int(0, kp);
			kp = min_int(kp, nSrcWidth - 2);

			for (k = 0; k<3; k++)
			{
				data2[j*step2 + i * 3 + k] = (pm0_16*data1[kq*step1 + kp * 3 + k] + pm1_16*data1[(kq + 1)*step1 + kp * 3 + k] +
					pm2_16*data1[kq*step1 + (kp + 1) * 3 + k] + pm3_16*data1[(kq + 1)*step1 + (kp + 1) * 3 + k]) >> 16;

			}
			srcx_16 += xrIntFloat_16;

		}
		srcy_16 += yrIntFloat_16;

	}

}




int strsplitcommon(const std::string& str, std::vector<std::string>& ret_, std::string sep)
{
	if (str.empty())
	{
		return 0;
	}

	std::string tmp;
	std::string::size_type pos_begin = str.find_first_not_of(sep);
	std::string::size_type comma_pos = 0;

	while (pos_begin != std::string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != std::string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			ret_.push_back(tmp);
			tmp.clear();
		}
	}
	return 0;
}
int allGpuCount = 0;
void SetGpuIDstr(char * ids)
{
	printf("**************inids %s *************\n", ids);
	std::string strid = ids;
	std::vector<std::string> outs;
	strsplitcommon(strid, outs, ",");
	if (outs.size() > 1)
	{
		allGpuCount = outs.size();
	}
	else
	{
		if (strid.size() > 0)
			gGpuCardID = atoi(ids);
	}

	printf("**************gpucardid %d *************\n", gGpuCardID);
}

void SetGpuID(int gpuID)
{
	gGpuCardID = gpuID;
}
int GetRandT()
{
	std::default_random_engine e(clock());
	std::uniform_int_distribution<unsigned> u(0,1000);
	return u(e);
}


int GetRandMy()
{
	int iValue = 0;
	//first rand
	if (allGpuCount > 1)
	{
		std::default_random_engine e(clock());
		std::uniform_int_distribution<unsigned> u(0, 100);
		iValue = u(e) % allGpuCount;
	}
	else
	{
		//else   gGpuCardID  or 0
		if (gGpuCardID == -1)
		{
			iValue =  0;
		}
		else
			iValue =gGpuCardID;
	}

	printf("now ivalue gpu is  ****************************** %d\n", iValue);
	return iValue;

}

// 主解析函数
std::vector<PhonemeData> parsePhonemeData(std::vector<std::string> strs ) {

	std::vector<PhonemeData> results;
	std::string line;

	for(int k=0;k<strs.size();k++)
	{
		line = strs[k];
		// 跳过空行和结束标记
		if (line.empty() || line == "." || line == " ." || line.find("audio") != -1) continue;

		std::istringstream iss(line);
		PhonemeData data;

		// 解析两个浮点数
		if (!(iss >> data.start >> data.end)) {
			// 处理可能的格式错误 [2,6](@ref)
			if (line.find("sil") != std::string::npos) {
			//	continue;  // 特殊处理包含"sil"的无效行
			}
			throw std::runtime_error("浮点数解析错误: " + line);
		}

		// 提取剩余部分作为音素字符串
		std::string remaining;
		std::getline(iss, remaining);

		// 清理首尾空格 [7](@ref)
		size_t first = remaining.find_first_not_of(' ');
		size_t last = remaining.find_last_not_of(' ');
		data.phoneme = (first == std::string::npos) ? "" : remaining.substr(first, (last - first + 1));

		// 验证数据有效性
		if (data.start < 0 || data.end <= data.start || data.phoneme.empty()) {
			throw std::runtime_error("数据验证失败: " + line);
		}

		results.push_back(std::move(data));  // 使用移动语义提高效率 [3,4](@ref)
	}

	return results;
}
