//
// Created by yuanhao on 20-6-10.
//
#include <opencv2/core.hpp>
#include "face_detector.h"
#include <algorithm>
#include <ncnn/platform.h>
#include "../android_log.h"
#include <Common/gLobalInfo.h>
 //bool AreaComp(FaceBox& l, FaceBox& r) {
 //   return ((l.x2 - l.x1 + 1) * (l.y2 - l.y1 + 1)) > ((r.x2 - r.x1 + 1) * (r.y2 - r.y1 + 1));
//}

//#include <cpu.h>

#if NCNN_VULKAN
#include "ncnn/gpu.h"
#endif // NCNN_VULKAN

FaceDetector::FaceDetector() : threshold_(0.53f), thread_num_(2){
    min_face_size_ = 120;
	
#ifdef NCNN_VULKAN
	min_face_size_ = 200;
#endif
    option_.lightmode = true;
    option_.num_threads = thread_num_;


}

FaceDetector::~FaceDetector() {
    net_.clear();
}

void FaceDetector::SetMinFaceSize(int size) {
    min_face_size_ = size;
}

int FaceDetector::LoadModel() {
#ifdef NCNN_VULKAN
	printf("FaceDetector11 in.... GPU....\n");


	//ncnn::set_cpu_powersave(2);
	//ncnn::set_omp_num_threads(1);

	net_.opt.use_vulkan_compute = false;
	option_.use_vulkan_compute = false;
	//net_.set_vulkan_device(GetRandMy()); // use device-x
	printf("FaceDetector using GPU....\n");
#else
	
	printf("else FaceDetector using cpu....\n");
#endif
	net_.opt.num_threads = thread_num_;
    int ret = net_.load_param((getgModelpath()+"/detection/detection.param").c_str());
    if(ret != 0) {
        //LOG_ERR("FaceDetector load param failed. %d", ret);
		printf(".........FaceDetector  load param Error %d\n", ret);
        return -1;
    }
	printf(".................model facedetector %d %s\n",ret, (getgModelpath() + "/detection/detection.bin").c_str());
    ret = net_.load_model((getgModelpath() + "/detection/detection.bin").c_str());
    if(ret != 0) {
		printf(".........FaceDetector  load bin Error %d\n",ret);
      //  LOG_ERR("FaceDetector load model failed. %d", ret);
        return -2;
    }
	printf(".................facedetector. %d\n", ret);
    return 0;
}


int FaceDetector::DetectRects(cv::Mat& dst, std::vector<cv::Rect>& outRs)
{

	std::vector<FaceBox> boxes;
	Detect(dst, boxes);
	int maxW = 0;
	//sel the max face
	for (int k = 0; k<boxes.size(); ++k)
	{

		int span = boxes[k].y2 - boxes[k].y1;

		int posx = boxes[k].x1;
		int posy = boxes[k].y1 + span*0.10;
		int w = boxes[k].x2 - boxes[k].x1;


		int h = w;
		if (posx < 0) posx = 0;
		if (posy < 0) posy = 0;
		if (posx + w > dst.cols) w = dst.cols - posx;
		if (posy + h > dst.rows) h = dst.rows - posy;

	
		cv::Rect outR;
			outR.x = posx;
			outR.y = posy;
			outR.width = w;
			outR.height = h;
			outRs.push_back(outR);
	}

	return outRs.size();
}

int FaceDetector::DetectRect(cv::Mat& dst, cv::Rect& outR)
{
	
	std::vector<FaceBox> boxes;
    Detect(dst, boxes);
	
	int maxW = 0;
	//sel the max face
	for (int k = 0; k<boxes.size(); ++k)
	{
		
		int span = boxes[k].y2 - boxes[k].y1;
		
		int posx = boxes[k].x1;
		int posy = boxes[k].y1 + span*0.10;
		int w = boxes[k].x2 - boxes[k].x1;

		
			int h = w;
			if (posx < 0) posx = 0;
			if (posy < 0) posy = 0;
			if (posx + w > dst.cols) w = dst.cols - posx;
			if (posy + h > dst.rows) h = dst.rows - posy;

		if (w > maxW)
		{
			maxW = w;
			outR.x = posx;
			outR.y = posy;
			outR.width = w;
			outR.height = h;
		}
	}

	return boxes.size();
}

int FaceDetector::Detect(cv::Mat &src, std::vector<FaceBox> &boxes) 
{
    int w = src.cols;
    int h = src.rows;

    float aspect_ratio = w / (float)h;

    int input_width = static_cast<int>(input_size_ * sqrt(aspect_ratio));
    int input_height = static_cast<int>(input_size_ / sqrt(aspect_ratio));

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(src.data, ncnn::Mat::PIXEL_BGR, src.cols, src.rows,
                                                 input_width, input_height);

    in.substract_mean_normalize(mean_val_, nullptr);

    ncnn::Extractor extractor = net_.create_extractor();

#ifndef NCNN_VULKAN
   // extractor.set_num_threads(thread_num_);
#endif

    extractor.input(net_input_name_.c_str(), in);

    ncnn::Mat out;
    extractor.extract(net_output_name_.c_str(), out);

    boxes.clear();
    for (int i = 0; i < out.h; ++i) {
	
        const float* values = out.row(i);
        float confidence = values[1];

        if(confidence < threshold_) continue;

        FaceBox box;
        box.confidence = confidence;
        box.x1 = values[2] * w;
        box.y1 = values[3] * h;
        box.x2 = values[4] * w;
        box.y2 = values[5] * h;

        // square
        float box_width = box.x2 - box.x1 + 1;
        float box_height = box.y2 - box.y1 + 1;

        float size = (box_width + box_height) * 0.5f;

        if(size < min_face_size_) continue;

        float cx = box.x1 + box_width * 0.5f;
        float cy = box.y1 + box_height * 0.5f;

        box.x1 = cx - size * 0.5f;
        box.y1 = cy - size * 0.5f;
        box.x2 = cx + size * 0.5f - 1;
        box.y2 = cy + size * 0.5f - 1;

        boxes.emplace_back(box);
    }

    // sort
   // std::sort(boxes.begin(), boxes.end(), AreaComp);
    return boxes.size() > 0;
}
