#include "ObjLoader.h"
#include"gl/glaux.h"			
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
// GLaux库的头文件
#pragma comment(lib, "legacy_stdio_definitions.lib")
using namespace std;
double xAngle = 0, yAngle = 0, zAngle = 0;

ObjModel::ObjModel(): 	NumNormal (0),
	NumTexCoord(0),
	NumTriangle(0),
	NumVertex(0),	
	NumTexture(0),
	NumMtl(0),
	NormalArray(NULL),
	TexCoordArray(NULL),
	TriangleArray(NULL),
	VertexArray(NULL),
	TextureMark(NULL),
	mTextureArray(NULL),
    TextureMarkArray(NULL) {}

ObjModel::~ObjModel() 
{
	FreeObjMdel();
}



void ObjModel::InitTxt()
{
	if (mTextureArray == NULL)
	{
			NumMtl = 17;
			mTextureArray = new GLuint[NumMtl];
			glGenTextures(NumMtl, mTextureArray);


			for (int t = 0; t < mTxtFiles.size(); ++t)
			{

				std::string filename = mTxtFiles[t];
				std::string srcfile = filename;
				if (filename.find(".mtl") != -1)
				{
					filename = filename.replace(filename.find(".mtl"), 4, ".png");
					cv::Mat img = cv::imread("detection/" + filename, -1);
					if (img.empty())
					{
						filename = srcfile.replace(srcfile.find(".mtl"), 4, ".jpg");
						img = cv::imread("detection/" + filename);
					}
					else
					{

					}
					if (!img.empty())
					{
						if (filename.find("uv_face") != -1)
						{
							flip(img, img, -1);
						}
						LoadUVMat(t,img);
					}
				}
			}


		
	}
	
	
   
}

void ObjModel::FreeObjMdel(void)
{
	//
	if(NormalArray!= NULL){ delete [] NormalArray;NormalArray= NULL;}
	if(TexCoordArray!= NULL){ delete [] TexCoordArray;TexCoordArray= NULL;}
	if(TriangleArray!= NULL){ delete [] TriangleArray;TriangleArray= NULL;}
	if(VertexArray!= NULL){ delete [] VertexArray;VertexArray= NULL;}
	if(TextureMark!= NULL){ delete [] TextureMark;TextureMark= NULL;}
	if(mTextureArray!= NULL){ delete [] mTextureArray; mTextureArray = NULL;}
	if(TextureMarkArray!= NULL){ delete [] TextureMarkArray;TextureMarkArray= NULL;}
}

void ObjModel::loadUvFile(std::string filename)
{
	mTxtFiles.push_back(filename);
}

ObjModel::ObjModel(const ObjModel &copy)
{

	ObjModel::TexCoordArray= new ObjTexCoord[copy.NumTexCoord];
	ObjModel::TriangleArray= new ObjTriangle[copy.NumTriangle];
	ObjModel::VertexArray= new ObjVertex[copy.NumVertex];
	ObjModel::NormalArray = new ObjNormal[copy.NumVertex];

	ObjModel::TextureMark= new unsigned int[copy.NumTexture];
	//ObjModel::TextureArray= new GLuint[copy.NumMtl];
	ObjModel::TextureMarkArray= new GLuint *[copy.NumTexture];
	ObjModel::NumNormal= copy.NumNormal;
	ObjModel::NumTexCoord= copy.NumTexCoord;
	ObjModel::NumTriangle= copy.NumTriangle;
	ObjModel::NumVertex= copy.NumVertex;
	ObjModel::NumTexture=copy.NumTexture;
	ObjModel::NumMtl=copy.NumMtl;



	for(int i= 0; i< NumVertex; ++i)
		ObjModel::NormalArray[i] = copy.NormalArray[i];

	for (int i = 0; i< NumVertex; ++i)
		ObjModel::VertexArray[i] = copy.VertexArray[i];


	for(int i= 0; i < NumTexCoord; ++i)
		ObjModel::TexCoordArray[i] = copy.TexCoordArray[i];

	for(int i= 0; i< NumTriangle; ++i)
		ObjModel::TriangleArray[i] = copy.TriangleArray[i];

	

	for(int i= 0; i <NumTexture; ++i)
	{
		ObjModel::TextureMarkArray[i] = copy.TextureMarkArray[i];
		ObjModel::TextureMark[i] = copy.TextureMark[i];
	}

	//for(int i= 0; i< NumMtl; ++i)
	//	ObjModel::TextureArray[i] = copy.TextureArray[i];

	
	 ObjModel::mTxtFiles = mTxtFiles;
	

}	

