
// MFCDlgDlg.h : 头文件
//

#pragma once


#include ".\Nehe\3DFace.h"
#include <Common/gLobalInfo.h>
class CDlgFaceMain;

class s_time_o
{
public:
	float m_listValue = 0.3;
	int keepTime;
	float diffvalue;
	float mOpennowvalue=0;//张嘴
	float mMouthSmile = 0;//微笑
	float mToplipPressValue = 0;//上嘴唇压
	float mMouthPuckerValue = 0;//合拢撅嘴
	float mMouthFunnel = 0;//张嘴撅嘴
	float mMouthStrechValue = 0;//嘴巴左右拉伸
	float mTopLipScrrol = 0;//上嘴唇卷起
	float mTopLipUpper = 0;//上嘴唇抬起
	float mBottomLipLower = 0;//上嘴唇抬起
	float mEyeClose = 0;//闭眼
	int mStartTL = 0;//队列中的起始时间
	int mEndTL = 0;//队列中的终止时间



	float mToplipPressValue_Start = 0;
	float mMouthPuckerValue_Start = 0;
	float mMouthStrechValue_Start = 0;
	float mOpennowvalue_Start = 0;

	float mfRatio;
	int sumtime;
	
	int mSpan;
	bool mBlastObj;
	bool mbJian;
	float mPreYu;
	float mPrequan;
	int mStartT = 0;
	
	s_time_o(int inTime)
	{
		mPreYu = 0.12;
		mPrequan = 1.0;
		mbJian = false;
		mfRatio = 0.40;
		mPre = NULL;
		
		keepTime = inTime;
		mSpan = 20;
		diffvalue = 0.02;// (mSpan * 2)*1.0 / keepTime*0.29;
		
		sumtime = 0;

		mToplipPressValue = 0;
		mMouthPuckerValue = 0;
		mMouthStrechValue = 0;
		mOpennowvalue = 0;

		mBlastObj = false;
		
	}
	std::string GetYinJie() { return mYin; }
	virtual void SetYinjie(std::string yin)
	{
		mYin = yin;
	}

	void SetPreItem(s_time_o* item)
	{
		mPre = item;
		ValueStart(item);
	}
	void SetValuesStart(float tTopLipPress, float mouthPuck, float mouthStrech, float openMouth)
	{
		 mToplipPressValue_Start = tTopLipPress;
		 mMouthPuckerValue_Start = mouthPuck;
		 mMouthStrechValue_Start = mouthStrech;
		 mOpennowvalue_Start = openMouth;
	}

	void ValueStart(s_time_o * item)
	{
		if (item != NULL)
		{
			mToplipPressValue_Start = item->mToplipPressValue;
			mMouthPuckerValue_Start = item->mMouthPuckerValue;
			mMouthStrechValue_Start = item->mMouthStrechValue;
			mOpennowvalue_Start = item->mOpennowvalue;
		}

	}
	
	void addSan()
	{
		if (mStartT == 0)
		{
			mStartT = clock();
		}
		clock_t tNow = clock();

		sumtime = tNow - mStartT;
	
	}


	virtual float GetOpenValue()
	{
		float fa = sumtime * 1.0 / keepTime;

		char szinfo[256];
		sprintf(szinfo, "sum keep %d %d\n", sumtime, keepTime);
		//OutputDebugStringA(szinfo);
		if (fa < 0.50)
		{
			return mOpennowvalue * fa / 0.35;
		}
		else
		{
			return mOpennowvalue * ((1 - fa) / 0.35);
		}
	}



	virtual float GetMouthStretch()
	{
		float fa = sumtime*1.0 / keepTime;
		if (fa < 0.45)
		{
			return mMouthStrechValue*fa / 0.35;
		}
		else
		{
			return mMouthStrechValue*(1 - fa) / 0.35;
		}
	}

