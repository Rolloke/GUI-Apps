// DVSTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DVSTest.h"
#include "DVSTestDlg.h"
#include "IPCDatabaseTest.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FPS_COUNTER_RESET	-1

extern CDVSTestApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CDVSTestDlg dialog

CDVSTestDlg::CDVSTestDlg(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CDVSTestDlg::IDD, pParent)
	, m_sChecking(_T(""))
	, m_sFPSEverage(_T(""))
{
	//{{AFX_DATA_INIT(CDVSTestDlg)
	m_sVolume = _T("");
	m_sPIN = _T("0000");
	m_sRecords = _T("");
	m_sChecking = _T("");
	m_sCopied = _T("");
	m_sError = _T("");
	m_bShowMD = FALSE;
	m_bShowFPS = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCIPCDatabaseTest = NULL;
	m_dwCalcTick = 0;
	m_bGotAlarmList = FALSE;
	m_sPath = theApp.m_sPath;

	m_bCheckPics = FALSE;
	m_dwWholePics = 0;
	m_iCountSecondsForFPS = FPS_COUNTER_RESET;
	m_iLastSecond = FPS_COUNTER_RESET;
	m_flEverageFPS_CurrSequence = 0;

}
CDVSTestDlg::~CDVSTestDlg()
{
	WK_DELETE(m_pCIPCDatabaseTest);
}

void CDVSTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDVSTestDlg)
	DDX_Control(pDX, IDC_CHECK_PICS, m_ctrlCheckOrder);
	DDX_Control(pDX, IDC_DISCONNECT, m_btnDisconnect);
	DDX_Control(pDX, IDC_CONNECT, m_btnConnect);
	DDX_Control(pDX, IDC_BUTTON_SHOW, m_btnShow);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
	DDX_Control(pDX, IDC_BUTTON_CALC, m_btnCalc);
	DDX_Control(pDX, IDC_BUTTON_BROWSE, m_btnBrowse);
	DDX_Control(pDX, IDC_BUTTON_BACKUP, m_btnBackup);
	DDX_Control(pDX, IDC_ALARM_LIST, m_btnAlarmList);
	DDX_Control(pDX, IDC_EDIT_PIN, m_editPIN);
	DDX_Control(pDX, IDC_LIST_RECORDS, m_Records);
	DDX_Control(pDX, IDC_COMBO_HOSTS, m_Hosts);
	DDX_Control(pDX, IDC_START_TIME, m_StartTime);
	DDX_Control(pDX, IDC_START_DATE, m_StartDate);
	DDX_Control(pDX, IDC_END_TIME, m_EndTime);
	DDX_Control(pDX, IDC_END_DATE, m_EndDate);
	DDX_Control(pDX, IDC_LIST_SEQUENCES, m_ListSequences);
	DDX_Control(pDX, IDC_LIST_ARCHIVES, m_ListArchives);
	DDX_Text(pDX, IDC_TXT_VOLUME, m_sVolume);
	DDX_Text(pDX, IDC_EDIT_PIN, m_sPIN);
	DDX_Text(pDX, IDC_TXT_RECORDS, m_sRecords);
	DDX_Text(pDX, IDC_TXT_CHECHING, m_sChecking);
	DDX_Text(pDX, IDC_TXT_PATH, m_sPath);
	DDX_Text(pDX, IDC_TXT_COPIED, m_sCopied);
	DDX_Text(pDX, IDC_ERROR, m_sError);
	DDX_Check(pDX, IDC_CHECKMD, m_bShowMD);
	DDX_Check(pDX, IDC_CHECK_FPS, m_bShowFPS);
	//}}AFX_DATA_MAP


	DDX_Text(pDX, IDC_FPS_EVERAGE, m_sFPSEverage);
}

BEGIN_MESSAGE_MAP(CDVSTestDlg, CWK_Dialog)
	//{{AFX_MSG_MAP(CDVSTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ARCHIVES, OnItemchangedListArchives)
	ON_BN_CLICKED(IDC_BUTTON_CALC, OnButtonCalc)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, OnButtonShow)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BACKUP, OnButtonBackup)
	ON_BN_CLICKED(IDC_ALARM_LIST, OnAlarmList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEQUENCES, OnItemchangedListSequences)
	ON_BN_CLICKED(IDC_CHECK_PICS, OnCheckPics)
	ON_BN_CLICKED(IDC_CHECKMD, OnCheckmd)
	ON_BN_CLICKED(IDC_CHECK_FPS, OnCheckFPS)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER,OnUser)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDVSTestDlg message handlers

