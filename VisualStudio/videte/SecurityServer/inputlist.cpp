/* GlobalReplace: pApp\-\> --> theApp. */
/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: inputlist.cpp $
// ARCHIVE:		$Archive: /Project/SecurityServer/inputlist.cpp $
// CHECKIN:		$Date: 30.08.06 11:06 $
// VERSION:		$Revision: 129 $
// LAST CHANGE:	$Modtime: 30.08.06 11:01 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"

#include "InputList.h"

#include "sec3.h"
#include "CipcInputServer.h"
#include "ProcessMacro.h"

#include "outputlist.h"
#include "CIPCOutputServer.h"
#include "Process.h"
#include "ProcessScheduler.h"

#include "ProcessListView.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CInputList::CInputList()
{
#ifdef TRACE_LOCKS
	m_nCurrentThread = theApp.m_bTraceLocks ? 0 : -1;
#endif
}

/////////////////////////////////////////////////////////////////////////////
CInputList::~CInputList()
{
	SafeDeleteAll();
}

/////////////////////////////////////////////////////////////////////////////
void CInputList::StartThreads()
{
	for (int i=0;i<GetSize();i++) 
	{
		GetAtFast(i)->StartThread();
		Sleep(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::StartInitialProcesses()
{
	for (int i=0;i<GetSize();i++) 
	{
		GetAtFast(i)->StartInitialProcesses();
	}
}
/////////////////////////////////////////////////////////////////////////////
// pName:		Der Name der neue Gruppe
// iNr:			Die Anzahl der Eingänge
// RETURN:		Ein Pointer auf die neue Gruppe wenn erfolgreich, sonst NULL
CIPCInputServer* CInputList::NewGroup(LPCTSTR pName, 
									  int iNr, 
									  LPCTSTR pSMName,
									  CSecID newID)
{
	if (iNr>=0 && pName) 
	{
		CIPCInputServer* pGrp = new CIPCInputServer(pName, iNr, pSMName);
		if (pGrp) 
		{
			SafeAdd(pGrp);
			pGrp->SetGroupID(newID.GetGroupID());
			return pGrp;
		}
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
void CInputList::Load(CWK_Profile& wkp)
{
	// Inputgroups aktivieren
	int iMaxGroups = wkp.GetInt(_T("InputGroups"), _T("MaxGroups"), 0);
	for (int i=0;i<iMaxGroups;i++)
	{
		CString sTmp = wkp.GetString(_T("InputGroups"), i+1,_T(""));
		if (sTmp.GetLength()) 
		{
			AddInputGroup(sTmp);
		}
	}
	int iWantsCommUnit = wkp.GetInt(_T("CommUnit"),_T("Enabled"),0);
	if (iWantsCommUnit) 
	{
		WK_TRACE(_T("Wants CommUnit\n"));
		CIPCInputServer* pGrp = NewGroup(_T("CommUnit"), 1, SM_COMMUNIT_INPUT, SECID_INPUT_COMMUNIT);
		CInput* pInput = new CInput(pGrp,(WORD)0);	// default dummy
		pInput->SetName(_T("CommPortDummyInput"));
		pInput->wState = I_STATE_IS_COMM_PORT; 
		pGrp->AddInput(pInput);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::CreateMDOKActivations()
{
	CIPCInputServer* pGroup;

	for (int i=0;i<GetSize();i++)
	{
		pGroup = GetGroupAt(i);
		if (pGroup && pGroup->IsMD())
		{
			pGroup->CreateMDOKActivations();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputList::RecalcAlarmOffSpans(const CSystemTime& st)
{
	BOOL bRet = FALSE;
	CIPCInputServer* pGroup;

	for (int i=0;i<GetSize();i++)
	{
		pGroup = GetGroupAt(i);
		if (pGroup)
		{
			bRet |= pGroup->RecalcAlarmOffSpans(st);
		}
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CInputList::InformClientsAlarmOffSpans()
{
	for (int i=0;i<GetSize();i++)
	{
		GetAtFast(i)->InformClientsAlarmOffSpans();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CInputList::AddInputGroup(const CString &str)
{
	int iNr = CWK_Profile::GetNrFromString(str, INI_NR, 0L);
	if (iNr<=0) {
		return FALSE;	// EXIT
	}

	CString sName = CWK_Profile::GetStringFromString(str, _T("\\CO"), _T(""));
	CString sShmName = CWK_Profile::GetStringFromString(str, INI_SMNAME, _T(""));	
	// get the unique id
	WORD storedID = (WORD)CWK_Profile::GetHexFromString(str, INI_ID, SECID_NO_GROUPID);	
	// NOY YET check for duplicate IDs
	ASSERT(storedID != -1);
	CIPCInputServer* pGrp = NewGroup(sName, iNr, sShmName, CSecID(storedID,0));
	if (!pGrp) {
		WK_TRACE_ERROR(_T("failed NewGroup\n"));
		return FALSE;
	}

	CString sSectionName,sAlarmOffSection;
	CWK_Profile wkp;
	sSectionName.Format(_T("InputGroups\\Group%x"),pGrp->GetGroupID());
	sAlarmOffSection.Format(_T("AlarmOffSpans\\Group%x"),pGrp->GetGroupID());

	// loop over all input for this group
	for (int i=0;i<iNr;i++) 
	{
		CInput* pInput = new CInput(pGrp,(WORD)i);	// default dummy
		// is there a value for InputGroups\Group%x\%d ?
		CString sSubString = wkp.GetString(sSectionName, i+1, _T("") );
		if (sSubString.GetLength()) 
		{
			// read input record
			// make sure to set/clear to avoid problems with default value fo wState
			if (CWK_Profile::GetNrFromString(sSubString, INI_EDGE, 0)) {
				pInput->wState |= I_STATE_EDGE; 
			} else {
				pInput->wState &= ~I_STATE_EDGE; 
			}

			DWORD tmpDW = CWK_Profile::GetHexFromString(sSubString, INI_ACTIVE, SECID_ACTIVE_OFF);
			CSecID tmpID;
			if (tmpDW==SECID_NO_ID) tmpID=SECID_ACTIVE_OFF;
#if 1
			// OOPS old stuff ?
			else if (tmpDW==0) tmpID=SECID_ACTIVE_OFF;
			else if (tmpDW==1) tmpID=SECID_ACTIVE_ON;
			else if (tmpDW==2) tmpID=SECID_ACTIVE_LOCK;
			else tmpID = tmpDW;
#endif
			pInput->SetIDActivate( tmpID );

			CString sTmp = CWK_Profile::GetStringFromString(sSubString, _T("\\CO"), _T(""));
			if (sTmp.GetLength()) {
				pInput->SetName(sTmp);
			}
			// Temporary deactivation allowed
			pInput->SetTempDeactivateAllowed((BOOL)CWK_Profile::GetNrFromString(sSubString,
																		  INI_TEMP_DEACT, 1));

			// read Activation records
			if (pInput->GetIDActivate() != SECID_ACTIVE_OFF)
			{
				BOOL bActivationFound=TRUE;
				for (int a=0; bActivationFound==TRUE; a++) 
				{
					bActivationFound = ReadIo(*pInput,sSectionName, i, a);
				}

				pInput->LoadAlarmOffSpans(wkp,sAlarmOffSection);
			}
		}
		pGrp->AddInput(pInput);
	}	// end of loop over all inputs
	
	for (int c=1;c < pGrp->GetSize();c++) 
	{
		if (pGrp->GetInputAt(c-1)->GetID()!=	
			pGrp->GetInputAt(c)->GetID().GetID()-1) 
		{
			WK_TRACE_ERROR(_T("%s/%s inputs not in sequence %c/%c\n"),pGrp->GetInputAt(c)->GetName(),pGrp->GetInputAt(c-1)->GetName(), c, c+1);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CInputList::ReadIo(CInput & oneInput,
						const CString &sSectionName, 
						int inputNr, int ioNr)
{
	CWK_Profile wkp;
	
	// Activations
	CString sActivationFolder;
	sActivationFolder.Format(_T("%s\\InputsToOutputs"),(LPCTSTR)sSectionName);

	CString sActivationEntry, sActivationValue;
	sActivationEntry.Format(_T("%d %d"),inputNr,ioNr);

	CString sData = wkp.GetString(sActivationFolder, sActivationEntry, _T(""));
	if (sData.GetLength()) 
	{
		CActivation* pAction = new CActivation(ioNr);
		
		pAction->SetInput(&oneInput);
		pAction->SetOutputID((DWORD)CWK_Profile::GetHexFromString(sData, _T("\\O"), SECID_NO_ID));
		
		CSecID archiveID1 = (DWORD)CWK_Profile::GetHexFromString(sData, _T("\\AR"), SECID_NO_ID);
		CSecID archiveID2 = (DWORD)CWK_Profile::GetHexFromString(sData, _T("\\ARID"), SECID_NO_ID);
		CSecID idNotification = (DWORD)CWK_Profile::GetHexFromString(sData, _T("\\NM"), SECID_NO_ID);

		// Typ, Alarm, OK, Off
		DWORD dwState = CWK_Profile::GetHexFromString(sData, _T("\\S"), IO_NOSTATE);
		if (dwState==1) 
		{
			pAction->SetTypeOfActivation(TOA_INPUT_ALARM);
		}
		else if (dwState==0) 
		{
			pAction->SetTypeOfActivation(TOA_INPUT_OK);
		}
		else if (dwState==0xffffffff) 
		{
			// OFF
			WK_DELETE(pAction);
			return TRUE;	// EXIT
		} 
		else 
		{
			// give up
			WK_TRACE_ERROR(_T("unknown state %x in process macro for input %s, %d\n"), 
					dwState,
					(LPCTSTR)oneInput.GetName(),
					ioNr);
			WK_DELETE(pAction);
			return TRUE;	// EXIT
		}

		// Priorität
		DWORD dwPriority = CWK_Profile::GetHexFromString(sData, _T("\\P"), IO_DEFAULTPRIOR);
		if (dwPriority >=1 && dwPriority<=NUM_PRIORITIES) 
		{
			dwPriority--;	// convert to 0..NUM_PRIORITIES-1
		} 
		else 
		{
			WK_TRACE_ERROR(_T("Invalid priority %d, using %d instead\n"),dwPriority,NUM_PRIORITIES-1);
			dwPriority = NUM_PRIORITIES-1;
		}
		pAction->SetPriority(dwPriority);
		pAction->m_idTimer = (DWORD)CWK_Profile::GetHexFromString(sData, _T("\\T"), SECID_NO_ID);
		pAction->m_idProcessMacro = (DWORD)CWK_Profile::GetHexFromString(sData, _T("\\M"), SECID_NO_ID);
		pAction->m_pProcessMacro = theApp.GetProcessMacros().GetMacroByID(pAction->m_idProcessMacro);
		if (pAction->m_idProcessMacro == SECID_PROCESS_PRESET)
		{
			CameraControlCmd ccc = (CameraControlCmd)CWK_Profile::GetHexFromString(sData, _T("\\CCC"), CCC_INVALID);
			pAction->SetCameraControlCommand(ccc);
		}
		if (archiveID1 == SECID_NO_ID)
		{
			if (pAction->m_pProcessMacro)
			{
				const BYTE* pArchivIDs = pAction->m_pProcessMacro->GetArchiveIDs();
				if (pArchivIDs)
				{
					archiveID1.SetSubID(pArchivIDs[0]);
				}
			}
		}
		pAction->SetArchiveID(archiveID1);
		pAction->SetNotificationID(idNotification);

		if (pAction->m_pProcessMacro) 
		{	
			// valid macro
			TypeOfActivation toa = pAction->GetTypeOfActivation();
			if (   toa == TOA_INPUT_OK 
				&& pAction->GetMacro().IsKeepAlive()) 
			{
				WK_TRACE_ERROR(_T("Invalid combination keepAlive/OK for input %s, macro %s\n"),
						(LPCTSTR)oneInput.GetName(),
						(LPCTSTR)pAction->m_pProcessMacro->GetName()
						);
					CString sMsg;
					sMsg.Format(_T(">>>Error: volle Zeitdauer halten bei OK auf Eingang %s Macro %s"),
						(LPCTSTR)oneInput.GetName(),
						(LPCTSTR)pAction->m_pProcessMacro->GetName()
						);
					UpdateProcessListView(sMsg);
			} 
			else 
			{
				// output debug info
				if (   toa == TOA_INPUT_ALARM 
					|| toa == TOA_INPUT_OK) 
				{
					CString sMsg;
					CString sTimer=_T("none");
					CString sState;
					
					if (toa==TOA_INPUT_ALARM) 
					{
						sState=_T("ALARM");
					}
					else 
					{
						sState=_T("OK");
					}
					
					if (pAction->m_idTimer!=SECID_NO_ID) 
					{
						const CSecTimer *pTimer = NULL;
						pTimer = theApp.GetTimers().GetTimerByID(pAction->m_idTimer);
						if (pTimer) 
						{
							sTimer = pTimer->GetName();
						} 
						else 
						{
							WK_TRACE_ERROR(_T("Timer %x not found\n"),pAction->m_idTimer.GetID());
						}
					}
					
					CString sOutputName;

					// NOT YET const COutput *pOutput = theApp.pAction->m_idOutput.GetID(),

					sOutputName.Format(_T("%x"),pAction->GetOutputID().GetID());


					sMsg.Format(_T("ACTIVATION input %s on %s prio. %d macro %s cam %s timer %s"),
								(LPCTSTR)pAction->GetInput()->GetName(),
								(LPCTSTR)sState,
								pAction->GetPriority()+1,
								(LPCTSTR)pAction->GetMacro().GetName(),
								sOutputName,
								(LPCTSTR)sTimer);
					WK_TRACE(_T("%s\n"),(LPCTSTR)sMsg);
				}
				// and the real action (makes a copy of action)
				oneInput.AddAction(pAction);
			}
		} 
		else 
		{
			// unknown macro 
			WK_TRACE_ERROR(_T("Unknown ProcessMacro %x for input %s, activation %d\n"),
					pAction->m_idProcessMacro.GetID(),
					(LPCTSTR)oneInput.GetName(),
					ioNr
					);
			WK_DELETE(pAction);
		}
		// keep on reading
		return TRUE;
	}
	// nothing found
	return FALSE;
}


CIPCInputServer* CInputList::GetGroupByID(CSecID id) const
{
	for (int i=0;i<GetSize();i++) {
		if (id.GetGroupID()==GetGroupAt(i)->GetGroupID()) {
			return (GetGroupAt(i));
		}
	}
	return (NULL);
}
CIPCInputServer* CInputList::GetGroupByShmName(const CString& shmName) const
{
	for (int i=0;i<GetSize();i++) 
	{
		CIPCInputServer* pInputGroup = GetGroupAt(i);
		//TRACE(_T("IG %s == %s\n"),shmName,pInputGroup->GetShmName());
		if (   pInputGroup
			&& shmName==pInputGroup->GetShmName()) 
		{
			return pInputGroup;
		}
	}
	return NULL;
}
CInput* CInputList::GetInputByID(CSecID id) const
{
	// NOT YET some faster search
	for (int i=0;i<GetSize();i++) {
		if (id.GetGroupID()==GetGroupAt(i)->GetGroupID()) {
			return ( GetGroupAt(i)->GetInputAtWritable(id.GetSubID()) );
		}
	}
	return NULL;
}


