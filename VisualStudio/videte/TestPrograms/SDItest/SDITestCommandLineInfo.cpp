// SDITestCommandLineInfo.cpp: implementation of the CSDITestCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SDITestCommandLineInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSDITestCommandLineInfo::CSDITestCommandLineInfo()
{
	m_bAutoStart = FALSE;
}
//////////////////////////////////////////////////////////////////////
CSDITestCommandLineInfo::~CSDITestCommandLineInfo()
{
}
void CSDITestCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag) {
		if (lstrcmpiW(pszParam, _T("autostart")) == 0) {
			m_bAutoStart = TRUE;
		}
	}
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}
