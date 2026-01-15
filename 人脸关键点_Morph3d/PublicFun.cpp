#include "stdafx.h"
#include "PublicFun.h"
#include <imagehlp.h>  
#pragma comment(lib,"imagehlp.lib")  

namespace PublicFun
{
	void writeLog(CString strInfo)
	{
		CString strFileName(_T("log.txt"));
		CString strName;

		CreateFileName(strName, strFileName);

		CTime time;
		time = CTime::GetCurrentTime();

		CString strdate = time.Format(_T("%H:%M:%S:"));

		strdate += strInfo;
		strdate += _T("\n");

		CFile file;
		if (!file.Open(strName, CFile::modeCreate | CFile::modeNoTruncate |
			CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone | CFile::modeWrite))
		{
			return;
		}

		file.SeekToEnd();
		WORD unicode = 0xFEFF;  //这句重要
		file.Write(&unicode, 2);
		file.Write(strdate, strdate.GetLength()*2);
		file.Close();
	}

	void CreateFileName(CString &strName,CString &strfilename)
	{
		CTime time = CTime::GetCurrentTime();
		CString cstrPath = PublicFun::GetTempFolderPath();


		strName.Format(_T("%sFaceLog\\%s\\"),
			cstrPath,
			time.Format(_T("%Y%m%d")));
		strName.Replace(_T("\\\\"), _T("\\"));

	

		strName.AppendFormat(strfilename);
		
	}

	void GetPictureName(CString &strName)
	{
		CTime time;
		time = CTime::GetCurrentTime();
		long lTick;
		lTick = GetTickCount();
		srand(lTick);
		int n = rand() % 99;

		CString strtemp;
		strtemp.Format(_T("%s%02d.jpg"), time.Format(_T("Pictrue_%H%M%S")), n);
		CreateFileName(strName, strtemp);
	}

	BOOL IsWow64() 
	{ 
		typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
		LPFN_ISWOW64PROCESS fnIsWow64Process; 
		BOOL bIsWow64 = FALSE; 
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process"); 
		if(NULL != fnIsWow64Process) 
		{ 
			fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
		} 
		return bIsWow64; 
	}

	CString GetSystemPath()
	{
		CString strPath;
		BOOL bIsWow64 = PublicFun::IsWow64();
		if(bIsWow64 == 0)//32位系统
			strPath = L"c:\\windows\\system32\\";
		else	//64位系统
			strPath = L"c:\\windows\\syswow64\\";
		return strPath;
	}

	CString GetTempFolderPath()
	{		
		CString wsTmpPath;
		TCHAR szTempFolder[MAX_PATH];
		GetTempPath(MAX_PATH, szTempFolder);   //得到的是短名
		TCHAR szTmpFolder[MAX_PATH];
		GetLongPathName(szTempFolder, szTmpFolder, MAX_PATH); 
		wsTmpPath = szTempFolder;

		return wsTmpPath;
	}

	CString CharArrayToCString(const char* pCharArray)
	{
		CString cstr; 
#ifdef _UNICODE
		//将char数组转换为wchar_t数组  
		int nCharLen = strlen(pCharArray); //计算pCharArray所指向的字符串大小，以字节为单位，一个汉字占两个字节  
		int nLen = MultiByteToWideChar(CP_ACP, 0, pCharArray, nCharLen, NULL, 0); //计算多字节字符的大小，按字符计算  
		wchar_t* pWChar = new wchar_t[nLen + 1]; //为宽字节字符数申请空间，  
		MultiByteToWideChar(CP_ACP, 0, pCharArray, nCharLen, pWChar, nLen); //多字节编码转换成宽字节编码  
		pWChar[nLen] = '\0';  

		//将wchar_t数组转换为CString 
		cstr.Append(pWChar);

		delete[] pWChar; 
		pWChar = NULL;
#else
		cstr = pCharArray;
#endif
		return cstr;
	}

	//UTF-8到GB2312的转换
	char* U2G(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len+1];
		memset(wstr, 0, len+1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len+1];
		memset(str, 0, len+1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if(wstr)
		{
			delete[] wstr;
			wstr = NULL;
		}
		return str;
	}

	//GB2312到UTF-8的转换
	char* G2U(const char* gb2312)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len+1];
		memset(wstr, 0, len+1);
		MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len+1];
		memset(str, 0, len+1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		if(wstr)
		{
			delete[] wstr;
			wstr = NULL;
		}
		return str;
	}

	unsigned long get_file_size(const char* path)
	{
		unsigned long filesize = -1;
		FILE* fp = NULL;
		fopen_s(&fp, path, "r");
		if(fp == NULL)
			return filesize;
		fseek(fp, 0L, SEEK_END);
		filesize = ftell(fp);
		fclose(fp);
		return filesize;
	}
	void writeErrLog(CString strInfo)
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

		writeLog(strInfo+(LPTSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
	// return true if s1 == s2, case insensitive
	bool EqI(const WCHAR *s1, const WCHAR *s2)
	{
		if (s1 == s2)
			return true;
		if (!s1 || !s2)
			return false;
		return 0 == _wcsicmp(s1, s2);
	}

	bool EndsWithI(const WCHAR *txt, const WCHAR *end)
	{
		if (!txt || !end)
			return false;
		size_t txtLen = wcslen(txt);
		size_t endLen = wcslen(end);
		if (endLen > txtLen)
			return false;
		return EqI(txt + txtLen - endLen, end);
	}
	CString GetLastErrInfo()
	{
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );
		CString csMsg((LPTSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		return csMsg;
	}
}