// IPCDataCarrierClient.cpp: implementation of the CIPCDataCarrierClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DCClient.h"
#include "DCClientDlg.h"
#include "IPCDataCarrierClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDataCarrierClient::CIPCDataCarrierClient(LPCTSTR shmName)
: CIPCDataCarrier(shmName, FALSE)
{
	StartThread();
	m_iBurnCounts = 0;
	m_dwTick = GetTickCount();
}
//////////////////////////////////////////////////////////////////////
CIPCDataCarrierClient::~CIPCDataCarrierClient()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmConnection()
{
	theApp.GetDCClientDlg()->AppendMessage(_T("CIPC connection established\n"));
	CIPCDataCarrier::OnConfirmConnection();
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnRequestDisconnect()
{
	theApp.GetDCClientDlg()->DoDisconnect();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmVolumeInfos(int iNumDrives, const CIPCDrive drives[])
{
	//this function is called only when a CD/DVD drive exists physically in the system
	//if not, OnIndicateError is called 
	
	CString sMsg;
	//found medium in CD/DVD drive
	if(iNumDrives == 1)
	{	
		m_sDriveLetter = drives[0].GetRootString();
		CString s;
		s.Format(_T("Drive root: %s\n"), m_sDriveLetter);
		theApp.GetDCClientDlg()->AppendMessage(s);

		DWORD dwType = drives[0].GetType();
		DWORD dwTotalFreeMB = drives[0].GetMB();
		DWORD dwTotalUsedMB = drives[0].GetUsedMB();
		DWORD dwTotalMB = dwTotalFreeMB + dwTotalUsedMB;
		DWORD dwUsedMB = drives[0].GetUsedBytes().GetInMB();
		
		CString sMsg;
		sMsg.Format(_T("TotalFreeBytes: %d \n"), dwTotalFreeMB);
		theApp.GetDCClientDlg()->AppendMessage(sMsg);

		sMsg.Format(_T("TotalUsedBytes: %d \n"), dwTotalUsedMB);
		theApp.GetDCClientDlg()->AppendMessage(sMsg);

		sMsg.Format(_T("TotalBytes: %d\n"), dwUsedMB);
		theApp.GetDCClientDlg()->AppendMessage(sMsg);		
		
		if(dwTotalFreeMB == 0)
		{
			theApp.GetDCClientDlg()->AppendMessage(_T("Error: no writable medium available\n"));
		}
		else
		{
			if(dwUsedMB == 0)
			{
				theApp.GetDCClientDlg()->AppendMessage(_T("Writable medium, burn process started\n"));
//				theApp.GetDCClientDlg()->StartBurning();
			}
			else
			{
				if(dwType == DRIVE_CD_RW)
				{
					BOOL bOK = MessageBox(theApp.m_pDlg->m_hWnd, _T("Erase CD-RW ?"), _T("Writable medium: CD-RW"), MB_OKCANCEL | MB_ICONQUESTION);
					if(bOK == IDOK)
					{
						theApp.GetDCClientDlg()->AppendMessage(_T("Please wait, erasing CD-RW...\n"));
						theApp.GetDCClientDlg()->StartBurning();
					}
					else
					{
						theApp.GetDCClientDlg()->AppendMessage(_T("Burn process canceled.\n"));
					}
				}
				else
				{
					theApp.GetDCClientDlg()->AppendMessage(_T("Error: Drive root: no writable medium available\n"));
				}

			}
		}
	}
	else
	{	//no medium was found in CD/DVD drive 
		theApp.GetDCClientDlg()->AppendMessage(_T("Error: Drive root: no medium available\n"));
	}

}

//////////////////////////////////////////////////////////////////////
CStringArray& CIPCDataCarrierClient::GetPathNames()
{
	return m_saPathNames;
}

//////////////////////////////////////////////////////////////////////
CString CIPCDataCarrierClient::GetDriveLetter()
{
	return m_sDriveLetter;
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmSession(DWORD dwSessionID)
{
	theApp.GetDCClientDlg()->AppendMessage(_T("Session finished\n"));
	
/*	DWORD dwTime = GetTimeSpan(m_dwTick);
	CString sTimeFormat;
	sTimeFormat.Format(_T("%02d:%02d:%02d"),dwTime/3600,
					 (dwTime/60)%60,
					 dwTime%60);

	m_iBurnCounts++;
	CString sCounts;
	sCounts.Format(_T("%d. Brennvorgang OK\n"), m_iBurnCounts);
	theApp.GetDCClientDlg()->AppendMessage(sCounts);

	Sleep(1000);

	m_dwTick = GetTickCount();
	theApp.GetDCClientDlg()->StartBurning();
*/

}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	theApp.GetDCClientDlg()->SetProgress(iProgress);
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnIndicateError(DWORD dwCmd, CSecID id, 
										    CIPCError errorCode, int iUnitCode,
											const CString &sErrorMsg)
{

	if(errorCode == CIPC_ERROR_DATA_CARRIER)
	{
		switch(dwCmd)
		{
		
		case CIPC_DC_REQUEST_VOLUME_INFOS:
		case CIPC_DC_CONFIRM_VOLUME_INFOS:
			TRACE(_T("unitCode: %d\n"), iUnitCode);
			theApp.GetDCClientDlg()->AppendMessage(sErrorMsg);
			break;

		case CIPC_DC_REQUEST_SESSION:
		case CIPC_DC_CONFIRM_SESSION:
			TRACE(_T("unitCode: %d\n"), iUnitCode);
			theApp.GetDCClientDlg()->AppendMessage(sErrorMsg);
			break;

		case CIPC_DC_INDICATE_SESSION_PROGRESS:
			break;

		case CIPC_DC_REQUEST_CANCEL_SESSION:
		case CIPC_DC_CONFIRM_CANCEL_SESSION:
			TRACE(_T("unitCode: %d\n"), iUnitCode);
			theApp.GetDCClientDlg()->AppendMessage(sErrorMsg);
			break;

		case CIPC_DC_REQUEST_DELETE_VOLUME:
		case CIPC_DC_CONFIRM_DELETE_VOLUME:

			break;

		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierClient::OnConfirmCancelSession(DWORD dwSessionID)
{
	CString sMsg = _T("user abort, session canceled.");
	theApp.GetDCClientDlg()->AppendMessage(sMsg);
}
