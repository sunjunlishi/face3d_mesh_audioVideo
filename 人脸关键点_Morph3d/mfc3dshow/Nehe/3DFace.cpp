
/****************************************************************

  Author      :  sunjunlishi
  Version     :  1.0a
  Create Date :  220619
  Description :  

*****************************************************************/

// --------------------------------------------------------------------------------------


#include "3dFace.h"
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <iostream>
#include <sstream>
#include "../GLDevice.h"
#include <thread>
using namespace std;
using namespace cv;
using PosXYZ = std::array<double, 3>;
using RotationMatrix = std::array<double, 9>;

PosXYZ R_active_x(double angle, PosXYZ p) {
	angle = 3.14159265359*angle / 180.0;
	const auto c = std::cos(angle);
	const auto s = std::sin(angle);
	// clang-format off
	RotationMatrix ret = { 1.0,  0.0,  0.0,
		0.0,    c,   -s,
		0.0,    s,    c };
	PosXYZ n;

	n[0] = p[0];
	n[1] = c*p[1] - s*p[2];
	n[2] = s*p[1] + c*p[2];
	// clang-format on
	return n;
}

PosXYZ R_active_y(double angle, PosXYZ p) {
	angle = 3.1415926*angle / 180.0;

	const auto c = std::cos(angle);
	const auto s = std::sin(angle);
	// clang-format off
	RotationMatrix ret = { c,  0.0,    s,
		0.0,  1.0,  0.0,
		-s,  0.0,    c };
	// clang-format on
	PosXYZ n;

	n[0] = c*p[0] + s*p[2];
	n[1] = p[1];
	n[2] = -s*p[0] + c*p[2];
	// clang-format on
	return n;
}

PosXYZ R_active_z(double angle, PosXYZ p) {

	angle = 3.1415926*angle / 180.0;
	const auto c = std::cos(angle);
	const auto s = std::sin(angle);
	// clang-format off
	RotationMatrix ret = { c,   -s,  0.0,
		s,    c,  0.0,
		0.0,  0.0,  1.0 };
	// clang-format on
	PosXYZ n;

	n[0] = c*p[0] - s*p[1];;
	n[1] = s*p[0] + c*p[1];
	n[2] = p[2];
	// clang-format on
	return n;
}


C3DFace::C3DFace()
{
   

	oldmx = -1;
	mrotateYF = 0.0;
	mrotateXF = 0.0;
	mrotateZF = 0.0;



	

	mMapWeights = {
			{FaceBlendShape::GenericNeutralMesh, &mGenericNeutralMesh},
			{FaceBlendShape::JawOpen,            &mJawOpen},
			{FaceBlendShape::MouthPucker,        &mMouthPucker},
			{FaceBlendShape::MouthSmile_L,       &mMouthSmile_L},
			{FaceBlendShape::MouthSmile_R,       &mMouthSmile_R},
			{FaceBlendShape::MouthShrugLower,    &mMouthShrugLower},
			{FaceBlendShape::MouthShrugUpper,    &mMouthShrugUpper},
			{FaceBlendShape::MouthPress_L,       &mMouthPress_L},
			{FaceBlendShape::MouthPress_R,       &mMouthPress_R},
			{FaceBlendShape::MouthStretch_L,     &mMouthStretch_L},
			{FaceBlendShape::MouthStretch_R,     &mMouthStretch_R},
			{FaceBlendShape::MouthFunnel,        &mMouthFunnel},
			{FaceBlendShape::MouthRollUpper,     &mMouthRollUpper},
			{FaceBlendShape::MouthRollLower,     &mMouthRollLower},
			{FaceBlendShape::MouthUpperUp_L,     &mMouthUpperUp_L},
			{FaceBlendShape::MouthUpperUp_R,     &mMouthUpperUp_R},
			{FaceBlendShape::MouthLowerDown_L,   &mMouthLowerDown_L},
			{FaceBlendShape::MouthLowerDown_R,   &mMouthLowerDown_R},
			{FaceBlendShape::MouthLeft,          &mMouthLeft},
			{FaceBlendShape::MouthRight,         &mMouthRight},
			{FaceBlendShape::EyeBlink_L,         &mEyeBlink_L},
			{FaceBlendShape::EyeBlink_R,         &mEyeBlink_R},
			{FaceBlendShape::BrowInnerUp_L,      &mBrowInnerUp_L},
			{FaceBlendShape::BrowInnerUp_R,      &mBrowInnerUp_R},
			{FaceBlendShape::BrowDown_L,         &mBrowDown_L},
			{FaceBlendShape::BrowDown_R,         &mBrowDown_R},
			{FaceBlendShape::BrowOuterUp_L,      &mBrowOuterUp_L},
			{FaceBlendShape::BrowOuterUp_R,      &mBrowOuterUp_R}
		};
}

C3DFace::~C3DFace()
{

}



bool    C3DFace::Initialize(std::vector<string> list)
{
	m_listInfo = list;
    if (!LoadTexture(list))
    {
        return false;
    }

    return true;
}

bool gBstarted = false;
void SetStart(bool bStart)
{
	gBstarted = bStart;
}

