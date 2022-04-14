// NetUnitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NetUnit.h"
#include "NetUnitDlg.h"

#include "nfc\IPCServerControlInterface.h"
#include "nfc\IPBook.h"
#include "nfc\NIC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNetUnitDlg dialog

/*------
*
* Name: CNetUnitDlg()
*
* Zweck: Konstruktor
*
* Ein- /Ausgabeparameter: (I) (CWnd*) pParent
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
CNetUnitDlg::CNetUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNetUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetUnitDlg)
	m_bAllowIncomingCalls = FALSE;
	m_bDoBeep = FALSE;
	m_bAllowOutgoingCalls = FALSE;
	m_bUseHostList = FALSE;
	m_strDUNName = _T("");
	m_strUserName = _T("");
	m_strPassword = _T("");
	m_strTelnum = _T("");
	m_bIsNewDUNEntry = FALSE;
	m_strSync = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
#ifdef _DTS	
	for(int iN=0;iN<10;iN++)
	{
		m_pIPBook[iN]=NULL;
	}
#endif
	Create(IDD);
}



/*------
*
* Name: ~CNetUnitDlg()
*
* Zweck: Destruktor
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
CNetUnitDlg::~CNetUnitDlg()
{
	//OnDestroy();
}



/*------
*
* Name: DoDataExchange()
*
* Zweck: IDC <-> Membervariable
*
* Ein- /Ausgabeparameter: (I) (CDataExchange*) pDX
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetUnitDlg)
	DDX_Control(pDX, IDC_IPLIST, m_buttonFirewall);
	DDX_Control(pDX, IDC_RICHEDIT_IPHOSTLIST, m_ctrlRichEditIPHostList);
	DDX_Control(pDX, IDC_BUTTON_CHANGEDUN, m_buttonChangeDUN);
	DDX_Control(pDX, IDC_BUTTON_CANCELNEWDUN, m_buttonCancelNewDUN);
	DDX_Control(pDX, IDC_LIST_DUNS, m_listBoxDUNs);
	DDX_Control(pDX, IDC_EDIT_TELNUM, m_editTelnum);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUserName);
	DDX_Control(pDX, IDC_EDIT_DUNNAME, m_editDUNName);
	DDX_Control(pDX, IDC_BUTTON_IPLIST, m_buttonIPList);
	DDX_Control(pDX, IDC_EDIT_SYNC, m_editSync);
	DDX_Control(pDX, IDC_DOBEEP, m_buttonDoBeep);
	DDX_Control(pDX, IDC_COMBO_DUNDEVICE, m_comboBoxDUNDevice);
	DDX_Control(pDX, IDC_COMBO_BITRATE, m_comboBoxBitrate);
	DDX_Control(pDX, IDC_BUTTON_NEWDUN, m_buttonNewDUN);
	DDX_Control(pDX, IDC_BUTTON_DETAILS, m_buttonDUNDetails);
	DDX_Control(pDX, IDC_BUTTON_DELETEDUN, m_buttonDeleteDUN);
	DDX_Control(pDX, IDC_ALLOW_OUTGOING_CALLS, m_buttonAllowOutgoingCalls);
	DDX_Control(pDX, IDC_ALLOW_INCOMING_CALLS, m_buttonAllowIncomingCalls);
	DDX_Control(pDX, IDOK, m_buttonClose);
	DDX_Control(pDX, IDC_BUTTON_REINIT, m_buttonReinit);
	DDX_Check(pDX, IDC_ALLOW_INCOMING_CALLS, m_bAllowIncomingCalls);
	DDX_Check(pDX, IDC_DOBEEP, m_bDoBeep);
	DDX_Check(pDX, IDC_ALLOW_OUTGOING_CALLS, m_bAllowOutgoingCalls);
	DDX_Check(pDX, IDC_IPLIST, m_bUseHostList);
	DDX_Text(pDX, IDC_EDIT_DUNNAME, m_strDUNName);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_TELNUM, m_strTelnum);
	DDX_Text(pDX, IDC_EDIT_SYNC, m_strSync);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BUTTON_NETWORKDETAILS, m_buttonNetworkDetails);
}

BEGIN_MESSAGE_MAP(CNetUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CNetUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_CLOSE_ALL_CONNECTIONS, OnCloseAllConnections)
	ON_EN_CHANGE(IDC_RICHEDIT_IPHOSTLIST, OnChangeRichEditIPList)
	ON_BN_CLICKED(IDC_ALLOW_INCOMING_CALLS, OnChangeNetUnitParams)
	ON_BN_CLICKED(IDC_BUTTON_REINIT, OnButtonReinit)
	ON_BN_CLICKED(IDC_BUTTON_DETAILS, OnButtonDetails)
	ON_LBN_DBLCLK(IDC_LIST_DUNS, OnDblclkListDuns)
	ON_LBN_SELCHANGE(IDC_LIST_DUNS, OnSelchangeListDuns)
	ON_BN_CLICKED(IDC_BUTTON_NEWDUN, OnButtonNewDUN)
	ON_BN_CLICKED(IDC_BUTTON_DELETEDUN, OnButtonDeleteDUN)
	ON_BN_CLICKED(IDC_BUTTON_CANCELNEWDUN, OnButtonCancelNewDUN)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEDUN, OnButtonChangeDUN)
	ON_EN_CHANGE(IDC_EDIT_DUNNAME, OnChangeEditDUNName)
	ON_EN_CHANGE(IDC_EDIT_USERNAME, OnChangeEditUsername)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_TELNUM, OnChangeEditTelnum)
	ON_CBN_SELCHANGE(IDC_COMBO_DUNDEVICE, OnSelchangeComboDundevice)
	ON_BN_CLICKED(IDC_IPLIST, OnIplistClicked)
	ON_BN_CLICKED(IDC_BUTTON_IPLIST, OnCopyTcpToList)
	ON_BN_CLICKED(IDC_DOBEEP, OnChangeNetUnitParams)
	ON_BN_CLICKED(IDC_ALLOW_OUTGOING_CALLS, OnChangeNetUnitParams)
	ON_CBN_SELCHANGE(IDC_COMBO_BITRATE, OnChangeNetUnitParams)
	ON_EN_CHANGE(IDC_EDIT_SYNC, OnChangeNetUnitParams)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_BN_CLICKED(IDC_BUTTON_NETWORKDETAILS, OnBnClickedButtonNetworkdetails)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetUnitDlg message handlers

/*------
*
* Name: OnInitDialog()
*
* Zweck: NetUnit-Dialog initialisieren
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::OnInitDialog()
{
	NETUNITPARAM netUnitParam;
	
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
/*
#ifdef _DTS
	//CString sTip = ;
	//GetWindowText(sTip);
#else
	//CString sTip = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
	SetWindowText(sTip);
#endif
*/
	SetWindowText(sTip);

	// CIPCControl-Verbindung herstellen
	// -> Es werden zusaetzlich alle NetUnit-Parameter festgelegt.
	m_pCIPCControlInterface = new CIPCControlInterface();
	if (m_pCIPCControlInterface)
	{
		netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();
		if(netUnitParam.m_bStandalone)
			WK_TRACE(_T("Running as 'standalone'\n"));
	}

	// Der NetUnit-Prozess erscheint als Icon im Systemtray. 
	// Im Standalone-Betrieb wird zusaetzlich der Dialog angezeigt.
	if(netUnitParam.m_bStandalone)
	{
		// Dialog-Elemente initialisieren und Dialog anzeigen
		if(InitDialogMembers())
		{
			CenterWindow();
			ShowWindow(SW_RESTORE);
		}
	}

	tnd.cbSize           = sizeof(NOTIFYICONDATA);
	tnd.hWnd             = m_hWnd;
	tnd.uID              = 1;
	tnd.uFlags           = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon            = m_hIcon;

	lstrcpyn(tnd.szTip, sTip, (sTip.GetLength()+1)*sizeof(TCHAR));
	Shell_NotifyIcon(NIM_ADD, &tnd);

	CWinSocket::InitSocketDLL();

