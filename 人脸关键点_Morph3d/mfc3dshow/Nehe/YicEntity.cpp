#include "YicEntity.h"



YicEntity::YicEntity(void)
{
	
	 mMouthLeft = 0;
	 mMouthRight = 0;
	m_characterStart = 0;
	mEyeRx = 0;
	mEyeRy = 0;

	mrotateXF = 0;
	mrotateYF = 0;
	mrotateZF = 0;

	m_toushi = 0;
	m_tietu = 0;

	//m_characterWeight = 0;
	for (int k = 0; k < 100; ++k)
	{
		m_characterWeight[k] = 0;
	}
}


YicEntity::~YicEntity(void)
{

}

bool  YicEntity::OnCreate(CWnd * pWnd)
{
	m_pParentWnd = pWnd;
	return true;
}
void YicEntity::Rerotate0()
{
	mrotateXF = 0;
	mrotateYF = 0;
	mrotateZF = 0;
}

void YicEntity::RotateX(bool bPosi)
{
	if (bPosi)
	{
		mrotateXF += 1;
	}
	else
	{
		mrotateXF -= 1;
	}
	
}

void YicEntity::RotateY(bool bPosi)
{
	if (bPosi)
	{
		mrotateYF += 1;
	}
	else
	{
		mrotateYF -= 1;
	}
}

void YicEntity::RotateZ(bool bPosi)
{
	if (bPosi)
	{
		mrotateZF += 1;
	}
	else
	{
		mrotateZF -= 1;
	}
}
void YicEntity::RotateXFv(float fV)
{
	mrotateXF = fV;
}

void YicEntity::RotateYFv(float fV)
{
	mrotateYF = fV;
}

void YicEntity::RotateZFv(float fV)
{
	mrotateZF = fV;
}


void YicEntity::SetEyeRx(float fV)
{
	mEyeRx = fV;
}
void YicEntity::SetEyeRy(float fV)
{
	mEyeRy = fV;
}

void YicEntity::SetEyeBlinkV(float fV)
{
	mEyeBlink_L = fV;
}


void  YicEntity::Render()
{

}

void   YicEntity::Execute(unsigned long deltaTime)
{

}


 
void YicEntity::DrawThePicPoint(cv::Mat dst,int showPosY)
{
		//若使用前不想把原来绘制的图片去掉，可以删去上面那三段
		CDC *pDC = m_pParentWnd->GetDC();
}
