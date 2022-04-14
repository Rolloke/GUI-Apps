// Win32FindData.h: interface for the CWin32FindData class.
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
class CWin32FindData : public WIN32_FIND_DATA  
{
	// construction / destruction
public:
	CWin32FindData();
	CWin32FindData(const WIN32_FIND_DATA& data);
	virtual ~CWin32FindData();

	// attributes
public:
	inline BOOL	IsDirectory() const;
	inline BOOL	IsCurrentDirectory() const;
	inline BOOL	IsParentDirectory() const;
};
/////////////////////////////////////////////////////////////////////////////
typedef CWin32FindData* CWin32FindDataPtr;
WK_PTR_ARRAY(CWin32FindDatas,CWin32FindDataPtr)
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CWin32FindData::IsCurrentDirectory() const
{
	return (0 == _tcscmp(cFileName,_T(".")));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CWin32FindData::IsParentDirectory() const
{
	return (0 == _tcscmp(cFileName,_T("..")));
}
/////////////////////////////////////////////////////////////////////////////
inline BOOL	CWin32FindData::IsDirectory() const
{
	return dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

#endif // !defined(AFX_WIN32FINDDATA_H__91BF8CD6_82AE_11D2_B50F_004005A19028__INCLUDED_)
