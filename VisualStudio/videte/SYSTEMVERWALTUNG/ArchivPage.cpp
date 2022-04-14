/* GlobalReplace: CDVRDrive --> CIPCDrive */
// ArchivPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ArchivPage.h"
#include ".\archivpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArchivPage dialog
CArchivPage::CArchivPage(CSVView* pParent) : CSVPage(CArchivPage::IDD)
{
	m_pParent = pParent;
	m_pSelectedArchivInfo = NULL;
	m_iSelectedItem = -1;
	m_pArchivs = pParent->GetDocument()->GetArchivs();
	m_pDrives = pParent->GetDocument()->GetDrives();
	m_pInputList = pParent->GetDocument()->GetInputs();
	m_pHostArray = pParent->GetDocument()->GetHosts();
	m_bActivationWereDeleted = FALSE;
	m_bHostArchivesWereDeleted = FALSE;
	m_bAutoReduceArchiveSize = TRUE;
	m_bDynamicExpandArchiveSize = TRUE;

	//{{AFX_DATA_INIT(CArchivPage)
	m_sName = _T("");
	m_iType = -1;
	m_dwSize = 15;
	m_sMax = _T("");
	m_sUsed = _T("");
	m_sWhole = _T("");
	m_dwMaxTime = 0;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::CanNew()
{
	BOOL bReturn =    (theApp.m_pPermission->IsSpecialReceiver() == FALSE)
				   && (m_pDrives->GetAvailableMB() > m_pArchivs->GetSizeMB()
				   );
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::CanDelete()
{
	if (m_Archivs.GetItemCount()>0)
	{
		if (m_pSelectedArchivInfo)
		{
			return (m_pSelectedArchivInfo->GetID().GetSubID() != LOCAL_SEARCH_RESULT_ARCHIV_NR);
		}
		return FALSE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::StretchElements()
{
	StretchButtonHeight(FALSE);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::CopyActualSettingsToNew(CArchivInfo* pAI)
{
	if (pAI && m_pSelectedArchivInfo)
	{
		pAI->SetSizeMB(m_pSelectedArchivInfo->GetSizeMB());
		pAI->SetMaxStorageTime(m_pSelectedArchivInfo->GetMaxStorageTime());
		pAI->SetTyp(m_pSelectedArchivInfo->GetType());
		pAI->SetSafeRingFor(m_pSelectedArchivInfo->GetSafeRingFor());
		pAI->SetFixedDrive(m_pSelectedArchivInfo->GetFixedDrive());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SaveChanges()
{
	WK_TRACE_USER(_T("Archiv-Einstellungen wurden geändert\n"));
	UpdateData();
	m_pArchivs->Save(GetProfile());
	GetProfile().WriteInt(SECTION_DBS, DBS_AUTO_REDUCE_ARCHIVES, m_bAutoReduceArchiveSize);
	if (m_bActivationWereDeleted)
	{
		m_pInputList->Save(GetProfile(),FALSE);
	}

	CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
	CProcess*pProcess;
	int i;
	BOOL bDeleted = FALSE;
	while (m_ProcessesToDelete.GetCount())
	{
		int nPTD = m_ProcessesToDelete.GetCount()-1;
		DWORD dwID = m_ProcessesToDelete.GetAt(nPTD);
		for (i=0; i<pProcessList->GetSize();i++) 
		{
			pProcess = pProcessList->GetAt(i);
			if (   pProcess 
				&& pProcess->IsBackup()
				&& pProcess->GetID() == dwID)
			{
				pProcessList->DeleteProcess(pProcess);
				bDeleted = TRUE;
				break;
			}
		}
		m_ProcessesToDelete.RemoveAt(nPTD);
	}
	if (bDeleted)
	{
		pProcessList->Save(GetProfile());
	}
	
	if (m_bHostArchivesWereDeleted)
	{
#ifdef _UNICODE
		if (m_pParent->GetDocument()->IsLocal())
		{
			m_pHostArray->m_bUseBinaryFormat = TRUE;
		}
		else
		{
			m_pHostArray->m_bUseBinaryFormat = m_pParent->GetDocument()->UseBinaryFormat();
		}
#endif
		m_pHostArray->Save(GetProfile());
	}
	GetProfile().DeleteSection(_T("ArchivManagment"));
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::CancelChanges()
{
	ClearArchivListBox();
	m_bHostArchivesWereDeleted = FALSE;
	m_pHostArray->Load(GetProfile());
	m_pArchivs->Load(GetProfile());
	m_bAutoReduceArchiveSize = GetProfile().GetInt(SECTION_DBS, DBS_AUTO_REDUCE_ARCHIVES, m_bAutoReduceArchiveSize);

	CArchivInfo* pArchivInfo = m_pArchivs->GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE, LOCAL_SEARCH_RESULT_ARCHIV_NR));
	if (pArchivInfo)
	{
		CString sName;
		sName.LoadString(IDS_SEARCH_RESULT);
		pArchivInfo->SetName(sName);
	}

	m_ProcessesToDelete.RemoveAll();
	m_bActivationWereDeleted = FALSE;
	m_pInputList->Load(GetProfile());
	FillArchivListBox();
	SelectItem((m_Archivs.GetItemCount()>0) ? 0 : -1);
	EnableExceptNew();
	if (m_Archivs.GetItemCount()>=0)
	{
		m_Archivs.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::IsDataValid()
{
	// check for size != 0
	if (m_dwSize == 0)
	{
		AfxMessageBox(IDP_ARCHIVE_SIZE_ZERO, MB_OK|MB_ICONSTOP);
		m_editSize.SetFocus();
		m_editSize.SetSel(0,-1);
		return FALSE;
	}

	// Check for total archiv size greater than total drive size
	if (m_pArchivs->GetSizeMB() > m_pDrives->GetAvailableMB())
	{
		DWORD dwOversize = m_pArchivs->GetSizeMB() - m_pDrives->GetAvailableMB();
		CString sMsg;
		sMsg.Format(IDP_ARCHIVS_TOO_BIG, dwOversize);
		AfxMessageBox(sMsg, MB_OK|MB_ICONSTOP);
		if (m_pDrives->GetAvailableMB() > 0)
		{
			SetModified();
			m_editSize.SetFocus();
			m_editSize.SetSel(0,-1);
		}
		return FALSE;
	}

	// some general variables
	int i,j;
	CString sMsg;
	CArchivInfo* pAI;

	// Check for fixed archives per drive
	CIPCDrive* pDrive;
	CString sRoot, sFixed;
	int iArchivesFound;
	DWORD dwApD = 0;
	for (i=0;i<m_pDrives->GetSize();i++)
	{
		pDrive = m_pDrives->GetAt(i);
		if (pDrive && (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			sRoot = pDrive->GetRootString();
			sRoot.MakeLower();
			iArchivesFound = 0;
			// Count all fixed archives for this drive
			for (j=0;j<m_pArchivs->GetSize();j++)
			{
				pAI = m_pArchivs->GetAt(j);
				sFixed = pAI->GetFixedDrive();
				sFixed.MakeLower();
				if (!sFixed.IsEmpty() && 0==sRoot.Find(sFixed))
				{
					dwApD += pAI->GetSizeMB();
					iArchivesFound++;
				}
			}
			// Check for total fixed archive size greater than the drive size
			if (dwApD > pDrive->GetAvailableMB())
			{
				sMsg.Format(IDP_DRIVE_TOO_LESS,pDrive->GetRootString());
				AfxMessageBox(sMsg,MB_OK|MB_ICONSTOP);
				return FALSE;
			}
			// Check for first fixed archive on drive
			if (iArchivesFound == 1)
			{
				// is there any data on drive
				// gf 24.07.02 There is ALWAYS some data on drive (Recycler + SystemVolumeInformation)!
				DWORD dwSafetySpace = 20;
				DWORD dw1Percent = pDrive->GetMB() / 100;
				if (dw1Percent>dwSafetySpace)
				{
					dwSafetySpace = dw1Percent;
				}

				if (pDrive->GetUsedMB() > dwSafetySpace)
				{
					sMsg.Format(IDP_DRIVE_FOR_FIXED_NOT_EMPTY,pDrive->GetRootString());
					AfxMessageBox(sMsg,MB_OK|MB_ICONSTOP);
					m_comboFixedDrive.SetFocus();
					return FALSE;
				}
			}
		}
	}

	// check all archives
	CArchivInfo* pAIAlarm;
	CString sDriveVA, sDriveAlarm;
	for (i=0;i<m_pArchivs->GetSize();i++)
	{
		pAI = m_pArchivs->GetAt(i);

		// check if safering
		if (pAI->GetType() == SICHERUNGS_RING_ARCHIV)
		{
			// check alarm archive for safering
			pAIAlarm = m_pArchivs->GetArchivInfo(CSecID(SECID_GROUP_ARCHIVE,
														pAI->GetSafeRingFor()));
			if (pAIAlarm)
			{
				// if fixed drive must be same drive
				sDriveVA = pAI->GetFixedDrive();
				sDriveAlarm = pAIAlarm->GetFixedDrive();
				if (sDriveVA != sDriveAlarm)
				{
					sMsg.Format(IDP_ALARM_SAFERING_DIFFERENT,
								pAIAlarm->GetName(),pAI->GetName());
					AfxMessageBox(sMsg,MB_OK|MB_ICONSTOP);
					return FALSE;
				}

				// check for size of alarm > safering
				DWORD dwSafeRingSize = pAI->GetSizeMB();
				DWORD dwAlarmSize = pAIAlarm->GetSizeMB();
				if (dwSafeRingSize >= dwAlarmSize)
				{
					sMsg.Format(IDP_ALARM_SMALLER_THAN_SAFERING,
								pAI->GetName(), pAIAlarm->GetName());
					AfxMessageBox(sMsg,MB_OK|MB_ICONSTOP);
					return FALSE;
				}
			}
			else
			{
				sMsg.Format(IDP_NO_ALARM_FOR_SAFERING,pAI->GetName());
				AfxMessageBox(sMsg,MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}


	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::EnableExceptNew()
{
	BOOL bEnable = (   (m_pDrives->GetAvailableMB() > 0)
				    && (m_Archivs.GetItemCount() > 0)
				   );
	m_Archivs.EnableWindow(bEnable);

	BOOL bEnable2 = (   bEnable
					 && m_pSelectedArchivInfo
					 && (m_pSelectedArchivInfo->GetArchivNr() != LOCAL_SEARCH_RESULT_ARCHIV_NR)
					);
	BOOL bMultiSel = m_Archivs.GetSelectedCount() > 1;

	BOOL bDynamicExpandArchives = (bEnable2 && m_pSelectedArchivInfo->GetType() == RING_ARCHIV);

	m_editName.EnableWindow(bEnable2 && !bMultiSel);
	m_editSize.EnableWindow(bEnable);
	m_comboFixedDrive.EnableWindow(bEnable && !bMultiSel);
	m_editMaxTime.EnableWindow(bEnable2);
	m_radioRing.EnableWindow(bEnable2);
	m_radioAlarm.EnableWindow(bEnable2);
	m_radioVorring.EnableWindow(bEnable2 && !bMultiSel);
	m_radioAlarmList.EnableWindow(bEnable2);
	m_comboAlarm.EnableWindow(bEnable2 && (m_iType==2) && !bMultiSel);

	m_ctrlDynamicExpandArchive.EnableWindow(bDynamicExpandArchives);

	// todo TO3000
	// Beim TO_3000 Prototyp erstmal nur Informationen anzeigen
	// es darf nichts verstellt werden können!
	CWK_Dongle dongle;
	if (dongle.GetProductCode() == IPC_TOBS_TO3K)
	{
		m_editName.EnableWindow(FALSE);
		m_editSize.EnableWindow(FALSE);
		m_comboFixedDrive.EnableWindow(FALSE);
		m_editMaxTime.EnableWindow(FALSE);
		m_radioRing.EnableWindow(FALSE);
		m_radioAlarm.EnableWindow(FALSE);
		m_radioVorring.EnableWindow(FALSE);
		m_radioAlarmList.EnableWindow(FALSE);
		m_comboAlarm.EnableWindow(FALSE);
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::UpdateSizeInfo()
{
	if (m_pSelectedArchivInfo) {
		// Ist das Archiv einem Laufwerk fest zuzugeordnet
		CString sRoot = m_pSelectedArchivInfo->GetFixedDrive();
		CIPCDrive* pDrive = NULL;
		if (sRoot.IsEmpty() == FALSE) {
			pDrive = m_pDrives->GetDrive(sRoot+_T('\\'));
		}

		DWORD dwAvailableMBForArchive;
		DWORD dwAvailableMBTotal;
		DWORD dwReservedForOtherArchives;
		if (pDrive) {
			// Wieviel Platz ist auf dem Laufwerk
			dwAvailableMBTotal = pDrive->GetAvailableMB();
			// Wieviel ist denn bereits von anderen Archiven reserviert (alle - das aktuelle)
			dwReservedForOtherArchives = m_pArchivs->GetSizeMBForFixedDrive(sRoot)
											- m_pSelectedArchivInfo->GetSizeMB();
			dwAvailableMBForArchive = dwAvailableMBTotal - dwReservedForOtherArchives;
		}
		else {
			// Wieviel Platz ist auf allen Laufwerken
			dwAvailableMBTotal = m_pDrives->GetAvailableMB();
			// Wieviel ist denn bereits von anderen Archiven reserviert (alle - das aktuelle)
			dwReservedForOtherArchives = m_pArchivs->GetSizeMB()
											- m_pSelectedArchivInfo->GetSizeMB();
			dwAvailableMBForArchive = dwAvailableMBTotal - dwReservedForOtherArchives;
		}

		m_sMax.Format(_T("%d"), dwAvailableMBForArchive);
		m_sUsed.Format(_T("%d"),m_pArchivs->GetSizeMB());
		m_sWhole.Format(_T("%d"),m_pDrives->GetAvailableMB());

		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SelectedItemToControl()
{
	m_sName = m_pSelectedArchivInfo->GetName();
	m_dwSize = m_pSelectedArchivInfo->GetSizeMB();
	switch(m_pSelectedArchivInfo->GetType())
	{
	case ALARM_ARCHIV:
		m_iType = 1;
		break;
	case RING_ARCHIV:
		m_iType = 0;
		break;
	case SICHERUNGS_RING_ARCHIV:
		m_iType = 2;
		break;
	case ALARM_LIST_ARCHIV:
		m_iType = 3;
		break;
	}
	FillAlarmArchivCombo();

	CString fD = m_pSelectedArchivInfo->GetFixedDrive();
	fD.MakeLower();
	if (!fD.IsEmpty())
	{
		int i;
		i = m_comboFixedDrive.FindString(0,fD);
		if (i!=CB_ERR)
		{
			m_comboFixedDrive.SetCurSel(i);
		}
		else
		{
			m_comboFixedDrive.SetCurSel(0);
		}
	}
	else
	{
		m_comboFixedDrive.SetCurSel(0);
	}
	m_dwMaxTime = m_pSelectedArchivInfo->GetMaxStorageTime();
	m_bDynamicExpandArchiveSize = m_pSelectedArchivInfo->GetDynamicSize();
	UpdateSizeInfo();
	UpdateData(FALSE);
	m_editName.EnableWindow(m_pSelectedArchivInfo->GetArchivNr() != LOCAL_SEARCH_RESULT_ARCHIV_NR);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SelectItem(int iIndex)
{
	// We have to consider multiple selection, so deselect all first
	for (int i=0 ; i<m_Archivs.GetItemCount() ; i++)
	{
		m_Archivs.SetItemState(i,0,LVIS_SELECTED|LVIS_FOCUSED);
	}
	
	if (iIndex >= 0)
	{
		m_iSelectedItem = iIndex;
		m_Archivs.SetItemState(m_iSelectedItem,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		m_pSelectedArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(m_iSelectedItem);
		SelectedItemToControl();
	}
	else
	{
		m_iSelectedItem = -1;
		m_pSelectedArchivInfo = NULL;
		m_sName.Empty();
		m_dwSize = 0;
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::FillArchivListBox()
{
	CArchivInfo* pAI;
	for (int i=0; i<m_pArchivs->GetSize(); i++)
	{
		pAI = (CArchivInfo*)m_pArchivs->GetAt(i);
		InsertItem(pAI);
	}
}
/////////////////////////////////////////////////////////////////////////////
int	 CArchivPage::InsertItem(CArchivInfo* pAI)
{
	LV_ITEM lvis;
	CString sName, sNr, sTyp, sSafeRingFor, sSize, sFixedDrive, sMaxTime;
	int i,c,iImage = 0;

	sName = pAI->GetName();
	sFixedDrive = pAI->GetFixedDrive();
	sNr.Format(_T("%d"),pAI->GetArchivNr());
	CSecID idRingFor(SECID_GROUP_ARCHIVE,pAI->GetSafeRingFor());
	CArchivInfo* pAI2 = m_pArchivs->GetArchivInfo(idRingFor);
	if (pAI2)
	{
		sSafeRingFor = pAI2->GetName();
	}
	sSize.Format(_T("%d"),pAI->GetSizeMB());
	sMaxTime.Format(_T("%u"),pAI->GetMaxStorageTime());

	iImage = GetTypImage(pAI->GetType());
	sTyp = GetTypString(pAI->GetType());

	c = m_Archivs.GetItemCount();

	lvis.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	lvis.pszText = sName.GetBuffer(0);
	lvis.cchTextMax = sName.GetLength();
	lvis.lParam = (LPARAM)pAI;
	lvis.iItem = c;
	lvis.iSubItem = 0;
	lvis.iImage = iImage;

	i = m_Archivs.InsertItem(&lvis);
	sName.ReleaseBuffer();
	lvis.iItem = i;

	m_Archivs.SetItemText(i,1,sNr);
	m_Archivs.SetItemText(i,2,sTyp);
	m_Archivs.SetItemText(i,3,sSafeRingFor);
	m_Archivs.SetItemText(i,4,sSize);
	m_Archivs.SetItemText(i,5,sFixedDrive);
	m_Archivs.SetItemText(i,6,sMaxTime);

	return i;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::ClearArchivListBox()
{
	m_Archivs.DeleteAllItems();
	m_sName.Empty();
	m_iSelectedItem = -1;
	m_pSelectedArchivInfo = NULL;
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CArchivPage::CheckActivationsWithArchiv(CArchivInfo* pArchiveInfo)
{
	if (pArchiveInfo)
	{
		CSecID aID = pArchiveInfo->GetID();
		// check wether there is any Activation (CInputToOutput)
		// with this archive
		CInputGroup* pInputGroup;
		CInput* pInput;
		CInputToOutput* pInputToOutput;
		int i,j,k,c,d,e;
		BOOL bShowMB = TRUE;
		
		c = m_pInputList->GetSize();
		for (i=0;i<c;i++)
		{
			pInputGroup = m_pInputList->GetGroupAt(i);
			d = pInputGroup->GetSize();
			for (j=0;j<d;j++)
			{
				pInput = pInputGroup->GetInput(j);
				e = pInput->GetNumberOfInputToOutputs();
				for (k=e-1;k>=0;k--)
				{
					pInputToOutput = pInput->GetInputToOutput(k);
					if (pInputToOutput->GetArchiveID() == aID)
					{
						// we still have one
						if (bShowMB)
						{
							CString sMsg, sInfo, sInputs;
							sInputs.LoadString(IDS_MELDER);
							sInfo.Format(_T("%s: %s (%s)"), pArchiveInfo->GetName(), pInput->GetName(), sInputs);
							sMsg.Format(IDP_DELETE_ARCHIV_WITH_ACTIVATION, sInfo);
							if (IDYES==AfxMessageBox(sMsg, MB_YESNO))
							{
								bShowMB = FALSE;
							}
							else
							{
								return FALSE;
							}
						}
						pInput->DeleteInputToOutput(pInputToOutput);
					}
				} // inputstooutputs
			} // inputs
		}//groups

		BOOL bShowMBP = TRUE;
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CProcess*pProcess;
		for (i=0; i<pProcessList->GetSize();i++) 
		{
			pProcess = pProcessList->GetAt(i);
			if (   pProcess 
				&& pProcess->IsBackup() 
				&& pProcess->GetBackupArchiv() == pArchiveInfo->GetID()) 
			{
				if (bShowMBP)
				{
					CString sMsg, sInfo, sProzess;
					sProzess.LoadString(IDS_BACKUPPROCESSPAGE);
					sInfo.Format(_T("%s: %s (%s)"), pArchiveInfo->GetName(), pProcess->GetName(), sProzess);
					sMsg.Format(IDP_DELETE_ARCHIV_WITH_ACTIVATION, sInfo);
					if (IDYES==AfxMessageBox(sMsg, MB_YESNO))
					{
						bShowMBP = FALSE;
					}
					else
					{
						return FALSE;
					}
				}
				// mark process for delete
				m_ProcessesToDelete.Add(pProcess->GetID());
				break;
			}
		}

		if (bShowMB==FALSE)
		{
			m_bActivationWereDeleted = TRUE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::FillDriveCombo()
{
	m_comboFixedDrive.ResetContent();

	CString s;
	s.LoadString(IDS_NO_DRIVE);
	m_comboFixedDrive.AddString(s);

	int i,c;
	CIPCDrive* pDrive;

	c = m_pDrives->GetSize();

	for (i=0;i<c;i++)
	{
		pDrive = m_pDrives->GetAt(i);
		if (pDrive && (pDrive->IsDatabase() || pDrive->IsRemoveableDatabase()))
		{
			CString sM;
			sM.Format(_T(" %d MB"),pDrive->GetAvailableMB());
			if (pDrive->IsNetworkDrive())
			{
				CString sNet;
				sNet.LoadString(IDS_DRIVE_NET);
				sM += _T(" ") + sNet;
			}
			m_comboFixedDrive.AddString(pDrive->GetRootString() + sM);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::FillAlarmArchivCombo()
{
	m_comboAlarm.ResetContent();

	int i,c,iCurSel;
	CArchivInfo* pAI;

	c = m_pArchivs->GetSize();
	iCurSel = CB_ERR;

	// CAVEAT Pre-alarm for ring archive only since Version 4.2
	BOOL bRingArchiveAllowed = FALSE;
	CString sVersion = GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
	if (sVersion >= _T("4.2"))
	{
		bRingArchiveAllowed = TRUE;
	}

	for (i=0;i<c;i++)
	{
		pAI = m_pArchivs->GetAt(i);
		if (   (   (pAI->GetType() == ALARM_ARCHIV)
				|| (bRingArchiveAllowed && (pAI->GetType() == RING_ARCHIV))
				)
			&& (pAI->GetArchivNr() != 255)
			)
		{
			int g = m_comboAlarm.AddString(pAI->GetName());
			m_comboAlarm.SetItemData(g, pAI->GetID().GetID());
			if (m_pSelectedArchivInfo &&
				m_pSelectedArchivInfo->GetType() == SICHERUNGS_RING_ARCHIV &&
				m_pSelectedArchivInfo->GetSafeRingFor() == pAI->GetID().GetSubID())
			{
				iCurSel = g;
			}
		}
	}
	if (iCurSel != CB_ERR)
	{
		m_comboAlarm.SetCurSel(iCurSel);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::SetNewTyp(ArchivType at)
{
	UpdateData();
	CString s;
	if (m_Archivs.GetSelectedCount() > 1)
	{
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		CArchivInfo* pArchivInfo;
		int iPos;
		while (pos)
		{
			iPos = m_Archivs.GetNextSelectedItem(pos);
			pArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(iPos);
			if (pArchivInfo)
			{
				if (at != SICHERUNGS_RING_ARCHIV &&
					pArchivInfo->GetType() == SICHERUNGS_RING_ARCHIV)
				{
					pArchivInfo->SetSafeRingFor(0);
					m_Archivs.SetItemText(iPos,3,_T(""));
				}
				pArchivInfo->SetTyp(at);

				LV_ITEM lv;
				lv.mask = LVIF_IMAGE;
				lv.iItem = iPos;
				lv.iSubItem = 0;
				lv.iImage = GetTypImage(at);
				m_Archivs.SetItem(&lv);
				m_Archivs.SetItemText(iPos, 2, GetTypString(at));

				SetModified();
				m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
			}
		}
	}
	else if (m_pSelectedArchivInfo)
	{
		if (at != SICHERUNGS_RING_ARCHIV &&
			m_pSelectedArchivInfo->GetType() == SICHERUNGS_RING_ARCHIV)
		{
			m_pSelectedArchivInfo->SetSafeRingFor(0);
			m_Archivs.SetItemText(m_iSelectedItem,3,_T(""));
		}
		m_pSelectedArchivInfo->SetTyp(at);
		
		LV_ITEM lv;
		lv.mask = LVIF_IMAGE;
		lv.iItem = m_iSelectedItem;
		lv.iSubItem = 0;
		lv.iImage = GetTypImage(at);
		m_Archivs.SetItem(&lv);
		m_Archivs.SetItemText(m_iSelectedItem,2,GetTypString(at));

		SetModified();
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		EnableExceptNew();
		FillAlarmArchivCombo();
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CArchivPage::GetTypString(ArchivType at)
{
	CString sTyp;
	switch(at)
	{
	case ALARM_ARCHIV:
		sTyp.LoadString(IDS_ALARM_ARCHIV);
		break;
	case RING_ARCHIV:
		sTyp.LoadString(IDS_RING_ARCHIV);
		break;
	case SICHERUNGS_RING_ARCHIV:
		sTyp.LoadString(IDS_VORRING_ARCHIV);
		break;
	case SUCHERGEBNIS_ARCHIV:
		sTyp.LoadString(IDS_RING_ARCHIV);
		break;
	case ALARM_LIST_ARCHIV:
		sTyp.LoadString(IDS_ALARM_LIST);
	}
	return sTyp;
}
/////////////////////////////////////////////////////////////////////////////
int	CArchivPage::GetTypImage(ArchivType at)
{
	int iImage=0;
	switch(at)
	{
	case ALARM_ARCHIV:
		iImage = 1;
		break;
	case RING_ARCHIV:
		iImage = 0;
		break;
	case SICHERUNGS_RING_ARCHIV:
		iImage = 2;
		break;
	case SUCHERGEBNIS_ARCHIV:
		iImage = 1;
		break;
	case ALARM_LIST_ARCHIV:
		iImage = 3;
		break;
	}
	return iImage;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchivPage)
	DDX_Control(pDX, IDC_TXT_MAX_TIME, m_txtMaxTime);
	DDX_Control(pDX, IDC_EDIT_MAX_TIME, m_editMaxTime);
	DDX_Control(pDX, IDC_COMBO_FIXED_DRIVE, m_comboFixedDrive);
	DDX_Control(pDX, IDC_COMBO_ALARM, m_comboAlarm);
	DDX_Control(pDX, IDC_RADIO_RING, m_radioRing);
	DDX_Control(pDX, IDC_RADIO_ALARM, m_radioAlarm);
	DDX_Control(pDX, IDC_RADIO_SAFERING, m_radioVorring);
	DDX_Control(pDX, IDC_RADIO_ALARM_LIST, m_radioAlarmList);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_editSize);
	DDX_Control(pDX, IDC_NAME_ARCHIV, m_editName);
	DDX_Control(pDX, IDC_ARCHIVE, m_Archivs);
	DDX_Text(pDX, IDC_NAME_ARCHIV, m_sName);
	DDV_MaxChars(pDX, m_sName, 250);
	DDX_Radio(pDX, IDC_RADIO_RING, m_iType);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_dwSize);
	DDX_Text(pDX, IDC_STATIC_MAX_MB, m_sMax);
	DDX_Text(pDX, IDC_STATIC_USED_MB, m_sUsed);
	DDX_Text(pDX, IDC_STATIC_WHOLE_MB, m_sWhole);
	DDX_Text(pDX, IDC_EDIT_MAX_TIME, m_dwMaxTime);
	DDV_MinMaxDWord(pDX, m_dwMaxTime, 0, 4294967294);
	DDX_Check(pDX, IDC_CK_AUTO_REDUCE_ARCHIVES, m_bAutoReduceArchiveSize);
	DDX_Check(pDX, IDC_CK_DYNAMIC_EXPAND_ARCHIVES, m_bDynamicExpandArchiveSize);
	DDX_Control(pDX, IDC_CK_DYNAMIC_EXPAND_ARCHIVES, m_ctrlDynamicExpandArchive);
	//}}AFX_DATA_MAP


}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CArchivPage, CSVPage)
	//{{AFX_MSG_MAP(CArchivPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ARCHIVE, OnItemchangedArchive)
	ON_EN_CHANGE(IDC_NAME_ARCHIV, OnChangeName)
	ON_EN_CHANGE(IDC_EDIT_SIZE, OnChangeEditSize)
	ON_CBN_SELCHANGE(IDC_COMBO_ALARM, OnSelchangeComboAlarm)
	ON_BN_CLICKED(IDC_RADIO_ALARM, OnRadioAlarm)
	ON_BN_CLICKED(IDC_RADIO_RING, OnRadioRing)
	ON_BN_CLICKED(IDC_RADIO_SAFERING, OnRadioSafering)
	ON_BN_CLICKED(IDC_RADIO_ALARM_LIST, OnRadioAlarmList)
	ON_CBN_SELCHANGE(IDC_COMBO_FIXED_DRIVE, OnSelchangeComboFixedDrive)
	ON_NOTIFY(NM_CLICK, IDC_ARCHIVE, OnClickArchive)
	ON_NOTIFY(NM_DBLCLK, IDC_ARCHIVE, OnDblclkArchive)
	ON_NOTIFY(NM_RCLICK, IDC_ARCHIVE, OnRclickArchive)
	ON_NOTIFY(NM_RDBLCLK, IDC_ARCHIVE, OnRdblclkArchive)
	ON_EN_CHANGE(IDC_EDIT_MAX_TIME, OnChangeEditMaxTime)
	ON_BN_CLICKED(IDC_CK_AUTO_REDUCE_ARCHIVES, OnBnClickedAutoReduceArchiveSize)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CK_DYNAMIC_EXPAND_ARCHIVES, OnBnClickedCkDynamicExpandArchives)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CArchivPage message handlers
BOOL CArchivPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	m_Images.Create(IDB_ARCHIVE,20,1,RGB(0,255,255));

	m_Archivs.SetImageList(&m_Images,LVSIL_SMALL);

	CRect			rect;
	LV_COLUMN		lvcolumn;
	const iColumns = 7;
	int w[iColumns] = {20,6,9,20,15,15,15};	// CAVET add sum has to be 100
	CString s[iColumns];

	m_Archivs.GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_NAME)->GetWindowText(s[0]);
	s[1].LoadString(IDS_NR);
	GetDlgItem(IDC_GROUP_TYP)->GetWindowText(s[2]);
	GetDlgItem(IDC_RADIO_SAFERING)->GetWindowText(s[3]);
	s[4].LoadString(IDS_SIZE_MB);
	s[5].LoadString(IDS_FIXED_DRIVE);
	m_txtMaxTime.GetWindowText(s[6]);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	for (int i = 0; i < iColumns; i++)  // add the columns to the list control
	{
		s[i].Replace(_T(":"), _T(""));
		s[i].Replace(_T("&"), _T(""));
		lvcolumn.cx = (rect.Width() * w[i])/100;
		lvcolumn.pszText = (LPTSTR)LPCTSTR(s[i]);
		lvcolumn.iSubItem = i;
		m_Archivs.InsertColumn(i, &lvcolumn);  // assumes return value is OK.
		s[i].ReleaseBuffer();
	}
	
	LONG dw = m_Archivs.GetExtendedStyle();
	dw |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_Archivs.SetExtendedStyle(dw);
	m_Archivs.ModifyStyle(LVS_SINGLESEL,0);

	FillDriveCombo();

	CString sVersion = GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
	BOOL bAlarmList = FALSE;
#ifdef _DEBUG
	bAlarmList = TRUE;
#else
	bAlarmList = (sVersion >= _T("5.0.4"));
#endif
	CWnd* pWnd = GetDlgItem(IDC_RADIO_ALARM_LIST);
	if (pWnd)
		pWnd->ShowWindow(bAlarmList ? SW_SHOW : SW_HIDE);

	CancelChanges();
	IsDataValid();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnNew()
{
	if ( IsDataValid() ) 
	{
		CArchivInfo* pAI = m_pArchivs->AddArchivInfo();
		// copy actual setting from selected archiv
		CopyActualSettingsToNew(pAI);

		CString s;
		s.LoadString(IDS_NEW_ARCHIV);
		pAI->SetName(s);
//		m_dwSize = pAI->GetSizeMB();
		int i = InsertItem(pAI);
		SelectItem(i);
//		m_sName = s;
//		UpdateData(FALSE);
		SetModified(TRUE,FALSE);
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);

		EnableExceptNew();

		m_editName.SetFocus();
		m_editName.SetSel(0,-1);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnDelete()
{
	if ( (m_iSelectedItem==-1) || (m_pSelectedArchivInfo==NULL))
	{
		return;
	}

	BOOL bDeleteOK = TRUE;
	BOOL bDeleted = FALSE;
	// m_iSelected may be changed by DeleteItem, so save the current selection
	int iNewSel = m_iSelectedItem;	// new selection
	int iCount = m_Archivs.GetSelectedCount();
	if (iCount)
	{
		CArchivInfo* pArchiveInfo = NULL;
		int *pnPos = new int[iCount];
		int i = 0, j = 0;
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		while (pos)
		{
			pnPos[i++] = m_Archivs.GetNextSelectedItem(pos);
		}
		// first check all activations
		for (i=0 ; bDeleteOK && i<iCount; i++)
		{
			pArchiveInfo = (CArchivInfo*)m_Archivs.GetItemData(pnPos[i]);
			bDeleteOK &= CheckActivationsWithArchiv(pArchiveInfo);
		}
		// second the real delete if ok
		if (bDeleteOK)
		{
			// before the deleting the member has to be cleared, to avoid SelectItem problems
			m_pSelectedArchivInfo = NULL;
			for (i=iCount-1; i>=0; i--)
			{
				pArchiveInfo = (CArchivInfo*)m_Archivs.GetItemData(pnPos[i]);
				if (pArchiveInfo->GetID().GetSubID() == LOCAL_SEARCH_RESULT_ARCHIV_NR)
				{
					continue;
				}
				CHost* pHost = m_pHostArray->GetLocalHost();
				if (pHost)
				{
					if (pHost->GetAlarmArchive() == pArchiveInfo->GetID())
					{
						pHost->SetAlarmArchive(SECID_NO_ID);
						m_bHostArchivesWereDeleted = TRUE;
					}
					else if (pHost->GetNormalArchive() == pArchiveInfo->GetID())
					{
						pHost->SetNormalArchive(SECID_NO_ID);
						m_bHostArchivesWereDeleted = TRUE;
					}
				}

				for (j=0 ; j<m_pHostArray->GetSize() ; j++)
				{
					pHost = m_pHostArray->GetAt(j);
					if (pHost)
					{
						if (pHost->GetAlarmArchive() == pArchiveInfo->GetID())
						{
							pHost->SetAlarmArchive(SECID_NO_ID);
							m_bHostArchivesWereDeleted = TRUE;
						}
						else if (pHost->GetNormalArchive() == pArchiveInfo->GetID())
						{
							pHost->SetNormalArchive(SECID_NO_ID);
							m_bHostArchivesWereDeleted = TRUE;
						}
						else if (pHost->GetSearchArchive() == pArchiveInfo->GetID())
						{
							pHost->SetSearchArchive(SECID_NO_ID);
							m_bHostArchivesWereDeleted = TRUE;
						}
					}
				}
				m_pArchivs->DeleteArchivInfo(pArchiveInfo);
				bDeleted |= m_Archivs.DeleteItem(pnPos[i]);
				// DeleteItem has called OnItemchangedArchive, so clear again
				m_pSelectedArchivInfo = NULL;
			}
		}
		delete pnPos;
	}

	if (bDeleted)	// remove from listCtrl
	{
		// calc new selection
		int newCount = m_Archivs.GetItemCount();	// pre-subtract before DeleteItem
		if (iCount == 1) // it was a single selection
		{
			// range check
			if (iNewSel > newCount-1) 
			{	// outside ?
				iNewSel = newCount-1;
			}
		}
		else // was multiple selection
		{
			iNewSel = (newCount>0 ? 0 :-1);
		}

		SelectItem(iNewSel);
		SetModified(TRUE,FALSE);
		m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		EnableExceptNew();
		m_Archivs.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnItemchangedArchive(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iItem != m_iSelectedItem)
	{
		if (pNMListView->uNewState & LVIS_SELECTED)
		{
			m_pSelectedArchivInfo = (CArchivInfo*)pNMListView->lParam;
			m_iSelectedItem = pNMListView->iItem;
			SelectedItemToControl();
			UpdateData(FALSE);
			EnableExceptNew();
		}
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnClickArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnDblclkArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRclickArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRdblclkArchive(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem!=m_iSelectedItem)
	{
		// Kann sein, dass ins leere Feld geklickt wurde
		// daher Select-Markierung wiederherstellen
		m_Archivs.SetItemState(m_iSelectedItem,
									LVIS_SELECTED|LVIS_FOCUSED,
									LVIS_SELECTED|LVIS_FOCUSED);
	}	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChangeName() 
{
	UpdateData();

	if (m_Archivs.GetSelectedCount() > 1)
	{
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		CArchivInfo* pArchivInfo;
		int iPos;
		CString sName;
		while (pos)
		{
			iPos = m_Archivs.GetNextSelectedItem(pos);
			pArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(iPos);
			if (pArchivInfo)
			{
				pArchivInfo->SetName(m_sName);
				m_Archivs.SetItemText(iPos, 0, m_sName);
			}
		}
	}
	else
	{
		if (m_pSelectedArchivInfo)
		{
			m_pSelectedArchivInfo->SetName(m_sName);
			m_Archivs.SetItemText(m_iSelectedItem, 0, m_sName);
		}
		else
		{
			//no host selected -> create and select a new one
			CArchivInfo* pAI = m_pArchivs->AddArchivInfo();

			pAI->SetName(m_sName);
			int i = InsertItem(pAI);
			SelectItem(i);
		}
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChange()
{
	SetModified(TRUE,FALSE);
	m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnBnClickedAutoReduceArchiveSize()
{
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnBnClickedCkDynamicExpandArchives()
{
	UpdateData();
	CString s;
	if (m_Archivs.GetSelectedCount() > 1)
	{
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		CArchivInfo* pArchivInfo;
		int iPos;
		while (pos)
		{
			iPos = m_Archivs.GetNextSelectedItem(pos);
			pArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(iPos);
			if (   pArchivInfo 
				&& (pArchivInfo->GetType() == RING_ARCHIV))
			{
				pArchivInfo->SetDynamicSize(m_bDynamicExpandArchiveSize);
			}
		}
	}
	else if (   m_pSelectedArchivInfo
			    && m_pSelectedArchivInfo->GetType() == RING_ARCHIV)
	{
		m_pSelectedArchivInfo->SetDynamicSize(m_bDynamicExpandArchiveSize);
	}		

	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChangeEditSize() 
{
	// Verhindert ein leeres Editfeld.
	CString sSize;
	m_editSize.GetWindowText(sSize.GetBuffer(255), 255);
	sSize.ReleaseBuffer(-1);
	if (sSize.IsEmpty())
	{
		m_editSize.SetWindowText(_T("1"));
		m_editSize.SetSel(0,-1);
	}

	UpdateData();
	m_dwSize = min(m_dwSize, 999999999); // Maximal 999 TeraByte

	CString s;
	if (m_Archivs.GetSelectedCount() > 1)
	{
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		CArchivInfo* pArchivInfo;
		int iPos;
		while (pos)
		{
			iPos = m_Archivs.GetNextSelectedItem(pos);
			pArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(iPos);
			if (pArchivInfo)
			{
				pArchivInfo->SetSizeMB(m_dwSize);
				s.Format(_T("%d"),m_dwSize);
				m_Archivs.SetItemText(iPos, 4, s);
			}
		}
	}
	else if (m_pSelectedArchivInfo)
	{
		m_pSelectedArchivInfo->SetSizeMB(m_dwSize);
		s.Format(_T("%d"),m_dwSize);
		m_Archivs.SetItemText(m_iSelectedItem, 4, s);
	}	
	// Groessen aktualisieren
	UpdateSizeInfo();

	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnSelchangeComboAlarm() 
{
	int iCurSel;
	CSecID id;

	iCurSel = m_comboAlarm.GetCurSel();
	if (iCurSel != CB_ERR &&
		m_pSelectedArchivInfo &&
		m_pSelectedArchivInfo->GetType() == SICHERUNGS_RING_ARCHIV)
	{
		id = m_comboAlarm.GetItemData(iCurSel);
		CArchivInfo* pAA = m_pArchivs->GetArchivInfo(id);
		if (   pAA 
			&& (   (pAA->GetType() == ALARM_ARCHIV)
			    || (pAA->GetType() == RING_ARCHIV)
				)
			)
		{
			m_pSelectedArchivInfo->SetSafeRingFor(id.GetSubID());
			m_Archivs.SetItemText(m_iSelectedItem,3,pAA->GetName());
			SetModified();
			m_pParent->AddInitApp(WK_APP_NAME_DB_SERVER);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnChangeEditMaxTime() 
{
	// Verhindert ein leeres Editfeld.
	CString sMaxTime;
	m_editMaxTime.GetWindowText(sMaxTime);
	if (sMaxTime.IsEmpty())
	{
		m_editMaxTime.SetWindowText(_T("0"));
		m_editMaxTime.SetSel(0,-1);
	}
	UpdateData();

	if (m_Archivs.GetSelectedCount() > 1)
	{
		POSITION pos = m_Archivs.GetFirstSelectedItemPosition();
		CArchivInfo* pArchivInfo;
		int iPos;
		while (pos)
		{
			iPos = m_Archivs.GetNextSelectedItem(pos);
			pArchivInfo = (CArchivInfo*)m_Archivs.GetItemData(iPos);
			if (pArchivInfo)
			{
				pArchivInfo->SetMaxStorageTime(m_dwMaxTime);
				sMaxTime.Format(_T("%u"), m_dwMaxTime);
				m_Archivs.SetItemText(iPos, 6, sMaxTime);
			}
		}
	}
	else if (m_pSelectedArchivInfo)
	{
		m_pSelectedArchivInfo->SetMaxStorageTime(m_dwMaxTime);
		sMaxTime.Format(_T("%u"), m_dwMaxTime);
		m_Archivs.SetItemText(m_iSelectedItem, 6, sMaxTime);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRadioAlarm() 
{
	SetNewTyp(ALARM_ARCHIV);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRadioRing() 
{
	SetNewTyp(RING_ARCHIV);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRadioAlarmList()
{
	SetNewTyp(ALARM_LIST_ARCHIV);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnRadioSafering() 
{
	SetNewTyp(SICHERUNGS_RING_ARCHIV);
}
/////////////////////////////////////////////////////////////////////////////
void CArchivPage::OnSelchangeComboFixedDrive() 
{
	int iCurSel = m_comboFixedDrive.GetCurSel();

	if (iCurSel!=CB_ERR)
	{
		// Get selection and save previous settings
		CString sFixed;
		m_comboFixedDrive.GetLBText(iCurSel,sFixed);
		CString sPrevFix = m_pSelectedArchivInfo->GetFixedDrive();

		// Set data to objects for calculation of size and validation
		CString sRoot = _T("");
		if (sFixed.GetLength()>=2 && sFixed[1]==_T(':'))
		{
			sRoot = sFixed.Left(2);
		}
		m_pSelectedArchivInfo->SetFixedDrive(sRoot);
		m_Archivs.SetItemText(m_iSelectedItem, 5, sRoot);
		UpdateSizeInfo();

		OnChange();
		// Wir wollen Fehler so frueh wie moeglich abfangen
		// Ausserdem: Wenn die Einzel-Archiv-Groesse zu gross ist,
		// kann die Gesamt-Archiv-Groesse das DWORD ueberschreiten
		if (IsDataValid() == FALSE)
		{
			// undo the changes
			m_pSelectedArchivInfo->SetFixedDrive(sPrevFix);
			m_Archivs.SetItemText(m_iSelectedItem, 5, sPrevFix);
			sPrevFix.MakeLower();
			int i = 0;
			if (!sPrevFix.IsEmpty())
			{
				i = m_comboFixedDrive.FindString(0, sPrevFix);
				if (i == CB_ERR)
				{
					i = 0;
				}
			}
			m_comboFixedDrive.SetCurSel(i);
		}
	}
}

