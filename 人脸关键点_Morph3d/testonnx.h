#pragma once




#ifdef THIS_FACE_DLL
#define EXPORT_CAFFE_DLL1  __declspec(dllexport) 
#else
#define EXPORT_CAFFE_DLL1 
#endif


#include <opencv2/core.hpp>
#include <vector>
EXPORT_CAFFE_DLL1 std::vector<float>  testBlender52(cv::Mat frame);