ObjModel& ObjModel::operator=(const ObjModel &right)
{
	FreeObjMdel();

	ObjModel::mTxtFiles = right.mTxtFiles;

	TexCoordArray = new ObjTexCoord[right.NumTexCoord];
	TriangleArray = new ObjTriangle[right.NumTriangle];

	NormalArray = new ObjNormal[right.NumVertex];
	VertexArray = new ObjVertex[right.NumVertex];
	TextureMark = new unsigned int[right.NumTexture];
	//TextureArray = new GLuint[right.NumMtl];
	TextureMarkArray = new GLuint *[right.NumTexture];
	NumNormal = right.NumNormal;
	NumTexCoord = right.NumTexCoord;
	NumTriangle = right.NumTriangle;
	NumVertex = right.NumVertex;
	NumTexture = right.NumTexture;
	NumMtl = right.NumMtl;
	

	

	for(int i = 0; i < NumTexCoord; i++)
		TexCoordArray[i] = right.TexCoordArray[i];

	for(int i = 0; i < NumTriangle; i++)
		TriangleArray[i] = right.TriangleArray[i];

	//for (int i = 0; i < NumVertex; i++)
	//	NormalArray[i] = right.NormalArray[i];

	for(int i = 0; i < NumVertex; i++)
		VertexArray[i] = right.VertexArray[i];

	for(int i= 0; i <NumTexture; i++)
	{
		ObjModel::TextureMarkArray[i] = right.TextureMarkArray[i];
		ObjModel::TextureMark[i] = right.TextureMark[i];
	}

	//for(int i= 0; i< NumMtl; i++)
	//	ObjModel::TextureArray[i] = right.TextureArray[i];

	return *this;
}


ObjLoader::ObjLoader() 
{
	theObj = NULL;

}

ObjLoader::~ObjLoader() 
{
	FreeObj();
}

void ObjLoader::FreeObj(void)
{

	if(theObj != NULL){ delete theObj;theObj= NULL;}
	
}

ObjModel ObjLoader::ReturnObj(void)
{
	return *theObj;
}


ObjModel * ObjLoader::ReturnPObj(void)
{
	return theObj;
}

void ObjModel::LoadUVMat(int mtlC, cv::Mat mimg)
{

	if (mimg.empty()) return;
	AUX_RGBImageRec * TextureImage = auxDIBImageLoadMy(mimg);
	
	glBindTexture(GL_TEXTURE_2D, mTextureArray[mtlC]);

	if (mimg.channels() == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->data);

	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX,
			TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
	}

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线形滤波
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线形滤波


	delete	TextureImage->data;
	delete TextureImage;
}

ObjLoader::ObjLoader(string file)  
{
	mfileName = file;
	theObj = new ObjModel();
	 flag = 0;
	ReadData();
}
void ObjLoader::LoadObjFromMesh(eos::core::Mesh& meshInfo,cv::Mat mimg)
{
	//FreeObj();
	if (theObj == NULL)
	{
		theObj = new ObjModel();
	}

	ReadDataFromMesh(meshInfo,mimg);
}

void ObjLoader::LoadObj(string file) 
{
	FreeObj();
	mfileName = file;
	theObj = new ObjModel();


	theObj->NumMtl = 0;
	mtlC = 0;
	
	ReadData();


}


AUX_RGBImageRec * auxDIBImageLoadMy(cv::Mat img)
{
	if (!img.empty())
	{
		AUX_RGBImageRec * rgbdata = new AUX_RGBImageRec;

		if (img.channels() == 4)
		{
			cv::cvtColor(img, img, CV_BGRA2RGBA);
		}
		else
			cv::cvtColor(img, img, CV_BGR2RGB);


		flip(img, img, 0);

		rgbdata->sizeX = img.cols;
		rgbdata->sizeY = img.rows;
		rgbdata->data = new unsigned char[img.cols*img.rows * 4];
		memcpy(rgbdata->data, img.data, img.cols*img.rows*img.channels());
		return rgbdata;
	}
	

}
void ObjLoader::LoadMtl(std::string filename)
{
	theObj->loadUvFile(filename);

}


void ObjLoader::LoadMtl(cv::Mat mimg)
{
	AUX_RGBImageRec * TextureImage  = auxDIBImageLoadMy(mimg);

	glBindTexture(GL_TEXTURE_2D, theObj->TextureArray()[mtlC]);

	if (mimg.channels() == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
			TextureImage->sizeX, TextureImage->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureImage->data);

	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX,
			TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
	}
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 线形滤波
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 线形滤波
					
					
	delete TextureImage->data;
	delete TextureImage;
			
	mtlC++;
			
			
}

