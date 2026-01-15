//
// Create by RangiLyu
// 2020 / 10 / 2
//
#ifdef _WIN32
#include <Common/CommonWin.h>
#endif
#include "nanodetOnnx.h"
#include <Common/gLobalInfo.h>
#include <numeric>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Common/gLobalInfo.h>
#include <ncnn/mat.h>
#include <ncnn/layer.h>
#include <NanodetOnnx/OcrUtils.h>

inline float fast_exp(float x)
{
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}

inline float sigmoid(float x)
{
    return 1.0f / (1.0f + fast_exp(-x));
}

template<typename _Tp>
int activation_function_softmax(const _Tp* src, _Tp* dst, int length)
{
    const _Tp alpha = *std::max_element(src, src + length);
    _Tp denominator{ 0 };

    for (int i = 0; i < length; ++i) {
        dst[i] = fast_exp(src[i] - alpha);
        denominator += dst[i];
    }

    for (int i = 0; i < length; ++i) {
        dst[i] /= denominator;
    }

    return 0;
}




bool NanoDetOnnx::hasGPU = false;
NanoDetOnnx* NanoDetOnnx::detector = nullptr;

void NanoDetOnnx::getInputName() {
    size_t numInputNodes = session->GetInputCount();
    if (numInputNodes > 0) {
        Ort::AllocatorWithDefaultOptions allocator;
        {

#ifdef _WIN32
			Ort::AllocatedStringPtr input_name_Ptr = session->GetInputNameAllocated(0, allocator);
			char * t = input_name_Ptr.get();

			inputName = my_strdup(t);

#else
			Ort::AllocatedStringPtr input_name_Ptr = session->GetInputNameAllocated(0, allocator);
			char * t = input_name_Ptr.get();

			inputName = my_strdup(t);
			
#endif
          
        }
    }
}

void NanoDetOnnx::getOutputName() {
    size_t numOutputNodes = session->GetOutputCount();
    if (numOutputNodes > 0) {

        Ort::AllocatorWithDefaultOptions allocator;


		for (int k = 0; k < numOutputNodes;++k)
        {
#ifdef _WIN32

			Ort::AllocatedStringPtr output_name_Ptr = session->GetOutputNameAllocated(k, allocator);
			char * t = output_name_Ptr.get();

			outputName = my_strdup(t);
#else
			
			Ort::AllocatedStringPtr output_name_Ptr = session->GetOutputNameAllocated(k, allocator);
				char * t = output_name_Ptr.get();

		    outputName = my_strdup(t);
				
#endif
         
        }
    }
}

// cuda EP options
void prepare_cuda_opt11(const OrtApi& api, Ort::SessionOptions& session_options)
{

	OrtCUDAProviderOptionsV2* options = nullptr;
	api.CreateCUDAProviderOptions(&options);
	std::unique_ptr<OrtCUDAProviderOptionsV2, decltype(api.ReleaseCUDAProviderOptions)> rel_cuda_options(
		options, api.ReleaseCUDAProviderOptions);
	std::vector<const char*> keys{ "device_id" };
	std::vector<const char*> values{ "0" };
	api.UpdateCUDAProviderOptions(rel_cuda_options.get(), keys.data(), values.data(), keys.size());
	api.SessionOptionsAppendExecutionProvider_CUDA_V2(static_cast<OrtSessionOptions*>(session_options),
		rel_cuda_options.get());
}

NanoDetOnnx::NanoDetOnnx( std::string pathStr, int numOfThread)
{
	m_pathmodel = pathStr;
    numThread = numOfThread;
	m_bUsing = false;
	m_target_size = 512;


	mean_vals[0] = 127.f; ;
	norm_vals[0] = 1/ 128.f;

	mean_vals[1] = 127.f; 
	norm_vals[1] = 1 / 128.f;

	mean_vals[2] = 127.f; 
	norm_vals[2] = 1 / 128.f;
	
	if (pathStr.find("320") != -1)
	{
		input_size[0] = 320;
		input_size[1] = 320;
		m_target_size = 320;

		mean_vals[0] = 103.53f; ;
		norm_vals[0] = 0.017429f;

		mean_vals[1] = 116.28f;
		norm_vals[1] = 0.017507f;

		mean_vals[2] = 123.675f;
		norm_vals[2] = 0.017125f;
	
	}

	if (pathStr.find("416") != -1)
	{
		input_size[0] = 416;
		input_size[1] = 416;
		m_target_size = 416;

		mean_vals[0] = 103.53f; ;
		norm_vals[0] = 0.017429f;

		mean_vals[1] = 116.28f;
		norm_vals[1] = 0.017507f;

		mean_vals[2] = 123.675f;
		norm_vals[2] = 0.017125f;

	}

	env = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "nanodet");





