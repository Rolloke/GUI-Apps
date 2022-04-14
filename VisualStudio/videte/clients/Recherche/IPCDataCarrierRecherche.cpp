// IPCDataCarrierRecherche.cpp: implementation of the CIPCDataCarrierRecherche class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Recherche.h"
#include "Server.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "BackupDialog.h"
#include "IPCDataCarrierRecherche.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDataCarrierRecherche::CIPCDataCarrierRecherche(LPCTSTR shmName, 
												   CServer* pServer, CSecID id)
	: CIPCDataCarrier(shmName, FALSE)
{
	m_pServer = pServer;
	m_ID = id;
	m_dwServerVersion = 0; // assume iST 3.0
	m_bGotVolumeInfo = FALSE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDataCarrierRecherche::~CIPCDataCarrierRecherche()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmConnection()
{
	DoRequestVersionInfo(0);
	//ask for installed burn software
	DoRequestGetValue(SECID_NO_ID,_T("Burn Software"),0);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmGetValue(CSecID id, 
											   const CString &sKey, 
											   const CString &sValue, 
											   DWORD dwServerData)
{
	if(sValue != _T("BP_NOPRODUCER"))
	{
		m_pServer->SetDataCarrierReady(TRUE);
	}
	else
	{
		m_pServer->SetDataCarrierReady(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwServerVersion = dwVersion;
	WK_TRACE(_T("DataCarrier version %lu\n"), m_dwServerVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnIndicateError(DWORD dwCmd, CSecID id, 
						   CIPCError errorCode, int iUnitCode,
						   const CString &sErrorMsg)
{
	if (errorCode == CIPC_ERROR_DATA_CARRIER)
	{
		CRechercheDoc* pDoc = m_pServer->GetDocument();
		CRechercheView* pRV = pDoc->GetRechercheView();
		if (WK_IS_WINDOW(pRV))
		{
			CBackupDialog* pBD = pRV->GetBackupDialog();
			if (WK_IS_WINDOW(pBD))
			{
				pBD->OnIndicateError(dwCmd, id, errorCode, iUnitCode, sErrorMsg);
			}
		}
	}
	else
	{
		WK_TRACE(_T("CIPCDataCarrierRecherche ErrorIndication NOT DC: cmd %s, id %x, %d/%d %s\n"),
					NameOfCmd(dwCmd), id.GetID(), errorCode, iUnitCode, sErrorMsg);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmVolumeInfos(int iNumDrives,
												    const CIPCDrive drives[])
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnConfirmVolumeInfos(iNumDrives, drives);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnIndicateSessionProgress(dwSessionID, iProgress);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmSession(DWORD dwSessionID)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnConfirmSession(dwSessionID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmCancelSession(DWORD dwSessionID)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnConfirmCancelSession(dwSessionID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierRecherche::OnConfirmDeleteVolume(const CString& sVolume)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnConfirmDeleteVolume(sVolume);
		}
	}
}
