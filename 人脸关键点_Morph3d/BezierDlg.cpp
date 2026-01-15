// BezierDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Bezier.h"
#include "BezierDlg.h"
#include <fstream>
#include <iostream>
#include<Common\CommonWin.h>
#include <stdio.h>
#include "atlimage.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include "resource.h"
#include <SpecialEfect\SpecialEfect.h>
#include <Common\gLobalInfo.h>
#include "globals.h"
#include <Common/hanziPinyin.h>
#include "testonnx.h"

void processWav(const std::string& inputFile, const std::string& outputFile);
CComPtr<ISpVoice>   m_cpVoice;
using namespace std;
using namespace cv;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool gbNeedSave = false;
/////////////////////////////////////////////////////////////////////////////
// CBezierDlg dialog
cv::Mat gMat;
CBezierDlg::CBezierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBezierDlg::IDD, pParent)
{

	m_bWorking = true;
	m_bMouseMove = false;
	m_leftBtnDown = false;
	m_bKeyOper = false;

	m_bDeleteFile = false;

	m_nowMulti = 1;
	m_points.resize(68);
	m_nLeftPos = 800;
	for(int k = 0;k<15;++k)
	   m_nCur[k]=0;
	m_3ddout = new float[500*3];
}

void CBezierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBezierDlg)
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_COMBO1, m_comboIdens);
	DDX_Control(pDX, IDC_COMBO2, m_combox2);
	DDX_Control(pDX, IDC_COMBO3, m_comboTietu);
	DDX_Control(pDX, IDC_COMBO4, m_comboToushi);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
}

BEGIN_MESSAGE_MAP(CBezierDlg, CDialog)
	//{{AFX_MSG_MAP(CBezierDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()


	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_SAVE, &CBezierDlg::OnBnClickedBtnSave)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOMMAND()

	ON_BN_CLICKED(IDC_BTN_BIG, &CBezierDlg::OnBnClickedBtnBig)
	ON_BN_CLICKED(IDC_BTN_ORIGIN, &CBezierDlg::OnBnClickedBtnOrigin)

	ON_MESSAGE(WM_USER+100, OnMyMessage)
	ON_MESSAGE(WM_USER + 101, OnMyMessage2)
	ON_MESSAGE(WM_USER + 102, OnMyMessageWave)
	ON_BN_CLICKED(IDC_BUTTON_IN, &CBezierDlg::OnBnClickedButtonIn)
	ON_BN_CLICKED(IDC_BUTTON_START, &CBezierDlg::OnBnClickedButton3d)
	ON_BN_CLICKED(IDC_BUTTON2, &CBezierDlg::OnBnClickedDrive3d)
	ON_BN_CLICKED(IDC_BTN_SAVEV, &CBezierDlg::OnBnClickedSavev)//保存参数
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER1, &CBezierDlg::OnNMThemeChangedSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER9, &CBezierDlg::OnNMCustomdrawSlider9)

	ON_CBN_SELCHANGE(IDC_COMBO1, &CBezierDlg::OnCbnSelchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CBezierDlg::OnCbnSelchangeCombo3)
	ON_CBN_SELCHANGE(IDC_COMBO4, &CBezierDlg::OnCbnSelchangeCombo4)
	ON_MESSAGE(WM_TTSAPPCUSTOMEVENT, &CBezierDlg::MainHandleSynthEvent)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK1, &CBezierDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_RELOADPIC, &CBezierDlg::OnBnClickedReloadpic)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBezierDlg message handlers

#include <thread>
void my_thread(char * pDlg)
{
	((CBezierDlg*)pDlg)->DoFrame();
}




bool file_exists(const char* filename) {
	FILE* file = fopen(filename, "r");
	if (file) {
		fclose(file);
		return true;
	}
	return false;
}


bool Exists(const std::string& filename) {
	std::ifstream is(filename);
	return is.good();
}

static std::condition_variable g_cv;
static std::mutex g_cv_m;

struct Samples {
	std::vector<float> data;
	int32_t consumed = 0;
};
#include <queue>
struct Buffer {
	std::queue<Samples> samples;
	std::mutex mutex;
};

static Buffer g_buffer;

static bool g_started = false;
static bool g_stopped = false;
static bool g_killed = false;

static int32_t AudioGeneratedCallback(const float* s, int32_t n) {
	if (n > 0) {
		Samples samples;
		samples.data = std::vector<float>{ s, s + n };

		std::lock_guard<std::mutex> lock(g_buffer.mutex);
		g_buffer.samples.push(std::move(samples));
		g_started = true;
	}
	if (g_killed) {
		return 0;
	}
	return 1;
}

#include "speech-aligner.h"
std::vector<std::string>  DoAlignAudio()
{
	// 原始命令行参数（模仿 speech-aligner 调用）
	std::vector<std::string> args = {
		"speech-aligner",               // 程序名（argv[0]）
		"--config=conf/align.conf",     // 配置文件
		"data/wav.scp",                 // 输入音频列表
		"data/text",                    // 输入文本
		"data/out.ali"                  // 输出对齐结果
	};

	// 构建 argv 数组
	int argc = args.size();
	char** argv = new char* [argc];

	for (int i = 0; i < argc; ++i) {
		argv[i] = new char[args[i].size() + 1]; // 分配内存
		strcpy(argv[i], args[i].c_str());       // 复制字符串
	}
	std::vector<std::string> result = dllspeechalign(argc, argv);
	// 调用 DLL 函数
	clock_t t1 = clock();
	result = dllspeechalign(argc, argv);
	clock_t t2 = clock();

	clock_t t3 = clock();
	int iCost = t3 - t2;
	printf("cost time is  %d %d\n", t3 - t2,t2-t1);
	for (int k = 0; k < result.size(); ++k)
	{
		printf("%s  \n", result[k].c_str());
	}
	// 释放内存
	for (int i = 0; i < argc; ++i) {
		delete[] argv[i];
	}
	delete[] argv;
	return result;
}

void CBezierDlg::GenerateAudio(std::string text)
{ 
	const SherpaOnnxGeneratedAudio* audio =
		SherpaOnnxOfflineTtsGenerateWithCallback(tts_, text.c_str(), 0, 1.0, &AudioGeneratedCallback);
	// generate_btn_.EnableWindow(TRUE);
	g_stopped = true;

	int ok = SherpaOnnxWriteWave(audio->samples, audio->n, audio->sample_rate, //
		"./data/wav/audio2.wav");

	SherpaOnnxDestroyOfflineTtsGeneratedAudio(audio);

	
	processWav("data/wav/audio2.wav", "data/wav/audio.wav");

	
}

