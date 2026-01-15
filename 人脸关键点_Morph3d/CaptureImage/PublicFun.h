#ifndef _PUBLICFUN_H_
#define _PUBLICFUN_H_
namespace PublicFun
{
	void writeLog( CString strInfo);
	void CreateFileName(CString &strName, CString &strfilename);
	void GetPictureName(CString &strName);
	BOOL IsWow64();
	CString GetSystemPath();
	CString GetTempFolderPath();
	CString CharArrayToCString(const char* pCharArray);
	char* U2G(const char* utf8);
	char* G2U(const char* gb2312);
	unsigned long get_file_size(const char* path);
	void writeErrLog(CString strInfo);
	bool EqI(const WCHAR *s1, const WCHAR *s2);
	bool EndsWithI(const WCHAR *txt, const WCHAR *end);
	CString GetLastErrInfo();
}


#endif