	virtual float GetMouthSmile()
	{
		float fa = sumtime*1.0 / keepTime;
		if (fa < 0.35)
		{
			return mMouthSmile*fa / 0.35;
		}
		else
		{
			return mMouthSmile*(1 - (fa - 0.35) / 0.65);
		}
	}
	virtual float GetTopLipUpper()
	{
		float fa = sumtime*1.0 / keepTime;
		if (fa < 0.35)
		{
			return mTopLipUpper*fa / 0.35;
		}
		else
		{
			return mTopLipUpper*(1 - (fa - 0.35) / 0.65);
		}
	}
	virtual float GeBottomLipLower()
	{
		float fa = sumtime * 1.0 / keepTime;
		if (fa < 0.35)
		{
			return mBottomLipLower * fa / 0.35;
		}
		else
		{
			return mBottomLipLower * (1 - (fa - 0.35) / 0.65);
		}
	}

	virtual float GetToplipPressValue()
	{
		float fa = sumtime*1.0 / keepTime;
		float limit = 0.25;
		if (fa < limit)
		{
			return mToplipPressValue*fa / limit;
		}
		else
		{
			return mToplipPressValue*(1 - (fa - limit) /(1- limit));
		}
		return 0;
	}
	virtual float GetMouthPuckerValue()
	{
		float fa = sumtime*1.0 / keepTime;
		if (fa < 0.35)
		{
			return mMouthPuckerValue*fa / 0.35;
		}
		else
		{
			return mMouthPuckerValue*(1 - (fa - 0.35) / 0.65);
		}
	}

	private: 
		s_time_o * mPre;
	protected:
		std::string mYin;
};


//单韵母6个：a、o、e、i、u、ü

//复韵母9个：ai、ei、ui、ao、ou、iu、ie、ue、er

//前鼻韵母5个：an、en、in、un、ün

//后鼻韵母4个：ang、eng ing ong


//每个辅音对应几种原因

//口型分为4种 姑且4种，也可以再提升
//压上嘴唇，抬上嘴巴，噘嘴，咧嘴； 上下颚 张开
class voice_bpmf:public  s_time_o
{
public:
	voice_bpmf(int keepTime):s_time_o(keepTime)
	{

	}

	virtual void SetYinjie(std::string yin)
	{
		mYin = yin;
		mOpennowvalue = 0.06;
		if (yin.find("in") != -1)
		{
			mOpennowvalue = 0.15;
			mToplipPressValue = 0.10;
			mMouthStrechValue = 0.15;
		}

		if (yin.find("an") != -1)
		{
			mOpennowvalue = 0.2;
			mMouthFunnel = 0.11;
		}
		else if (yin.find("ao") != -1)
		{
			mOpennowvalue = 0.2;
			mMouthPuckerValue = 0.25;
		}
		else if (yin.find("ai") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthSmile = 0.1;
			mMouthStrechValue = 0.11;
		}
		
		else if (yin.find("ui") != -1 || yin.find("ei") != -1)
		{
			mMouthPuckerValue = 0.20;
			mMouthSmile = 0.15;//有咧嘴的动作
		}
		else if (yin.find("u") != -1 || yin.find("o") != -1)
		{
			mMouthPuckerValue = 0.3;
			
		}
	}
	//张嘴延后，让阻塞时间长点
	virtual float GetOpenValue()
	{
		float fa = sumtime*1.0 / keepTime;
		float limit = 0.50;
		if (fa < limit)
		{
			//return mOpennowvalue*fa / limit;
			return 0;
		}
		else
		{
			return mOpennowvalue*(1 - (fa - limit) /(1- limit));
		}
	}
	virtual float GetToplipPressValue()
	{
		float fa = sumtime*1.0 / keepTime;
		float limit = 0.45;
		if (fa < limit)
		{
			return mToplipPressValue;
		}
		else
		{
			return mToplipPressValue*(1 - (fa - limit) / (1 - limit));
		}
		
	}
};