#ifdef ONNX_VULKAN
#ifdef _WIN32

#else
	printf("NanoDetOnnx session value gpu....    %d  %s\n", GetRandMy(), pathStr.c_str());

	auto providers = Ort::GetAvailableProviders();
	for (auto provider : providers)
	{
		printf("providers2 %s ....\n", provider.c_str());
	}
	const auto& api = Ort::GetApi();
	//OrtCUDAProviderOptions cuda_options{};
	//cuda_options.device_id = GetRandMy(); // 指定GPU设备ID，如果有多个GPU
	//sessionOptions.AppendExecutionProvider_CUDA(cuda_options);

	if(providers.size() >1)
	prepare_cuda_optbase(api, sessionOptions);

#endif

#endif


#ifdef _WIN32
	std::wstring crnnPath = AnsiToUnicode(pathStr);
	session = new Ort::Session(*env, crnnPath.c_str(), sessionOptions);
#else
	printf("session start .....\n");
	
	session = new Ort::Session(*env, pathStr.c_str(), sessionOptions);
	printf("session end .....\n");
#endif

    getInputName();
    getOutputName();


}

NanoDetOnnx::~NanoDetOnnx()
{
    delete session;
  
    free(outputName);
}

// void NanoDetOnnx::preprocess(cv::Mat& image, ncnn::Mat& in)
// {
//     int img_w = image.cols;
//     int img_h = image.rows;

//     in = ncnn::Mat::from_pixels(image.data, ncnn::Mat::PIXEL_BGR, img_w, img_h);
//     //in = ncnn::Mat::from_pixels_resize(image.data, ncnn::Mat::PIXEL_BGR, img_w, img_h, this->input_width, this->input_height);


//     in.substract_mean_normalize(mean_vals, norm_vals);
// }

//plus



std::vector<float> NanoDetOnnx::preprocessAnti(cv::Mat &src)
{

	float mean_valsp[3] = { 127.50f, 127.50f, 127.50f };
	float norm_valsp[3] = { 0.00784f, 0.00784f, 0.00784f };


	auto inputTensorSize = src.cols * src.rows * src.channels();
	std::vector<float> inputTensorValues(inputTensorSize);
	size_t numChannels = src.channels();
	size_t imageSize = src.cols * src.rows;
	std::vector<cv::Mat> channels1;

	cv::split(src, channels1);

	for (size_t ch = 0; ch < numChannels; ++ch) {

		int step1 = ch*src.rows*src.cols;


		unsigned char * inchannelData = channels1.at(ch).data;
		for (int i = 0; i < src.rows; i++)
		{
			int step = i*src.cols;
			for (int j = 0; j < src.cols; j++) {
				inputTensorValues[step1 + step + j] = (inchannelData[step + j] - mean_valsp[ch])*norm_valsp[ch];
			}
		}


	}
	return inputTensorValues;
}


std::vector<float> NanoDetOnnx::preprocessPlus(cv::Mat &src)
{

	 float mean_valsp[3] = { 103.53f, 116.28f, 123.675f };
	 float norm_valsp[3] = { 0.017429f, 0.017507f, 0.017125f };


	auto inputTensorSize = src.cols * src.rows * src.channels();
	std::vector<float> inputTensorValues(inputTensorSize);
	size_t numChannels = src.channels();
	size_t imageSize = src.cols * src.rows;
	std::vector<cv::Mat> channels1;

	cv::split(src, channels1);

	for (size_t ch = 0; ch < numChannels; ++ch) {

		int step1 = ch*src.rows*src.cols;


		unsigned char * inchannelData = channels1.at(ch).data;
		for (int i = 0; i < src.rows; i++)
		{
			int step = i*src.cols;
			for (int j = 0; j < src.cols; j++) {
				inputTensorValues[step1 + step + j] = (inchannelData[step + j] - mean_valsp[ch])*norm_valsp[ch];
			}
		}
		

	}
	return inputTensorValues;
}

std::vector<float> NanoDetOnnx::preprocess(cv::Mat &src)
{
    auto inputTensorSize = src.cols * src.rows * src.channels();
    std::vector<float> inputTensorValues(inputTensorSize);
    size_t numChannels = src.channels();
    size_t imageSize = src.cols * src.rows;
	std::vector<cv::Mat> channels1;
	
	cv::split(src, channels1);

	for (size_t ch = 0; ch < numChannels; ++ch) {

		int step1 = ch*src.rows*src.cols;


		unsigned char * inchannelData = channels1.at(ch).data;
		for (int i = 0; i < src.rows; i++)
		{
			int step = i*src.cols;
			for (int j = 0; j < src.cols; j++) {
				inputTensorValues[step1 + step + j] = (inchannelData[step + j] - mean_vals[ch])*norm_vals[ch];
			}
		}
   

	}
    return inputTensorValues;
}


