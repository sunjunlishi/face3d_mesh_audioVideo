
// MFCDlgDlg.cpp : 实现文件
//



#include "MFCDlgDlg.h"
#include "afxdialogex.h"

#include "GLDevice.h"
#include <gl\GL.h>
#include <gl/GLU.h>
#include <vector>
#include <afxdlgs.h>





#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMFCDlgDlg::CMFCDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCDlgDlg::IDD, pParent)
{

	m_bFullScreen =false;
	m_timerID = -1;
	m_nowEntity = NULL;

	for (int k = 0; k < 13; ++k)
	{
		m_motionWeight[k] = 0;
	}
	m_TimeSpan = 25;
}

void CMFCDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCDlgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

// char szinfo[256];
//	sprintf(szinfo,"%.3f %.3f %.3f %.3f %.3f %.3f\n",GetEyeBlinkV(),GetRotateXFv(), GetRotateYFv(), GetRotateZFv(),
//		 GetEyeRx(),GetEyeRy());
// CMFCDlgDlg 对话框

#include <Common/gLobalinfo.h>
BOOL CMFCDlgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	string buffer, mtlfilename;

	ifstream input("ani.txt");
	if (input)
	{
		while (!input.eof()) {
			getline(input, buffer);	while (!input.eof())
			{
				getline(input, buffer);
				std::vector<string> strs = testSplit(buffer," ");
				s_ani item;
				if (strs.size() > 3)
				{
					item.eyeblink = atof(strs[0].c_str());
					item.rotatex = atof(strs[1].c_str());
					item.rotatey = atof(strs[2].c_str());
					item.rotatez = atof(strs[3].c_str());
					item.eyerx = atof(strs[4].c_str());
					item.eyery = atof(strs[5].c_str());
					m_listani.push_back(item);
				}
				
			}
		}
	}


	

	m_nowEntity = NULL;
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMFCDlgDlg::voiceStart(int longtime, std::vector<PhonemeData>  tstrs)
{
	mstimeos.m_timeWhole = longtime;
	mstimeos.DoPareStr(tstrs);
}

void CMFCDlgDlg::StartTimer()
{
	m_TimeSpan = 20;
	m_timerID = SetTimer(1, m_TimeSpan, NULL);
}

void CMFCDlgDlg::ShowToFront()
{
	SetParent(GetDesktopWindow());
	ModifyStyle(WS_CHILD,WS_POPUP|WS_VISIBLE,NULL);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);	

	
	SetWindowPos(&wndTopMost,0,0,cx,cy,SWP_SHOWWINDOW);


	//m_clockDlg.StartShow();
	//m_clockDlg.MoveWindow(cx-290,5,290,124);

	m_bFullScreen = true;
}



void CMFCDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		
	}
	else
	{
		CDialog::OnPaint();
	}
}




int CMFCDlgDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	 SetupGLDevice(m_hWnd);



	return 0;
}

void CMFCDlgDlg::Execute()
{
	if (m_nowEntity)
	{
		if (!m_nowEntity->Is2DRender())
		{
			glClearColor(0.5, 0.5, 0.5, 0.5);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清一下当前屏幕
		}

		
		m_nowEntity->Execute(0.3);
		m_nowEntity->Render();

		if (!m_nowEntity->Is2DRender())
		{
			FlushGLDevice();
		}

		
	}
	

  
}
void CMFCDlgDlg::Rerotate0()
{
	if (m_nowEntity) m_nowEntity->Rerotate0();
}

void CMFCDlgDlg::RotateX(bool bPosi)
{
	if (m_nowEntity) m_nowEntity->RotateX(bPosi);
}

void CMFCDlgDlg::RotateY(bool bPosi)
{
	if (m_nowEntity) m_nowEntity->RotateY(bPosi);
}

void CMFCDlgDlg::RotateZ(bool bPosi)
{
	if (m_nowEntity) m_nowEntity->RotateZ(bPosi);
}

void CMFCDlgDlg::RotateXFv(float fV)
{
	if (m_nowEntity) m_nowEntity->RotateXFv(fV);
}