#ifdef _DTS
	//IP-BOOK
	int m_iNrOfIP;
	unsigned long *lIPList;
	CString sIP;
	CNIC NIC;
	int   iNrOfIP   = 0L;
	DWORD dwNICType = 0L;

	// die Anzahl der im System befindlichen IP-Adresseen ermitteln
	iNrOfIP = NIC.GetNrOfIPAddresses();

	lIPList = CIPSocket::GetLocalIP(m_iNrOfIP);

	WK_TRACE(_T("Anzahl der lokalen IP-Adresseinträge: %i\n"),m_iNrOfIP);
	
	for(int iN=1; iN<iNrOfIP+1; iN++)
	{ 
		sIP = NIC.GetNICIPAddresse(iN);
		dwNICType = NIC.GetNICType(sIP);

		// Ist der aktuelle Netzwerkadapter gueltig?
		if(dwNICType != -1)
		{
			WK_TRACE(_T("----------------------------------------------------------\n"));			
			WK_TRACE(_T("Starte IP-Book auf %s...\n"),sIP);
			WK_TRACE(_T("%s\n"),NIC.GetNICName(sIP));
			WK_TRACE(_T("Bitrate : %d Bit/s\n"),NIC.GetBitrate(sIP));
			
			// die aktuelle IP-Adresse bzw. der zugehoerige NICType ist gueltig
			// => IP-Book zur IP starten
			m_pIPBook[iN] = new CIPBook();
			m_pIPBook[iN]->StartIPBook(sIP);
		}
	}
	WK_TRACE(_T("----------------------------------------------------------\n"));
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}