void generate_proposalsOnnx(Ort::Value &cls_pred1, Ort::Value &dis_pred1, cv::Mat& in_pad,int stride, float prob_threshold, std::vector<Object>& objects)
{

	try {
		std::vector<int64_t> outputShape = cls_pred1.GetTensorTypeAndShapeInfo().GetShape();
		int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1,
			std::multiplies<int64_t>());
		float *floatCls = cls_pred1.GetTensorMutableData<float>();




		 int num_grid = outputShape[1];// cls_pred.h;

		int num_grid_x;
		int num_grid_y;
		if (in_pad.cols > in_pad.rows)
		{
			num_grid_x = in_pad.cols / stride;
			num_grid_y = num_grid / num_grid_x;
		}
		else
		{
			num_grid_y = in_pad.rows / stride;
			num_grid_x = num_grid / num_grid_y;
		}


	

		


		std::vector<int64_t> outputShape2 = dis_pred1.GetTensorTypeAndShapeInfo().GetShape();
		int64_t outputCount2 = std::accumulate(outputShape2.begin(), outputShape2.end(), 1,
			std::multiplies<int64_t>());
		float *floatDis= dis_pred1.GetTensorMutableData<float>();
		/*if (in_pad.w > in_pad.h)
		{
			num_grid_x = in_pad.w / stride;
			num_grid_y = num_grid / num_grid_x;
		}
		else
		{
			num_grid_y = in_pad.h / stride;
			num_grid_x = num_grid / num_grid_y;
		}
*/
		const int num_class = outputShape[2];
		const int reg_max_1 =  outputShape2[2] / 4;

		for (int i = 0; i < num_grid_y; i++)
		{
			for (int j = 0; j < num_grid_x; j++)
			{
				const int idx = i * num_grid_x + j;

				const float* scores = floatCls + idx*num_class;

				// find label with max score
				int label = -1;
				float score = -FLT_MAX;
				for (int k = 0; k < num_class; k++)
				{
					if (scores[k] > score)
					{
						label = k;
						score = scores[k];
					}
				}

				if (score >= prob_threshold)
				{
					ncnn::Mat bbox_pred(reg_max_1, 4, (void*)(floatDis + idx*outputShape2[2]));
					{
						ncnn::Layer* softmax = ncnn::create_layer("Softmax");

						ncnn::ParamDict pd;
						pd.set(0, 1); // axis
						pd.set(1, 1);
						softmax->load_param(pd);

						ncnn::Option opt;
						opt.num_threads = 1;
						opt.use_packing_layout = false;
						softmax->create_pipeline(opt);

						softmax->forward_inplace(bbox_pred, opt);

						softmax->destroy_pipeline(opt);

						delete softmax;
					}
					float pred_ltrb[4];
					for (int k = 0; k < 4; k++)
					{
						float dis = 0.f;
						const float* dis_after_sm = bbox_pred.row(k);
						for (int l = 0; l < reg_max_1; l++)
						{
							dis += l * dis_after_sm[l];
						}

						pred_ltrb[k] = dis * stride;
					}

					float pb_cx = (j + 0.5f) * stride;
					float pb_cy = (i + 0.5f) * stride;

					float x0 = pb_cx - pred_ltrb[0];
					float y0 = pb_cy - pred_ltrb[1];
					float x1 = pb_cx + pred_ltrb[2];
					float y1 = pb_cy + pred_ltrb[3];

					Object obj;
					obj.rect.x = x0;
					obj.rect.y = y0;
					obj.rect.width = x1 - x0;
					obj.rect.height = y1 - y0;
					obj.label = label;
					obj.prob = score;

					objects.push_back(obj);
				}
			}
		}

	}

	catch (std::exception ex)
	{

	}
}


