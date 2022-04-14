// RecordingProcess.cpp: implementation of the CRecordingProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sec3.h"
#include "CipcDatabaseClientServer.h"
#include "input.h"
#include "outputlist.h"
#include "RecordingProcess.h"
#include "ProcessScheduler.h"
#include "ProcessSchedulerTasha.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR BASED_CODE szSectionTemp[] = _T("SecurityServer\\Temp");
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRecordingProcess::CRecordingProcess(CProcessScheduler* pScheduler,
									 CActivation *pActivation, 
									 CompressionType ct,
									 int iNumPicturesRecorded)
 :CProcess(pScheduler)
{
	m_pActivation = pActivation;
	pActivation->SetActiveProcess(this);

	InitTimer();

	m_type = SPT_ENCODE;
	m_CompressionType = ct;
	m_bDoStoring = TRUE;
	m_iFlush = 0;
	SetDoStoring(TRUE);

	if (   GetMacro().IsKeepAlive()
		&& (GetMacro().GetOverallTime() != CPM_TIME_INFINITY)
		&& (GetMacro().GetOverallTime() >= 60*1000))
	{
		m_sRegistryName.Format(_T("INP%08lxIONR%d"),
			pActivation->GetInput()->GetID().GetID(),
			pActivation->GetIONr());

		m_iFlush = 15;
		if (GetMacro().GetPicsPerSlice()==12)
		{
			m_iFlush = 31;
		}
		else if (GetMacro().GetPicsPerSlice()==25)
		{
			m_iFlush = 63;
		}

		if (iNumPicturesRecorded>0)
		{
			m_iNumPicturesReceived = iNumPicturesRecorded;
		}
	}
	m_bTrackingInfoAdded = FALSE;
	// TRACE(_T("CRecordingProcess::CRecordingProcess %s %s\n"),GetDescription(TRUE),NameOfEnum(GetState()));
}
//////////////////////////////////////////////////////////////////////
CRecordingProcess::~CRecordingProcess()
{
	if (!m_sRegistryName.IsEmpty())
	{
		CWK_Profile wkp;
		wkp.DeleteEntry(szSectionTemp,m_sRegistryName);
	}
}
//////////////////////////////////////////////////////////////////////
void CRecordingProcess::SetDoStoring(BOOL bDoStoring)
{
	if (m_bDoStoring != bDoStoring)
	{
//		TRACE(_T("%s DoStoring = %d\n"),GetDescription(IsActiveRecordingProcess()),bDoStoring);
		m_bDoStoring = bDoStoring;
	}
}
//////////////////////////////////////////////////////////////////////
void CRecordingProcess::CleanNotStoringProcesses()
{
	m_NotStoringProcesses.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CRecordingProcess::AddNotStoringProcess(CProcess* pProcess)
{
	m_NotStoringProcesses.Add(pProcess);
}
//////////////////////////////////////////////////////////////////////
void CRecordingProcess::PictureData(const CIPCPictureRecord& pict,DWORD dwMDX, DWORD dwMDY,DWORD dwUserID)
{
	if (GetState()==SPS_PAUSED)
	{
		return;
	}
	/*
	TRACE(_T("PictureData %08lx %d %s,%03d\n"),pict.GetCamID().GetID(),pict.GetNumberOfPictures(),
		pict.GetTimeStamp().GetTime(),pict.GetTimeStamp().GetMilliseconds());*/

	CSecID idCamera = GetActivation()->GetOutputID();
	WORD wArchive = GetActivation()->GetArchiveID().GetSubID();

	if (   idCamera.IsOutputID()
		&& idCamera == pict.GetCamID())
	{
		if (m_bDoStoring)
		{
			CIPCFields fields;
			CIPCPictureRecord* pPict = new CIPCPictureRecord(pict);
			AddAlarmFields(fields,pPict->GetNumberOfPictures());
			for (int i=0;i<m_NotStoringProcesses.GetSize();i++)
			{
				CProcess* pProcess = m_NotStoringProcesses.GetAtFast(i);
				if (pProcess)
				{
					pProcess->AddAlarmFields(fields,pPict->GetNumberOfPictures());
				}
			}
			AddDefaultFields(fields);
			CInput* pInput = GetAlarmInput();
			if (   pInput
				&& GetActivation()->GetTypeOfActivation() == TOA_INPUT_ALARM)
			{
				// add tracking info ??
				if (!m_bTrackingInfoAdded)
				{
					fields.Add(new CIPCField(_T("DBD_MD_S"),_T("1"),'C'));
					m_bTrackingInfoAdded = TRUE;
				}
			}
			theApp.SavePicture(GetMacro().GetCompression(),*pPict,fields,wArchive);
			WK_DELETE(pPict);
		}

		CIPCOutputServer* pCIPCOutputServer = (CIPCOutputServer*)GetScheduler()->GetParentCIPC();
		if (pCIPCOutputServer)
		{
			COutput* pOutput = pCIPCOutputServer->GetOutputAtWritable(idCamera.GetSubID());
			if (pOutput)
			{
				pOutput->SetCurrentArchiveID(CSecID(SECID_GROUP_ARCHIVE,wArchive));
			}
		}

		IncrementPictureCount(pict.GetNumberOfPictures());

		if (   GetMacro().GetNumExpectedPictures()!=0
			&& GetPictureCount()>= (int) GetMacro().GetNumExpectedPictures()) 
		{
			/*
			TRACE(_T("%d of %d Terminate Process %08lx\n"),
				GetPictureCount(),GetMacro().GetNumExpectedPictures(),GetCamID().GetID());*/
			SetState(SPS_TERMINATED,GetTickCount());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CRecordingProcess::OnStateChanging(SecProcessState newState, BOOL bShutDown)
{
	if (!bShutDown)
	{
//		TRACE(_T("CRecordingProcess::OnStateChanging %s\n"),NameOfEnum(newState));
		CInput* pInput = GetAlarmInput();
		if (   pInput   
			&& pInput->IsMD()
			&& !pInput->GetGroup()->IsTasha() // tasha will be done with picture
			&& theApp.GetServerControl()
			&& GetActivation())
		{
			CSecID id = GetActivation()->GetOutputID();
			if (newState==SPS_TERMINATED)
			{
				theApp.GetServerControl()->IndicateAlarmToClients(id,FALSE,0,0);
			}
			else
			{
				CIPCField* pX = pInput->GetFields().GetCIPCField(_T("DBD_MD_X"));
				CIPCField* pY = pInput->GetFields().GetCIPCField(_T("DBD_MD_Y"));
				if (pX && pY)
				{
					DWORD dwX,dwY;
					dwX = 0;
					dwY = 0;
					_stscanf(pX->GetValue(),_T("%04x"),&dwX);
					_stscanf(pY->GetValue(),_T("%04x"),&dwY);
					theApp.GetServerControl()->IndicateAlarmToClients(id,TRUE,MAKELONG((WORD)dwX, (WORD)dwY),0);
				}
			}
		}
		if (   pInput   
			&& pInput->IsMD()
			&& pInput->GetGroup()->IsSavic())
		{
			COutput* pOutput = theApp.GetOutputGroups().GetOutputByID(GetCamID());
			if (pOutput)
			{
				if (newState==SPS_TERMINATED)
				{
					pOutput->SetMDCoordinates(0,0);
				}
				else
				{
					CIPCField* pX = pInput->GetFields().GetCIPCField(_T("DBD_MD_X"));
					CIPCField* pY = pInput->GetFields().GetCIPCField(_T("DBD_MD_Y"));
					if (pX && pY)
					{
						DWORD dwX,dwY;
						dwX = 0;
						dwY = 0;
						_stscanf(pX->GetValue(),_T("%04x"),&dwX);
						_stscanf(pY->GetValue(),_T("%04x"),&dwY);
						pOutput->SetMDCoordinates(dwX,dwY);
					}
				}
			}
		}
		if (   m_pProcessScheduler->ParentIsTasha()
			|| m_pProcessScheduler->ParentIsQ()
			|| m_pProcessScheduler->ParentIsIP())
		{
			CProcessScheduler_Tasha_Q_IP* pPST = (CProcessScheduler_Tasha_Q_IP*)m_pProcessScheduler;
			if (   GetState() == SPS_ACTIVE_RECORDING
				&& (newState == SPS_PAUSED || newState == SPS_TERMINATED)
				)
			{
				// stop request
				pPST->RemoveRequest(this);
			}
			else if (   GetState() == SPS_PAUSED
				&& newState == SPS_ACTIVE_RECORDING)
			{
				// Restart request
				pPST->AddRequest(this);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecordingProcess::IncrementPictureCount(int iNumPics)
{
	m_csPics.Lock();

	m_iNumPicturesReceived += iNumPics;
	
	if (m_iNumPicturesReceived==1 && m_bAdjustStartTime) 
	{
		// modify the start time, to the time when the first picture is received
		m_tickInit=WK_GetTickCount();
	}
	m_iNumSlicePicturesReceived +=iNumPics;
	m_csPics.Unlock();

	if (   !m_sRegistryName.IsEmpty()
		&& (m_iFlush>0)
		&& ((m_iNumPicturesReceived&m_iFlush)==m_iFlush)
		)
	{
		CWK_Profile wkp;
		wkp.WriteInt(szSectionTemp,m_sRegistryName,m_iNumPicturesReceived);
		wkp.FlushKey(szSectionTemp);
	}
}
