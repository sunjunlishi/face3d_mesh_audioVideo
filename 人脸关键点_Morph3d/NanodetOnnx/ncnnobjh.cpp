#include "ncnnobjh.h"
#include <SpecialEfect/SpecialEfect.h>
#include <math.h>
bool   cmpt(Object t1, Object t2) {
	return t1.prob > t2.prob;     // 可以简单理解为 >： 降序排列;  < ： 升序排列
}

 float intersection_area(const Object& a, const Object& b)
{
	cv::Rect_<float> inter = a.rect & b.rect;
	return inter.area();
}

bool compObjPosY12(const Object &a, const Object  &b)
{
	  return a.rect.y < b.rect.y;
 }


void nms_sorted_bboxesCombine(const std::vector<Object>& faceobjects, std::vector<int>& picked, float nms_threshold)
{
	picked.clear();

	const int n = faceobjects.size();

	std::vector<float> areas(n);
	for (int i = 0; i < n; i++)
	{
		areas[i] = faceobjects[i].rect.width * faceobjects[i].rect.height;
	}

	for (int i = 0; i < n; i++)
	{
		const Object& a = faceobjects[i];

		int keep = 1;
		for (int j = 0; j < (int)picked.size(); j++)
		{
			const Object& b = faceobjects[picked[j]];

			// intersection over union
			float inter_area = intersection_area(a, b);
			float union_area = areas[i] + areas[picked[j]] - inter_area;
			// float IoU = inter_area / union_area
			if (inter_area / union_area > nms_threshold
				|| inter_area / (a.rect.width*a.rect.height + 0.001)>nms_threshold
				|| inter_area / (a.rect.width*a.rect.height + 0.001)>nms_threshold)
				keep = 0;
		}

		if (keep)
			picked.push_back(i);
	}
}