std::vector<Object> NanoDetOnnx::detect(cv::Mat bgr, float score_threshold, float nms_threshold)
{
	
	// pad to multiple of 32
	int w = bgr.cols;
	int h = bgr.rows;
	float scale = 1.f;
	if (w > h)
	{
		scale = (float)m_target_size / w;
		w = m_target_size;
		h = h * scale;
	}
	else
	{
		scale = (float)m_target_size / h;
		h = m_target_size;
		w = w * scale;
	}

	// pad to target_size rectangle
	int wpad = (w + 31) / 32 * 32 - w;
	int hpad = (h + 31) / 32 * 32 - h;
	if (w > h)
	{
		hpad = m_target_size - h;
	}
	else
	{
		wpad = m_target_size - w;
	}

	cv::Mat dst;
	cv::resize(bgr,dst,cv::Size(w,h));


	//ncnn::copy_make_border
	cv::Mat image(m_target_size, m_target_size,CV_8UC3,cv::Scalar(255,255,255));

	dst.copyTo(image(cv::Rect(wpad / 2, hpad / 2, w, h)));

	
	
	//std::array<float, image.cols * image.rows*image.channels()> input_image_{};


    std::array<int64_t, 4> inputShape{1, image.channels(), image.rows, image.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);


	std::vector<float> inputTensorValues = preprocess(image);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    
	
	char* outputName2[] = { "cls_pred_stride_8","cls_pred_stride_16","cls_pred_stride_32","dis_pred_stride_8","dis_pred_stride_16","dis_pred_stride_32" };
    std::vector<Ort::Value> outputTensor = session->Run(Ort::RunOptions{nullptr}, &inputName, &inputTensor, 1, outputName2, 6);

    //assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());
    //double start = ncnn::get_current_time();


	std::vector<Object> proposals;
	// stride 8
	std::vector<Object> objects8;
	
	generate_proposalsOnnx(outputTensor[0], outputTensor[3],image,8, score_threshold, objects8);
	if (objects8.size() > 0)
	{
		proposals.insert(proposals.end(), objects8.begin(), objects8.end());
	}
	std::vector<Object> objects16;
	generate_proposalsOnnx(outputTensor[1], outputTensor[4], image,16, score_threshold, objects16);
	if (objects16.size() > 0)
	{
		proposals.insert(proposals.end(), objects16.begin(), objects16.end());
	}

	std::vector<Object> objects32;
	generate_proposalsOnnx(outputTensor[2], outputTensor[5], image, 32, score_threshold, objects32);
	if (objects32.size() > 0)
	{
		proposals.insert(proposals.end(), objects32.begin(), objects32.end());
	}

	// sort all proposals by score from highest to lowest
	qsort_descent_inplace(proposals);

	// apply nms with nms_threshold
	std::vector<int> picked;
	nms_sorted_bboxes(proposals, picked, nms_threshold);

	int count = picked.size();


	std::vector<Object> mobjects;
	mobjects.resize(count);
	int width = bgr.cols;
	int height = bgr.rows;
	
	for (int i = 0; i < count; i++)
	{
		mobjects[i] = proposals[picked[i]];

		// adjust offset to original unpadded
		float x0 = (mobjects[i].rect.x - (wpad / 2)) / scale;
		float y0 = (mobjects[i].rect.y - (hpad / 2)) / scale;
		float x1 = (mobjects[i].rect.x + mobjects[i].rect.width - (wpad / 2)) / scale;
		float y1 = (mobjects[i].rect.y + mobjects[i].rect.height - (hpad / 2)) / scale;

		// clip
		x0 = (std::max)((std::min)(x0, (float)(width - 1)), 0.f);
		y0 = (std::max)((std::min)(y0, (float)(height - 1)), 0.f);
		x1 = (std::max)((std::min)(x1, (float)(width - 1)), 0.f);
		y1 = (std::max)((std::min)(y1, (float)(height - 1)), 0.f);

		mobjects[i].rect.x = x0;
		mobjects[i].rect.y = y0;
		mobjects[i].rect.width = x1 - x0;
		mobjects[i].rect.height = y1 - y0;

	}

	// sort objects by area
	struct
	{
		bool operator()(const Object& a, const Object& b) const
		{
			return a.rect.area() > b.rect.area();
		}
	} objects_area_greater;

	std::sort(mobjects.begin(), mobjects.end(), objects_area_greater);


    return mobjects;
}

