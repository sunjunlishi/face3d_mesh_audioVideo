#include "stdafx.h"
#include <windows.h>
#include <windowsx.h>
#include <onnxruntime_cxx_api.h>
#include <onnxruntime_c_api.h>
#include <cpu_provider_factory.h>
//#include <mkldnn_provider_factory.h>

#include <opencv2/core.hpp>  
#include <opencv2/highgui.hpp>  
#include <opencv2/imgproc.hpp>  
#include <vector>
#include <stdlib.h> 
#include <iostream> 
#include <time.h>
#include "3ddfav2.h"
using namespace cv;
using namespace std;


#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")



static constexpr const int width_ = 192;
static constexpr const int height_ = 192;
static constexpr const int channel = 3;

std::array<float, 1 * width_ * height_*channel> input_image_{};
std::array<float, 1 * 62> results_{};


std::array<float, 1 * 1> resultsnew2_{};

std::array<float, 3 * 38365> results_2{};

std::array<float, 3> results_extra{};
//(3, 3)
//(3, 1)
//(40, 1)
//(10, 1)
std::array<float, 1 *9> ind1{};
std::array<float, 1 *3> ind2{};
std::array<float, 1 * 40> ind3{};
std::array<float, 1 * 10> ind4{};
std::array<int64_t, 2> insh1{ 3,3 };
std::array<int64_t, 2> insh2{ 3,1 };
std::array<int64_t, 2> insh3{ 40,1 };
std::array<int64_t, 2> insh4{ 10,1};

Ort::Value input_tensor_{ nullptr };
std::array<int64_t, 4> input_shape_{ 1,channel, height_, width_ };

Ort::Value output_tensor_{ nullptr };
Ort::Value output_tensor_2{ nullptr };
std::array<int64_t, 2> output_shape_{ 1,62 };
std::array<int64_t, 2> output_shape_2{ 3,38365 };


OrtSessionOptions* session_option;

Ort::Value  input_tensor_21[4] = {Ort::Value(nullptr),Ort::Value(nullptr),Ort::Value(nullptr),Ort::Value(nullptr) };


std::array<int64_t, 4> outputsnewSh1{ 1,1404,1,1 };
std::array<int64_t, 4> outputsnewSh2{ 1,1,1,1 };

Ort::Env * env;
Ort::Session * session_;
Ort::Session * session1_;
void  DoInitSession()
{
	env = new Ort::Env;
	session_ = new Ort::Session{ *env, L"mb1_120x120.onnx", Ort::SessionOptions{ nullptr } };
	session1_ = new Ort::Session{ *env, L"bfm_noneck_v3.onnx", Ort::SessionOptions{ nullptr } };



	auto allocator_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	input_tensor_ = Ort::Value::CreateTensor<float>(allocator_info, input_image_.data(), input_image_.size(), input_shape_.data(), input_shape_.size());
	output_tensor_ = Ort::Value::CreateTensor<float>(allocator_info, results_.data(), results_.size(), output_shape_.data(), output_shape_.size());


	output_tensor_2 = Ort::Value::CreateTensor<float>(allocator_info, results_2.data(), results_2.size(),
		output_shape_2.data(), output_shape_2.size());

	const char* input_names[] = { "input_1" };
	const char* output_names[] = { "conv2d_20","conv2d_30" };
	const char* output_names2[] = { "output" };
	const char* input_names2[] = { "R","offset","alpha_shp","alpha_exp" };
	input_tensor_21[0] = Ort::Value::CreateTensor<float>(allocator_info, ind1.data(), ind1.size(), insh1.data(), insh1.size());
	input_tensor_21[1] = Ort::Value::CreateTensor<float>(allocator_info, ind2.data(), ind2.size(), insh2.data(), insh2.size());
	input_tensor_21[2] = Ort::Value::CreateTensor<float>(allocator_info, ind3.data(), ind3.size(), insh3.data(), insh3.size());
	input_tensor_21[3] = Ort::Value::CreateTensor<float>(allocator_info, ind4.data(), ind4.size(), insh4.data(), insh4.size());


	//outputsnew[1] = Ort::Value::CreateTensor<float>(allocator_info, resultsnew2_.data(), resultsnew2_.size(), outputsnewSh2.data(), outputsnewSh2.size());

}
bool bInitedSession = false;
#include <mutex>
float gOParam1[62];
bool  gbpram1 = false;
float gOParam2[62];
bool  gbparam2 = false;

