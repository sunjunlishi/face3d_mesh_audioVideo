#pragma once

#include <opencv2/core.hpp>


struct HPoint {
	float x;
	float y;
	float prob;
};

struct Landmarks
{
	float x;
	float y;
	float score;
};

typedef struct BoxInfo
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score;
	int label;
} BoxInfo;

struct TextBox {
	std::vector<cv::Point> boxPoint;
	float score;
	std::string text;
	double time;
};

struct ScaleParam {
	int srcWidth;
	int srcHeight;
	int dstWidth;
	int dstHeight;
	float ratioWidth;
	float ratioHeight;
	float scale;
	int padLeft;  // 新增左填充
	int padTop;    // 新增上填充
};




struct TextLine {
	std::string text;
	std::vector<float> charScores;
	double time;
};

struct Angle {
	int index;
	float score;
	double time;
};

struct Object
{
	cv::Rect_<float> rect;
	float x;
	float y;
	float w;
	float h;
	int label;
	float prob;
	//other define
	float score;
	float x1;
	float y1;
	float x2;
	float y2;
	HPoint landmark[5];
	std::vector<cv::Point2f> pts;
	
};


enum filterType
{
	filtertype_none,
	filtertype_companyname,
	filtertype_type,
	filtertype_address,
	filtertype_phonenum,
	filtertype_idnum,
	filtertype_date
};

struct item_pos
{
	int px;
	int py;
	int cy;
	int pw;
	int ph;
	std::string text;
};
enum zhizhao_yolotype
{
	zhiyolo_ma,//m_comboProper.AddString("信用代码");//为了防止误判0
	zhiyolo_ma_v1,//m_comboProper.AddString("信用代码V1");//为了防止误判1

	zhiyolo_name,//.AddString("名称");2
	zhiyolo_name_v1,//.AddString("名称V1");3

	zhiyolo_type,//.AddString("类型");4
	zhiyolo_type_v1,//.AddString("类型V1");5

	zhiyolo_person,//.AddString("代表人");6
	zhiyolo_person_v1,//.AddString("代表人V1");7

	zhiyolo_money,//.AddString("注册资本");//为了防止误判8
	zhiyolo_money_v1,//.AddString("注册资本V1");//为了防止误判9

	zhiyolo_start,//.AddString("成立日期");//为了防止误判10
	zhiyolo_start_v1,//,AddString("成立日期V1");//为了防止误判11


	zhiyolo_spantime,//.AddString("营业期限");//为了防止误判12
	zhiyolo_spantime_v1,//.AddString("营业期限V1");//为了防止误判13

	zhiyolo_address,//.AddString("住所");//为了防止误判14
	zhiyolo_address_v1,//AddString("住所V1");//为了防止误判15
	zhiyolo_address_v2,//.AddString("住所V2");//为了防止误判16
	zhiyolo_address_v3,//.AddString("住所V3");//为了防止误判17

	zhiyolo_fanwei,//AddString("经营范围");
	zhiyolo_fanwei_v1,//AddString("经营范围V1");

	zhiyolo_name_v2,//公司名称v2
	zhiyolo_issueautho,//签发机关
	zhiyolo_issueautho_V1,//签发机关V1
	zhiyolo_spantime_v2,//有效期限v2
	zhiyolo_zhizhaotype_v1,//执照类型值v1

	zhiyolo_None = 3000
};

enum typeZhizhao
{
	zhizhao_name,//机构名称0
	zhizhao_daima,//代码1
	zhizhao_startTime,//注册时间2
	zhizhao_type,//类型3
	zhizhao_personname,//代表人4
	zhizhao_money,//注册资本5
	zhizhao_fanwei,//经营范围6
	zhizhao_address,//注册地址7
	zhizhao_spantime,//有效期8
	zhizhao_zhizhaotype,//有效期8

};


struct s_zhizhaoitem
{
	std::vector<cv::Point> pts;
	zhizhao_yolotype itype;
};

struct s_textrect_reco
{
	zhizhao_yolotype mProperty;
	cv::Rect mR;
	std::string mstr;
	std::string mstrveri;
	std::string mstrWai;//outner key value
	filterType mType;
	std::vector<cv::Point> mpts;//four corner points
	float  mangle;
	int iDex;
	bool bRecoed;
	s_textrect_reco()
	{
		mangle = 0;
		iDex = -1;
		bRecoed = false;
	}
};