void NanoDetOnnx::decode_inferPlus(ncnn::Mat& feats, std::vector<CenterPrior>& center_priors,
	            float threshold, std::vector<std::vector<BoxInfo>>& results)
{
	const int num_points = center_priors.size();
	//printf("num_points:%d\n", num_points);

	//cv::Mat debug_heatmap = cv::Mat(feature_h, feature_w, CV_8UC3);
	for (int idx = 0; idx < num_points; idx++)
	{
		const int ct_x = center_priors[idx].x;
		const int ct_y = center_priors[idx].y;
		const int stride = center_priors[idx].stride;

		const float* scores = feats.row(idx);
		float score = 0;
		int cur_label = 0;
		for (int label = 0; label < this->num_class; label++)
		{
			if (scores[label] > score)
			{
				score = scores[label];
				cur_label = label;
			}
		}
		if (score > threshold)
		{
			if (cur_label == 1)
			{
				int kk = 0;
			}
			//std::cout << "label:" << cur_label << " score:" << score << std::endl;
			const float* bbox_pred = feats.row(idx) + this->num_class;
			results[cur_label].push_back(this->disPred2Bbox(bbox_pred, cur_label, score, ct_x, ct_y, stride));
			//debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
			//cv::imshow("debug", debug_heatmap);
		}
	}
}
//now 宽高等同
std::vector<Object> NanoDetOnnx::detectPlus(cv::Mat bgr, float score_threshold, float nms_threshold)
{

	// pad to multiple of 32
	int w = bgr.cols;
	int h = bgr.rows;
	float scale = 1.f;
	if (w > h)
	{
		scale = (float)m_target_size / w;
		w = m_target_size;
		h = h * scale;
	}
	else
	{
		scale = (float)m_target_size / h;
		h = m_target_size;
		w = w * scale;
	}

	// pad to target_size rectangle
	int wpad = (w + 31) / 32 * 32 - w;
	int hpad = (h + 31) / 32 * 32 - h;
	if (w > h)
	{
		hpad = m_target_size - h;
	}
	else
	{
		wpad = m_target_size - w;
	}

	cv::Mat dst;
	cv::resize(bgr, dst, cv::Size(w, h));


	//ncnn::copy_make_border
	cv::Mat image(m_target_size, m_target_size, CV_8UC3, cv::Scalar(0, 0, 0));

	dst.copyTo(image(cv::Rect(wpad / 2, hpad / 2, w, h)));



	//std::array<float, image.cols * image.rows*image.channels()> input_image_{};


	std::array<int64_t, 4> inputShape{ 1, image.channels(), image.rows, image.cols };

	auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);


	std::vector<float> inputTensorValues = preprocessPlus(image);
	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
		inputTensorValues.size(), inputShape.data(),
		inputShape.size());



	char * outputname = "output";
	std::vector<Ort::Value> outputTensor = session->Run(Ort::RunOptions{ nullptr }, &inputName, &inputTensor, 1, &outputname,1);

	//assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());
	//double start = ncnn::get_current_time();

	std::vector<CenterPrior> center_priors;
	generate_grid_center_priors(this->input_size[0], this->input_size[1], this->strides, center_priors);

	

	std::vector<Object> mobjects;
	

	std::vector<std::vector<BoxInfo>> results;
	results.resize(this->num_class);
	decode_infer(outputTensor[0], center_priors, score_threshold, results);


	int width = bgr.cols;
	int height = bgr.rows;

	

	std::vector<BoxInfo> dets;
	for (int i = 0; i < (int)results.size(); i++)
	{
		this->nms(results[i], nms_threshold);

		for (auto box : results[i])
		{
			Object obj;
			obj.rect = cv::Rect(box.x1,box.y1,box.x2- box.x1,box.y2- box.y1);
			obj.label = 1;
			dets.push_back(box);
			mobjects.push_back(obj);
		}
	}




	for (int i = 0; i < mobjects.size(); i++)
	{
		
		// adjust offset to original unpadded
		float x0 = (mobjects[i].rect.x - (wpad / 2)) / scale;
		float y0 = (mobjects[i].rect.y - (hpad / 2)) / scale;
		float x1 = (mobjects[i].rect.x + mobjects[i].rect.width - (wpad / 2)) / scale;
		float y1 = (mobjects[i].rect.y + mobjects[i].rect.height - (hpad / 2)) / scale;

		// clip
		x0 = (std::max)((std::min)(x0, (float)(width - 1)), 0.f);
		y0 = (std::max)((std::min)(y0, (float)(height - 1)), 0.f);
		x1 = (std::max)((std::min)(x1, (float)(width - 1)), 0.f);
		y1 = (std::max)((std::min)(y1, (float)(height - 1)), 0.f);

		mobjects[i].rect.x = x0;
		mobjects[i].rect.y = y0;
		mobjects[i].rect.width = x1 - x0;
		mobjects[i].rect.height = y1 - y0;

	}

	return mobjects;
}


std::vector<float> softmaxt(std::vector<float> input)
{
	float total = 0.;
	for (auto x : input)
	{
		total += exp(x);
	}
	std::vector<float> result;
	for (auto x : input)
	{
		result.push_back(exp(x) / total);
	}
	return result;
}


