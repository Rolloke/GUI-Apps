// DirectCDFinalize.cpp: implementation of the CDirectCDFinalize class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WatchCD.h"
#include "DirectCDFinalize.h"
#include "DirectCDStart.h"
#include "DirectCDError.h"
#include "WatchCDDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CDirectCDFinalize* m_pFinalize;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCDFinalize::CDirectCDFinalize(CWatchCDDlg* pWatchCD, 
									 CDirectCDStart* pDCDStart,
									 CDirectCDError* pDCDError)
{

	m_pWatchCD	= pWatchCD;
	m_pDCDError = pDCDError;
//	m_pDCDStart = new CDirectCDStart(pWatchCD, pDCDError);
	m_pDCDStart = pDCDStart;
	m_pFinalize	= this;

	//ID-Checkbox Protect Disc on Finalize dialog
	m_dwIDCheckbox		= 0x00401;

	//ID-Button Finish on Finalize dialog
	m_dwIDFinishButton	= 0x0004;

	m_DCDFinalizeStatus	= FINALIZE_EXEC_DIRECTCD; 
	m_dwTickCount = 0;
	m_hDCDFinalizeDlg		= NULL;
	m_hDCDCollectInfoDlg	= NULL;
	m_hDCDFinalizingDlg		= NULL;
	m_hFinalizeCheckbox		= NULL;
	m_bDeviceLockStatus		= FALSE;
}

CDirectCDFinalize::~CDirectCDFinalize()
{

}