BOOL CDVSTestDlg::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CRect rect;
	m_ListArchives.GetClientRect(rect);
	int w = rect.Width();
	m_ListArchives.InsertColumn(0,_T("A-Nr"),LVCFMT_LEFT,2*w/5);
	m_ListArchives.InsertColumn(1,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListArchives.InsertColumn(2,_T("Typ"),LVCFMT_LEFT,w/5);
	m_ListArchives.InsertColumn(3,_T("Größe"),LVCFMT_LEFT,w/5);
	m_ListArchives.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListSequences.GetClientRect(rect);
	w = rect.Width();
	m_ListSequences.InsertColumn(0,_T("A-Nr"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(1,_T("S-Nr"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(2,_T("Name"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(3,_T("Anzahl"),LVCFMT_LEFT,w/5);
	m_ListSequences.InsertColumn(4,_T("Größe"),LVCFMT_LEFT,w/5);
	m_ListSequences.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_Records.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_EndDate.SetFormat(_T("dd.MM.yy"));
	m_StartDate.SetFormat(_T("dd.MM.yy"));

	CSystemTime t;
	if (!theApp.m_sStart.IsEmpty())
	{
		t.SetDBDate(theApp.m_sStart.Left(8));
		t.SetDBTime(theApp.m_sStart.Right(6));
	}
	else
	{
		t.GetLocalTime();
	}
	m_StartDate.SetTime(&t);
	m_StartTime.SetTime(&t);
	if (!theApp.m_sEnd.IsEmpty())
	{
		t.SetDBDate(theApp.m_sEnd.Left(8));
		t.SetDBTime(theApp.m_sEnd.Right(6));
	}
	else
	{
		t.GetLocalTime();
	}
	m_EndDate.SetTime(&t);
	m_EndTime.SetTime(&t);

	CWK_Dongle dongle;
	CString sPath;

	if (   dongle.GetProductCode() == IPC_DTS
		|| dongle.GetProductCode() == IPC_DTS_RECEIVER
		|| dongle.GetProductCode() == IPC_DTS_IP_RECEIVER)
	{
		sPath = _T("DVRT\\DV");
	}
	else
	{
		sPath = _T("DVRT");
	}

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,sPath,_T(""));
	CHostArray ha;
	ha.Load(wkp);

	CHost* pHost = ha.GetLocalHost();
	int j = m_Hosts.AddString(pHost->GetName());
	m_Hosts.SetItemData(j,pHost->GetID().GetID());
	m_Hosts.SetCurSel(j);

	for (int i=0;i<ha.GetSize();i++)
	{
		pHost = ha.GetAt(i);
		j = m_Hosts.AddString(pHost->GetName());
		m_Hosts.SetItemData(j,pHost->GetID().GetID());
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDVSTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CWK_Dialog::OnPaint();
	}

	Enable();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDVSTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////
LRESULT CDVSTestDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case CIPC_DB_CONFIRM_INFO:
		FillArchives();
		break;
	case CIPC_DB_CONFIRM_SEQUENCE_LIST:
		FillSequences((WORD)lParam);
		break;
	case 1234:
		OnDisconnect();
		break;
	case CIPC_DB_CONFIRM_BACKUP_BY_TIME:
		{
			CString s;
			s.Format(_T("%s Bytes in %d ms"),m_sVolume,GetTimeSpan(m_dwCalcTick));
			m_sVolume = s;
		}
		UpdateData(FALSE);
		break;
	case CIPC_DB_INDICATE_DELETE_SEQUENCE:
		DeleteSequence(HIWORD(lParam),LOWORD(lParam));
		break;
	case CIPC_DB_CONFIRM_RECORDS:
		if(m_bCheckPics)
		{
			CheckOrder_CheckRecords(HIWORD(lParam),LOWORD(lParam));
		}
		else
		{
			FillRecords(HIWORD(lParam),LOWORD(lParam));
		}
		break;
	case CIPC_DB_CONFIRM_ALARM_LIST_ARCHIVES:
		FillArchives();
		m_bGotAlarmList = TRUE;
		Enable();
		break;
	}
	return 0L;
}
/////////////////////////////////////////////////////////////
void CDVSTestDlg::FillArchives()
{
	m_ListArchives.DeleteAllItems();
	for (int i=0;i<m_pCIPCDatabaseTest->GetRecords().GetSize();i++)
	{
		CIPCArchivRecord* pAR = m_pCIPCDatabaseTest->GetRecords().GetAt(i);
		CString s;
		s.Format(_T("0x%04x|%d"),pAR->GetID(),pAR->GetID());
		int j = m_ListArchives.InsertItem(m_ListArchives.GetItemCount(),s);
		m_ListArchives.SetItemText(j,1,pAR->GetName());
		if (pAR->IsAlarm())
		{
			s = _T("Alarm");
		}
		else if (pAR->IsAlarmList())
		{
			s = _T("Alarmliste");
		}
		else if (pAR->IsBackup())
		{
			s = _T("Backup");
		}
		else if (pAR->IsRing())
		{
			s = _T("Ring");
		}
		else if (pAR->IsSafeRing())
		{
			s = _T("Vorring");
		}
		else if (pAR->IsSearch())
		{
			s = _T("Suche");
		}
		else
		{
			s = _T("Unbekannt");
		}
		m_ListArchives.SetItemText(j,2,s);
		s.Format(_T("%dMB"),CIPCInt64(pAR->BenutzterPlatz()).GetInMB());
		m_ListArchives.SetItemText(j,3,s);
		m_ListArchives.SetItemData(j,pAR->GetID());
	}

	for (i=0;i<m_ListArchives.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListArchives.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}
	Enable();
}

void CDVSTestDlg::OnItemchangedListArchives(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	int i = pNMListView->iItem;
	DWORD dwData = m_ListArchives.GetItemData(i);
	WORD wArchivNr = (WORD)dwData;
	if (LVIS_SELECTED & pNMListView->uNewState)
	{
		m_pCIPCDatabaseTest->DoRequestSequenceList(wArchivNr);
	}
	else
	{
		DeleteSequences(wArchivNr);
	}
	Enable();
	
	*pResult = 0;
}
void CDVSTestDlg::DeleteSequences(WORD wArchiveNr)
{
	if (m_ListSequences.GetItemCount()>0)
	{
		for (int i=m_ListSequences.GetItemCount()-1;i>=0;i--)
		{
			DWORD dwData = m_ListSequences.GetItemData(i);
			if (wArchiveNr == HIWORD(dwData))
			{
				m_ListSequences.DeleteItem(i);
			}
		}
	}
	Enable();
}
void CDVSTestDlg::DeleteSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	if (m_ListSequences.GetItemCount()>0)
	{
		for (int i=m_ListSequences.GetItemCount()-1;i>=0;i--)
		{
			DWORD dwData = m_ListSequences.GetItemData(i);
			if (   wArchiveNr == HIWORD(dwData)
				&& wSequenceNr == LOWORD(dwData))
			{
				m_ListSequences.DeleteItem(i);
			}
		}
	}
	Enable();
}
void CDVSTestDlg::FillSequences(WORD wArchiveNr)
{
	m_ctrlCheckOrder.EnableWindow();
	DeleteSequences(wArchiveNr);
	CIPCSequenceRecords& seq=m_pCIPCDatabaseTest->GetSequences();
	seq.Lock();

	for (int i=0;i<seq.GetSize();i++)
	{
		CIPCSequenceRecord* pR = seq.GetAt(i);
		if (pR->GetArchiveNr()==wArchiveNr)
		{
			CString s;
			s.Format(_T("%04x"),pR->GetArchiveNr());
			int j=m_ListSequences.InsertItem(m_ListSequences.GetItemCount(),s);
			s.Format(_T("%d"),pR->GetSequenceNr());
			m_ListSequences.SetItemText(j,1,s);
			m_ListSequences.SetItemText(j,2,pR->GetName());
			s.Format(_T("%d"),pR->GetNrOfPictures());
			m_ListSequences.SetItemText(j,3,s);
			m_ListSequences.SetItemText(j,4,pR->GetSize().Format(TRUE));
			m_ListSequences.SetItemData(j,MAKELONG(pR->GetSequenceNr(),pR->GetArchiveNr()));

		}
	}

	seq.Unlock();
	for (i=0;i<m_ListSequences.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_ListSequences.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}
	Enable();
}

static BOOL bForward = TRUE;
void CDVSTestDlg::OnButtonCalc() 
{
	int iNumSel = m_ListArchives.GetSelectedCount();
	int r = 0;
	DWORD* pIDs = NULL;	

	if (iNumSel>0)
	{
		pIDs = new DWORD[iNumSel];

		if (bForward)
		{
			for (int i=0;i<m_ListArchives.GetItemCount();i++)
			{
				if (LVIS_SELECTED & m_ListArchives.GetItemState(i,LVIS_SELECTED))
				{
					WORD wArchiveNr = (WORD)m_ListArchives.GetItemData(i);
					TRACE(_T("calc archiv %04hx\n"),wArchiveNr);
					pIDs[r++] = MAKELONG(0,wArchiveNr);
				}
			}
		}
		else
		{
			for (int i=m_ListArchives.GetItemCount()-1;i>=0;i--)
			{
				if (LVIS_SELECTED & m_ListArchives.GetItemState(i,LVIS_SELECTED))
				{
					WORD wArchiveNr = (WORD)m_ListArchives.GetItemData(i);
					TRACE(_T("calc archiv %04hx\n"),wArchiveNr);
					pIDs[r++] = MAKELONG(0,wArchiveNr);
				}
			}
		}

		CSystemTime s,e,t;

		m_StartTime.GetTime(&s);
		m_StartDate.GetTime(&t);
		s.wDay = t.wDay;
		s.wDayOfWeek = t.wDayOfWeek;
		s.wMonth = t.wMonth;
		s.wYear = t.wYear;
		m_EndTime.GetTime(&e);
		m_EndDate.GetTime(&t);
		e.wDay = t.wDay;
		e.wDayOfWeek = t.wDayOfWeek;
		e.wMonth = t.wMonth;
		e.wYear = t.wYear;

		TRACE(_T("calc from %s to %s\n"),s.GetDateTime(),e.GetDateTime());

		m_dwCalcTick = GetTickCount();

		m_pCIPCDatabaseTest->DoRequestBackupByTime(1,_T(""),_T(""),r,pIDs,s,e,BBT_CALCULATE);
	}

	WK_DELETE_ARRAY(pIDs);

	bForward = !bForward;
}

void CDVSTestDlg::OnConnect() 
{
	m_sError.Empty();
	UpdateData();
	if (!m_pCIPCDatabaseTest)
	{
		CWK_Dongle dongle;
		CString sPath;

		if (   dongle.GetProductCode() == IPC_DTS
			|| dongle.GetProductCode() == IPC_DTS_RECEIVER
			|| dongle.GetProductCode() == IPC_DTS_IP_RECEIVER)
		{
			sPath = _T("DVRT\\DV");
		}
		else
		{
			sPath = _T("DVRT");
		}

		CWK_Profile wkp(CWK_Profile::WKP_REGPATH,HKEY_LOCAL_MACHINE,sPath,_T(""));
		CHostArray ha;
		ha.Load(wkp);

		CIPCFetch fetch;
		CConnectionRecord c;
		CSecID id = m_Hosts.GetItemData(m_Hosts.GetCurSel());
		CHost* pHost = ha.GetHost(id);
		if (pHost)
		{
			CString sNumber = pHost->GetNumber();

			if (id == SECID_LOCAL_HOST)
			{
				sNumber = LOCAL_LOOP_BACK;
			}

			c.SetPassword(m_sPIN);
			c.SetPermission(_T("SuperVisor"));
			c.SetDestinationHost(sNumber);
			c.SetFieldValue(_T("PIN"),m_sPIN);

			CIPCFetchResult r = fetch.FetchDatabase(c);

			if (r.IsOkay())
			{
				m_pCIPCDatabaseTest = new CIPCDatabaseTest(this,r.GetShmName());
				m_sError.Empty();
			}
			else
			{
				CIPCError  Error = r.GetError();
				m_sError.Format(_T("Error:  %s"), NameOfEnum(Error));
			}

			UpdateData(FALSE);
		}
	}
	Enable();
}

void CDVSTestDlg::OnDisconnect() 
{
	WK_DELETE(m_pCIPCDatabaseTest);
	m_ListArchives.DeleteAllItems();
	m_ListSequences.DeleteAllItems();
	m_Records.DeleteAllItems();
	for (int j=m_Records.GetHeaderCtrl()->GetItemCount()-1;j>=0;j--)
	{
		m_Records.DeleteColumn(j);
	}
	m_sRecords.Empty();
	m_sChecking.Empty();
	Enable();
}

void CDVSTestDlg::OnButtonDelete() 
{

	BOOL bDelete = FALSE;
	bDelete = MessageBox(_T("Soll die Sequenz wirklich gelöscht werden?\n\n"),
					              NULL,
							   	  MB_OKCANCEL | MB_ICONQUESTION);
	if(bDelete == IDOK)
	{
		for (int i=0;i<m_ListSequences.GetItemCount();i++)
		{
			if (LVIS_SELECTED & m_ListSequences.GetItemState(i,LVIS_SELECTED))
			{
				DWORD dwData = m_ListSequences.GetItemData(i);
				WORD wArchiveNr = HIWORD(dwData);
				WORD wSequenceNr = LOWORD(dwData);
				m_pCIPCDatabaseTest->DoRequestDeleteSequence(wArchiveNr,wSequenceNr);
			}
		}
		m_Records.DeleteAllItems();
		for (int j=m_Records.GetHeaderCtrl()->GetItemCount()-1;j>=0;j--)
		{
			m_Records.DeleteColumn(j);
		}
		m_sRecords.Empty();
		m_sChecking.Empty();
		Enable();
		UpdateData(FALSE);
	}

}

void CDVSTestDlg::OnButtonShow() 
{
	m_bCheckPics = FALSE;

	if (m_ListSequences.GetSelectedCount() == 1)

	{
		// get all the records
		for (int i=0;i<m_ListSequences.GetItemCount();i++)
		{
			if (LVIS_SELECTED & m_ListSequences.GetItemState(i,LVIS_SELECTED))
			{
				DWORD dwData = m_ListSequences.GetItemData(i);
				WORD wArchiveNr = HIWORD(dwData);
				WORD wSequenceNr = LOWORD(dwData);
				m_pCIPCDatabaseTest->DoRequestRecords(wArchiveNr,wSequenceNr,0,0);
				break;
			}
		}
	}
	else if (m_Records.GetSelectedCount() == 1)
	{
		// show image
	}
	Enable();
}
void CDVSTestDlg::FillRecords(WORD wArchiveNr, WORD wSequenceNr)
{
	int i = 0;
	m_Records.DeleteAllItems();
	for (i=m_Records.GetHeaderCtrl()->GetItemCount()-1;i>=0;i--)
	{
		m_Records.DeleteColumn(i);
	}

	CIPCFields& fields = m_pCIPCDatabaseTest->GetSequenceFields();
	int* pLengths = new int[fields.GetSize()];
	int iSum = 0;
	CString sFieldName;
	int iCountRecordsWithFPS = 0;
	int iFieldTIME = 0;
	
	m_Records.InsertColumn(0,_T("Nr"));
	for (i=0;i<fields.GetSize();i++)
	{
		CIPCField* pField = fields[i];
		pLengths[i] = pField->GetMaxLength();
		iSum += pField->GetMaxLength();
		sFieldName = pField->GetName().Mid(4);
		if (   m_bShowMD
			|| 0!=sFieldName.Find(_T("MD")))
		{
			m_Records.InsertColumn(i+1,sFieldName);
		}

		if(m_bShowFPS)
		{
			if(sFieldName == _T("TIME")) 
			{
				if(iFieldTIME == 0)
				{
					//merken, an welcher Stelle das DB-Feld "TIME" steht
					iFieldTIME = i;
				}
			}
		}
	}

	if(m_bShowFPS)
	{	
		CString sFPS(_T("FPS"));
		m_Records.InsertColumn(i+2,sFPS);
		iCountRecordsWithFPS = m_Records.GetHeaderCtrl()->GetItemCount();

		m_iCountSecondsForFPS = FPS_COUNTER_RESET; //reset Sekundenzähler für FPS: Sekunde kann nie 100 sein
		m_iLastSecond = FPS_COUNTER_RESET;
	}

	CIPCFields& records = m_pCIPCDatabaseTest->GetSequenceRecords();
	m_flEverageFPS_CurrSequence = 0;
	//for (i=0;i<10;i++)
	for (i=0;i<records.GetSize();i++)	//gehe alle records der sequenz durch
	{
		CIPCField* pRecord = records.GetAt(i);
		CString s,v;
		int l = 0;
		if(pRecord)
		{
			v = pRecord->GetValue();
		}
		else
		{
			TRACE(_T("kein pRecord da\n"));
			break;
		}
		
		s.Format(_T("%04d"),i+1);
		int index = m_Records.InsertItem(m_Records.GetItemCount(),s);

		int j=0;
		CString sSekunde;

		int iSizeFields = fields.GetSize();
		if(m_bShowFPS && m_bShowMD) //bei FPS und MD muss ein Feld rangehängt werden
		{
			iSizeFields++;
		}

		for (j=0;j<iSizeFields ;j++) //nimm jedes Feld und fülle es mit dem Inhalt aus dem Record String
		{
			if (l+pLengths[j]<=v.GetLength())
			{
				if(m_bShowFPS)
				{
					if(j == iCountRecordsWithFPS-2)
					{
						// bei FPS hier die durchschnittliche FPS ausrechnen und am Ende der
						//letzten Sekunde mehrerer Sekunden eintragen
						s = "--";
						m_Records.SetItemText(index,j+1,s);
						l += pLengths[j];
					}
					else
					{
						s = v.Mid(l,pLengths[j]);

						if(j == iFieldTIME) //Zeitstempel z.B. 155340
						{
							//hole die Sekunde des Zeitstempels
							CString sCurrSecond = s.Right(2);
							int iCurrSecond = _ttoi(sCurrSecond);
							
							if(m_iLastSecond == FPS_COUNTER_RESET) //start
							{
								m_iLastSecond = iCurrSecond;
								m_iCountSecondsForFPS = 0;
							}
							else if(m_iLastSecond == iCurrSecond)
							{
								if(m_iCountSecondsForFPS == FPS_COUNTER_RESET)
								{
									m_iCountSecondsForFPS = 1;
								}
								else
								{
									m_iCountSecondsForFPS++;
								}
							}
							else if(m_iLastSecond != iCurrSecond)
							{
								CString sFPS;
								if(m_iCountSecondsForFPS == FPS_COUNTER_RESET)
								{
									//hier rein, wenn die Sekunde nur 1mal da war = 1FPS
									m_iCountSecondsForFPS = 1;
								}
								else
								{
									m_iCountSecondsForFPS++;
								}
	
								//hier die gezählten Sekunden im vorherigen Record eintragen
								sFPS.Format(_T("%02d"),m_iCountSecondsForFPS);
								m_Records.SetItemText(index-1,iCountRecordsWithFPS-1,sFPS);

								m_iCountSecondsForFPS = FPS_COUNTER_RESET;
								m_iLastSecond = iCurrSecond;
								m_flEverageFPS_CurrSequence++;
							}
						}
						
						m_Records.SetItemText(index,j+1,s);
						l += pLengths[j];
					}
				}
				else
				{
					s = v.Mid(l,pLengths[j]);
					m_Records.SetItemText(index,j+1,s);
					l += pLengths[j];
				}
			}
			else
			{
				//TRACE(_T("string to short %d,%d <%s>\n"),i,j,v);
			}
		}
	}

	if(m_bShowFPS)
	{
		m_flEverageFPS_CurrSequence = i / m_flEverageFPS_CurrSequence;
	}

	m_sFPSEverage.Format(_T("%06fl"), m_flEverageFPS_CurrSequence);

	delete [] pLengths;
	for (i=0;i<m_Records.GetHeaderCtrl()->GetItemCount();i++)
	{
		m_Records.SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
	}

	m_sRecords.Format(_T("A-Nr: %04x, S-Nr: %d, R: %d"),wArchiveNr,wSequenceNr,records.GetSize());
	UpdateData(FALSE);
	Enable();
}

static TCHAR BASED_CODE szFilter[] = _T("Invisible (*.qqq)|*.qqq||");
void CDVSTestDlg::OnButtonBrowse() 
{
	CDirDialog dlg(TRUE,NULL,_T("trick.dat"),OFN_HIDEREADONLY,szFilter);
	dlg.m_ofn.lpstrInitialDir = m_sPath.GetBufferSetLength(_MAX_PATH);
	dlg.m_sDir = m_sPath;
	int res = dlg.DoModal();
	if (res==IDOK)
	{
		m_sPath.ReleaseBuffer();
		CString sTemp = dlg.GetPathName();
		int p = sTemp.ReverseFind(_T('\\'));
		m_sPath = sTemp.Left(p);
		UpdateData(FALSE);
	}
}

void CDVSTestDlg::OnButtonBackup() 
{
	int iNumSel = m_ListArchives.GetSelectedCount();
	int r = 0;
	DWORD* pIDs = NULL;	

	if (iNumSel>0)
	{
		pIDs = new DWORD[iNumSel];

		for (int i=0;i<m_ListArchives.GetItemCount();i++)
		{
			if (LVIS_SELECTED & m_ListArchives.GetItemState(i,LVIS_SELECTED))
			{
				WORD wArchiveNr = (WORD)m_ListArchives.GetItemData(i);
				TRACE(_T("calc archiv %04hx\n"),wArchiveNr);
				pIDs[r++] = MAKELONG(0,wArchiveNr);
			}
		}

		CSystemTime s,e,t;

		m_StartTime.GetTime(&s);
		m_StartDate.GetTime(&t);
		s.wDay = t.wDay;
		s.wDayOfWeek = t.wDayOfWeek;
		s.wMonth = t.wMonth;
		s.wYear = t.wYear;
		m_EndTime.GetTime(&e);
		m_EndDate.GetTime(&t);
		e.wDay = t.wDay;
		e.wDayOfWeek = t.wDayOfWeek;
		e.wMonth = t.wMonth;
		e.wYear = t.wYear;

		TRACE(_T("backup from %s to %s\n"),s.GetDateTime(),e.GetDateTime());

		m_dwCalcTick = GetTickCount();

		m_pCIPCDatabaseTest->DoRequestBackupByTime(2,m_sPath+_T('\\'),_T(""),r,pIDs,s,e,BBT_EXECUTE|BBT_PROGRESS);
	}

	WK_DELETE_ARRAY(pIDs);
}

void CDVSTestDlg::OnAlarmList() 
{
	m_pCIPCDatabaseTest->DoRequestAlarmListArchives();
	
}
void CDVSTestDlg::Enable()
{
	if (m_pCIPCDatabaseTest)
	{
		m_btnConnect.EnableWindow(FALSE);
		m_btnDisconnect.EnableWindow();
		m_btnAlarmList.EnableWindow(!m_bGotAlarmList);

		m_btnShow.EnableWindow(m_ListSequences.GetSelectedCount());
		m_btnDelete.EnableWindow(m_ListSequences.GetSelectedCount());
		m_btnCalc.EnableWindow(m_ListArchives.GetSelectedCount());
		m_btnBackup.EnableWindow(m_ListArchives.GetSelectedCount());
	}
	else
	{
		m_btnConnect.EnableWindow();
		m_btnDisconnect.EnableWindow(FALSE);
		m_btnShow.EnableWindow(FALSE);
		m_btnDelete.EnableWindow(FALSE);
		m_btnCalc.EnableWindow(FALSE);
		m_btnBackup.EnableWindow(FALSE);
		m_btnAlarmList.EnableWindow(FALSE);
	}
}

void CDVSTestDlg::OnItemchangedListSequences(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	Enable();
	
	*pResult = 0;
}


void CDVSTestDlg::OnOK() 
{
	CSystemTime s,e,t;

	m_StartTime.GetTime(&s);
	m_StartDate.GetTime(&t);
	s.wDay = t.wDay;
	s.wDayOfWeek = t.wDayOfWeek;
	s.wMonth = t.wMonth;
	s.wYear = t.wYear;
	m_EndTime.GetTime(&e);
	m_EndDate.GetTime(&t);
	e.wDay = t.wDay;
	e.wDayOfWeek = t.wDayOfWeek;
	e.wMonth = t.wMonth;
	e.wYear = t.wYear;

	theApp.m_sStart = s.GetDBDate() + s.GetDBTime();
	theApp.m_sEnd = e.GetDBDate() + e.GetDBTime();
	theApp.m_sPath = m_sPath;
	
	CWK_Dialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////////////
//prüft die gesamte Datenbank, ob alle Bilder in der richtigen zeitlichen Reihenfolge
//pro Sequenz abgespeichert sind
void CDVSTestDlg::OnCheckPics() 
{
	CSystemTime st;
	m_stMaxDiffOverAll = st; //reset
	m_sTimeDiff.Empty();
	m_dwWholePics = 0;
	m_Records.DeleteAllItems();

	m_sRecords.Empty();
	m_sChecking.Empty();
	UpdateData(FALSE);
	
	for (int k=m_Records.GetHeaderCtrl()->GetItemCount()-1;k>=0;k--)
	{
		m_Records.DeleteColumn(k);
	}
	m_Records.InsertColumn(0,_T("A-Nr"));
	m_Records.InsertColumn(1,_T("S-Nr"));
	m_Records.InsertColumn(2,_T("BildNr"));
	
	for (k=0;k<m_Records.GetHeaderCtrl()->GetItemCount();k++)
	{
		m_Records.SetColumnWidth(k,LVSCW_AUTOSIZE_USEHEADER);
	}

	m_bCheckPics = TRUE;
	for(int i = 0; i < m_ListSequences.GetItemCount(); i++)
	{
		DWORD dwData = m_ListSequences.GetItemData(i);
//		WORD wArchiveNr = HIWORD(dwData);
//		WORD wSequenceNr = LOWORD(dwData);
		m_dwaCheckPicsSequences.InsertAt(0, dwData);
//		m_dwaCheckPicsSequences.Add(dwData);
	}
	CheckPics_RequestPictures();
}
/////////////////////////////////////////////////////////////////////////////////////
void CDVSTestDlg::CheckPics_RequestPictures() 
{
	int iSize = m_dwaCheckPicsSequences.GetSize();

	if(iSize > 0)
	{
		DWORD dwData = m_dwaCheckPicsSequences.GetAt(iSize-1);
		WORD wArchiveNr = HIWORD(dwData);
		WORD wSequenceNr = LOWORD(dwData);
		
		m_pCIPCDatabaseTest->DoRequestRecords(wArchiveNr,wSequenceNr,0,0);

		m_dwaCheckPicsSequences.RemoveAt(iSize-1);
	}


}
/////////////////////////////////////////////////////////////////////////////////////
void CDVSTestDlg::CheckOrder_CheckRecords(WORD wArchiveNr, WORD wSequenceNr) 
{
	CIPCFields& fields = m_pCIPCDatabaseTest->GetSequenceFields();
	int* pLengths = new int[fields.GetSize()];
//	int iSum = 0;
	CIPCFields& records = m_pCIPCDatabaseTest->GetSequenceRecords();

	int iDate = 0;
	int iTime = 0;
	int iMS   = 0;
	CString sDate, sTime, sMS;
	BOOL bDate = FALSE;
	BOOL bTime = FALSE;
	for (int i=0;i<fields.GetSize();i++)
	{
		CIPCField* pField = fields[i];
		pLengths[i] = pField->GetMaxLength();

		if(pField->GetName().Mid(4) == _T("DATE"))
		{
			if(!bDate)
			{
				iDate = i;

				//nur einmal das Datum holen, da auch das GAA-datum vorhanden sein kann
				bDate = TRUE; 
			}
		}
		if(pField->GetName().Mid(4) == _T("TIME"))
		{
			if(!bTime)
			{
				iTime = i;

				//nur einmal die Zeit holen, da auch die GAA-Zeit vorhanden sein kann
				bTime = TRUE;
			}
		}

		CString sName = pField->GetName();
		if(pField->GetName().Mid(4) == _T("MS"))
		{
			iMS = i;
		}
	}

	ULARGE_INTEGER ulMaxDiff;
	CSystemTime stDiff;
	ulMaxDiff.QuadPart = 0;

	int iIndex = 0;
	BOOL bSaveDateTime = TRUE;
	for (i=0;i<records.GetSize();i++)
	{
		CIPCField* pRecord = records.GetAt(i);
		CString s,v;
		int l = 0;
		v = pRecord->GetValue();
		s.Format(_T("%04d"),i+1);

		for (int j=0;j<fields.GetSize() ;j++)
		{
			if (l+pLengths[j]<=v.GetLength())
			{
				s = v.Mid(l,pLengths[j]);
				//TRACE(_T("s: %s\n"), s);
				if(j == iDate)
				{
					sDate = s;
					sDate.TrimRight();
				}
				if(j == iTime)
				{
					sTime = s;
					sTime.TrimRight();
				}
				if(j == iMS)
				{
					sMS = s;
					sMS.TrimRight();
				}
				l += pLengths[j];
			}
		}
		WORD wDay, wMonth, wYear, wHour, wMinute, wSecond, wMilliSecond;
		wDay = wMonth = wYear = wHour = wMinute = wSecond = wMilliSecond = 0;

		if(sDate.GetLength() == 8)
		{
			wYear	= (WORD)_ttoi(sDate.Left(4));
			wMonth	= (WORD)_ttoi(sDate.Mid(4,2));
			wDay	= (WORD)_ttoi(sDate.Right(2));	
		}
		
		if(sTime.GetLength() == 6)
		{
			wHour	= (WORD)_ttoi(sTime.Left(2));
			wMinute = (WORD)_ttoi(sTime.Mid(2,2));
			wSecond = (WORD)_ttoi(sTime.Right(2));
		}
		
		if(!sMS.IsEmpty())
		{
			wMilliSecond = (WORD)_ttoi(sMS);
		}

		CSystemTime stCurrent(wDay, wMonth, wYear, wHour, wMinute, wSecond, wMilliSecond);

		if (i!=0)
		{
			FILETIME ftCurrent,ftLastRecord,ftDiff;

			SystemTimeToFileTime(&stCurrent,&ftCurrent);
			SystemTimeToFileTime(&m_stLastRecord,&ftLastRecord);
			ULARGE_INTEGER ulCurrent,ulLastRecord,ulDiff;

			ulCurrent.HighPart = ftCurrent.dwHighDateTime;
			ulCurrent.LowPart = ftCurrent.dwLowDateTime;
			ulLastRecord.HighPart = ftLastRecord.dwHighDateTime;
			ulLastRecord.LowPart = ftLastRecord.dwLowDateTime;

			ulDiff.QuadPart = ulCurrent.QuadPart-ulLastRecord.QuadPart;

			if (ulDiff.QuadPart>ulMaxDiff.QuadPart)
			{
				ulMaxDiff.QuadPart = ulDiff.QuadPart;
			}

			ftDiff.dwHighDateTime = ulDiff.HighPart;
			ftDiff.dwLowDateTime = ulDiff.LowPart;
			FileTimeToSystemTime(&ftDiff,&stDiff);

//			TRACE(_T("%02d:%02d,%03d\n"),stDiff.wMinute,stDiff.wSecond,stDiff.wMilliseconds);
		}

		
		WORD wCur = stCurrent.GetMilliseconds();
		WORD wLast = m_stLastRecord.GetMilliseconds();
		if(stCurrent < m_stLastRecord)
		{
			TRACE(_T("Current: %s:%d  Last: %s:%d\n"), stCurrent.GetDateTime(), 
					stCurrent.GetMilliseconds(), m_stLastRecord.GetDateTime(), m_stLastRecord.GetMilliseconds());
			bSaveDateTime = FALSE;
			TRACE(_T("### BildNr. %d falsch einsortiert\n"), i+1);
			CString sArchivNr, sSequenceNr, sBildNr;
			sArchivNr.Format(_T("%04d"), wArchiveNr & 0x0F);
			sSequenceNr.Format(_T("%d"), wSequenceNr);
			sBildNr.Format(_T("%04d"), i+1);
			int iItemCount = m_Records.GetItemCount();
			m_Records.InsertItem(iItemCount+1, _T(""));

			m_Records.SetItemText(iItemCount,0,sArchivNr);
			m_Records.SetItemText(iItemCount,1,sSequenceNr);
			m_Records.SetItemText(iItemCount,2,sBildNr);
			iIndex++;
			UpdateData(FALSE);
		}
		else if((wCur - wLast) == 40)
		{
			TRACE(_T("### Abstand 40ms A: %d  S: %d  R: %d\n"), wArchiveNr, wSequenceNr, (i+1));
		}
		else
		{
			m_stLastRecord.wYear = wYear;
			m_stLastRecord.wMonth = wMonth;
			m_stLastRecord.wDay = wDay;
			m_stLastRecord.wHour = wHour;
			m_stLastRecord.wMinute = wMinute;
			m_stLastRecord.wSecond = wSecond;
			m_stLastRecord.wMilliseconds = wMilliSecond;
		}
	}

	//member wieder resetten
	CSystemTime st;
	m_stLastRecord.wYear = st.wYear;
	m_stLastRecord.wMonth = st.wMonth;
	m_stLastRecord.wDay = st.wDay;
	m_stLastRecord.wHour = st.wHour;
	m_stLastRecord.wMinute = st.wMinute;
	m_stLastRecord.wSecond = st.wSecond;
	m_stLastRecord.wMilliseconds = st.wMilliseconds;
	TRACE(_T("### %d Bilder geprüft\n"), i );

	//check nächste Sequenz
	int iSize = m_dwaCheckPicsSequences.GetSize();
	if (iSize > 0)
	{
		DWORD dwData = m_dwaCheckPicsSequences.GetAt(iSize-1);
		WORD wArchiveNr = HIWORD(dwData);
		WORD wSequenceNr = LOWORD(dwData);
		m_pCIPCDatabaseTest->DoRequestRecords(wArchiveNr,wSequenceNr,0,0);
		m_dwaCheckPicsSequences.RemoveAt(iSize-1);
	}
	
	FILETIME ftMaxDiff;
	ftMaxDiff.dwHighDateTime = ulMaxDiff.HighPart;
	ftMaxDiff.dwLowDateTime = ulMaxDiff.LowPart;
	FileTimeToSystemTime(&ftMaxDiff,&stDiff);

	if(stDiff > m_stMaxDiffOverAll)
	{
		m_stMaxDiffOverAll = stDiff;
		m_sTimeDiff.Format(_T("Max. zeitl. Diff. alle Seq. %02dh : %02dm : %02ds ,%03dms"),
			m_stMaxDiffOverAll.wHour, m_stMaxDiffOverAll.wMinute,m_stMaxDiffOverAll.wSecond,m_stMaxDiffOverAll.wMilliseconds);

	}


	m_dwWholePics += i;
	

	m_sChecking.Format(_T("A-Nr: %04d, S-Nr:%d"),
		wArchiveNr&0x0F, wSequenceNr);
	UpdateData(FALSE);

	m_sRecords.Format(_T("Bilder falsch: %d  Bilder gesamt %d %s"),
						m_Records.GetItemCount(), 
						m_dwWholePics,
						m_sTimeDiff);
	
	UpdateData(FALSE);
	WK_DELETE(pLengths);
	

}
BOOL CDVSTestDlg::StretchElements()
{
	return TRUE;
}

void CDVSTestDlg::OnCheckmd() 
{
	UpdateData();
}

void CDVSTestDlg::OnCheckFPS()
{
	UpdateData();
}