//now 宽高等同
float  NanoDetOnnx::detectfaceswapAnti(cv::Mat bgr)
{

	
	cv::Mat dst;
	cv::resize(bgr, dst, cv::Size(299, 299));


	cv::Mat image = dst.clone();


	//std::array<float, image.cols * image.rows*image.channels()> input_image_{};


	std::array<int64_t, 4> inputShape{ 1, image.channels(), image.rows, image.cols };

	auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);


	cv::cvtColor(image, image,CV_BGR2RGB);
	std::vector<float> inputTensorValues = preprocessAnti(image);
	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
		inputTensorValues.size(), inputShape.data(),
		inputShape.size());


	clock_t t1 = clock();
	char * outputname = "output";
	std::vector<Ort::Value> outputTensor = session->Run(Ort::RunOptions{ nullptr }, &inputName, &inputTensor, 1, &outputname, 1);

	//assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());
	//double start = ncnn::get_current_time();

	clock_t t2 = clock();


	std::vector<int64_t> outputShape = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape();

	int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1,
		std::multiplies<int64_t>());

	float *floatArray = outputTensor[0].GetTensorMutableData<float>();


	std::vector<float> inputs;
	inputs.push_back(floatArray[0]);
	inputs.push_back(floatArray[1]);

	std::vector<float> outputs = softmaxt(inputs);

	char szinfo[128];
	sprintf(szinfo,"%.2f %.2f %d", outputs[0], outputs[1],t2-t1);
	printf("anti onnxcost %.2f %.2f %d\n", outputs[0], outputs[1], t2 - t1);
	return outputs[1];
}


void NanoDetOnnx::decode_infer(Ort::Value &feats, std::vector<CenterPrior>& center_priors, float threshold, std::vector<std::vector<BoxInfo>>& results)
{
    std::vector<int64_t> outputShape = feats.GetTensorTypeAndShapeInfo().GetShape();

    int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1,
                                          std::multiplies<int64_t>());

    float *floatArray = feats.GetTensorMutableData<float>();
    // std::vector<float> outputData(floatArray, floatArray + outputCount);

    // printf("outputShape[0] = %d\n", outputShape[0]);
    // printf("outputShape[1] = %d\n", outputShape[1]);
    // printf("outputShape[2] = %d\n", outputShape[2]);

    const int num_points = center_priors.size();
    //printf("num_points:%d\n", num_points);
#if 1
    //cv::Mat debug_heatmap = cv::Mat(feature_h, feature_w, CV_8UC3);
    for (int idx = 0; idx < num_points; idx++) {
        const int ct_x = center_priors[idx].x;
        const int ct_y = center_priors[idx].y;
        const int stride = center_priors[idx].stride;

        const float* scores = floatArray + idx * outputShape[2];
        float score = 0;
        int cur_label = 0;
        for (int label = 0; label < this->num_class; label++) {
            if (scores[label] > score) {
                score = scores[label];
                cur_label = label;
            }
        }
        if (score > threshold) {
            //std::cout << "label:" << cur_label << " score:" << score << std::endl;
            const float* bbox_pred = floatArray + idx * outputShape[2] + this->num_class;
            results[cur_label].push_back(this->disPred2Bbox(bbox_pred, cur_label, score, ct_x, ct_y, stride));
            //debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
            //cv::imshow("debug", debug_heatmap);
        }
    }
#endif
}

BoxInfo NanoDetOnnx::disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride)
{
    float ct_x = x * stride;
    float ct_y = y * stride;
    std::vector<float> dis_pred;
    dis_pred.resize(4);
    for (int i = 0; i < 4; i++) {
        float dis = 0;
        float* dis_after_sm = new float[this->reg_max + 1];
        activation_function_softmax(dfl_det + i * (this->reg_max + 1), dis_after_sm, this->reg_max + 1);
        for (int j = 0; j < this->reg_max + 1; j++) {
            dis += j * dis_after_sm[j];
        }
        dis *= stride;
        //std::cout << "dis:" << dis << std::endl;
        dis_pred[i] = dis;
        delete[] dis_after_sm;
    }
    float xmin = (std::max)(ct_x - dis_pred[0], .0f);
    float ymin = (std::max)(ct_y - dis_pred[1], .0f);
    float xmax = (std::min)(ct_x + dis_pred[2], (float)this->input_size[0]);
    float ymax = (std::min)(ct_y + dis_pred[3], (float)this->input_size[1]);

    //std::cout << xmin << "," << ymin << "," << xmax << "," << xmax << "," << std::endl;
    return BoxInfo { xmin, ymin, xmax, ymax, score, label };
}