void my_threadObjInit(C3DFace * d3Obj)
{
	d3Obj->DoInitObj();
}
void C3DFace::DoInitObj()
{
	std::vector<std::string> files;
	files.push_back("detection/generic_neutral_mesh.obj"); //0 SubTool-0-1332617  SubTool-4-6493626.OBJ
	
	files.push_back("detection/jawOpen.OBJ");//1张嘴

	files.push_back("detection/mouthPucker.OBJ");//2 合拢嘴撅嘴

	files.push_back("detection/mouthSmile_L.OBJ");//3 微笑
	files.push_back("detection/mouthSmile_R.OBJ");//4;

	files.push_back("detection/mouthShrugLower.OBJ");//5下嘴唇向下
	files.push_back("detection/mouthShrugUpper.OBJ");//6 抬起上嘴唇,上唇耸肩

	files.push_back("detection/mouthPress_L.OBJ");//7 嘴唇压
	files.push_back("detection/mouthPress_R.OBJ");//8

	files.push_back("detection/mouthStretch_L.OBJ");//9 拉伸嘴角
	files.push_back("detection/mouthStretch_R.OBJ");//10

	files.push_back("detection/mouthFunnel.OBJ");//11 张嘴撅嘴

	files.push_back("detection/mouthRollUpper.OBJ");//12 上嘴唇卷曲
	files.push_back("detection/mouthRollLower.OBJ");//13 下嘴唇卷曲

	files.push_back("detection/mouthUpperUp_L.OBJ");//14 上嘴唇抬起左
	files.push_back("detection/mouthUpperUp_R.OBJ");//15 上嘴唇抬起右

	files.push_back("detection/mouthLowerDown_L.OBJ");//16 下嘴唇降低左
	files.push_back("detection/mouthLowerDown_R.OBJ");//17 下嘴唇降低右

	files.push_back("detection/mouthLeft.OBJ");//18 眉毛向下
	files.push_back("detection/mouthRight.OBJ");//19 眉毛向下右

	files.push_back("detection/eyeBlink_L.OBJ"); //20闭眼
	files.push_back("detection/eyeBlink_R.OBJ");//21

	files.push_back("detection/browInnerUp_L.OBJ");//22
	files.push_back("detection/browInnerUp_R.OBJ");//23

	files.push_back("detection/browDown_L.OBJ");//24
	files.push_back("detection/browDown_R.OBJ");//25

	files.push_back("detection/browOuterUp_L.OBJ");//26 眉毛外升
	files.push_back("detection/browOuterUp_R.OBJ");//27 眉毛外升右



	
	//files.push_back("detection/cloth2.obj");//13
	//files.push_back("detection/hair4.OBJ");//13
	//files.push_back("detection/hair3.OBJ");//13
	//files.push_back("detection/hair2.OBJ");//13


	LoadTexture(files);
	bInitObjListEnd = true;
}
bool    C3DFace::Initialize(eos::core::Mesh& obj, cv::Mat mat)
{
	
	if (!LoadTexture(obj,mat))
	{
		return false;
	}

	if (!bInitObjListStart)
	{
		bInitObjListStart = true;

		//DoInitObj();
		std::thread t(my_threadObjInit, this);
		t.detach();
	    
	}


	return true;
}

 void   C3DFace::OnLbtnUp(int x, int y)
{
	oldmx = -1;
	oldmy = -1;
	m_bLbtnDown = false;
}

void    C3DFace::OnLbtnDwon(int x, int y) 
{
	
	oldmx = x;
	oldmy = y;
	m_bLbtnDown = true;
}

void  C3DFace::OnMouseMove(int x, int y)
{

	//if(oldmx != -1 && m_bLbtnDown)
	//{

	//	mrotateYF +=x-oldmx; //鼠标在窗口x轴方向上的增量加到视点绕y轴的角度上，这样就左右转了  

	//	mrotateXF +=0.8f*(y-oldmy); //鼠标在窗口y轴方向上的改变加到视点的y坐标上，就上下转了  

	//}

 //   oldmx=x,oldmy=y; //把此时的鼠标坐标作为旧值，为下一次计算增量做准备  

	
}

void     C3DFace::Destroy()
{
    glDisable(GL_TEXTURE_2D);
}


void    C3DFace::Execute(unsigned long deltaTime)
{
   
    if (deltaTime > 30)
    {
        deltaTime = 30;
    }

    
}
void C3DFace::DoLoop()
{
	

}

void    C3DFace::Render()
{
	
	if(bInitObjListEnd)
	//display();
	displayValue();
}

bool  C3DFace::LoadTexture(eos::core::Mesh& obj, cv::Mat mat)
{
	m_objLoader.LoadObjFromMesh(obj,mat);
	mdataObj = m_objLoader.ReturnPObj();
	return true;
}


//18 = 225  # right eyebrow outer - corner(18)
//19 = 229  # right eyebrow between middle and outer corner
//20 = 233  # right eyebrow middle, vertical middle(20)
//21 = 2086  # right eyebrow between middle and inner corner
//22 = 157  # right eyebrow inner - corner(19)

//23 = 590  # left eyebrow inner - corner(23)
//24 = 2091  # left eyebrow between inner corner and middle
//25 = 666  # left eyebrow middle(24)
//26 = 662  # left eyebrow between middle and outer corner
//27 = 658  # left eyebrow outer - corner(22)


//37   177; right eye outer - corner(1)
//38   172; right eye pupil top right(from subject's perspective)
//	39   191; right eye pupil top left
//	40   181; right eye inner - corner(5)
//	41   173; right eye pupil bottom left
//	42   174; right eye pupil bottom right

//43 = 614  # left eye inner - corner(8)
//44 = 624  # left eye pupil top right
//45 = 605  # left eye pupil top left
//46 = 610  # left eye outer - corner(2)
//47 = 607  # left eye pupil bottom left
//48 = 606  # left eye pupil bottom right