class voice_dtnl :public s_time_o
{
public:
	voice_dtnl(int keepTime) :
		s_time_o(keepTime)
	{

	}
	virtual void SetYinjie(std::string yin)
	{
		mYin = yin;

		 mTopLipUpper = 0.15;//上嘴唇抬起
		 mBottomLipLower = 0.15;//上嘴唇抬起
		if (yin.find("ai") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthSmile = 0.25;
			mMouthStrechValue = 0.15;
			mTopLipScrrol = 0.15;
		}
		if (yin.find("ei") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthStrechValue = 0.15;
		}
		if (yin.find("e") != -1)
		{
			mOpennowvalue = 0.15;
			mTopLipUpper = 0.25;
		}
		if (yin.find("ao") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthFunnel = 0.15;
			mTopLipUpper = 0.15;
		}
		if (yin.find("ang") != -1)
		{
			mOpennowvalue = 0.20;
			
		}
		if (yin.find("ong") != -1)
		{
			 mMouthPuckerValue = 0.15;//合拢撅嘴
		}
		else if (yin.find("an") != -1)
		{
			mOpennowvalue = 0.20;
			mMouthStrechValue = 0.10;
		}

		if (yin.find("ou") != -1)
		{
			mOpennowvalue = 0.1;
			mMouthPuckerValue = 0.25;

		}
		if (yin.find("in") != -1)
		{
			mOpennowvalue = 0.10;
			mMouthStrechValue = 0.17;
			mTopLipUpper = 0.15;
		}
		else  if (yin.find("i") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthStrechValue = 0.20;
			
		}
		else  if (yin.find("u") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthPuckerValue = 0.25;

		}
	}
};

class voice_gkh :public s_time_o
{
public:
	voice_gkh(int keepTime):
		s_time_o(keepTime)
	{
	}

	/*if (mYin.find("uo") != -1
		|| mYin.find("ou") != -1
		|| mYin.find("ue") != -1
		|| mYin.find("uang") != -1)*/

	virtual void SetYinjie(std::string yin)
	{
		mOpennowvalue = 0.10;
		mYin = yin;
		if (yin[yin.length() - 1] == 'e' && yin.length() == 2)
		{
			
			mTopLipUpper = 0.1;
		}
		if (yin.find("ao") != -1)
		{
			mMouthFunnel = 0.2;
		}
		if (yin.find("u") != -1)
		{
			mMouthPuckerValue = 0.3;
		}
		if (yin.find("o") != -1)
		{
			mMouthPuckerValue = 0.3;
		}
	}

};

class voice_jqx : public s_time_o
{
public:
	voice_jqx(int keepTime):
		s_time_o(keepTime)
	{
	}
	
	virtual void SetYinjie(std::string yin)
	{
		mYin = yin;
		
		if (yin.find("ing") != -1)
		{
			mTopLipUpper = 0.15;
			mBottomLipLower = 0.15;
			mMouthStrechValue = 0.15;
		}
		if (yin.find("ian") != -1 || yin.find("iang") != -1)
		{
			mTopLipUpper = 0.15;
			mBottomLipLower = 0.15;
			mMouthSmile = 0.15;
		}
		else if (yin.find("in") != -1)
		{
			mMouthStrechValue = 0.25;
			mMouthSmile = 0.15;
			mTopLipUpper = 0.1;
			mBottomLipLower = 0.1;
		}
		else if (yin.find("ie") != -1)
		{
			mTopLipUpper = 0.15;//上嘴唇抬起
			mMouthStrechValue = 0.20;

		}
		else if (yin.find("iao") != -1)
		{
			mMouthPuckerValue = 0.25;
			mOpennowvalue = 0.15;//打开嘴巴
		
		}
		else if (yin.find("ia") != -1)
		{
			mOpennowvalue = 0.2;//打开嘴巴
			mMouthStrechValue = 0.25;//咧嘴
		}
		else if (yin.find("a") != -1)
		{
			 mTopLipUpper =0.2;//上嘴唇抬起
			 mBottomLipLower = 0.2;//上嘴唇抬起
		}
		else if (yin.find("i") != -1)
		{
			mTopLipUpper = 0.1;//上嘴唇抬起
			mBottomLipLower = 0.1;//上嘴唇抬起
			mMouthStrechValue = 0.25;
		}
		else if (yin.find("uan") != -1)
		{
			mMouthPuckerValue = 0.3;
			mOpennowvalue = 0.1;//张开嘴巴，微微
		}
		else if (yin.find("u") != -1)
		{
			mMouthPuckerValue = 0.3;
		}
	}
	

};