void CMFCDlgDlg::RotateYFv(float fV)
{
	if (m_nowEntity) m_nowEntity->RotateYFv(fV);
}

void CMFCDlgDlg::RotateZFv(float fV)
{
	if (m_nowEntity) m_nowEntity->RotateZFv(fV);
}

void CMFCDlgDlg::SetTieTu(int itietu)
{
	if (m_nowEntity) m_nowEntity->m_tietu = itietu;
}
void CMFCDlgDlg::SetTushi(int tushi)
{
	if (m_nowEntity) m_nowEntity->m_toushi = tushi;
}
int *CMFCDlgDlg::GetCharcterWeight()
{

	if (m_nowEntity)
	{
		return m_nowEntity->m_characterWeight;
	}
}


void CMFCDlgDlg::SaveCharacter(int iSel)
{
	for (int k = 0; k < 20;++k)
	{
		char szinfo[256];
		sprintf(szinfo,"value%02d",k);
		char szvalue[256];
		sprintf(szvalue,"%d",m_nowEntity->m_characterWeight[k]);
		char key[128];
		sprintf(key,"character%d",iSel);
		WritePrivateProfileString(key, szinfo, szvalue, "./3dface.ini");
	}

}
void CMFCDlgDlg::ReadCharacter(int iSel)
{
	for (int k = 0; k < 20; ++k)
	{
		char key[128];
		sprintf(key, "character%d", iSel);

		char szinfo[256];
		sprintf(szinfo, "value%02d", k);
		int iValue = GetPrivateProfileInt(key, szinfo, 0, "./3dface.ini");
		m_nowEntity->m_characterWeight[k] = iValue;
	}
	

}
void CMFCDlgDlg::ChangeMotion(int* weight)
{
     //更改面部表情做，手动看实时效果
	memcpy(m_motionWeight, weight, 13 * 4);

}
void CMFCDlgDlg::SetCharcterWeight(int* weight, int starP)
{
	if (m_nowEntity)
	{
		memcpy(m_nowEntity->m_characterWeight + starP, weight, 13 * 4);
	}
}

void CMFCDlgDlg::ChangeIdentityIdexStart(int idexStart)
{
	if (m_nowEntity) m_nowEntity->m_characterStart = idexStart;
}
void CMFCDlgDlg::ChangeIdentityName(char *objname)
{
	
	if (m_nowEntity) m_nowEntity->ChangeIdentityName(objname);

}
void  CMFCDlgDlg::SetEeyRx(float fv)
{
	if (m_nowEntity) m_nowEntity->SetEyeRx(fv);
}
void  CMFCDlgDlg::SetEeyRy(float fv)
{
	if (m_nowEntity) m_nowEntity->SetEyeRy(fv);
}

void CMFCDlgDlg::SetEyeBlinkV(float fEyeBlink)
{
	if (m_nowEntity) m_nowEntity->SetEyeBlinkV(fEyeBlink);
}

void CMFCDlgDlg::ChangeYicEnity(YicEntity * entity, eos::core::Mesh& obj,cv::Mat mat)
{

	if(m_nowEntity != entity)
	{
		if(m_nowEntity != NULL) m_nowEntity->Destroy();
		m_nowEntity = entity;
	}

	m_nowEntity->Initialize(obj,mat);
	
}

void CMFCDlgDlg::SetDlgMain(CDlgFaceMain * pMain)
{
	m_dlgMain = pMain;
}


bool bInitGlOk = false;

void CMFCDlgDlg::InitWndDC()
{

	if (!bInitGlOk)
	{
		CRect rt;
		GetClientRect(rt);
		//从新更改视角大小
		int width = rt.Width();
		int height = rt.Height();

		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glShadeModel(GL_SMOOTH);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, .5f, 660.0f);
		glShadeModel(GL_SMOOTH);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();




		

		ShowWindow(SW_SHOW);

		bInitGlOk = true;
	}

}