/*------
*
* Name: OnSysCommand()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: (UINT)      nID:
*                         (LPARAM) lParam:
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

/*------
*
* Name: OnPaint()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnPaint() 
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
		CDialog::OnPaint();
	}
}



// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/*------
*
* Name: OnQueryDragIcon()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
HCURSOR CNetUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



/*------
*
* Name: OnDestroy()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnDestroy() 
{
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	CDialog::OnDestroy();
	m_pCIPCControlInterface->Exit();

#ifdef _DTS	
	for(int iN=0;iN<10;iN++)
	{
		if(m_pIPBook[iN])
		{
			(m_pIPBook[iN])->StopIPBook();
			WK_DELETE(m_pIPBook[iN]);
		}
	}
#endif
	WK_DELETE(m_pCIPCControlInterface);
}



/*------
*
* Name: OnTrayClicked()
*
* Zweck: Systemtray clicked
*
* Ein- /Ausgabeparameter: (WPARAM) wParam
*                         (LPARAM) lParam
*
* Rueckgabewert: (LRESULT)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
LRESULT CNetUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONDBLCLK:
		//ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
		//OnBnClickedButtonNetworkdetails();
		break;
	case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;
			
			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MENU1);
			pM = menu.GetSubMenu(0);
			
			CString sAbout;
			menu.GetMenuString(ID_APP_ABOUT, sAbout, MF_BYCOMMAND);
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}
	return 0;
}



/*------
*
* Name: DoConfirmSelfcheck()
*
* Zweck: Selfcheck
*
* Ein- /Ausgabeparameter: (WPARAM) wParam
*                         (LPARAM) lParam
*
* Rueckgabewert: (LRESULT)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
LRESULT CNetUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SOCKET_UNIT, 0);
	}
	return 0;
}



/*------
*
* Name: PostNcDestroy()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::PostNcDestroy() 
{
	delete this;
}



/*------
*
* Name: OnOK()
*
* Zweck: OK-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnOK() 
{
	int iRet = 0;

	// Ist Button 'Reinitialisieren' aktiviert - wurden also Werte ohne Reinit
	// der NetUnit geaendert -, soll vor dem Beenden der Unit 
	// nachgefragt werden, ob die Aenderungen uebernommen werden koennen.
	CButton *pButtonReinit;	// Button: 'Reinitialisieren'
	pButtonReinit = (CButton *) GetDlgItem(IDC_BUTTON_REINIT);

	// Ist Button 'Speichern' aktiviert - wurde also eine DFUE-Verbindung
	// eingetragen bzw. eine bestehende geaendert, ohne dieses zu uebernehmen -,
	// soll vor dem Beenden der Unit nachgefragt werden, ob die Aenderungen 
	// uebernommen werden koennen.
	CButton *pButtonSave;	// Button: 'Speichern'
	pButtonSave = (CButton *) GetDlgItem(IDC_BUTTON_CHANGEDUN);
	
	if(!pButtonReinit->IsWindowEnabled() && !pButtonSave->IsWindowEnabled())
	{
		DestroyWindow();
	}
	else
	{
		iRet = AfxMessageBox(IDS_DISCARD_CHANGES_AND_EXIT, MB_YESNOCANCEL | MB_ICONQUESTION);

		switch(iRet)
		{
		case IDYES:
			if(SaveChanges())
				DestroyWindow();
			break;

		case IDNO:
			DestroyWindow();
			break;

		case IDCANCEL:
			break;
		}
	}
}



/*------
*
* Name: OnCancel()
*
* Zweck: Cancel-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnCancel() 
{
	NETUNITPARAM netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();
	if	(netUnitParam.m_bStandalone)
	{
		DestroyWindow();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}



/*------
*
* Name: OnAppAbout()
*
* Zweck: About-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}



/*------
*
* Name: OnChangeRichEditIPList()
*
* Zweck: ChangeRichEditIPList-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeRichEditIPList() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	CheckIPList();

	OnChangeNetUnitParams();
}



/*------
*
* Name: OnChangeNetUnitParams()
*
* Zweck: ChangeNetUnitParams-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeNetUnitParams() 
{
	// Reinitialisierung ermoeglichen
	m_buttonReinit.EnableWindow(TRUE);
}



/*------
*
* Name: InitDialogMembers()
*
* Zweck: Initialisiert alle relevanten Elemente des NetUnit-Dialogs
*
* Eingabeparameter: -
*
* Rueckgabewert: (BOOL) TRUE:  Initialisierung erfolgreich
*                       FALSE: Initialisierung fehlgeschlagen
*
* Datum: 17.12.2003
*
* letzte Aenderung: 12.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::InitDialogMembers()
{
	CWK_Profile wkp;
	int i = 0;
	int iComboBoxBitrateIndex = 1;
	CDialUpNetwork2 dun;
	CList<CString,CString>* phonebookEntryList;
	CString str;
	CSize iMaxHorPixSize=0, curHorPixSize=0;
	CDC* pDC = GetDC();

	// aktuelle NetUnit-Parameter ermitteln
	NETUNITPARAM netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();

	// 1. die allgemeinen NetUnit-Optionen initialisieren
	m_bAllowIncomingCalls = netUnitParam.m_bDoListen;
	m_bDoBeep             = netUnitParam.m_bDoBeepOnIncoming;
	m_bAllowOutgoingCalls = netUnitParam.m_bAllowOutgoingCalls;
	m_bUseHostList     = netUnitParam.m_bUseHostList;
	m_strSync.Format(_T("%d"), netUnitParam.m_dwSyncRetryTime/1000);
	CString comboBoxBitrate[] = {_T("64 kBit"),
		                         _T("128 kBit"),
								 _T("256 kBit"),
								 _T("512 kBit"),
								 _T("1 MBit"),
								 _T("2 MBit"),
								 _T("4 MBit"),
								 _T("8 MBit")};
	
	m_comboBoxBitrate.ResetContent();
	
	for(int k = 0; k < 8; k++)
		m_comboBoxBitrate.AddString(comboBoxBitrate[k]);

	switch(netUnitParam.m_dwSelectedBitrate/1024)
	{
	case 64:
		iComboBoxBitrateIndex = 0;
		break;
	case 128:
		iComboBoxBitrateIndex = 1;
		break;
	case 256:
		iComboBoxBitrateIndex = 2;
		break;
	case 512:
		iComboBoxBitrateIndex = 3;
		break;
	case 1024:
		iComboBoxBitrateIndex = 4;
		break;
	case 2048:
		iComboBoxBitrateIndex = 5;
		break;
	case 4096:
		iComboBoxBitrateIndex = 6;
		break;
	case 8192:
		iComboBoxBitrateIndex = 7;
		break;
	}

	m_comboBoxBitrate.SetCurSel(iComboBoxBitrateIndex);

	// hosts aus der Registrierung laden
	LoadHostListFromRegistry();

	// Einen nicht proportionalen Font wählen
	CFont font;
	font.CreatePointFont(100, _T("Courier New"));
	m_ctrlRichEditIPHostList.SetFont(&font, TRUE);
	m_ctrlRichEditIPHostList.SetEventMask(m_ctrlRichEditIPHostList.GetEventMask() | ENM_CHANGE);


	// 2. den DFUE-Bereich initialisieren

	// - aktuelle DFUE-Eintraege ermitteln und in die DUN-ListBox eintragen
	// -> Damit die horizontale Scrollbar angezeigt wird, muss von jedem 
	//    Eintrag die Pixelweite mit der maximalen verglichen werden.
	//    SetHorizontalExtent(...) ermoeglicht dann das horizontale Scrollen
	//    gemaess der maximalen Pixelweite.
	m_listBoxDUNs.ResetContent();
	dun.ReloadPhonebookEntryList();		// Standard-Phonebook
	phonebookEntryList = dun.GetPhonebookEntryList();
	
	POSITION pos = phonebookEntryList->GetHeadPosition();

	while(pos!=NULL)
	{
		//m_listBoxDUNs.InsertString(-1, phonebookEntryList->GetNext(pos));
		str = phonebookEntryList->GetNext(pos);
		curHorPixSize = pDC->GetTextExtent(str);
		//iMaxHorSize = max(iMaxHorPixSize, curHorPixSize);
		if(curHorPixSize.cx > iMaxHorPixSize.cx)
			iMaxHorPixSize = curHorPixSize;
		m_listBoxDUNs.AddString(str);
	}

	m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

	// - ComboBox fuer die DFUE-Geraete initialisieren
	int iNrOfRasDevices = 0;
	RASDEVINFO *pRasDevInfo = dun.EnumDevices(iNrOfRasDevices);
	for(i=0; i<iNrOfRasDevices; i++)
		m_comboBoxDUNDevice.AddString((pRasDevInfo+i)->szDeviceName);
	WK_DELETE(pRasDevInfo);

	//UpdateData(FALSE);

	// alle Elemente en- oder disablen
	GetDlgItem(IDC_ALLOW_INCOMING_CALLS)->EnableWindow(TRUE);
	GetDlgItem(IDC_DOBEEP)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALLOW_OUTGOING_CALLS)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_BITRATE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SYNC)->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_SYNC))->SetReadOnly(FALSE);
	GetDlgItem(IDC_IPLIST)->EnableWindow(TRUE);
	if( m_bUseHostList )
	{	// Firewall verwenden
		//m_ctrlRichEditIPHostList.EnableWindow(TRUE);
		GetDlgItem(IDC_RICHEDIT_IPHOSTLIST)->EnableWindow(TRUE);
	}
	else
	{	// Firewall nicht verwenden
		//m_ctrlRichEditIPHostList.EnableWindow(FALSE);
		GetDlgItem(IDC_RICHEDIT_IPHOSTLIST)->EnableWindow(FALSE);
	}
	GetDlgItem(IDC_BUTTON_IPLIST)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_DUNNAME)->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_DUNNAME))->SetReadOnly(TRUE);
	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_USERNAME))->SetReadOnly(TRUE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_PASSWORD))->SetReadOnly(TRUE);
	GetDlgItem(IDC_EDIT_TELNUM)->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_TELNUM))->SetReadOnly(TRUE);
	GetDlgItem(IDC_COMBO_DUNDEVICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NEWDUN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CHANGEDUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CANCELNEWDUN)->EnableWindow(FALSE);
	
	// Windows 95/98/ME kennen _RasEntryDlg() nicht!
	// -> also 'Details'-Button nicht anzeigen
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO *)  &osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		GetDlgItem(IDC_BUTTON_DETAILS)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_DETAILS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_DELETEDUN)->EnableWindow(FALSE);
	GetDlgItem(IDC_LIST_DUNS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_REINIT)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);

	// Solange die Anzeige des Netzwerkdetails nicht funktioniert,
	// wird der Netzwerkdetails-Button versteckt.
	if(wkp.GetInt(REG_KEY_SOCKETUNIT,_T("NetworkDetails"),FALSE))
		GetDlgItem(IDC_BUTTON_NETWORKDETAILS)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_NETWORKDETAILS)->ShowWindow(SW_HIDE);
//#ifdef _DEBUG
//	GetDlgItem(IDC_BUTTON_NETWORKDETAILS)->EnableWindow(TRUE);
//#else
//	GetDlgItem(IDC_BUTTON_NETWORKDETAILS)->ShowWindow(SW_HIDE);
//#endif

	// DUN-Textfelder leeren
	m_strDUNName  = _T("");
	m_strPassword = _T("");
	m_strTelnum   = _T("");
	m_strUserName = _T("");
	m_comboBoxDUNDevice.SetCurSel(-1);

	UpdateData(FALSE);

	return TRUE;
}



/*------
*
* Name: OnButtonReinit()
*
* Zweck: ButtonReinit-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonReinit() 
{
	EnDisableAllDialogMembers(FALSE);

	NETUNITPARAM netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();

	// Aenderungen uebernehmen
	UpdateData(TRUE);

	SaveHostListToRegistry();

	netUnitParam.m_bDoListen = m_bAllowIncomingCalls;
	netUnitParam.m_bDoBeepOnIncoming = m_bDoBeep;
	netUnitParam.m_bAllowOutgoingCalls = m_bAllowOutgoingCalls;
	netUnitParam.m_bUseHostList = m_bUseHostList;
	TCHAR *stopstring;
	netUnitParam.m_dwSyncRetryTime = (DWORD) _tcstod(m_strSync, &stopstring)*1000;

	switch(m_comboBoxBitrate.GetCurSel())
	{
	case 0:
		netUnitParam.m_dwSelectedBitrate = 64*1024;
		break;
	case 1:
		netUnitParam.m_dwSelectedBitrate = 128*1024;
		break;
	case 2:
		netUnitParam.m_dwSelectedBitrate = 256*1024;
		break;
	case 3:
		netUnitParam.m_dwSelectedBitrate = 512*1024;
		break;
	case 4:
		netUnitParam.m_dwSelectedBitrate = 1024*1024;
		break;
	case 5:
		netUnitParam.m_dwSelectedBitrate = 2048*1024;
		break;
	case 6:
		netUnitParam.m_dwSelectedBitrate = 4096*1024;
		break;
	case 7:
		netUnitParam.m_dwSelectedBitrate = 8192*1024;
		break;
	}

	// alle Listener und Gateways schliessen
	m_pCIPCControlInterface->Exit();

	// ListenerControl-Thread beenden, ListenerControl- und CIPCControlInterface-Objekt zerstoeren
	WK_DELETE(m_pCIPCControlInterface);

	// NetUnit-Parametern setzen und zusaetzlich in die Registrierung eintragen
	//m_pCIPCControlInterface->SetNetUnitParam(netUnitParam);
	CWK_Profile wkp;

	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("BeepOnIncoming")    ,netUnitParam.m_bDoBeepOnIncoming);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("DoListen")          ,netUnitParam.m_bDoListen);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("AllowOutgoingCalls"),netUnitParam.m_bAllowOutgoingCalls);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("UseHostList")	    ,netUnitParam.m_bUseHostList);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("SyncRetryTimeMS")   ,netUnitParam.m_dwSyncRetryTime);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("DefaultBitrate")    ,netUnitParam.m_dwSelectedBitrate/1024 );
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("AllowAnyHost")      ,netUnitParam.m_bAllowAnyHost);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("DenyNoHost")        ,netUnitParam.m_bDenyNoHost);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("UseDNS")            ,netUnitParam.m_bUseDNS);
	wkp.WriteInt(REG_KEY_SOCKETUNIT,_T("Mode")              ,netUnitParam.m_bStandalone);


	// neues CIPCControlInterface-Objekt erstellen
	m_pCIPCControlInterface = new CIPCControlInterface();

	//EnDisableAllDialogMembers(TRUE);
	InitDialogMembers();
}



/*------
*
* Name: EnDisableAllDialogMembers()
*
* Zweck: Alle Dialogelemente en- oder disablen
*
* Ein- /Ausgabeparameter: (I) (BOOL) state: en- oder disablen
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::EnDisableAllDialogMembers(BOOL state)
{
	m_buttonClose.EnableWindow(state);
	m_buttonAllowIncomingCalls.EnableWindow(state);
	m_buttonAllowOutgoingCalls.EnableWindow(state);
	m_buttonDoBeep.EnableWindow(state);
	m_buttonAllowOutgoingCalls.EnableWindow(state);
	m_buttonDeleteDUN.EnableWindow(state);
	m_buttonDUNDetails.EnableWindow(state);
	m_buttonIPList.EnableWindow(state);
	m_buttonNewDUN.EnableWindow(state);
	m_buttonCancelNewDUN.EnableWindow(state);
	m_buttonChangeDUN.EnableWindow(state);
	m_buttonIPList.EnableWindow(state);
	m_buttonReinit.EnableWindow(state);
	m_buttonFirewall.EnableWindow(state);
	m_comboBoxBitrate.EnableWindow(state);
	m_comboBoxDUNDevice.EnableWindow(state);
	m_ctrlRichEditIPHostList.EnableWindow(state);
	m_editDUNName.EnableWindow(state);
	m_editPassword.EnableWindow(state);
	m_editSync.EnableWindow(state);
	m_editTelnum.EnableWindow(state);
	m_editUserName.EnableWindow(state);
	m_listBoxDUNs.EnableWindow(state);
}



/*------
*
* Name: OnButtonDetails()
*
* Zweck: Zeigt den System-Dialog des selektierten Phonebook-Eintrags
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 05.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonDetails() 
{
	CDialUpNetwork2 dun;
	int iCurItem = 0;
	CString strCurItem;
	LPTSTR pStr;
	
	// Windows 95/98/ME kennen _RasEntryDlg() nicht!
	// -> also Details nur bei NT-Kernel anzeigen
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO *)  &osvi);
	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		// selektierten Index ermitteln
		iCurItem = m_listBoxDUNs.GetCurSel();

		// wurde selektiert?
		if(iCurItem != LB_ERR)
		{
			// entsprechenden String auslesen
			m_listBoxDUNs.GetText(iCurItem, pStr=strCurItem.GetBuffer(m_listBoxDUNs.GetTextLen(iCurItem)));

			// Variante 1
			LPRASENTRYDLG lpInfo = (LPRASENTRYDLG) GlobalAlloc(GPTR, sizeof(RASENTRYDLG));
			ZeroMemory(lpInfo, sizeof(RASENTRYDLG));
			lpInfo->dwSize = sizeof(RASENTRYDLG);

			if(!dun.ShowRasEntryDlg(NULL, pStr, lpInfo))
				TRACE(_T("[CNetUnitDlg::OnButtonDetails] Fehler bei RasEntryDlg()\n"));

			// Variante 2
/*
			LPRASDIALDLG lpInfo = (LPRASDIALDLG) GlobalAlloc(GPTR, sizeof(RASDIALDLG));
			ZeroMemory(lpInfo, sizeof(RASDIALDLG));
			lpInfo->dwSize = sizeof(RASDIALDLG);

			if(!dun.ShowRasDialDlg(NULL, "XPeisdn", NULL, lpInfo))
				TRACE(_T("[CNetUnitDlg::OnButtonDetails] 'Cancel' oder Fehler bei RasDialDlg()\n"));
*/

			// belegten Speicher freigeben
			strCurItem.ReleaseBuffer();

			// relevante Textfelder aktualisieren
			OnSelchangeListDuns();
		}
		else
			TRACE(_T("[CNetUnitDlg::OnButtonDetails] m_listBoxDUNs nicht selektiert!\n"));
	}
}



