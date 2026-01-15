//
// Create by RangiLyu
// 2020 / 10 / 2
//

#pragma once
#include <ncnn/ncnnobjh.h>
#include <ncnn/nanodet/nanodet.h>
#include "onnxruntime_cxx_api.h"

#include <opencv2/core.hpp>
#include <mutex>




#ifdef _WIN32
#define my_strtol wcstol
#define my_strrchr wcsrchr
#define my_strcasecmp _wcsicmp
#define my_strdup _strdup
#else
#define my_strtol strtol
#define my_strrchr strrchr
#define my_strcasecmp strcasecmp
#define my_strdup strdup
#endif


void generate_proposalsOnnx(Ort::Value &cls_pred1, Ort::Value &dis_pred1, cv::Mat& in_pad, int stride, float prob_threshold, std::vector<Object>& objects);



class NanoDetOnnx
{
public:
    NanoDetOnnx( std::string pathStr, int numOfThread);

    ~NanoDetOnnx();

	
	bool  BUsing() { return m_bUsing; }
	void SetBUsing(bool bUsing) { m_bUsing = bUsing; }

    static NanoDetOnnx* detector;
    Ort::Session *session;
    Ort::Env * env;
    Ort::SessionOptions  sessionOptions = Ort::SessionOptions();
    int numThread =2;
    static bool hasGPU;
    // modify these parameters to the same with your config if you want to use your own model
    int input_size[2] = {512, 512 }; // input height and width
    int num_class = 12; // number of classes. 80 for COCO
    int reg_max = 7; // `reg_max` set in the training config. Default: 7.
    std::vector<int> strides = { 8, 16, 32, 64 }; // strides of the multi-level feature.
	float detectfaceswapAnti(cv::Mat bgr);
	void decode_inferPlus(ncnn::Mat& feats, std::vector<CenterPrior>& center_priors,
		float threshold, std::vector<std::vector<BoxInfo>>& results);
	std::vector<Object> detect(cv::Mat image, float score_threshold, float nms_threshold);
	std::vector<Object> detectPlus(cv::Mat image, float score_threshold, float nms_threshold);
    std::vector<std::string> labels{ "0", "1", "2", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
		"fire hydrant", "stop sign" };

	 float mean_vals[3] ;
	float norm_vals[3];

	
    char* inputName;
    char *outputName;

	std::string m_pathmodel;
private:

	int m_target_size;
	bool m_bUsing;

	
    std::vector<float> preprocess(cv::Mat &src);
	std::vector<float> preprocessPlus(cv::Mat &src);
	std::vector<float> preprocessAnti(cv::Mat &src);
    void decode_infer(Ort::Value &feats, std::vector<CenterPrior>& center_priors, float threshold, std::vector<std::vector<BoxInfo>>& results);
	
	BoxInfo disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride);
    static void nms(std::vector<BoxInfo>& result, float nms_threshold);
    
    void getInputName();
    void getOutputName();
};



class NanoDetOnnx;
class CNanoDetOnnxMgr
{
public:

	NanoDetOnnx * GetOneObj(std::string pathname);
	
	void SetObjNouse(NanoDetOnnx * obj);
public:

	std::mutex m_mutex;
	std::vector<NanoDetOnnx*> m_listObj;
	CNanoDetOnnxMgr();
	~CNanoDetOnnxMgr();
	static CNanoDetOnnxMgr * Instance();
private:
	static CNanoDetOnnxMgr * m_instance;
};




int image_demo(NanoDetOnnx &detector, const char* imagepath);

std::vector<cv::Rect>  NanoDetOnnxRectsInter(cv::Mat img, std::string pathmodel, float thredhold = 0.38);

std::vector<Object>  NanoDetOnnxObjsInter(cv::Mat img, std::string pathmodel, float thredhold = 0.38);
std::vector<cv::Rect>  NanoDetOnnxRectsPlusInter(cv::Mat img, std::string pathmodel, float thredhold = 0.38);

float SwapFaceAntiInter(cv::Mat img, std::string pathmodel);