//28 = 2842  # bridge of the nose(parallel to upper eye lids)
//29 = 379  # middle of the nose, a bit below the lower eye lids
//30 = 272  # above nose - tip(1cm or so)
//31 = 114  # nose - tip(3)
//32 = 100  # right nostril, below nose, nose - lip junction
//33 = 2794  # nose - lip junction
//34 = 270  # nose - lip junction(28)
//35 = 2797  # nose - lip junction
//36 = 537  # left nostril, below nose, nose - lip junction

float getthedistance(int i1, int i2, ObjModel * obj)
{
	float f1 = fabs(obj->VertexArray[i1].X - obj->VertexArray[obj->TriangleArray[i2].Vertex[0]].X)
		+ fabs(obj->VertexArray[i1].Y - obj->VertexArray[obj->TriangleArray[i2].Vertex[0]].Y)
		+ fabs(obj->VertexArray[i1].Z - obj->VertexArray[obj->TriangleArray[i2].Vertex[0]].Z);

	float f2 = fabs(obj->VertexArray[i1].X - obj->VertexArray[obj->TriangleArray[i2].Vertex[1]].X)
		+ fabs(obj->VertexArray[i1].Y - obj->VertexArray[obj->TriangleArray[i2].Vertex[1]].Y)
		+ fabs(obj->VertexArray[i1].Z - obj->VertexArray[obj->TriangleArray[i2].Vertex[1]].Z);


	float f3 = fabs(obj->VertexArray[i1].X - obj->VertexArray[obj->TriangleArray[i2].Vertex[2]].X)
		+ fabs(obj->VertexArray[i1].Y - obj->VertexArray[obj->TriangleArray[i2].Vertex[2]].Y)
		+ fabs(obj->VertexArray[i1].Z - obj->VertexArray[obj->TriangleArray[i2].Vertex[2]].Z);

	return min(min(f1, f2), f3);
}




Vector3 Vector3_Sub(Vector3 src, Vector3 dest)
{
	return Vector3(src.x - dest.x, src.y - dest.y, src.z - dest.z);
}
void Vector3_norm(float& x, float& y, float& z)
{
	float tmp = sqrtf(x*x + y*y + z*z);
	x = x / tmp; y = y / tmp; z = z / tmp;
}


void Vector3_norm(Vector3& vec)
{
	float tmp = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	vec.x = vec.x / tmp;
	vec.y = vec.y / tmp;
	vec.z = vec.z / tmp;

}
Vector3 Vector3_cross(Vector3& v1, Vector3& v2)
{
	return Vector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


  void calculateNormal(float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float* normal) {
	normal[0] = (y2 - y1) * (z3 - z1) - (y3 - y1) * (z2 - z1);
	normal[1] = (z2 - z1) * (x3 - x1) - (x2 - x1) * (z3 - z1);
	normal[2] = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);
}

  void ScaleObj(ObjModel * obj, float fRatio, float disx, float disy, float disz,bool bR = false)
  {
	  for (int k = 0; k < obj->NumVertex; ++k)
	  {
		  obj->VertexArray[k].X = obj->VertexArray[k].X*fRatio + disx;
		  obj->VertexArray[k].Y = obj->VertexArray[k].Y*fRatio + disy;
		  if (bR)
		  {
			  obj->VertexArray[k].Z = -obj->VertexArray[k].Z*fRatio + disz;
		  }
		  else
		  {
			  obj->VertexArray[k].Z = obj->VertexArray[k].Z*fRatio + disz;
		  }
		
	  }
	 

  }
 
void CalcNormals(ObjModel * obj,bool bZRe =false)
{
	/*float fv = 1.0;
	if (bZRe)
	{
		fv = -1.0;
	}*/
	int _PolygonNum = obj->NumTriangle;
	Vector3 pv3;
	for (int i = 0; i<_PolygonNum; ++i)
	{
		{
			int iDex1 = obj->TriangleArray[i].Vertex[0];
			int iDex2 = obj->TriangleArray[i].Vertex[1];
			int iDex3 = obj->TriangleArray[i].Vertex[2];


			float norma3[3];
			calculateNormal(obj->VertexArray[iDex1].X, obj->VertexArray[iDex1].Y, obj->VertexArray[iDex1].Z,
				obj->VertexArray[iDex2].X, obj->VertexArray[iDex2].Y, obj->VertexArray[iDex2].Z,
				obj->VertexArray[iDex3].X, obj->VertexArray[iDex3].Y, obj->VertexArray[iDex3].Z, norma3);

			Vector3 v1 = Vector3_Sub(Vector3(obj->VertexArray[iDex1].X, obj->VertexArray[iDex1].Y, obj->VertexArray[iDex1].Z),
				Vector3(obj->VertexArray[iDex2].X, obj->VertexArray[iDex2].Y, obj->VertexArray[iDex2].Z));

			Vector3 v2 = Vector3_Sub(Vector3(obj->VertexArray[iDex1].X, obj->VertexArray[iDex1].Y, obj->VertexArray[iDex1].Z),
				Vector3(obj->VertexArray[iDex3].X, obj->VertexArray[iDex3].Y, obj->VertexArray[iDex3].Z));


			Vector3 tmp1 = Vector3_cross(v1, v2);

			Vector3 pv1 = Vector3(obj->VertexArray[iDex1].X, obj->VertexArray[iDex1].Y, obj->VertexArray[iDex1].Z);
			Vector3 pv2 = Vector3(obj->VertexArray[iDex2].X, obj->VertexArray[iDex2].Y, obj->VertexArray[iDex2].Z);
			pv3 = Vector3(obj->VertexArray[iDex3].X, obj->VertexArray[iDex3].Y, obj->VertexArray[iDex3].Z);

			pv1.nx += tmp1.x;
			pv1.ny += tmp1.y;
			pv1.nz += tmp1.z;

			pv2.nx += tmp1.x;
			pv2.ny += tmp1.y;
			pv2.nz += tmp1.z;

			pv3.nx += tmp1.x;
			pv3.ny += tmp1.y;
			pv3.nz += tmp1.z;

			Vector3_norm(pv1.nx, pv1.ny, pv1.nz);
			obj->gts1.push_back(pv1);

			Vector3_norm(pv2.nx, pv2.ny, pv2.nz);
			obj->gts2.push_back(pv2);

			Vector3_norm(pv3.nx, pv3.ny, pv3.nz);
			obj->gts3.push_back(pv3);
		}

	}


}




