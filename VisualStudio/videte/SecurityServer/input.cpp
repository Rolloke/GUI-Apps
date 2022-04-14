/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: input.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/input.cpp $
// CHECKIN:		$Date: 17.07.06 16:40 $
// VERSION:		$Revision: 101 $
// LAST CHANGE:	$Modtime: 17.07.06 16:32 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "input.h"
#include "CIPCOutputServer.h"
#include "ProcessMacro.h"
#include "ProcessSchedulerVideo.h"
#include "ProcessSchedulerAudio.h"
#include "SecTimer.h"
#include "sec3.h"
#include "cipcdatabaseclientserver.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static TCHAR BASED_CODE szSectionTemp[] = _T("SecurityServer\\Temp");

BOOL CInput::m_bTraceAlarms = FALSE;

void CInput::AddAction(CActivation* pNewActivation)
{
	pNewActivation->SetInput(this);
	m_actions.Add(pNewActivation);
}

CInput::CInput(CIPCInputServer* pParent,WORD wSubID)
{
	m_pParent = pParent;
	m_bMD = pParent->IsMD();
	m_id = CSecID(pParent->GetGroupID(),wSubID);
	m_sName.Format(_T("ohne Name(%x)"),m_id.GetID());
	wState = I_STATE_DEFAULT; 
	m_idActivate = SECID_ACTIVE_OFF;
	m_bIsActive=FALSE;
	m_iNumKeepAliveProcesses=0;
	m_bAlarmOff = FALSE;
	m_bAlarmOffIn15Min = FALSE;
	m_bNoMDRestart = FALSE;
	m_bSDI = pParent->IsSDI();
}

CInput::~CInput()
{
	m_actions.DeleteAll();
}
///////////////////////////////////////////////////////////////
void CInput::IncrementKeepAlive()
{
	m_csNumKeepAlive.Lock();
	m_iNumKeepAliveProcesses++;
	m_csNumKeepAlive.Unlock();
}
///////////////////////////////////////////////////////////////
void CInput::DecrementKeepAlive()
{
	m_csNumKeepAlive.Lock();
	m_iNumKeepAliveProcesses--;
	m_csNumKeepAlive.Unlock();
}
///////////////////////////////////////////////////////////////
void CInput::AddLockTimer(CSecTimer *pTimer)
{
	if (m_lockTimers.Index(pTimer) != -1 ) {
		WK_TRACE(_T("AddLockTimer, already have that timer\n"));
	} else {
		m_lockTimers.Add(pTimer);
	}
}

