// Win32FindData.h: interface for the CFindData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIN32FINDDATA_H__91BF8CD6_82AE_11D2_B50F_004005A19028__INCLUDED_)
#define AFX_WIN32FINDDATA_H__91BF8CD6_82AE_11D2_B50F_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
typedef struct _WIN32_FIND_DATA { // wfd 
    DWORD dwFileAttributes; 
    FILETIME ftCreationTime; 
    FILETIME ftLastAccessTime; 
    FILETIME ftLastWriteTime; 
    DWORD    nFileSizeHigh; 
    DWORD    nFileSizeLow; 
    DWORD    dwReserved0; 
    DWORD    dwReserved1; 
    TCHAR    cFileName[ MAX_PATH ]; 
    TCHAR    cAlternateFileName[ 14 ]; 
} WIN32_FIND_DATA; 
 
*/
class CFindData : public WIN32_FIND_DATA  
{
	// construction / destruction
public:
	CFindData();
	CFindData(const WIN32_FIND_DATA& data);
	virtual ~CFindData();

	// attributes
public:
	inline BOOL	IsDirectory() const;
	inline BOOL	IsCurrentDirectory() const;
	inline BOOL	IsParentDirectory() const;

	inline CSystemTime GetCreationTime() const;
	inline CSystemTime GetLastAccessTime() const;
	inline CSystemTime GetLastWriteTime() const;
};
/////////////////////////////////////////////////////////////////////////////
typedef CFindData* CFindDataPtr;
WK_PTR_ARRAY(CFindDatas,CFindDataPtr);
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CFindData::IsCurrentDirectory() const
{
	return 0==_tcscmp(cFileName,_T("."));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CFindData::IsParentDirectory() const
{
	return 0==_tcscmp(cFileName,_T(".."));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CFindData::IsDirectory() const
{
	return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}
/////////////////////////////////////////////////////////////////////////////
inline CSystemTime CFindData::GetCreationTime() const
{
	CSystemTime st;
	FILETIME ft;
	FileTimeToLocalFileTime(&ftCreationTime,&ft);
	FileTimeToSystemTime(&ft,&st);
	return st;
}
/////////////////////////////////////////////////////////////////////////////
inline CSystemTime CFindData::GetLastAccessTime() const
{
	CSystemTime st;
	FILETIME ft;
	FileTimeToLocalFileTime(&ftLastAccessTime,&ft);
	FileTimeToSystemTime(&ft,&st);
	return st;
}
/////////////////////////////////////////////////////////////////////////////
inline CSystemTime CFindData::GetLastWriteTime() const
{
	CSystemTime st;
	FILETIME ft;
	FileTimeToLocalFileTime(&ftLastWriteTime,&ft);
	FileTimeToSystemTime(&ft,&st);
	return st;
}

#endif // !defined(AFX_WIN32FINDDATA_H__91BF8CD6_82AE_11D2_B50F_004005A19028__INCLUDED_)