void CMFCDlgDlg::ShowNowTestB(eos::core::Mesh& obj, cv::Mat mat)
{
	
	
	ChangeYicEnity(&m_face3D, obj, mat);



	
	
}


void CMFCDlgDlg::OnDestroy()
{  
	DestroyGLDevice();
	
	CDialog::OnDestroy();
	
	
}


void CMFCDlgDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	   // TODO: Add your message handler code here
	if (0 >= cx || 0 >= cy)
	{

		return;

	}

	 
}
int tellfiles(char * filename) {
	long size = 0;
	FILE *fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	printf("size: %ld\n", size);
	/* fseek(fp, 0, SEEK_SET); */
	fclose(fp);
	return size;
}
enum class BlendShape : int
{
	_neutral = 0,
	browDownLeft,
	browDownRight,
	browInnerUp,
	browOuterUpLeft,
	browOuterUpRight,
	cheekPuff,
	cheekSquintLeft,
	cheekSquintRight,
	eyeBlinkLeft,
	eyeBlinkRight,
	eyeLookDownLeft,
	eyeLookDownRight,
	eyeLookInLeft,
	eyeLookInRight,
	eyeLookOutLeft,
	eyeLookOutRight,
	eyeLookUpLeft,
	eyeLookUpRight,
	eyeSquintLeft,
	eyeSquintRight,
	eyeWideLeft,
	eyeWideRight,
	jawForward,
	jawLeft,
	jawOpen,
	jawRight,
	mouthClose,
	mouthDimpleLeft,
	mouthDimpleRight,
	mouthFrownLeft,
	mouthFrownRight,
	mouthFunnel,
	mouthLeft,
	mouthLowerDownLeft,
	mouthLowerDownRight,
	mouthPressLeft,
	mouthPressRight,
	mouthPucker,
	mouthRight,
	mouthRollLower,
	mouthRollUpper,
	mouthShrugLower,
	mouthShrugUpper,
	mouthSmileLeft,
	mouthSmileRight,
	mouthStretchLeft,
	mouthStretchRight,
	mouthUpperUpLeft,
	mouthUpperUpRight,
	noseSneerLeft,
	noseSneerRight,

	// 方便遍历
	COUNT
};

int lastfilesize = 0;
int iCountRender = 0;
clock_t t1 =0;

FILE * fptxt = 0;
int iTagF = 0;
float mLastOpen = 0;
float mLastMouthPuckerValue = 0;
float mLastMouthFunnel = 0;

float mLastMouthstretch = 0;
float mLastMouthstretchR = 0;

float mLastMouthRollUp = 0;

float mLastToplipPressValue = 0;
float mLastToplipPressValueR = 0;

float mLastSmileValue = 0;
float mLastSmileRValue = 0;

float mLastMouthUpUpper = 0;
float mLastMouthUpUpperR = 0;



float mLastMouthBottomLower = 0;
float mLastMouthBottomLowerR = 0;
float mLastMouthLeft = 0;
float mLastMouthRight = 0;

float mLastEyeBlinkL = 0;
float mLastEyeBlinkR = 0;

float mLastBrownInnerUp = 0;
float mLastBrownOuterUpL = 0;
float mLastBrownOuterUpR = 0;

float mLastBrownDownL = 0;
float mLastBrownDownR = 0;


