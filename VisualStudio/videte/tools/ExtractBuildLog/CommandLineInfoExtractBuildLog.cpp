// CommandLineInfoExtractBuildLog.cpp: implementation of the CCommandLineInfoExtractBuildLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExtractBuildLog.h"
#include "CommandLineInfoExtractBuildLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCommandLineInfoExtractBuildLog::CCommandLineInfoExtractBuildLog()
{
}
//////////////////////////////////////////////////////////////////////
CCommandLineInfoExtractBuildLog::~CCommandLineInfoExtractBuildLog()
{
}
//////////////////////////////////////////////////////////////////////
void CCommandLineInfoExtractBuildLog::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL /*bLast*/)
{
	static int iIndex =0;
	CString sParam = lpszParam;
	if (!bFlag)
	{
/*		CString sMsg;
		sMsg.Format("%i %s\n", iIndex, sParam);
		AfxMessageBox(sMsg);
		TRACE(sMsg);
*/		if (iIndex == 0)
		{
			theApp.m_eFileType = FT_BUILDLOG;
			theApp.m_sWorkspaceDir = sParam;
		}
		else if (iIndex == 1)
		{
			theApp.m_sWorkspaceName = sParam;
		}

		iIndex++;
	}
}