void CBezierDlg::Init() {



	bool ok = true;
	std::string error_message = "--------------------\r\n";

	if (!file_exists("./model.onnx")) {
		error_message += "Cannot find ./model.onnx\r\n";
		ok = false;
	}

	if (!file_exists("./tokens.txt")) {

		error_message += "Cannot find ./tokens.txt\r\n";
		ok = false;
	}
	// it is OK to leave lexicon.txt and espeak-ng-data empty
	// since models using characters don't need them

	if (!ok) {

		return;
	}

	// Now init tts
	SherpaOnnxOfflineTtsConfig config;
	memset(&config, 0, sizeof(config));
	config.model.debug = 0;
	config.model.num_threads = 4;
	config.model.provider = "cpu";

	if (file_exists("./voices.bin"))
	{
		// it is a kokoro tts model
		config.model.kokoro.model = "./model.onnx";
		config.model.kokoro.voices = "./voices.bin";
		config.model.kokoro.tokens = "./tokens.txt";
		config.model.kokoro.data_dir = "./espeak-ng-data";
		if (file_exists("./dict/jieba.dict.utf8") && Exists("./lexicon-zh.txt")) {
			config.model.kokoro.dict_dir = "./dict";
			config.model.kokoro.lexicon = "./lexicon-us-en.txt,./lexicon-zh.txt";
		}
	}
	else if (file_exists("./hifigan.onnx") || Exists("./vocos.onnx")) {
		// it is a matcha tts model
		config.model.matcha.acoustic_model = "./model.onnx";

		if (Exists("./hifigan.onnx")) {
			config.model.matcha.vocoder = "./hifigan.onnx";
		}
		else if (Exists("./vocos.onnx")) {
			config.model.matcha.vocoder = "./vocos.onnx";
		}

		config.model.matcha.tokens = "./tokens.txt";

		if (file_exists("./espeak-ng-data/phontab")) {
			config.model.matcha.data_dir = "./espeak-ng-data";
		}

		if (file_exists("./lexicon.txt")) {
			config.model.matcha.lexicon = "./lexicon.txt";
		}

		if (Exists("./dict/jieba.dict.utf8")) {
			config.model.matcha.dict_dir = "./dict";
		}
	}
	else {
		// it is a vits tts model
		config.model.vits.model = "./model.onnx";
		config.model.vits.tokens = "./tokens.txt";
		if (Exists("./espeak-ng-data/phontab")) {
			config.model.vits.data_dir = "./espeak-ng-data";
		}

		if (Exists("./lexicon.txt")) {
			config.model.vits.lexicon = "./lexicon.txt";
		}

		if (Exists("./dict/jieba.dict.utf8")) {
			config.model.vits.dict_dir = "./dict";
		}
	}

	if (Exists("./phone.fst") && Exists("./date.fst") && Exists("./number.fst")) {
		config.rule_fsts = "./phone.fst,./date.fst,number.fst";
	}

	if (Exists("./phone-zh.fst") && Exists("./date-zh.fst") && Exists("./number-zh.fst")) {
		config.rule_fsts = "./phone-zh.fst,./date-zh.fst,number-zh.fst";
	}

	if (Exists("./rule.far"))
	{
		config.rule_fars = "./rule.far";
	}
	
	
	tts_ = SherpaOnnxCreateOfflineTts(&config);
}
void CBezierDlg::DoInitObj()
{
    m_dlgMfc.ShowNowTestB(m_obj, gMat);

	m_dlgMfc.StartTimer();

}
#include <opencv2/core.hpp>
#include <iostream>
#include <algorithm>   // std::min / std::max
using namespace cv;
using namespace std;

const int QR_W = 128;
const int BLK = 8;
const int TOTAL = QR_W * QR_W;

// ---------- 工具 ----------
static inline bool  getB(const Mat& m, int k) { return m.at<uchar>(k) > 0; }
static inline void  setB(Mat& m, int k, bool v) { m.at<uchar>(k) = v ? 255 : 0; }

Mat interleave(const Mat& qr) {
	Mat flat(1, TOTAL, CV_8UC1); int k = 0;
	for (int i = 0; i < QR_W; i++) for (int j = 0; j < QR_W; j++)
		setB(flat, k++, qr.at<uchar>(i, j));
	return flat;
}
Mat deinterleave(const Mat& flat) {
	Mat qr(QR_W, QR_W, CV_8UC1); int k = 0;
	for (int i = 0; i < QR_W; i++) for (int j = 0; j < QR_W; j++)
		qr.at<uchar>(i, j) = getB(flat, k++) ? 255 : 0;
	return qr;
}

// ---------- 嵌入 ----------
void embed(Mat& host, const Mat& qr) {
	Mat bits = interleave(qr);
	host.convertTo(host, CV_32F);
	vector<Mat> ch; split(host, ch); Mat& B = ch[0];

	const float delta = 25.f;               // 固定大 Δ
	int idx = 0;
	for (int y = 0; y + BLK <= B.rows; y += BLK)
		for (int x = 0; x + BLK <= B.cols; x += BLK) {
			if (idx >= TOTAL) break;
			Rect r(x, y, BLK, BLK);
			Mat blk = B(r), dct; cv::dct(blk, dct);
			float v = dct.at<float>(2, 1);   // 用 (2,1)
			bool b = getB(bits, idx);
			int q = cvRound(v / delta);
			if (b) q = (q & ~1) + 1; else q = (q | 1) - 1;
			dct.at<float>(2, 1) = q * delta;
			cv::idct(dct, blk);
			// 重复 8 次 → 用 8 个不同系数
			for (int k = 0; k < 16; k++) {
				if (idx >= TOTAL) break;
				idx++;
			}
		}
	merge(ch, host); host.convertTo(host, CV_8U);
}