class voice_zcs : public s_time_o
{
public:

	voice_zcs(int keepTime):
		s_time_o(keepTime)
	{

	}

	virtual void SetYinjie(std::string yin)
	{
		mOpennowvalue = 0.08;
		mMouthStrechValue = 0.15;//这几个声母有咧嘴的趋势

		mYin = yin;
		if (yin.find("i") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthStrechValue = 0.25;
			mMouthSmile = 0.1;
		}
		else if (yin.find("uo") != -1)
		{
			mMouthPuckerValue = 0.35;
		}
		else if (yin.find("an") != -1)
		{
			mMouthStrechValue = 0.2;
			mTopLipUpper = 0.1;
			mMouthSmile = 0.1;
		}
		else if (yin.find("ao") != -1)
		{
			mOpennowvalue = 0.22;
			mMouthPuckerValue = 0.1;
		}
		else if (yin.find("a") != -1)
		{
			mOpennowvalue = 0.25;

		}
		else if (yin[yin.length() - 1] == 'e')
		{
			mOpennowvalue = 0.15;
			mTopLipUpper = 0.15;
		}
		else if (yin.find("en") != -1)
		{
			mOpennowvalue = 0.12;
			mMouthStrechValue = 0.2;
			mTopLipUpper = 0.1;

		}
		else if (yin.find("ang") != -1)
		{
			mMouthStrechValue = 0.2;
			mTopLipUpper = 0.15;

		}
		else if (yin.find("ong") != -1)
		{
			mMouthPuckerValue = 0.3;
			mMouthStrechValue = 0.1;
		}
		else if (yin.find("ou") != -1)
		{
			mMouthPuckerValue = 0.3;
		}
		else if (yin.find("un") != -1)
		{
			mMouthPuckerValue = 0.15;
		}
		
	}
	


};

//float mOpennowvalue;//张嘴
//float mMouthSmile;//微笑
//float mToplipPressValue;//上嘴唇压
//float mMouthPuckerValue;//合拢撅嘴
//float mMouthFunnel;//嘴巴张撅嘴
//float mMouthStrechValue;//嘴巴左右拉伸
//float mTopLipScrrol;//上嘴唇卷起
//float mTopLipUpper;//上嘴唇抬起
//float mEyeClose;//闭眼

class voice_w :public s_time_o
{
public:
	voice_w(int keepTime) :
		s_time_o(keepTime)
	{

	}
	virtual void SetYinjie(std::string yin)
	{
		mYin = yin;
		mMouthPuckerValue = 0.2;
		if (yin[1] == 'o' || yin[1] == 'u')
		{
			mOpennowvalue = 0.10;
			mMouthPuckerValue = 0.50;
		}
		if (yin.find("ei") != -1)
		{
			mOpennowvalue = 0.15;
			mMouthSmile = 0.2;
		}
		if (yin.find("en") != -1)
		{
			mOpennowvalue = 0.1;
			mMouthPuckerValue = 0.25;
		}
		if (yin.find("an") != -1)//an 比 en 的口型 大点
		{
			mOpennowvalue = 0.15;
			mMouthPuckerValue = 0.25;
		}
	}
};

class voice_other :public s_time_o
{
public:
	voice_other(int keepTime):
		s_time_o(keepTime)
	{

	}
};


class voice_sil :public s_time_o
{
public:
	voice_sil(int keepTime) :
		s_time_o(keepTime)
	{
		 mOpennowvalue = 0;//张嘴
		 mMouthSmile = 0;//微笑
		 mToplipPressValue = 0;//上嘴唇压
		 mMouthPuckerValue = 0;//合拢撅嘴
		 mMouthFunnel = 0;//张嘴撅嘴
		 mMouthStrechValue = 0;//嘴巴左右拉伸
		 mTopLipScrrol = 0;//上嘴唇卷起
		 mTopLipUpper = 0;//上嘴唇抬起
		 mEyeClose = 0;//闭眼
	}
};