int iTag = 0;
int doSession(cv::Mat img,float * outinfo, std::mutex& msessionMutex)
{
//	img = cv::imread("D:/4.jpg");
	if (!bInitedSession)
	{
		DoInitSession();
		bInitedSession = true;
	}
	const char* input_names[] = { "input_1" };
	const char* output_names[] = { "conv2d_20","conv2d_30" };
	const char* output_names2[] = { "output" };
	const char* input_names2[] = { "R","offset","alpha_shp","alpha_exp" };


	Ort::Value *input_tensor_1 = &input_tensor_;
	Ort::Value *output_tensor_1 = &output_tensor_;


	Mat dst1;
	resize(img,dst1,Size(192, 192));
	const int row = height_;
	const int col = width_;
	vector<Mat> channels1;
	vector<Mat> channels2;
	Mat imageYChannel;
	Mat imageCrChannel;
	Mat imageCbChannel;
	split(dst1, channels1);
	
	float* inputdata = input_image_.data();
	for (int c = 0;c < 3;c++)
	{
		int step1 = c*row*col;
		unsigned char * inchannelData = channels1.at(c).data;
		for (int i = 0;i < row;i++) 
		{
			int step = i*col;
			for (int j = 0;j < col;j++) {
				inputdata[step1 + step + j] = (inchannelData[step +j]-127.0)/128.0;
			}
		}
	}

	clock_t t1 = clock();

		//OrtRun(session_, nullptr, input_names, &input_tensor_1, 1, output_names, 1, &output_tensor_1);
		//output_tensor_ = session_.Run(Ort::RunOptions{ nullptr },input_names, input_tensor_1, 1, output_names, output_tensor_1,1 );
		//session_->Run(Ort::RunOptions{ nullptr }, input_names, input_tensor_1, 2, output_names, &outputsnew, 1);
		
		static float parammean[] = { 3.4926363e-4,2.5279013e-07 ,-6.8751979e-07 , 6.0167957e+01,
			-6.2955132e-07 , 5.7572004e-04 ,-5.0853912e-05 , 7.4278198e+01,
			5.4009172e-07 , 6.5741384e-05 , 3.4420125e-04 ,-6.6671577e+01,
			-3.4660369e+05, -6.7468234e+04,  4.6822266e+04 ,-1.5262047e+04,
			4.3505889e+03 ,-5.4261453e+04 ,-1.8328033e+04 ,-1.5843289e+03,
			-8.4566344e+04,  3.8359607e+03 ,-2.0811361e+04 , 3.8094930e+04,
			-1.9967855e+04 ,-9.2413701e+03, -1.9600715e+04,  1.3168090e+04,
			-5.2591440e+03,  1.8486478e+03 ,-1.3030662e+04 ,-2.4355562e+03,
			-2.2542065e+03, -1.4396562e+04, -6.1763291e+03 ,-2.5621920e+04,
			2.2639447e+02 ,-6.3261235e+03 ,-1.0867251e+04 , 8.6846509e+02,
			-5.8311479e+03 , 2.7051238e+03, -3.6294177e+03 , 2.0439901e+03,
			-2.4466162e+03,  3.6586970e+03 ,-7.6459897e+03 ,-6.6744526e+03,
			1.1638839e+02,  7.1855972e+03, -1.4294868e+03 , 2.6173665e+03,
			-1.2070955e+00,  6.6907924e-01, -1.7760828e-01,  5.6725528e-02,
			3.9678156e-02, -1.3586316e-01 ,-9.2239931e-02 ,-1.7260718e-01,
			-1.5804484e-02, -1.4168486e-01
		};
		static float paramstd[] = { 1.76321526e-04,6.73794348e-05 ,4.47084894e-04 ,2.65502319e+01,
			1.23137695e-04 ,4.49302170e-05 ,7.92367064e-05 ,6.98256302e+00,
			4.35044407e-04 ,1.23148900e-04 ,1.74000015e-04 ,2.08030396e+01,
			5.75421125e+05 ,2.77649062e+05 ,2.58336844e+05 ,2.55163125e+05,
			1.50994375e+05 ,1.60086109e+05 ,1.11277305e+05 ,9.73117812e+04,
			1.17198453e+05 ,8.93173672e+04 ,8.84935547e+04 ,7.22299297e+04,
			7.10802109e+04 ,5.00139531e+04 ,5.59685820e+04 ,4.75255039e+04,
			4.95150664e+04 ,3.81614805e+04 ,4.48720586e+04 ,4.62732383e+04,
			3.81167695e+04 ,2.81911621e+04 ,3.21914375e+04 ,3.60061719e+04,
			3.25598926e+04 ,2.55511172e+04 ,2.42675098e+04 ,2.75213984e+04,
			2.31665312e+04 ,2.11015762e+04 ,1.94123242e+04 ,1.94522031e+04,
			1.74549844e+04 ,2.25376230e+04 ,1.61742812e+04 ,1.46716406e+04,
			1.51156885e+04, 1.38700732e+04 ,1.37463125e+04, 1.26631338e+04,
			1.58708346e+00 ,1.50770092e+00 ,5.88135779e-01, 5.88974476e-01,
			2.13278517e-01 ,2.63020128e-01 ,2.79642940e-01 ,3.80302161e-01,
			1.61628410e-01 ,2.55969286e-01 };

		float resnew[141];
	
		cv::Mat timg;
		cv::resize(img,timg,Size(192,192));
		char szinfo[256];
		
		cv::imwrite("d:/outinfo.jpg",timg);
		
		gbparam2 = true;
	

		
		for (int k = 0; k < 141; ++k)
		{
			resnew[k] = 0.01;
		}

		//
		////进行赋值操作
		//
		int iTage = 0;
		//float * f1data = ind1.data();

		//f1data[0] = resnew[0];
		//f1data[1] = resnew[1];
		//f1data[2] = resnew[2];
		//f1data[3] = resnew[4];
		//f1data[4] = resnew[5];
		//f1data[5] = resnew[6];
		//f1data[6] = resnew[8];
		//f1data[7] = resnew[9];
		//f1data[8] = resnew[10];

		//

		//float * f2data = ind2.data();
		//f2data[0] = resnew[3];
		//f2data[1] = resnew[7];
		//f2data[2] = resnew[11];

		//float * f3data = ind3.data();
		// iTage = 0;
		// //if (bFirst)
		// {
		//	 for (int k = 12; k < 12 + 40; ++k)
		//	 {
		//		 f3data[iTage] = 0;
		//		 iTage++;
		//	 }

		// }
		//
		//

		
		float * f4data = ind4.data();
		iTage = 0;
		/*for (int k = 52; k < 62; ++k)
		{
			f4data[iTage] = 0;
			iTage++;
		}*/




		session1_->Run(Ort::RunOptions{ nullptr }, input_names2, input_tensor_21,4, output_names2, &output_tensor_2, 1);
		

	float gOParam1[62];
	bool  gbpram1 = false;
	float gOParam2[62];
	bool  gbparam2 = false;

	iTag++;

	cout << "running time ：" << (clock()-t1) << "sec\n" << endl;
	//FILE * fp = fopen("D:/out.obj","w");
	float * out2data = results_2.data();

	msessionMutex.lock();
	memcpy(outinfo, out2data, 38365*3*4);
	msessionMutex.unlock();
	//for (int k = 0; k < 38365; ++k)
	//{
	//	char szinfo[256];
	//	memset(szinfo,0,sizeof(szinfo));
	//	sprintf(szinfo,"v %.3f %.3f %.3f\n", out2data[k]-60, out2data[k+ 38365], out2data[k + 2*38365]+60);
	//	//fwrite(szinfo, 1, strlen(szinfo), fp);
	//}

	return 0;
}