/*------
*
* Name: OnDblclkListDuns()
*
* Zweck: DblclkListDuns-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnDblclkListDuns() 
{
	// bei Doppelclick auf String: DFUE-Eigenschaften-Dialog des Eintrags anzeigen
	OnButtonDetails();
}



/*------
*
* Name: OnSelchangeListDuns()
*
* Zweck: Reagiert auf Selektionsaenderungen in der ListBox der DUNs.
*        Insbesondere werden die RasDial-Parameter des selektierten DUNs
*        in den entsprechenden Textfeldern aktualisiert.
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 05.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnSelchangeListDuns() 
{
	CDialUpNetwork2 dun;
	int iCurItem = 0, iRet = 0, iCurDUNListBoxSel = 0;
	CString strCurItem;
	CString *strDUNName;
	CString *strUserName;
	CString *strPassword;
	CString *strTelnum;
	CString strCurDUNListBoxSel;
	RASDIALPARAMS rasdialparams;
	RASENTRY rasentry;
	CString str;
	CSize iMaxHorPixSize = 0, curHorPixSize = 0;
	CList<CString,CString>* phonebookEntryList;
	POSITION pos;
	CDC* pDC = GetDC();


	if(m_bIsNewDUNEntry)
	{
		iRet = AfxMessageBox(IDS_DISCARD_NEW_DUN_ENTRY, MB_YESNO | MB_ICONQUESTION);
		switch(iRet)
		{
		case IDYES:
			m_bIsNewDUNEntry = FALSE;

			// neue DUN-ListBox-Selection und entsprechenden Verbindungsnamen merken
			iCurDUNListBoxSel = m_listBoxDUNs.GetCurSel();
			m_listBoxDUNs.GetText(iCurDUNListBoxSel, strCurDUNListBoxSel);

			// vorhandene Eintrage in der ListBox loeschen und neu aufbauen
			m_listBoxDUNs.ResetContent();
			dun.ReloadPhonebookEntryList();		// Standard-Phonebook
			phonebookEntryList = dun.GetPhonebookEntryList();
			
			pos = phonebookEntryList->GetHeadPosition();

			while(pos != NULL)
			{
				str = phonebookEntryList->GetNext(pos);
				curHorPixSize = pDC->GetTextExtent(str);
				if(curHorPixSize.cx > iMaxHorPixSize.cx)
					iMaxHorPixSize = curHorPixSize;
				m_listBoxDUNs.AddString(str);
			}

			m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

			// gemerkten Verbindungsnamen in der aktualisierten ListBox suchen,
			// selektieren und anzeigen
			m_listBoxDUNs.SetCurSel(m_listBoxDUNs.FindString(0, strCurDUNListBoxSel));

			break;

		case IDNO:
			m_listBoxDUNs.SetCurSel(0);
			return;
		}
	}

	// selektierten Index ermitteln
	iCurItem = m_listBoxDUNs.GetCurSel();

	// entsprechenden String auslesen
	m_listBoxDUNs.GetText(iCurItem, strCurItem);

	// RASDIALPARAMS ermitteln
	if(dun.GetEntryDialParameter(strCurItem, rasdialparams, _T("NULL")/*Standard-Phonebook*/))
	{
		// Daten aus RASDIALPARAMS-Struktur auslesen ...
		strDUNName  = new CString(rasdialparams.szEntryName);
		strUserName = new CString(rasdialparams.szUserName);
		strPassword = new CString(rasdialparams.szPassword);
		strTelnum   = new CString(rasdialparams.szPhoneNumber);

		// ... und in die Textfelder eintragen
		m_strDUNName  = *strDUNName;
		m_strUserName = *strUserName;
		m_strPassword = *strPassword;
		m_strTelnum   = *strTelnum;

		// RASENTRY-Daten (->DUN-Eigenschaften) ermitteln, um den verwendeten Geraetenamen
		// herauszufinden
		dun.GetEntryProperties(m_strDUNName, rasentry, _T("NULL"));

		// DUN-ComboBox aktualisieren
		if(m_comboBoxDUNDevice.GetCount() == CB_ERR || m_comboBoxDUNDevice.GetCount() == 0)
			TRACE(_T("[CNetUnitDlg::OnSelchangeListDuns] m_comboBoxDUNDevice ist leer oder fehlerhaft\n"));
		else
			m_comboBoxDUNDevice.SelectString(0, rasentry.szDeviceName);

		UpdateData(FALSE);

		// relevante Buttons de-/aktivieren
		m_buttonDeleteDUN.EnableWindow(TRUE);
		m_buttonDUNDetails.EnableWindow(TRUE);
		m_buttonChangeDUN.EnableWindow(FALSE);
		m_buttonCancelNewDUN.EnableWindow(FALSE);

		// relevante Textfelder und ComboBoxes aktivieren
		m_editDUNName.SetReadOnly(FALSE);
		m_editPassword.SetReadOnly(FALSE);
		m_editTelnum.SetReadOnly(FALSE);
		m_editUserName.SetReadOnly(FALSE);
		m_comboBoxDUNDevice.EnableWindow(TRUE);

		// belegten Speicher wieder freigeben
		delete strDUNName;
		delete strUserName;
		delete strPassword;
		delete strTelnum;
	}
}