void CMFCDlgDlg::SetBlends(std::vector<float>  blends)
{
	mBlends = blends;
}
void CMFCDlgDlg::OnTimer(UINT_PTR nIDEvent)
{
	
	if (nIDEvent == 2)
	{
		m_timerID = SetTimer(1, 16, NULL);
		KillTimer(2);
	}
	else
	{
		SetStart(true);
		
		s_time_o* titemVoice = mstimeos.GetVoice();
		if (titemVoice  != NULL)
		{
			titemVoice->addSan();
			   float fa = 1.1;


			  
				float nowOpen = titemVoice->GetOpenValue() * fa;
				
			float nowMouthPuckerValue = titemVoice->GetMouthPuckerValue() * fa;
			float nowMouthstretch = titemVoice->GetMouthStretch() * fa;
			float nowToplipPressValue = titemVoice->GetToplipPressValue() * fa;
			float nowSmileValue = titemVoice->GetMouthSmile() * fa + m_motionWeight[1] / 15.0;
			float nowMouthUpUpper = titemVoice->GetTopLipUpper() * fa;
			float nowMouthBottomLower = titemVoice->GeBottomLipLower() * fa;

			if (nowOpen < 0) nowOpen = 0;
			if (nowMouthPuckerValue < 0) nowMouthPuckerValue = 0;
			if (nowMouthstretch < 0) nowMouthstretch = 0;
			if (nowToplipPressValue < 0) nowToplipPressValue = 0;
			if (nowSmileValue < 0) nowSmileValue = 0;
			if (nowMouthUpUpper < 0) nowMouthUpUpper = 0;
			if (nowMouthBottomLower < 0) nowMouthBottomLower = 0;
		


				m_nowEntity->mJawOpen = (nowOpen + mLastOpen) / 2;
		
				m_nowEntity->mMouthPucker = (mLastMouthPuckerValue + nowMouthPuckerValue)/2;
				m_nowEntity->mMouthStretch_L = (mLastMouthstretch + nowMouthstretch) / 2;

                m_nowEntity->mMouthStretch_R = m_nowEntity->mMouthStretch_L;

				m_nowEntity->mMouthPress_L = (mLastToplipPressValue + nowToplipPressValue)/2;
                m_nowEntity->mMouthPress_R = m_nowEntity->mMouthPress_L;


				m_nowEntity->mMouthSmile_L = (mLastSmileValue + nowSmileValue) / 2;
                m_nowEntity->mMouthSmile_R = m_nowEntity->mMouthSmile_L;

				m_nowEntity->mMouthUpperUp_L = (mLastMouthUpUpper+ nowMouthUpUpper) / 2;
                m_nowEntity->mMouthUpperUp_R = m_nowEntity->mMouthUpperUp_L;


				m_nowEntity->mMouthLowerDown_L =(mLastMouthBottomLower + nowMouthBottomLower)/2;
                m_nowEntity->mMouthLowerDown_R = m_nowEntity->mMouthLowerDown_L;

				 //mLastOpen =nowOpen;
				 //mLastMouthPuckerValue = nowMouthPuckerValue;
				 //mLastMouthstretch = nowMouthstretch;
				 //mLastToplipPressValue = nowToplipPressValue;
				 //mLastSmileValue = nowSmileValue;
				 //mLastMouthUpUpper = nowMouthUpUpper;
				 //mLastMouthBottomLower = nowMouthBottomLower;

				 mLastOpen = m_nowEntity->mJawOpen;
				 mLastMouthPuckerValue = m_nowEntity->mMouthPucker;
				 mLastMouthstretch = m_nowEntity->mMouthStretch_L;
				 mLastToplipPressValue = m_nowEntity->mMouthPress_L;
				 mLastSmileValue = m_nowEntity->mMouthSmile_L;
				 mLastMouthUpUpper = m_nowEntity->mMouthUpperUp_L;
				 mLastMouthBottomLower = m_nowEntity->mMouthLowerDown_L;

				
				//OutputDebugString(szinfo);

				if (m_nowEntity->mJawOpen < 0) m_nowEntity->mJawOpen = 0;
				if (m_nowEntity->mMouthPucker < 0) m_nowEntity->mMouthPucker = 0;
				if (m_nowEntity->mMouthStretch_L < 0) m_nowEntity->mMouthStretch_L = 0;
				if (m_nowEntity->mMouthPress_L < 0) m_nowEntity->mMouthPress_L = 0;
				if (m_nowEntity->mMouthSmile_L < 0) m_nowEntity->mMouthSmile_L = 0;
				if (m_nowEntity->mMouthUpperUp_L < 0) m_nowEntity->mMouthUpperUp_L = 0;
				if (m_nowEntity->mMouthLowerDown_L < 0) m_nowEntity->mMouthLowerDown_L = 0;
			
			
				s_ani item = m_listani[iTagF % m_listani.size()];
				SetEeyRx(item.eyerx);
				SetEeyRy(item.eyery);

				RotateXFv(item.rotatex);
				RotateYFv(item.rotatey);
				RotateZFv(item.rotatez);
			
			//sprintf(szinfo,"%d ...!!!!! cost\n",t1);
			//(szinfo);
		}
		else
		{
			t1 = clock();
			char szinfo[256];
			//sprintf(szinfo, "%d ... cost ********\n", t1);
			//OutputDebugString(szinfo);
			//0 张嘴；1 微笑；2 嘴角拉伸 ；3 上嘴唇内卷 ；4 上嘴唇压
			// 5 上嘴唇抬起；6 闭眼；7 嘴巴张嘴撅嘴 ；8 嘴巴闭撅嘴
			//
			//
			//
			if (mBlends.size() > 0)
			{
				float fa = 0.8;
				float fb = 1 - fa;
				m_nowEntity->mJawOpen = (mBlends[(int)BlendShape::jawOpen]* fa+ mLastOpen*fb);//打开嘴巴jawOpen
				m_nowEntity->mMouthSmile_L = (mBlends[(int)BlendShape::mouthSmileLeft] * fa + mLastSmileValue * fb);//L微笑
				m_nowEntity->mMouthSmile_R = (mBlends[(int)BlendShape::mouthSmileRight] * fa + mLastSmileRValue * fb) ;//R微笑

				m_nowEntity->mMouthStretch_L =( mBlends[(int)BlendShape::mouthStretchLeft] * fa + mLastMouthstretch * fb);//[47]嘴角拉伸
				m_nowEntity->mMouthStretch_R = (mBlends[(int)BlendShape::mouthStretchRight] * fa + mLastMouthstretchR * fb) ;//[47]嘴角拉伸

				m_nowEntity->mMouthRollUpper = (mBlends[(int)BlendShape::mouthRollUpper] * fa +mLastMouthRollUp * fb);//上嘴唇卷

				m_nowEntity->mMouthPress_L = (mBlends[(int)BlendShape::mouthPressLeft] * fa + mLastToplipPressValue * fb);//上嘴唇压
				m_nowEntity->mMouthPress_R =( mBlends[(int)BlendShape::mouthPressRight] * fa + mLastToplipPressValueR * fb);//上嘴唇压

				m_nowEntity->mMouthUpperUp_L =( mBlends[(int)BlendShape::mouthUpperUpLeft] * fa + mLastMouthUpUpper * fb);//上嘴唇抬
				m_nowEntity->mMouthUpperUp_R = (mBlends[(int)BlendShape::mouthUpperUpRight] * fa + mLastMouthUpUpperR * fb);//上嘴唇抬

				m_nowEntity->mEyeBlink_L = (mBlends[(int)BlendShape::eyeBlinkLeft] * fa +mLastEyeBlinkL * fb);//闭眼
				m_nowEntity->mEyeBlink_R = (mBlends[(int)BlendShape::eyeBlinkRight] * fa + mLastEyeBlinkR * fb);//闭眼

				m_nowEntity->mMouthFunnel = (mBlends[(int)BlendShape::mouthFunnel] * fa + mLastMouthFunnel * fb);//嘴巴张嘴撅嘴

				m_nowEntity->mMouthPucker =( mBlends[(int)BlendShape::mouthPucker] * fa+ mLastMouthPuckerValue*fb);//嘴巴合撅嘴

				m_nowEntity->mMouthLowerDown_L = (mLastMouthBottomLower * fb + mBlends[(int)BlendShape::mouthLowerDownLeft] * fa) ;//嘴巴下拉
				m_nowEntity->mMouthLowerDown_R = (mLastMouthBottomLowerR * fb + mBlends[(int)BlendShape::mouthLowerDownRight] * fa);//嘴巴下拉

				m_nowEntity->mMouthLeft = (mLastMouthLeft * fb + mBlends[(int)BlendShape::mouthLeft] * fa);//嘴巴合撅嘴
				m_nowEntity->mMouthRight = (mLastMouthRight * fb + mBlends[(int)BlendShape::mouthRight] * fa);//嘴巴合撅嘴
				


				//m_nowEntity->mBrowInnerUp_L =(mBlends[(int)BlendShape::browInnerUp] * fa + mLastBrownInnerUp * fb);;//眉毛内升
				//m_nowEntity->mBrowInnerUp_R = (mBlends[(int)BlendShape::browInnerUp] * fa + mLastBrownInnerUp * fb);;//眉毛内升

				//m_nowEntity->mBrowOuterUp_L =  (mBlends[(int)BlendShape::browOuterUpLeft] * fa + mLastBrownOuterUpL * fb);;//眉毛外升
				//m_nowEntity->mBrowOuterUp_R = (mBlends[(int)BlendShape::browOuterUpRight] * fa + mLastBrownOuterUpR * fb);;//眉毛外升

				//m_nowEntity->mBrowDown_L = (mBlends[(int)BlendShape::browDownLeft] * fa + mLastBrownDownL * fb);;//眉毛下拉
				//m_nowEntity->mBrowDown_R = (mBlends[(int)BlendShape::browDownRight] * fa + mLastBrownDownR * fb);;//眉毛下拉

				mLastOpen = m_nowEntity->mJawOpen;

				mLastMouthPuckerValue = m_nowEntity->mMouthPucker;

				mLastMouthRollUp = m_nowEntity->mMouthRollUpper;
				mLastMouthFunnel = m_nowEntity->mMouthFunnel;

				mLastMouthstretch = m_nowEntity->mMouthStretch_L;
				mLastMouthstretchR = m_nowEntity->mMouthStretch_R;

				mLastToplipPressValue = m_nowEntity->mMouthPress_L;
				mLastToplipPressValueR = m_nowEntity->mMouthPress_R;

				mLastSmileValue = m_nowEntity->mMouthSmile_L;
				mLastSmileRValue = m_nowEntity->mMouthSmile_R;

				mLastMouthUpUpper = m_nowEntity->mMouthUpperUp_L;
				mLastMouthUpUpperR = m_nowEntity->mMouthUpperUp_R;


				mLastMouthBottomLower = m_nowEntity->mMouthLowerDown_L;
                mLastMouthBottomLowerR = m_nowEntity->mMouthLowerDown_R;

                mLastMouthLeft = m_nowEntity->mMouthLeft;
                mLastMouthRight = m_nowEntity->mMouthRight;

				mLastEyeBlinkL = m_nowEntity->mEyeBlink_L;
                mLastEyeBlinkR = m_nowEntity->mEyeBlink_R;

				mLastBrownInnerUp = m_nowEntity->mBrowInnerUp_L;

				mLastBrownOuterUpL = m_nowEntity->mBrowOuterUp_L;
                mLastBrownOuterUpR = m_nowEntity->mBrowOuterUp_R;

				mLastBrownDownL = m_nowEntity->mBrowDown_L;
                mLastBrownDownR = m_nowEntity->mBrowDown_R;


				for (int t = 0; t < 2; ++t)
				{
					if (fabs(mBlends[52+t]) > 12)
					{
						if (mBlends[52+t] > 12) mBlends[52+t] = 12;
						if (mBlends[52+t] < -12) mBlends[52+t] = -12;

					}
				}

				

				

				RotateXFv(mBlends[52]);
				RotateYFv(mBlends[53]);
				RotateZFv(mBlends[54]);

			}
			else
			{
				m_nowEntity->mJawOpen = m_motionWeight[0] / 15.0;//打开嘴巴
				m_nowEntity->mMouthSmile_L = m_motionWeight[1] / 15.0;//微笑
				m_nowEntity->mMouthStretch_L = m_motionWeight[2] / 15.0;//嘴角拉伸
				m_nowEntity->mMouthRollUpper = m_motionWeight[3] / 15.0;//上嘴唇卷
				m_nowEntity->mMouthPress_L = m_motionWeight[4];//上嘴唇压
				m_nowEntity->mMouthUpperUp_L = m_motionWeight[5] / 15.0;//上嘴唇抬
				m_nowEntity->mEyeBlink_L = m_motionWeight[6] / 15.0;//闭眼
				m_nowEntity->mMouthFunnel = m_motionWeight[7] / 15.0;//嘴巴张嘴撅嘴
				m_nowEntity->mMouthPucker = m_motionWeight[8] / 15.0;//嘴巴合撅嘴

				m_nowEntity->mMouthLowerDown_L = m_motionWeight[9] / 15.0;//嘴巴下拉
				m_nowEntity->mBrowInnerUp_L = m_motionWeight[10] / 15.0;//眉毛内升
				m_nowEntity->mBrowOuterUp_L = m_motionWeight[11] / 15.0;//眉毛外升
				m_nowEntity->mBrowDown_L = m_motionWeight[12] / 15.0;//眉毛下拉
			}
		
		
		}


		//if (fptxt == 0)
		//{
		//	fptxt = fopen("D:/ani.txt","wb");
		//}
		//if (fptxt)
		//{
		//	char szinfo[256];
		//	sprintf(szinfo,"%.3f %.3f %.3f %.3f %.3f %.3f\n",GetEyeBlinkV(),GetRotateXFv(), GetRotateYFv(), GetRotateZFv(),
		//		 GetEyeRx(),GetEyeRy());
		//	fwrite(szinfo,1,strlen(szinfo), fptxt);
		//}
	
		//
		//if (iTagF >= 2200)
		//{
		//	fclose(fptxt);
		//	
		//}
		//if (iTagF %5 == 0)
		{
		
			// feye = item.eyeblink * 1.2 + m_nowEntity->mEyeBlink_L;
			//if (feye > 1) feye = 1;
			//SetEyeBlinkV(feye);
		}
		

		if (iTagF % 70 == 0 && rand() % 2 == 0)
		{
			//m_nowEntity->mEyeClose = rand()%10*0.1+ m_motionWeight[6] / 15.0;
		}
	

		t1 = clock();
		iTagF++;
		m_nowEntity->SetWeightChanged();
		Execute();
		clock_t t2= clock();
		
		char szinfo[256];
		sprintf(szinfo, "%d .. cost *\n", t2-t1);
		OutputDebugString(szinfo);
		// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	}
	iCountRender++;
	


	//不断的读取文件看文件大小是否变化


	//int tempfilesize  = tellfiles("./detection/uv_face.png");
	//if (lastfilesize != tempfilesize)
	{
		//重新加载
	  // m_nowEntity->DoReloadMat();

	}

	//lastfilesize = tempfilesize;

	CDialog::OnTimer(nIDEvent);
}
void CMFCDlgDlg::DoReloadMat()
{
	m_nowEntity->DoReloadMat();
}
void CMFCDlgDlg::StopTimerMy()
{
		if(m_timerID != -1)
		{
			KillTimer(m_timerID);
			m_timerID = -1;
		}
}

void CMFCDlgDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

void CMFCDlgDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDblClk(nFlags, point);
}


LRESULT CMFCDlgDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_nowEntity)
	{
		switch (message)
		{
		case WM_MOUSEMOVE:
			if (m_nowEntity)
			{
				int xPos = LOWORD(lParam);
				int yPos = HIWORD(lParam);

				m_nowEntity->OnMouseMove(xPos, yPos);

			}
			break;
		case WM_LBUTTONDOWN:
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);

			m_nowEntity->OnLbtnDwon(xPos, yPos);

		}
		break;

		case WM_LBUTTONUP:
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			m_nowEntity->OnLbtnUp(xPos, yPos);

		}
		break;

		default:
			break;
		}
	}
	

	return CDialog::WindowProc(message, wParam, lParam);
}





void CMFCDlgDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
