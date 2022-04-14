// DirectCDFormat.cpp: implementation of the CDirectCDFormat class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WatchCD.h"
#include "DirectCDFormat.h"
#include "WatchCDDlg.h"
#include "SystemTime.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CDirectCDFormat* m_pThis;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectCDFormat::CDirectCDFormat(CWatchCDDlg* pWatchCD, CDirectCDError* pDCDError)
{
	m_pThis					= this;
	m_pWatchCD				= pWatchCD;
	m_pDCDError				= pDCDError;
	m_DCDFormatStatus		= FORMAT_FIND_DLG_FORMAT_DISC;
	m_dwTickCount			= GetTickCount();
	m_hDCDFormatDiscDlg		= NULL;
	m_hDCDNameYourDiscDlg	= NULL;
	m_hDCDFormattingDlg		= NULL;
	m_hNextButton			= NULL;
	m_hEditField			= NULL;
	m_hFinishButton			= NULL;

	//ID next-button on Format Disc dialog
	m_dwIDButtonNext = 0x045E;

	//ID edit-field on Name-Your-Disc-dialog for new CD-R name
	m_dwIDEditField = 0x0458;

	//ID finish-button in Name Your Disc Dialog
	m_dwIDButtonFinish = 0x0407;
}

CDirectCDFormat::~CDirectCDFormat()
{

}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::IsFormatted()
{
	BOOL bRet = FALSE;
	Sleep(20);
	switch(m_DCDFormatStatus)
	{

	case FORMAT_FIND_DLG_FORMAT_DISC:
		OnFormatFindDlgFormatDisc();
		break;

	case FORMAT_FIND_DLG_NAME_YOUR_DISC:
		OnFormatFindDlgNameYourDisc();
		break;

	case FORMAT_SET_NEW_CDR_NAME:
		OnFormatSetNewCDRName();
		break;

	case FORMAT_FIND_DLG_FORMATTING_DISC:
		OnFormatFindDlgFormattingDisc();
		break;
		
	case FORMAT_WAIT_FOR_SUCCESS:
		OnFormatWaitForSuccess();
		break;

	case FORMAT_IS_FORMATTED:
		bRet = TRUE;
		break;

	case FORMAT_ERROR:
		OnFormatError();
		break;

	default:
		break;
	}



	return bRet;

}


