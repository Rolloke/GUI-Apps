// CIPCOutputServerCall.cpp: implementation of the CIPCOutputServerCall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCOutputServerCall.h"

#include "MultipleCallProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCOutputServerCall::CIPCOutputServerCall(CMultipleCallProcess* pProcess,LPCTSTR shmName)
 : CIPCOutputClient(shmName,FALSE)
{
	m_pProcess = pProcess;
	m_bIsReady = FALSE;
	m_sName = m_pProcess->GetCurrentHost()->GetName();

	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCOutputServerCall::~CIPCOutputServerCall()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OverrideError()
{

}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnAddRecord(const CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnUpdateRecord(const CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnDeleteRecord(const CIPCOutputRecord &pRec)
{
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnConfirmConnection()
{
	// may be a trace
	WK_TRACE(_T("output connection with %s\n"),m_sName);
	CIPCOutputClient::OnConfirmConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnConfirmInfoOutputs(WORD wGroupID, int iNumGroups, 
												int iNumRecords, 
												const CIPCOutputRecord records[]
												)
{
	WK_TRACE(_T("output info %s\n"),m_sName);
	CIPCOutputClient::OnConfirmInfoOutputs(wGroupID,iNumGroups,iNumRecords,records);

	if (m_pProcess->IsImageCallProcess())
	{
		CString sIniFile,sKey,sSection;

		CString sDir;

		sDir.Format(_T("%s\\%08lx"),CNotificationMessage::GetWWWRoot(),
			m_pProcess->GetCurrentHost()->GetID().GetID());
		WK_CreateDirectory(sDir);

		sIniFile.Format(_T("%s\\%08lx\\names.ini"),CNotificationMessage::GetWWWRoot(),
			m_pProcess->GetCurrentHost()->GetID().GetID());


		// first update already existing records
		for (int i=0;i<GetNumberOfOutputs();i++) 
		{
			const CIPCOutputRecord& rec = GetOutputRecordFromIndex(i);
			// add new record
			if (rec.IsEnabled())
			{
				if (rec.IsCamera())
				{
					if (rec.IsReference())
					{
						sSection = _T("Camera");
					}
				}
				else
				{
					sSection = _T("Relais");
				}
				if (!sSection.IsEmpty())
				{
					// ML 23.9.99 Es muß rec verwendet werden und nicht records[i]
					sKey.Format(_T("%08lx"),rec.GetID().GetID());
					WritePrivateProfileString(sSection,sKey,rec.GetName(),sIniFile);
	// ML 23.9.99	sKey.Format(_T("%08lx"),records[i].GetID().GetID());
	// ML 23.9.99	WritePrivateProfileString(sSection,sKey,records[i].GetName(),sIniFile);
				}
			}
		}
		// flush file
		WritePrivateProfileString(NULL,NULL,NULL,sIniFile);
	}
	m_bIsReady = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCOutputServerCall::OnRequestDisconnect()
{
	DelayedDelete();
}
