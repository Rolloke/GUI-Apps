// DlgBackupOld.cpp: implementation of the CDlgBackupOld class.
//

#include "stdafx.h"
#include "IdipClient.h"
#include "mainfrm.h"
#include "DlgBackupOld.h"

#include "Server.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewArchive.h"
#include "CopyReadOnlyVersion.h"

#include "DlgSelectBackupDrive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupOld dialog
CDlgBackupOld::CDlgBackupOld(CServer* pServer, CViewIdipClient* pParent /*=NULL*/)
	: CDlgBackup(CDlgBackupOld::IDD, pServer, pParent)
{
	//{{AFX_DATA_INIT(CDlgBackupOld)
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
CDlgBackupOld::~CDlgBackupOld()
{
	WK_TRACE(_T("CDlgBackupOld::~CDlgBackupOld():MainThread:%d\n"), theApp.IsInMainThread());
	m_SequencesToCopy.SafeDeleteAll();
	m_ArchivesToCopy.SafeDeleteAll();
}


void CDlgBackupOld::DoDataExchange(CDataExchange* pDX)
{
	CDlgBackup::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBackupOld)
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


BEGIN_MESSAGE_MAP(CDlgBackupOld, CDlgBackup)
	//{{AFX_MSG_MAP(CDlgBackupOld)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTINATION, OnSelchangeComboDestination)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CDR, OnCdr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgBackupOld message handlers

void CDlgBackupOld::OnOK() 
{
	int i;
	m_SequencesToCopy.Lock(_T(__FUNCTION__));
	int c = m_SequencesToCopy.GetSize();
	Lock(_T(__FUNCTION__));
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
			wArchiveNr = m_SequencesToCopy.GetAtFast(i)->GetArchiveNr();
			wSequenceNr = m_SequencesToCopy.GetAtFast(i)->GetSequenceNr();
			pDWORD[i] = MAKELONG(wSequenceNr,wArchiveNr); // low,high
			WK_TRACE(_T("requesting backup a=%d, s=%d\n"),wArchiveNr,wSequenceNr);
		}
		m_SequencesToCopy.Unlock();

		for (i=0;i<d;i++)
		{
			wArchiveNr = m_ArchivesToCopy.GetAtFast(i)->GetID();
			wSequenceNr = 0;
			pDWORD[c+i] = MAKELONG(wSequenceNr,wArchiveNr); // low,high
			WK_TRACE(_T("requesting backup a=%d,\n"),wArchiveNr);
		}
		Unlock();


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

		theApp.GetMainFrame()->GetViewArchive()->EnableWindow(FALSE);

		CServer *pServer = GetServer();
		if (pServer)
		{
			CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();
			pDatabase->DoRequestBackup(m_dwBackupID,
										m_pDrive->GetRootString(),
										m_sBackupName,
										c+d,pDWORD);
		}
		WK_DELETE_ARRAY(pDWORD);

	}
	else
	{
		Unlock();
		m_SequencesToCopy.Unlock();
		// no sequences
		CString s;
		s.LoadString(IDP_NOTHING_TO_BACKUP);
		COemGuiApi::MessageBox(s,20,MB_OK|MB_ICONSTOP);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::OnCancel() 
{
	if (IDYES==AfxMessageBox(IDS_CANCEL_BACKUP,MB_YESNO|MB_ICONQUESTION))
	{
		CServer *pServer = GetServer();
		if (pServer)
		{
			CIPCDatabaseIdipClient* pDatabase = pServer->GetDatabase();

			pDatabase->DoRequestCancelBackup(m_dwBackupID);

			CString s;
			s.LoadString(IDS_BACKUP_CANCELLED);
			m_staticState.SetWindowText(s);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::OnSelchangeComboDestination() 
{
	int iCurSel = m_comboDestination.GetCurSel();

	if (iCurSel!=CB_ERR)
	{
		CString sRoot;
		m_comboDestination.GetLBText(iCurSel,sRoot);

		WK_DELETE(m_pDrive);
		CServer *pServer = GetServer();
		if (pServer)
		{
			m_pDrive = new CIPCDrive(*(pServer->GetDrives().GetDrive(sRoot)));
		}

		if (m_pDrive)
		{
			SelectDrive(m_pDrive);
		}
	}
	m_ctrlOK.EnableWindow(CheckSpaceOnDestination());
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::SelectDrive(CIPCDrive* pDrive)
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
BOOL CDlgBackupOld::OnInitDialog() 
{
	CDlgBackup::OnInitDialog();

	if (InitDrives() == FALSE)
	{
		PostMessage(WM_COMMAND,
					MAKEWPARAM(m_ctrlClose.GetDlgCtrlID(), BN_CLICKED),
					(LPARAM)m_ctrlClose.m_hWnd);
	}

	int iCount = m_comboDestination.GetCount();
	CServer *pServer = GetServer();
	if (pServer && iCount > 0)
	{
		m_comboDestination.SetCurSel(0);
		m_comboDestination.EnableWindow(iCount>1);
		CString sRoot;
		m_comboDestination.GetLBText(0,sRoot);

		m_pDrive = new CIPCDrive(*(pServer->GetDrives().GetDrive(sRoot)));

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
BOOL CDlgBackupOld::InitDrives()
{
	BOOL bReturn = TRUE;
	BOOL bDirectCDInit = FALSE;
	CIPCDrive* pDrive = NULL;
	CServer *pServer = GetServer();
	if (pServer)
	{
		CIPCDrives& drives = pServer->GetDrives();
		drives.Lock(_T(__FUNCTION__));
		for (int i=0;i<drives.GetSize();i++)
		{
			pDrive = drives.GetAtFast(i);
			if (   pDrive->IsEnabled()
				&& !pDrive->IsSystem()
				&& (   pDrive->IsWriteBackup()					// as backup drive declared
				    || pDrive->GetType() == DRIVE_REMOVABLE))	// removeable drives like memory stick
			{
				if (   pServer->IsLocal() 
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
			{
				CAutoCritSec acs(&m_ArchivesToCopy.m_cs);
				WK_DELETE(m_pDrive);
			}
			if (m_pSelectDrive == NULL)
			{
				m_pSelectDrive = new CDlgSelectBackupDrive(this);
				
				INT_PTR nResult = m_pSelectDrive->DoModal();
				if (IDOK==nResult)
				{
					m_comboDestination.AddString(m_pSelectDrive->GetRootString());
				}
				else if (-1 == nResult)	// database disconnect
				{
					PostMessage(WM_USER, IDC_CLOSE, NULL);
					bReturn = FALSE;
				}
				else
				{
					// no drive no backup
					EnableAll(FALSE);
					m_ctrlClose.EnableWindow();
					bReturn = FALSE;
				}

				WK_DELETE(m_pSelectDrive);
			}
			else
			{
				m_pSelectDrive->InsertDrives();
			}
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::AddArchive(CIPCArchivRecord* pArchiv)
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
BOOL CDlgBackupOld::DeleteArchive(WORD wArchivNr, BOOL bDeleteSequences)
{
	BOOL bRet = FALSE;
	for (int i=0;i<m_ArchivesToCopy.GetSize();i++)
	{
		CIPCArchivRecord* pPivot = m_ArchivesToCopy.GetAtFast(i);
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
void CDlgBackupOld::OnChanged()
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
void CDlgBackupOld::AddSequence(CIPCSequenceRecord* pSequence)
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
		m_SequencesToCopy.SafeAdd(new CIPCSequenceRecord(*pSequence));
		OnChanged();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::DeleteSequences(WORD wArchivNr)
{
	if (!theApp.IsInMainThread())
	{
		WK_TRACE(_T("CDlgBackupOld::DeleteSequences is not in MainThread\n"));
	}

	m_SequencesToCopy.Lock(_T(__FUNCTION__));
	for (int i=m_SequencesToCopy.GetSize()-1;i>=0;i--)
	{
		CIPCSequenceRecord* pSequencePivot = m_SequencesToCopy.GetAtFast(i);
		if (pSequencePivot->GetArchiveNr() == wArchivNr)
		{
			// delete it from list
			RemoveSequenceFromList(MAKELONG(pSequencePivot->GetSequenceNr(),
				pSequencePivot->GetArchiveNr()));

			m_SequencesToCopy.RemoveAt(i);
			WK_DELETE(pSequencePivot);
		}
	}
	m_SequencesToCopy.Unlock();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDlgBackupOld::DeleteSequence(CIPCSequenceRecord* pSequence)
{
	BOOL bRet = FALSE;
	// first check wether archive is checked
	// and one sequence will removed
	if (DeleteArchive(pSequence->GetArchiveNr(),FALSE))
	{
		// the whole archive was selected and now removed
		// now all sequences of the archive
		// except pSequence must be added
		CViewArchive* pOV = theApp.GetMainFrame()->GetViewArchive();
		pOV->AddAllBackupSequencesExcept(m_wServerID, pSequence);
		bRet = FALSE;
	}
	else
	{
		m_SequencesToCopy.Lock(_T(__FUNCTION__));
		for (int i=0;i<m_SequencesToCopy.GetSize();i++)
		{
			CIPCSequenceRecord* pSequencePivot = m_SequencesToCopy.GetAtFast(i);
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
		m_SequencesToCopy.Unlock();
		bRet = TRUE;
	}
	OnChanged();

	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::BackupConfirmed(WORD wArchiveNr, WORD wSequenceNr)
{
	WK_TRACE(_T("confirm backup a=%d, s=%d, MainThread:%d\n"), wArchiveNr, wSequenceNr, theApp.IsInMainThread());

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
				CServer *pServer = GetServer();
				if (pServer)
				{
					pSequence = pServer->GetCIPCSequenceRecord(wArchiveNr, wSequenceNr);
					if (pSequence)
					{
						m_i64BytesCopied +=  pSequence->GetSize();
					}
				}
			}
		}
	}

	m_ctrlProgress.SetPos(m_i64BytesCopied.GetInMB());

	DWORD dwKBToCopy = (DWORD)(m_i64BytesToCopy.GetInt64() / 1024);
	DWORD dwKBCopied = (DWORD)(m_i64BytesCopied.GetInt64() / 1024);
	m_sSize.Format(_T("%d / %d KBytes"),dwKBCopied,dwKBToCopy);

	int nElapsed = GetTimeSpanSigned(m_dwStartTime);
	if (nElapsed!=0)
	{
		m_dwBytesPerSecond = (DWORD)((m_i64BytesCopied.GetInt64()*1000) / nElapsed);
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
void CDlgBackupOld::CancelBackupConfirmed(DWORD dwBackupID)
{
	m_ctrlCancel.EnableWindow(FALSE);
	m_ctrlClose.EnableWindow(TRUE);
	KillTimer(1);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::BackupFinished()
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
void CDlgBackupOld::BackupError(LPARAM lParam)
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
void CDlgBackupOld::RemoveSequenceFromList(DWORD dwID)
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
void CDlgBackupOld::RemoveArchiveFromList(WORD wID)
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
void CDlgBackupOld::CountDownToControl()
{
	CString sTime;
	sTime.Format(_T("%02d:%02d:%02d"),m_dwCountDownInSeconds/3600,
				 (m_dwCountDownInSeconds/60)%60,
				 m_dwCountDownInSeconds%60);
	m_staticTime.SetWindowText(sTime);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::OnTimer(UINT nIDEvent) 
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
	
	CDlgBackup::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::CopyReadOnlyVersionFinished()
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
void CDlgBackupOld::CopyReadOnlyVersion()
{
	m_pCopyReadOnlyVersion = new CCopyReadOnlyVersion(m_pDrive->GetRootString(), TRUE);

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
void CDlgBackupOld::HandleDeviceChange(BOOL bInsert, BOOL bFormat, DWORD dwData/*ignored*/)
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
void CDlgBackupOld::OnCdr() 
{
	if (m_pDrive && m_pDrive->IsCDROM())
	{
		m_pDrive->CheckSpace();
		SelectDrive(m_pDrive);
		m_ctrlOK.EnableWindow(CheckSpaceOnDestination());
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDlgBackupOld::OnClose() 
{
//	RKE: BackupMode obsolete
//	if (theApp.m_bBackupModus)
//	{
//		AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_APP_EXIT);
//		Do not post ID_APP_EXIT, this will also close the launcher
//	}
//	else
	{
		theApp.GetMainFrame()->GetViewArchive()->EnableWindow(TRUE);
		m_pParent->OnBackupClosed();
	}
}