class timeVoiceMgr
{
public:
	clock_t mStartT;
	int m_timeWhole;
	timeVoiceMgr()
	{
		mStartT = 0;
		m_timeWhole = 3200;
	}
	std::vector<s_time_o*> m_voiceMgr;
	void DoPareStr(std::vector<PhonemeData>  strs)
	{
		DeleteAll();
		
		s_time_o * pitem;
		for (int k = 0; k < strs.size(); ++k)
		{
			int inTime = strs[k].end- strs[k].start;
			if (strs[k].newstr.find("sil") != -1)
			{
				pitem = new voice_sil(inTime);
			}
			else
			{
				switch (strs[k].newstr[0])
				{
				case 'b':
				case 'p':
				case 'm':
				case 'f':
				{
					pitem = new voice_bpmf(inTime);

				}

				break;
				case 'd':
				case 't':
				case 'n':
				case 'l':
				case 'e':
				case 'ei':
				case 'ai':
				case 'an':
				{
					pitem = new voice_dtnl(inTime);

				}

				break;

				case 'g':
				case 'k':
				case 'h':

				{
					pitem = new voice_gkh(inTime);

				}

				break;
				case 'j':
				case 'q':
				case 'x':
				{
					pitem = new voice_jqx(inTime);

				}

				break;
				case 'z':
				case 'c':
				case 's':
				case 'y':
				case 'r':
				{
					pitem = new voice_zcs(inTime);

				}

				break;
				case 'w':
				{
					pitem = new voice_w(inTime);

				}

				break;

				default:
					pitem = new voice_other(inTime);


					break;
				}
			}
			
			pitem->SetYinjie(strs[k].newstr);
			pitem->mStartTL = strs[k].start;
			pitem->mEndTL = strs[k].end;
			m_voiceMgr.push_back(pitem);
		}
		
	}
	
	void DeleteFirst()
	{
		if (m_voiceMgr.size() > 0)
		{
			delete m_voiceMgr[0];
			m_voiceMgr.erase(m_voiceMgr.begin());
		}
	}
	s_time_o * GetVoice()
	{
		if (m_voiceMgr.size() > 0)
		{
			if (mStartT == 0)
			{
				mStartT = clock();
			}
			clock_t tNow = clock();


			int span = tNow - mStartT;
			//整体时间表
			if (span >= m_voiceMgr[0]->mStartTL && span <= m_voiceMgr[0]->mEndTL)
			{
				return m_voiceMgr[0];
			}
			else
			{
				delete m_voiceMgr[0];
				m_voiceMgr.erase(m_voiceMgr.begin());
				if (m_voiceMgr.size() > 0)
				{
					return m_voiceMgr[0];
				}
				else
				{
					return NULL;
				}
			}

			
		}
		else
		{
			mStartT = 0;
			DeleteAll();
		}
		
		
		return NULL;
	}
	
	void DeleteAll()
	{
		for (int k = m_voiceMgr.size() - 1; k >= 0; --k)
		{
			delete m_voiceMgr[k];
		}
		m_voiceMgr.clear();
	}
};

class randAnit
{
public:
	int m_timeAdd;
	int m_timeNorm;
	int m_timeDel;

	float m_spanx;
	float m_spany;
	float m_spanz;
	randAnit()
	{
		m_roatex = 0.0;
		m_roatey = 0;
		m_roatez = 0;
		mBStart = false;
		m_spanx = 0;
		m_spany = 0;
		m_spanz = 0;
	}

