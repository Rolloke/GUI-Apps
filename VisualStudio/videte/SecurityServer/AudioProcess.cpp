// AudioProcess.cpp: implementation of the CAudioProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "AudioProcess.h"
#include "Input.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAudioProcess::CAudioProcess(CProcessScheduler* pScheduler,
							 CSecID idClient,
							 CSecID mediaID,
							 DWORD dwFlags)
: CProcess(pScheduler)
{
	CIPCAudioServerClient* pClient = theApp.GetAudioClients()->GetClientByID(idClient);
	CString sMacroName;
	sMacroName.Format(_T("Audio %08lx %s"),
		pClient->GetID().GetID(),pClient->GetConnection().GetSourceHost());

	CProcessMacro *pMacro = new CProcessMacro(PMT_RECORD,
											  sMacroName,
											  SECID_NO_ID);
	pMacro->SetOverallTime(CPM_TIME_INFINITY);
	pMacro->SetIsIntervall(TRUE);

	m_pActivation = new CActivation(pMacro, this);
	m_pActivation->SetOutputID(mediaID);
	m_pActivation->SetPriority(pClient->GetPriority()); 
	
	m_idClient = idClient;
	m_type = SPT_AUDIO;
}
//////////////////////////////////////////////////////////////////////
CAudioProcess::CAudioProcess(CProcessScheduler* pScheduler, CActivation *pActivation)
: CProcess(pScheduler)
{
	m_idClient = SECID_NO_ID;
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);
	InitTimer();
	m_type = SPT_AUDIO;
}
//////////////////////////////////////////////////////////////////////
CAudioProcess::~CAudioProcess()
{
	if (IsClientProcess())
	{
		if (m_pActivation)
		{
			m_pActivation->SetActiveProcess(NULL);
			WK_DELETE(m_pActivation);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CAudioProcess::MediaData(const CIPCMediaSampleRecord& rec, CSecID idMedia)
{
	if (IsClientProcess())
	{
		CIPCAudioServerClient* pClient = theApp.GetAudioClients()->GetClientByID(m_idClient);
		if (   pClient
			&& pClient->GetIPCState() == CIPC_STATE_CONNECTED)
		{
			pClient->DoIndicateMediaData(rec,idMedia,0);
		}
	}
	else
	{
		// connected to database ?
		WORD wArchive = GetActivation()->GetArchiveID().GetSubID();
		
		CIPCFields fields;

		if (GetAlarmInput())
		{
			BOOL bAlarmData = TRUE;
			
			if (GetActivation() &&
				GetActivation()->GetTypeOfActivation()!=TOA_INPUT_ALARM)
			{
				bAlarmData = FALSE;
			}
			
			// Daten nur bei Alarmprozessen
			if (bAlarmData)
			{
				GetAlarmInput()->GetFields().Lock(_T(__FUNCTION__));
				for (int i=0;i<GetAlarmInput()->GetFields().GetSize();i++)
				{
					fields.SafeAdd(new CIPCField(*GetAlarmInput()->GetFields().GetAtFast(i)));
				}
				GetAlarmInput()->GetFields().Unlock();
			}
		} 
		AddDefaultFields(fields);
		theApp.SaveMedia(rec,fields,wArchive);
	}
}