ObjModel * tmpObj= NULL;



void DoDrawObjGeneral(ObjModel *tObj1,float f1,float yix,float yiy,float yiz,bool bOutNormal)
{

	bool bFoure = false;

	for (int j = 0; j < tObj1->NumTriangle; ++j)//6736
	{

		int iCount = 3;
		if (tObj1->TriangleArray[j].bFoure)
		{
			glBegin(GL_QUADS);

			iCount = 4;
			bFoure = true;
		}
		else
		{
			glBegin(GL_TRIANGLES);
			bFoure = false;
		}

		for (int k = 0; k < iCount; k++)
		{
			int iDex = tObj1->TriangleArray[j].Vertex[k];

			int iDex1 = tObj1->TriangleArray[j].TexCoord[k];

			glColor3f(0.02, 0.02, 0.02);
			if (bOutNormal)
			{
				glNormal3f(tObj1->gts1[j].nx, tObj1->gts1[j].ny, tObj1->gts1[j].nz);//法向量
			}
			//glTexCoord2f(tObj1->TexCoordArray[iDex1].U, tObj1->TexCoordArray[iDex1].V);


			glVertex3f(tObj1->VertexArray[iDex].X*f1 - yix,
				tObj1->VertexArray[iDex].Y*f1 - yiy,
				(tObj1->VertexArray[iDex].Z*f1 - yiz));


		}

		glEnd();//结束绘图


	}

}
float gfmax = 0;
float gfmin = 1000;


float fmaxV = 0;
float fminV = 1000;
bool bCalted = false;
void DoDrawObjRxyz( int iTexID, int startID, int endID,float fRx,float fRy,float fRz, bool bChange = false, float yix = 0, float yiy = 0, float yiz = 0)
{
	glEnable(GL_TEXTURE_2D);
	if (iTexID == 10)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.5f,0.5f,0.5f };
		float ks[] = { 0.0,0.0,0.0 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);

	}
	if (iTexID == 11)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.0f,0.0f,0.0f };
		float ks[] = { 0.0,0.0,0.0 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);

	}

	if (iTexID == 5 || iTexID == 7)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.2f,0.2f,0.2f };
		float ks[] = { 0.0,0.0,0.0 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);

	}

	if (iTexID == 6 || iTexID == 4)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.6f,0.7f,0.6f };
		float ks[] = { 0.7,0.7,0.70 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 43.0);


		/*if (iTexID == 0)
		{
			float ka[] = { 0.7f, 0.6f, 0.6f }; 
		
			float kd[] = { 0.8f,0.7f,0.6f };
			float ks[] = { 0.8,0.7,0.70 };
			float ns[] = { 18.0 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 43.0);

		}*/

	}
	glBindTexture(GL_TEXTURE_2D, tmpObj->TextureArray()[iTexID]);
	glBegin(GL_QUADS);


	glShadeModel(GL_SMOOTH);

	if (!bCalted)
	{
		for (int j = startID; j <= endID; ++j)//6736
		{
			bool  bFoure = false;
			int iCount = 3;
			if (tmpObj->TriangleArray[j].bFoure)
			{
				iCount = 4;
				bFoure = true;
			}


			for (int k = 0; k < iCount; k++)
			{
				int iDex = tmpObj->TriangleArray[j].Vertex[k];

				if (tmpObj->VertexArray[iDex].Y > fmaxV)
				{
					fmaxV = tmpObj->VertexArray[iDex].Y;
				}

				if (tmpObj->VertexArray[iDex].Y < fminV)
				{
					fminV = tmpObj->VertexArray[iDex].Y;
				}
			}
		}
		if (gfmax < fmaxV)
		{
			gfmax = fmaxV;
		}
		fmaxV = gfmax;

		if (gfmin < fminV)
		{
			gfmin = fminV;
		}
		bCalted = true;
	}

	
	//fminV = gfmin;

	//fminV -=1.0;

	float  dis = (fmaxV - fminV)*0.6;
	
	

	bool bFoure = false;
	//Gums and tongue
	for (int j = startID; j <= endID; ++j)//6736
	{
		int iCount = 3;
		if (tmpObj->TriangleArray[j].bFoure)
		{
			iCount = 4;
			bFoure = true;
		}
		else
		{
			bFoure = false;
		}

		for (int k = 0; k < iCount; k++)
		{
			int iDex = tmpObj->TriangleArray[j].Vertex[k];

			int iDex1 = tmpObj->TriangleArray[j].TexCoord[k];

			glNormal3f(tmpObj->gts1[j].nx, tmpObj->gts1[j].ny, tmpObj->gts1[j].nz);//法向量
			

			if (j < 9230)
			{
				glTexCoord2f(tmpObj->TexCoordArray[iDex].U, tmpObj->TexCoordArray[iDex].V);
			}
			else
			{
				glTexCoord2f(tmpObj->TexCoordArray[iDex1].U, tmpObj->TexCoordArray[iDex1].V);
			}
		

			PosXYZ p0;
			p0[0] = tmpObj->VertexArray[iDex].X;
			p0[1] = tmpObj->VertexArray[iDex].Y;
			p0[2] = tmpObj->VertexArray[iDex].Z;

			float fa = 0.29;
			if (!bChange)
			{
				PosXYZ p1 = R_active_x(fRx, p0);
				PosXYZ p2 = R_active_y(fRy, p1);
				PosXYZ p3 = R_active_z(fRz, p2);



				glVertex3f(p3[0] - yix,
					p3[1] - yiy,
					p3[2] - yiz);
			}
		
			else if (tmpObj->VertexArray[iDex].Y >tmpObj->VertexArray[966].Y 
				|| (tmpObj->VertexArray[iDex].Y<(tmpObj->VertexArray[966].Y- dis)))
			{
				PosXYZ p1 = R_active_x(fRx, p0);
				PosXYZ p2 = R_active_y(fRy, p1);
				PosXYZ p3 = R_active_z(fRz, p2);

			

				glVertex3f(p3[0] - yix,
					p3[1] - yiy,
					p3[2] - yiz);
			}
			else
			{
				float fr = (tmpObj->VertexArray[966].Y-tmpObj->VertexArray[iDex].Y) / dis;
				fr = 1 - fr;
				fr *= 1.05;  if (fr > 1) fr = 1.0;
				fr = fr*fr*fr;

				PosXYZ p1 = R_active_x(fRx*fr, p0);
				PosXYZ p2 = R_active_y(fRy*fr, p1);
				PosXYZ p3 = R_active_z(fRz*fr, p2);


				glVertex3f(p3[0] - yix,
					p3[1] - yiy,
					p3[2] - yiz);
			}
			


				/*	glVertex3f(tmpObj->VertexArray[iDex].X - yix,
				tmpObj->VertexArray[iDex].Y - yiy,
				tmpObj->VertexArray[iDex].Z - yiz);*/

			

		}

	}
	glEnd();//结束绘图

	glDisable(GL_TEXTURE_2D);

}



