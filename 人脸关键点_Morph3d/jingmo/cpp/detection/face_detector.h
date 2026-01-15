//
// Created by yuanhao on 20-6-10.
//

#ifndef LIVEBODYEXAMPLE_FACE_DETECTOR_H
#define LIVEBODYEXAMPLE_FACE_DETECTOR_H
#include <stdio.h>
#include <string>
#include <vector>
#include "ncnn/net.h"
#include "../definition.h"
#include <opencv2/core.hpp>
class FaceDetector {
public:
    FaceDetector();

    ~FaceDetector();

    void SetMinFaceSize(int size);

virtual int LoadModel();

  virtual  int Detect(cv::Mat& src, std::vector<FaceBox>& boxes);
  virtual int DetectRect(cv::Mat& src, cv::Rect& outR);
  virtual int DetectRects(cv::Mat& dst, std::vector<cv::Rect>& outR);
private:
	
    ncnn::Net net_;
    int input_size_ = 192;
    const std::string net_input_name_ = "data";
    const std::string net_output_name_ = "detection_out";
    ncnn::Option option_;
    float threshold_;
    const float mean_val_[3] = {104.f, 117.f, 123.f};
    int thread_num_;
    int min_face_size_;
};



#endif //LIVEBODYEXAMPLE_FACE_DETECTOR_H