//////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::IsFinalized()
{
	BOOL bRet = FALSE;

	switch(m_DCDFinalizeStatus)
	{
		
	case FINALIZE_EXEC_DIRECTCD:
		OnFinalizeExecDirectCD();
		break;

	case FINALIZE_IS_CDR_READABLE:
		OnFinalizeIsCDRReadable();
		break;

	case FINALIZE_FIND_FINALIZE_DLG:
		OnFinalizeFindFinalizeDlg();
		break;

	case FINALIZE_SET_PROTECT_DISC:
		OnFinalizeSetProtectDisc();
		break;

	case FINALIZE_CLICK_FINISH:
		OnFinalizeClickFinish();
		break;

	case FINALIZE_FIND_COLLECTINFO_DLG:
		OnFinalizeFindCollectInfoDlg();
		break;

	case FINALIZE_FIND_EJECTING_DLG:
		OnFinalizeFindEjectingDlg();
		break;

	case FINALIZE_WAIT_FOR_SUCCESS:
		OnFinalizeWaitForSuccess();
		break;

	case FINALIZE_IS_FINALIZED:
		bRet = TRUE;
		break;

	case FINALIZE_ERROR:
		OnFinalizeError();
		break;

	default:
		break;
	}
	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeExecDirectCD()
{
	if(m_pDCDStart->IsStarted())
	{
		m_dwTickCount = GetTickCount();
		ChangeDCDFinalizeStatus(FINALIZE_IS_CDR_READABLE);
	}
	else
	{
		if(m_pDCDError->IsError())
		{
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeIsCDRReadable()
{

 		if(IsCDRReadable()) //First word in Device field is "DirectCD"
		{
			m_bDeviceLockStatus = TRUE;
			if(m_pDCDStart->OpenNextDlg())
			{
				m_dwTickCount = GetTickCount();
				ChangeDCDFinalizeStatus(FINALIZE_FIND_FINALIZE_DLG);
			}
		}
		else
		{
			if(GetTimeSpan(m_dwTickCount) > 15*1000)
			{
				ChangeDCDFinalizeStatus(FINALIZE_ERROR);
				m_pDCDError->ChangeErrorStatus(ERROR_CDR_NOT_READY_FOR_FINALIZE);
			}
		}

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::IsCDRReadable()
{
	Sleep(1000);
	BOOL bRet = FALSE;
	HWND hWnd = m_pDCDStart->GetHdlDeviceField();
	CString sText;

	CString sLoc = _T("DirectCD.loc");
	if (hWnd && IsWindow(hWnd))
	{
		GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();

		WK_TRACE(_T("CDirectCDFinalize::IsCDRReadable(): WindowText: %s  m_sDirectCD: %s\n"), sText, m_sDirectCD); 
	
		if(m_pWatchCD->FileExists(m_pWatchCD->GetPathToDirectCD(), sLoc))
		{
			m_sDirectCD = m_pWatchCD->GetStringFromTable(m_pWatchCD->GetPathToDirectCD(),
											_T("directcd.loc"), 404);
		}
		else
		{
			m_sDirectCD = m_pWatchCD->GetStringFromTable(m_pWatchCD->GetPathToDirectCD(),
											_T("directcd.exe"), 404);
		}

		if(sText.Left(15) == m_sDirectCD.Left(15))
		{
			WK_TRACE(_T("CDirectCDFinalize::IsCDRReadable(): CDR is readable, because Text= %s\n"), sText.Left(15));
			bRet = TRUE;
		}
	}

	return bRet;
}
//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeFindFinalizeDlg()
{
	if(FindFinalizeDlg())
	{
		ChangeDCDFinalizeStatus(FINALIZE_SET_PROTECT_DISC);
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_SET_PROTECT_DISC);
		}
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeSetProtectDisc()
{
	LPARAM lParam = 0;
	if(!EnumChildWindows(m_hDCDFinalizeDlg, (WNDENUMPROC)EnumChildProc, lParam))
	{
		//click checkbox to protect disc
		::PostMessage(m_hFinalizeCheckbox, BM_SETCHECK, 1, 0);
	}

	m_dwTickCount = GetTickCount();
	ChangeDCDFinalizeStatus(FINALIZE_CLICK_FINISH);
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeClickFinish()
{
	//get one child window of Finalize Window to access every child window
	//on Finalize window. It`s not nessessary to use EnumChildWindows !!!!!

	HWND hFirstChild = ::GetWindow(m_hDCDFinalizeDlg, GW_CHILD);
	//click finish-button
	
	if(m_pDCDStart->GetIDFinalizeFinishButton() != 0xFFFF)
	{
		::PostMessage(m_hDCDFinalizeDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_pDCDStart->GetIDFinalizeFinishButton(),BN_CLICKED),
					(LPARAM)hFirstChild);
		m_dwTickCount = GetTickCount();
		ChangeDCDFinalizeStatus(FINALIZE_FIND_COLLECTINFO_DLG);
	}
	else
	{
		WK_TRACE_ERROR(_T("OnFinalizeClickFinish(): Couldn`t click finish button.\n"));
		::PostMessage(m_hDCDFinalizeDlg, WM_CLOSE, 0, 0);
		ChangeDCDFinalizeStatus(FINALIZE_ERROR);
		m_pDCDError->ChangeErrorStatus(ERROR_FINISH_BUTTON_NOT_FOUND);
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeFindCollectInfoDlg()
{
	if(FindCollectInfoDlg())
	{
		m_dwTickCount = GetTickCount();
		ChangeDCDFinalizeStatus(FINALIZE_FIND_EJECTING_DLG);
//		ChangeDCDFinalizeStatus(FINALIZE_WAIT_FOR_SUCCESS);
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_FINALZE_PROCESS_NOT_STARTED);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeFindEjectingDlg()
{
	//wait until the string "Ejecting disc..." pops up in the Collect Info Dlg 
	//it is the same dlg, smae handle, but with a different string
	//only if "Ejecting disc.." was there, DirectCD UNLOCKS the backupdrive
	//if I close DirectCD before "Ejecting disc...", the CD-R cannot be removed from 
	//backupdrive !!

	Sleep(50);
	TCHAR sText[128];

	::GetDlgItemText(m_hDCDCollectInfoDlg, 0x0403, sText, sizeof(sText));
	CString sEject(sText);
	if(sEject == m_pDCDStart->GetEjectingString())
	{
		WK_TRACE(_T("Ejecting gefunden !!\n"));
		ChangeDCDFinalizeStatus(FINALIZE_WAIT_FOR_SUCCESS);
	}
}
	

//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeWaitForSuccess()
{
	//wait until all Finalize Dialog closed automaticly from DirectCD
	if(!IsWindow(m_hDCDCollectInfoDlg)) 
	{
		ChangeDCDFinalizeStatus(FINALIZE_IS_FINALIZED);
	}

	if(GetTimeSpan(m_dwTickCount) > 5*60*1000)
	{
		WK_TRACE(_T("Finalisieren hat länger als 5 minuten gedauert-> Fehler\n"));
	
		ChangeDCDFinalizeStatus(FINALIZE_ERROR);
		m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_FINALIZING);
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::OnFinalizeError()
{
	m_pDCDError->SetError(TRUE);
	CloseFinalizeDialogs();
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::CloseFinalizeDialogs()
{
	HWND hDriveInfoDlg = m_pDCDStart->GetHdlDriveInfoDlg();
	if(IsWindow(hDriveInfoDlg))
	{
		::PostMessage(hDriveInfoDlg, WM_CLOSE, 0,0); 
	}
}

							 
//////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::IsDeciveLocked()
{
	return m_bDeviceLockStatus;
}
////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::FindFinalizeDlg()
{
	BOOL bRet = FALSE;
	//get handle from DirectCD Finalize Dialog
	m_hDCDFinalizeDlg = FindWindow(_T("#32770"), m_pWatchCD->GetDCDWndTitleWithDrive());
	if(IsWindow(m_hDCDFinalizeDlg))
	{
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Finalize Dialog not opened
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_FINALIZE_DLG);
		}
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::FindCollectInfoDlg()
{
	BOOL bRet = FALSE;
	//get handle from DirectCD Collect Info Dialog
	HWND hWnd = ::FindWindow(_T("#32770"), m_pWatchCD->GetDCDWndTitleWithDrive());
	if((hWnd != m_hDCDFinalizeDlg) && IsWindow(hWnd))
	{
		m_hDCDCollectInfoDlg = hWnd;
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Collect Info Dialog not opened
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_COLLECT_INFO_DLG);
		}
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFinalize::FindFinalizingDlg()
{
	BOOL bRet = FALSE;
	//get handle from DirectCD Finalizing Dialog
	HWND hWnd = ::FindWindow(_T("#32770"), m_pWatchCD->GetDCDWndTitleWithDrive());
	if(    (hWnd != m_hDCDFinalizeDlg) 
		&& (hWnd != m_hDCDCollectInfoDlg)
		&& (IsWindow(hWnd)))
	{
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Finalizing Dialog not opened
			ChangeDCDFinalizeStatus(FINALIZE_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_FINALIZING_DLG);
		}
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
void CDirectCDFinalize::ChangeDCDFinalizeStatus(DirectCDStatus DCDFinalizeStatus)
{
	DirectCDStatus LastDCDFinalizeStatus = m_DCDFinalizeStatus;
	m_DCDFinalizeStatus = DCDFinalizeStatus;

	WK_TRACE(_T("CDirectCDFinalize() ChangeDCDFinalizeStatus: from last %s to new %s\n")
				,NameOfEnumDirectCD(LastDCDFinalizeStatus)
				,NameOfEnumDirectCD(m_DCDFinalizeStatus));

}

//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDirectCDFinalize::EnumChildProc(HWND hWnd, LPARAM lParam)
{
	BOOL bRet = TRUE;

	switch(lParam)
	{
	case 0:
		{
		DWORD dwCheckbox = 0x0401;
		//find checkbox for protect disc in finalize dialog
		if((DWORD)::GetDlgCtrlID(hWnd) == dwCheckbox)
		{
			m_pFinalize->m_hFinalizeCheckbox = hWnd;
			bRet = FALSE;
		}
		break;
		}

	default:
		break;
	}

	return bRet;

}
