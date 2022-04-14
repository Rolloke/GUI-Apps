// DirectCD.cpp: implementation of the CDirectCD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "DirectCD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCD::CDirectCD()
{
	m_DCDErrorStatus = DCD_NO_ERROR;
}

CDirectCD::~CDirectCD()
{

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCD::ExecuteWatchCD(CString sDrive)
{
	//check first, if WatchCD is running. If true, close it
	HWND hWatchCD = ::FindWindow(NULL, _T("WatchCD"));
	if(IsWindow(hWatchCD))
	{
		::PostMessage(hWatchCD, WM_CLOSE, 0, 0);
	}

	BOOL bRet = FALSE;
	BOOL bProcess;

	STARTUPINFO			startUpInfo;
	PROCESS_INFORMATION prozessInformation;
	CString sCommandLine;
	
	startUpInfo.cb				= sizeof(STARTUPINFO);
	startUpInfo.lpReserved      = NULL;
	startUpInfo.lpDesktop       = NULL;
	startUpInfo.lpTitle			= NULL;
	startUpInfo.dwX				= 0;
	startUpInfo.dwY				= 0;
	startUpInfo.dwXSize			= 0;
	startUpInfo.dwYSize			= 0;
	startUpInfo.dwXCountChars   = 0;
	startUpInfo.dwYCountChars   = 0;
	startUpInfo.dwFillAttribute = 0;
	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
	startUpInfo.wShowWindow     = SW_HIDE;
	startUpInfo.cbReserved2     = 0;
	startUpInfo.lpReserved2     = NULL;

	CString sExe = theApp.GetHomeDir() + _T("\\WatchCD.exe");

//TODO TKR nur zum Debuggen von WatchCD
//#ifdef _DEBUG
//	sExe = _T("E:\\Project\\DV\\WatchCD\\Debug\\WatchCD.exe");
//#endif

	if(!sDrive.IsEmpty())
	{
		sCommandLine = sExe + _T(" ")+sDrive;
	}
	else
	{
		sCommandLine = sExe;
	}

	bProcess = CreateProcess(
				(LPCTSTR)sExe,				// pointer to name of executable module 
				(LPTSTR)sCommandLine.GetBuffer(0),		// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);
	sCommandLine.ReleaseBuffer();	

	if(bProcess)
	{
		bRet = TRUE;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////
CString CDirectCD::GetErrorString()
{
	switch(m_DCDErrorStatus)
	{
	case DCD_NO_ERROR:		
		return _T("No Error\n");
	case DCD_ERROR_START_WATCHCD:
		return _T("Couldn`t start WatchCD.exe\n");
	case DCD_ERROR_WAIT_FOR_EMPTY_CDR:
		return _T("No answer from WatchCD.exe after timeout of 60 sec.\n");
		break;
	case DCD_ERROR_FORMAT:
		return _T("Timeout format CDR reached\n");
	case DCD_ERROR_CANT_FIND_WATCHCD:
		return _T("Cannot find WatchCD on desktop\n.");
	case DCD_ERROR_WAIT_FOR_FINALIZE_SUCCESS:
		return _T("Wait for finalize success\n");

	default:
		return _T("Caseless call, but No Error\n");
	}
}

//////////////////////////////////////////////////////////////////////
/*
void CDirectCD::ChangeWCDStatus(WatchCDStatus newStatus)
{
	WatchCDStatus LastWCDStatus = m_WCDStatus;
	m_WCDStatus = newStatus;

	WK_TRACE(_T("CDirectCD() WatchCDStatus: from last %s to %s\n")
			,NameOfEnumWCDStatus(LastWCDStatus)
			,NameOfEnumWCDStatus(newStatus));
}
*/
//////////////////////////////////////////////////////////////////////
/*
CString CDirectCD::NameOfEnumWCDStatus(WatchCDStatus Status)
{
	switch(Status)
	{
	case WCD_EXEC_WATCHCD:				return _T("WCD_EXEC_WATCHCD");
	case WCD_WAIT_FOR_FORMAT_SUCCESS:	return _T("WCD_WAIT_FOR_FORMAT_SUCCESS");

	default:					return _T("unknown WCDStatus");

	}
}
*/
//////////////////////////////////////////////////////////////////////
void CDirectCD::ChangeDCDErrorStatus(DCDErrorStatus newStatus)
{
	DCDErrorStatus LastDCDErrorStatus = m_DCDErrorStatus;
	m_DCDErrorStatus = newStatus;

	WK_TRACE(_T("CDirectCD() DCDErrorStatus: from last %s to %s\n")
			,NameOfEnumDCDErrorStatus(LastDCDErrorStatus)
			,NameOfEnumDCDErrorStatus(newStatus));
}

//////////////////////////////////////////////////////////////////////
CString CDirectCD::NameOfEnumDCDErrorStatus(DCDErrorStatus Status)
{
	switch(Status)
	{
	case DCD_NO_ERROR:					return _T("DCD_NO_ERROR");
	case DCD_ERROR_START_WATCHCD:		return _T("DCD_ERROR_START_WATCHCD");
	case DCD_ERROR_WAIT_FOR_EMPTY_CDR:	return _T("DCD_ERROR_WAIT_FOR_EMPTY_CDR");
	case DCD_ERROR_FORMAT:				return _T("DCD_ERROR_FORMAT");

	default:					return _T("unknown DCDErrorStatus");

	}
}