/*------
*
* Name: OnButtonNewDUNViaAssist()
*
* Zweck: Ruft den entsprechenden Windows-Assistenten auf, um eine neue
*        DFUE-Verbindung anzulegen. 
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 05.01.2003
*
* letzte Aenderung: 05.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonNewDUNViaAssist() 
{
	CDialUpNetwork2 dun;
	CString str;
	CSize iMaxHorPixSize=0, curHorPixSize=0;
	CList<CString,CString>* phonebookEntryList;
	CDC* pDC = GetDC();

	dun.CreateRasPhonebookEntryViaAssist();

	// aktuelle DFUE-Eintraege ermitteln und die DUN-ListBox aktualisieren
	// - zunaechst Selektion aufheben und relevante Buttons deaktivieren
	m_listBoxDUNs.SetCurSel(-1);
	m_buttonDeleteDUN.EnableWindow(FALSE);
	m_buttonDUNDetails.EnableWindow(FALSE);
	// - vorhandene Eintrage in der ListBox loeschen
	m_listBoxDUNs.ResetContent();
	dun.ReloadPhonebookEntryList();		// Standard-Phonebook
	phonebookEntryList = dun.GetPhonebookEntryList();
		
	POSITION pos = phonebookEntryList->GetHeadPosition();

	while(pos!=NULL)
	{
		str = phonebookEntryList->GetNext(pos);
		curHorPixSize = pDC->GetTextExtent(str);
		if(curHorPixSize.cx > iMaxHorPixSize.cx)
			iMaxHorPixSize = curHorPixSize;
		m_listBoxDUNs.AddString(str);
	}

	m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

	// alle relevanten Textfelder und ComboBoxes reinitialisieren
	m_strDUNName  = _T("");
	m_strPassword = _T("");
	m_strTelnum   = _T("");
	m_strUserName = _T("");
	m_comboBoxDUNDevice.SetCurSel(-1);

	UpdateData(FALSE);
}



/*------
*
* Name: OnButtonDeleteDUN()
*
* Zweck: Entfernt eine DFUE-Verbindung aus dem System.
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 05.01.2003
*
* letzte Aenderung: 05.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonDeleteDUN() 
{
	CDialUpNetwork2 dun;
	CString strCurItem, str;
	int iCurItem = 0;
	CSize iMaxHorPixSize=0, curHorPixSize=0;
	CList<CString,CString>* phonebookEntryList;
	CDC* pDC = GetDC();

	// selektierten Index ermitteln
	iCurItem = m_listBoxDUNs.GetCurSel();

	// wurde selektiert?
	if(iCurItem != LB_ERR)
	{
		// entsprechenden String auslesen
		m_listBoxDUNs.GetText(iCurItem, strCurItem);

		// selektierte DFUE-Verbindung aus dem System loeschen
		
		if(dun.DeleteRasPhonebookEntry(NULL, strCurItem))
		{
			// aktuelle DFUE-Eintraege ermitteln und die DUN-ListBox aktualisieren
			// - zunaechst Selektion aufheben und relevante Buttons deaktivieren
			m_listBoxDUNs.SetCurSel(-1);
			m_buttonDeleteDUN.EnableWindow(FALSE);
			m_buttonDUNDetails.EnableWindow(FALSE);
			// - vorhandene Eintrage in der ListBox loeschen und neu aufbauen
			m_listBoxDUNs.ResetContent();
			dun.ReloadPhonebookEntryList();		// Standard-Phonebook
			phonebookEntryList = dun.GetPhonebookEntryList();
			
			POSITION pos = phonebookEntryList->GetHeadPosition();

			while(pos!=NULL)
			{
				str = phonebookEntryList->GetNext(pos);
				curHorPixSize = pDC->GetTextExtent(str);
				if(curHorPixSize.cx > iMaxHorPixSize.cx)
					iMaxHorPixSize = curHorPixSize;
				m_listBoxDUNs.AddString(str);
			}

			m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

			// alle relevanten Textfelder und ComboBoxes reinitialisieren
			m_strDUNName  = _T("");
			m_strPassword = _T("");
			m_strTelnum   = _T("");
			m_strUserName = _T("");
			m_comboBoxDUNDevice.SetCurSel(-1);

			// alle aktivierten DUN-Elemente deaktivieren
			m_editDUNName.SetReadOnly(TRUE);
			m_editPassword.SetReadOnly(TRUE);
			m_editTelnum.SetReadOnly(TRUE);
			m_editUserName.SetReadOnly(TRUE);
			m_comboBoxDUNDevice.EnableWindow(FALSE);

			// relevante Buttons de-/aktivieren
			m_buttonDeleteDUN.EnableWindow(FALSE);
			m_buttonDUNDetails.EnableWindow(FALSE);
			m_buttonCancelNewDUN.EnableWindow(FALSE);
			m_buttonChangeDUN.EnableWindow(FALSE);
			m_buttonNewDUN.EnableWindow(TRUE);

			UpdateData(FALSE);
		}
		else
			TRACE(_T("[CNetUnitDlg::OnButtonDeleteDUN] DFUE-Eintrag konnte nicht geloescht werden\n"));
	}
	else
		TRACE(_T("[CNetUnitDlg::OnButtonDeleteDUN] m_listBoxDUNs nicht selektiert!\n"));
}



/*------
*
* Name: OnButtonNewDUN()
*
* Zweck: Erstellt eine neue DFUE-Verbindung gemaess den Benutzereingaben. 
*
* Eingabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 05.01.2003
*
* letzte Aenderung: 09.01.2003
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonNewDUN()
{
	int iRet = 0;
	int curSelIndex = 0;
	CString strNewDun;

	// Ist Button 'Speichern' aktiviert - wurden also die Werte einer
	// DFUE-Verbindung geaendert -, soll vor dem Neuanlegen einer Verbindung 
	// nachgefragt werden, ob die Aenderungen verworfen werden koennen.
	CButton *pButtonStoreDUN;	// Button: 'Speichern'
	pButtonStoreDUN = (CButton *) GetDlgItem(IDC_BUTTON_CHANGEDUN);
	
	if(!pButtonStoreDUN->IsWindowEnabled())
	{
		// es soll eine neue DFUE-Verbinding eingerichtet werden
		m_bIsNewDUNEntry = TRUE;

		// relevante Buttons de-/aktivieren
		m_buttonDeleteDUN.EnableWindow(FALSE);
		m_buttonDUNDetails.EnableWindow(FALSE);
		m_buttonCancelNewDUN.EnableWindow(TRUE);
		m_buttonChangeDUN.EnableWindow(TRUE);
		m_buttonNewDUN.EnableWindow(FALSE);

		// relevante Textfelder und ComboBoxes aktivieren
		m_editDUNName.SetReadOnly(FALSE);
		m_editPassword.SetReadOnly(FALSE);
		m_editTelnum.SetReadOnly(FALSE);
		m_editUserName.SetReadOnly(FALSE);
		m_comboBoxDUNDevice.EnableWindow(TRUE);

		// ... und resetten
		m_strDUNName  = _T("");
		m_strPassword = _T("");
		m_strTelnum   = _T("");
		m_strUserName = _T("");
		m_comboBoxDUNDevice.SetCurSel(-1);
		UpdateData(FALSE);

		// eventuelle Selektion in der DUN-ListBox aufheben
		m_listBoxDUNs.SetCurSel(-1);
		strNewDun.LoadString(IDS_NEW_DUN);
		m_listBoxDUNs.InsertString(0, strNewDun);
		m_listBoxDUNs.SetCurSel(0);		// erste Zeile in der DUN-ListBox selektieren

		// 'Verbindungsname' = "Neue DFÜ-Verbindung" und Focus setzen
		m_strDUNName = strNewDun;
		UpdateData(FALSE);
		m_editDUNName.SetSel(0,-1);
		m_editDUNName.SetFocus();
	}
	else
	{
		iRet = AfxMessageBox(IDS_DISCARD_CHANGES, MB_YESNO | MB_ICONQUESTION);
		switch(iRet)
		{
		case IDYES:
			// DUN-ListBox-Eintrag wieder herstellen
			curSelIndex = m_listBoxDUNs.GetCurSel();
			m_listBoxDUNs.DeleteString(curSelIndex);
			m_listBoxDUNs.InsertString(curSelIndex, m_strDUNName);

			pButtonStoreDUN->EnableWindow(FALSE);
			OnButtonNewDUN();
			break;

		case IDNO:
			break;
		}
	}
}



/*------
*
* Name: OnButtonCancelNewDUN()
*
* Zweck: ButtonCancelNewDUN-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonCancelNewDUN() 
{
	CDialUpNetwork2 dun;
	CList<CString,CString>* phonebookEntryList;
	CString str;
	CSize iMaxHorPixSize=0, curHorPixSize=0;
	CDC* pDC = GetDC();

	// relevante Textfelder und ComboBoxes resetten
	m_strDUNName  = _T("");
	m_strPassword = _T("");
	m_strTelnum   = _T("");
	m_strUserName = _T("");
	m_comboBoxDUNDevice.SetCurSel(-1);
	UpdateData(FALSE);

	// alle aktivierten DUN-Elemente deaktivieren
	m_editDUNName.SetReadOnly(TRUE);
	m_editPassword.SetReadOnly(TRUE);
	m_editTelnum.SetReadOnly(TRUE);
	m_editUserName.SetReadOnly(TRUE);
	m_comboBoxDUNDevice.EnableWindow(FALSE);

	// relevante Buttons de-/aktivieren
	m_buttonCancelNewDUN.EnableWindow(FALSE);
	m_buttonChangeDUN.EnableWindow(FALSE);
	m_buttonNewDUN.EnableWindow(TRUE);
	m_buttonDUNDetails.EnableWindow(FALSE);
	m_buttonDeleteDUN.EnableWindow(FALSE);

	// DFUE-Eintraege in der DUN-ListBox aktualisieren
	m_listBoxDUNs.ResetContent();
	dun.ReloadPhonebookEntryList();		// Standard-Phonebook
	phonebookEntryList = dun.GetPhonebookEntryList();
	POSITION pos = phonebookEntryList->GetHeadPosition();
	while(pos!=NULL)
	{
		str = phonebookEntryList->GetNext(pos);
		curHorPixSize = pDC->GetTextExtent(str);
		if(curHorPixSize.cx > iMaxHorPixSize.cx)
			iMaxHorPixSize = curHorPixSize;
		m_listBoxDUNs.AddString(str);
	}
	m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

	// Sollte urspruenglich eine neue DFUE-Verbindung eingerichtet werden, 
	// so wird dieser Vorgang abgebrochen.
	m_bIsNewDUNEntry = FALSE;
}



/*------
*
* Name: OnButtonChangeDUN()
*
* Zweck: ButtonChangeDUN-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnButtonChangeDUN() 
{
	ChangeDUN();
}



/*------
*
* Name: ChangeDUN()
*
* Zweck: DFUE-Verbindung aendern oder erstellen
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::ChangeDUN() 
{
	CDialUpNetwork2 dun;
	BOOL bRet = TRUE;

	// Unterscheidung: neue DFUE-Verbindung erstellen oder bestehende aendern
	if(m_bIsNewDUNEntry)
	{	// ... neue DFUE-Verbindung erstellen
		bRet = CreateNewDUNEntry();
	}
	else
	{	// ... bestehenden Eintrag aendern
		bRet = ChangeExistingDUNEntry();
	}

	return bRet;
}



/*------
*
* Name: OnChangeEditDUNName()
*
* Zweck: ChangeEditDUNName-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeEditDUNName() 
{
	// relevante Buttons de-/aktivieren
	m_buttonChangeDUN.EnableWindow(TRUE);
	m_buttonCancelNewDUN.EnableWindow(TRUE);

	if(m_bIsNewDUNEntry)
	{
		// Benutzereingaben in die erste Zeile der DUN-ListBox uebernehmen
		UpdateData(TRUE);
		m_listBoxDUNs.DeleteString(0);
		m_listBoxDUNs.InsertString(0, m_strDUNName);
		m_listBoxDUNs.SetCurSel(0);
	}
	else
	{
		// ebenfalls Benutzereingaben in die DUN-ListBox uebernehmen,
		// jedoch an der richtigen Position
		int curListBoxSel = m_listBoxDUNs.GetCurSel();
		//UpdateData(TRUE);
		m_listBoxDUNs.DeleteString(curListBoxSel);
		//m_listBoxDUNs.InsertString(curListBoxSel, m_strDUNName);
		CString strNewEntry;
		CWnd *pWnd = GetDlgItem(IDC_EDIT_DUNNAME);
		if(pWnd)
			pWnd->GetWindowText(strNewEntry);
		m_listBoxDUNs.InsertString(curListBoxSel, strNewEntry);
		m_listBoxDUNs.SetCurSel(curListBoxSel);
	}
}



/*------
*
* Name: OnChangeEditUsername()
*
* Zweck: ChangeEditUsername-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeEditUsername() 
{
	// relevante Buttons de-/aktivieren
	m_buttonChangeDUN.EnableWindow(TRUE);
	m_buttonCancelNewDUN.EnableWindow(TRUE);
}



/*------
*
* Name: OnChangeEditPassword()
*
* Zweck: ChangeEditPassword-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeEditPassword() 
{
	// relevante Buttons de-/aktivieren
	m_buttonChangeDUN.EnableWindow(TRUE);
	m_buttonCancelNewDUN.EnableWindow(TRUE);
}



/*------
*
* Name: OnChangeEditTelnum()
*
* Zweck: ChangeEditTelnum-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnChangeEditTelnum() 
{
	// relevante Buttons de-/aktivieren
	m_buttonChangeDUN.EnableWindow(TRUE);
	m_buttonCancelNewDUN.EnableWindow(TRUE);
}



/*------
*
* Name: OnSelchangeComboDundevice()
*
* Zweck: SelchangeComboDundevice-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnSelchangeComboDundevice() 
{
	// relevante Buttons de-/aktivieren
	m_buttonChangeDUN.EnableWindow(TRUE);
	m_buttonCancelNewDUN.EnableWindow(TRUE);
}



/*------
*
* Name: CreateNewDUNEntry()
*
* Zweck: Ueberprueft die Benutzereingaben und erstellt daraufhin einen
*        neuen DFUE-Eintrag.
*
* Eingabeparameter: -
*
* Rueckgabewert: (BOOL) 
*
* Datum: 08.01.2004
*
* letzte Aenderung: 08.01.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::CreateNewDUNEntry()
{
	CDialUpNetwork2 dun;
	CList<CString,CString>* phonebookEntryList;
	CString str, strEntryName;
	BOOL bCreateDUNok = FALSE;
	CSize iMaxHorPixSize = 0, curHorPixSize = 0;
	DWORD dwRet = 0;
	BOOL bRet = FALSE;
	POSITION pos;
	CDC* pDC = GetDC();

	// relevante Werte aktualisieren
	if(m_comboBoxDUNDevice.GetCurSel() != CB_ERR)
		m_comboBoxDUNDevice.GetLBText(m_comboBoxDUNDevice.GetCurSel(), m_strDUNDeviceName);
	UpdateData(TRUE);

	// Benutzereingaben ueberpruefen
	// - alle Eintraege vorhanden?
	if(    m_strDUNName.IsEmpty()
		|| m_strUserName.IsEmpty()
		|| m_strPassword.IsEmpty()
		|| m_strTelnum.IsEmpty()
		|| m_comboBoxDUNDevice.GetCurSel() == CB_ERR )
	{
		AfxMessageBox(IDS_DIALOG_DUN_ENTRY_ERROR, MB_OK);
	}
	else
	{
		// - Verbindungsname eindeutig?
		dwRet = dun.ValidateRasPhonebookEntryName(m_strDUNName);

		switch (dwRet)
		{
		case ERROR_SUCCESS:
			strEntryName = m_strDUNName;
				
			// voruebergehend alle Buttons deaktivieren
			m_buttonNewDUN.EnableWindow(FALSE);
			m_buttonDeleteDUN.EnableWindow(FALSE);
			m_buttonDUNDetails.EnableWindow(FALSE);
			m_buttonCancelNewDUN.EnableWindow(FALSE);
			m_buttonChangeDUN.EnableWindow(FALSE);

			// neue DFUE-Verbindung speichern
			bCreateDUNok = dun.CreateRasPhonebookEntry( m_strDUNName,
														m_strUserName,
														m_strPassword,
														m_strTelnum,
														m_strDUNDeviceName);

			if( bCreateDUNok )
			{
				m_bIsNewDUNEntry = FALSE;

				// DUN-ListBox aktualisieren
				m_listBoxDUNs.ResetContent();
				dun.ReloadPhonebookEntryList();		// Standard-Phonebook
				phonebookEntryList = dun.GetPhonebookEntryList();
				pos = phonebookEntryList->GetHeadPosition();
				while(pos!=NULL)
				{
					str = phonebookEntryList->GetNext(pos);
					curHorPixSize = pDC->GetTextExtent(str);
					if(curHorPixSize.cx > iMaxHorPixSize.cx)
						iMaxHorPixSize = curHorPixSize;
					m_listBoxDUNs.AddString(str);
				}
				m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

				// neue DFUE-Verbindung in der DUN-ListBox selektieren
				m_listBoxDUNs.SelectString(0, strEntryName);

				// relevante Textfelder aktualisieren
				OnSelchangeListDuns();
					
				// relevante Buttons de-/aktivieren
				m_buttonNewDUN.EnableWindow(TRUE);
				m_buttonDeleteDUN.EnableWindow(TRUE);
				m_buttonDUNDetails.EnableWindow(TRUE);
				m_buttonCancelNewDUN.EnableWindow(FALSE);
				m_buttonChangeDUN.EnableWindow(FALSE);

				bRet = TRUE;
			}
			else
			{
				AfxMessageBox(IDS_ERROR_CREATE_DUN, MB_OK | MB_ICONSTOP);
				TRACE(_T("[CNetUnitDlg::OnButtonChangeDUN] neue DFUE-Verbindung konnte nicht erstellt werden\n"));
				// voruebergehend alle Buttons deaktivieren
				m_buttonNewDUN.EnableWindow(TRUE);
				m_buttonDeleteDUN.EnableWindow(FALSE);
				m_buttonDUNDetails.EnableWindow(FALSE);
				m_buttonCancelNewDUN.EnableWindow(TRUE);
				m_buttonChangeDUN.EnableWindow(TRUE);
			}

			break;

		case ERROR_ALREADY_EXISTS:
			AfxMessageBox(IDS_ENTRY_NAME_ALREADY_EXISTS, MB_OK);
			break;

		case ERROR_INVALID_NAME:
			AfxMessageBox(IDS_INVALID_ENTRY_NAME, MB_OK);
			break;
		}
	}
	
	return bRet;
}



/*------
*
* Name: ChangeExistingDUNEntry()
*
* Zweck: Ueberprueft die Benutzereingaben und aktualisiert daraufhin einen
*        bereits bestehenden DFUE-Eintrag.
*
* Eingabeparameter: -
*
* Rueckgabewert: (BOOL) 
*
* Datum: 08.01.2004
*
* letzte Aenderung: 08.01.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::ChangeExistingDUNEntry()
{
	CDialUpNetwork2 dun;
	CList<CString,CString>* phonebookEntryList;
	CString str, strEntryName;
	CString strOldEntryName;
	BOOL bCreateDUNok = FALSE;
	CSize iMaxHorPixSize = 0, curHorPixSize = 0;
	DWORD dwRet = 0;
	BOOL bRet = FALSE;
	POSITION pos;
	CDC* pDC = GetDC();

	// urspruengliche Werte merken
	strOldEntryName = m_strDUNName;
		
	// relevante Werte aktualisieren
	if(m_comboBoxDUNDevice.GetCurSel() != CB_ERR)
		m_comboBoxDUNDevice.GetLBText(m_comboBoxDUNDevice.GetCurSel(), m_strDUNDeviceName);
	UpdateData(TRUE);

	// Benutzereingaben ueberpruefen
	// - alle Eintraege vorhanden?
	if(    m_strDUNName.IsEmpty()
		|| m_strUserName.IsEmpty()
		|| m_strPassword.IsEmpty()
		|| m_strTelnum.IsEmpty()
		|| m_comboBoxDUNDevice.GetCurSel() == CB_ERR )
	{
		AfxMessageBox(IDS_DIALOG_DUN_ENTRY_ERROR, MB_OK);
	}
	else
	{
		// - Verbindungsname gueltig?
		dwRet = dun.ValidateRasPhonebookEntryName(m_strDUNName);

		switch (dwRet)
		{
		case ERROR_SUCCESS:
		case ERROR_ALREADY_EXISTS:
			strEntryName = m_strDUNName;
				
			// voruebergehend alle Buttons deaktivieren
			m_buttonNewDUN.EnableWindow(FALSE);
			m_buttonDeleteDUN.EnableWindow(FALSE);
			m_buttonDUNDetails.EnableWindow(FALSE);
			m_buttonCancelNewDUN.EnableWindow(FALSE);
			m_buttonChangeDUN.EnableWindow(FALSE);

			// - Verbindungsname aendern (falls notwendig)
			dwRet = dun.RenameRasPhonebookEntry(_T("NULL"), strOldEntryName, m_strDUNName);

			switch(dwRet)
			{
			case ERROR_INVALID_NAME:
				AfxMessageBox(IDS_INVALID_ENTRY_NAME, MB_OK);
				return bRet;

			case ERROR_ALREADY_EXISTS:
				AfxMessageBox(IDS_ENTRY_NAME_ALREADY_EXISTS, MB_OK);
				return bRet;

			case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY:
				TRACE(_T("[CNetUnitDlg::OnButtonChangeDUN] invalid phonebook entry\n"));
				return bRet;
			}

			// neue DFUE-Verbindung speichern
			bCreateDUNok = dun.CreateRasPhonebookEntry( m_strDUNName,
														m_strUserName,
														m_strPassword,
														m_strTelnum,
														m_strDUNDeviceName,
														TRUE);

			if( bCreateDUNok )
			{
				m_bIsNewDUNEntry = FALSE;

				// DUN-ListBox aktualisieren
				m_listBoxDUNs.ResetContent();
				dun.ReloadPhonebookEntryList();		// Standard-Phonebook
				phonebookEntryList = dun.GetPhonebookEntryList();
				pos = phonebookEntryList->GetHeadPosition();
				while(pos!=NULL)
				{
					str = phonebookEntryList->GetNext(pos);
					curHorPixSize = pDC->GetTextExtent(str);
					if(curHorPixSize.cx > iMaxHorPixSize.cx)
						iMaxHorPixSize = curHorPixSize;
					m_listBoxDUNs.AddString(str);
				}
				m_listBoxDUNs.SetHorizontalExtent(iMaxHorPixSize.cx);

				// neue DFUE-Verbindung in der DUN-ListBox selektieren
				m_listBoxDUNs.SelectString(0, strEntryName);

				// relevante Textfelder aktualisieren
				OnSelchangeListDuns();
					
				// relevante Buttons de-/aktivieren
				m_buttonNewDUN.EnableWindow(TRUE);
				m_buttonDeleteDUN.EnableWindow(TRUE);
				m_buttonDUNDetails.EnableWindow(TRUE);
				m_buttonCancelNewDUN.EnableWindow(FALSE);
				m_buttonChangeDUN.EnableWindow(FALSE);

				bRet = TRUE;
			}
			else
			{
				AfxMessageBox(IDS_ERROR_CREATE_DUN, MB_OK | MB_ICONSTOP);
				TRACE(_T("[CNetUnitDlg::OnButtonChangeDUN] neue DFUE-Verbindung konnte nicht erstellt werden\n"));

				// relevante Buttons de-/aktivieren
				m_buttonNewDUN.EnableWindow(TRUE);
				m_buttonDeleteDUN.EnableWindow(FALSE);
				m_buttonDUNDetails.EnableWindow(FALSE);
				m_buttonCancelNewDUN.EnableWindow(TRUE);
				m_buttonChangeDUN.EnableWindow(TRUE);
			}

		break;

		case ERROR_INVALID_NAME:
			AfxMessageBox(IDS_INVALID_ENTRY_NAME, MB_OK);
			break;
		}
	}
	

	return bRet;
}



/*------
*
* Name: OnIplistClicked()
*
* Zweck: IplistClicked-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnIplistClicked() 
{
	if( ((CButton *)GetDlgItem(IDC_IPLIST))->GetState() & 0x0003)
	{	// m_buttonIPList ist checked
		m_bUseHostList = TRUE;
		m_ctrlRichEditIPHostList.EnableWindow(TRUE);
	}
	else
	{	// m_buttonIPList ist nicht checked
		m_bUseHostList = FALSE;
		m_ctrlRichEditIPHostList.EnableWindow(FALSE);
	}

	OnChangeNetUnitParams();
}



/*------
*
* Name: OnCopyTcpToList()
*
* Zweck: CopyTcpToList-Handler
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnCopyTcpToList() 
{
	CHostArray	Hosts;

	CWK_Profile wkp;

	//Hosts.Load(GetProfile());
	Hosts.Load(wkp);
   	m_ctrlRichEditIPHostList.SetSel(0, 0);

	// Alle TCP-Hosts durchsuchen
	for (int nI = 0; nI < Hosts.GetSize(); nI++)
	{
		if (Hosts.GetAt(nI)->GetTyp() == _T("TCP/IP"))
		{
			CString sNumber	= Hosts.GetAt(nI)->GetNumber();
			sNumber = sNumber.Mid(4); // _T('tcp:') entfernen
			
			// Ist der Host schon in der Liste?
			if (Hosts.GetAt(nI)->IsTCPIP())
			{
				if (CheckIPSyntax(sNumber))
				{
					if (!IsHostInHostList(sNumber))
					{
						m_ctrlRichEditIPHostList.ReplaceSel(sNumber + _T("\r\n"));
						OnChangeNetUnitParams();
					}
				}
			}
		}
	}
}



/*------
*
* Name: CheckIPSyntax()
*
* Zweck: IP-Syntax ueberpruefen
*
* Ein- /Ausgabeparameter: (I/O) (const CString) &sHost
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::CheckIPSyntax(const CString &sHost)
{
	char	sRest[1024]  = {0};
	int		a,b,c,d;
	BOOL	bRet = FALSE;

	if (sHost.IsEmpty())
		return FALSE;

	a=-1;
	b=-1;
	c=-1;
	d=-1;
	_stscanf(sHost, _T("%d.%d.%d.%d%s"), &a,&b,&c,&d,&sRest);
	if ((a<0) || (a>255) ||
		(b<0) || (b>255) ||
		(c<0) || (c>255) ||
		(d<0) || (d>255))
	{
		return FALSE;
	}
	else
	{
		if (((CString)sRest).IsEmpty())
			bRet =(sHost.SpanIncluding(_T("1234567890.")) == sHost);
		else 
			bRet = FALSE;

	}

	return bRet;
}



/*------
*
* Name: IsHostInHostList()
*
* Zweck: Befindet sich der angegebene Host in der Hostlist?
*
* Ein- /Ausgabeparameter: (I/O) (const CString) &sHost
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::IsHostInHostList(const CString &sHost)
{
	TCHAR	Buffer[1024];
	BOOL	bFind = FALSE;

	for (int nI = 0; nI < m_ctrlRichEditIPHostList.GetLineCount() && !bFind; nI++)
	{
		int nCnt = m_ctrlRichEditIPHostList.GetLine(nI, Buffer, 1024);
		if (nCnt >= 2)
			Buffer[nCnt-2] = 0;

		if (sHost == (CString)Buffer)
			bFind = TRUE;
	}
	return bFind;
}



/*------
*
* Name: CheckIPList()
*
* Zweck: IP-List ueberpruefen
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::CheckIPList()
{
	BOOL bRet = TRUE;

	//if (m_bUseHostList) // && (m_EditHostList.GetTextLength() != 0))
	if(m_bUseHostList)
	{
		for (int nI = 0; nI < m_ctrlRichEditIPHostList.GetLineCount(); nI++)
		{
			if (!CheckLine(nI))
			{
				bRet = FALSE;
				break;
			}
		}
	}
	return bRet;
}



/*------
*
* Name: CheckLine()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: (int) nLine
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::CheckLine(int nLine)
{
	TCHAR Buffer[1024] = {0};
	BOOL	bRet = TRUE;

	int nCnt = m_ctrlRichEditIPHostList.GetLine(nLine, Buffer, 1024);

	if (nCnt >= 3) // Mindestens ein Zeichen +CrLf
	{
		Buffer[nCnt-2] = 0;
		bRet = CheckIPSyntax(Buffer);

		if (bRet)
			SetSelection(nLine, RGB(0, 0, 255));
		else
			SetSelection(nLine, RGB(255, 0, 0));
	}

	return bRet;
}



/*------
*
* Name: SetSelection()
*
* Zweck: 
*
* Ein- /Ausgabeparameter: (I) (int) nLine
*                         (I) (COLORREF) colRGB
*
* Rueckgabewert: -
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::SetSelection(int nLine, COLORREF colRGB)
{
	CHARFORMAT chfmt;
	long nStartChar, nEndChar;

	memset(&chfmt, 0, sizeof(chfmt));
	m_ctrlRichEditIPHostList.GetSel(nStartChar, nEndChar);
   	m_ctrlRichEditIPHostList.SetSel(m_ctrlRichEditIPHostList.LineIndex(nLine), m_ctrlRichEditIPHostList.LineIndex(nLine+1));

	chfmt.cbSize		= sizeof(chfmt);
	chfmt.dwMask		= CFM_COLOR;
	chfmt.crTextColor 	= colRGB;
	
	m_ctrlRichEditIPHostList.SetSelectionCharFormat(chfmt);
   	m_ctrlRichEditIPHostList.SetSel(nStartChar, nEndChar);
}



/*------
*
* Name: SaveChanges()
*
* Zweck: speichern
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::SaveChanges()
{
	BOOL bRet = TRUE;

	WK_TRACE_USER(_T("TCP/IP-Modul-Einstellungen wurden geändert\n"));

	// Ist Button 'Speichern' aktiviert - wurde also eine DFUE-Verbindung
	// eingetragen bzw. eine bestehende geaendert, ohne dieses zu uebernehmen -,
	// soll vor dem Beenden der Unit nachgefragt werden, ob die Aenderungen 
	// uebernommen werden koennen.
	CButton *pButtonSave;	// Button: 'Speichern'
	pButtonSave = (CButton *) GetDlgItem(IDC_BUTTON_CHANGEDUN);
	
	if(pButtonSave->IsWindowEnabled())
		bRet = ChangeDUN();

	if(bRet)
	{
		NETUNITPARAM netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();

		// Aenderungen uebernehmen
		UpdateData(TRUE);

		SaveHostListToRegistry();

		netUnitParam.m_bDoListen = m_bAllowIncomingCalls;
		netUnitParam.m_bDoBeepOnIncoming = m_bDoBeep;
		netUnitParam.m_bAllowOutgoingCalls = m_bAllowOutgoingCalls;
		netUnitParam.m_bUseHostList = m_bUseHostList;
		TCHAR *stopstring;
		netUnitParam.m_dwSyncRetryTime = (DWORD) _tcstod(m_strSync, &stopstring)*1000;
		//netUnitParam.m_dwSyncRetryTime = strtol(m_strSync, &stopstring, 10)*1000;

		switch(m_comboBoxBitrate.GetCurSel())
		{
		case 0:
			netUnitParam.m_dwSelectedBitrate = 64*1024;
			break;
		case 1:
			netUnitParam.m_dwSelectedBitrate = 128*1024;
			break;
		case 2:
			netUnitParam.m_dwSelectedBitrate = 256*1024;
			break;
		case 3:
			netUnitParam.m_dwSelectedBitrate = 512*1024;
			break;
		case 4:
			netUnitParam.m_dwSelectedBitrate = 1024*1024;
			break;
		case 5:
			netUnitParam.m_dwSelectedBitrate = 2048*1024;
			break;
		case 6:
			netUnitParam.m_dwSelectedBitrate = 4096*1024;
			break;
		case 7:
			netUnitParam.m_dwSelectedBitrate = 8192*1024;
			break;
		}

		// NetUnit-Parametern setzen und zusaetzlich in die Registrierung eintragen
		m_pCIPCControlInterface->SetNetUnitParam(netUnitParam);
	}

	return bRet;
}



/*------
*
* Name: SaveHostListToRegistry()
*
* Zweck: HostList in die Registrierungsdatenbank eintragen.
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::SaveHostListToRegistry()
{
	CWK_Profile wkp;
	TCHAR Buffer[1024];
	CString sLineNr;

	if (!m_bUseHostList)
		return TRUE;

	wkp.DeleteSection(_T("SocketUnit\\HostList"));

	for (int nI = 0; nI < m_ctrlRichEditIPHostList.GetLineCount(); nI++)
	{
		memset(Buffer, 0, 1024*sizeof(TCHAR));
		int nCnt = m_ctrlRichEditIPHostList.GetLine(nI, Buffer, 1024);
		if (nCnt >= 2)
			Buffer[nCnt-2] = 0;
		sLineNr.Format(_T("%05d"), nI);
		wkp.WriteString(_T("SocketUnit\\HostList"), sLineNr, Buffer);
	}

	return TRUE;
}



/*------
*
* Name: LoadHostListFromRegistry()
*
* Zweck: HostList aus der Registrierungsdatenbank lesen.
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
BOOL CNetUnitDlg::LoadHostListFromRegistry()
{
	CString sLineNr;
	CString sHost;
	int nI = 0;

	CWK_Profile wkp;

   	m_ctrlRichEditIPHostList.SetSel(0, -1);

	BOOL bLoop = TRUE;
	
	while (bLoop)
	{
		sLineNr.Format(_T("%05d"), nI++);
		sHost = wkp.GetString(_T("SocketUnit\\HostList"), sLineNr, _T(""));

		sLineNr.Format(_T("%05d"), nI);
		if (wkp.GetString(_T("SocketUnit\\HostList"), sLineNr, _T("<<EndeDerListe>>")) != _T("<<EndeDerListe>>"))
		{
			m_ctrlRichEditIPHostList.ReplaceSel(sHost + _T("\r\n"));
		}
		else
		{
			m_ctrlRichEditIPHostList.ReplaceSel(sHost);  // Die letzte Zeile hat kein CrLf
			bLoop = FALSE;
		}
	};

	//m_bLoadingHostList = TRUE;

	return TRUE;
}



/*------
*
* Name: OnCloseAllConnections()
*
* Zweck: CloseAllConnections-Handler -> alle offenen Verbindungen schliessen
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
 ------*/
