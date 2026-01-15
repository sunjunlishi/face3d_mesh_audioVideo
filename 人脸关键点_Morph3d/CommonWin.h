#pragma once


#include <afxwin.h>
#include <string>
#include <vector>

int cmm_rand();

CString ShortName(CString name);

//宽窄字节相互转换
std::wstring stringToWstring(const std::string &str);
std::string  wstringToString(std::wstring & wstr);


std::wstring AnsiToUnicode(const std::string &strAnsi);
std::string AnsiToUtf8(const std::string &strAnsi);
std::string Utf8ToAnsi(const std::string &strAnsi);

std::wstring Utf8ToWString(const char* lpcszString);
std::string UnicodeToUtf8(const std::wstring& wstr);
void ContructBih(int nWidth,int nHeight,BITMAPINFOHEADER& bih,int Bit = 24);

void filereadparse(char * filename, std::vector<std::string>& outinfo);


void DrawBmpBuf(BITMAPINFOHEADER& bih,unsigned char* pDataBuf,HWND hShowWnd,int posX = 0,int posY = 0);

void DrawMyText(CString text,int posX,int posY,CDC* pDc,int FontSize,COLORREF textColor);
void DrawBmpBuf(BITMAPINFOHEADER& bih,unsigned char* pDataBuf,HWND hShowWnd,CWnd * pWnd,char * szText,int posX,int posY);



int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
bool DeleteDirectory(LPCTSTR DirName);
//遍历文件夹内容
void TraverseDir(CString dir, std::vector<std::string>& vec);
void TraverseDirShortName(CString dir, std::vector<std::string>& vec);

 CString	GetExePath();

bool IsNumber( LPCTSTR pszText );

void DEBUG_STRING(const char *fmt,...);
bool CreateMultipleDirectory(const CString& szPath);
void GetMacString(std::string& strMac,std::string& strIP);


unsigned long GetFileSize(const char *filename);
HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color,CWnd * pWnd);
//获取文件名和扩展名
CString GetFileTitleFromFileName(CString FileName,CString& ExtendName); 

//画文字
void DrawMyText(CString text,int posX,int posY,CWnd * pWnd,int FontSize = 30,COLORREF textColor = RGB(0,0,0));
void DrawMyTextBK(CString text,int posX,int posY,CWnd * pWnd,COLORREF bkColor ,int FontSize = 30,COLORREF textColor = RGB(0,0,0));

void DrawMyTextType(CString type,CString text,
					int posX,int posY,CWnd * pWnd,int FontSize = 30);


//通过进程名获得进程句柄
HANDLE GetHandleByProcessName(char * name,DWORD ProcessID=0);


void SetIECoreVersion();
bool RaisePrivileges();