Mat extract(const Mat& img) {
	Mat g; cvtColor(img, g, CV_BGR2GRAY); g.convertTo(g, CV_32F);
	const float delta = 25.f;

	vector<int> vote(TOTAL, 0);
	int idx = 0;                 // 当前比特序号
	int blockIdx = 0;            // 当前 8×8 块序号

	for (int y = 0; y + BLK <= g.rows; y += BLK) {
		for (int x = 0; x + BLK <= g.cols; x += BLK) {
			if (idx >= TOTAL) break;

			Rect r(x, y, BLK, BLK);
			Mat blk = g(r), dct; cv::dct(blk, dct);
			bool b = (cvRound(dct.at<float>(2, 1) / delta) & 1) == 1;

			// 把同一比特投 8 票
			for (int rep = 0; rep < 16; ++rep) {
				if (idx >= TOTAL) break;
				vote[idx] += b ? 1 : -1;
				++idx;
			}
		}
	}

	Mat flat(1, TOTAL, CV_8UC1);
	for (int i = 0; i < TOTAL; i++) setB(flat, i, vote[i] > 0);
	Mat qr = deinterleave(flat);
	medianBlur(qr, qr, 3);
	return qr;
}
// ------------- demo -------------
int maintest() {
	Mat host = imread("c:/work/host.png");                 // 任意尺寸
	Mat qr = imread("c:/work/qr128.png", IMREAD_GRAYSCALE);
	if (host.empty() || qr.empty()) {
		cerr << "图像加载失败\n";
		return -1;
	}

	// 二值化 QR
	if (host.empty() || qr.empty()) { cerr << "load fail\n"; return -1; }
	threshold(qr, qr, 127, 255, THRESH_BINARY);
	cv::imwrite("c:/work/qr.png", qr);

	embed(host, qr);
	imwrite("c:/work/embedded.png", host);

	// 模拟：随机裁剪 25% 区域 + 遮挡
	//Rect r(rand() % 100, rand() % 100, host.cols /4, host.rows /4);
	//host(r).setTo(0);
	//imwrite("c:/work/cropped.png", host);

	Mat recovered = extract(host);
	imwrite("c:/work/recovered.png", recovered);

	cout << "done\n";
	return 0;
}

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#pragma pack(push, 1)
struct WavHeader {
	char riff[4] = { 'R', 'I', 'F', 'F' };
	uint32_t fileSize;
	char wave[4] = { 'W', 'A', 'V', 'E' };
	char fmt[4] = { 'f', 'm', 't', ' ' };
	uint32_t fmtSize = 16;
	uint16_t audioFormat = 1;  // PCM
	uint16_t numChannels = 1;  // 单声道
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign = 2;
	uint16_t bitsPerSample = 16;
	char data[4] = { 'd', 'a', 't', 'a' };
	uint32_t dataSize;
};
#pragma pack(pop)

class AudioResampler {
public:
	AudioResampler(int inRate, int outRate)
		: inRate(inRate), outRate(outRate), ratio((float)outRate / inRate) {
	}

	std::vector<int16_t> resample(const std::vector<int16_t>& input) {
		size_t outSamples = (size_t)(input.size() * ratio);
		std::vector<int16_t> output(outSamples, 0);

		double phase = 0.0;
		double phaseInc = 1.0 / ratio;

		for (size_t i = 0; i < outSamples; ++i) {
			int pos = (int)phase;
			double frac = phase - pos;

			int nextPos = (pos + 1) < input.size() ? (pos + 1) : (input.size() - 1);
			int16_t val = (1 - frac) * input[pos] + frac * input[nextPos];

			output[i] = val;
			phase += phaseInc;
		}

		return output;
	}

private:
	int inRate, outRate;
	float ratio;
};

void processWav(const std::string& inputFile, const std::string& outputFile) {
	// 读取原始文件
	std::ifstream inFile(inputFile, std::ios::binary);
	WavHeader header;
	inFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	std::vector<int16_t> audioData(header.dataSize / sizeof(int16_t));
	inFile.read(reinterpret_cast<char*>(audioData.data()), header.dataSize);
	inFile.close();

	// 执行重采样
	AudioResampler resampler(header.sampleRate, 16000);
	auto outputData = resampler.resample(audioData);

	// 更新文件头
	header.sampleRate = 16000;
	header.byteRate = 16000 * header.blockAlign;
	header.dataSize = outputData.size() * sizeof(int16_t);
	header.fileSize = sizeof(WavHeader) + header.dataSize;

	// 写入新文件
	std::ofstream outFile(outputFile, std::ios::binary);
	outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
	outFile.write(reinterpret_cast<char*>(outputData.data()), outputData.size() * sizeof(int16_t));
	outFile.close();
}


BOOL CBezierDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	

	for (int k = 0; k < 13;++k )
	{
		CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + k);
		pSlidCtrl->SetRange(0,15);
		pSlidCtrl->SetPos(7);
	}



	m_dlgMfc.Create(130, this);
	//m_dlgMfc1.Create(130, this);
	CRect rect;
	GetClientRect(rect);
	m_dlgMfc.MoveWindow(20, 60, 900, 860);
	//m_dlgMfc1.MoveWindow(550, 240, 460, 460);
	
	m_dlgVideo.CreateObj(this);



	
	m_dlgMfc.ShowWindow(SW_SHOW);
	//m_dlgMfc1.ShowWindow(SW_SHOW);



	ObjLoader * tObjLoader = new ObjLoader();
	tObjLoader->LoadObj("./detection/canonical_face_model.obj");
	ObjModel * mdataObj = tObjLoader->ReturnPObj();

	//float* vert_data = (float*)tMmesh.vertices.data;
	//unsigned char* color_data = tMmesh.colors.data;
	if (m_obj.vertices.size() == 0)
	{
		m_obj.vertices.resize(mdataObj->NumVertex);
		m_obj.texcoords.resize(mdataObj->NumVertex);
	}

	for (int i = 0; i < mdataObj->NumVertex; ++i)
	{
		Vector3f vf;

		vf.x = mdataObj->VertexArray[i].X;
		vf.y = mdataObj->VertexArray[i].Y;
		vf.z = mdataObj->VertexArray[i].Z;
		m_obj.vertices[i] = vf;

		Vector2f tf;
		tf.x = mdataObj->TexCoordArray[i].U;
		tf.y = mdataObj->TexCoordArray[i].V;
		m_obj.texcoords[i] = tf;

	}


	if (m_obj.tvi.size() == 0)
	{
		m_obj.tvi.resize(mdataObj->NumTriangle);
	}

	for (int i = 0; i < mdataObj->NumTriangle; ++i)
	{
		std::array<int, 3> idexs;
		idexs[0] = mdataObj->TriangleArray[i].Vertex[0];
		idexs[1] = mdataObj->TriangleArray[i].Vertex[1];
		idexs[2] = mdataObj->TriangleArray[i].Vertex[2];
		m_obj.tvi[i] = idexs;
	}


    m_comboIdens.AddString("角色1");
	m_comboIdens.AddString("角色2");
	m_comboIdens.AddString("角色3");
	m_comboIdens.AddString("角色4");
	m_comboIdens.AddString("角色5");

	m_combox2.AddString("0-10");
	m_combox2.AddString("10-20");

	m_comboTietu.AddString("无胡须贴图");
	m_comboTietu.AddString("胡须贴图");
	m_comboTietu.AddString("贴图3");
	m_comboTietu.AddString("贴图4");
	m_comboTietu.AddString("贴图5");
	m_comboTietu.AddString("贴图6");

	m_comboToushi.AddString("无");
	m_comboToushi.AddString("头发1");
	m_comboToushi.AddString("头发2");
	m_comboToushi.AddString("头发3");
	m_comboToushi.AddString("头发4");


	m_dlgMfc.InitWndDC();


	DoInitObj();

	Init();
	// Initialize the voice object


	HRESULT hr = CoInitialize(NULL);



	return TRUE;  // return TRUE  unless you set the focus to a control
}