void ObjLoader::ReadDataFromMesh(eos::core::Mesh& meshInfo,cv::Mat mimg)
{

	if (theObj->TexCoordArray == NULL)
	{
		theObj->NumVertex = meshInfo.vertices.size();
		theObj->NumTexCoord = meshInfo.texcoords.size();
		theObj->NumTriangle = meshInfo.tvi.size();
		theObj->NumTexture = 1;

		theObj->NormalArray = new ObjNormal[theObj->NumVertex];
		theObj->VertexArray = new ObjVertex[theObj->NumVertex];

		theObj->TexCoordArray = new ObjTexCoord[theObj->NumTexCoord];
		theObj->TriangleArray = new ObjTriangle[theObj->NumTriangle];

		theObj->TextureMark = new unsigned int[theObj->NumTexture];
		theObj->TextureMarkArray = new GLuint *[theObj->NumTexture];

	}

	

	int nC, vC, tC, fC, mC, sC;
	nC = vC = tC = fC = mC = sC = 0;


	for (int tC = 0; tC < meshInfo.texcoords.size(); ++tC)
	{
		theObj->TexCoordArray[tC].U = meshInfo.texcoords[tC].x;
		theObj->TexCoordArray[tC].V = 1.0f - meshInfo.texcoords[tC].y;
	}

	for (int vC = 0; vC < meshInfo.vertices.size(); ++vC)
	{
		theObj->VertexArray[vC].X = meshInfo.vertices[vC].x;
		theObj->VertexArray[vC].Y = meshInfo.vertices[vC].y;
		theObj->VertexArray[vC].Z = meshInfo.vertices[vC].z;

	
	}

	for (int vC = 0; vC < meshInfo.colors.size(); ++vC)
	{
		theObj->NormalArray[vC].X = meshInfo.colors[vC].x;
		theObj->NormalArray[vC].Y = meshInfo.colors[vC].y;
		theObj->NormalArray[vC].Z = meshInfo.colors[vC].z;

	}


	 for(int fC=0;fC < meshInfo.tvi.size();++fC)
	{
		theObj->TriangleArray[fC].Vertex[0] = meshInfo.tvi[fC][0];
		theObj->TriangleArray[fC].TexCoord[0] = meshInfo.tvi[fC][0];
		theObj->TriangleArray[fC].Normal[0] = meshInfo.tvi[fC][0];

		theObj->TriangleArray[fC].Vertex[1] = meshInfo.tvi[fC][1];
		theObj->TriangleArray[fC].TexCoord[1] = meshInfo.tvi[fC][1];
		theObj->TriangleArray[fC].Normal[1] = meshInfo.tvi[fC][1];

		theObj->TriangleArray[fC].Vertex[2] = meshInfo.tvi[fC][2];
		theObj->TriangleArray[fC].TexCoord[2] = meshInfo.tvi[fC][2];
		theObj->TriangleArray[fC].Normal[2] = meshInfo.tvi[fC][2];

	}

	 if (m_mattexture.empty() && !mimg.empty())
	 {
		 
		 m_mattexture = mimg;
		LoadMtl(m_mattexture);
	 }
	
}
void ObjLoader::ParseVertex(const char* str, FaceVertex& v)
{
	const char* p = str;

	// 解析顶点位置
	v.pos = atoi(p) - 1;

	// 查找斜线
	const char* slash1 = strchr(p, '/');
	if (!slash1) return;

	// 解析纹理坐标
	if (*(slash1 + 1) != '/') {
		v.tex = atoi(slash1 + 1) - 1;
	}

	// 查找第二个斜线
	const char* slash2 = strchr(slash1 + 1, '/');
	if (slash2) {
		v.norm = atoi(slash2 + 1) - 1;
	}
}