	void DoRand()
	{
		if (!mBStart && (rand()%3 == 2)&& clock()/10%60==0)
		{

			mBStart = true;
			mStartT = clock();
			m_timeAdd = 460 + rand() % 100;
			m_timeNorm = 1900 + rand() %500+ m_timeAdd;
			m_timeDel = m_timeAdd + 60+ m_timeNorm;
			

			float mspan = 0.0020;
			m_spanx = ((rand() % 3 )*mspan*1.2);
			m_spany = ((rand() % 3 + 1)*mspan*1.2);
			m_spanz = ((rand() % 2 )*mspan*0.3);

			if (rand() % 2 == 0)
			{
				m_spanx *= -1;
			}
			if (rand() % 2 == 0)
			{
				m_spany *= -1;
			}
			if (rand() % 2 == 0)
			{
				m_spanz *= -1;
			}
		}

		if (mBStart)
		{
		
			if (clock() - mStartT < m_timeAdd)
			{
				m_roatex += m_spanx;
				m_roatey += m_spany;
				m_roatez += m_spanz;
			}
			else if (clock() - mStartT < m_timeNorm)
			{

			}
			else  if (clock() - mStartT < m_timeDel)
			{
				m_roatex -= m_spanx*0.9;
				m_roatey -= m_spany*0.9;
				m_roatez -= m_spanz*0.9;
			}
			else
			{
				m_spanx = 0;
				m_spany = 0;
				m_spanz = 0;

				m_roatex = 0;
				m_roatey = 0;
				m_roatez = 0;
				mBStart = false;
			}
		}
	}

	clock_t mStartT;
	bool mBStart;
	float m_roatex;
	float m_roatey;
	float m_roatez;

};


struct s_ani
{
	float eyeblink;
	float rotatex;
	float rotatey;
	float rotatez;
	float eyerx;
	float eyery;
};
#include <Common/gLobalInfo.h>
class CMFCDlgDlg : public CDialog
{
// 构造
public:
	std::vector<s_ani> m_listani;
	timeVoiceMgr mstimeos;
	void voiceStart(int longtime, std::vector<PhonemeData>  tstrs);

	CMFCDlgDlg(CWnd* pParent = NULL);	// 标准构造函数
	void StartTimer();
	void InitWndDC();
	randAnit mrandAnit;
	int m_TimeSpan;
// 对话框数据
	enum { IDD = 130 };

	void StopTimerMy();
	void DoReloadMat();

	//广告显示效果B
	void ShowNowTestB(eos::core::Mesh& obj,cv::Mat mat);


	void SaveCharacter(int iSel);
	void ReadCharacter(int iSel);
	void RotateX(bool bPosi);
	void RotateY(bool bPosi);
	void RotateZ(bool bPosi);
	
	void RotateXFv(float fV);
	float GetRotateXFv() { return m_nowEntity->GetRotateXFv(); }

	void RotateYFv(float fV);
	float GetRotateYFv() { return m_nowEntity->GetRotateYFv(); }

	void RotateZFv(float fV);
	float GetRotateZFv() { return m_nowEntity->GetRotateZFv(); }


	void SetEeyRx(float fv);
	float GetEyeRx() { return m_nowEntity->GetEyeRx(); }

	void SetEeyRy(float fv);
	float GetEyeRy() { return m_nowEntity->GetEyeRy(); }

	void SetEyeBlinkV(float fEyeBlink);
	float GetEyeBlinkV() { return m_nowEntity->GetEyeBlinkV(); }

	void Rerotate0();
	void SetDlgMain(CDlgFaceMain * pMain);

	void ChangeIdentityName(char *objname);

	void ChangeIdentityIdexStart(int idexStart);
	void SetTieTu(int itietu);
	void SetTushi(int tushi);

	void SetCharcterWeight(int* weight,int starP=0);
	void ChangeMotion(int* weight);
	int *GetCharcterWeight();
protected:
	void ChangeYicEnity(YicEntity * entity, eos::core::Mesh& obj, cv::Mat mat);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	int m_motionWeight[100];//角色变形的权重
protected:
	CDlgFaceMain * m_dlgMain;

	C3DFace m_face3D;
	YicEntity * m_nowEntity;
	
	bool  m_bFullScreen;
	int m_timerID;
// 实现
protected:
	std::vector<float> mBlends;
	
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

	void ShowToFront();

	void Execute();


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	void SetBlends(std::vector<float>  blends);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
