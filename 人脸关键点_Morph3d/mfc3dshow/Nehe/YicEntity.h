#pragma once

#include <afxwin.h>
#include <vector>
#include <opencv2\core.hpp>
#include <Common\Mesh.hpp>
class  YicEntity
{
public:
	YicEntity(void);
	~YicEntity(void);
	
	bool      OnCreate(CWnd * pWnd);
	void SetWeightChanged() { m_weightsDirty = true; }
	bool m_weightsDirty = true;
	virtual void  Render();
	virtual void Execute(unsigned long deltaTime);

	 virtual void   OnMouseMove(int x, int y) {}
	 virtual void   OnLbtnDwon(int x, int y) {}
	 virtual void   OnLbtnUp(int x, int y) {}
	 virtual bool   Is2DRender(){return false;}
	 virtual  void  Destroy(){};
	 virtual bool   Initialize(eos::core::Mesh& obj,cv::Mat mat){return false;};

	 virtual void DoReloadMat() {};
	 void RotateX(bool bPosi);
	 void RotateY(bool bPosi);
	 void RotateZ(bool bPosi);

	 void RotateXFv(float fV);
	 float GetRotateXFv() { return mrotateXF; }

	 void RotateYFv(float fV);
	 float GetRotateYFv() { return mrotateYF; }

	 void RotateZFv(float fV);
	 float GetRotateZFv() { return mrotateZF; }


	 virtual void ChangeIdentityName(char *objname) {};

	 void SetEyeRx(float fV);
	 float GetEyeRx() { return mEyeRx; }

	 void SetEyeRy(float fV);
	 float GetEyeRy() { return mEyeRy; }

	 void SetEyeBlinkV(float vV);
	 float GetEyeBlinkV() { return 	mEyeBlink_L; }

	 void Rerotate0();

	 virtual std::string GetMyClassName() { return ""; };

	 int m_toushi;
	 int m_tietu;

	 int m_characterWeight[100];//角色变形的权重
	 int m_characterStart;

public:
	float mGenericNeutralMesh = 0.f;  // 0
	float mJawOpen = 0.f;  // 1
	float mMouthPucker = 0.f;  // 2
	float mMouthSmile_L = 0.f;  // 3
	float mMouthSmile_R = 0.f;  // 4
	float mMouthShrugLower = 0.f;  // 5
	float mMouthShrugUpper = 0.f;  // 6
	float mMouthPress_L = 0.f;  // 7
	float mMouthPress_R = 0.f;  // 8
	float mMouthStretch_L = 0.f;  // 9
	float mMouthStretch_R = 0.f;  // 10
	float mMouthFunnel = 0.f;  // 11
	float mMouthRollUpper = 0.f;  // 12
	float mMouthRollLower = 0.f;  // 13
	float mMouthUpperUp_L = 0.f;  // 14
	float mMouthUpperUp_R = 0.f;  // 15
	float mMouthLowerDown_L = 0.f;  // 16
	float mMouthLowerDown_R = 0.f;  // 17
	float mMouthLeft = 0.f;  // 18
	float mMouthRight = 0.f;  // 19
	float mEyeBlink_L = 0.f;  // 20
	float mEyeBlink_R = 0.f;  // 21
	float mBrowInnerUp_L = 0.f;  // 22
	float mBrowInnerUp_R = 0.f;  // 23
	float mBrowDown_L = 0.f;  // 24
	float mBrowDown_R = 0.f;  // 25
	float mBrowOuterUp_L = 0.f;  // 26
	float mBrowOuterUp_R = 0.f;  // 27
protected:
	void DrawThePicPoint(cv::Mat dst,int showPosY);
protected:
	

	float mrotateXF;
	float mrotateYF;
	float mrotateZF;

	float mEyeRx;
	float mEyeRy;

	bool m_bLbtnDown;
	CWnd * m_pParentWnd;
};

