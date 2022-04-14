// FindData.cpp: implementation of the CFindData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindData::CFindData()
{
	ZeroMemory(this,sizeof(WIN32_FIND_DATA));
}
CFindData::CFindData(const WIN32_FIND_DATA& data)
{
    dwFileAttributes = data.dwFileAttributes; 
    ftCreationTime = data.ftCreationTime; 
    ftLastAccessTime = data.ftLastAccessTime; 
    ftLastWriteTime = data.ftLastWriteTime; 
    nFileSizeHigh = data.nFileSizeHigh; 
    nFileSizeLow = data.nFileSizeLow; 
    dwReserved0 = data.dwReserved0; 
    dwReserved1 = data.dwReserved1; 
	_tcscpy(cFileName,data.cFileName);
	_tcscpy(cAlternateFileName,data.cAlternateFileName);
}
CFindData::~CFindData()
{

}