HRESULT CBezierDlg::VoiceChange()
/////////////////////////////////////////////////////////////////
//
// This function is called during initialization and whenever the
// selection for the voice combo box changes.
// It gets the token pointer associated with the voice.
// If the new voice is different from the one that's currently
// selected, it first stops any synthesis that is going on and
// sets the new voice on the global voice object.
//
{
	HRESULT         hr = S_OK;
	
	return hr;
}



LRESULT  CBezierDlg::MainHandleSynthEvent(WPARAM wparam, LPARAM lParam)
{
	CSpEvent        event;  // helper class in sphelper.h for events that releases any
							// allocated memory in it's destructor - SAFER than SPEVENT
	SPVOICESTATUS   Stat;
	WPARAM          nStart;
	LPARAM          nEnd;
	//int             i = 0;
	HRESULT 		hr = S_OK;

	if (event.GetFrom(m_cpVoice) == S_OK)
	{
		switch (event.eEventId)
		{
		case SPEI_START_INPUT_STREAM:

			break;

		case SPEI_END_INPUT_STREAM:
			// Set global boolean stop to TRUE when finished speaking

			// Highlight entire text
			nStart = 0;

			// Mouth closed
			g_iBmp = 0;
			char szinfo[256];
			wsprintf(szinfo, "kkkkk  g_iBmp %d *********************************\n", g_iBmp);
			OutputDebugString(szinfo);
		
			break;

		case SPEI_VOICE_CHANGE:

			break;

		case SPEI_TTS_BOOKMARK:

			break;

		case SPEI_WORD_BOUNDARY:
			hr = m_cpVoice->GetStatus(&Stat, NULL);


			// Highlight word
			nStart = (LPARAM)(Stat.ulInputWordPos / sizeof(char));
			nEnd = nStart + Stat.ulInputWordLen;

			break;

		case SPEI_PHONEME:

			break;

		case SPEI_VISEME:
			// Get the current mouth viseme position and map it to one of the
			// 7 mouth bitmaps.
			g_iBmp = g_aMapVisemeToImage[event.Viseme()]; // current viseme
			

			wsprintf(szinfo, "kkkkk  g_iBmp %d\n", g_iBmp);
			OutputDebugString(szinfo);

			break;

		case SPEI_SENTENCE_BOUNDARY:

			break;

		case SPEI_TTS_AUDIO_LEVEL:

			break;

		case SPEI_TTS_PRIVATE:

			break;

		default:

			break;
		}
	}

	return 1;
}
void CBezierDlg::DrawThePicPoint(cv::Mat clmResult, int posX, int posY, int dstW)
{

	//若使用前不想把原来绘制的图片去掉，可以删去上面那三段
	CDC *pDC = GetDC();


	int biaoWidht = dstW;
	cv::Mat dst;
	if (clmResult.cols > dstW)
	{
		float ratio = clmResult.cols*1.0 / biaoWidht;
		int height = clmResult.rows / ratio;

		resize(clmResult, dst, cv::Size(biaoWidht, height));
	}
	else
	{
		dst = clmResult.clone();
	}

	CImage Image;
	Image.Create(dst.cols, dst.rows, 24);
	int i;
	int j;
	for (i = 0; i < Image.GetHeight(); i++)
	{
		int step = dst.step[0] * i;
		int jump = 0;
		for (j = 0; j < Image.GetWidth(); j++)
		{
			byte *pByte = (byte *)Image.GetPixelAddress(j, i);

			if (dst.channels() != 1)
			{
				pByte[0] = (unsigned char)dst.data[step + jump + 0];
				pByte[1] = (unsigned char)dst.data[step + jump + 1];
				pByte[2] = (unsigned char)dst.data[step + jump + 2];

				jump += 3;
			}
			else
			{
				pByte[0] = (unsigned char)dst.data[step + jump + 0];
				pByte[1] = (unsigned char)dst.data[step + jump + 0];
				pByte[2] = (unsigned char)dst.data[step + jump + 0];

				jump += 1;
			}

		}
	}

	Image.Draw(pDC->m_hDC, posX, posY);
	Image.Destroy();

	ReleaseDC(pDC);
}


vector<string> splitstr(const string& str, const string& delim) {
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char * strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}
	return res;
}


void DrawMyTextBK(CString text, int posX, int posY, CWnd * pWnd, COLORREF bkColor, int FontSize, COLORREF textColor)
{
	if (pWnd == NULL) return;

	CDC * pDc = pWnd->GetDC();
	if (pDc)
	{
		CFont font;
		font.CreateFont(
			FontSize, // nHeight
			0, // nWidth
			0, // nEscapement
			0, // nOrientation
			FW_NORMAL, // nWeight
			FALSE, // bItalic
			FALSE, // bUnderline
			0, // cStrikeOut
			ANSI_CHARSET, // nCharSet
			OUT_DEFAULT_PRECIS, // nOutPrecision
			CLIP_DEFAULT_PRECIS, // nClipPrecision
			DEFAULT_QUALITY, // nQuality
			DEFAULT_PITCH | FF_SWISS,
			_T("微软雅黑") // nPitchAndFamily Arial
		);
		CFont * oldFont = pDc->SelectObject(&font);

		CRect shortrect;
		shortrect.left = posX;
		shortrect.top = posY;

		shortrect.right = shortrect.left + 70*FontSize*0.5;
		shortrect.bottom = shortrect.top + FontSize + 2;

		pDc->SetTextColor(textColor);
		pDc->FillSolidRect(shortrect, bkColor);
		//pDc->SetBkColor(RGB(240,240,240));
		//pDc->SetBkMode(TRANSPARENT);

		pDc->DrawText(text, shortrect, DT_SINGLELINE | DT_LEFT | DT_TOP); //显示文本
		pDc->SelectObject(oldFont);

		pWnd->ReleaseDC(pDc);
	}

}


LRESULT CBezierDlg::OnMyMessage2(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 5000)
	{
		CRect rect(400, 0, 800, 185);
		InvalidateRect(rect);
		return 1;
	}
	
	return 0;
}

LRESULT CBezierDlg::OnMyMessageWave(WPARAM wParam, LPARAM lParam)
{
	

	//m_cpVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	// do we speak or interpret the XML
	//HRESULT  hr = m_cpVoice->Speak(L"这里注意，", SPF_ASYNC, 0);

	// m_cpVoice->Speak(L"曾国藩道光十八年后升迁内阁学士", SPF_ASYNC, 0); //曾国藩道光十八年后升迁内阁学士
	char szname[256];
	char szcontent[256];

	memset(szname,0,sizeof(szname));
	memset(szcontent, 0, sizeof(szcontent));

	FILE * fp = fopen("D:/1.txt", "rb");
	if (fp)
	{
		fread(szname, 1, sizeof(szname), fp);
		fclose(fp);
	}

	fp = fopen("D:/2.txt", "rb");
	if (fp)
	{
		fread(szcontent, 1, sizeof(szcontent), fp);
		fclose(fp);
	}
	

	std::vector<std::string>	strzis = getLetters(szcontent);
	int Len = GetFileSize(szname);
	//m_dlgMfc.voiceStart(Len*1.0 / 32 - 800, strzis);


	clock_t t2 = clock();

	return 0;
}