//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatFindDlgFormatDisc()
{
	
	if(FindFormatDiscDlg())
	{
		//find next-button
		LPARAM lParam = 0;
		if(!EnumChildWindows(m_hDCDFormatDiscDlg, (WNDENUMPROC)EnumChildProc, lParam))
		{
			//click button "&Next >" in Welcome-Dialog
			::PostMessage(m_hDCDFormatDiscDlg,WM_COMMAND,
					(WPARAM)MAKELONG(m_dwIDButtonNext,BN_CLICKED),
					(LPARAM)m_hNextButton);	

			m_dwTickCount = GetTickCount();
			ChangeDCDFormatStatus(FORMAT_FIND_DLG_NAME_YOUR_DISC);
		}
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatFindDlgNameYourDisc()
{
	if(FindNameYourDiscDlg())
	{
		//find edit-field for new CD-R name
		LPARAM lParam = 1;
		if(!EnumChildWindows(m_hDCDNameYourDiscDlg, (WNDENUMPROC)EnumChildProc, lParam))
		{
			ChangeDCDFormatStatus(FORMAT_SET_NEW_CDR_NAME);
		}
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatSetNewCDRName()
{
	//set new CD-R name
	if(SetNewCDRName())
	{
		//click button "&Finish" in Name Your Disc Dialog
		::PostMessage(m_hDCDNameYourDiscDlg,WM_COMMAND,
				(WPARAM)MAKELONG(m_dwIDButtonFinish,BN_CLICKED),
				(LPARAM)m_hFinishButton);	

		m_dwTickCount = GetTickCount();
		ChangeDCDFormatStatus(FORMAT_FIND_DLG_FORMATTING_DISC);
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::SetNewCDRName()
{
	BOOL bRet = FALSE;

	CSystemTime sysTime;
	CString sDateTime;
	sysTime.GetLocalTime();
	WORD wYear = sysTime.GetYear();
	CString sYear;
	sYear.Format(_T("%4d"), wYear);
	sYear = sYear.Right(2);

	//sDateTime maximal 11 zeichen lang
	sDateTime.Format(_T("%02d%02d%s_%02d%02d"),
			sysTime.GetDay(),
			sysTime.GetMonth(),
			//sysTime.GetYear(),
			sYear,
			sysTime.GetHour(),
			sysTime.GetMinute(),
			sysTime.GetSecond()
			);

	if(::SendMessage(m_hEditField,WM_SETTEXT, 0, (LPARAM)(LPCTSTR)sDateTime))
	{
		bRet = TRUE;
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatFindDlgFormattingDisc()
{
	if(FindFormattingDlg())
	{
		m_dwTickCount = GetTickCount();
		ChangeDCDFormatStatus(FORMAT_WAIT_FOR_SUCCESS);
	}
}


//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatWaitForSuccess()
{
	if(AllFormatDlgsClosed())
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			WK_TRACE(_T("OnFormatWaitForSuccess(): all format dlg`s closed after min. %i sec\n"), 
														GetTimeSpan(m_dwTickCount));
			ChangeDCDFormatStatus(FORMAT_IS_FORMATTED);
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::OnFormatError()
{
	m_pDCDError->SetError(TRUE);
	CloseFormatDialogs();
}

////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::FindFormatDiscDlg()
{
	BOOL bRet = FALSE;
	//get handle from DirectCD Format Disc Dialog
	CString sTitle = m_pWatchCD->GetDCDWndTitleWithDrive();
	m_hDCDFormatDiscDlg = FindWindow(_T("#32770"), sTitle);
	if(m_hDCDFormatDiscDlg && IsWindow(m_hDCDFormatDiscDlg))
	{
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Format Disc Dialog not opened
			ChangeDCDFormatStatus(FORMAT_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_FORMAT_DISC_DLG);
		}
		WK_TRACE(_T("FindFormatDiscDlg(): %s Zeit: %i\n"), 
			sTitle,GetTimeSpan(m_dwTickCount));
	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::FindNameYourDiscDlg()
{
	BOOL bRet = FALSE;
	LPARAM lParam = 0;
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);

	if(IsWindow(m_hDCDNameYourDiscDlg))
	{
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Drive Info Dialog not opened
			ChangeDCDFormatStatus(FORMAT_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_NAME_YOUR_DISC_DLG);
		}
	}

	return bRet;
}


////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::FindFormattingDlg()
{
	BOOL bRet = FALSE;
	LPARAM lParam = 1;
	EnumDesktopWindows(NULL, (WNDENUMPROC)EnumWindowsProc, lParam);
	if(IsWindow(m_hDCDFormattingDlg))
	{
		//msg to DVClient that formatting process started
		HWND hDVClient = m_pWatchCD->GetHandleDVClient();
		::PostMessage(hDVClient, WM_CHECK_BACKUP_STATUS, BS_DCD_FORMATTING, 0);
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 30*1000)
		{
			//Formatting Dialog not opened
			ChangeDCDFormatStatus(FORMAT_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_FORMATTING_DLG);
		}

	}
	return bRet;
}

////////////////////////////////////////////////////////////////////////////
BOOL CDirectCDFormat::AllFormatDlgsClosed()
{
	BOOL bRet = FALSE;
	//wait until all dialogs of DirectCD are closed automaticly from DirectCD
	if(    (!IsWindow(m_hDCDFormatDiscDlg))
		&& (!IsWindow(m_hDCDNameYourDiscDlg))
		&& (!IsWindow(m_hDCDFormattingDlg)))
	{
		bRet = TRUE;
	}
	else
	{
		if(GetTimeSpan(m_dwTickCount) > 120*1000)
		{
			ChangeDCDFormatStatus(FORMAT_ERROR);
			m_pDCDError->ChangeErrorStatus(ERROR_TIMEOUT_WAIT_FOR_FORMAT_IS_FORMATTED);
		}
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDirectCDFormat::EnumChildProc(HWND hWnd, LPARAM lParam)
{
	BOOL bRet = TRUE;

	switch(lParam)
	{
	case 0:
		//find next-button on Format-Disc-dialog
		if ((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwIDButtonNext)
		{
			m_pThis->m_hNextButton = hWnd; 
			bRet = FALSE;
		}
		break;

	case 1:
		//find edit field for new CD-R name on Name Your Disc Dialog
		if((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwIDEditField)
		{
			m_pThis->m_hEditField = hWnd;
			bRet = FALSE;
		}
		break;

	case 2:
		//find finish-button on Name Your Disc Dialog
		if((DWORD)::GetDlgCtrlID(hWnd) == m_pThis->m_dwIDButtonFinish)
		{
			m_pThis->m_hFinishButton = hWnd;
			bRet = FALSE;
		}
		break;

	default:
		break;
	}

	return bRet;

}

//////////////////////////////////////////////////////////////////////
void CDirectCDFormat::CloseFormatDialogs()
{
	if(IsWindow(m_hDCDFormatDiscDlg))
	{
		::PostMessage(m_hDCDFormatDiscDlg, WM_CLOSE, 0,0); 
	}

	if(IsWindow(m_hDCDNameYourDiscDlg))
	{
		::PostMessage(m_hDCDNameYourDiscDlg, WM_CLOSE, 0,0); 
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDirectCDFormat::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{

	BOOL bRet = TRUE;
	CString sText;
	if (hWnd && IsWindow(hWnd))
	{
		GetWindowText(hWnd,sText.GetBuffer(_MAX_PATH),_MAX_PATH);
		sText.ReleaseBuffer();
		switch(lParam)
		{
		case 0:
			//find Name Your Disc dialog
			if(    (hWnd != m_pThis->m_hDCDFormatDiscDlg)
				&& (sText == m_pThis->m_pWatchCD->GetDCDWndTitleWithDrive()))
			{
				m_pThis->m_hDCDNameYourDiscDlg = hWnd;
				bRet = FALSE;
			}
			break;

		case 1:
			//find Formatting dialog
			if(    (hWnd != m_pThis->m_hDCDFormatDiscDlg)
				&& (hWnd != m_pThis->m_hDCDNameYourDiscDlg)
				&& (sText == m_pThis->m_pWatchCD->GetDCDWndTitleWithDrive()))
			{
				m_pThis->m_hDCDFormattingDlg = hWnd;
				bRet = FALSE;
			}
			break;

		default:
			break;
		
		}
	}

	return bRet;
}
////////////////////////////////////////////////////////////////////////////
void CDirectCDFormat::ChangeDCDFormatStatus(DirectCDStatus DCDFormatStatus)
{
	DirectCDStatus LastDCDFormatStatus = m_DCDFormatStatus;
	m_DCDFormatStatus = DCDFormatStatus;

	WK_TRACE(_T("CDirectCDFormat() DCDFormatStatus: from last %s to new %s\n")
			,NameOfEnumDirectCD(LastDCDFormatStatus)
			,NameOfEnumDirectCD(m_DCDFormatStatus));

}
