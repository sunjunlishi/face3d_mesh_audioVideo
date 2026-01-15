#pragma once
#ifdef THIS_FACE_DLL
#define EXPORT_CAFFE_DLL1  __declspec(dllexport) 
#else
#define EXPORT_CAFFE_DLL1 __declspec(dllimport) 
#endif

#include <vector>
#include <string>
#ifdef __cplusplus
//extern "C" {
#endif

	// 主接口函数声明
EXPORT_CAFFE_DLL1 std::vector<std::string> dllspeechalign(int argc, char* argv[]);

#ifdef __cplusplus
//}
#endif