float * out2data = NULL;
LRESULT CBezierDlg::OnMyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1)
	{
		
		clock_t t1 = clock();
		std::vector<float> outblends ;
		mMutexBk.lock();
		outblends = mBlends;
		mMutexBk.unlock();

		do3dWork(outblends);
		
		char szinfo[256];
        wsprintf(szinfo, "kkkkk  %d\n", lParam);
		SetWindowText(szinfo);
	}
	
	
	return 1;
}

void CBezierDlg::DoFrame()
{
	while (m_bWorking)
	{
		cv::Mat imgMat;
		mMutex.lock();
		imgMat = m_srcImg.clone();
		mMutex.unlock();

		if (!imgMat.empty())
		{
			cv::Mat outimg;
		    CLMGetCNN106(imgMat, outimg);
		}
	
		Sleep(10);
	}
}


void CBezierDlg::VdoFrameData(long lw, long lh, unsigned char * pBuffer, long lBufferSize)
{
	if (m_dlgVideo.IsWindowVisible())
	{
		Mat src(lh, lw, CV_8UC3);
		memcpy(src.data, pBuffer, lh*lw * 3);
		flip(src, src,0);
		
		mMutex.lock();
		m_srcImg = src;
		mMutex.unlock();
		
	}
	

}




void CBezierDlg::CLMGetCNN106(cv::Mat dst, cv::Mat showMat)
{

	
	
	char szInfo[256];

	float outRect[100];
	int iState[20];
	vector<cv::Point2f> _points;
	clock_t t1 = clock();
	float fScore = 0.0;
	cv::Rect outR;

	
	std::vector<float> outblends = 	testBlender52(dst);
	clock_t t2 = clock();
	mMutexBk.lock();
	mBlends = outblends;
	mMutexBk.unlock();

	if(outblends.size() > 0)
	{
	   PostMessage(WM_USER + 100, 1,t2-t1);
	}
	else
	{
		PostMessage(WM_USER + 100, 0,0);
	}
	
}



/*

	功能：画人脸关键点对应的 移动框
*/
void CBezierDlg::DrawPoint(int start,int end,CDC* pDC)
{
	pDC->MoveTo(m_points[start].x, m_points[start].y);
	for(int i = start; i<=end; i++)
	{
			CPen penStroke(PS_SOLID,1,0x007700);
			CPen *ppenPrevious=pDC->SelectObject(&penStroke);
			pDC->LineTo(m_points[i].x,m_points[i].y);
			pDC->SelectObject(ppenPrevious);

			CPen penStroke1(PS_SOLID,1,0x00FFFF);
		    CPen penStroke2(PS_SOLID,1,0x0000FF);

			CPen *ppenPrevious2;
			if(m_currentMark == i)
			{
				ppenPrevious2 = pDC->SelectObject(&penStroke2);
			}
			else
			{
				ppenPrevious2 = pDC->SelectObject(&penStroke1);
			}
			

			pDC->SetBkMode(TRANSPARENT);
		
			cv::Point2d pt = m_points[i];
			CPoint tmpPts[4];
			int spanD = 6;
			tmpPts[0].x = pt.x - spanD;
			tmpPts[0].y = pt.y - spanD;

			tmpPts[1].x = pt.x + spanD;
			tmpPts[1].y = pt.y - spanD;
			tmpPts[2].x = pt.x + spanD;
			tmpPts[2].y = pt.y + spanD;
			tmpPts[3].x = pt.x - spanD;
			tmpPts[3].y = pt.y + spanD;
		
			
			 pDC->SelectStockObject(NULL_BRUSH);
			 pDC->Ellipse(tmpPts[0].x,tmpPts[0].y,tmpPts[2].x,tmpPts[2].y);
			 char szinfo[256];
			 sprintf(szinfo,"%d",i);
			 CRect rt(pt.x, pt.y, 40, 40); 
			 pDC->SetTextColor(RGB(255,255 , 0));
			 pDC->DrawText(szinfo, rt, TA_CENTER);
		
			pDC->SelectObject(ppenPrevious2);
	}
}
/*

功能：本绘制主要是  因为在进行微调关键点的时候，防止闪烁， 采用双缓冲区绘制
  绘制 背景待处理图片，然后绘制关键点，绘制到缓冲区，接着整体绘制
  
*/
void CBezierDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
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

void CBezierDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	point.x -= m_nLeftPos;
	
	m_leftBtnDown = true;
	// TODO: Add your message handler code here and/or call default
	m_currentMark = -1;
		double x, y;
		double t=50;
		for(int i = 0; i < m_points.size(); i++)
		{
			x = m_points[i].x - point.x, y = m_points[i].y - point.y;
			x*=x; y*=y;
			if(x + y < t)
			{	
				m_currentMark = i;	t=x+y;	
			}

		}

		
	
	CDialog::OnLButtonDown(nFlags, point);
	RedrawWindow();
}

void CBezierDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_leftBtnDown = false;
	// TODO: Add your message handler code here and/or call default
	m_bMouseMove = false;
	

	CDialog::OnLButtonUp(nFlags, point);
}

void CBezierDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(!m_bMouseMove && m_leftBtnDown)
	{
		m_bMouseMove = true;
	}

	
	// TODO: Add your message handler code here and/or call default
	if(m_currentMark >= 0 && m_leftBtnDown) 
	{
		m_points[m_currentMark].x = (point.x- m_nLeftPos);
		m_points[m_currentMark].y = point.y;
		RedrawWindow();
	}
	

	CDialog::OnMouseMove(nFlags, point);
}







