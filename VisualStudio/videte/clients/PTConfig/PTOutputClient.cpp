

#include "stdafx.h"
#include "PTOutputClient.h"
#include "PTConfigDoc.h"


CPTOutputClient::CPTOutputClient(CPTConfigDoc *pDoc,const char *shmName)
: CIPCOutputClient(shmName,FALSE)
{
	m_pDoc = pDoc;
}
CPTOutputClient::~CPTOutputClient()
{
}
//
void CPTOutputClient::OnReadChannelFound()
{
	DoRequestConnection();
}
void CPTOutputClient::OnConfirmConnection()
{
	DoRequestGetValue(SECID_NO_ID,"BoxName");
	DoRequestGetValue(SECID_NO_ID,"BoxNumber");

	DoRequestInfoOutputs(SECID_NO_GROUPID );
}
void CPTOutputClient::OnConfirmHardware(WORD wGroupID,	
									int errorCode,	// 0==okay, !=0 some error
									BOOL bCanSWCompress,
									BOOL bCanSWDecompress,
									BOOL bCanColorCompress,
									BOOL bCanColorDecompress,
									BOOL bCanOverlay
									)
{
	WK_TRACE("OnConfirmHardware\n");
}
void CPTOutputClient::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, int numRecords, const CIPCOutputRecord records[])
{
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID, iNumGroups, numRecords, records);
	m_pDoc->m_state = PCS_CONNECTED;
	m_pDoc->AppendState("KameraInfos");
}
void CPTOutputClient::OnAddRecord(const CIPCOutputRecord &pRec)
{
	WK_TRACE("OnAddOutputRecord %s\n",pRec.GetName());
}

void CPTOutputClient::OnUpdateRecord(const CIPCOutputRecord &pRec)
{
	WK_TRACE("OnUpdateRecord\n");
}
void CPTOutputClient::OnDeleteRecord(const CIPCOutputRecord &pRec)
{
	WK_TRACE("OnDeleteRecord\n");
}


void CPTOutputClient::OnConfirmGetValue(
						CSecID id, // might be used as group ID only
						const CString &sKey,
						const CString &sValue,
						DWORD dwServerData
						)
{
	WK_TRACE("NOT YET OnConfirmValue %s %s\n",
		(const char *)sKey,
		(const char *)sValue
		);
	if (sKey=="BoxName") {
		m_pDoc->m_sBoxName = sValue;
	} else if (sKey=="BoxNumber") {
		m_pDoc->m_sBoxNumber= sValue;
	}

}
