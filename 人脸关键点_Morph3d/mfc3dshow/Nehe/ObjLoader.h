#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include"gl/glaux.h"							// GLaux库的头文件
#include <Common\Mesh.hpp>
#include <opencv2\core.hpp>
//extern double xAngle , yAngle , zAngle ;
class Vector3
{
public:
	Vector3() :x(0), y(0), z(0) {
		nx = 0;
		ny = 0;
		nz = 0;
	}
	Vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {
		nx = 0.0;
		ny = 0.0;
		nz = 0.0;


	}
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
};
using namespace std;

	struct ObjVertex{
		 float X, Y, Z;
	};

	struct ObjNormal{
		 double X, Y, Z;
	};

	struct ObjTexCoord{
		 float U, V;
	};

	struct ObjTriangle{
		unsigned int Vertex[4];
		unsigned int Normal[4];
		unsigned int TexCoord[4];
		bool bFoure;
		ObjTriangle()
		{
			bFoure = false;
		}
	};

	struct ObjModel {
		public:
			std::vector<Vector3> gts1;
			std::vector<Vector3> gts2;
			std::vector<Vector3> gts3;

			std::vector<Vector3> gts11;
			std::vector<Vector3> gts12;
			std::vector<Vector3> gts13;
			ObjModel();
			~ObjModel();
			void loadUvFile(std::string filename);

			void LoadUVMat(int mtlC, cv::Mat mimg);

			ObjModel(const ObjModel& copy);
			ObjModel& operator=(const ObjModel& right);
			void InitTxt();

			void FreeObjMdel(void);
			
			unsigned int NumVertex, NumNormal, NumTexCoord, NumTriangle, NumTexture, NumMtl, *TextureMark;

			ObjVertex *VertexArray;
			ObjNormal *NormalArray;
			ObjTexCoord *TexCoordArray;

			ObjTriangle *TriangleArray;

			

			GLuint	**TextureMarkArray;


			std::vector<std::string> mTxtFiles;
	public:
		GLuint * TextureArray() {
			InitTxt();
			return mTextureArray;
		}
	private:
		
		GLuint *mTextureArray;
			

	};

	class ObjLoader  {
		public:
			int flag;
			int mtlC;
		
			ObjLoader();
			~ObjLoader();
			cv::Mat m_mattexture;
			ObjLoader(string file);
			void LoadObj(string file);
			
			void LoadObjFromMesh(eos::core::Mesh& meshInfo, cv::Mat mimg);
			void FreeObj(void);
			ObjModel ReturnObj(void);
			ObjModel * ReturnPObj(void);
			struct FaceVertex {
				int pos = -1;
				int tex = -1;
				int norm = -1;
			};

			void ParseVertex(const char* str, FaceVertex& v);
			
		protected:
			string  mfileName;
			ObjModel *theObj;
		
			void LoadMtl(cv::Mat mimg);
			void LoadMtl(std::string filename);
			void ReadData(void);
			void ReadDataFromMesh(eos::core::Mesh& meshInfo, cv::Mat mimg);
	};
	AUX_RGBImageRec * auxDIBImageLoadMy(cv::Mat img);
	
#endif