void CBezierDlg::DoGetPtsAndDraw(CString fileName)
{
	   m_nowMulti = 1;
	   m_fileName=fileName; //文件名保存在了FilePathName里
		CString ptsFile = m_fileName;
	
		int index = ptsFile.Replace(".jpg",".pts");
		if(index == 0)
		{
			index = ptsFile.Replace(".png",".pts");
			if(index == 0)
			{
				index = ptsFile.Replace(".bmp",".pts");
			}
		}

		ifstream locations(ptsFile.GetBuffer(0), ios_base::in);
		if(locations.is_open() && index)
		{
			m_srcImg = cv::imread(m_fileName.GetBuffer(0));
			string line;
			// The main file contains the references to other files
			while (!locations.eof())
			{ 
				getline(locations, line);

				if(line.compare("{") == 0)
				{
						for(int i=0;i<68;i++)
						{
							locations>>m_points[i].x;
							locations>>m_points[i].y;
						}
				}
			}
		 
			
		}
		else
		{

				//自动生成 关键点
				m_fileName=fileName; //文件名保存在了FilePathName里
				Mat img = imread(m_fileName.GetBuffer(0));
				cv::Mat outimg;
				CLMGetCNN106(img, outimg);
				

				m_srcImg = img;


		}


		if(!m_srcImg.empty())
		{
			m_nowImg = m_srcImg.clone();

			CString strInfo;
			strInfo.Format(" 当前放大倍数 %d倍.可移动上下左右按键微调 ",m_nowMulti);
			SetWindowText("关键点编辑器   "+m_fileName+strInfo);

			Invalidate();
		}
		else
		{ 
			
			   SetWindowText("关键点编辑器    "+m_fileName);
			
			    Mat dst = imread(m_fileName.GetBuffer(0)); 
				CDC * pDC = GetDC();
				if(!dst.empty() && pDC)
				{
					
					CImage Image;
					Image.Create(dst.cols,dst.rows,24);

					int i;
					int j;
					for (i = 0; i < Image.GetHeight(); i++)
					{
						int step = dst.step[0]*i;
						int jump = 0;
						for (j = 0; j < Image.GetWidth(); j++)
						{
							byte *pByte = (byte *)Image.GetPixelAddress(j,i);
							pByte[0] = (unsigned char)dst.data[step+jump+0];
							pByte[1] = (unsigned char)dst.data[step+jump+1];
							pByte[2] = (unsigned char)dst.data[step+jump+2];
		
							jump+=3;
						}
					}

					Image.Draw(pDC->m_hDC,0,0);
					ReleaseDC(pDC);
				}
			  

		}

}
void CBezierDlg::OnBnClickedBtnImport()
{
	// TODO: 在此添加控件通知处理程序代码
	CString FilePathName;
    CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
        NULL, 
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        (LPCTSTR)_TEXT("pic Files (*.jpg)|*.jpg|(*.bmp)|*.bmp|(*.png)|*.png|All Files (*.*)|*.*||"),
        NULL);
    if(dlg.DoModal()==IDOK)
    {
		DoGetPtsAndDraw(dlg.GetPathName());
		m_dlgMfc.ShowWindow(SW_HIDE);
	}
}
float tYinzi[30];
float tRoate[30];
float tYan[30];
int iYinziTag = 0;


void CBezierDlg::do3dWork(std::vector<float> blends)
{
	

	SetStart(true);

	//对3d进行blends赋值
	m_dlgMfc.SetBlends(blends);



}
void CBezierDlg::OnBnClickedBtnSave()
{

	
	// TODO: 在此添加控件通知处理程序代码
	if(!m_srcImg.empty())
	{
		CString ptsFile = m_fileName;
		int index = ptsFile.Replace(".jpg",".pts");
		if(index == 0)
		{
			index = ptsFile.Replace(".png",".pts");
		}
	
		if(index == 0)
		{
			index = ptsFile.Replace(".bmp",".pts");
		}
		ofstream locations(ptsFile.GetBuffer(0), ios_base::out);
		if(!locations.is_open())
		{
			cout << "Couldn't open the model file, aborting" << endl;
			return ;
		}
		string str = "version: 1";
		locations<<str<<"\n";
		str= "n_points:  68";
		locations <<str<<"\n";
		locations<<"{"<<"\n";

		for(int i=0;i<68;i++)
		{
			locations<<(m_points[i].x/m_nowMulti)<<" ";
			locations<<(m_points[i].y/m_nowMulti)<<"\n";
		}
		locations<<"}";
		locations.close();

	

		OnBnClickedButton3d();
	}
	
	
}


BOOL CBezierDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_leftBtnDown)
	{
		if(m_bKeyOper)
		{
			m_bKeyOper = false;return 1;
		}
		else
		{
			return CDialog::OnEraseBkgnd(pDC);
		}
	}
	else
	{
		return 1;
	}
}



void CBezierDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nID == 61488)
	{
		

	
		Invalidate();
	}
	CDialog::OnSysCommand(nID, lParam);
}



BOOL CBezierDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_TTSAPPCUSTOMEVENT)
	{
		return MainHandleSynthEvent(pMsg->wParam, pMsg->lParam);
	}
	if(pMsg->message == WM_KEYUP)
	{
		
			int span = 1;
			switch (pMsg->wParam)
			{
			case VK_UP:
				{
					if(m_currentMark > 0)
					{
						m_points[m_currentMark].y-=span;
						m_bKeyOper = true;
						RedrawWindow();
					}
				}
				break;
			case VK_DOWN:
				if(m_currentMark > 0)
					{
						m_points[m_currentMark].y+=span;
						m_bKeyOper = true;
						RedrawWindow();
					}
				break;
			case VK_RIGHT:
				if(m_currentMark > 0)
					{
						m_points[m_currentMark].x+=span;
						m_bKeyOper = true;
						RedrawWindow();
					}
				break;
			case VK_LEFT:
				if(m_currentMark > 0)
					{
						m_points[m_currentMark].x-=span;
						m_bKeyOper = true;
						RedrawWindow();
					}
			
				break;
			default:
				break;
			}
			

		
		
	}

	if(pMsg->wParam == VK_UP
				|| pMsg->wParam == VK_DOWN
				|| pMsg->wParam == VK_LEFT
				|| pMsg->wParam == VK_RIGHT)
			{
				if(m_currentMark > 0)
				{
					return 1;
				}
			}

	return CDialog::PreTranslateMessage(pMsg);
}


void CBezierDlg::OnBnClickedBtnBig()
{
	m_nowMulti ++;
	if(!m_srcImg.empty())
	{
		resize(m_srcImg,m_nowImg,Size(m_srcImg.cols*m_nowMulti,m_srcImg.rows*m_nowMulti));
		for(int k = 0;k<m_points.size();++k)
		{
			m_points[k].x = m_points[k].x*m_nowMulti;
			m_points[k].y = m_points[k].y*m_nowMulti;
		}
	}
	RedrawWindow();
}


void CBezierDlg::OnBnClickedBtnOrigin()
{

	if(!m_srcImg.empty())
	{
		m_nowImg = m_srcImg.clone();
		for(int k = 0;k<m_points.size();++k)
		{
			m_points[k].x = m_points[k].x/m_nowMulti;
			m_points[k].y = m_points[k].y/m_nowMulti;
		}
	}

	RedrawWindow();
	m_nowMulti = 1;
}


