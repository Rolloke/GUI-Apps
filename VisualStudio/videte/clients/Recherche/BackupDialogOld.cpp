// BackupDialogOld.cpp: implementation of the CBackupDialogOld class.
//

#include "stdafx.h"
#include "recherche.h"
#include "BackupDialogOld.h"

#include "Server.h"
#include "RechercheDoc.h"
#include "RechercheView.h"
#include "ObjectView.h"
#include "CopyReadOnlyVersion.h"

#include "SelectBackupDriveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBackupDialogOld dialog
CBackupDialogOld::CBackupDialogOld(CServer* pServer, CRechercheView* pParent /*=NULL*/)
	: CBackupDialog(CBackupDialogOld::IDD, pServer, pParent)
{
	//{{AFX_DATA_INIT(CBackupDialogOld)
	m_sSize = _T("");
	m_sVelocity = _T("");
	m_sBackupName = _T("");
	//}}AFX_DATA_INIT

	m_sBackupName.LoadString(IDS_TITLE_BACKUP);
	CSystemTime st;
	st.GetLocalTime();
	m_sBackupName += " " + st.GetDateTime();

	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CBackupDialogOld::~CBackupDialogOld()
{
	m_SequencesToCopy.DeleteAll();
	m_ArchivesToCopy.DeleteAll();
}


void CBackupDialogOld::DoDataExchange(CDataExchange* pDX)
{
	CBackupDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackupDialogOld)
	DDX_Control(pDX, IDC_LIST_ARCHIVES, m_ctrlArchives);
	DDX_Control(pDX, IDC_CDR, m_ctrlCDR);
	DDX_Control(pDX, IDC_TXT_STATE, m_staticState);
	DDX_Control(pDX, IDC_TXT_SELECTED_MB, m_staticToCopy);
	DDX_Control(pDX, IDC_TXT_TYPE, m_staticType);
	DDX_Control(pDX, IDC_TXT_MB_FREE, m_staticFree);
	DDX_Control(pDX, IDC_TIME_S, m_staticTime);
	DDX_Control(pDX, IDC_CLOSE, m_ctrlClose);
	DDX_Control(pDX, IDCANCEL, m_ctrlCancel);
	DDX_Control(pDX, IDOK, m_ctrlOK);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS, m_ctrlProgress);
	DDX_Control(pDX, IDC_LIST_SEQUENCES, m_ctrlSequences);
	DDX_Control(pDX, IDC_COMBO_DESTINATION, m_comboDestination);
	DDX_Text(pDX, IDC_SIZE_MB, m_sSize);
	DDX_Text(pDX, IDC_VELOCITY_KBS, m_sVelocity);
	DDX_Text(pDX, IDC_EDIT_BACKUP_NAME, m_sBackupName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackupDialogOld, CBackupDialog)
	//{{AFX_MSG_MAP(CBackupDialogOld)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTINATION, OnSelchangeComboDestination)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CDR, OnCdr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackupDialogOld message handlers