void CNetUnitDlg::OnCloseAllConnections()
{
	WK_TRACE(_T("All connections closed...\n"));
	m_pCIPCControlInterface->CloseAllGateways();
}



/*------
*
* Name: OnBnClickedButtonNetworkdetails()
*
* Zweck: BnClickedButtonNetworkdetails-Handler -> Dialog fuer weitere 
*        Netzwerkinformationen wie Netzwerkauslastung etc. anzeigen
*
* Ein- /Ausgabeparameter: -
*
* Rueckgabewert: (BOOL)
*
* Datum: 17.12.2003
*
* letzte Aenderung: 02.04.2004
*
* Autor: Andreas Hagen
*
------*/
void CNetUnitDlg::OnBnClickedButtonNetworkdetails()
{
	//CNetworkDetailsDlg* dlg = new CNetworkDetailsDlg();
	CNetworkDetailsDlg dlg(m_pCIPCControlInterface, NULL);

	dlg.DoModal();
}

void CNetUnitDlg::OnSettings()
{
	COEMLoginDialog dlg;
							    
	if (IDOK==dlg.DoModal())
	{
		SetWindowPos(&CWnd::wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		SetForegroundWindow(); 
	}
}

void CNetUnitDlg::OnUpdateSettings(CCmdUI *pCmdUI)
{
	NETUNITPARAM netUnitParam = m_pCIPCControlInterface->GetNetUnitParam();
	pCmdUI->Enable(!netUnitParam.m_bStandalone);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CNetUnitDlg::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif

}
