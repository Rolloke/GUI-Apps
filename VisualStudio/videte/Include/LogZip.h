#ifndef _CLogZip_H
#define _CLogZip_H

#ifdef _UNICODE

#ifndef wchar_t
 typedef unsigned short wchar_t;
#endif

class CLogZip
{
public:
	// iCompressFactor 1(fast)..9(slow)
	static int CompressLogFile(const wchar_t * szFileName, int iCompressFactor=1);
	static int DecompressLogFile(const wchar_t * szFileName);

	static int CompressFile(const wchar_t * source, const wchar_t * dest, int iFactor=1);
	static int DecompressFile(const wchar_t * source, const wchar_t * dest);
};

#else // !_UNICODE

class CLogZip
{
public:
	// iCompressFactor 1(fast)..9(slow)
	static int CompressLogFile(const char * szFileName, int iCompressFactor=1);
	static int DecompressLogFile(const char * szFileName);

	static int CompressFile(const char * source, const char * dest, int iFactor=1);
	static int DecompressFile(const char * source, const char * dest);
};
#endif // _UNICODE

#endif // _CLogZip_H