void DoDrawObj(int iTexID,int startID,int endID,bool bOutNormal=false,float yix=0,float yiy=0,float yiz=0)
{
	glEnable(GL_TEXTURE_2D);
	if (iTexID == 10)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };
	
		float kd[] = {0.5f,0.5f,0.5f};
		float ks[] = {0.0,0.0,0.0};
		float ns[] = {18.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);
		
	}
	if (iTexID == 11)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.0f,0.0f,0.0f };
		float ks[] = { 0.0,0.0,0.0 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);

	}

	if (iTexID == 5 || iTexID == 7)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.2f,0.2f,0.2f };
		float ks[] = { 0.0,0.0,0.0 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 18.0);

	}

	if (iTexID == 6 || iTexID == 4)
	{
		float ka[] = { 0.0f, 0.0f, 0.0f };

		float kd[] = { 0.6f,0.7f,0.6f };
		float ks[] = { 0.7,0.7,0.70 };
		float ns[] = { 18.0 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 43.0);


		if (iTexID ==0)
		{
			float ka[] = { 0.7f, 0.6f, 0.6f };

			float kd[] = { 0.8f,0.7f,0.6f };
			float ks[] = { 0.8,0.7,0.70 };
			float ns[] = { 18.0 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ka);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kd);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 43.0);

		}

	}
	glBindTexture(GL_TEXTURE_2D, tmpObj->TextureArray()[iTexID]);
	glBegin(GL_QUADS);
	

	glShadeModel(GL_SMOOTH);
	bool bFoure = false;
	//Gums and tongue
	for (int j = startID; j <= endID; ++j)//6736
	{
		int iCount = 3;
		if (tmpObj->TriangleArray[j].bFoure)
		{
			iCount = 4;
			bFoure = true;
		}
		else
		{
			bFoure = false;
		}

		for (int k = 0; k < iCount; k++)
		{
			int iDex = tmpObj->TriangleArray[j].Vertex[k];

			int iDex1 = tmpObj->TriangleArray[j].TexCoord[k];

			

			
			if (bOutNormal)
			{
				glNormal3f(tmpObj->gts1[j].nx, tmpObj->gts1[j].ny, tmpObj->gts1[j].nz);//法向量
			}
			glTexCoord2f(tmpObj->TexCoordArray[iDex1].U, tmpObj->TexCoordArray[iDex1].V);

          
		

			glVertex3f(tmpObj->VertexArray[iDex].X- yix,
				tmpObj->VertexArray[iDex].Y-yiy,
				tmpObj->VertexArray[iDex].Z-yiz);


		}

	}
	glEnd();//结束绘图

	glDisable(GL_TEXTURE_2D);

}


int gIdex = 0;

float fEyemax = -1;



bool bTopBrown = false;
float fBrowv = 0.0;


