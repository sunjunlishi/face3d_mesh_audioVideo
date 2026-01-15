#include "SpecialEfect.h"
#include <algorithm>
#include <time.h>

#ifdef _WIN32
#include <opencv2/highgui.hpp>
#endif
using namespace cv;

struct S_BGRA
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
};
cv::Mat PerspectiveTransSmall(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints)
{
	cv::Mat dst;
	cv::Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
	warpPerspective(src, dst, Trans, cv::Size(dstPoints[3].x, dstPoints[3].y), CV_INTER_CUBIC);
	return dst;
}

cv::Mat PerspectiveTrans(cv::Mat src, cv::Point2f* scrPoints, cv::Point2f* dstPoints)
{
	cv::Mat dst;
	cv::Mat Trans = getPerspectiveTransform(scrPoints, dstPoints);
	warpPerspective(src, dst, Trans, cv::Size(640, 400), CV_INTER_CUBIC,0, cv::Scalar(255,255,255));
	return dst;
}



// get img blur value
int GetImgBlurValue(cv::Mat img1)
{
	cv::Mat img;
	cv::resize(img1, img, cv::Size(168, 190));

	cv::Mat gray, dst, abs_dst;
	//高斯滤波消除噪声
	GaussianBlur(img, img, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
	//转换为灰度图
	cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	//使用Laplace函数
	//第三个参数：目标图像深度；第四个参数：滤波器孔径尺寸；第五个参数：比例因子；第六个参数：表示结果存入目标图
	Laplacian(gray, dst, CV_16S, 3, 1, 0, cv::BORDER_DEFAULT);
	//计算绝对值，并将结果转为8位
	convertScaleAbs(dst, abs_dst);

	int isum = 0;
	for (int i = 0; i < abs_dst.rows; ++i)
	{
		for (int j = 0; j < abs_dst.cols; ++j)
		{
			isum += abs_dst.data[abs_dst.cols*i + j];
		}
	}
	return isum;
}





int GetLightValue(cv::Mat img, int& outmax, int& outmin)
{
	cv::Mat gray;
	cvtColor(img, gray, CV_BGR2GRAY);
	cv::resize(gray, gray, cv::Size(90, 90));
	int spanx = gray.cols / 3;
	int spany = gray.rows / 3;
	//cv::imwrite("D:/out.jpg",gray);
	int max = 0;
	int min = 1900000;
	for (int m = 0; m < 3; ++m)
	{
		int step = m*gray.cols*spany;
		for (int n = 0; n < 3; ++n)
		{
			int iSum = 0;

			int jump = n*spanx;
			for (int i = 0; i < spany; ++i)
			{
				for (int j = 0; j < spanx; ++j)
				{
					iSum += gray.data[(i + m*spany)*gray.cols + jump + j];
				}
			}

			if (iSum < min)
			{
				min = iSum;
			}
			if (iSum > max)
			{
				max = iSum;
			}
		}
	}


	outmax = max / (spanx*spany);
	outmin = min / (spanx*spany);

	return 0;
}




//cv::Mat PerspectiveTransSmall(Mat I, vector<Point2f> inpts)
//{
//
//
//	Point2f AffinePoints0[4] = { inpts[0], inpts[1], inpts[2], inpts[3] };
//	Point2f AffinePoints1[4] = { Point2f(0, 0), Point2f(144, 0), Point2f(144, 38), Point2f(0, 38) };
//
//	Mat dst_perspective = PerspectiveTransSmall(I, AffinePoints0, AffinePoints1);
//
//	for (int i = 0; i < 4; i++)
//	{
//		//circle(I, AffinePoints0[i], 2, Scalar(0, 0, 255), 2);
//		//circle(dst_perspective, AffinePoints1[i], 2, Scalar(0, 0, 255), 2);
//	}
//
//	return dst_perspective;
//}


bool ContentInHorizonAddres(cv::Rect& r1, cv::Rect& r2, int disW)
{


	int cr1 = r1.x + r1.width;


	int cr2 = r2.x + r2.width;

	int cy1 = r1.y + r1.height / 2;
	int cy2 = r2.y + r2.height / 2;

	//判断两个矩形中心以及 水平距离 在一个 范围内
	if (((cr1>r2.x - disW && cr1 < cr2) || (cr2>r1.x - disW && cr2 < cr1))
		&& ((cy1 >r2.y && cy1 <(r2.y + r2.height*1.05)) || (cy2 >r1.y && cy2 <(r1.y + r1.height*1.05))))
		return true;


	return false;

}
void CombineHorizonWithLm(std::vector<cv::Rect>& outRect, int disW)
{
	int iCount = outRect.size();

	int index = 0;
	for (int k = 0; k < iCount; ++k)
	{
		char szBuf[256];

		for (int j = outRect.size() - 1; j > index; j--)
		{
			sprintf(szBuf, "%d %d %d %d; %d %d %d %d",
				outRect[index].x, outRect[index].y, outRect[index].width, outRect[index].height,
				outRect[j].x, outRect[j].y, outRect[j].width, outRect[j].height);
			if (ContentInHorizonAddres(outRect[index], outRect[j], disW)
				&& outRect[index].height*1.0/ outRect[j].height > 0.8 
				&& outRect[index].height*1.0 / outRect[j].height < 1.3)
			{

				int minX = min(outRect[index].x, outRect[j].x);
				int minY = min(outRect[index].y, outRect[j].y);

				int maxX = max(outRect[index].x + outRect[index].width, outRect[j].x + outRect[j].width);

				int y1 = outRect[index].y + outRect[index].height;
				int y2 = outRect[j].y + outRect[j].height;
				int maxY = max(y1, y2);

				//if (maxY - minY > 150) continue;
				//
				outRect[index].x = minX; outRect[index].y = minY;
				outRect[index].width = maxX - minX; outRect[index].height = maxY - minY;
				outRect.erase(outRect.begin() + j);
			}

		}
		index++;

	}
}

void CombineHorizonAddres(std::vector<cv::Rect>& outRect, int disW)
{
	int iCount = outRect.size();

	int index = 0;
	for (int k = 0; k < iCount; ++k)
	{
		char szBuf[256];

		for (int j = outRect.size() - 1; j > index; j--)
		{
			sprintf(szBuf, "%d %d %d %d; %d %d %d %d",
				outRect[index].x, outRect[index].y, outRect[index].width, outRect[index].height,
				outRect[j].x, outRect[j].y, outRect[j].width, outRect[j].height);
			if (ContentInHorizonAddres(outRect[index], outRect[j], disW))
			{

				int minX = min(outRect[index].x, outRect[j].x);
				int minY = min(outRect[index].y, outRect[j].y);

				int maxX = max(outRect[index].x + outRect[index].width, outRect[j].x + outRect[j].width);

				int y1 = outRect[index].y + outRect[index].height;
				int y2 = outRect[j].y + outRect[j].height;
				int maxY = max(y1, y2);

				//if (maxY - minY > 150) continue;
				//
				outRect[index].x = minX; outRect[index].y = minY;
				outRect[index].width = maxX - minX; outRect[index].height = maxY - minY;
				outRect.erase(outRect.begin() + j);
			}

		}
		index++;

	}
}


//透视变换测试
cv::Mat PerspectiveTransTest(cv::Mat I, vector<cv::Point2f> inpts)
{

	cv::Point2f AffinePoints0[4] = { inpts[0], inpts[1], inpts[2], inpts[3] };
	cv::Point2f AffinePoints1[4] = { cv::Point2f(0, 0), cv::Point2f(640, 0), cv::Point2f(640, 400), cv::Point2f(0, 400) };
	cv::Mat dst_perspective = PerspectiveTrans(I, AffinePoints0, AffinePoints1);

	return dst_perspective;
}



void ReAjustByJawPosSmall(cv::Mat& src,int JawPosY)
{
	if(src.cols > 0)
	{
		Mat tmpMat(src.rows,src.cols,src.type());
		tmpMat.setTo(0);

		int destYPos = 140;
		int jawPos = JawPosY*0.278;
		//
		if(jawPos < (destYPos))
		{
		
			int cha = destYPos - jawPos;
			memcpy(tmpMat.data+src.step[0]*cha,src.data,(src.step[0]*(src.rows-cha)));
		
			src = tmpMat;
		}

	
	}
	
}


void average4b(const cv::Mat &img, cv::Point_<int> a, cv::Point_<int> b, Vec4b &p)
{
	if(a.x >=0 && a.x < img.cols && b.x >= 0 && b.x < img.cols
		&& a.y >=0 && a.y < img.rows && b.y >=0 && b.y < img.rows)
	{
		  const Vec4b *pix;
			Vec4i temp;
		    temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			temp[3] = 0;
			for (int i = a.y; i <= b.y; i++){
				pix = img.ptr<Vec4b>(i);
				for (int j = a.x; j <= b.x; j++){
					temp[0] += pix[j][0];
					temp[1] += pix[j][1];
					temp[2] += pix[j][2];
					temp[3] += pix[j][3];
				}
			}

			int count = (b.x - a.x+1 ) * (b.y - a.y+1 );
			if(count > 0)
			{
				p[0] = temp[0] / count;
				p[1] = temp[1] / count;
				p[2] = temp[2] / count;
				p[3] = temp[3] / count;
			}
	}
  
   
}

void average3b(const Mat &img, Point_<int> a, Point_<int> b, Vec3b &p)
{
	if(a.x < img.rows && b.x < img.rows && b.y < img.cols && a.y < img.cols)
	{
		 const Vec3b *pix;
			Vec3i temp;
			for (int i = a.x; i <= b.x; i++){
				pix = img.ptr<Vec3b>(i);
				for (int j = a.y; j <= b.y; j++){
					temp[0] += pix[j][0];
					temp[1] += pix[j][1];
					temp[2] += pix[j][2];
			
				}
			}

			int count = (abs(b.x - a.x) + 1) * (abs(b.y - a.y) + 1);
		    if(count != 0)
			{
				p[0] = temp[0] / count;
				p[1] = temp[1] / count;
				p[2] = temp[2] / count;
			}
			else
			{
				p[0] = temp[0] ;
				p[1] = temp[1];
				p[2] = temp[2];
			}
	

	}
		 
   
}

void scalePartAverage4B(const Mat &src, Mat &dst, int cols, int rows)
{
  
	double yRatio = rows*1.0/src.rows;
	double xRatio = cols*1.0/src.cols;
	dst.create(rows, cols, src.type());
	
    int lastRow = 0;
    int lastCol = 0;

    Vec4b *p;
    for (int i = 0; i < rows; i++) {

        p = dst.ptr<Vec4b>(i);
        int row = static_cast<int>((i + 1) / yRatio + 0.5) - 1;
		if(i == 106)
		{
			int k = 0;
		}
        for (int j = 0; j < cols; j++) {
            int col = static_cast<int>((j + 1) / xRatio + 0.5) - 1;

			
            Vec4b pix;
            average4b(src, Point_<int>(lastCol,lastRow ), Point_<int>(col, row), pix);

            p[j] = pix;
            lastCol = col + 1; //下一个子块左上角的列坐标，行坐标不变
        }
        lastCol = 0; //子块的左上角列坐标，从0开始
        lastRow = row + 1; //子块的左上角行坐标
    }
}
void scalePartAverage3B(const Mat &src, Mat &dst, int cols, int rows)
{
  
	double xRatio = rows*1.0/src.rows;
	double yRatio = cols*1.0/src.cols;
	dst.create(rows, cols, src.type());
	
    int lastRow = 0;
    int lastCol = 0;

    Vec3b *p;
    for (int i = 0; i < rows; i++) {

        p = dst.ptr<Vec3b>(i);
        int row = static_cast<int>((i + 1) / xRatio + 0.5) - 1;

        for (int j = 0; j < cols; j++) {
            int col = static_cast<int>((j + 1) / yRatio + 0.5) - 1;

			
            Vec3b pix;
            average3b(src, Point_<int>(lastRow, lastCol), Point_<int>(row, col), pix);

            p[j] = pix;
            lastCol = col + 1; //下一个子块左上角的列坐标，行坐标不变
        }
        lastCol = 0; //子块的左上角列坐标，从0开始
        lastRow = row + 1; //子块的左上角行坐标
    }
}
Mat gaussian_kernal(int dim,double sigma)
{
	int c = dim / 2;
	Mat K(dim, dim, CV_32FC1);
	//生成二维高斯核
	float s2 = 2.0 * sigma * sigma;
	for(int i = (-c); i <= c; i++)
	{
		int m = i + c;
		for (int j = (-c); j <= c; j++)
		{
			int n = j + c;
			float v = exp(-(1.0*i*i + 1.0*j*j) / s2);
			K.ptr<float>(m)[n] = v;
		}
	}
	Scalar all = sum(K);
	Mat gaussK;
	K.convertTo(gaussK, CV_32FC1, (1/all[0]));
	all = sum(gaussK);
	return gaussK;
}


//int GetValueSpan(const Mat& bigMat,int i,int j,double spanX,double spanY,int depth,int index)
// {
//	 float sum = 0.0; 
//	 int jump = j*depth*spanX;
//
//	 int ispanX = spanX+0.5;
//	 int ispanY = spanY+ 0.5;
//	 for(int m = 0;m<ispanY;++m)
//	 {
//		 int step = bigMat.step[0]*(i*spanY+0.5+m);
//		if((i*spanY+m) < bigMat.rows)
//		 {
//			 int jump1=jump;
//			 for(int n = 0;n<ispanX;++n)
//			 {
//				 if(jump1/depth < bigMat.cols)
//				 {
//					 sum+= bigMat.data[step+jump1+0.5+index];
//					 jump1+= depth;
//				 }
//				
//			 }
//		 }
//		
//	 }
//	 return sum/(ispanY*ispanX);
// }
void scalePartAverage(const Mat &src, Mat &dst,int cols ,int rows )
{
	if(src.type() == CV_8UC4)
	{
		scalePartAverage4B(src,dst,cols,rows);
	}
	else
	{
		scalePartAverage3B(src,dst,cols,rows);
	}
}



void Soft_Lighten(Mat& src1, Mat& src2, Mat& dst)
{
	
    float a=0;
    float b=0;
     for(int index_row=0; index_row<src1.rows; index_row++)
    {
        for(int index_col=0; index_col<src1.cols; index_col++)
        {
            for(int index_c=0; index_c<3; index_c++)
            {
                a=src1.at<Vec3b>(index_row, index_col)[index_c]*1.0/255;
                b=src2.at<Vec3b>(index_row, index_col)[index_c]*1.0/255;
				int iValue = 0;
                if(a<=0.5)
                {
					iValue = ((2*a-1)*(b-b*b)+b)*255;
					iValue = (iValue>255)?255:iValue;
					
					dst.at<Vec3b>(index_row, index_col)[index_c]=iValue;
                }
                else
                {
					iValue = ( (2*a-1)*(sqrt(b)-b)+b)*255;
					iValue = (iValue>255)?255:iValue;
					dst.at<Vec3b>(index_row, index_col)[index_c]=iValue;
                }
            }
        }
    }
}


//提取图像高光区
void GetTheHighLight(Mat& src,Mat& dstHight)
{
	dstHight = Mat(src.rows,src.cols,CV_8UC4);
	for(int i=0;i<src.rows;++i)
		for(int j=0;j<src.cols;++j)
		{
			dstHight.data[dstHight.step[0]*i+j*4+0] = 255;
			dstHight.data[dstHight.step[0]*i+j*4+1] = 255;
			dstHight.data[dstHight.step[0]*i+j*4+2] = 255;
			dstHight.data[dstHight.step[0]*i+j*4+3] = 
				(src.data[src.step[0]*i+3*j+0]+src.data[src.step[0]*i+3*j+1]+src.data[src.step[0]*i+3*j+2])/3;
		}
}

Mat WhitePic(Mat& src)
{
	Mat blurImg;
	//blur(src,blurImg,Size(5,5));
	Mat dstImg = src.clone();
	Soft_Lighten(blurImg,src,dstImg);

	Mat dstHeigh;
	GetTheHighLight(src,dstHeigh);

	//MixLayer(dstHeigh,dstImg,0.3);

	return dstImg;
}

//滤色
void Screen(Mat& src1, Mat& src2, Mat& dst)
{
     for(int index_row=0; index_row<src1.rows; index_row++)
    {
        for(int index_col=0; index_col<src1.cols; index_col++)
        {
	
			float a1 = src1.data[src1.step[0]*index_row+index_col*3]*1.0/255;
			float b1 = src2.data[src2.step[0]*index_row+index_col*3]*1.0/255;

			dst.data[dst.step[0]*index_row+index_col*3] = (1-(1-a1)*(1-b1))*255;

			a1 = src1.data[src1.step[0]*index_row+index_col*3+1]*1.0/255;
			b1 = src2.data[src2.step[0]*index_row+index_col*3+1]*1.0/255;
			dst.data[dst.step[0]*index_row+index_col*3+1] = (1-(1-a1)*(1-b1))*255;


			a1 = src1.data[src1.step[0]*index_row+index_col*3+2]*1.0/255;
			b1 = src2.data[src2.step[0]*index_row+index_col*3+2]*1.0/255;
			dst.data[dst.step[0]*index_row+index_col*3+2] = (1-(1-a1)*(1-b1))*255;

			
       
        }
    }
}

Mat WhitePicReturnU3(Mat& src,vector<Point> mainPts)
{
	//进行处理的时候，首先要对像素点的范围进行判断，对美白的强度进行调整
	Point ptEyeLeft = mainPts[0];
	Point ptEyeRight = mainPts[1];

	Point ptNose = mainPts[2];
	Point ptMouth = mainPts[3];

	//
	int wValue = 0;
	int iCount = 0;
	for(int i=ptNose.y-(ptNose.y-ptEyeRight.y)*0.3;i<ptMouth.y;++i)
	{
		for(int j=ptEyeLeft.x;j<ptEyeRight.x;++j)
		{
			wValue+= src.data[src.step[0]*i+j*3];
			wValue+= src.data[src.step[0]*i+j*3+1];
			wValue+= src.data[src.step[0]*i+j*3+2];
			
			iCount++;
		}
		
	}
	float alpha = 1.0;
	int iResult = wValue/iCount/3;

	
	if(iResult > 190)
	{
		alpha = 0.1;
	}
	else if(iResult > 180)
	{
		alpha = 0.2;
	}
	else if(iResult > 170)
	{
		alpha = 0.3;
	}
	else if(iResult > 160)
	{
		alpha = 0.6;
	}
	else if(iResult > 150)
	{
		alpha = 0.8;
	}



	Mat blurImg;
	//blur(src,blurImg,Size(5,5));
	Mat dstImg = src.clone();
	Soft_Lighten(blurImg,src,dstImg);
	cvtColor(dstImg,dstImg,CV_BGR2BGRA);

	Mat dstHeigh;
	GetTheHighLight(src,dstHeigh);

	//MixLayer(dstHeigh,dstImg,alpha);

	cvtColor(dstImg,dstImg,CV_BGRA2BGR);

	return dstImg;
}



void Rotate90_180(Mat& src,Mat& dst,int type)
{
	int jump = 3;
	if(src.type() == CV_8UC1)
	{
		jump = 1;
	}
	int width=src.cols;
	int height=src.rows;
	
	if (-1 == type)//逆时针旋转90
	{
		dst = cv::Mat(src.cols,src.rows,src.type());
          unsigned char *ptrSrc=(unsigned char *)(src.data);
	unsigned char *ptrDst=(unsigned char *)(dst.data);
		for (int j=width;j>0;j--)
		{
			int n = 0;
			int step1 = dst.step[0]*(width-j);
			for (int i=0;i<height;i++)
			{
				memcpy((void *)(ptrDst+n+step1),(void *)(ptrSrc+src.step[0]*i+(j-1)*jump),jump);
				n=n+jump;
			}
		}
	}
	else if(type == 1)//顺时针旋转90度
	{
		dst = cv::Mat(src.cols, src.rows, src.type());
      unsigned char *ptrSrc=(unsigned char *)(src.data);
	unsigned char *ptrDst=(unsigned char *)(dst.data);
		for (int j=1;j<=width;j++)
		{
			int n = 0;
			int step1 = dst.step[0]*(j-1);
			for (int i=height;i>0;i--)
			{
				memcpy((void *)(ptrDst+step1+n),(void *)(ptrSrc+src.step[0]*(i-1)+(j-1)*jump),jump);
				n=n+jump;
			}
		}
	}
	else if (type == 0)
	{
		dst = cv::Mat(src.rows,src.cols,src.type());
      unsigned char *ptrSrc=(unsigned char *)(src.data);
	unsigned char *ptrDst=(unsigned char *)(dst.data);
		//垂直镜像
		for(int i=0;i<height;++i)
		{
			int step1 = dst.step[0]*i;
			int step2 = src.step[0]*(height-i-1);
			for(int j=0;j<width;++j)
			{
				memcpy((void*)&(dst.data[step1+j*jump]),
					(const void*)&(src.data[step2+j*jump]),jump);
			}
		}
	}
	else if (2 == type)//逆时针旋转90
	{
		dst = cv::Mat(src.cols, src.rows, src.type());
		unsigned char *ptrSrc = (unsigned char *)(src.data);
		unsigned char *ptrDst = (unsigned char *)(dst.data);
		for (int j = width; j>0; j--)
		{
			int n = 0;
			int step1 = dst.step[0] * (width - j);
			for (int i = 0; i<height; i++)
			{
				memcpy((void *)(ptrDst + n + step1), (void *)(ptrSrc + src.step[0] * i + (j - 1)*jump), jump);
				n = n + jump;
			}
		}
		flip(dst,dst,0);

		
	}
	else if (3 == type)//180度
	{
		flip(src, dst, 0);
	}
	else
	{
		dst = src;
	}
}

//agnle
Point_<double> GetNewPoint(Point_<double> in, double angle, Point_<double> center)
{
	//要锟接角讹拷转锟斤拷锟斤拷锟斤拷锟斤拷
	angle = (-angle)*CV_PI / 180;
	//锟斤拷锟斤拷锟斤拷为原锟斤拷
	double Tx = in.x - center.x;
	double Ty = in.y - center.y;

	double nowY = cos(angle)*Ty + sin(angle)*Tx;
	double nowX = -sin(angle)*Ty + cos(angle)*Tx;
	Point resultPt;
	resultPt.x = nowX + center.x;
	resultPt.y = center.y + nowY;

	return resultPt;
}

void BGR2GrayMy(unsigned char * bgr, int w, int h, unsigned char* gray)
{
	for (int i = 0; i < h; ++i)
	{
		int step1 = w * 3 * i;
		int jump1 = 0;

		int step2 = w*i;
		int jump2 = 0;
		for (int j = 0; j < w; ++j)
		{
			gray[step2 + jump2] = bgr[step1+jump1];

			jump1 += 3;
			jump2++;
		}
	}
}
void RotateMatAndPts(std::vector<cv::Point2f>& mainPts, cv::Mat& clmResult, double angle)
{

	Mat tmpMat = rotateImage2(clmResult, angle);
	Point center(tmpMat.cols / 2, tmpMat.rows / 2);
	vector<Point2f> tmpPts;
	int X = (tmpMat.cols + 1) / 2 - clmResult.cols / 2;
	int Y = (tmpMat.rows + 1) / 2 - clmResult.rows / 2;
	for (int i = 0; i <mainPts.size(); ++i)
	{
		Point2f pt = mainPts[i];
		pt.x += X;
		pt.y += Y;
		//
		Point tmpPt = GetNewPoint(pt, angle, center);
		tmpPts.push_back(tmpPt);
	}
	mainPts = tmpPts;
	clmResult = tmpMat;

}
void RotateMatAndPts(vector<cv::Point>& mainPts, cv::Mat& clmResult, double angle)
{
		//锟斤拷锟?锟斤拷锟斤拷2锟饺ｏ拷锟斤拷锟斤拷锟斤拷锟阶拷锟斤拷锟?
		Mat tmpMat = rotateImage2(clmResult, angle);
		Point center(tmpMat.cols / 2, tmpMat.rows / 2);
		vector<Point> tmpPts;
		int X = (tmpMat.cols + 1) / 2 - clmResult.cols / 2;
		int Y = (tmpMat.rows + 1) / 2 - clmResult.rows / 2;
		for (int i = 0; i <mainPts.size(); ++i)
		{
			Point pt = mainPts[i];
			pt.x += X;
			pt.y += Y;
			//
			Point tmpPt = GetNewPoint(pt, angle, center);
			tmpPts.push_back(tmpPt);
		}
		mainPts = tmpPts;
		clmResult = tmpMat;

}


//旋转图像内容不变，尺寸相应变大
Mat rotateImage2(Mat& img, int degree,int bkcolor)
{  
	double angle =degree; 

	int width=img.cols, height=img.rows;
	//旋转后的新图尺寸
	
// = Mat(height_rotate,width_rotate,CV_8CU3); 
	//img_rotate.setTo(0);  
	//保证原图可以任意角度旋转的最小尺寸
	int tempLength = ((int)sqrt(width * width + height *height)+3)/4*4;  
	int tempX = (tempLength + 1) / 2 - width / 2;  
	int tempY = (tempLength + 1) / 2 - height / 2;  
  
	int channeles = img.channels();

	Mat temp = Mat(tempLength,tempLength, CV_8UC(channeles));
	temp.setTo(bkcolor);
	Mat img_rotate = temp.clone();

	//将原图复制到临时图像tmp
	for(int i=0;i<height;++i)
	{
		int step1 = temp.step[0]*(i+tempY);
		int step2 = img.step[0]*i;
	/*	for(int j=0;j<width;++j)
		{
			memcpy((void*)&(temp.data[step1+(j+tempX)*channeles]),
						(const void*)&(img.data[step2+j*channeles]), channeles);
		}*/

	
		memcpy((void*)&(temp.data[step1 + (0 + tempX)*channeles]),
		(const void*)&(img.data[step2 + 0*channeles]), channeles*width);
		
	}
	//旋转数组map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float m[6];  
	int w = temp.cols;  
	int h = temp.rows;  

	// 将旋转中心移至图像中间  
	m[2] = w * 0.5f;  
	m[5] = h * 0.5f;  

	Mat M = getRotationMatrix2D(Point(m[2],m[5]),angle,1.0);
	warpAffine(temp,img_rotate,M,Size(tempLength,tempLength),1,0,Scalar(bkcolor, bkcolor, bkcolor));

	return img_rotate;
}  


//旋转图像内容不变，尺寸相应变大
Mat rotateImageC(Mat& img, int degree, int bkcolor)
{
	double angle = degree;

	int width = img.cols, height = img.rows;
	//旋转后的新图尺寸

	// = Mat(height_rotate,width_rotate,CV_8CU3); 
	//img_rotate.setTo(0);  
	//保证原图可以任意角度旋转的最小尺寸
	int tempLength = img.cols;

	int tempX = (tempLength + 1) / 2 - width / 2;
	int tempY = (tempLength + 1) / 2 - height / 2;

	int channeles = img.channels();

	Mat temp = Mat(tempLength, tempLength, CV_8UC(channeles));
	temp.setTo(bkcolor);
	Mat img_rotate = temp.clone();

	//将原图复制到临时图像tmp
	for (int i = 0; i<height; ++i)
	{
		int step1 = temp.step[0] * (i + tempY);
		int step2 = img.step[0] * i;
		/*	for(int j=0;j<width;++j)
		{
		memcpy((void*)&(temp.data[step1+(j+tempX)*channeles]),
		(const void*)&(img.data[step2+j*channeles]), channeles);
		}*/


		memcpy((void*)&(temp.data[step1 + (0 + tempX)*channeles]),
			(const void*)&(img.data[step2 + 0 * channeles]), channeles*width);

	}
	//旋转数组map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float m[6];
	int w = temp.cols;
	int h = temp.rows;

	// 将旋转中心移至图像中间  
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;

	Mat M = getRotationMatrix2D(Point(m[2], m[5]), angle, 1.0);
	warpAffine(temp, img_rotate, M, Size(tempLength, tempLength), 1, 0, Scalar(bkcolor, bkcolor, bkcolor));

	return img_rotate;
}



cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R)
{


	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));

	bool singular = sy < 1e-6; // If

	float x, y, z;
	if (!singular)
	{
		x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
		y = atan2(-R.at<double>(2, 0), sy);
		z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
	}
	else
	{
		x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
		y = atan2(-R.at<double>(2, 0), sy);
		z = 0;
	}
	return cv::Vec3f(x, y, z);
}



 Mat sharpenImage0(const cv::Mat &image)
 {
     //为输出图像分配内存

	 Mat result[3];
	 Mat rgb[3];
	 split(image,rgb);
 
	 for(int n=0;n<3;++n)
	 {
		 result[n] = rgb[n];
	 }

	 cv::Mat kernel(3,3,CV_32F,cv::Scalar(0));
	 kernel.at<float>(1,1) = 5.0;
	kernel.at<float>(0,1) = -1.0;
	 kernel.at<float>(1,0) = -1.0;
	 kernel.at<float>(1,2) = -1.0;
	kernel.at<float>(2,1) = -1.0;

	 for(int m=0;m<3;++m)
	 {
		 //对图像进行滤波
		 cv::filter2D(rgb[m],result[m],rgb[m].depth(),kernel);
	 }
     
     Mat dst;
	 merge(result,3,dst);

	 return dst;
 }



 float intersectRectMax(const cv::Rect& rectA, const cv::Rect& rectB,cv::Rect& comBineRe)
 {

	 if (rectA.x > rectB.x + rectB.width) { return 0.; }

	 if (rectA.y > rectB.y + rectB.height) { return 0.; }

	 if ((rectA.x + rectA.width) < rectB.x) { return 0.; }

	 if ((rectA.y + rectA.height) < rectB.y) { return 0.; }

	 float colInt = min(rectA.x + rectA.width, rectB.x + rectB.width) - max(rectA.x, rectB.x);

	 float rowInt = min(rectA.y + rectA.height, rectB.y + rectB.height) - max(rectA.y, rectB.y);

	 if (colInt < 10 || rowInt < 10) return 0.;

	 float intersection = colInt * rowInt;

	 float areaA = rectA.width * rectA.height;

	 float areaB = rectB.width * rectB.height;

	 float intersectionPercent = max(intersection / areaA, intersection/ areaB);

	 int minx = min(rectA.x, rectB.x);
	 int miny = min(rectA.y,rectB.y);

	 int maxX = max(rectA.x+rectA.width,rectB.x+rectB .width);
	 int maxY = max(rectA.y+rectA.height,rectB.y+rectB.height);

	 comBineRe.x = minx;
	 comBineRe.y = miny;
	 comBineRe.width = maxX - minx;
	 comBineRe.height = maxY - miny;

	 return intersectionPercent;

 }


 float intersectRect(const cv::Rect& rectA, const cv::Rect& rectB, cv::Rect& intersectRect) 
 {

	 if (rectA.x > rectB.x + rectB.width) { return 0.; }

	 if (rectA.y > rectB.y + rectB.height) { return 0.; }

	 if ((rectA.x + rectA.width) < rectB.x) { return 0.; }

	 if ((rectA.y + rectA.height) < rectB.y) { return 0.; }

	 float colInt = min(rectA.x + rectA.width, rectB.x + rectB.width) - max(rectA.x, rectB.x);

	 float rowInt = min(rectA.y + rectA.height, rectB.y + rectB.height) - max(rectA.y, rectB.y);

	 if (colInt < 10 || rowInt < 10) return 0.;

	 float intersection = colInt * rowInt;

	 float areaA = rectA.width * rectA.height;

	 float areaB = rectB.width * rectB.height;

	 float intersectionPercent = intersection / (areaA + areaB - intersection);



	 intersectRect.x = max(rectA.x, rectB.x);

	 intersectRect.y = max(rectA.y, rectB.y);

	 intersectRect.width = min(rectA.x + rectA.width, rectB.x + rectB.width) - intersectRect.x;

	 intersectRect.height = min(rectA.y + rectA.height, rectB.y + rectB.height) - intersectRect.y;

	 return intersectionPercent;

 }




 void CombineRectMaxContent(std::vector<cv::Rect>& outRect, float fa)
 {
	 int iCount = outRect.size();
	 int index = 0;
	 char szBuf[256];
	 for (int k = 0; k < iCount; ++k)
	 {

		 if (index < outRect.size())
		 {
			 for (int j = outRect.size() - 1; j > index; j--)
			 {
				 cv::Rect comR;
				 sprintf(szBuf,"%.2f", intersectRectMax(outRect[index], outRect[j], comR));
			
				 if (intersectRectMax(outRect[index], outRect[j], comR) > fa)
				 {
					 outRect[index] = comR;
					 outRect.erase(outRect.begin() + j);
				 }
			 }
		 }

		 index++;
	 }
 }

 std::vector<cv::Point> GetBdCorners(std::vector<cv::Point> ptsf, bool& bOutOk)
 {

	 if (ptsf.size() < 4)
	 {
		 ptsf.push_back(cv::Point(0, 10));
		 ptsf.push_back(cv::Point(0, 0));
		 ptsf.push_back(cv::Point(10, 0));
		 ptsf.push_back(cv::Point(10, 10));
		 printf("GetBdCorners ERROR!!!!!!!!!!!!!!!!!!...\n");
		 bOutOk = false;
		 return ptsf;
	 }
	 vector<cv::Point> lpt;
	 vector<cv::Point> rpt;
	 int avx = (ptsf[0].x + ptsf[1].x + ptsf[2].x + ptsf[3].x) / 4;
	 for (int k = 0; k < 4; ++k)
	 {
		 if (ptsf[k].x < avx)
		 {
			 lpt.push_back(ptsf[k]);
		 }
		 else
		 {
			 rpt.push_back(ptsf[k]);
		 }
	 }
	 if (lpt.size() != 2 || rpt.size() != 2)
	 {
		 bOutOk = false;
		 return ptsf;
	 }

	 vector<cv::Point> rpts;
	 if (lpt[0].y > lpt[1].y)
	 {
		 rpts.push_back(lpt[0]);
		 rpts.push_back(lpt[1]);
	 }
	 else
	 {
		 rpts.push_back(lpt[1]);
		 rpts.push_back(lpt[0]);
	 }

	 if (rpt[0].y > rpt[1].y)
	 {
		 rpts.push_back(rpt[1]);
		 rpts.push_back(rpt[0]);
	 }
	 else
	 {
		 rpts.push_back(rpt[0]);
		 rpts.push_back(rpt[1]);
	 }
	 bOutOk = true;
	 return rpts;
 }

 std::vector<cv::Point> GetLtBdCorners(std::vector<cv::Point> ptsf, bool& bOutOk)
 {
	 std::vector<cv::Point> outpts = GetBdCorners(ptsf, bOutOk);
	 if (bOutOk)
	 {
		 std::vector<cv::Point> tmpouts;
		 tmpouts.push_back(outpts[1]);
		 tmpouts.push_back(outpts[2]);
		 tmpouts.push_back(outpts[3]);
		 tmpouts.push_back(outpts[0]);
		 return  tmpouts;
	 }
	 return  outpts;
	
 }

 std::vector<cv::Point2f> GetBdCorners(std::vector<cv::Point2f> ptsf, bool& bOutOk)
 {

	 if (ptsf.size() < 4)
	 {
		 ptsf.push_back(cv::Point2f(0, 10));
		 ptsf.push_back(cv::Point2f(0, 0));
		 ptsf.push_back(cv::Point2f(10, 0));
		 ptsf.push_back(cv::Point2f(10, 10));
		 printf("GetBdCorners ERROR!!!!!!!!!!!!!!!!!!...\n");
		 bOutOk = false;
		 return ptsf;
	 }
	 vector<cv::Point2f> lpt;
	 vector<cv::Point2f> rpt;
	 int avx = (ptsf[0].x + ptsf[1].x + ptsf[2].x + ptsf[3].x) / 4;
	 for (int k = 0; k < 4; ++k)
	 {
		 if (ptsf[k].x < avx)
		 {
			 lpt.push_back(ptsf[k]);
		 }
		 else
		 {
			 rpt.push_back(ptsf[k]);
		 }
	 }
	 if (lpt.size() != 2 || rpt.size() != 2)
	 {
		 bOutOk = false;
		 return ptsf;
	 }

	 vector<cv::Point2f> rpts;
	 if (lpt[0].y > lpt[1].y)
	 {
		 rpts.push_back(lpt[0]);
		 rpts.push_back(lpt[1]);
	 }
	 else
	 {
		 rpts.push_back(lpt[1]);
		 rpts.push_back(lpt[0]);
	 }

	 if (rpt[0].y > rpt[1].y)
	 {
		 rpts.push_back(rpt[1]);
		 rpts.push_back(rpt[0]);
	 }
	 else
	 {
		 rpts.push_back(rpt[0]);
		 rpts.push_back(rpt[1]);
	 }
	 bOutOk = true;
	 return rpts;
 }
 /*

 根据检测到的四个角点，提取出身份证图像
 参数1 输入 4个角点信息 inpts
 参数2 输入图像 inImg
 返回值 输出的身份证信息

 */
 cv::Mat GetOutIdentiByPts(std::vector<cv::Point> inpts, cv::Mat inImg)
 {
	 if (!inpts.empty())
	 {
		 bool bOutTrue = false;
		 //根据四个角点，要进行纠正处理
		 std::vector<cv::Point> pc = GetBdCorners(inpts, bOutTrue);
		 if (!bOutTrue)
		 {
			 return  inImg;
		 }

		 pc[0].x -= 2;
		 pc[1].x -= 2;


		 pc[3].x += 3;
		 pc[2].x += 3;

		 pc[2].y += 2;
		 pc[3].y += 2;
		 cv::Point2f AffinePoints0[4] = { pc[0], pc[1], pc[2], pc[3] };


		 cv::Point2f AffinePoints1[4] = { cv::Point2f(0,400),cv::Point2f(0,0), cv::Point2f(640, 0), cv::Point2f(640,400) };
		 char szinfo[256];
		 //printf( "%d %d %d %d p2 p3 %d %d %d %d .....\n", pc[0].x, pc[0].y, pc[1].x, pc[1].y,
		 //	pc[2].x, pc[2].y, pc[3].x, pc[3].y);
		 if (abs(pc[0].y - pc[1].y) > abs(pc[1].x - pc[2].x))
		 {
			 AffinePoints1[0] = cv::Point2f(0, 640);
			 AffinePoints1[1] = cv::Point2f(0, 0);
			 AffinePoints1[2] = cv::Point2f(400, 0);
			 AffinePoints1[3] = cv::Point2f(400, 640);
		 }

		 cv::Mat dst_perspective = PerspectiveTransSmall(inImg, AffinePoints0, AffinePoints1);
		 if (abs(pc[0].y - pc[1].y) > abs(pc[1].x - pc[2].x))
		 {
			 //进行旋转
			 cv::Mat tmp;
			 Rotate90_180(dst_perspective, tmp, 1);
			 dst_perspective = tmp;
		 }

		 return dst_perspective;
	 }

	 return inImg;

 }


 bool Content2RectInHorizon(cv::Rect& r1, cv::Rect& r2, int disCha)
 {

	 int cx1 = r1.x + r1.width / 2;
	 int cx2 = r2.x + r2.width / 2;

	 int cy1 = r1.y + r1.height / 2;
	 int cy2 = r2.y + r2.height / 2;
	 int span = 6;


	 //先判断两个矩形相交
	 int rx1 = r1.x; int rx2 = r2.x; int ry1 = r1.y; int ry2 = r2.y;
	 if (abs(cy1 - cy2) < r1.height * 0.3 && abs(cy1 - cy2) < r2.height * 0.3
		 && (abs(r1.x + r1.width - r2.x) < disCha || abs(r2.x + r2.width - r1.x) < disCha))
	 {
		 return true;
	 }


	 return false;

 }

 bool Content2RectInHorizon(cv::Rect_<float>& r1, cv::Rect_<float>& r2, int disCha)
 {

	 int cx1 = r1.x + r1.width / 2;
	 int cx2 = r2.x + r2.width / 2;

	 int cy1 = r1.y + r1.height / 2;
	 int cy2 = r2.y + r2.height / 2;
	 int span = 6;
	

	 //先判断两个矩形相交
	 int rx1 = r1.x; int rx2 = r2.x; int ry1 = r1.y; int ry2 = r2.y;
	 if (abs(cy1-cy2) < r1.height*0.3 &&abs(cy1 - cy2) < r2.height*0.3
		 && (abs(r1.x+r1.width-r2.x)<disCha||abs(r2.x+r2.width-r1.x)<disCha))
	 {
		 return true;
	 }


	 return false;

 }
 void CombineRectContentHorizon(std::vector<cv::Rect_<float>>& outRect, int discha)
 {
	 int iCount = outRect.size();
	 int index = 0;

	 for (int k = 0; k < iCount; ++k)
	 {

		 if (index < outRect.size())
		 {
			 for (int j = outRect.size() - 1; j > index; j--)
			 {
				
				 if (Content2RectInHorizon(outRect[index], outRect[j], discha))
				 {

					 int minX = min(outRect[index].x, outRect[j].x);
					 int minY = min(outRect[index].y, outRect[j].y);

					 int maxX = max(outRect[index].x + outRect[index].width, outRect[j].x + outRect[j].width);

					 int y1 = outRect[index].y + outRect[index].height;
					 int y2 = outRect[j].y + outRect[j].height;
					 int maxY = max(y1, y2);

					 //if (maxY - minY > 150) continue;
					 //
					 outRect[index].x = minX; outRect[index].y = minY;
					 outRect[index].width = maxX - minX; outRect[index].height = maxY - minY;
					 outRect.erase(outRect.begin() + j);
				 }
			 }
		 }

		 index++;
	 }
 }