void NanoDetOnnx::nms(std::vector<BoxInfo>& input_boxes, float NMS_THRESH)
{
    std::sort(input_boxes.begin(), input_boxes.end(), [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
    std::vector<float> vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i) {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
                   * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_boxes.size()); ++i) {
        for (int j = i + 1; j < int(input_boxes.size());) {
            float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
            float w = (std::max)(float(0), xx2 - xx1 + 1);
            float h = (std::max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);
            if (ovr >= NMS_THRESH) {
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            } else {
                j++;
            }
        }
    }
}

//int resize_uniform(cv::Mat& src, cv::Mat& dst, cv::Size dst_size, object_rect& effect_area)
//{
//	int w = src.cols;
//	int h = src.rows;
//	int dst_w = dst_size.width;
//	int dst_h = dst_size.height;
//	//std::cout << "src: (" << h << ", " << w << ")" << std::endl;
//	dst = cv::Mat(cv::Size(dst_w, dst_h), CV_8UC3, cv::Scalar(0));
//
//	float ratio_src = w * 1.0 / h;
//	float ratio_dst = dst_w * 1.0 / dst_h;
//
//	int tmp_w = 0;
//	int tmp_h = 0;
//	if (ratio_src > ratio_dst) {
//		tmp_w = dst_w;
//		tmp_h = floor((dst_w * 1.0 / w) * h);
//	}
//	else if (ratio_src < ratio_dst) {
//		tmp_h = dst_h;
//		tmp_w = floor((dst_h * 1.0 / h) * w);
//	}
//	else {
//		cv::resize(src, dst, dst_size);
//		effect_area.x = 0;
//		effect_area.y = 0;
//		effect_area.width = dst_w;
//		effect_area.height = dst_h;
//		return 0;
//	}
//
//	//std::cout << "tmp: (" << tmp_h << ", " << tmp_w << ")" << std::endl;
//	cv::Mat tmp;
//	cv::resize(src, tmp, cv::Size(tmp_w, tmp_h));
//
//	if (tmp_w != dst_w) {
//		int index_w = floor((dst_w - tmp_w) / 2.0);
//		//std::cout << "index_w: " << index_w << std::endl;
//		for (int i = 0; i < dst_h; i++) {
//			memcpy(dst.data + i * dst_w * 3 + index_w * 3, tmp.data + i * tmp_w * 3, tmp_w * 3);
//		}
//		effect_area.x = index_w;
//		effect_area.y = 0;
//		effect_area.width = tmp_w;
//		effect_area.height = tmp_h;
//	}
//	else if (tmp_h != dst_h) {
//		int index_h = floor((dst_h - tmp_h) / 2.0);
//		//std::cout << "index_h: " << index_h << std::endl;
//		memcpy(dst.data + index_h * dst_w * 3, tmp.data, tmp_w * tmp_h * 3);
//		effect_area.x = 0;
//		effect_area.y = index_h;
//		effect_area.width = tmp_w;
//		effect_area.height = tmp_h;
//	}
//	else {
//		printf("error\n");
//	}
//	//cv::imshow("dst", dst);
//	//cv::waitKey(0);
//	return 0;
//}

#include <ncnn/nanodet/nanodet.h>

//
//
int image_demo(NanoDetOnnx &detector, const char* imagepath)
{
	// const char* imagepath = "D:/Dataset/coco/val2017/*.jpg";


	int height = detector.input_size[0];
	int width = detector.input_size[1];

	
		cv::Mat image = cv::imread(imagepath);
		if (image.empty()) {
			fprintf(stderr, "cv::imread %s failed\n", imagepath);
			return -1;
		}
		clock_t t1 = clock();
	
		cv::Mat resized_img;
		
		auto results = detector.detect(image, 0.35, 0.5);
		clock_t t2 = clock();
		printf("result size %d %d...\n", results.size(),t2-t1);

		draw_bboxes(image, results);
	
		

	return 0;
}


CNanoDetOnnxMgr * CNanoDetOnnxMgr::m_instance = NULL;
CNanoDetOnnxMgr::CNanoDetOnnxMgr()
{

}


CNanoDetOnnxMgr::~CNanoDetOnnxMgr()
{

}


