// CIPCDataCarrierIdipClient.cpp: implementation of the CIPCDataCarrierIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "MainFrm.h"
#include "IdipClientDoc.h"

#include "Server.h"

#include "ViewIdipClient.h"
#include "DlgBackup.h"
#include "CIPCDataCarrierIdipClient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDataCarrierIdipClient::CIPCDataCarrierIdipClient(LPCTSTR shmName, 
												   CIdipClientDoc* pDoc, CSecID id)
	: CIPCDataCarrier(shmName, FALSE)
{
	m_pIdipClientDoc = pDoc;
	m_ID = id;
	m_dwServerVersion = 0; // assume iST 3.0
	m_bGotVolumeInfo = FALSE;
	m_bIsReady = FALSE;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDataCarrierIdipClient::~CIPCDataCarrierIdipClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmConnection()
{
	DoRequestVersionInfo(0);
	//ask for installed burn software
	DoRequestGetValue(SECID_NO_ID,_T("Burn Software"),0);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnRequestDisconnect()
{
	DelayedDelete();
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmGetValue(CSecID id, 
											   const CString &sKey, 
											   const CString &sValue, 
											   DWORD dwServerData)
{
	if(sValue != _T("BP_NOPRODUCER"))
	{
		m_bIsReady = TRUE;
	}
	else
	{
		m_bIsReady = FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwServerVersion = dwVersion;
	WK_TRACE(_T("DataCarrier version %lu\n"), m_dwServerVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnIndicateError(DWORD dwCmd, CSecID id, 
						   CIPCError errorCode, int iUnitCode,
						   const CString &sErrorMsg)
{
	if (errorCode == CIPC_ERROR_DATA_CARRIER)
	{
		CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
		if (pRV)
		{
			CDlgBackup* pBD = pRV->GetDlgBackup();
			if (pBD)
			{
				pBD->OnIndicateError(dwCmd, id, errorCode, iUnitCode, sErrorMsg);
			}
		}
	}
	else
	{
		WK_TRACE(_T("CIPCDataCarrierIdipClient ErrorIndication NOT DC: cmd %s, id %x, %d/%d %s\n"),
					NameOfCmd(dwCmd), id.GetID(), errorCode, iUnitCode, sErrorMsg);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmVolumeInfos(int iNumDrives,
												    const CIPCDrive drives[])
{
	CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pRV)
	{
		CDlgBackup* pBD = pRV->GetDlgBackup();
		if (pBD)
		{
			pBD->OnConfirmVolumeInfos(iNumDrives, drives);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnIndicateSessionProgress(DWORD dwSessionID, int iProgress)
{
	CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pRV)
	{
		CDlgBackup* pBD = pRV->GetDlgBackup();
		if (pBD)
		{
			pBD->OnIndicateSessionProgress(dwSessionID, iProgress);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmSession(DWORD dwSessionID)
{
	CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pRV)
	{
		CDlgBackup* pBD = pRV->GetDlgBackup();
		if (pBD)
		{
			pBD->OnConfirmSession(dwSessionID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmCancelSession(DWORD dwSessionID)
{
	CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pRV)
	{
		CDlgBackup* pBD = pRV->GetDlgBackup();
		if (pBD)
		{
			pBD->OnConfirmCancelSession(dwSessionID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDataCarrierIdipClient::OnConfirmDeleteVolume(const CString& sVolume)
{
	CViewIdipClient* pRV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pRV)
	{
		CDlgBackup* pBD = pRV->GetDlgBackup();
		if (pBD)
		{
			pBD->OnConfirmDeleteVolume(sVolume);
		}
	}
}