void CBezierDlg::OnBnClickedButton3d(cv::Mat img, vector<cv::Point2f> pts)
{
	clock_t t1 = clock();
	
	clock_t t2 = clock();
	char szinfo[256];
	sprintf(szinfo, "%d", t2 - t1);
	SetWindowText(szinfo);

	eos::core::Mesh * mesh = new eos::core::Mesh;
	*mesh = m_obj;
	cv::Mat * mat = new cv::Mat;
	*mat = img;
	
	PostMessage(WM_USER+100,(WPARAM) mesh, (LPARAM)mat);

	

}

ISpVoice * pVoice = NULL;
void CBezierDlg::OnRecord(char *filename,char* szContent)
{
	
	HRESULT hr = 0;

	

	if (pVoice == NULL)
	{
		 hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	}
	
	if (SUCCEEDED(hr))
	{
	
		USES_CONVERSION;
		WCHAR m_szWFileName[256];
		WCHAR m_szContent[256];
		wcscpy(m_szWFileName, T2W(filename));
		wcscpy(m_szContent, T2W(szContent));

		CSpStreamFormat OriginalFmt;
		CComPtr<ISpStream> cpWavStream;
		CComPtr<ISpStreamFormat> cpOldStream;

		//设置语音相关参数
		//SPSTREAMFORMAT eFmt = OriginalFmt.ComputeFormatEnum().SPSF_22kHz8BitStereo;   
		//SPSTREAMFORMAT eFmt = 21; 
		//SPSTREAMFORMAT eFmt = OriginalFmt.ComputeFormatEnum().SPSF_22kHz8BitStereo; //SPSF_22kHz 8Bit Stereo		
		//CSpStreamFormat Fmt;
		//Fmt.AssignFormat(eFmt);

		HRESULT hr = pVoice->GetOutputStream(&cpOldStream);
		if (hr == S_OK)
		{
			hr = OriginalFmt.AssignFormat(cpOldStream);
		}
		else
		{
			hr = E_FAIL;
		}

		if (SUCCEEDED(hr))
		{
			hr = SPBindToFile(m_szWFileName, SPFM_CREATE_ALWAYS, &cpWavStream,
				&OriginalFmt.FormatId(), OriginalFmt.WaveFormatExPtr());
		}
		if (SUCCEEDED(hr))
		{
			pVoice->SetOutput(cpWavStream, TRUE);
		}

		//pVoice->Speak(m_Text.AllocSysString(), SPF_ASYNC | SPF_IS_NOT_XML, 0 );
		pVoice->Speak(m_szContent, SPF_ASYNC | SPF_IS_NOT_XML, 0);
		pVoice->WaitUntilDone(INFINITE);
		cpWavStream.Release();
		pVoice->SetOutput(cpOldStream, FALSE);
	}

}
bool containsDigit(const std::string& str) {
	return std::any_of(str.begin(), str.end(), [](char c) {
		return isdigit(static_cast<unsigned char>(c));
		});
}
#include <locale>
#include <codecvt>
// ANSI 字符串 → UTF-8 字符串（Windows 示例）
std::string ansi_to_utf8(const std::string& ansi_str) {
	// Step 1: ANSI → UTF-16
	int utf16_len = MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), -1, nullptr, 0);
	std::wstring utf16_str(utf16_len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, ansi_str.c_str(), -1, &utf16_str[0], utf16_len);

	// Step 2: UTF-16 → UTF-8
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
	return converter.to_bytes(utf16_str);
}
int itag = 0;

void remove_first(std::string& str, const std::string& target) noexcept
{
	auto pos = str.find(target);
	if (pos != std::string::npos)
		str.erase(pos, target.length());
}
#include <unordered_map>
#include <regex>
#include <iostream>
#include <string>
unordered_map<char, string> digitToChinese = {
	{'0', "零"}, {'1', "一"}, {'2', "二"}, {'3', "三"}, {'4', "四"},
	{'5', "五"}, {'6', "六"}, {'7', "七"}, {'8', "八"}, {'9', "九"}
};

string convertNumberToChinese(const string& numStr) {
	int len = numStr.length();
	if (len == 1) {
		return digitToChinese[numStr[0]];
	}
	else if (len == 2) {
		int tens = numStr[0] - '0';
		int ones = numStr[1] - '0';
		string result;
		if (tens == 1) {
			result += "十";
		}
		else if (tens > 1) {
			result += digitToChinese[numStr[0]] + "十";
		}
		if (ones != 0) {
			result += digitToChinese[numStr[1]];
		}
		return result;
	}
	else {
		string result;
		for (char c : numStr) {
			result += digitToChinese[c];
		}
		return result;
	}
}
inline bool isDigit(char c) { return c >= '0' && c <= '9'; }

std::string processText(const std::string& text)
{
	std::string out;
	for (size_t i = 0; i < text.size(); ++i)
	{
		if (isDigit(text[i]))
		{
			size_t j = i;
			while (j < text.size() && isDigit(text[j])) ++j;
			out += convertNumberToChinese(text.substr(i, j - i));
			i = j - 1;               // -1 因为 for 会再 ++
		}
		else
		{
			out += text[i];
		}
	}
	return out;
}

void Replaleace(std::string& strinfo, std::string value,std::string replace)
{
	if (strinfo.find(value) != std::string::npos)
	{
		strinfo = strinfo.replace(strinfo.find(value), value.size(), replace);
	}
}
void FilterString(std::string& strinfo)
{
	Replaleace(strinfo,"朝辞白","昭辞白");
	Replaleace(strinfo, "万重", "万虫");
	Replaleace(strinfo, "重试", "虫试");
	Replaleace(strinfo, "日还", "日环");
	
}

