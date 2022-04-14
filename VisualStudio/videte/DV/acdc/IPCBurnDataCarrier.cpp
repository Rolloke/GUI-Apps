// IPCBurnDataCarrier.cpp: implementation of the CIPCBurnDataCarrier class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "acdc.h"

#include "IPCBurnDataCarrier.h"
#include "NeroThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCBurnDataCarrier::CIPCBurnDataCarrier(LPCTSTR shmName)
: CIPCDataCarrier(shmName,TRUE)
{
	StartThread();
	m_dwSessionID = 0;
}

CIPCBurnDataCarrier::~CIPCBurnDataCarrier()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestDisconnect()
{
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestVolumeInfos()
{
	TRACE(_T("OnRequestVolumeInfos\n"));
	CAcdcDlg* pDlg = theApp.GetAcdcDlg();

	if(WK_IS_WINDOW(pDlg))
	{
		BurnProducer bp = pDlg->GetBurnProducer();
		if(bp == BP_NERO)
		{
			BOOL bValidNeroVersion = FALSE;
			bValidNeroVersion = pDlg->IsValidNeroVersion();
			
			if(bValidNeroVersion)
			{
				pDlg->SetMainFunction(MF_CD_INFO);
			}
			else
			{
				PostMessage(pDlg->m_hWnd, WM_BURN_ERROR, MF_CD_INFO, EXITCODE_WRONG_NERO_VERSION);
			}

		}
		else if(bp == BP_XP)
		{
//TODO tkr not yet implemented
			PostMessage(pDlg->m_hWnd, WM_BURN_ERROR, MF_CD_INFO, EXITCODE_NO_BURN_SOFTWARE);
		}
		else	//bp == NOPRODUCER
		{
			PostMessage(pDlg->m_hWnd, WM_BURN_ERROR, MF_CD_INFO, EXITCODE_NO_BURN_SOFTWARE);			
		}

	}
	

}


//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestCancelSession(DWORD dwSessionID)
{

	CAcdcDlg* pDlg = theApp.GetAcdcDlg();

	if(WK_IS_WINDOW(pDlg))
	{
		if(m_dwSessionID == dwSessionID)
		{
			m_dwSessionID = dwSessionID;

			CNeroThread* pNeroThread = pDlg->GetNeroThread();

			if(pNeroThread)
			{
				CNeroBurn* pNeroBurn = pNeroThread->GetNeroBurn();
				if(pNeroBurn)
				{	
					pNeroBurn->SetAbortFlag();
					WK_TRACE(_T("OnRequestCancelSession : setting aborted flag \n"));
				}
			}
		}
		else
		{
			//tell the client that requested cancel already is running
			CString sCurrentCancel;
			sCurrentCancel.Format(_T("Error: Cannot cancel unknown session %d \n"), dwSessionID);

			DoIndicateError(CIPC_DC_REQUEST_CANCEL_SESSION,
						    dwSessionID,
							CIPC_ERROR_DATA_CARRIER,
							DC_ERROR_SESSION_IS_RUNNING,
							sCurrentCancel);
			pDlg->AppendMessage(sCurrentCancel);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestSession(DWORD dwSessionID,
										   const CString& sVolume,
										   const CStringArray& sPathnames,
										   DWORD dwFlags)
{
	CAcdcDlg* pDlg = theApp.GetAcdcDlg();

	if(WK_IS_WINDOW(pDlg))
	{
		BurnProducer bp = pDlg->GetBurnProducer();
		if(bp == BP_NERO)
		{
			OnRequestSession_Nero(dwSessionID, sVolume, sPathnames, dwFlags);
		}
		else if(bp == BP_XP)
		{
//TODO tkr not yet implemented
			PostMessage(pDlg->m_hWnd, WM_BURN_ERROR, (WPARAM)MF_CD_WRITE_ISO, (LPARAM)DC_ERROR_NO_BURN_SOFTWARE);
		}
		else	//bp == NOPRODUCER
		{
			PostMessage(pDlg->m_hWnd, WM_BURN_ERROR, (WPARAM)MF_CD_WRITE_ISO, (LPARAM)DC_ERROR_NO_BURN_SOFTWARE);			
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestSession_Nero(DWORD dwSessionID,
								  const CString& sVolume,
								  const CStringArray& sPathnames,
								  DWORD dwFlags)
{
	CAcdcDlg* pDlg = theApp.GetAcdcDlg();
	
	if(WK_IS_WINDOW(pDlg))
	{
		if(m_dwSessionID != dwSessionID)
		{
			if(    dwFlags == CD_WRITE_ISO
				|| dwFlags == DVD_WRITE_ISO)
			{
				TRACE(_T("OnRequestSession: New Session: %d\n"), dwSessionID);
				m_dwSessionID = dwSessionID;
				pDlg->SetVolumeName(sVolume);
				pDlg->SetFilesToBackup(sPathnames);
				pDlg->SetMainFunction(MF_CD_WRITE_ISO);
			}
			else
			{
				//TODO tkr not yet implemented
				//CD_WRITE_VCD	- write CD in VideoCD format
				//CD_WRITE_SVCD	- write CD in SuperVideoCD format

				CString sOnlyIso;
				sOnlyIso.Format(_T("Error: Only burning ISO mediums is supported\n"));
				DoIndicateError(CIPC_DC_REQUEST_SESSION,
								dwSessionID,
								CIPC_ERROR_DATA_CARRIER,
								DC_ERROR_UNKNOWN_ERROR,
								sOnlyIso);
				pDlg->AppendMessage(sOnlyIso);
			}
		}
		else
		{
			//tell the client that requested session already is running
			CString sCurrentSession;
			sCurrentSession.Format(_T("Error: Session %d already is running\n"), dwSessionID);
			TRACE(_T("OnRequestSession: Session %d is running\n"), dwSessionID);
			DoIndicateError(CIPC_DC_REQUEST_SESSION,
						    dwSessionID,
							CIPC_ERROR_DATA_CARRIER,
							DC_ERROR_USER_ABORT,
							sCurrentSession);
			pDlg->AppendMessage(sCurrentSession);
		}
	}
	else
	{
		TRACE(_T("OnRequestSession: no window\n"));
	}
	

}

//////////////////////////////////////////////////////////////////////
DWORD CIPCBurnDataCarrier::GetSessionID()
{
	return m_dwSessionID;
}

//////////////////////////////////////////////////////////////////////
void CIPCBurnDataCarrier::OnRequestGetValue(CSecID id,
								   const CString &sKey,
								   DWORD dwServerData)
{
	CAcdcDlg* pAcdcDlg = theApp.GetAcdcDlg();
	if(WK_IS_WINDOW(pAcdcDlg))
	{
		if(sKey == _T("Burn Software"))
		{
			//check if some burn software is installed
			BurnProducer bp = pAcdcDlg->GetBurnProducer();
			CString sValue = _T("BP_NOPRODUCER");

			switch(bp)
			{
			case BP_NERO:
				sValue = _T("BP_NERO");
				if(pAcdcDlg->GetUsedDriveLetter().IsEmpty())
				{
					sValue = _T("NO_DRIVE");
				}
				break;
			case BP_DIRECT_CD:
				sValue = _T("BP_DIRECT_CD");
				break;
			case BP_XP:
				sValue = _T("BP_XP");
				break;

			default:
				sValue = _T("BP_NOPRODUCER");
			}
			DoConfirmGetValue(id, sKey, sValue, dwServerData);
			
		}
		else if(sKey == _T("Eject DVD"))
		{
			pAcdcDlg->AppendMessage(_T("DVD option not available, eject medium"));
			pAcdcDlg->SetMainFunction(MF_EJECT);	
		}
		
		else if(sKey == _T("Eject"))
		{
			pAcdcDlg->AppendMessage(_T("eject medium"));
			pAcdcDlg->SetMainFunction(MF_EJECT);	
		}
		
		else if(sKey == _T("Backup start"))
		{
			//das Backup wurde gestartet. Nun darf ACDC nicht mehr auf neu erkannte bzw.
			//entfernte USB CD Laufwerke reagieren. Erst nach Abschluss des Backups darf darauf
			//wieder reagiert werden und ACDC neu gestartet werden, um das USB Laufwerk einzubinden
			pAcdcDlg->SetIsInBackup(TRUE);

		}
		else if(sKey == _T("Backup complete"))
		{
			//das Backup wurde wieder beendet
			pAcdcDlg->SetIsInBackup(FALSE);
		}
	}
}