bool bInitedEeye = false;
bool bFrist = true;


ObjModel * gObjIdentity[20];


ObjModel * gIdenNew=NULL;
bool bInitedObj = false;



std::vector<float> gRxs;
std::vector<float> gRys;
std::vector<float> gRzs;
float getResAvg(float fV,std::vector<float>& gRxs1)
{
	gRxs1.push_back(fV);
	if (gRxs1.size() >3)
	{
		gRxs1.erase(gRxs1.begin());
	}
	float sum = 0;
	for (int k = 0; k < gRxs1.size(); ++k)
	{
		sum += gRxs1[k];
	}
	return  sum / gRxs1.size();
}





void C3DFace::DoInitEyeOther()
{
	int iLimitP = 4527;
	//28点  4527 4942

	for (int k = 1; k < mObjsDiff.size(); ++k)
	{
        ObjModel * tmpObjDiff=  mObjsDiff[k];

		for (int iDex = 0; iDex < 21450; ++iDex)
		{
			tmpObjDiff->VertexArray[iDex].X = mObjs[k]->VertexArray[iDex].X - mObjs[0]->VertexArray[iDex].X;
			tmpObjDiff->VertexArray[iDex].Y = mObjs[k]->VertexArray[iDex].Y - mObjs[0]->VertexArray[iDex].Y;
			tmpObjDiff->VertexArray[iDex].Z = mObjs[k]->VertexArray[iDex].Z - mObjs[0]->VertexArray[iDex].Z;


		}
	}


	

	for (int k = 1; k < mObjs.size(); k++)
	{
		delete mObjs[k];
	}

}


void InitPos()
{
	glLoadIdentity();
	gluLookAt(0, 1.5f, 63, 0, -4.8f, 0, 0, 1, 0);
}


