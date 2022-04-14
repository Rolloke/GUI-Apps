/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: activationpage.cpp $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/activationpage.cpp $
// CHECKIN:		$Date: 24.08.06 13:05 $
// VERSION:		$Revision: 95 $
// LAST CHANGE:	$Modtime: 24.08.06 13:04 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "systemverwaltung.h"

#include "SVDoc.h"
#include "SVView.h"

#include "InputGroup.h"
#include "OutputGroup.h"

#include "InputList.h"
#include "OutputList.h"
#include "ProcessList.h"
#include "ActivationPage.h"
#include "ArchivPage.h"
#include "SelectInputDlg.h"
#include "SecTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// promoto to 1998

/////////////////////////////////////////////////////////////////////////////
int CALLBACK CActivationPage::MyCompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CActivationPage *pPage = (CActivationPage *)lParamSort;

	CInputToOutput *pA = (CInputToOutput *)lParam1;	// input from MFC (item data)
	CInputToOutput *pB = (CInputToOutput *)lParam2;	// input from MFC (item data)

	// get according strings
	CString sInputA = pA->GetInput()->GetName();
	CString sInputB = pB->GetInput()->GetName();

	CString sOutputA;
	CString sOutputB;
	sOutputA.LoadString(IDS_NOTHING);
	sOutputB.LoadString(IDS_NOTHING);
	const COutput* pOutputA = pPage->GetOutputList()->GetOutputByID(pA->GetOutputOrMediaID());
	if (pOutputA) {
		sOutputA = pOutputA->GetName();
	}
	const COutput* pOutputB = pPage->GetOutputList()->GetOutputByID(pB->GetOutputOrMediaID());
	if (pOutputB) {
		sOutputB = pOutputB->GetName();
	}

	CString sProcessA,sProcessB,sDestA,sDestB;
	const CProcess* pProcessA = pPage->GetProcessList()->GetProcess(pA->GetProcessID().GetID());
	if (pProcessA) 
	{
		sProcessA = pProcessA->GetName();
		if (pProcessA->IsSave())
		{
			CSecID archiveIDA =  pA->GetArchiveID();
			CArchivInfo *pArchive = pPage->m_pArchivList->GetArchivInfo(archiveIDA);
			if (pArchive) 
			{
				sDestA = pArchive->GetName();
			} 
		}
		else if (pProcessA->IsCall())
		{
			CSecID id =  pA->GetArchiveID();
			CNotificationMessage *pNM = pPage->m_pNotificationList->GetNotificationMessage(id);
			if (pNM) 
			{
				sDestA = pNM->GetName();
			} 
		}
	}
	const CProcess* pProcessB = pPage->GetProcessList()->GetProcess(pB->GetProcessID().GetID());
	if (pProcessB) 
	{
		sProcessB = pProcessB->GetName();
		if (pProcessB->IsSave())
		{
			CSecID id =  pB->GetArchiveID();
			CArchivInfo *pArchive = pPage->m_pArchivList->GetArchivInfo(id);
			if (pArchive) 
			{
				sDestB = pArchive->GetName();
			} 
		}
		else if (pProcessB->IsCall())
		{
			CSecID id =  pB->GetArchiveID();
			CNotificationMessage *pNM = pPage->m_pNotificationList->GetNotificationMessage(id);
			if (pNM) 
			{
				sDestB = pNM->GetName();
			} 
		}
	}

	CString sTimerA;
	CString sTimerB;
	sTimerA.LoadString(IDS_NOTIMER);
	sTimerB.LoadString(IDS_NOTIMER);
	const CSecTimer* pTimerA = pPage->GetTimerList()->GetTimerByID(pA->GetTimerID());
	if (pTimerA) {
		sTimerA = pTimerA->GetName();
	}
	const CSecTimer* pTimerB = pPage->GetTimerList()->GetTimerByID(pB->GetTimerID().GetID());
	if (pTimerB) {
		sTimerB = pTimerB->GetName();
	}


	/*
	The comparison function must return 
		a negative value if the first item should precede the second, 
		a positive value if the first item should follow the second, 
		or zero if the two items are equivalent.
	*/
	// switch sort style by column
	int cmp=0;
	switch (pPage->m_iSelectedColumn) {
		case 1:	// state input output timer proc prio
		cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
		if (cmp==0) {
			cmp = sInputA.CompareNoCase(sInputB); // input 
			if (cmp==0) {
				cmp = sOutputA.CompareNoCase(sOutputB); // output 
				if (cmp==0) {
					cmp = sTimerA.CompareNoCase(sTimerB); // timer
					if (cmp==0) {
						cmp = sProcessA.CompareNoCase(sProcessB); // process
						if (cmp==0) {
							cmp = sDestA.CompareNoCase(sDestB); // archive names
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}
						}
					}
				}
			}
		}
		break;
		//////////////////////////////
		case 2:	// timer, input, output, state, proc, prio
		cmp = sTimerA.CompareNoCase(sTimerB); // timer
		if (cmp==0) {
			cmp = sInputA.CompareNoCase(sInputB); // input 
			if (cmp==0) {
				cmp = sOutputA.CompareNoCase(sOutputB); // output 
				if (cmp==0) {
					cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
					if (cmp==0) {
						cmp = sProcessA.CompareNoCase(sProcessB); // process
						if (cmp==0) {
							cmp = sDestA.CompareNoCase(sDestB); // archive names
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}

						}
					}
				}
			}
		}
		break;
		//////////////////////////////
		case 3:	// output, input, state, timer, proc, prio
		// inputName, ouputName, state, timerName, priority
		cmp = sOutputA.CompareNoCase(sOutputB); // output 
		if (cmp==0) {
			cmp = sInputA.CompareNoCase(sInputB); // input 
			if (cmp==0) {
				cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
				if (cmp==0) {
					cmp = sTimerA.CompareNoCase(sTimerB); // timer
					if (cmp==0) {
						cmp = sProcessA.CompareNoCase(sProcessB); // process
						if (cmp==0) {
							cmp = sDestA.CompareNoCase(sDestB); // archive names
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}
						}
					}
				}
			}
		}
		break;
		//////////////////////////////
		case 4:	// Prozesse, inputName, outputName, state, timer, prio
			cmp = sProcessA.CompareNoCase(sProcessB); // process
			if (cmp==0) {
				cmp = sInputA.CompareNoCase(sInputB); // input 
				if (cmp==0) {
					cmp = sOutputA.CompareNoCase(sOutputB); // output 
					if (cmp==0) {
						cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
						if (cmp==0) {
							cmp = sTimerA.CompareNoCase(sTimerB); // timer
							if (cmp==0) {
								cmp = sDestA.CompareNoCase(sDestB); // archive names
								if (cmp==0) {
									cmp = pA->GetPriority() - pB->GetPriority();	// prio
								}
							}
						}
					}
				}
			}
		break;
		//////////////////////////////
		case 5:	// prio input output state timer process
		cmp = pA->GetPriority() - pB->GetPriority();	// prio
		if (cmp==0) {
			cmp = sInputA.CompareNoCase(sInputB); // input 
			if (cmp==0) {
				cmp = sOutputA.CompareNoCase(sOutputB); // output 
				if (cmp==0) {
					cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
					if (cmp==0) {
						cmp = sTimerA.CompareNoCase(sTimerB); // timer
						if (cmp==0) {
							cmp = sDestA.CompareNoCase(sDestB); // archive names
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}
						}
					}
				}
			}
		}
		break;
		//////////////////////////////
		case 6:	// sort by archives first
		cmp = sDestA.CompareNoCase(sDestB); // archive names
		if (cmp==0) {
			cmp = sOutputA.CompareNoCase(sOutputB); // output 
			if (cmp==0) {
				cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
				if (cmp==0) {
					cmp = sTimerA.CompareNoCase(sTimerB); // timer
					if (cmp==0) {
						cmp = sProcessA.CompareNoCase(sProcessB); // process
						if (cmp==0) {
							cmp = sInputA.CompareNoCase(sInputB); // input 
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}
						}
					}
				}
			}
		}
		break;
		default:
		// inputName, ouputName, state, timerName, priority
		cmp = sInputA.CompareNoCase(sInputB); // input 
		if (cmp==0) {
			cmp = sOutputA.CompareNoCase(sOutputB); // output 
			if (cmp==0) {
				cmp = (int) (pA->GetInputState() - pB->GetInputState());	// state
				if (cmp==0) {
					cmp = sTimerA.CompareNoCase(sTimerB); // timer
					if (cmp==0) {
						cmp = sProcessA.CompareNoCase(sProcessB); // process
						if (cmp==0) {
							cmp = sDestA.CompareNoCase(sDestB); // archive names
							if (cmp==0) {
								cmp = pA->GetPriority() - pB->GetPriority();	// prio
							}
						}
					}
				}
			}
		}
	}	// end of column switch

	return cmp;
}
CString CameraControlCmd2String(CameraControlCmd ccc)
{
	CString s;
	switch (ccc)
	{
	case CCC_POS_1:
		s.Format(IDS_PTZ_POSITION,1);
		break;
	case CCC_POS_2:
		s.Format(IDS_PTZ_POSITION,2);
		break;
	case CCC_POS_3:
		s.Format(IDS_PTZ_POSITION,3);
		break;
	case CCC_POS_4:
		s.Format(IDS_PTZ_POSITION,4);
		break;
	case CCC_POS_5:
		s.Format(IDS_PTZ_POSITION,5);
		break;
	case CCC_POS_6:
		s.Format(IDS_PTZ_POSITION,6);
		break;
	case CCC_POS_7:
		s.Format(IDS_PTZ_POSITION,7);
		break;
	case CCC_POS_8:
		s.Format(IDS_PTZ_POSITION,8);
		break;
	case CCC_POS_9:
		s.Format(IDS_PTZ_POSITION,9);
		break;
	case CCC_POS_HOME:
		s = _T("Position Home");
		break;
	default:
		s.Empty();
		break;
	}

	return s;
}

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CActivationPage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
// CActivationPage property page
CActivationPage::CActivationPage(CSVView* pParent) : CSVPage(CActivationPage::IDD)
{
	//{{AFX_DATA_INIT(CActivationPage)
	//}}AFX_DATA_INIT
	m_pParent			= pParent;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pTimerList		= pParent->GetDocument()->GetTimers();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_pProcessList		= pParent->GetDocument()->GetProcesses();
	m_pArchivList		= pParent->GetDocument()->GetArchivs();
	m_pNotificationList = pParent->GetDocument()->GetNotifications();
	m_pMediaList		= pParent->GetDocument()->GetMedias();
	m_bEnableAudio		= GetDongle().RunAudioUnit();

	m_iSelectedItem		= -1;
	m_pSelectedITO		= NULL;
	m_iSelectedColumn	= 0;	// used for sort

	m_bWeAreDeletingDoNotSelect = FALSE;

	Create(IDD,(CWnd*)m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
CActivationPage::~CActivationPage()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActivationPage::IsDataValid()
{
	CInputToOutput* pIO = NULL;
	CProcess* pProcess = NULL;
	const COutput* pOutput = NULL;
	const CMedia* pMedia = NULL;
	BOOL bIsRelayProcess = FALSE;
	BOOL bIsAudioProcess = FALSE;
	BOOL bIsPTZProcess = FALSE;
	BOOL bIsAlarmListProcess = FALSE;

	CSecID id;

	// check the activations one by one
	int iCount = m_InputsToOutputs.GetItemCount();
	for (int i=0 ; i<iCount ; i++) 
	{
		// reset temporary pointers and variables
		pProcess = NULL;
		pOutput = NULL;
		bIsRelayProcess = FALSE;
		bIsAudioProcess = FALSE;
		// first: is there really an activation behind
		pIO = (CInputToOutput*)m_InputsToOutputs.GetItemData(i);
	
		if (pIO==NULL) {
			// no one behind, next one please
			continue;
		}

		id = pIO->GetOutputOrMediaID();

		// second: check for output, no output is allowed
		if (id.IsOutputID()) 
		{
			pOutput = m_pOutputList->GetOutputByID(id);
		}
		if (id.IsMediaID())
		{
			pMedia = m_pMediaList->GetMediaByID(id);
		}
		
		// 3. check for process
		if (pIO->GetProcessID().IsProcessID())
		{
			pProcess = m_pProcessList->GetProcess(pIO->GetProcessID().GetID());
			if (NULL == pProcess) 
			{
				// can be relay process
				if ((pIO->GetProcessID() == SECID_PROCESS_RELAY_EDGE_CONTROL) ||
					(pIO->GetProcessID() == SECID_PROCESS_RELAY_STATE_CONTROL))
				{
					bIsRelayProcess = TRUE;
				}
				else if (pIO->GetProcessID() == SECID_PROCESS_AUDIO)
				{
					bIsAudioProcess = TRUE;
				}
				else if (pIO->GetProcessID() == SECID_PROCESS_PRESET)
				{
					bIsPTZProcess = TRUE;
				}
				else if (pIO->GetProcessID() == SECID_PROCESS_ALARM_LIST)
				{
					bIsAlarmListProcess = TRUE;
				}
			}
		}
		// no process is not allowed
		if (   NULL == pProcess 
			&& !bIsRelayProcess
			&& !bIsAudioProcess
			&& !bIsPTZProcess
			&& !bIsAlarmListProcess) 
		{
			AfxMessageBox(IDP_ACTIVATION_WITHOUT_PROCESS);
			SelectActivation(i);
			m_InputsToOutputs.SetFocus();
			return FALSE;
		}


		// 4. check for correct output/process combination
		if (pOutput) 
		{
			if (pOutput->IsRelay()) 
			{
				if (!bIsRelayProcess) 
				{
					AfxMessageBox(IDP_ACTIVATION_RELAY_WP);
					SelectActivation(i);
					m_InputsToOutputs.SetFocus();
					return FALSE;
				}
			}
			else if (pOutput->IsCamera()) 
			{
				if (bIsRelayProcess) 
				{
					AfxMessageBox(IDP_ACTIVATION_CAMERA_WP);
					SelectActivation(i);
					m_InputsToOutputs.SetFocus();
					return FALSE;
				}
			}
		}
		else if (pMedia)
		{
		}
		else if (!bIsAlarmListProcess)
		{
			// no output, no save or relay process allowed
			if ( bIsRelayProcess || pProcess->IsSave() ) 
			{
				AfxMessageBox(IDP_ACTIVATION_NO_OUTPUT_OR_WRONG_PROCESS);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
		}

		// 5.	if status OK:
		//			no Keep Alive, 
		//			no ISDN,
		//			no Backup
		if ( (pIO->GetInputState()==IO_OK) && pProcess) 
		{
			if (pProcess->IsCall()) 
			{
				AfxMessageBox(IDP_ACTIVATION_OK_ISDN);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
			if (pProcess->IsBackup()) 
			{
				AfxMessageBox(IDP_ACTIVATION_OK_BACKUP);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
			if (pProcess->IsHold()) 
			{
				AfxMessageBox(IDP_ACTIVATION_OK_KEEPALIVE);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
			if (pProcess->IsPTZPreset())
			{
				AfxMessageBox(IDP_ACTIVATION_OK_PTZ);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
			if (pProcess->IsAlarmList())
			{
				AfxMessageBox(IDP_ACTIVATION_OK_ALARMLIST);
				SelectActivation(i);
				m_InputsToOutputs.SetFocus();
				return FALSE;
			}
		}

		// 6. save process needs archiv
		if (pProcess && pProcess->IsSave() && (pIO->GetArchiveID() == SECID_NO_ID)) 
		{
			AfxMessageBox(IDP_ACTIVATION_WITHOUT_ARCHIV);
			SelectActivation(i);
			m_InputsToOutputs.SetFocus();
			return FALSE;
		}

		// 7. MD nur mit volle Zeitdauer halten
		if (   pProcess
			&& pProcess->IsSave()
			&& !pProcess->IsHold())
		{
			CInput* pInput = pIO->GetInput();
			if (pInput)
			{
				CInputGroup* pInputGroup = m_pInputList->GetGroupByID(pInput->GetID());
				if (   pInputGroup
					&& pInputGroup->IsMD())
				{
					AfxMessageBox(IDP_ACTIVATION_MD_WITHOUT_HOLD);
					SelectActivation(i);
					m_InputsToOutputs.SetFocus();
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::ShowHideAdditional(BOOL bEnable,CProcess::ProcessType typ, CSecID idArchiv)
{
	m_txtNotification.ShowWindow((bEnable && typ==CProcess::P_CALL) ? SW_SHOW : SW_HIDE);
	m_Notifications.ShowWindow((bEnable && typ==CProcess::P_CALL) ? SW_SHOW : SW_HIDE);
	m_Notifications.EnableWindow(bEnable && typ==CProcess::P_CALL);

	if (   typ == CProcess::P_SAVE
		|| typ == CProcess::P_ALARM_LIST)
	{
		if (bEnable)
		{
			m_txtArchiv.ShowWindow(SW_SHOW);
			m_Archivs.ShowWindow(SW_SHOW);
			m_Archivs.EnableWindow(TRUE);
			FillArchivs(typ == CProcess::P_ALARM_LIST);
			//m_Archivs.Set
			m_Archivs.SetRadioItemFromlParam(idArchiv.GetID());
		}
		else
		{
			m_txtArchiv.ShowWindow(SW_HIDE);
			m_Archivs.ShowWindow(SW_HIDE);
			m_Archivs.EnableWindow(FALSE);
		}
	}
	else
	{
		m_txtArchiv.ShowWindow(SW_HIDE);
		m_Archivs.ShowWindow(SW_HIDE);
		m_Archivs.EnableWindow(bEnable);
	}


	m_txtPTZPreset.ShowWindow((bEnable && typ==CProcess::P_PTZ_PRESET) ? SW_SHOW : SW_HIDE);
	m_PTZPresets.ShowWindow((bEnable && typ==CProcess::P_PTZ_PRESET) ? SW_SHOW : SW_HIDE);
	m_PTZPresets.EnableWindow(bEnable && typ==CProcess::P_PTZ_PRESET);
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::EnableExceptNew(BOOL bAdditional /*=TRUE*/)
{
	BOOL bEnable;
	int c;
	c = m_InputsToOutputs.GetItemCount();
	bEnable = (c>0);
	
	m_InputsToOutputs.EnableWindow(bEnable);
	m_Timers.EnableWindow(bEnable);
	m_Processes.EnableWindow(bEnable);
	m_Outputs.EnableWindow(bEnable);
	if (bAdditional)
	{
		CSecID idArchive;
		CProcess::ProcessType type = CProcess::P_NOTYPE;
		if (m_pSelectedITO)
		{
			CProcess* pProcess = m_pProcessList->GetProcess(m_pSelectedITO->GetProcessID());
			if (pProcess)
			{
				type = pProcess->GetType();
			}
			idArchive = m_pSelectedITO->GetArchiveID();
		}
		ShowHideAdditional(bEnable, type, idArchive);
	}

	m_radioOFF.EnableWindow(bEnable);
	m_radioOK.EnableWindow(bEnable);
	m_radioALARM.EnableWindow(bEnable);

	m_radioPriority1.EnableWindow(bEnable);
	m_radioPriority2.EnableWindow(bEnable);
	m_radioPriority3.EnableWindow(bEnable);
	m_radioPriority4.EnableWindow(bEnable);
	m_radioPriority5.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::SelectedItemToControl()
{
	if (m_pSelectedITO==NULL)
	{
		// status
		m_radioOFF.SetCheck(1);
		m_radioOK.SetCheck(0);
		m_radioALARM.SetCheck(0);
		// priority
		m_radioPriority1.SetCheck(0);
		m_radioPriority2.SetCheck(0);
		m_radioPriority3.SetCheck(1);
		m_radioPriority4.SetCheck(0);
		m_radioPriority5.SetCheck(0);
		// timer
		m_Timers.SetRadioItem(0);
		// output
		m_Outputs.SetRadioItem(0);
		// process
		FillProcesses(NULL,NULL);
		m_Processes.SetRadioItem(-1);
		// additional
		m_Archivs.SetRadioItem(-1);
		m_Notifications.SetRadioItem(-1);
		m_PTZPresets.SetRadioItem(-1);
		UpdateData(FALSE);
	}
	else
	{
		const COutput* pOutput = NULL;
		const CMedia* pMedia = NULL;

		// set the input name
		CString sName = m_pSelectedITO->GetInput()->GetName();
		m_InputsToOutputs.SetItemText(m_iSelectedItem,0,sName);

		// set the status
		m_radioOFF.SetCheck(m_pSelectedITO->GetInputState()==IO_NOSTATE);
		m_radioOK.SetCheck(m_pSelectedITO->GetInputState()==IO_OK);
		m_radioALARM.SetCheck(m_pSelectedITO->GetInputState()==IO_ALARM);
		CString sState;
		switch (m_pSelectedITO->GetInputState())
		{
		case IO_NOSTATE:
			sState.LoadString(IDS_OFF);
			break;
		case IO_OK:
			sState.LoadString(IDS_OK);
			break;
		case IO_ALARM:
			sState.LoadString(IDS_ALARM);
			break;
		}
		m_InputsToOutputs.SetItemText(m_iSelectedItem,1,sState);

		// set priority
		m_radioPriority1.SetCheck(m_pSelectedITO->GetPriority()==1);
		m_radioPriority2.SetCheck(m_pSelectedITO->GetPriority()==2);
		m_radioPriority3.SetCheck(m_pSelectedITO->GetPriority()==3);
		m_radioPriority4.SetCheck(m_pSelectedITO->GetPriority()==4);
		m_radioPriority5.SetCheck(m_pSelectedITO->GetPriority()==5);
		CString sPriority;
		sPriority.Format(_T("%d"),m_pSelectedITO->GetPriority());
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,sPriority);

		// set the timer
		const CSecTimer* pTimer = m_pTimerList->GetTimerByID(m_pSelectedITO->GetTimerID());
		CString sTimer;
		if (pTimer) 
		{
			sTimer = pTimer->GetName();
			m_Timers.SetRadioItemFromlParam((LPARAM)m_pSelectedITO->GetTimerID().GetID());	
		}
		else 
		{
			sTimer.LoadString(IDS_NOTIMER);
			m_Timers.SetRadioItemFromlParam(SECID_NO_ID);	
		}
		m_InputsToOutputs.SetItemText(m_iSelectedItem,2,sTimer);

		// camera / relais
		pOutput = m_pOutputList->GetOutputByID(m_pSelectedITO->GetOutputOrMediaID());
		CString sOutput;
		if (pOutput)
		{
			m_Outputs.SetRadioItemFromlParam((LPARAM)m_pSelectedITO->GetOutputOrMediaID().GetID());
			sOutput = pOutput->GetName();
		}
		else
		{
			pMedia = m_pMediaList->GetMediaByID(m_pSelectedITO->GetOutputOrMediaID());
			if (pMedia)
			{
				m_Outputs.SetRadioItemFromlParam((LPARAM)m_pSelectedITO->GetOutputOrMediaID().GetID());
				sOutput = pMedia->GetName();
			}
			else
			{
				m_Outputs.SetRadioItem(0);
				sOutput.LoadString(IDS_NOTHING);
			}
		}
		m_InputsToOutputs.SetItemText(m_iSelectedItem,3,sOutput);

		// process
		// first fill listbox
		FillProcesses(pOutput,pMedia);
		UpdateProcessSettings();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::SelectActivation(int i, BOOL bSetSel/* = TRUE*/)
{
	// DO NOT check i==m_iSelectedItem, if a new item is inserted
	// the first index might need a refresh

	if (i>=0)
	{
		m_iSelectedItem = i;
		if (m_pSelectedITO)
		{
			// data from controls to InputToOutput
			m_pSelectedITO->SetTimerID((DWORD)m_Timers.GetRadioItemData());
			m_pSelectedITO->SetProcessID((DWORD)m_Processes.GetRadioItemData());
			m_pSelectedITO->SetOutputID((DWORD)m_Outputs.GetRadioItemData());

			if (m_radioOFF.GetCheck()==1)
				m_pSelectedITO->SetInputState(IO_NOSTATE);
			else if (m_radioOK.GetCheck()==1)
				m_pSelectedITO->SetInputState(IO_OK);
			else if (m_radioALARM.GetCheck()==1)
				m_pSelectedITO->SetInputState(IO_ALARM);

			if (m_radioPriority1.GetCheck()==1)
				m_pSelectedITO->SetPriority(1);
			else if (m_radioPriority2.GetCheck()==1)
				m_pSelectedITO->SetPriority(2);
			else if (m_radioPriority3.GetCheck()==1)
				m_pSelectedITO->SetPriority(3);
			else if (m_radioPriority4.GetCheck()==1)
				m_pSelectedITO->SetPriority(4);
			else if (m_radioPriority5.GetCheck()==1)
				m_pSelectedITO->SetPriority(5);

		}//if

		
		if (bSetSel)
		{
			m_InputsToOutputs.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			// Selected item shall be visible
			m_InputsToOutputs.EnsureVisible(m_iSelectedItem, FALSE);
		}

		m_pSelectedITO = (CInputToOutput*)m_InputsToOutputs.GetItemData(m_iSelectedItem);

		SelectedItemToControl();

	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedITO = NULL;
		SelectedItemToControl();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::CancelChanges()
{
	ClearActivations();	

	m_pInputList->Reset();
	m_pInputList->Load(GetProfile());

	FillActivations();
	EnableExceptNew();

	int	c = m_InputsToOutputs.GetItemCount();
	SelectActivation((c>0) ? 0 : -1);

	m_InputsToOutputs.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::Initialize()
{
	FillActivations();
	FillOutputs();
	FillTimers();
	FillProcesses(NULL,NULL);
	FillArchivs(FALSE);
	FillNotifications();
	FillPTZPresets();

	EnableExceptNew();

	m_iSelectedColumn = 0;	// default sortiing
	m_InputsToOutputs.SortItems(MyCompareFunction , (DWORD) this);
  
	int	c = m_InputsToOutputs.GetItemCount();
	SelectActivation((c>0) ? 0 : -1);

	m_InputsToOutputs.SetFocus();
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::SaveChanges()
{
	WK_TRACE_USER(_T("Aktivierungen wurden geändert\n"));
	m_pInputList->Save(GetProfile(),FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CActivationPage::ClearActivations()
{
	m_InputsToOutputs.DeleteAllItems();
	m_iSelectedItem = -1;
	m_pSelectedITO = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillActivations() 
{
	ClearActivations();
	CInputGroup* pInputGroup = NULL;
	CInput* pInput = NULL;
	// Schleife ueber alle Inputgroups
	for (int ia=0 ; ia<m_pInputList->GetSize() ; ia++) 
	{
		pInputGroup = m_pInputList->GetGroupAt(ia);
		// Schleife ueber alle Eingaenge der Gruppe
		for ( int ib=0 ; ib<pInputGroup->GetSize() ; ib++) 
		{
			// Input nur anzeigen, wenn aktiv ist
			pInput = pInputGroup->GetInput(ib);
			if ( pInput && pInput->GetIDActivate() != SECID_ACTIVE_OFF ) 
			{
				// Schleife ueber alle Aktivierungen dieses Eingangs
				for (int ic=0 ; ic<pInput->GetNumberOfInputToOutputs() ; ic++) 
				{
					CInputToOutput* pInputToOutput = pInput->GetInputToOutput(ic);
					if ( ( (   pInputToOutput->GetOutputOrMediaID().IsOutputID() 
							|| pInputToOutput->GetOutputOrMediaID().IsMediaID()
						    || pInputToOutput->GetOutputOrMediaID()==SECID_NO_ID)
							&& (   pInputToOutput->GetProcessID().IsProcessID() 
							    || pInputToOutput->GetProcessID()==SECID_NO_ID)))
					{
						InsertActivation(pInput, pInputToOutput);
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
int CActivationPage::InsertActivation(const CInput* pInput, CInputToOutput* pInputToOutput)
{
	LV_ITEM lvis;
	CString s;

	// Am Ende einfuegen, daher ListControl-Count holen
	int iCount = m_InputsToOutputs.GetItemCount();

	s = pInput->GetName();

	// Item einfuegen
	lvis.mask = LVIF_TEXT | LVIF_PARAM;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.lParam = (LPARAM)pInputToOutput;
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	int iIndex = m_InputsToOutputs.InsertItem(&lvis);
	s.ReleaseBuffer();
	// Eingangsstatus setzen
	if (pInputToOutput->GetInputState() == IO_OK) 
	{
		s.LoadString(IDS_OK);
	}
	else if (pInputToOutput->GetInputState() == IO_ALARM)
	{
		s.LoadString(IDS_ALARM);
	}
	else
	{
		s.LoadString(IDS_OFF);
	}
	m_InputsToOutputs.SetItemText(iIndex,1,s);

	// Timer setzen
	CSecID idTimer = pInputToOutput->GetTimerID();
	const CSecTimer* pTimer = m_pTimerList->GetTimerByID(idTimer);
	if (pTimer) 
	{
		s = pTimer->GetName();
	}
	else 
	{
		s.LoadString(IDS_NOTIMER);
	}
	m_InputsToOutputs.SetItemText(iIndex,2,s);

	// Ausgang setzen
	CSecID id = pInputToOutput->GetOutputOrMediaID();
	const COutput* pOutput = m_pOutputList->GetOutputByID(id);
	const CMedia* pMedia = m_pMediaList->GetMediaByID(id);
	if (pOutput) 
	{
		s = pOutput->GetName();
	}
	else if (pMedia)
	{
		s = pMedia->GetName();
	}
	else
	{
		s.LoadString(IDS_NOTHING);
	}
	m_InputsToOutputs.SetItemText(iIndex,3,s);

	// Prozess setzen
	CSecID idProcess = pInputToOutput->GetProcessID();
	CProcess* pProcess = m_pProcessList->GetProcess(idProcess.GetID());
	CString sNewDestination;
	
	if (pProcess) 
	{
		s = pProcess->GetName();
	}
	else 
	{
		if (idProcess == SECID_PROCESS_RELAY_EDGE_CONTROL)
		{
			s.LoadString(IDS_EDGECONTROL);
		}
		else if (idProcess == SECID_PROCESS_RELAY_STATE_CONTROL)
		{
			s.LoadString(IDS_LEVELCONTROL);
		}
		else if (idProcess == SECID_PROCESS_AUDIO)
		{
			s.LoadString(IDS_AUDIO_PROCESS);
			CSecID archiveID = pInputToOutput->GetArchiveID();
				
			if (archiveID!=SECID_NO_ID) 
			{
				CArchivInfo *pArchive = m_pArchivList->GetArchivInfo(archiveID);
				if (pArchive) 
				{
					sNewDestination = pArchive->GetName();
				}
			}
		}
		else if (idProcess == SECID_PROCESS_ALARM_LIST)
		{
			s.LoadString(IDS_ALARM_LIST);
			CSecID archiveID = pInputToOutput->GetArchiveID();

			if (archiveID!=SECID_NO_ID) 
			{
				CArchivInfo *pArchive = m_pArchivList->GetArchivInfo(archiveID);
				if (pArchive) 
				{
					sNewDestination = pArchive->GetName();
				}
			}
		}
		else if (idProcess == SECID_PROCESS_PRESET)
		{
			s.LoadString(IDS_PTZ_PRESET);
			sNewDestination = CameraControlCmd2String(pInputToOutput->GetPTZPreset());
		}
		else
		{
			s.Empty();
		}
	}
	m_InputsToOutputs.SetItemText(iIndex,4,s);

	// Prioritaet setzen
	s.Format(_T("%u"), pInputToOutput->GetPriority());
	m_InputsToOutputs.SetItemText(iIndex,5,s);

	if (pProcess)
	{
		if (pProcess->IsSave())
		{
			// HEDU 011098
			CSecID archiveID= pInputToOutput->GetArchiveID();
				
			if (archiveID!=SECID_NO_ID) 
			{
				CArchivInfo *pArchive = m_pArchivList->GetArchivInfo(archiveID);
				if (pArchive) 
				{
					sNewDestination = pArchive->GetName();
				}
				else 
				{
					// archive not found or non archive destination
				}
			}
		}
		else if (pProcess->IsCall())
		{
			// HEDU 011098
			CSecID id = pInputToOutput->GetNotificationID();
				
			if (id!=SECID_NO_ID) 
			{
				CNotificationMessage *pNM = m_pNotificationList->GetNotificationMessage(id);
				if (pNM) 
				{
					sNewDestination = pNM->GetName();
				}
				else 
				{
					// archive not found or non archive destination
				}
			}
		}
	}

	m_InputsToOutputs.SetItemText(iIndex,6,sNewDestination);

	return iIndex;
}

/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillTimers() 
{
	m_Timers.DeleteAllItems();

	InsertTimer(NULL);

	int iCount = m_pTimerList->GetSize();
	for (int j=0 ; j<iCount ; j++) {
		const CSecTimer* pTimer = m_pTimerList->GetAt(j);
		InsertTimer(pTimer);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CActivationPage::InsertTimer(const CSecTimer* pTimer)
{
	CString s;
	LV_ITEM lvis;

	lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;

	if ( pTimer ) 
	{
		s = pTimer->GetName();
		if ( s.IsEmpty() ) 
		{
			s.LoadString(IDS_NONAME);
		}
		lvis.lParam = pTimer->GetID().GetID();
		if (pTimer->GetLockIDs().GetSize())
		{
			lvis.iImage = IMAGE_TIMER_LOCK;
		}
		else
		{
			lvis.iImage = IMAGE_TIMER;
		}
	}
	else 
	{
		s.LoadString(IDS_NOTIMER);
		lvis.lParam = SECID_NO_ID;
		lvis.iImage = IMAGE_BLUE;
	}

	int iCount = m_Timers.GetItemCount();
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.stateMask=LVIS_STATEIMAGEMASK;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	m_Timers.InsertItem(&lvis);
	s.ReleaseBuffer();
}

/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillOutputs() 
{
	m_Outputs.DeleteAllItems();

	InsertOutput(NULL);
	int iMaxGroups = m_pOutputList->GetSize();
	int iCount;
	for (int iGroupNr=0 ; iGroupNr<iMaxGroups ; iGroupNr++)
	{
		COutputGroup* pGroup = m_pOutputList->GetGroupAt(iGroupNr);
		if (pGroup) 
		{
			iCount = pGroup->GetSize();
			for (int j=0 ; j<iCount ; j++) 
			{
				const COutput* pOutput = pGroup->GetOutput(j);
				if ( pOutput->GetExOutputType()!=EX_OUTPUT_OFF )
				{
					InsertOutput(pOutput);
				}
			}
		}
	}

	if (m_bEnableAudio)
	{
		int iMaxGroups = m_pMediaList->GetSize();
		for (iGroupNr=0;iGroupNr<iMaxGroups;iGroupNr++)
		{
			CMediaGroup* pGroup = m_pMediaList->GetGroupAt(iGroupNr);
			iCount = pGroup->GetSize();
			for (int j=0 ; j<iCount ; j++) 
			{
				const CMedia* pMedia = pGroup->GetMedia(j);
				if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
				{
					InsertMedia(pMedia);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CActivationPage::InsertOutput(const COutput* pOutput)
{
	CString s;
	if (pOutput)
	{
		s = pOutput->GetName();
	}
	if ( s.IsEmpty() ) 
	{
		s.LoadString(IDS_NOTHING);
	}

	LV_ITEM lvis;
	lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
	int iCount = m_Outputs.GetItemCount();
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	if (pOutput)
	{
		lvis.lParam = pOutput->GetID().GetID();
	}
	else
	{
		lvis.lParam = SECID_NO_ID;
	}
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	if (pOutput)
	{
		lvis.iImage = theApp.ImageByExType(pOutput->GetExOutputType());
	}
	else
	{
		lvis.iImage = IMAGE_NULL;
	}
	lvis.stateMask=LVIS_STATEIMAGEMASK;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	m_Outputs.InsertItem(&lvis);
	s.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::InsertMedia(const CMedia* pMedia)
{
	CString s;
	if (pMedia)
	{
		s = pMedia->GetName();
		LV_ITEM lvis;
		lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
		int iCount = m_Outputs.GetItemCount();
		lvis.iItem = iCount;
		lvis.iSubItem = 0;

		lvis.lParam = pMedia->GetID().GetID();

		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();
		lvis.iImage = IMAGE_AUDIO;
		if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
			lvis.iImage = IMAGE_AUDIO_IN;
		else if (pMedia->GetType() == SEC_MEDIA_TYPE_OUTPUT)
			lvis.iImage = IMAGE_AUDIO_OUT;

		lvis.stateMask=LVIS_STATEIMAGEMASK;
		lvis.state=INDEXTOSTATEIMAGEMASK(1);
		m_Outputs.InsertItem(&lvis);
		s.ReleaseBuffer();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillProcesses(const COutput* pOutput,const CMedia* pMedia) 
{
	m_Processes.DeleteAllItems();

	int iCount = m_pProcessList->GetSize();
	const CProcess* pProcess = NULL;
	for (int j=0 ; j<iCount ; j++) 
	{
		pProcess = m_pProcessList->GetAt(j);
		if (   pOutput 
			&& pOutput->IsCamera())
		{
			// allowed are: save, call
			if (   pProcess->IsSave() 
				|| pProcess->IsCall()
				|| pProcess->IsUpload())
			{
				InsertProcess(pProcess);
			}
		}
		else if (pOutput && pOutput->IsRelay())
		{
			if (pProcess->IsAlarmList())
			{
				continue;
			}
			// no relay processes in list,
			// see special case further down
		}
		else if (pMedia)
		{
			// no audio processes in list,
			// see special case further down
		}
		else
		{
			// no Output
			// allowed are: call, backup, actual image, video out
			if (   pProcess->IsCall() 
				|| pProcess->IsBackup()
				|| pProcess->IsActualImage()
				|| pProcess->IsImageCall()
				|| pProcess->IsCheckCall()
				|| pProcess->IsGuardTour()
				|| pProcess->IsVideoOut()
				) 
			{
				InsertProcess(pProcess);
			}
		}
	} // for

	CString s;
	CString sVersion = GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
	LV_ITEM lvis;
	BOOL bOutputRelais = FALSE;

	lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
	lvis.iSubItem = 0;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	lvis.stateMask=LVIS_STATEIMAGEMASK;
	if (pOutput)
	{
		if (pOutput->IsCamera())
		{
			BOOL bPTZ= FALSE;
#ifdef _DEBUG
			bPTZ = TRUE;
#else
			bPTZ = (sVersion >= _T("5.0.3"));
#endif
			lvis.iImage = IMAGE_PTZ_PRESET;
			// allowed are: only cameras
			// special case PTZ

			iCount = m_Processes.GetItemCount();
			s.LoadString(IDS_PTZ_PRESET);
			lvis.iItem = iCount;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			lvis.lParam = SECID_PROCESS_PRESET;
			m_Processes.InsertItem(&lvis);
			s.ReleaseBuffer();
		}
		else if (pOutput->IsRelay())
		{
			bOutputRelais = TRUE;
			lvis.iImage = IMAGE_RELAY;
			// allowed are: only relay
			// special case for relay

			iCount = m_Processes.GetItemCount();
			s.LoadString(IDS_EDGECONTROL);
			lvis.iItem = iCount;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			lvis.lParam = SECID_PROCESS_RELAY_EDGE_CONTROL;
			m_Processes.InsertItem(&lvis);
			s.ReleaseBuffer();

			iCount = m_Processes.GetItemCount();
			s.LoadString(IDS_LEVELCONTROL);
			lvis.iItem = iCount;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			lvis.lParam = SECID_PROCESS_RELAY_STATE_CONTROL;
			m_Processes.InsertItem(&lvis);
			s.ReleaseBuffer();
		}
	}
	if (pMedia)
	{
		if (pMedia->GetType() == SEC_MEDIA_TYPE_INPUT)
		{
			lvis.iImage = IMAGE_AUDIO_IN;
			iCount = m_Processes.GetItemCount();
			s.LoadString(IDS_AUDIO_PROCESS);
			lvis.iItem = iCount;
			lvis.pszText = s.GetBuffer(0);
			lvis.cchTextMax = s.GetLength();
			lvis.lParam = SECID_PROCESS_AUDIO;
			m_Processes.InsertItem(&lvis);
			s.ReleaseBuffer();
		}
	}

	BOOL bAlarmList = FALSE;
#ifdef _DEBUG
	bAlarmList = !bOutputRelais;
#else
	bAlarmList = (sVersion >= _T("5.0.4")) && !bOutputRelais;
#endif

	BOOL bFound = FALSE;
	for (int i=0;i<m_pArchivList->GetSize()&&!bFound;i++)
	{
		bFound = m_pArchivList->GetAtFast(i)->GetType() == ALARM_LIST_ARCHIV;
	}

	if (bAlarmList && bFound)
	{
		lvis.iImage = IMAGE_ALARM_LIST;
		iCount = m_Processes.GetItemCount();
		s.LoadString(IDS_ALARM_LIST);
		lvis.iItem = iCount;
		lvis.pszText = s.GetBuffer(0);
		lvis.cchTextMax = s.GetLength();
		lvis.lParam = SECID_PROCESS_ALARM_LIST;
		m_Processes.InsertItem(&lvis);
		s.ReleaseBuffer();
	}


	// set previous selection, will be ignored if not furthermore in list
	if (m_pSelectedITO)
	{
		m_Processes.SetRadioItemFromlParam(m_pSelectedITO->GetProcessID().GetID());
		OnItemChangedProcessList(m_Processes.GetRadioItem());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::InsertProcess(const CProcess* pProcess)
{
	CString s;
	s = pProcess->GetName();
	if ( s.IsEmpty() ) {
		s.LoadString(IDS_NONAME);
	}

	int iCount = m_Processes.GetItemCount();
	LV_ITEM lvis;

	lvis.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
	lvis.iItem = iCount;
	lvis.iSubItem = 0;
	lvis.state=INDEXTOSTATEIMAGEMASK(1);
	lvis.stateMask=LVIS_STATEIMAGEMASK;
	lvis.pszText = s.GetBuffer(0);
	lvis.cchTextMax = s.GetLength();
	lvis.iImage = pProcess->ImageByType();
	lvis.lParam = pProcess->GetID();
	m_Processes.InsertItem(&lvis);
	s.ReleaseBuffer();
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillArchivs(BOOL bOnlyAlarmList) 
{
	m_Archivs.DeleteAllItems();

	int iCount = m_pArchivList->GetSize();
	int iIndex;
	for (int j=0 ; j<iCount ; j++) 
	{
		CArchivInfo *pArchive = m_pArchivList->GetAt(j);
		// exclude Suchergebn.
		if (pArchive->GetID().GetSubID() != LOCAL_SEARCH_RESULT_ARCHIV_NR) 
		{	
			// Archive von Hosts nicht zeigen
			CHostArray* m_pHostArray = m_pParent->GetDocument()->GetHosts();
			if (NULL==m_pHostArray->GetHostForArchive(pArchive->GetID()))
			{
				if ((bOnlyAlarmList && pArchive->GetType() == ALARM_LIST_ARCHIV)
					|| (!bOnlyAlarmList && pArchive->GetType() != ALARM_LIST_ARCHIV)
					)
				{
					iIndex = m_Archivs.GetItemCount();
					int iImage = CArchivPage::GetTypImage(pArchive->GetType());

					if (-1 == m_Archivs.InsertItem(	LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE,iIndex,
						pArchive->GetName(),
						INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,
						iImage,	// NOT YET destintion images, alarm, ring, vorring
						pArchive->GetID().GetID()	// use ID as item data
						)
						)
					{
						WK_TRACE_ERROR(_T("Insert archives FAILED\n"));
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillNotifications()
{
	m_Notifications.DeleteAllItems();

	CString l;

	l.LoadString(IDS_NO_NOTIFICATION);
	
	m_Notifications.InsertItem(
		LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE,0,
		l,
		INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,
		IMAGE_NULL,
		SECID_NO_ID
		);
	
	int iCount = m_pNotificationList->GetSize();
	for (int j=0 ; j<iCount ; j++) 
	{
		CNotificationMessage *pNM = m_pNotificationList->GetAt(j);
		int ix = m_Notifications.GetItemCount();
		if (-1 == m_Notifications.InsertItem(	LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM|LVIF_STATE,ix,
												pNM->GetName(),
												INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,
												IMAGE_NOTIFICATION,	// NOT YET destintion images, alarm, ring, vorring
												pNM->GetID().GetID()	// use ID as item data
												)
			)
		{
			WK_TRACE_ERROR(_T("Insert notification FAILED\n"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::FillPTZPresets()
{
	m_PTZPresets.DeleteAllItems();

	int ix = m_PTZPresets.GetItemCount();
	CString s;
	DWORD dwFlags = LVIF_TEXT|LVIF_PARAM|LVIF_STATE;

	// attention Rolf do not optimize with a loop
	// enum is not in order, do not change the order
	// of the enum!
	s = CameraControlCmd2String(CCC_POS_1);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_1);
	s = CameraControlCmd2String(CCC_POS_2);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_2);
	s = CameraControlCmd2String(CCC_POS_3);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_3);
	s = CameraControlCmd2String(CCC_POS_4);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_4);
	s = CameraControlCmd2String(CCC_POS_5);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_5);
	s = CameraControlCmd2String(CCC_POS_6);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_6);
	s = CameraControlCmd2String(CCC_POS_7);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_7);
	s = CameraControlCmd2String(CCC_POS_8);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_8);
	s = CameraControlCmd2String(CCC_POS_9);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_9);
	s = CameraControlCmd2String(CCC_POS_HOME);
	m_PTZPresets.InsertItem(dwFlags,ix,	s,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK,0,CCC_POS_HOME);
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActivationPage)
	DDX_Control(pDX, IDC_RADIO_PRIORITY_1, m_radioPriority1);
	DDX_Control(pDX, IDC_RADIO_PRIORITY_2, m_radioPriority2);
	DDX_Control(pDX, IDC_RADIO_PRIORITY_3, m_radioPriority3);
	DDX_Control(pDX, IDC_RADIO_PRIORITY_4, m_radioPriority4);
	DDX_Control(pDX, IDC_RADIO_PRIORITY_5, m_radioPriority5);
	DDX_Control(pDX, IDC_RADIO_STATE_OFF, m_radioOFF);
	DDX_Control(pDX, IDC_RADIO_STATE_OK, m_radioOK);
	DDX_Control(pDX, IDC_RADIO_STATE_ALARM, m_radioALARM);
	DDX_Control(pDX, IDC_NOTIFICATIONS, m_Notifications);
	DDX_Control(pDX, IDC_TXT_NOTIFICATION, m_txtNotification);
	DDX_Control(pDX, IDC_TXT_ARCHIV, m_txtArchiv);
	DDX_Control(pDX, IDC_ARCHIVS, m_Archivs);
	DDX_Control(pDX, IDC_TXT_PTZ_PRESET, m_txtPTZPreset);
	DDX_Control(pDX, IDC_PTZ_PRESET, m_PTZPresets);
	DDX_Control(pDX, IDC_LIST_TIMER, m_Timers);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_Processes);
	DDX_Control(pDX, IDC_LIST_OUTPUT, m_Outputs);
	DDX_Control(pDX, IDC_LIST_ACTIVATION, m_InputsToOutputs);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CActivationPage, CSVPage)
	//{{AFX_MSG_MAP(CActivationPage)
	ON_BN_CLICKED(IDC_RADIO_STATE_ALARM, OnRadioStateAlarm)
	ON_BN_CLICKED(IDC_RADIO_STATE_OFF, OnRadioStateOff)
	ON_BN_CLICKED(IDC_RADIO_STATE_OK, OnRadioStateOk)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_1, OnRadioPriority1)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_2, OnRadioPriority2)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_3, OnRadioPriority3)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_4, OnRadioPriority4)
	ON_BN_CLICKED(IDC_RADIO_PRIORITY_5, OnRadioPriority5)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ACTIVATION, OnItemchangedListActivation)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ACTIVATION, OnColumnclickListActivation)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ACTIVATION, OnDblclkListActivation)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ACTIVATION, OnClickListActivation)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ACTIVATION, OnRclickListActivation)
	ON_NOTIFY(NM_RDBLCLK, IDC_LIST_ACTIVATION, OnRdblclkListActivation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivationPage message handlers
BOOL CActivationPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	CSVApp* pApp = (CSVApp*)AfxGetApp();

	const int		iColumns = 7;
	CRect			rect;
	LV_COLUMN		lvcolumn;
	CString s[iColumns];
	int i;
	int w[] = {18,12,14,15,15,10,16};	// CAVET add sum has to be 100

	m_headerArrowBitmap.LoadBitmap(IDB_DOWN);
	// Activation ListCtrl initialisieren
	m_InputsToOutputs.GetClientRect(&rect);
	s[0].LoadString(IDS_ACTIVATION_INPUT);
	GetDlgItem(IDC_GROUP_STATE)->GetWindowText(s[1]);
	GetDlgItem(IDC_STATIC_TIMER)->GetWindowText(s[2]);
	GetDlgItem(IDC_STATIC_OUT)->GetWindowText(s[3]);
	GetDlgItem(IDC_STATIC_PROCESS)->GetWindowText(s[4]);
	GetDlgItem(IDC_GROUP_PRIORITY)->GetWindowText(s[5]);
	GetDlgItem(IDC_TXT_ARCHIV)->GetWindowText(s[6]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (i = 0 ; i < iColumns ; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_InputsToOutputs.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
	}

	m_iSelectedColumn = 0;

	// enable string and bitmp
	HD_ITEM hdItem;
	memset(&hdItem,0,sizeof(HD_ITEM));
	hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
	m_InputsToOutputs.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
	hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
	hdItem.fmt |= HDF_BITMAP;
	hdItem.hbm = m_headerArrowBitmap;
	m_InputsToOutputs.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);

	lvcolumn.pszText = NULL;
	lvcolumn.iSubItem = 0;

	// Timer StatusList initialisieren
	m_Timers.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_Timers.SetImageList(pApp->GetSmallImage(),LVSIL_SMALL);
	m_Timers.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_Timers.InsertColumn(0,&lvcolumn);

	// Output StatusList initialisieren
	m_Outputs.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_Outputs.SetImageList(pApp->GetSmallImage(),LVSIL_SMALL);
	m_Outputs.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_Outputs.InsertColumn(0,&lvcolumn);

	// Process StatusList initialisieren
	m_Processes.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_Processes.SetImageList(pApp->GetSmallImage(),LVSIL_SMALL);
	m_Processes.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_Processes.InsertColumn(0,&lvcolumn);

	// Archivs initialisieren
	m_Archivs.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_ArchiveImages.Create(IDB_ARCHIVE,20,1,RGB(0,255,255));
	m_Archivs.SetImageList(&m_ArchiveImages,LVSIL_SMALL);
	m_Archivs.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_Archivs.InsertColumn(0,&lvcolumn);

	// Notifications initialisieren
	m_Notifications.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_Notifications.SetImageList(pApp->GetSmallImage(),LVSIL_SMALL);
	m_Notifications.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_Notifications.InsertColumn(0,&lvcolumn);

	// PTZ Presets initialisieren
	m_PTZPresets.SetStateInfo(pApp->GetRadioImage(),0,16);
	m_PTZPresets.GetClientRect(&rect);
	lvcolumn.cx = rect.Width();
	m_PTZPresets.InsertColumn(0,&lvcolumn);

	Initialize();

	LONG dw = ListView_GetExtendedListViewStyle(m_InputsToOutputs.m_hWnd);
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle(m_InputsToOutputs.m_hWnd,dw);
	m_InputsToOutputs.ModifyStyle(LVS_SINGLESEL,0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemchangedListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		UINT    uNewState = pNMListView->uNewState;
//		UINT    uOldState = pNMListView->uOldState;
		UINT    uChanged  = pNMListView->uChanged;

		// Only select, if really selected!
		// The state has to be SELECTED
		// Function is called even, if
		// - previous clicked in empty line
		// - then a new one was added
		// - now the "old" one shall be deselected,
		//   but we call "select it", thats wrong
		// leads to recursion => stackoverflow => GP!
		// gf todo: Better solution:
		//		    Re-select previous item, if clicked in empty line, see at the "click" functions
		if (uChanged & LVIF_STATE)
		{
			if (uNewState != 0)
			{
//				SelectActivation(pNMListView->iItem,FALSE);
				if (m_bWeAreDeletingDoNotSelect == FALSE)
				{
					SelectActivation(pNMListView->iItem /*, TRUE*/);
				}
			}
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActivationPage::CanNew()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActivationPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActivationPage::CanDelete()
{
	return m_InputsToOutputs.GetItemCount()>0;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnNew() 
{
	CSelectInputDlg dlg(this,m_pInputList);

	if (IDOK==dlg.DoModal())
	{
		CInputToOutput* pInputToOutput;
		CInput* pInput;
		int i, newCount;

		pInput = dlg.GetSelectedInput();
		pInputToOutput = pInput->AddInputToOutput();

		if (m_pSelectedITO)
		{
			pInputToOutput->SetInputState(m_pSelectedITO->GetInputState());
			pInputToOutput->SetTimerID(m_pSelectedITO->GetTimerID());
			pInputToOutput->SetPriority(m_pSelectedITO->GetPriority());
			pInputToOutput->SetProcessID(m_pSelectedITO->GetProcessID());
			pInputToOutput->SetOutputID(m_pSelectedITO->GetOutputOrMediaID());
			pInputToOutput->SetArchiveID(m_pSelectedITO->GetArchiveID());
			pInputToOutput->SetNotificationID(m_pSelectedITO->GetNotificationID());
			pInputToOutput->SetPTZPreset(m_pSelectedITO->GetPTZPreset());
		}
		newCount = m_InputsToOutputs.GetItemCount();
		for (i=0; i<newCount;i++)
		{
			m_InputsToOutputs.SetItemState(i,0,LVIS_SELECTED|LVIS_FOCUSED);
		}

		i = InsertActivation(pInput,pInputToOutput);
		SelectActivation(i);
		SetModified();
		EnableExceptNew(FALSE);
		m_InputsToOutputs.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnDelete() 
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedITO==NULL))
	{
		return;
	}

	BOOL bDeleted = FALSE;
	int i, nCount = m_InputsToOutputs.GetSelectedCount();
	if (nCount)
	{
	// CAVEAT DeleteItem seems to cause SelectActivation automatically
		m_bWeAreDeletingDoNotSelect = TRUE;

		int *pnPos = new int[nCount];
		i = 0;
		POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
		while (pos)
		{
			pnPos[i++] = m_InputsToOutputs.GetNextSelectedItem(pos);
		}
		for (i=nCount-1; i>=0; i--)
		{
			m_pSelectedITO = (CInputToOutput*)m_InputsToOutputs.GetItemData(pnPos[i]);
			m_pSelectedITO->DeleteInputToOutputFromInput();
			if (m_InputsToOutputs.DeleteItem(pnPos[i]))
			{
				m_pSelectedITO = NULL;
				bDeleted = TRUE;
			}
		}
		delete pnPos;
		m_bWeAreDeletingDoNotSelect = FALSE;
	}

	if (bDeleted)	// remove from listCtrl
	{
		// calc new selection
		int newCount = m_InputsToOutputs.GetItemCount();	// pre-subtract before DeleteItem
		if (nCount == 1) // was a single selection
		{
			i=m_iSelectedItem;	// new selection

			// range check
			if (i > newCount-1) 
			{	// outside ?
				i=newCount-1;
			}
		}
		else
		{
			i = (newCount>0 ? 0 :-1);
		}

		SelectActivation(i);
		SetModified();
		EnableExceptNew();
		m_InputsToOutputs.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioStateAlarm() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetInputState() != IO_ALARM)
	{
		CString s;
		s.LoadString(IDS_ALARM);
		m_pSelectedITO->SetInputState(IO_ALARM);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,1,s);

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetInputState(IO_ALARM);
						m_InputsToOutputs.SetItemText(nPos,1,s);
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioStateOff() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetInputState() != IO_NOSTATE)
	{
		CString s;
		s.LoadString(IDS_OFF);
		m_pSelectedITO->SetInputState(IO_NOSTATE);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,1,s);

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetInputState(IO_NOSTATE);
						m_InputsToOutputs.SetItemText(nPos,1,s);
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioStateOk() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetInputState() != IO_OK)
	{
		CString s;
		s.LoadString(IDS_OK);
		m_pSelectedITO->SetInputState(IO_OK);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,1,s);

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetInputState(IO_OK);
						m_InputsToOutputs.SetItemText(nPos,1,s);
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioPriority1() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetPriority() != 1)
	{
		m_pSelectedITO->SetPriority(1);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,_T("1"));

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetPriority(1);
						m_InputsToOutputs.SetItemText(nPos,5,_T("1"));
					}
				}
			}
		}

		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioPriority2() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetPriority() != 2)
	{
		m_pSelectedITO->SetPriority(2);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,_T("2"));

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetPriority(2);
						m_InputsToOutputs.SetItemText(nPos,5,_T("2"));
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioPriority3() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetPriority() != 3)
	{
		m_pSelectedITO->SetPriority(3);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,_T("3"));

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetPriority(3);
						m_InputsToOutputs.SetItemText(nPos,5,_T("3"));
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioPriority4() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetPriority() != 4)
	{
		m_pSelectedITO->SetPriority(4);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,_T("4"));

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetPriority(4);
						m_InputsToOutputs.SetItemText(nPos,5,_T("4"));
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRadioPriority5() 
{
	if (m_pSelectedITO && m_pSelectedITO->GetPriority() != 5)
	{
		m_pSelectedITO->SetPriority(5);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,5,_T("5"));

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetPriority(5);
						m_InputsToOutputs.SetItemText(nPos,5,_T("5"));
					}
				}
			}
		}
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedTimerList(int nListItem)
{
	// may be a new item was selected
	CSecID timerID;
	const CSecTimer *pTimer;
	
	timerID = (DWORD)(m_Timers.GetRadioItemData());
	if (m_pSelectedITO->GetTimerID()!=timerID)
	{
		CString s;
		if (timerID.IsTimerID())
		{
			pTimer = m_pTimerList->GetTimerByID(timerID);
			if (pTimer)
			{
				m_pSelectedITO->SetTimerID(timerID);
				s = pTimer->GetName();
				SetModified();
			}
		}
		else if (timerID == SECID_NO_ID)
		{
			m_pSelectedITO->SetTimerID(timerID);
			s.LoadString(IDS_NOTIMER);
			SetModified();
		}
		m_InputsToOutputs.SetItemText(m_iSelectedItem,2,s);

		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						pItem->SetTimerID(timerID);
						m_InputsToOutputs.SetItemText(nPos,2,s);
					}
				}
			}
		}

	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedOutputList(int nListItem)
{
	CSecID id;
	const COutput*	pOutput = NULL;
	const CMedia*	pMedia = NULL;
	
	id = (DWORD)(m_Outputs.GetRadioItemData());
	if (m_pSelectedITO->GetOutputOrMediaID()!=id)
	{
		if (id.IsOutputID())
		{
			pOutput = m_pOutputList->GetOutputByID(id);
			if (pOutput)
			{
				m_pSelectedITO->SetOutputID(id);
				m_InputsToOutputs.SetItemText(m_iSelectedItem,3,pOutput->GetName());
//				m_pSelectedITO->SetProcessID((DWORD)m_Processes.GetRadioItemData());
				SetModified();
			}
			else
			{
				m_Processes.EnableWindow();
			}
		}
		else if (id.IsMediaID())
		{
			pMedia = m_pMediaList->GetMediaByID(id);
			if (pMedia)
			{
				m_pSelectedITO->SetOutputID(id);
				m_InputsToOutputs.SetItemText(m_iSelectedItem,3,pMedia->GetName());
				SetModified();
			}
			else
			{
				m_Processes.EnableWindow();
			}
		}
		else if (id == SECID_NO_ID)
		{
			CString s;
			s.LoadString(IDS_NOTHING);
			m_pSelectedITO->SetOutputID(SECID_NO_ID);
			m_InputsToOutputs.SetItemText(m_iSelectedItem,3,s);
			SetModified();
		}
		FillProcesses(pOutput,pMedia);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedProcessList(int nListItem)
{
	CSecID processID;
	
	processID = (DWORD)(m_Processes.GetRadioItemData());
	// has the process really changed
	if (m_pSelectedITO->GetProcessID()!=processID)
	{
		// is it a process
		if (processID.IsProcessID())
		{
			m_pSelectedITO->SetProcessID(processID);
		}
		// is it no process
		else
		{
			m_pSelectedITO->SetProcessID(SECID_NO_ID);
		}
		
		CString sProcess = UpdateProcessSettings();
		if (m_InputsToOutputs.GetSelectedCount() > 1)
		{
			POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			CInputToOutput	*pItem;
			int nPos;
			while (pos)
			{
				nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
				if (nPos != m_iSelectedItem)
				{
					pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
					if (pItem)
					{
						if (processID.IsProcessID())
						{
							pItem->SetProcessID(processID);
						}
						// is it no process
						else
						{
							pItem->SetProcessID(SECID_NO_ID);
						}
						m_InputsToOutputs.SetItemText(nPos,4,sProcess);
					}
				}
			}
		}

		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedArchivList(int nListItem)
{
	// HEDU 011098
	CSecID archiveID( (DWORD)(m_Archivs.GetRadioItemData())) ;
	if (m_pSelectedITO->GetArchiveID()!=archiveID) 
	{
		// update the activation text
		// NOT YET some extra function for this kind of update
		CString sNewDestination;
		
		if (archiveID!=SECID_NO_ID) 
		{
			CArchivInfo *pArchive = m_pArchivList->GetArchivInfo(archiveID);
			if (pArchive) 
			{
				sNewDestination = pArchive->GetName();
			}
			else 
			{
				// archive not found or non archive destination
			}
		}
		
		// do the modification
		m_pSelectedITO->SetArchiveID(archiveID);
		
		m_InputsToOutputs.SetItemText(m_iSelectedItem,6,sNewDestination);
		SetModified();
	}
	else 
	{
		// no change
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedNotificationList(int nListItem)
{
	CSecID id( (DWORD)(m_Notifications.GetRadioItemData())) ;

	if (m_pSelectedITO->GetNotificationID()!=id) 
	{
		// update the activation text
		// NOT YET some extra function for this kind of update
		CString sNewDestination;
		
		if (id!=SECID_NO_ID) 
		{
			CNotificationMessage *pNM = m_pNotificationList->GetNotificationMessage(id);
			if (pNM) 
			{
				sNewDestination = pNM->GetName();
			}
			else 
			{
				// notification not found or non notification destination
			}
		}
		
		// do the modification
		m_pSelectedITO->SetNotificationID(id);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,6,sNewDestination);
		SetModified();
	}
	else 
	{
		// no change
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedPTZPresetList(int nListItem)
{
	CameraControlCmd ccc = (CameraControlCmd)m_PTZPresets.GetRadioItemData();

	if (m_pSelectedITO->GetPTZPreset()!=ccc) 
	{
		m_pSelectedITO->SetPTZPreset(ccc);
		m_InputsToOutputs.SetItemText(m_iSelectedItem,6,CameraControlCmd2String(ccc));
		SetModified();
	}
	else 
	{
		// no change
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnItemChangedStatusList(CStatusList* pStatusList, int nListItem)
{
	if (m_pSelectedITO==NULL)
		return;
	
	if (pStatusList == &m_Timers)
	{
		OnItemChangedTimerList(nListItem);
	}
	else if (pStatusList == &m_Outputs)
	{
		OnItemChangedOutputList(nListItem);
	}
	else if (pStatusList == &m_Processes)
	{
		OnItemChangedProcessList(nListItem);
	} 
	else if (pStatusList == &m_Archivs)
	{
		OnItemChangedArchivList(nListItem);
	}
	else if (pStatusList == &m_Notifications)
	{
		OnItemChangedNotificationList(nListItem);
	}
	else if (pStatusList == &m_PTZPresets)
	{
		OnItemChangedPTZPresetList(nListItem);
	}
	else 
	{
		TRACE(_T("Invalid CStatusList* for OnItemChangedStatusList\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnColumnclickListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// remove the bitmap from the current column
	HD_ITEM hdItem;
	memset(&hdItem,0,sizeof(HD_ITEM));
	hdItem.mask = HDI_FORMAT;
	m_InputsToOutputs.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
	hdItem.mask = HDI_FORMAT;
	hdItem.fmt &= ~HDF_BITMAP;
	m_InputsToOutputs.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);

	// take the newly activated column
	m_iSelectedColumn = pNMListView->iSubItem;

	HBITMAP hbmp = m_headerArrowBitmap;
	if (hbmp) 
	{
		// enable string and bitmp
		memset(&hdItem,0,sizeof(HD_ITEM));
		hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
		m_InputsToOutputs.GetHeaderCtrl()->GetItem(m_iSelectedColumn ,&hdItem);
		hdItem.mask = HDI_BITMAP | HDI_FORMAT;;
		hdItem.fmt |= HDF_BITMAP;
		hdItem.hbm = m_headerArrowBitmap;
		m_InputsToOutputs.GetHeaderCtrl()->SetItem(m_iSelectedColumn ,&hdItem);
	}

	CPtrList Selections;
	int nPos, nCount;
	CInputToOutput *pItem, *pSelected = NULL;

	POSITION pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
	while (pos)
	{
		nPos = m_InputsToOutputs.GetNextSelectedItem(pos);
		pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
		if (nPos == m_iSelectedItem)
		{
			pSelected = pItem;
		}
		if (pItem)
		{
			m_InputsToOutputs.SetItemState(nPos,0,LVIS_SELECTED|LVIS_FOCUSED);
			Selections.AddHead(pItem);
		}
	}

	m_InputsToOutputs.SortItems(MyCompareFunction , (DWORD)this);
	
	nCount = m_InputsToOutputs.GetItemCount();
	for (nPos=0; nPos<nCount; nPos++)
	{
		pItem = (CInputToOutput*)m_InputsToOutputs.GetItemData(nPos);
		if (pItem == pSelected)
		{
			m_iSelectedItem = nPos;
		}
		if (Selections.Find(pItem))
		{
			m_InputsToOutputs.SetItemState(nPos,LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}
	}
	
	BOOL bEnable = (m_InputsToOutputs.GetSelectedCount() == 1) ? TRUE : FALSE;
	m_Archivs.EnableWindow(bEnable);
	m_Outputs.EnableWindow(bEnable);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnClickListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_InputsToOutputs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}
	BOOL bEnable = (m_InputsToOutputs.GetSelectedCount() == 1) ? TRUE : FALSE;
	m_Archivs.EnableWindow(bEnable);
	m_Outputs.EnableWindow(bEnable);
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnDblclkListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_InputsToOutputs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	// change the CInput of CInputToOutput
	if (m_pSelectedITO)
	{
		CSelectInputDlg dlg(this,m_pInputList);

		if (IDOK==dlg.DoModal())
		{
			CInput* pNewInput = dlg.GetSelectedInput();
			CInputToOutput *pCurrent = NULL;
			POSITION pos = NULL;
			BOOL bChanged = FALSE, bSingleSelection = m_InputsToOutputs.GetSelectedCount() == 1;
			int iCurrent = 0, iSelected = m_iSelectedItem;
			if (bSingleSelection)
			{
				pCurrent = m_pSelectedITO;
				iCurrent = m_iSelectedItem;
			}
			else
			{
				pos = m_InputsToOutputs.GetFirstSelectedItemPosition();
			}
			while (pos || bSingleSelection)
			{
				if (pos)
				{
					iCurrent = m_InputsToOutputs.GetNextSelectedItem(pos);
					pCurrent = (CInputToOutput*)m_InputsToOutputs.GetItemData(iCurrent);
				}
				CInput* pOldInput = pCurrent->GetInput();

				if (pNewInput->GetID() != pOldInput->GetID())
				{
					// change it
					bChanged = TRUE;
					DWORD	dwState = pCurrent->GetInputState();
					CSecID	id = pCurrent->GetOutputOrMediaID();
					CSecID	idTimer = pCurrent->GetTimerID();
					CSecID	idProcess = pCurrent->GetProcessID();
					DWORD	dwPriority = pCurrent->GetPriority();
					CSecID  idArchive = pCurrent->GetArchiveID();
					CSecID  idNotification = pCurrent->GetNotificationID();
					CameraControlCmd ccc = pCurrent->GetPTZPreset();

					pOldInput->DeleteInputToOutput(pCurrent);
					pCurrent = pNewInput->AddInputToOutput();
					m_InputsToOutputs.SetItemData(iCurrent,(DWORD)pCurrent);
					m_pSelectedITO = pCurrent;
					m_iSelectedItem = iCurrent;

					pCurrent->SetInputState(dwState);
					pCurrent->SetOutputID(id);
					pCurrent->SetTimerID(idTimer);
					pCurrent->SetProcessID(idProcess);
					pCurrent->SetPriority(dwPriority);
					pCurrent->SetArchiveID(idArchive);
					pCurrent->SetNotificationID(idNotification);
					pCurrent->SetPTZPreset(ccc);

					SelectedItemToControl();
				}
				bSingleSelection = FALSE;
			}
			if (bChanged)
			{
				m_iSelectedItem = iSelected;
				m_pSelectedITO = (CInputToOutput*)m_InputsToOutputs.GetItemData(m_iSelectedItem);
				SetModified();
				EnableExceptNew(FALSE);
			}
		}
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRclickListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_InputsToOutputs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CActivationPage::OnRdblclkListActivation(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (   (pNMListView->iItem == -1)
		&& (m_iSelectedItem != -1)
		)
	{
		// Es wurde ins leere Feld geklickt,
		// daher Select-Markierung wiederherstellen
		m_InputsToOutputs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
CString CActivationPage::UpdateProcessSettings()
{
	BOOL bEnable = FALSE;
	CProcess::ProcessType processType = CProcess::P_NOTYPE;
	CProcess* pProcess = m_pProcessList->GetProcess(m_pSelectedITO->GetProcessID());
	CString sProcess, sAdditional;
	// nevertheless we have to set the archive or notification radios
	CSecID idArchive = m_pSelectedITO->GetArchiveID();
	m_Archivs.SetRadioItemFromlParam(idArchive.GetID());
	CSecID idNotification = m_pSelectedITO->GetNotificationID();
	m_Notifications.SetRadioItemFromlParam(idNotification.GetID());
	CameraControlCmd ccc = m_pSelectedITO->GetPTZPreset();
	if (ccc != CCC_INVALID)
	{
		m_PTZPresets.SetRadioItemFromlParam(ccc);
	}
	// do we have a process
	if (pProcess) 
	{
		m_Processes.SetRadioItemFromlParam(m_pSelectedITO->GetProcessID().GetID());
		sProcess = pProcess->GetName();
		//	SECID_NO_ID is also in the list, so we can always set something
		// if it is a save process
		if (pProcess->IsSave())
		{
			CArchivInfo* pAI = m_pArchivList->GetArchivInfo(idArchive);
			if (pAI)
			{
				sAdditional = pAI->GetName();
			}
		}
		else if (pProcess->IsCall())
		{
			CNotificationMessage* pNM = m_pNotificationList->GetNotificationMessage(idNotification);
			if (pNM)
			{
				sAdditional = pNM->GetName();
			}
		}
		bEnable = TRUE;
		processType = pProcess->GetType();
	}
	else 
	{
		if (m_pSelectedITO->GetProcessID()==SECID_PROCESS_RELAY_EDGE_CONTROL)
		{
			m_Processes.SetRadioItemFromlParam(SECID_PROCESS_RELAY_EDGE_CONTROL);
			sProcess.LoadString(IDS_EDGECONTROL);
		}
		else if (m_pSelectedITO->GetProcessID()==SECID_PROCESS_RELAY_STATE_CONTROL)
		{
			m_Processes.SetRadioItemFromlParam(SECID_PROCESS_RELAY_STATE_CONTROL);
			sProcess.LoadString(IDS_LEVELCONTROL);
		}
		else if (m_pSelectedITO->GetProcessID()==SECID_PROCESS_PRESET)
		{
			m_Processes.SetRadioItemFromlParam(SECID_PROCESS_PRESET);
			sProcess.LoadString(IDS_PTZ_PRESET);
			processType = CProcess::P_PTZ_PRESET;
			bEnable = TRUE;
			CameraControlCmd ccc = m_pSelectedITO->GetPTZPreset();
			sAdditional = CameraControlCmd2String(ccc);
		}
		else if (m_pSelectedITO->GetProcessID()==SECID_PROCESS_ALARM_LIST)
		{
			m_Processes.SetRadioItemFromlParam(SECID_PROCESS_ALARM_LIST);
			sProcess.LoadString(IDS_ALARM_LIST);
			processType = CProcess::P_ALARM_LIST;
			bEnable = TRUE;
			CArchivInfo* pAI = m_pArchivList->GetArchivInfo(idArchive);
			if (pAI)
			{
				sAdditional = pAI->GetName();
			}
		}
		else if (m_pSelectedITO->GetProcessID()==SECID_PROCESS_AUDIO)
		{
			m_Processes.SetRadioItemFromlParam(SECID_PROCESS_AUDIO);
			sProcess.LoadString(IDS_AUDIO_PROCESS);
			bEnable = TRUE;
			processType = CProcess::P_SAVE;
			CArchivInfo* pAI = m_pArchivList->GetArchivInfo(idArchive);
			if (pAI)
			{
				sAdditional = pAI->GetName();
			}
		}
		else
		{
			m_Processes.SetRadioItem(-1);
		}
	}
	ShowHideAdditional(bEnable, processType,idArchive);
	m_InputsToOutputs.SetItemText(m_iSelectedItem,4,sProcess);
	m_InputsToOutputs.SetItemText(m_iSelectedItem,6,sAdditional);
	
	return sProcess;
}