void CBackupDialogOld::OnOK() 
{
	int i;
	int c = m_SequencesToCopy.GetSize();
	int d = m_ArchivesToCopy.GetSize();

	if (theApp.m_pUser)
	{
		WK_TRACE_USER(_T("%s startet backup\n"), theApp.m_pUser->GetName());
	}

	if ((c+d)>0)
	{
		UpdateData();

		DWORD* pDWORD = new DWORD[c+d];
		WORD wArchiveNr,wSequenceNr;

		for (i=0;i<c;i++)
		{
			wArchiveNr = m_SequencesToCopy.GetAt(i)->GetArchiveNr();
			wSequenceNr = m_SequencesToCopy.GetAt(i)->GetSequenceNr();
			pDWORD[i] = MAKELONG(wSequenceNr,wArchiveNr); // low,high
			WK_TRACE(_T("requesting backup a=%d, s=%d\n"),wArchiveNr,wSequenceNr);
		}
		for (i=0;i<d;i++)
		{
			wArchiveNr = m_ArchivesToCopy.GetAt(i)->GetID();
			wSequenceNr = 0;
			pDWORD[c+i] = MAKELONG(wSequenceNr,wArchiveNr); // low,high
			WK_TRACE(_T("requesting backup a=%d,\n"),wArchiveNr);
		}

		m_ctrlOK.EnableWindow(FALSE);
		m_ctrlCancel.EnableWindow(TRUE);
		m_ctrlClose.EnableWindow(FALSE);

		m_i64BytesToCopy = m_SequencesToCopy.GetSizeBytes();
		m_i64BytesToCopy += m_ArchivesToCopy.GetSizeBytes();
		DWORD dwMBytes = m_i64BytesToCopy.GetInMB();
		m_ctrlProgress.SetRange32(0,dwMBytes);
		m_ctrlProgress.SetPos(0);
		m_dwStartTime = GetTickCount();

		CString s;
		s.LoadString(IDS_BACKUP_RUNNING);
		m_staticState.SetWindowText(s);

		SetTimer(1,1000,NULL);

		m_pParent->GetDocument()->GetObjectView()->EnableWindow(FALSE);

		CIPCDatabaseRecherche* pDatabase = m_pServer->GetDatabase();
		pDatabase->DoRequestBackup(m_dwBackupID,
									m_pDrive->GetRootString(),
									m_sBackupName,
									c+d,pDWORD);
		WK_DELETE_ARRAY(pDWORD);

	}
	else
	{
		// no sequences
		CString s;
		s.LoadString(IDP_NOTHING_TO_BACKUP);
		COemGuiApi::MessageBox(s,20,MB_OK|MB_ICONSTOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnCancel() 
{
	if (IDYES==AfxMessageBox(IDS_CANCEL_BACKUP,MB_YESNO|MB_ICONQUESTION))
	{
		CIPCDatabaseRecherche* pDatabase = m_pServer->GetDatabase();

		pDatabase->DoRequestCancelBackup(m_dwBackupID);

		CString s;
		s.LoadString(IDS_BACKUP_CANCELLED);
		m_staticState.SetWindowText(s);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnSelchangeComboDestination() 
{
	int iCurSel = m_comboDestination.GetCurSel();

	if (iCurSel!=CB_ERR)
	{
		CString sRoot;
		m_comboDestination.GetLBText(iCurSel,sRoot);

		WK_DELETE(m_pDrive);
		m_pDrive = new CIPCDrive(*(m_pServer->GetDrives().GetDrive(sRoot)));

		if (m_pDrive)
		{
			SelectDrive(m_pDrive);
		}
	}
	m_ctrlOK.EnableWindow(CheckSpaceOnDestination());
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::SelectDrive(CIPCDrive* pDrive)
{
	CString sFree,sType;
	sFree.Format(_T("%d"),pDrive->GetFreeMB());
	m_staticFree.SetWindowText(sFree);

	switch (pDrive->GetType())
	{
	case DRIVE_UNKNOWN:
		// The drive type cannot be determined. 
		break;
	case DRIVE_NO_ROOT_DIR:
		// The root directory does not exist. 
		break;
	case DRIVE_REMOVABLE:
		// The disk can be removed from the drive. 
		sType.LoadString(IDS_DRIVE_FD);
		break;
	case DRIVE_FIXED:
		// The disk cannot be removed from the drive. 
		sType.LoadString(IDS_DRIVE_HD);
		break;
	case DRIVE_REMOTE:
		// The drive is a remote (network) drive. 
		sType.LoadString(IDS_DRIVE_NET);
		break;
	case DRIVE_CDROM:
		// The drive is a CD-ROM drive. 
		sType.LoadString(IDS_DRIVE_CD);
		break;
	case DRIVE_RAMDISK:
		// The drive is a RAM disk
		break;
	}
	m_staticType.SetWindowText(sType);
	m_ctrlCDR.EnableWindow(m_pDrive->IsCDROM());
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialogOld::OnInitDialog() 
{
	CBackupDialog::OnInitDialog();

	if (InitDrives() == FALSE)
	{
		PostMessage(WM_COMMAND,
					MAKEWPARAM(m_ctrlClose.GetDlgCtrlID(), BN_CLICKED),
					(LPARAM)m_ctrlClose.m_hWnd);
	}

	int iCount = m_comboDestination.GetCount();
	if (iCount > 0)
	{
		m_comboDestination.SetCurSel(0);
		m_comboDestination.EnableWindow(iCount>1);
		CString sRoot;
		m_comboDestination.GetLBText(0,sRoot);

		m_pDrive = new CIPCDrive(*(m_pServer->GetDrives().GetDrive(sRoot)));

		if (m_pDrive)
		{
			SelectDrive(m_pDrive);
		}
		CheckSpaceOnDestination();
	}

	m_ctrlOK.EnableWindow(FALSE);
	m_ctrlCancel.EnableWindow(FALSE);

	CString s;
	s.LoadString(IDS_BACKUP_NOT_STARTED);
	m_staticState.SetWindowText(s);

	s.Format(_T("%d"),m_i64BytesToCopy.GetInMB());
	m_staticToCopy.SetWindowText(s);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialogOld::InitDrives()
{
	BOOL bReturn = TRUE;
	BOOL bDirectCDInit = FALSE;
	CIPCDrive* pDrive = NULL;
	CIPCDrives& drives = m_pServer->GetDrives();
	drives.Lock();
	for (int i=0;i<drives.GetSize();i++)
	{
		pDrive = drives.GetAt(i);
		if (pDrive->IsWriteBackup())
		{
			if (   m_pServer->IsLocal() 
				|| !pDrive->IsCDROM())
			{
				m_comboDestination.AddString(pDrive->GetRootString());
			}
			if (   (pDrive->GetFSProducer() == FSP_UDF_ADAPTEC_DIRECTCD)
				&& (bDirectCDInit == FALSE)
				)
			{
				InitDirectCDLanguageStrings();
				// init only once
				bDirectCDInit = TRUE;
			}
		}
	}
	drives.Unlock();

	int iCount = m_comboDestination.GetCount();
	
	if (iCount == 0)
	{
		// OOPS no drive
		WK_DELETE(m_pDrive);
		CSelectBackupDriveDialog dlg(this);
		
		if (IDOK==dlg.DoModal())
		{
			m_comboDestination.AddString(dlg.GetRootString());
		}
		else
		{
			// no drive no backup
			EnableAll(FALSE);
			m_ctrlClose.EnableWindow();
			bReturn = FALSE;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::AddArchive(CIPCArchivRecord* pArchiv)
{
	m_ArchivesToCopy.Add(new CIPCArchivRecord(*pArchiv));

	CString s;
	s.Format(_T("%d, %s"),pArchiv->GetID(),pArchiv->GetName());
	int i = m_ctrlArchives.AddString(s);
	m_ctrlArchives.SetItemData(i,pArchiv->GetID());
	DeleteSequences(pArchiv->GetID());
	OnChanged();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialogOld::DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences)
{
	BOOL bRet = FALSE;
	for (int i=0;i<m_ArchivesToCopy.GetSize();i++)
	{
		CIPCArchivRecord* pPivot = m_ArchivesToCopy.GetAt(i);
		if (pPivot->GetID()==wArchivNr)
		{
			// delete it from list
			RemoveArchiveFromList(wArchivNr);
			m_ArchivesToCopy.RemoveAt(i);
			WK_DELETE(pPivot);
			bRet = TRUE; 
			break;
		}
	}
	if (!bRet && bDeleteSequences)
	{
		DeleteSequences(wArchivNr);
	}
	OnChanged();
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnChanged()
{
	m_i64BytesToCopy  = m_SequencesToCopy.GetSizeBytes();
	m_i64BytesToCopy += m_ArchivesToCopy.GetSizeBytes();

	CString s;
	s.Format(_T("%d"),m_i64BytesToCopy.GetInMB());
	m_staticToCopy.SetWindowText(s);

	m_ctrlOK.EnableWindow((m_i64BytesToCopy > 0) 
					      && CheckSpaceOnDestination());
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::AddSequence(CIPCSequenceRecord* pSequence)
{
	CString s,n;
	n = pSequence->GetName();
	if (n.IsEmpty())
	{
		n = pSequence->GetTime().GetDateTime();
	}
	s.Format(_T("%d, %s"),pSequence->GetArchiveNr(),n);
	if (m_ctrlSequences.FindStringExact(-1, s) == LB_ERR)
	{
		int i = m_ctrlSequences.AddString(s);
		m_ctrlSequences.SetItemData(i,MAKELONG(pSequence->GetSequenceNr(),
					pSequence->GetArchiveNr()));
		m_SequencesToCopy.Add(new CIPCSequenceRecord(*pSequence));
		OnChanged();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::DeleteSequences(WORD wArchivNr)
{
	for (int i=m_SequencesToCopy.GetSize()-1;i>=0;i--)
	{
		CIPCSequenceRecord* pSequencePivot = m_SequencesToCopy.GetAt(i);
		if (pSequencePivot->GetArchiveNr() == wArchivNr)
		{
			// delete it from list
			RemoveSequenceFromList(MAKELONG(pSequencePivot->GetSequenceNr(),
				pSequencePivot->GetArchiveNr()));

			m_SequencesToCopy.RemoveAt(i);
			WK_DELETE(pSequencePivot);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBackupDialogOld::DeleteSequence(CIPCSequenceRecord* pSequence)
{
	BOOL bRet = FALSE;
	// first check wether archive is checked
	// and one sequence will removed
	if (DeleteArchive(pSequence->GetArchiveNr(),FALSE))
	{
		// the whole archive was selected and now removed
		// now all sequences of the archive
		// except pSequence must be added
		CRechercheDoc* pDoc = m_pParent->GetDocument();
		CObjectView* pOV = pDoc->GetObjectView();
		pOV->AddAllBackupSequencesExcept(m_pServer->GetID(),pSequence);
		bRet = FALSE;
	}
	else
	{
		for (int i=0;i<m_SequencesToCopy.GetSize();i++)
		{
			CIPCSequenceRecord* pSequencePivot = m_SequencesToCopy.GetAt(i);
			if (   (pSequencePivot->GetSequenceNr() == pSequence->GetSequenceNr())
				&& (pSequencePivot->GetArchiveNr() == pSequence->GetArchiveNr())
				)
			{
				// delete it from list
				RemoveSequenceFromList(MAKELONG(pSequence->GetSequenceNr(),
					pSequence->GetArchiveNr()));

				m_SequencesToCopy.RemoveAt(i);
				WK_DELETE(pSequencePivot);
				break;
			}
		}
		bRet = TRUE;
	}
	OnChanged();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr)
{
	WK_TRACE(_T("confirm backup a=%d, s=%d\n"), wArchiveNr, wSequenceNr);

	CIPCSequenceRecord* pSequence = m_SequencesToCopy.GetSequence(wArchiveNr, wSequenceNr);
	
	if (pSequence)
	{
		// a real sequence
		m_i64BytesCopied +=  pSequence->GetSize();
		RemoveSequenceFromList(MAKELONG(wSequenceNr, wArchiveNr));
		m_SequencesToCopy.RemoveSequence(wArchiveNr, wSequenceNr);
	}
	else
	{
		if (wArchiveNr!=0)
		{
			// sequence from an archive
			if (wSequenceNr ==0)
			{
				// the last one of the archive
				CIPCArchivRecord* pArchiv = m_ArchivesToCopy.GetArchiv(wArchiveNr);
				if (pArchiv)
				{
					RemoveArchiveFromList(wArchiveNr);
					m_ArchivesToCopy.RemoveArchiv(wArchiveNr);
				}
			}
			else
			{
				pSequence = m_pServer->GetCIPCSequenceRecord(wArchiveNr, wSequenceNr);
				if (pSequence)
				{
					m_i64BytesCopied +=  pSequence->GetSize();
				}
			}
		}
	}

	m_ctrlProgress.SetPos(m_i64BytesCopied.GetInMB());

	DWORD dwKBToCopy = (DWORD)(m_i64BytesToCopy.GetInt64() / 1024);
	DWORD dwKBCopied = (DWORD)(m_i64BytesCopied.GetInt64() / 1024);
	m_sSize.Format(_T("%d / %d KBytes"),dwKBCopied,dwKBToCopy);

	DWORD dwNow		= GetTickCount();
	DWORD dwElapsed = dwNow - m_dwStartTime;

	if (dwElapsed!=0)
	{
		m_dwBytesPerSecond = (DWORD)((m_i64BytesCopied.GetInt64()*1000) / dwElapsed);
	}

	m_sVelocity.Format(_T("%d kByte/s"),m_dwBytesPerSecond / 1024);

	CIPCInt64 iBytesStillToCopy = m_i64BytesToCopy;
	iBytesStillToCopy -= m_i64BytesCopied;

	if (m_dwBytesPerSecond!=0)
	{
		m_dwCountDownInSeconds = (DWORD)(iBytesStillToCopy.GetInt64() / m_dwBytesPerSecond);
	}

	UpdateData(FALSE);
	
	if (	(wSequenceNr==0)
		&& 	(wArchiveNr==0) )
	{
		BackupFinished();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::CancelBackupConfirmed(DWORD dwBackupID)
{
	m_ctrlCancel.EnableWindow(FALSE);
	m_ctrlClose.EnableWindow(TRUE);
	KillTimer(1);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::BackupFinished()
{
	WK_TRACE(_T("backup finished %d\n"),m_dwBackupID);
	m_ctrlCancel.EnableWindow(FALSE);
	m_ctrlClose.EnableWindow(TRUE);

	CString s;
	s.LoadString(IDS_BACKUP_FINISHED);
	m_staticState.SetWindowText(s);

	KillTimer(1);
	m_ctrlProgress.SetPos(0);
	m_dwCountDownInSeconds = 0;
	CountDownToControl();

	// copy reader for cd/r
	if (m_pDrive && m_pDrive->IsCDROM())
	{
		CopyReadOnlyVersion();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::BackupError(LPARAM lParam)
{
	CString sError;
	sError.LoadString(IDS_BACKUP_FAILED);
	m_staticState.SetWindowText(sError);

	m_ctrlCancel.EnableWindow(FALSE);
	m_ctrlClose.EnableWindow(TRUE);
	KillTimer(1);

	if (lParam == 2)
	{
		AfxMessageBox(IDP_BACKUP_INITIALIZE_FAILED,MB_OK|MB_ICONSTOP);
	}
	else if (lParam == 3)
	{
		AfxMessageBox(IDP_BACKUP_COPY_FAILED,MB_OK|MB_ICONSTOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::RemoveSequenceFromList(DWORD dwID)
{
	int i,c;

	c = m_ctrlSequences.GetCount();
	for (i=0;i<c;i++)
	{
		DWORD data = m_ctrlSequences.GetItemData(i);
		if (data==dwID)
		{
			m_ctrlSequences.DeleteString(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::RemoveArchiveFromList(WORD wID)
{
	int i,c;

	c = m_ctrlArchives.GetCount();
	for (i=0;i<c;i++)
	{
		DWORD data = m_ctrlArchives.GetItemData(i);
		if (data==wID)
		{
			m_ctrlArchives.DeleteString(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::CountDownToControl()
{
	CString sTime;
	sTime.Format(_T("%02d:%02d:%02d"),m_dwCountDownInSeconds/3600,
				 (m_dwCountDownInSeconds/60)%60,
				 m_dwCountDownInSeconds%60);
	m_staticTime.SetWindowText(sTime);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1)
	{
		// the file copy process
		if (m_SequencesToCopy.GetSize()+m_ArchivesToCopy.GetSize())
		{
			if (m_dwCountDownInSeconds>0)
			{
				m_dwCountDownInSeconds--;
			}
			CountDownToControl();
		}
	}
	else if (nIDEvent==2)
	{
		// the copy read only version process
		if (m_pCopyReadOnlyVersion->IsRunning())
		{
			// running
		}
		else
		{
			// finished
			CopyReadOnlyVersionFinished();
		}
	}
	
	CBackupDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::CopyReadOnlyVersionFinished()
{
	KillTimer(2);
	WK_DELETE(m_pCopyReadOnlyVersion);
	CString s;
	s.LoadString(IDS_COPY_ROV_DONE);
	m_staticState.SetWindowText(s);
	m_ctrlClose.EnableWindow();
	AskForCDRFinalize();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::CopyReadOnlyVersion()
{
	m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(m_pDrive->GetRootString());

	if (m_pCopyReadOnlyVersion->IsAlreadyDone())
	{
		WK_DELETE(m_pCopyReadOnlyVersion);
		AskForCDRFinalize();
		return;
	}

	m_ctrlClose.EnableWindow(FALSE);
	CString s;
	s.LoadString(IDS_COPY_ROV);
	m_staticState.SetWindowText(s);
	SetTimer(2,1000,NULL);

	m_pCopyReadOnlyVersion->StartThread();
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData/*ignored*/)
{
	if (bInsert)
	{
		WK_TRACE(_T("CD/R inserted\n"));
	}
	else
	{
		WK_TRACE(_T("CD/R removed\n"));
	}
	WK_TRACE(_T("überprüfe Sicherungslaufwerke\n"));
	CString s;
	s.LoadString(IDS_CDR_CHECK);
	m_staticType.SetWindowText(s);
	Sleep(500);
	if (m_pDrive)
	{
		m_pDrive->CheckSpace();
		SelectDrive(m_pDrive);
	}
	m_ctrlOK.EnableWindow(CheckSpaceOnDestination(bFormat));
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnCdr() 
{
	if (m_pDrive && m_pDrive->IsCDROM())
	{
		m_pDrive->CheckSpace();
		SelectDrive(m_pDrive);
		m_ctrlOK.EnableWindow(CheckSpaceOnDestination());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CBackupDialogOld::OnClose() 
{
	if (theApp.m_bBackupModus)
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT);
	}
	else
	{
		m_pParent->GetDocument()->GetObjectView()->EnableWindow(TRUE);
		m_pParent->OnBackupClosed();
	}
}