NanoDetOnnx * CNanoDetOnnxMgr::GetOneObj(std::string pathmodel)
{
	NanoDetOnnx * objOut = NULL;
	m_mutex.lock();
	//printf("count******************************* %d \n", m_listObj.size());
	for (int k = 0; k < m_listObj.size(); ++k)
	{
		if (!m_listObj[k]->BUsing() && m_listObj[k]->m_pathmodel.find(pathmodel) != -1)
		{
			objOut = m_listObj[k];
			objOut->SetBUsing(true);
			break;
		}
	}

	if (objOut == NULL && m_listObj.size() < 100)
	{
		objOut = new  NanoDetOnnx((getgModelpath() + "/detection/" + pathmodel + ".onnx").c_str(),1);
		objOut->SetBUsing(true);
		m_listObj.push_back(objOut);
	}
	m_mutex.unlock();

	return objOut;
}
CNanoDetOnnxMgr * CNanoDetOnnxMgr::Instance()
{
	if (m_instance == NULL)
	{
		m_instance = new CNanoDetOnnxMgr;
	}
	return m_instance;
}

void CNanoDetOnnxMgr::SetObjNouse(NanoDetOnnx * obj)
{
	m_mutex.lock();
	for (int k = 0; k < m_listObj.size(); ++k)
	{
		if (m_listObj[k] == obj)
		{
			obj->SetBUsing(false);
			break;
		}
	}
	m_mutex.unlock();
}




/*

检测输出物体的框
*/
std::vector<cv::Rect> imagnanodetonnxRects(NanoDetOnnx *detector, cv::Mat image, float threadhold=0.38)
{
	std::vector<Object> object = detector->detect(image, threadhold,0.5);

#ifdef _WIN32
	//detector.draw(image);
#endif
	std::vector<cv::Rect> outRs;
	for (int i = 0; i < object.size(); ++i)
	{
		cv::Rect bd = object[i].rect;
		if (bd.width >= 4
			&& bd.height >= 4)
		{
			outRs.push_back(bd);
		}

	}

	return outRs;
}
std::vector<Object> imagnanodetonnxObjs(NanoDetOnnx *detector, cv::Mat image, float threadhold)
{
	std::vector<Object> object = detector->detect(image, threadhold, 0.5);
#ifdef _WIN32
	//draw_bboxes(image, object);
#endif
	return object;
}

std::vector<cv::Rect> nanodetPlusonnxRects(NanoDetOnnx *detector, cv::Mat image, float threadhold = 0.38)
{
	std::vector<Object> object = detector->detectPlus(image, threadhold, 0.5);

#ifdef _WIN32
	draw_bboxes(image, object);
#endif
	std::vector<cv::Rect> outRs;
	for (int i = 0; i < object.size(); ++i)
	{
		cv::Rect bd = object[i].rect;
		if (bd.width >= 4
			&& bd.height >= 4)
		{
			outRs.push_back(bd);
		}

	}

	return outRs;
}

std::vector<Object>  NanoDetOnnxObjsInter(cv::Mat img, std::string pathmodel, float thredhold)
{
	std::vector<Object> tRs;

	NanoDetOnnx * detector = CNanoDetOnnxMgr::Instance()->GetOneObj(pathmodel);
	if (detector != NULL)
	{
		tRs = imagnanodetonnxObjs(detector, img, thredhold);


		CNanoDetOnnxMgr::Instance()->SetObjNouse(detector);
	}

	return tRs;
}

std::vector<cv::Rect>  NanoDetOnnxRectsInter(cv::Mat img, std::string pathmodel, float thread)
{
	std::vector<cv::Rect> tRs;

	NanoDetOnnx * detector = CNanoDetOnnxMgr::Instance()->GetOneObj(pathmodel);
	if (detector != NULL)
	{
		tRs = imagnanodetonnxRects(detector, img, thread);


		CNanoDetOnnxMgr::Instance()->SetObjNouse(detector);
	}

	return tRs;
}
std::vector<cv::Rect>  NanoDetOnnxRectsPlusInter(cv::Mat img, std::string pathmodel, float thread)
{
	std::vector<cv::Rect> tRs;

	NanoDetOnnx * detector = CNanoDetOnnxMgr::Instance()->GetOneObj(pathmodel);
	if (detector != NULL)
	{
		tRs = nanodetPlusonnxRects(detector, img, thread);


		CNanoDetOnnxMgr::Instance()->SetObjNouse(detector);
	}

	return tRs;
}




float  SwapFaceAntiInter(cv::Mat img, std::string pathmodel)
{

	
	float fRes = 0;
	NanoDetOnnx * detector = CNanoDetOnnxMgr::Instance()->GetOneObj(pathmodel);
	if (detector != NULL)
	{
		fRes =  detector->detectfaceswapAnti(img);


		CNanoDetOnnxMgr::Instance()->SetObjNouse(detector);
	}
	clock_t  t2 = clock();




	return fRes;
	

}