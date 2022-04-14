/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputVisionCommData.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Vision/CIPCInputVisionCommData.cpp $
// CHECKIN:		$Date: 25.02.04 12:31 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 25.02.04 12:07 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$

#include "stdafx.h"

#include "Vision.h"

#include "CIPCInputVisionCommData.h"

#include "Server.h"
#include "CIPCInputVision.h"
#include "VisionDoc.h"


//////////////////////////////////////////////////////////////////////////////////////
CIPCInputVisionCommData::CIPCInputVisionCommData(LPCTSTR shmName, CVisionDoc* pDoc)
	: CIPCInput(shmName, FALSE)
{
	m_pVisionDoc = pDoc;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCInputVisionCommData::~CIPCInputVisionCommData()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnConfirmConnection()
{
	DoRequestInfoInputs(SECID_NO_GROUPID);	// all groups
//	WK_TRACE("connected to local input comm data\n");
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	// browse InputRecords for CommPort Input
	int i;

	for (i=0;i<numRecords;i++)
	{
		const CIPCInputRecord& rec = records[i];
		if (rec.IsCommPort())
		{
			m_idCommPort = rec.GetID();
			WK_TRACE(_T("local comm is %lx\n"), m_idCommPort.GetID());
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	// nothing to do
	// prevent from ... not overwritten
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
					LPCTSTR szString)	// 1 high, 0 low; 1 changed, 0 unchanged
{
	// nothing to do
	// prevent from ... not overwritten
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	CServer* pServer = NULL;

	pServer= m_pVisionDoc->GetActiveServer();
	if (pServer && pServer->IsConnected())
	{
		CIPCInputVision* pInput = pServer->GetInput();
		if (pInput && (pInput->GetCommID()!=SECID_NO_ID))
		{
			pInput->DoWriteCommData(pInput->GetCommID(),data,dwBlockNr);
		}
	}
	else
	{
		WK_TRACE(_T("local comm data without active remote server\n"));
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputVisionCommData::OnRequestDisconnect()
{
	CWnd* pWnd = AfxGetMainWnd();

//	WK_TRACE("disconnected from local input comm data\n");
	DelayedDelete();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