void CInput::CreateMDOKActivation()
{
	CActivation* pActivation;
	CActivation* pALARMActivation = NULL;
	CActivation* pOKActivation = NULL;

	for (int i=0;i<m_actions.GetSize();i++)
	{
		pActivation = m_actions.GetAtFast(i);
		if (pActivation)
		{
			if (pActivation->GetTypeOfActivation() == TOA_INPUT_ALARM)
			{
				if (pALARMActivation)
				{
					if (   pALARMActivation->GetMacro().GetType() != PMT_RECORD
						&& pActivation->GetMacro().GetType() == PMT_RECORD)
					{
						pALARMActivation = pActivation;
					}
				}
				else
				{
					pALARMActivation = pActivation;
				}
			}
			else if (pActivation->GetTypeOfActivation() == TOA_INPUT_OK)
			{
				pOKActivation = pActivation;
			}
		}
	}

	if (pALARMActivation)
	{
		// there is an alarm activation
		// make sure that MD will always work
		// so create an OK activation if not
		// already done
		if (!pOKActivation)
		{
			CActivation* pAction = new CActivation(-1);

			pAction->SetTypeOfActivation(TOA_INPUT_OK);
			pAction->SetInput(this);
			pAction->SetOutputID(pALARMActivation->GetOutputID());
			pAction->SetPriority(pALARMActivation->GetPriority());
			pAction->SetArchiveID(pALARMActivation->GetArchiveID());
			pAction->m_idTimer = pALARMActivation->m_idTimer;

			const CProcessMacro& alarmMacro = pALARMActivation->GetMacro();
			CString s;
			s.LoadString(IDS_MD_OK);
			CProcessMacro* pMDMakro = new CProcessMacro(PMT_RECORD,s,SECID_PROCESS_CAMERA_MD_OK);
			pMDMakro->SetPictureParameter(alarmMacro.GetCompression(),alarmMacro.GetResolution());

			
			int iNumPics = 1;
			BOOL bIsTasha = FALSE;
			CIPCOutputServer* pCIPCOutputServer = theApp.GetOutputGroups().GetGroupByID(pALARMActivation->GetOutputID());

			if (pCIPCOutputServer)
			{
				bIsTasha = pCIPCOutputServer->IsTasha();
				if (pCIPCOutputServer->IsQ())
				{
					iNumPics = 12;
					int iNrOfCameras = pCIPCOutputServer->GetNrOfActiveCameras();
					if (iNrOfCameras>0)
					{
						iNumPics = 100 / iNrOfCameras;
						if (iNumPics>12)
						{
							iNumPics = 12;
						}
						if (alarmMacro.GetType() == PMT_RECORD)
						{
							if (iNumPics>(int)alarmMacro.GetNumPictures())
							{
								iNumPics = alarmMacro.GetNumPictures();
							}
						}
					}
				}
			}
			else
			{
				CProcessScheduler* pScheduler = GetScheduler(*pALARMActivation);
				if (pScheduler)
				{
					bIsTasha = pScheduler->ParentIsTasha();
				}
			}
			if (bIsTasha)
			{
				if (alarmMacro.GetType() == PMT_RECORD)
				{
					iNumPics = alarmMacro.GetNumPictures();
				}
				else
				{
					iNumPics = 25;
				}
			}

			pMDMakro->SetRecordValues(1000,iNumPics);
			pMDMakro->SetIsIntervall(TRUE);
			pMDMakro->SetOverallTime(CPM_TIME_INFINITY);

			pAction->m_pProcessMacro = pMDMakro;
			pAction->m_idProcessMacro = pMDMakro->GetID();

			WK_TRACE(_T("adding automatic MD OK process %s\n"),GetName());
			theApp.GetProcessMacros().Add(pMDMakro);
			AddAction(pAction);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////
CProcessScheduler* CInput::GetScheduler(CActivation& record)
{
	if (record.GetMacro().GetType() == PMT_AUDIO)
	{
		CIPCAudioServer* pCIPCAudioServer = theApp.GetAudioGroups().GetGroupByID(record.GetOutputID());
		if (pCIPCAudioServer)
		{
			return pCIPCAudioServer->GetProcessScheduler();
		}
	}
	else
	{
		CIPCOutputServer *pOutputGroup = NULL;
		if (   (record.GetMacro().GetType()==PMT_ALARM_CALL)
			|| (record.GetMacro().GetType()==PMT_BACKUP)
			|| (record.GetMacro().GetType()==PMT_ACTUAL_IMAGE)
			|| (record.GetMacro().GetType()==PMT_IMAGE_CALL)
			|| (record.GetMacro().GetType()==PMT_CHECK_CALL)
			|| (record.GetMacro().GetType()==PMT_GUARD_TOUR)
			)
		{
			// alarm call
			// backup process
			// actual image
			// image call
			// check call
			pOutputGroup = theApp.GetOutputDummy();
		}
		else if (   record.GetMacro().GetType()==PMT_VIDEO_OUT
			     || record.GetMacro().GetType()==PMT_PTZ_PRESET
				 || record.GetMacro().GetType() == PMT_ALARM_LIST)
		{
			// video out läuft im Scheduler der Karte
			CSecID id(record.GetMacro().GetVideoOutGroupID(),0);
			if (id.IsOutputID())
			{
				pOutputGroup = theApp.GetOutputGroups().GetGroupByID(id);
			}
			else
			{
				// Quad's laufen im Dummy
				pOutputGroup = theApp.GetOutputDummy();
			}

		}
		else 
		{
			// normal process
			pOutputGroup = theApp.GetOutputGroups().GetGroupByID(CSecID(record.GetOutputID()));
		}

		if (pOutputGroup)
		{
			return pOutputGroup->GetProcessScheduler();
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////
void CInput::StartInitialProcesses()
{
	TRACE(_T("StartInitialProcesses %08lx %s, %d\n"),GetID().GetID(),GetName(),m_bIsActive);
	TypeOfActivation inputAction;
	CWK_Profile wkp;
	
	if (m_bIsActive) 
	{
		inputAction = TOA_INPUT_ALARM;
	} 
	else 
	{
		inputAction = TOA_INPUT_OK;
	}

	int  iNumPicturesRecorded = 0;
	BOOL bFoundOneReanimatedProcess = FALSE;
	CString sKey;

	// gibt es noch einen process, der nach stromausfall
	// wiederbelebt werden muss?
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		if (activation.GetMacro().IsKeepAlive())
		{
			sKey.Format(_T("INP%08lxIONR%d"),GetID().GetID(),activation.GetIONr());
			iNumPicturesRecorded = wkp.GetInt(szSectionTemp,sKey,-1);
			if (iNumPicturesRecorded>0)
			{
				// input action dann einfach auf alarm
				inputAction = TOA_INPUT_ALARM;
				bFoundOneReanimatedProcess = TRUE;
				break;
			}
		}
	}



	for (i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		if (activation.GetMacro().IsKeepAlive()) 
		{
			sKey.Format(_T("INP%08lxIONR%d"),GetID().GetID(),activation.GetIONr());
			iNumPicturesRecorded = wkp.GetInt(szSectionTemp,sKey,-1);
		}
		else
		{
			iNumPicturesRecorded = 0;
		}

		if (activation.GetActiveProcess()) 
		{
			WK_TRACE(_T("Already have process in StartInitalProcesses\n"));
		}
		
		CProcessScheduler* pScheduler = GetScheduler(activation);

		if (pScheduler) 
		{
			CProcess *pProcess = NULL;
			BOOL bThereWasAProcess = FALSE;

			if (   (   bFoundOneReanimatedProcess
				    && activation.GetTypeOfActivation() == inputAction
				    && iNumPicturesRecorded!=-1)
				|| (   !bFoundOneReanimatedProcess
				    && activation.GetTypeOfActivation() == inputAction)
				)
			{
				if (activation.GetMacro().IsKeepAlive()) 
				{
					// CAVEAT preIncrement, see StartNewProcess
					IncrementKeepAlive();
				}

				pScheduler->StartNewProcess(this,
											activation,
											bThereWasAProcess,
											pProcess,
											iNumPicturesRecorded);
				if (pProcess) 
				{
					// fine
				} 
				else 
				{
					// not started
					if (   bThereWasAProcess==FALSE 
						&& activation.GetMacro().IsKeepAlive()) 
					{
						// undo the preIncrement ARGH
						DecrementKeepAlive();
					}
				}
			} else {
				// the other kind of action --> no action now
			}
		} 
		else 
		{
			TRACE(_T("OutputGroup %x not found\n"),activation.GetOutputID().GetID());
		}
	}	// end of loop over actions
}
///////////////////////////////////////////////////////////////////////////
void CInput::StartMissingAlarmCalls()
{
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& record=GetActivation(i);
		CProcess *pProcess = record.GetActiveProcess();

		if (record.GetMacro().GetType()==PMT_ALARM_CALL) 
		{
			CIPCOutputServer *pOutputGroup = theApp.GetOutputDummy();

			if (record.GetTypeOfActivation()==TOA_INPUT_ALARM) 
			{
				// an ALARM process
				if (pProcess) 
				{
					// restart keep alive
					pProcess->Restart();
				} 
				else 
				{
					BOOL bThereWasAProcess;
					if (record.GetMacro().IsKeepAlive()) 
					{
						// preIncrement see CprocessScheduler::StartNewProcess
						IncrementKeepAlive();
					}

					pOutputGroup->GetProcessScheduler()->StartNewProcess(this,record, bThereWasAProcess,pProcess);

					if (!pProcess) 
					{
						// not started
						if (   !bThereWasAProcess 
							&& record.GetMacro().IsKeepAlive()) 
						{
							// undo the preIncrement
							DecrementKeepAlive();
						}
					}
				}
			} 
		} // pOutputGroup
	}	// end of loop over all actions
}
///////////////////////////////////////////////////////////////////////////
void CInput::StartAlarmProcesses()
{
//	TRACE(_T("StartAlarmProcesses %s\n"),GetName());
	// loop over all activations, start alarm processes
	// stop ok processes
	
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		CProcessScheduler *pProcessScheduler = GetScheduler(activation);
			
		if (pProcessScheduler) 
		{
			if (activation.GetTypeOfActivation()==TOA_INPUT_ALARM) 
			{
				// an ALARM process
				pProcessScheduler->Lock(_T(__FUNCTION__));
				CProcess *pProcess = activation.GetActiveProcess();
				if (pProcess) 
				{
					if (   pProcess->IsRelayProcess()
						|| pProcess->IsGuardTourProcess()) 
					{
						// relay processes are not keepAlive process
						// but act similar.
					} 
					else 
					{
						// process already exists, must be keepAlive
						ASSERT(pProcess->GetMacro().IsKeepAlive());
						ASSERT(m_iNumKeepAliveProcesses);
					}
					// restart keep alive
					if (   pProcess->IsRecordingProcess()
						|| pProcess->IsRelayProcess())
					{
						pProcess->Restart();
					}
				} 
				else 
				{
					BOOL bThereWasAProcess = FALSE;
					if (activation.GetMacro().IsKeepAlive()) 
					{
						// preIncrement see CprocessScheduler::StartNewProcess
						IncrementKeepAlive();
					}

					// bThereWasAProcess will be set by reference
					pProcessScheduler->StartNewProcess(this,activation, bThereWasAProcess,pProcess);
					if (pProcess) 
					{
						// fine
					} 
					else 
					{
						// not started
						if (   !bThereWasAProcess 
							&& activation.GetMacro().IsKeepAlive()) 
						{
							// undo the preIncrement
							DecrementKeepAlive();
						}
					}
				}
				pProcessScheduler->Unlock();
			} 
		} // pProcessScheduler
	}	// end of loop over all actions
}
///////////////////////////////////////////////////////////////////////////
void CInput::RestartAlarmProcesses()
{
	//	TRACE(_T("StartAlarmProcesses %s\n"),GetName());
	// loop over all activations, start alarm processes
	// stop ok processes

	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		CProcessScheduler *pProcessScheduler = GetScheduler(activation);

		if (pProcessScheduler) 
		{
			if (activation.GetTypeOfActivation()==TOA_INPUT_ALARM) 
			{
				// an ALARM process
				pProcessScheduler->Lock(_T(__FUNCTION__));
				CProcess *pProcess = activation.GetActiveProcess();
				if (pProcess) 
				{
					if (   pProcess->IsRelayProcess()
						|| pProcess->IsGuardTourProcess()) 
					{
						// relay processes are not keepAlive process
						// but act similar.
					} 
					else 
					{
						// process already exists, must be keepAlive
						ASSERT(pProcess->GetMacro().IsKeepAlive());
						ASSERT(m_iNumKeepAliveProcesses);
					}
					// restart keep alive
					if (   pProcess->IsRecordingProcess()
						|| pProcess->IsRelayProcess())
					{
						pProcess->Restart();
					}
				} 
				pProcessScheduler->Unlock();
			} 
		} // pProcessScheduler
	}	// end of loop over all actions
}
///////////////////////////////////////////////////////////////
void CInput::StartOKProcesses()
{
//	TRACE(_T("StartOKProcesses %s\n"),GetName());
	// loop over all activations
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation &record=GetActivation(i);
		CProcess *pProcess = record.GetActiveProcess();

		CProcessScheduler* pScheduler = GetScheduler(record);

		if (pScheduler) 
		{
			if (record.GetTypeOfActivation()==TOA_INPUT_OK) 
			{
				m_csNumKeepAlive.Lock();
				int iNumKeepAliveProcesses = m_iNumKeepAliveProcesses;
				m_csNumKeepAlive.Unlock();
				if (iNumKeepAliveProcesses==0) 
				{
					BOOL bThereWasAProcess = FALSE;
					BOOL bRet = pScheduler->StartNewProcess(this, record, bThereWasAProcess,pProcess);
					if (IsMD())
					{
						if (bRet && m_bNoMDRestart)
						{
						//	TRACE(_T("restart with MD OK !! %d %s\n"),m_bIsActive,GetName());
						}
						m_bNoMDRestart = FALSE;
					}
				}
				// can't be keepAlice process, so no need to count m_iNumKeepAlive
			} 
		}
	}	// end of loop over actions
}
///////////////////////////////////////////////////////////////
void CInput::StopOKProcesses()
{
//	TRACE(_T("StopOKProcesses %s\n"),GetName());
	// loop over all activations, start alarm processes
	// stop ok processes
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		CProcess *pProcess = activation.GetActiveProcess();
		CProcessScheduler *pScheduler = GetScheduler(activation);
		
		if (pScheduler) 
		{
			if (activation.GetTypeOfActivation() == TOA_INPUT_OK) 
			{
				if (pProcess) 
				{
					// a relay process might stay even after the Stop
					pScheduler->StopProcess(pProcess);
				}
			} 
		} // pOutputGroup
	}	// end of loop over all actions
}
///////////////////////////////////////////////////////////////
// stops all alarm processes
void CInput::StopAlarmProcesses()
{
//	TRACE(_T("StopAlarmProcesses %s\n"),GetName());
	// loop over all activations
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation& activation = GetActivation(i);
		CProcess *pProcess = activation.GetActiveProcess();
		CProcessScheduler *pScheduler = GetScheduler(activation);

		if (pScheduler) 
		{
			if (activation.GetTypeOfActivation() == TOA_INPUT_ALARM) 
			{
				// an ALARM process
				if (pProcess) 
				{
					if (activation.GetMacro().IsKeepAlive()) 
					{
						// don't touch
					} 
					else 
					{
						// stop plain ALARMs, relays are handled in StopProcess
						pScheduler->StopProcess(pProcess);
					}
				} 
			} 
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
// CAVEAT only does start missing processes
// check calls !!! NOT YET
void CInput::StartMissingProcesses(WORD wOutputGroupID, const CSecTimer *pTimer)
{
	TypeOfActivation inputAction;
	if (m_bIsActive) {
		inputAction=TOA_INPUT_ALARM;
	} else {
		inputAction=TOA_INPUT_OK;
	}

	// loop over all actions and distribute to
	// the according process scheduler in the output groups
	for (int i=0;i<GetNumActions();i++) 
	{
		CActivation &record=GetActivation(i);
		const CProcessMacro *pProcessMacro=NULL;
		pProcessMacro = theApp.GetProcessMacros().GetMacroByID(record.m_idProcessMacro);
		m_csNumKeepAlive.Lock();
		int iNumKeepAliveProcesses = m_iNumKeepAliveProcesses;
		m_csNumKeepAlive.Unlock();
		
		if (pProcessMacro==NULL) 
		{
			TRACE(_T("ProcessMacro %x not found\n"),record.m_idProcessMacro);
		} 
		else if (record.GetTypeOfActivation()==inputAction
					&& record.GetActiveProcess()==NULL
					&& 
						(  inputAction==TOA_INPUT_ALARM	// always
						|| (inputAction==TOA_INPUT_OK 
						    && iNumKeepAliveProcesses==0)
						)
					) 
		{	
			// the above means: start a missing process, if it matches the action
			// and there is no active process already running for this action
			// and there are no keepAliveProcesses running in case this
			//			 action is an OK process

			// macro found and no active process
			// a) any output-group
			// b) explictit output group no timer
			// c) no id, but explicit timer
			BOOL bDoIt=FALSE;
			if (wOutputGroupID==SECID_NO_GROUPID) 
			{
				if (pTimer==NULL || record.m_idTimer==pTimer->GetID()) 
				{
					bDoIt = TRUE;
				}
			} 
			else 
			{	// expictit group wOutputGroupID
				if (wOutputGroupID==record.GetOutputID().GetGroupID()
					&& (pTimer==NULL || record.m_idTimer==pTimer->GetID())
					) {
					bDoIt=TRUE;
				}
			}
			if ( bDoIt ) 
			{
				CProcessScheduler* pScheduler = GetScheduler(record);
				if (pScheduler) 
				{
					if (record.GetMacro().IsKeepAlive() && inputAction==TOA_INPUT_ALARM) 
					{
						IncrementKeepAlive();
					}

					BOOL bThereWasAProcess = FALSE;
					CProcess *pProcess = NULL;
					pScheduler->StartNewProcess(this,record, bThereWasAProcess,pProcess);
					if (pProcess) 
					{
						// fine
					} 
					else 
					{
						if (bThereWasAProcess==FALSE && 
							record.GetMacro().IsKeepAlive()
							&& inputAction==TOA_INPUT_ALARM) 
						{
							// undo the preIncrement
							DecrementKeepAlive();
						}
					}
				} 
				else 
				{
					TRACE(_T("Output %x not found\n"),record.GetOutputID().GetID());
				}
			} // end of output group ok
		}	// end of macro found
	}	// end of loop over actions
}
/////////////////////////////////////////////////////////////////////
void CInput::ProcessTerminated(CProcess *pProcess)
{
	BOOL bStartOKProcesses = FALSE;
	BOOL bIsKeepAlive = pProcess->GetMacro().IsKeepAlive();
	if (bIsKeepAlive) 
	{
		m_csNumKeepAlive.Lock();
		m_iNumKeepAliveProcesses--;
		if (m_iNumKeepAliveProcesses==0) 
		{
			// keepAlive is restricted to ALARM processes
			// so the it's only required to start OK processes now
			if (m_bIsActive==FALSE) 
			{
				bStartOKProcesses = TRUE;
			}
		}
		m_csNumKeepAlive.Unlock();
	}

	if (bStartOKProcesses)
	{
		StartOKProcesses();
	}
	else
	{
		if (   bIsKeepAlive 
			&& IsMD())
		{
			TRACE(_T("do not restart with MD OK ?? %d %s\n"),
				m_bIsActive,GetName());
			m_bNoMDRestart = TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CInput::SetActive(BOOL bIsActive, BOOL bStartProcesses)
{
	m_bIsActive = bIsActive;
//	TRACE(_T("CInput::SetActive %s %d\n"),GetName(),m_bIsActive);
	if (theApp.IsUpAndRunning())
	{
		// UNUSED BOOL bOldState = m_bIsActive;
		// set new state
		CString sState;
		
		if (m_bTraceAlarms)
		{
			WK_STAT_LOG(_T("Units"),m_bIsActive,GetName());
		}
		
		if (bStartProcesses) 
		{
			if (m_bIsActive) 
			{	
				// new state ALARM
				StopOKProcesses();
				StartAlarmProcesses();

				if (IsMD())
				{
					// Activity an Video out weiterleiten
					CString sOutputShm(m_pParent->GetShmName());
					sOutputShm.Replace(_T("MDInput"),_T("OutputCamera"));
					CIPCOutputServer* pOutputServer = theApp.GetOutputGroups().GetGroupShmName(sOutputShm);
					if (pOutputServer)
					{
						CSecID idCam(pOutputServer->GetGroupID(),GetID().GetSubID());
//						TRACE(_T("md video out %08lx\n"),idCam.GetID());
						pOutputServer->DoActivityVideoOut(idCam,-1);
					}
					else
					{
						TRACE(_T("no video out group found\n"));
					}
				}
			} 
			else 
			{	
				// new state OK
				StopAlarmProcesses();
				StartOKProcesses();
				// CheckForUnselectCamera();
			}
			
			// NEW 260397
			if (IsUsedAsLockForTimer()) 
			{
				if (m_bIsActive==FALSE) 
				{
					CleanUpLockProcesses();
				} 
				else 
				{
					StartLockProcesses();
				}
			}
		}	// end of do start processes
	}
}

/*@MD
Called from @MLINK{CInput::SetActive}.
*/
void CInput::CleanUpLockProcesses()
{
	// PRE current state is OK
	for (int i=0;i<m_lockTimers.GetSize();i++) 
	{
		CSecTimer *pTimer = m_lockTimers[i];
		theApp.KillProcessesByTimer(pTimer);
	}
}

/*@MD
Called from @MLINK{CInput::SetActive}.
*/
void CInput::StartLockProcesses()
{
	// PRE current state is ALARM or lost
	for (int i=0;i<m_lockTimers.GetSize();i++) 
	{
		CSecTimer *pTimer = m_lockTimers[i];
		theApp.StartProcessesByTimer(pTimer);
	}
}
//////////////////////////////////////////////////////////////////
void CInput::SetFields(const CIPCFields& fields)
{
	m_Fields.Lock(_T(__FUNCTION__));
	m_Fields.DeleteAll();
	for (int i=0;i<fields.GetSize();i++)
	{
		m_Fields.Add(new CIPCField(*fields.GetAtFast(i)));
	}
	m_Fields.Unlock();
}
//////////////////////////////////////////////////////////////////
void CInput::AddFields(const CIPCFields& fields)
{
	m_Fields.Lock(_T(__FUNCTION__));
	if (IsSDI())
	{
		for (int i=0;i<fields.GetSize();i++)
		{
			m_Fields.Add(new CIPCField(*fields.GetAtFast(i)));
		}
	}
	else
	{
		m_Fields.AddNoDuplicates(fields);
	}
	m_Fields.Unlock();
}
//////////////////////////////////////////////////////////////////
void CInput::CopyFields(CIPCFields& fields)
{
	m_Fields.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Fields.GetSize();i++)
	{
		fields.Add(new CIPCField(*m_Fields.GetAtFast(i)));
	}
	m_Fields.Unlock();
}
//////////////////////////////////////////////////////////////////
void CInput::DeleteFirstFields(int iNumFields, LPCTSTR szName)
{
	m_Fields.Lock(_T(__FUNCTION__));
	for (int j=0;j<iNumFields;j++)
	{
		for (int i=0;i<m_Fields.GetSize();i++)
		{
			CIPCField* pField = m_Fields.GetAtFast(i);
			if (pField->GetName() == szName)
			{
				m_Fields.RemoveAt(i);
				WK_DELETE(pField);
				break;
			}
		}
	}
	m_Fields.Unlock();
}
//////////////////////////////////////////////////////////////////
void CInput::LoadAlarmOffSpans(CWK_Profile& wkp,const CString& sSection)
{
	CString s;
	s.Format(_T("%s\\%08lx"),sSection,GetID().GetID());
	m_AlarmCallDeactivationSpans.DeleteAll();
	m_AlarmCallDeactivationSpans.Load(wkp,s);
	TRACE(_T("%s %d AOS loaded\n"),GetName(),m_AlarmCallDeactivationSpans.GetSize());
	CSystemTime st;
	st.GetLocalTime();
	m_bAlarmOff = m_AlarmCallDeactivationSpans.IsIncluded(st);
/*
	CSystemTime st15Min(0, 0, 0, 0, 15, 0);
	st = st + st15Min;
	m_bAlarmOffIn15Min = m_AlarmCallDeactivationSpans.IsIncluded(st);
	if (m_bAlarmOffIn15Min)
	{
		CWK_RunTimeError e(WAI_SECURITY_SERVER,
						   REL_MESSAGE,
						   RTE_ALARM_OFF_SPAN,
						   m_sName,
						   0,
						   TRUE);
		e.Send();
		e.SetParam2(FALSE);
		e.Send();
	}
*/
}
//////////////////////////////////////////////////////////////////
void CInput::SaveAlarmOffSpans(CWK_Profile& wkp,const CString& sSection)
{
	CString s;
	s.Format(_T("%s\\%08lx"),sSection,GetID().GetID());
	m_AlarmCallDeactivationSpans.Save(wkp,s);
}
//////////////////////////////////////////////////////////////////
void CInput::SetAlarmOffSpans(int iNumRecords, 
							  const CSystemTimeSpan data[],
							  const CString& sUser,
							  const CString& sHost)
{
	CString sSectionName;
	CWK_Profile wkp;
	sSectionName.Format(_T("AlarmOffSpans\\Group%x\\%08lx"),
		GetID().GetGroupID(),GetID().GetID());
	m_AlarmCallDeactivationSpans.DeleteAll();
	m_AlarmCallDeactivationSpans.FromArray(iNumRecords,data);
	m_AlarmCallDeactivationSpans.Save(wkp,sSectionName);

	WK_TRACE(_T("User %s from Host %s\n"),sUser,sHost);
	for (int i=0;i<m_AlarmCallDeactivationSpans.GetSize();i++)
	{
		WK_TRACE(_T("Alarm Off Span %s, %s -> %s\n"),GetName(),
			m_AlarmCallDeactivationSpans.GetAtFast(i)->GetStartTime().GetDateTime(),
			m_AlarmCallDeactivationSpans.GetAtFast(i)->GetEndTime().GetDateTime());
	}

	WriteAlarmOffSpanProtocol(sUser,sHost);

	CSystemTime st;
	st.GetLocalTime();
	m_bAlarmOff = m_AlarmCallDeactivationSpans.IsIncluded(st);
	InformClientsAlarmOffSpans();
	// send no RTE_ALARM_OFF_SPAN regarding 15 min before,
	// because the User has just changed the settings,
	// he should KNOW what he has done!
}
//////////////////////////////////////////////////////////////////
BOOL CInput::IsInAlarmOffSpan()
{
	CSystemTime s;
	s.GetLocalTime();
	return m_AlarmCallDeactivationSpans.IsIncluded(s);
}
//////////////////////////////////////////////////////////////////
BOOL CInput::RecalcAlarmOffSpans(const CSystemTime& st)
{
	if (GetIDActivate() != SECID_ACTIVE_OFF)
	{
		BOOL bAlarmOff = FALSE;
		BOOL bAlarmOffIn15Min = FALSE;
		CSystemTimeSpan* pSpan = NULL;
		CSystemTime* pStartTime15Min = NULL;
		m_AlarmCallDeactivationSpans.Lock(_T(__FUNCTION__));
		for (int i=0 ; i<m_AlarmCallDeactivationSpans.GetSize() ; i++)
		{
			pSpan = m_AlarmCallDeactivationSpans.GetAtFast(i);
			if (pSpan->GetEndTime() < st)
			{
				WK_DELETE(pSpan);
				m_AlarmCallDeactivationSpans.RemoveAt(i);
			}
			else
			{
				bAlarmOff |= pSpan->IsIncluded(st);
				CSystemTime st15Min(1, 1, 1601, 0, 15, 0);
				st15Min = st + st15Min;
				if (pSpan->IsIncluded(st15Min))
				{
					if (   (pStartTime15Min == NULL)
						|| (pSpan->GetStartTime() < *pStartTime15Min)
						)
					{
						pStartTime15Min = &pSpan->GetStartTime();
					}
					bAlarmOffIn15Min |= TRUE;
				}
			}
		}
		m_AlarmCallDeactivationSpans.Unlock();

		if (m_bAlarmOff!=bAlarmOff)
		{
			// Clients informieren
			m_bAlarmOff = bAlarmOff;
			InformClientsAlarmOffSpans();

			// bei Alarm fehlende Anrufprozesse starten
			if (IsActive())
			{
				StartMissingAlarmCalls();
			}
		}
		// RTE already sent? Otherwise: Send!
		if (   m_bAlarmOffIn15Min == FALSE
			&& bAlarmOffIn15Min
			)
		{
			CString sErrorText,s;

			sErrorText = m_sName;
			for (int i=0;i<m_AlarmCallDeactivationSpans.GetSize();i++)
			{
				s.Format(IDS_ALARM_OFF3,
						 m_AlarmCallDeactivationSpans.GetAtFast(i)->GetStartTime().GetDateTime(),
						 m_AlarmCallDeactivationSpans.GetAtFast(i)->GetEndTime().GetDateTime());
				sErrorText += _T(" ") + s;

			}

			CWK_RunTimeError e(WAI_SECURITY_SERVER,
							   REL_MESSAGE,
							   RTE_ALARM_OFF_SPAN,
							   sErrorText,
							   0,
							   TRUE);
			// set the alarm
			e.Send();
			Sleep(10);
			// reset the alarm
			e.SetErrorText(_T(""));
			e.SetParam2(FALSE);
			e.Send();
		}
		// set always, can be FALSE, if no matching time span
		m_bAlarmOffIn15Min = bAlarmOffIn15Min;
	}

	return m_bAlarmOff;
}
//////////////////////////////////////////////////////////////////
void CInput::InformClientsAlarmOffSpans()
{
	CString sValue = m_bAlarmOff ? CSD_ON : CSD_OFF;
	for (int i=0;i<theApp.GetInputClients().GetSize();i++)
	{
		theApp.GetInputClients().GetAtFast(i)->DoConfirmGetValue(
			GetID(),CSD_ALARM_SPAN_OFF_STATUS,sValue,0);
	}
}
//////////////////////////////////////////////////////////////////
void CInput::WriteAlarmOffSpanProtocol(const CString& sUser,
								       const CString& sHost)
{
	// TODO
	CWK_Profile prof;

	CString sDir = prof.GetString(SECTION_LOG, LOG_PROTOCOLPATH, DEFAULT_PROTOCOLPATH);
	WK_CreateDirectory(sDir);

	CString sFile,sTime;
	CSystemTime st;
	st.GetLocalTime();
	sTime = st.GetDBDate() + st.GetDBTime();

	sFile.Format(_T("%s\\%s %s.htm"),sDir,GetName(),sTime);
	
	CString sTitle, s, sMetaCodePage, sMetaGenerator;

	s.LoadString(IDS_ALARM_OFF);
	sTitle.LoadString(IDS_PROTOCOL);
	sTitle += _T(": ");
	sTitle += s;
	sTitle += _T(" ");
	sTitle += GetName();
	sTitle += _T(" ");
	sTitle += st.GetDateTime();

	CString sText;
	sText.Format(IDS_ALARM_OFF2,sUser,sHost,GetName());
	sText += _T("\n<ol>\n");

	for (int i=0;i<m_AlarmCallDeactivationSpans.GetSize();i++)
	{
		s.Format(IDS_ALARM_OFF3,
				 m_AlarmCallDeactivationSpans.GetAtFast(i)->GetStartTime().GetDateTime(),
				 m_AlarmCallDeactivationSpans.GetAtFast(i)->GetEndTime().GetDateTime());
		sText += "<li><font color=\"#0000FF\">" + s + _T("</font></li>\n");
	}
	sText += _T("</ol>\n");

	CStdioFileU f;
	UINT nCodePage = CWK_String::GetCodePage();
	f.SetFileMCBS((WORD)nCodePage);
	if (nCodePage != CP_ACP)
	{
		sMetaCodePage.Format(HTML_CODEPAGE_METATAG, theApp.GetHTMLCodePage(nCodePage));
	}
	sMetaGenerator.Format(HTML_GENERATOR_METATAG, theApp.m_pszAppName);

	TRY
	{
		if (f.Open(sFile,CFile::modeCreate|CFile::modeWrite))
		{
			f.WriteString(_T("<html>\n"));
			f.WriteString(_T(" <head>\n"));
			if (nCodePage != CP_ACP)
			{
				f.WriteString(sMetaCodePage);
			}
			f.WriteString(sMetaGenerator);
			f.WriteString(_T("  <title>\n"));
			f.WriteString(sTitle);
			f.WriteString(_T("  </title>\n"));
			f.WriteString(_T(" </head>\n"));
			f.WriteString(_T("<body>\n"));
			f.WriteString(_T("<h3>")+sTitle+_T("</h3>\n"));
			f.WriteString(sText);
			f.WriteString(_T("</body>\n"));
			f.WriteString(_T("</html>\n"));

			f.Flush();
			f.Close();
		}
	}
	WK_CATCH (_T("cannot write html protocol"));
}
