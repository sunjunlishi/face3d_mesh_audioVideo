#pragma once
/*
	一些特殊效果，羽化，以及旋转操作
	缩放-局部均值图像缩小，避免缩小后的锯齿效果


*/
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>


using namespace std;
struct S_BGR
{
	unsigned B;
	unsigned G;
	unsigned R;
};

cv::Mat PerspectiveTransSmall(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints);


//透视变换
cv::Mat PerspectiveTransTest(cv::Mat I, vector<cv::Point2f> inpts);

void BGR2GrayMy(unsigned char * bgr,int w,int h,unsigned char* gray);

//photoshop中的图层柔光处理
void Soft_Lighten(cv::Mat& src1, cv::Mat& src2,cv::Mat& dst);

//提取图像高光区
void GetTheHighLight(cv::Mat& src,cv::Mat& dstHight);

//进行滤色处理
void Screen(cv::Mat& src1, cv::Mat& src2, cv::Mat& dst);

//锐化操作
cv::Mat sharpenImage0(const cv::Mat &image);

//局部均值图像缩小，避免缩小图片后的锯齿效果
void scalePartAverage(const cv::Mat &src, cv::Mat &dst, int dstcols, int dstrows);

//逆时针90，顺时针90度，垂直镜像，3中转转
void Rotate90_180(cv::Mat& src,cv::Mat& dst,int type);
//旋转任意角度
cv::Mat rotateImage2(cv::Mat& img, int degree,int bkcolor = 0);  

cv::Mat rotateImageC(cv::Mat& img, int degree, int bkcolor = 0);

cv::Point_<double>  GetNewPoint(cv::Point_<double> in, double angle, cv::Point_<double> center);

/*
进行旋转操作
*/
void RotateMatAndPts(std::vector<cv::Point>& mainPts, cv::Mat& clmResult, double angle);
void RotateMatAndPts(std::vector<cv::Point2f>& mainPts,cv::Mat& clmResult, double angle);
bool Content2RectInHorizon(cv::Rect& r1, cv::Rect& r2, int disCha);

//对下巴进行调整
void ReAjustByJawPosSmall(cv::Mat& src, int JawPosY);

//产生高斯核
cv::Mat gaussian_kernal(int dim,double sigma);

//连接水平区域
void CombineHorizonAddres(std::vector<cv::Rect>& outRect, int disW = 15);

void CombineHorizonWithLm(std::vector<cv::Rect>& outRect, int disW = 15);

//求相交区域大小
float intersectRect(const cv::Rect& rectA, const cv::Rect& rectB, cv::Rect& intersectRect);

float intersectRectMax(const cv::Rect& rectA, const cv::Rect& rectB,cv::Rect& comBineRe);

//合并最大重叠区域
void CombineRectMaxContent(std::vector<cv::Rect>& outRect, float fa);

void CombineRectContentHorizon(std::vector<cv::Rect_<float>>& outRect, int discha);
bool Content2RectInHorizon(cv::Rect_<float>& r1, cv::Rect_<float>& r2, int disCha);

int GetLightValue(cv::Mat img, int& outmax, int& outmin);

// get img blur value
int GetImgBlurValue(cv::Mat img);

std::vector<cv::Point> GetBdCorners(std::vector<cv::Point> ptsf, bool& bOutOk);
std::vector<cv::Point> GetLtBdCorners(std::vector<cv::Point> ptsf, bool& bOutOk);
std::vector<cv::Point2f> GetBdCorners(std::vector<cv::Point2f> ptsf, bool& bOutOk);
//四个角进行纠正
cv::Mat GetOutIdentiByPts(std::vector<cv::Point> inpts, cv::Mat inImg);


cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R);