void ObjLoader::ReadData(void)  
{
	string buffer,mtlfilename;

	ifstream input(mfileName);


	if( !input)
	{
		//cerr<<"error: unable to open input file: "<<input<<endl;
		return;
	}
	std::vector<std::string> lines;
	while( !input.eof() )  {
		getline(input, buffer);
		lines.push_back(buffer);
		if(strncmp(buffer.c_str(), "vn", 2) == 0)
			theObj->NumNormal++;
		else if(strncmp(buffer.c_str(), "vt", 2) == 0)
			theObj->NumTexCoord++;
		else if(strncmp(buffer.c_str(), "v", 1) == 0)/////////////////////////////判断顺序极其重要，不能颠倒！！！
			theObj->NumVertex++; 
		else if(strncmp(buffer.c_str(), "f", 1) == 0)
			theObj->NumTriangle++;
		else if(strncmp(buffer.c_str(), "usemtl",6) == 0)
			theObj->NumTexture++;

	}
	
	theObj->NormalArray =   new ObjNormal[theObj->NumNormal]; 
	theObj->TexCoordArray = new ObjTexCoord[theObj->NumTexCoord];
	theObj->TriangleArray = new ObjTriangle[theObj->NumTriangle];
	theObj->VertexArray =   new ObjVertex[theObj->NumVertex];
	theObj->TextureMark =   new unsigned int[theObj->NumTexture];
	theObj->TextureMarkArray =new GLuint *[theObj->NumTexture];

	input.close();
	input.clear();
	
	int nC, vC, tC, fC, mC, sC;
	nC = vC = tC = fC = mC = sC = 0;
	char szinfo[256];
	for (int t = 0; t < lines.size();++t)
	{
		buffer = lines[t];
		istringstream line(buffer);
		string temp;
		string	f1, f2, f3,f4;
			
		if(buffer.substr(0,2) == "vn")  {
			line>> temp>> f1>> f2>> f3;
			theObj->NormalArray[nC].X = atof(f1.c_str());
			theObj->NormalArray[nC].Y = atof(f2.c_str());
			theObj->NormalArray[nC].Z = atof(f3.c_str());
			++nC;
		}
		else if(buffer.substr(0,2) == "vt")  {
			line >> temp >> f1 >> f2;
			theObj->TexCoordArray[tC].U = atof(f1.c_str());
			theObj->TexCoordArray[tC].V = atof(f2.c_str());
			++tC;
		}				
		else if(buffer.substr(0,1) == "v")  {
			line>> temp>> f1>> f2>> f3;
			theObj->VertexArray[vC].X = atof(f1.c_str());
			theObj->VertexArray[vC].Y = atof(f2.c_str());
			theObj->VertexArray[vC].Z = atof(f3.c_str());
			++vC;
		}
		else if(buffer.substr(0,1) == "f")  {
			line>> temp>> f1>> f2>> f3>>f4;
				
			int sPos = 0;
			int ePos = sPos;
			string temp;
			FaceVertex fV;

		
			if(f1.find("/") != -1)
			{

				ParseVertex(f1.c_str(), fV);

				theObj->TriangleArray[fC].Vertex[0] = fV.pos;
				theObj->TriangleArray[fC].TexCoord[0] = fV.tex;
				theObj->TriangleArray[fC].Normal[0] = fV.norm;
				
			}
			else
			{
				theObj->TriangleArray[fC].Vertex[0] = atoi(f1.c_str()) - 1;
			}

			if (f2.find("/") != -1)
			{

				ParseVertex(f2.c_str(), fV);

				theObj->TriangleArray[fC].Vertex[1] = fV.pos;
				theObj->TriangleArray[fC].TexCoord[1] = fV.tex;
				theObj->TriangleArray[fC].Normal[1] = fV.norm;
				
			}
			else
			{
				theObj->TriangleArray[fC].Vertex[1] = atoi(f2.c_str()) - 1;
			}

			if (f3.find("/") != -1)
			{

				ParseVertex(f3.c_str(), fV);

				theObj->TriangleArray[fC].Vertex[2] = fV.pos;
				theObj->TriangleArray[fC].TexCoord[2] = fV.tex;
				theObj->TriangleArray[fC].Normal[2] = fV.norm;
				
			}
			else
			{
				theObj->TriangleArray[fC].Vertex[2] = atoi(f3.c_str()) - 1;
			}


			if (f4.find("/") != -1)
			{
				ParseVertex(f4.c_str(), fV);

				theObj->TriangleArray[fC].Vertex[3] = fV.pos;
				theObj->TriangleArray[fC].TexCoord[3] = fV.tex;
				theObj->TriangleArray[fC].Normal[3] = fV.norm;

				theObj->TriangleArray[fC].bFoure = true;
			
			}
			else
			{
				int kk = 0;
				int kk1 = 0;
				theObj->TriangleArray[fC].bFoure = false;
				/*
				theObj->TriangleArray[fC].Vertex[3] = theObj->TriangleArray[fC].Vertex[2];
					theObj->TriangleArray[fC].Normal[3] = theObj->TriangleArray[fC].Vertex[2];
					theObj->TriangleArray[fC].TexCoord[3] = theObj->TriangleArray[fC].Vertex[2];*/

				//theObj->TriangleArray[fC].Vertex[2] = atoi(f4.c_str()) - 1;
			}

			++fC;
		}
		
		else if(buffer.substr(0,6) == "usemtl")
		{
		/*	line>>temp>>f1;
			if(fC!=0)
			{
			   theObj->TextureMark[mC] = fC;
			   ++mC;
			}
			for(sC=0;sC<theObj->NumMtl;sC++)
			{
				
			}
			theObj->TextureMarkArray[mC]=&theObj->TextureArray[sC];*/
		}
		else if(buffer.substr(0,6) == "mtllib")
		{
			line>>temp>>mtlfilename;
			LoadMtl(mtlfilename);
		}
	}
	theObj->TextureMark[mC] = theObj->NumTriangle;
	
	
}