void CBezierDlg::OnBnClickedButtonIn()
{
	std::string strinfo = "如果物理内存不足，会自动使用虚拟内存，但你可以手动增加.";
	
	CString strtext;
	GetDlgItemText(IDC_EDIT1, strtext);
	if (strtext.IsEmpty())
	{
		strinfo = "请在此文本框输入文字。";
	}
	else
	{
		strinfo = strtext.GetBuffer(0);
	}
	//多音字进行过滤
	FilterString(strinfo);
	//处理数字
	strinfo = processText(strinfo);
	GenerateAudio(strinfo);

	for (int k = 0; k < 3; ++k)
	{
		remove_first(strinfo, ",");
		remove_first(strinfo, ".");
		remove_first(strinfo, "，");
		remove_first(strinfo, "。");
	}


	std::vector<std::string>	strzis = getLetters(strinfo);
	//"data/text",                    // 输入文本
	//audio1 不一样的烟火不一样的我
	//audio1 bu1 yi2 yang3 de2 yan2 huo2 bu1 yi2 yang2 de3 wo4
	FILE * fp = fopen("./data/text", "wb");
	char szinfo[256];

	std::string str = "audio ";
	str+=strinfo;
	str += "\n";
	//str转为utf-8
    str = ansi_to_utf8(str);
	fwrite(str.c_str(),1, strlen(str.c_str()), fp);

	str = "audio ";
	for (int k = 0; k < strzis.size(); ++k)
	{
		str += strzis[k] + "2";
		if (k != strzis.size() - 1)
		{
			str += " ";
		}
		
	}
	str += "\n";
    fwrite(str.c_str(), 1, strlen(str.c_str()), fp);
    fclose(fp);

	std::vector<std::string> alignstrs = DoAlignAudio();

	clock_t t1 = clock();
	//m_cpVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
	// do we speak or interpret the XML
	//HRESULT  hr = m_cpVoice->Speak(L"这里注意，", SPF_ASYNC, 0);

	// m_cpVoice->Speak(L"曾国藩道光十八年后升迁内阁学士", SPF_ASYNC, 0); //曾国藩道光十八年后升迁内阁学士

	sprintf(szinfo, "./data/wav/audio.wav");
	StopPlayingFromFile(szinfo);//m_SoundOut.stop()
	m_nPlayLen = GetFileSize(szinfo);
	

	
	 std::vector<PhonemeData> outinfos = parsePhonemeData(alignstrs);
	 // 打印解析结果
	 for ( auto& data : outinfos) {
		 //std::cout << "Start: " << data.start
		 //	<< " \tEnd: " << data.end
		 //	<< " \tPhoneme: " << data.phoneme << "\n";
		 data.start *= 1000;
		 data.end *= 1000;

	 }

	 for (int k = outinfos.size() - 1; k >= 1; --k)
	 {
		 if (containsDigit(outinfos[k].phoneme))
		 {
			 //
			 if (outinfos[k - 1].phoneme.find("sil") != -1)
			 {
				 continue;
			 }
			 else
			 {
				 if (!containsDigit(outinfos[k - 1].phoneme) || outinfos[k-1].phoneme.find("$0") != -1)
				 {
					 //合并两个
					 outinfos[k].start = outinfos[k - 1].start;
					 outinfos.erase(outinfos.begin()+k-1);
				 }
			 }
		 }
	 }
	

	 int tdex = 0;
	 for (int k = 0; k < outinfos.size(); ++k)
	 {
		 if (containsDigit(outinfos[k].phoneme))
		 {
			 outinfos[k].newstr = strzis[tdex];
			 tdex++;
		 }
		 else
		 {
			 outinfos[k].newstr = outinfos[k].phoneme;
		 }
	 }
	 OutputDebugStringA("infos....\n");
	 for (auto& data : outinfos) {
		 //std::cout << "Start: " << data.start
		 //	<< " \tEnd: " << data.end
		 //	<< " \tPhoneme: " << data.phoneme << "\n";

		 char szinfo[256];
		 sprintf(szinfo, "%.3f %.3f %s\n", data.start, data.end, data.newstr.c_str());
		 OutputDebugStringA(szinfo);
	 }
	
	m_dlgMfc.voiceStart(m_nPlayLen*1.0/32, outinfos);
	

	//等一段时间再播放
	SetTimer(111, 100, 0);
	//m_dlgMfc.DoReloadMat();



}

void CBezierDlg::OnBnClickedDrive3d()
{
	std::thread t(my_thread, (char*)this);
	t.detach();
}

//保存人脸形变参数
void CBezierDlg::OnBnClickedSavev()
{
	int iSel = m_comboIdens.GetCurSel();
	if (iSel >= 0)
	{
		m_dlgMfc.SaveCharacter(iSel);
	}

}

void CBezierDlg::OnBnClickedButton3d()
{
	m_dlgVideo.ShowWindow(SW_SHOW);

	m_dlgVideo.SetVdoFrameHandler(this);
	m_dlgVideo.OnOpenDevices();

	m_dlgVideo.ShowWindow(SW_SHOW);
	m_dlgVideo.MoveWindow(600, 0,400, 230);
	m_dlgVideo.ShowToFront();
	m_dlgVideo.OnStartCapture();

	m_dlgVideo.SetTopDlgPos();



}


void CBezierDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

	int startP = 0;
	


	if (!m_characerID.IsEmpty() &&(( CButton *)GetDlgItem(IDC_CHECK1))->GetCheck()==false)
	{
		int ivalue[13];
		for (int k = 0; k < 13; ++k)
		{
			CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1+k);
			ivalue[k] = pSlidCtrl->GetPos();
		}
		
		m_dlgMfc.SetCharcterWeight(ivalue, startP);

		
		m_dlgMfc.ChangeIdentityName((char*)m_characerID.GetBuffer(0));


	}
	else
	{
		int ivalue[13];
		for (int k = 0; k < 13; ++k)
		{
			CSliderCtrl *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + k);
			ivalue[k] = pSlidCtrl->GetPos();
		}
		m_dlgMfc.ChangeMotion(ivalue);
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CBezierDlg::OnDestroy()
{
	m_bWorking = false;
	Sleep(200);

	__super::OnDestroy();
	exit(0);
	// TODO: 在此处添加消息处理程序代码
}


void CBezierDlg::OnMove(int x, int y)
{
	__super::OnMove(x, y);

	m_dlgVideo.SetTopDlgPos();
}


void CBezierDlg::OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CBezierDlg::OnNMCustomdrawSlider9(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}




void CBezierDlg::OnCbnSelchangeCombo1()
{
	int iSel = m_comboIdens.GetCurSel();
	if (iSel != -1)
	{
		CString info;
		m_comboIdens.GetLBText(iSel, info);
		m_dlgMfc.ReadCharacter(iSel);
	
		 m_dlgMfc.ChangeIdentityName((char*)info.GetBuffer(0));

		int * fw = m_dlgMfc.GetCharcterWeight();
		for (int k = 0; k < 13; ++k)
		{
			CSliderCtrl* pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + k);
		
			pSlidCtrl->SetPos(fw[k]);
		}
		 m_characerID = info;
	}

}


void CBezierDlg::OnCbnSelchangeCombo3()
{
	int iSel = m_comboTietu.GetCurSel();
	if (iSel != -1)
	{
		CString info;
		m_dlgMfc.SetTieTu(iSel);
	}

}

void CBezierDlg::OnCbnSelchangeCombo4()
{
	int iSel = m_comboToushi.GetCurSel();
	if (iSel != -1)
	{
		CString info;
	
		m_dlgMfc.SetTushi(iSel);
	}

}


void CBezierDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent = 111)
	{
		KillTimer(111);
		StartPlayingFromFile(m_nPlayLen*1.0 / 32 - 300);
		
	}

	__super::OnTimer(nIDEvent);
}

void CBezierDlg::OnBnClickedCheck1()
{
	for (int k = 0; k < 13; ++k)
	{
		CSliderCtrl* pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1 + k);

		pSlidCtrl->SetPos(0);
	}
}

void CBezierDlg::OnBnClickedReloadpic()
{
	m_dlgMfc.DoReloadMat();
}
