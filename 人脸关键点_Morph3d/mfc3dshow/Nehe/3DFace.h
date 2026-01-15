
/****************************************************************

  File name   :  3DRotate.h
  Author      :  sunjunlishi
  Version     :  1.0a
  Create Date :  2016/03/21
  Description :  3D自动旋转广告牌功能

*****************************************************************/

// --------------------------------------------------------------------------------------

#ifndef _3DFACE_H_
#define _3DFACE_H_
#include <afxwin.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>


#include "YicEntity.h"
#include "ObjLoader.h"


enum class FaceBlendShape : int {
    GenericNeutralMesh = 0,   // 0
    JawOpen,                  // 1
    MouthPucker,              // 2
    MouthSmile_L,             // 3
    MouthSmile_R,             // 4
    MouthShrugLower,          // 5
    MouthShrugUpper,          // 6
    MouthPress_L,             // 7
    MouthPress_R,             // 8
    MouthStretch_L,           // 9
    MouthStretch_R,           // 10
    MouthFunnel,              // 11
    MouthRollUpper,           // 12
    MouthRollLower,           // 13
    MouthUpperUp_L,           // 14
    MouthUpperUp_R,           // 15
    MouthLowerDown_L,         // 16
    MouthLowerDown_R,         // 17
    MouthLeft,                // 18
    MouthRight,               // 19
    EyeBlink_L,               // 20
    EyeBlink_R,               // 21
    BrowInnerUp_L,            // 22
    BrowInnerUp_R,            // 23
    BrowDown_L,               // 24
    BrowDown_R,               // 25
    BrowOuterUp_L,            // 26
    BrowOuterUp_R,            // 27

    COUNT                     // 28
};


void SetStart(bool bStart);



class  C3DFace :public YicEntity
{
public:
	bool bInitObjListStart = false;
	bool bInitObjListEnd = false;
    C3DFace();

    ~C3DFace();
	void DoInitObj();
	bool        Initialize(std::vector<string> list);

    void        Destroy();
	bool        LoadTexture(eos::core::Mesh& obj,  cv::Mat mat);
    void        Execute(unsigned long deltaTime);

    void        Render();

	virtual void  OnMouseMove(int x, int y);
	virtual void  OnLbtnDwon(int x, int y) ;
	virtual void  OnLbtnUp(int x, int y);
	//void display(void);
	void displayValue(void);
	void ChangeIdentityName(char *objname);

	void DoReloadMat();

private:
	
	void DoInitEyeOther();
	ObjLoader m_objLoader;
	void DoLoop();

	bool Initialize(eos::core::Mesh& obj, cv::Mat mat);
    bool   LoadTexture(std::vector<string> list);

	CPoint m_ptOld;

	int oldmx;
	int oldmy;

private:
	std::vector<ObjModel*> mObjs;
    std::vector<ObjModel*> mObjsDiff;
	ObjModel * mdataObj;
	std::vector<string> m_listInfo;
    std::map<FaceBlendShape, float*> mMapWeights;


};

// --------------------------------------------------------------------------------------

#endif