void C3DFace::displayValue(void)
{

	GLfloat green[] = { 0.85f,0.80f,0.75f,1.0f };
	GLfloat yellow[] = { 0.2f,0.1f,0.1f,1.0f };
	GLfloat white[] = { 0.2f,0.2f,0.2f,1.0f };
	GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 1.0f };

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);


	glShadeModel(GL_SMOOTH);




	ObjModel* tObEnd = mObjs[mObjs.size() - 1];//brown down
	if (bFrist)
	{

		CalcNormals(mdataObj);
		//CalcNormals(mObjs[mObjs.size() - 4]);
		//CalcNormals(mObjs[mObjs.size() - 3]);
		//CalcNormals(mObjs[mObjs.size() - 2]);
		//CalcNormals(mObjs[mObjs.size() - 1]);
		//SaveNormalsToBuffer(); // 存储到VBO
	}


	if (tmpObj == NULL)
	{
		gIdenNew = new ObjModel();
		tmpObj = new ObjModel();

		for (int k = 0; k < mObjsDiff.size(); k++)
		{
			ObjModel* item = new ObjModel();
			mObjsDiff[k] = item;

		}



		*tmpObj = *mObjs[0];
		CalcNormals(tmpObj);
		*gIdenNew = *mObjs[0];


		for (int k = 0; k < mObjsDiff.size(); k++)
		{
			*mObjsDiff[k] = *mObjs[1];

		}
	}


	//glRotated(180, 0, 1, 0);//绕(0,1,0)
	//char szinfo[256];
	float fRx = -mrotateXF * 180 / 3.1415926 * 0.55;
	float fRy = mrotateYF * 180 / 3.1415926 * 0.75;
	float fRz = mrotateZF * 180 / 3.1415926 * 0.35;

	fRx = getResAvg(fRx, gRxs);
	fRy = getResAvg(fRy, gRys);
	fRz = getResAvg(fRz, gRzs);

	//159 145  eyedis

	if (gBstarted)
	{



		//25351:26034
		//[26035:26718

		//  Eye blend left[24999:25022][25018:25032]	24	15
		//	Eye blend right[25023:25046][25033:25047]	24	15


		//#13	Eye occlusion left[25047:25198][25048:25175]	152	128
		//#14	Eye occlusion right[25199:25350][25176:25303]	152	128
		//Eye socket left[13294:13677][13226:13629]	384	404
		//Eye socket right[13678:14061]

		//1225, 1888, 1052, 367, 1719, 1722, 2199, 1447, 966, 3661, 4390, 3927, 3924, 2608, 3272, 4088, 3443, 
		//268, 493, 1914, 2044, 1401, 3615, 4240, 4114, 2734, 2509, 978, 4527, 4942, 4857, 1140, 2075, 1147, 4269, 3360, 1507, 1542, 1537, 1528, 1518, 1511, 3742, 3751, 3756, 3721, 3725, 3732, 5708, 5695, 2081, 0, 4275, 6200, 6213, 6346, 6461, 5518, 5957, 5841, 5702, 5711, 5533, 6216, 6207, 6470, 5517, 5966


		if (!bInitedEeye)
		{
			DoInitEyeOther();

			bInitedEeye = true;
		}




	   // displayValue() 中
		//
		if (m_weightsDirty)
		{
			for (int m = 1; m < mMapWeights.size(); m++)
			{
				ObjModel* item = mObjsDiff[m];
				float tWeight = *mMapWeights[(FaceBlendShape)m];

				for (int iDex = 0; iDex < 21450; ++iDex)
				{
					float fSumX = 0;
					float fSumY = 0;
					float fSumZ = 0;

					if (tWeight > 0)
					{
						fSumX += tWeight * item->VertexArray[iDex].X;
						fSumY += tWeight * item->VertexArray[iDex].Y;
						fSumZ += tWeight * item->VertexArray[iDex].Z;
					}
					if (m == 1)
					{
						tmpObj->VertexArray[iDex].X = fSumX;
						tmpObj->VertexArray[iDex].Y = fSumY;
						tmpObj->VertexArray[iDex].Z = fSumZ;
					}
					else
					{
						tmpObj->VertexArray[iDex].X += fSumX;
						tmpObj->VertexArray[iDex].Y += fSumY;
						tmpObj->VertexArray[iDex].Z += fSumZ;
					}
				

				}
			}

			for (int iDex = 0; iDex < 21450; ++iDex)
			{
				tmpObj->VertexArray[iDex].X += gIdenNew->VertexArray[iDex].X;
				tmpObj->VertexArray[iDex].Y += gIdenNew->VertexArray[iDex].Y;
				tmpObj->VertexArray[iDex].Z += gIdenNew->VertexArray[iDex].Z;

			}

			m_weightsDirty = false;
		}
				

		//[24591:24794][24692:24854]	204	163
		//#10	Lacrimal      [24795:24998][24855:25017]	204	163
		//#11	Eye blend left[24999:25022][25018:25032]	24	15
		//#12	Eye blend right[25023:25046][25033:25047]	24	1
		//随机眼睛进行动




		gIdex++;

		InitPos();

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// 自发光
		glMaterialfv(GL_FRONT, GL_EMISSION, green);
		// 环境光
		glMaterialfv(GL_FRONT, GL_AMBIENT, green);
		// 漫反射光
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		// 镜面反射光
		glMaterialfv(GL_FRONT, GL_SPECULAR, green);
		glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

		//DoDrawObj(&gtObjs[0], 0, 0, 6559, true);

		//DoDrawObj(&gtObjs[0],0, 6560, 9229,true);
		//DoDrawObj(&gtObjs[0], 1, 9230, 11143, true);//uv_headneck

		InitPos();
		//glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		//glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		//glRotated(fRz, 0, 0, 1);//绕(0,0,1)

		if (m_tietu)
		{
			DoDrawObjRxyz(11 + m_tietu, 0, 6559, fRx, fRy, fRz, true);
			DoDrawObjRxyz(11 + m_tietu, 6560, 9229, fRx, fRy, fRz, true);
		}
		else
		{
			DoDrawObjRxyz(0, 0, 6559, fRx, fRy, fRz, true);
			DoDrawObjRxyz(0, 6560, 9229, fRx, fRy, fRz, true);
		}


		DoDrawObjRxyz(1, 9230, 14033, fRx, fRy, fRz, true);



		glLoadIdentity();
		gluLookAt(0, 1.5f, 56, 0, -4.8f, 0, 0, 1, 0);

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)

		//脖子扭动有瑕疵，再绘制一次



		InitPos();

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)



		DoDrawObj(2, 14034, 17005, true);//uv_gums_tongue

		//teeth
		DoDrawObj(3, 17006, 21495, true);//uv_teeth



		float fxa = 0.0;
		float fya = 0.0;
		float fza = 0.0;
		for (int t = 21451; t <= 22220; ++t)
		{
			fxa += tmpObj->VertexArray[t].X;
			fya += tmpObj->VertexArray[t].Y;
			fza += tmpObj->VertexArray[t].Z;
		}
		fxa = fxa / (22220 - 21451 + 1);
		fya = fya / (22220 - 21451 + 1);
		fza = fza / (22220 - 21451 + 1);


		PosXYZ s1;
		s1[0] = fxa;
		s1[1] = fya;
		s1[2] = fza;
		PosXYZ p1 = R_active_x(mrotateXF, s1);
		PosXYZ p2 = R_active_y(mrotateYF, p1);
		PosXYZ p3 = R_active_z(mrotateZF, p2);

		InitPos();


		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)


		glTranslatef(p3[0], p3[1], p3[2] - 0.7); // 正方体的中心从（0.5,0.5,0.5)移动到（0,0,0)

		glRotated(mEyeRx, 1, 0, 0);//绕(1,0,0)
		glRotated(mEyeRy, 0, 1, 0);//绕(0,1,0)

		DoDrawObj(4, 21496, 22295, true, fxa, fya, fza);
		//DoDrawObj(&gtObjs[0], 5, 22296, 23093, true);



		fxa = 0.0;
		fya = 0.0;
		fza = 0.0;
		for (int t = 23021; t <= 23790; ++t)
		{
			fxa += tmpObj->VertexArray[t].X;
			fya += tmpObj->VertexArray[t].Y;
			fza += tmpObj->VertexArray[t].Z;
		}
		fxa = fxa / (23790 - 23021 + 1);
		fya = fya / (23790 - 23021 + 1);
		fza = fza / (23790 - 23021 + 1);


		s1[0] = fxa;
		s1[1] = fya;
		s1[2] = fza;
		p1 = R_active_x(mrotateXF, s1);
		p2 = R_active_y(mrotateYF, p1);
		p3 = R_active_z(mrotateZF, p2);

		InitPos();

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)


		glTranslatef(p3[0], p3[1], p3[2] - 0.7); // 正方体的中心从（0.5,0.5,0.5)移动到（0,0,0)


		//glRotated(fEyeRatio, 0, 1, 0);//绕(1,0,0)


		glRotated(mEyeRx, 1, 0, 0);//绕(1,0,0)
		glRotated(mEyeRy, 0, 1, 0);//绕(0,1,0)


		//Sclera right  Iris right
		DoDrawObj(6, 23094, 23893, true, fxa, fya, fza);

		//DoDrawObj(&gtObjs[0], 7, 23894, 24691, true);
		//两个6 7


		InitPos();

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//lacrimal fluid
		DoDrawObj(8, 24692, 25017, true);

		//blend
		///DoDrawObj(&gtObjs[0],9, 25018, 25047, true);


		//occlusion
		DoDrawObj(10, 25048, 25175, true);
		DoDrawObj(10, 25176, 25303, true);

		//eyelash
		//DoDrawObj(&gtObjs[0], 11, 25304, 26383,true);

		DoDrawObj(10, 25176, 25303, true);
		glDisable(GL_BLEND);

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHTING);
		// 自发光
		glMaterialfv(GL_FRONT, GL_EMISSION, gray);

		// 环境光
		glMaterialfv(GL_FRONT, GL_AMBIENT, gray);
		// 漫反射光
		glMaterialfv(GL_FRONT, GL_DIFFUSE, gray);
		// 镜面反射光
		glMaterialfv(GL_FRONT, GL_SPECULAR, gray);

		glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);
		//DoDrawObjGeneral(mObjs[mObjs.size()-2],242.2,0,145,0,true);

		InitPos();

		glRotated(fRx, 1, 0, 0);//绕(1,0,0)
		glRotated(fRy, 0, 1, 0);//绕(0,1,0)
		glRotated(fRz, 0, 0, 1);//绕(0,0,1)

		if (m_toushi)
		{

			//if (m_toushi == 4)
			//{
			//	//DoDrawObjGeneral(mObjs[mObjs.size() - 4], 1.17, 0, 179, -1.6, true);
			//	DoDrawObjGeneral(mObjs[mObjs.size() - 4], 55.17, 0, 5.50, -0.0, true);
			//}
			//if (m_toushi == 3)
			//{
			//	DoDrawObjGeneral(mObjs[mObjs.size() - 3], 83.75, 0, 121, -1.6, true);
			//}
			//if (m_toushi == 2)
			//{
			//	DoDrawObjGeneral(mObjs[mObjs.size() - 2], 14.75, 0, -1.3, -1.6, true);
			//}
			//if (m_toushi == 1)
			//{
			//	DoDrawObjGeneral(mObjs[mObjs.size() - 1], 1.149, 0, 180, -4, true);
			//}


		}


		glColor3f(0.85, 0.8, 0.7);

		glMaterialfv(GL_FRONT, GL_EMISSION, green);

		// 环境光
		glMaterialfv(GL_FRONT, GL_AMBIENT, green);
		// 漫反射光
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		// 镜面反射光
		glMaterialfv(GL_FRONT, GL_SPECULAR, green);





		bFrist = false;

	}
}


