/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: CIPCInputIdipClientCommData.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/CIPCInputIdipClientCommData.cpp $
// CHECKIN:		$Date: 19.07.05 12:30 $
// VERSION:		$Revision: 7 $
// LAST CHANGE:	$Modtime: 19.07.05 12:11 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"

#include "IdipClient.h"

#include "CIPCInputIdipClientCommData.h"

#include "Server.h"
#include "CIPCInputIdipClient.h"
#include "IdipClientDoc.h"


//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIdipClientCommData::CIPCInputIdipClientCommData(LPCTSTR shmName, CIdipClientDoc* pDoc)
	: CIPCInput(shmName, FALSE)
{
	m_pDoc = pDoc;
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
CIPCInputIdipClientCommData::~CIPCInputIdipClientCommData()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnConfirmConnection()
{
	DoRequestInfoInputs(SECID_NO_GROUPID);	// all groups
//	WK_TRACE("connected to local input comm data\n");
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	theApp.EndResetting();
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
void CIPCInputIdipClientCommData::OnConfirmHardware(WORD wGroupID,int iErrorCode)
{
	// nothing to do
	// prevent from ... not overwritten
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnIndicateAlarmState(WORD wGroupID,DWORD inputMask, DWORD changeMask,
					LPCTSTR szString)	// 1 high, 0 low; 1 changed, 0 unchanged
{
	// nothing to do
	// prevent from ... not overwritten
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnIndicateCommData(CSecID id,
									const CIPCExtraMemory &data,
									DWORD dwBlockNr
									)
{
	CServer* pServer = NULL;

	pServer= m_pDoc->GetActiveServer();
	if (pServer && pServer->IsConnectedLive())
	{
		CIPCInputIdipClient* pInput = pServer->GetInput();
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
void CIPCInputIdipClientCommData::OnRequestDisconnect()
{
	CWnd* pWnd = AfxGetMainWnd();

//	WK_TRACE("disconnected from local input comm data\n");
	DelayedDelete();
	if (pWnd)
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnIndicateAlarmNr(CSecID id, BOOL bAlarm, DWORD dwData1, DWORD dwData2)
{
	// does nothing here
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputIdipClientCommData::OnConfirmAlarmState(WORD wGroupID, DWORD inputMask)
{
	TRACE(_T("OnConfirmAlarmState(%d, %08x)\n"), wGroupID, inputMask);
}
