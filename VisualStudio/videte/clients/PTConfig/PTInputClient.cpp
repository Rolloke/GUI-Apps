

#include "stdafx.h"
#include "PTInputClient.h"
#include "PTConfigDoc.h"

CPTInputClient::CPTInputClient(CPTConfigDoc *pDoc,const char *shmName)
: CIPCInputClient(shmName,FALSE)
{
	m_pDoc = pDoc;
}
CPTInputClient::~CPTInputClient()
{
}
void CPTInputClient::OnReadChannelFound()
{
	DoRequestConnection();
}
//
void CPTInputClient::OnConfirmConnection()
{
	m_pDoc->m_sState = "verbunden";
	m_pDoc->ForceUpdate();
	DoRequestInfoInputs(SECID_NO_GROUPID );
}
void CPTInputClient::OnConfirmInfoInputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCInputRecord records[])
{
	CIPCInputClient::OnConfirmInfoInputs(wGroupID,iNumGroups,
		numRecords, records);
	m_pDoc->AppendState("AlarmInfos");
}
void CPTInputClient::OnAddRecord(const CIPCInputRecord &pRec)
{
	WK_TRACE("OnAddRecord\n");
}

void CPTInputClient::OnUpdateRecord(const CIPCInputRecord &pRec)
{
	WK_TRACE("OnUpdateRecord\n");
}
void CPTInputClient::OnDeleteRecord(const CIPCInputRecord &pRec)
{
	WK_TRACE("OnDeleteRecord\n");
}