bool bInitIdentity= false;
void C3DFace::ChangeIdentityName(char *objname)
{
	if (!bInitIdentity)
    {
		for (int t = 0; t < 13; ++t)
		{
			char szname[256];
			sprintf(szname, "identity%03d.obj", t);

				ObjLoader* idenObjLoader = new ObjLoader();
				idenObjLoader->LoadObj(string("detection/") + szname);
				gObjIdentity[t] = idenObjLoader->ReturnPObj();
			
		}

        bInitIdentity = true;
    }
	
    //identity006.obj
	for (int iDex = 0; iDex <= 26718; ++iDex)
	{
		gIdenNew->VertexArray[iDex].X = mObjs[0]->VertexArray[iDex].X;
		gIdenNew->VertexArray[iDex].Y = mObjs[0]->VertexArray[iDex].Y;
		gIdenNew->VertexArray[iDex].Z = mObjs[0]->VertexArray[iDex].Z;
	}
	for (int t = 0; t < 13; ++t)
	{
		float fa = 0.3;
		for (int iDex = 0; iDex <= 26718; ++iDex)
		{
			gIdenNew->VertexArray[iDex].X += (gObjIdentity[t]->VertexArray[iDex].X - mObjs[0]->VertexArray[iDex].X)*(m_characterWeight[t]-7)*fa;
			gIdenNew->VertexArray[iDex].Y += (gObjIdentity[t]->VertexArray[iDex].Y - mObjs[0]->VertexArray[iDex].Y)*(m_characterWeight[t]-7)*fa;
			gIdenNew->VertexArray[iDex].Z += (gObjIdentity[t]->VertexArray[iDex].Z - mObjs[0]->VertexArray[iDex].Z)*(m_characterWeight[t]-7)*fa;
		}

	}


	gIdex = 0;
	
}

void C3DFace::DoReloadMat()
{
	if (bInitObjListEnd)
	{                     
	    cv:Mat img = cv::imread("./detection/uv_face5.png");
		flip(img, img, -1);
		tmpObj->LoadUVMat(11+5, img);
	}
 
}

bool  C3DFace::LoadTexture(std::vector<string> list)
{
	
	for (int k = 0; k < list.size(); ++k)
	{
		ObjLoader * tObjLoader = new ObjLoader();
		tObjLoader->LoadObj((char*)list[k].c_str());
		ObjModel * pdataObj = tObjLoader->ReturnPObj();
		mObjs.push_back(pdataObj);
	}

	mObjsDiff.resize(mObjs.size());
		
	
    return true;										// Return The Status
}


