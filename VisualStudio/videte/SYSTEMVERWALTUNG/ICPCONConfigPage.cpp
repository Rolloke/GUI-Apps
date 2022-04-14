// ICPCONClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "ICPCONConfigPage.h"
#include <i7000.h>

#include <ICPCONDll\ICP7000Module.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ICP_IMAGE_PORT		0	
#define ICP_IMAGE_MODULE	1	
#define ICP_IMAGE_INPUTS	2
#define ICP_IMAGE_OUTPUTS	3
#define ICP_IMAGE_INFO		4
#define ICP_IMAGE_BAUDRATE	5
#define ICP_IMAGE_LOCATION	6
#define ICP_IMAGE_DIOMODE	7
#define ICP_IMAGE_VERSION	8
#define ICP_IMAGE_CHECKSUM	9
#define ICP_IMAGE

//	Tree tier						| nImage			| nSelectedImage	| lParam	| HasChildren
//----------------------------------|-------------------|-------------------|-----------|------------
//	COMX:...						| ICP_IMAGE_PORT	| 0					| 0			| yes
//		Address: x, Module:			| ICP_IMAGE_MODULE	| ModuleAddress		| hPort		| yes
//			Name/Location			| ICP_IMAGE_INFO	| ICP_IMAGE_LOCATION| hModule	| no
//			IO Mode of Module		| ICP_IMAGE_INFO	| ICP_IMAGE_DIOMODE	| hModule	| no
//			Com Baudrate of Module	| ICP_IMAGE_INFO	| ICP_IMAGE_BAUDRATE| hModule	| no
//			Checksum				| ICP_IMAGE_INFO	| ICP_IMAGE_CHECKSUM| hModule	| no
//			Version of module		| ICP_IMAGE_INFO	| ICP_IMAGE_VERSION	| hModule	| no
//			Input					| ICP_IMAGE_INFO	| GroupID			| hModule	| yes
//				Inputs				| ICP_IMAGE_INPUTS	| 0					| hParent	| no
//			Output					| ICP_IMAGE_INFO	| GroupID			| hModule	| yes
//				Outputs				| ICP_IMAGE_OUTPUTS	| 0					| hParent	| no

#define POLL_PORT			1
#define POLL_ALL_PORTS		2
#define INITIALIZE_MODULES	3

#define USER_MSG_SET_PARENT_CHECK_STATE 1	//
#define USER_MSG_SET_THREAD_CONTROL_BTN 2
#define USER_MSG_EXPAND_TREE_ITEM       3
#define USER_MSG_SET_PROGRESS_ADDRESS   4
#define USER_MSG_SET_PROGRESS_PORT      5
#define USER_MSG_DELETE_IF_NO_CHILDREN  6
#define USER_MSG_SET_CHILD_CHECK_STATE  7
#define USER_MSG_TOGGLE_CHECK_STATE     8
#define USER_MSG_INIT_DIALOG           10
#define USER_MSG_REQUEST_DELETE_PORT   11
#define USER_MSG_REQUEST_SWITCH_GROUP  12

#define CHANGE_SAVEPARAM			0x00020000
#define CHANGE_SAVE_MODULES			0x00040000

#define CONFIG_BAUDRATE			0x0000FFFF
#define CONFIG_ADDRESS			0x00010000
#define CONFIG_CHECKSUM			0x00020000

IMPLEMENT_DYNAMIC(CICPCONConfigPage, CSVPage)

/////////////////////////////////////////////////////////////////////////////
// CICPCONConfigPage dialog
CICPCONConfigPage::CICPCONConfigPage(CSVView* pParent): CSVPage(CICPCONConfigPage::IDD)
{
	m_pParent = pParent;

	if (m_pParent->GetDocument()->IsLocal())
	{
		m_sBaseDVRT = REG_LOC_BASE;
		m_bIsLocal  = TRUE;
	}
	else 
	{
		CWK_Profile &prof = m_pParent->GetDocument()->GetProfile();
		m_sBaseDVRT       = prof.GetSection() + _T("\\");
		m_bIsLocal        = FALSE;
	}

	m_sRegistryLocation        = m_sBaseDVRT + REG_LOC_UNIT;
	m_sRegistryLocationModules = m_sRegistryLocation + REG_LOC_MODULE_UNIT;
	m_sRegistryLocationPorts   = m_sRegistryLocation + REG_LOC_PORT_UNIT;

	//{{AFX_DATA_INIT(CICPCONConfigPage)
	m_dwBaudrate     = CBR_9600;
	m_nComPort       = 1;
	m_strModuleID    = _T("");
	m_nModuleAddress = 0;
	m_bState         = FALSE;
	m_nTimeOut       = 400;
	m_nPollTime      = 1500;
	m_bPollState     = FALSE;
	m_bSwitchPanel   = FALSE;
	m_bICPCONEnabled = FALSE;
	m_sAddresses = _T("1-255");
	m_sPorts = _T("1-255");
	//}}AFX_DATA_INIT

	m_szButton.cx = 100;
	m_szButton.cy = 75;
	
	SetBaudCheck(0);

	m_hThread         = INVALID_HANDLE_VALUE;
	m_nThreadID       = 0;
	m_nWhat           = 0;
	m_bStopThread     = 0;

	m_pModule         = NULL;
	m_nActive         = -1;
	m_dwConfigChanged = 0;

	m_dwChanged       = 0;

	m_hAccelerator    = NULL;

	m_szButton.cx     = 100;
	m_szButton.cy     = 60;
	m_bExpandModules  = FALSE;

	m_nInitToolTips = MAKELONG(TOOLTIPS_SIMPLE, 300);

	Create(IDD,(CWnd*)m_pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CICPCONConfigPage)
	DDX_Control(pDX, IDC_BTN_CONFIG_MODULE, m_cBtnConfigModules);
	DDX_Control(pDX, IDC_PROGRESS_PORT, m_cProgressPorts);
	DDX_Control(pDX, IDC_PROGRESS_ADDRESS, m_cProgressAddress);
	DDX_Control(pDX, IDC_TREE_OF_MODULES, m_cTreeOfModules);
	DDX_Check(pDX, IDC_CK_BAUD_1200, m_bBaud1200);
	DDX_Check(pDX, IDC_CK_BAUD_2400, m_bBaud2400);
	DDX_Check(pDX, IDC_CK_BAUD_4800, m_bBaud4800);
	DDX_Check(pDX, IDC_CK_BAUD_9600, m_bBaud9600);
	DDX_Check(pDX, IDC_CK_BAUD_19200, m_bBaud19200);
	DDX_Check(pDX, IDC_CK_BAUD_38400, m_bBaud38400);
	DDX_Check(pDX, IDC_CK_BAUD_57600, m_bBaud57600);
	DDX_Check(pDX, IDC_CK_BAUD_115200, m_bBaud115200);
	DDX_Check(pDX, IDC_CK_POLL_STATE, m_bPollState);
	DDX_Check(pDX, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
	DDX_Check(pDX, IDC_CK_ENABLE_ICPCON_MODULE, m_bICPCONEnabled);
	DDX_Text(pDX, IDC_EDT_TIMEOUT, m_nTimeOut);
	DDV_MinMaxInt(pDX, m_nTimeOut, 100, 2000);
	DDX_Text(pDX, IDC_EDT_TIME_POLL_RELAIS, m_nPollTime);
	DDV_MinMaxInt(pDX, m_nPollTime, m_nTimeOut+100, 60000);
	DDX_Text(pDX, IDC_EDT_BTN_CX, m_szButton.cx);
	DDV_MinMaxInt(pDX, m_szButton.cx, 40, 300);
	DDX_Text(pDX, IDC_EDT_BTN_CY, m_szButton.cy);
	DDV_MinMaxInt(pDX, m_szButton.cy, 10, 100);
	DDX_Control(pDX, IDC_IPC_COMBO_MINIMIZE, m_comboMinimize);
	DDX_Text(pDX, IDC_EDT_PORT, m_nComPort);
	DDV_MinMaxInt(pDX, m_nComPort, 1, 255);
	DDX_Text(pDX, IDC_EDT_MODULE_ADDRESS, m_nModuleAddress);
	DDV_MinMaxInt(pDX, m_nModuleAddress, 0, 255);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		DWORD dw;
		DDX_Text(pDX, IDC_EDT_MODULE_ADDRESS_START, m_sAddresses);
		dw = ExtractNumbers(m_sAddresses, 0, 255, m_wAddresses);
		if (dw)
		{
			CString sMsg;
			sMsg.FormatMessage(AFX_IDP_ARCH_BADINDEX, m_sAddresses);
			AfxMessageBox(sMsg, IDOK|MB_ICONERROR);
			((CEdit*)GetDlgItem(IDC_EDT_MODULE_ADDRESS_START))->SetSel(LOWORD(dw), LOWORD(dw)+HIWORD(dw));
			pDX->m_bEditLastControl = FALSE;
			pDX->Fail();
		}
		DDX_Text(pDX, IDC_EDT_PORT_START, m_sPorts);
		dw = ExtractNumbers(m_sPorts, 1, 255, m_wComPorts);
		if (dw)
		{
			((CEdit*)GetDlgItem(IDC_EDT_PORT_START))->SetSel(LOWORD(dw), LOWORD(dw)+HIWORD(dw));
			CString sMsg;
			sMsg.FormatMessage(AFX_IDP_ARCH_BADINDEX, m_sPorts);
			AfxMessageBox(sMsg, IDOK|MB_ICONERROR);
			pDX->m_bEditLastControl = FALSE;
			pDX->Fail();
		}
		if (!m_bBaud1200 && !m_bBaud2400 && !m_bBaud4800 && !m_bBaud9600 && !m_bBaud19200 && !m_bBaud38400 && !m_bBaud57600 && !m_bBaud115200)
		{
			pDX->m_bSaveAndValidate = FALSE;
			m_bBaud9600 = TRUE;
			DDX_Check(pDX, IDC_CK_BAUD_9600, m_bBaud9600);
		}
	}
	else
	{
		DDX_Text(pDX, IDC_EDT_MODULE_ADDRESS_START, m_sAddresses);
		DDX_Text(pDX, IDC_EDT_PORT_START, m_sPorts);
	}

}
/////////////////////////////////////////////////////////////////////////////
DWORD CICPCONConfigPage::ExtractNumbers(const CString &sAddresses, WORD wMinVal, WORD wMaxVal, CWordArray&waNumbers)
{
	DWORD dwReturn = 0;
	waNumbers.RemoveAll();
	CStringArray saRange, saSingle;
	CString sRange, sSingle, sWrong = StringCharsNotInSet(sAddresses, _T("1234567890;- "));
	if (sWrong.GetLength() == 0)
	{
		if (sAddresses.FindOneOf(_T("123456789")) != -1)
		{
			SplitString(sAddresses, saRange, _T(';'));
			int iR, nR = saRange.GetSize();
			for (iR=0; iR<nR; iR++)
			{
				if (saRange[iR].Find(_T("-")) != -1)
				{
					saSingle.RemoveAll();
					SplitString(saRange[iR], saSingle, _T('-'));
					if (saSingle.GetSize() == 2)
					{
						WORD w, wMin, wMax;
						sSingle = saSingle[0];
						sSingle.Trim();
						wMin = (WORD)_ttoi(sSingle);
                        sSingle = saSingle[1];
						sSingle.Trim();
						wMax = (WORD)_ttoi(sSingle);
						if (   wMin < wMax
							&& IsBetween(wMax, wMinVal, wMaxVal)
							&& IsBetween(wMin, wMinVal, wMaxVal))
						{
							for (w=wMin; w<=wMax; w++)
							{
								AddNoDoubleToWordArray(w, waNumbers);
							}
						}
						else
						{
							return MAKELONG(sAddresses.Find(saRange[iR]), saRange[iR].GetLength());
						}
					}
					else
					{
						return MAKELONG(sAddresses.Find(saRange[iR]), saRange[iR].GetLength());
					}
				}
				else
				{
					sSingle = saRange[iR];
					sSingle.Trim();
					WORD w = (WORD)_ttoi(sSingle);
					if (IsBetween(w, wMinVal, wMaxVal))
					{
						AddNoDoubleToWordArray(w, waNumbers);
					}
					else
					{
						return MAKELONG(sAddresses.Find(saRange[iR]), saRange[iR].GetLength());
					}
				}
			}
		}
		else
		{
			dwReturn = MAKELONG(0, sAddresses.GetLength());
		}
	}
	else
	{
		dwReturn = MAKELONG(0, sAddresses.GetLength());
	}
	return dwReturn;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CICPCONConfigPage, CSVPage)
	//{{AFX_MSG_MAP(CICPCONConfigPage)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_POLL_PORT, OnBtnPollPort)
	ON_BN_CLICKED(IDC_BTN_SCAN_ALL_PORTS, OnBtnScanAllPorts)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_OF_MODULES, OnDeleteitemTreeOfModules)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_OF_MODULES, OnBeginlabeleditTreeOfModules)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_OF_MODULES, OnEndlabeleditTreeOfModules)
	ON_NOTIFY(NM_CLICK, IDC_TREE_OF_MODULES, OnClickTreeOfModules)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_CK_MULTIPLE_ACCESS, OnCkComStates)
	ON_BN_CLICKED(IDC_CK_POLL_STATE, OnCkModuleStates)
	ON_COMMAND(IDC_EDIT_TREE_LABEL, OnEditTreeLabel)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDT_MODULE_ADDRESS, OnChangeEdtModuleAddress)
	ON_BN_CLICKED(IDC_BTN_CONFIG_MODULE, OnBtnConfigModule)
	ON_EN_CHANGE(IDC_EDT_TIMEOUT, OnChangeEdtTimeout)
	ON_EN_CHANGE(IDC_EDT_TIME_POLL_RELAIS, OnChangeEdtTimePollRelais)
	ON_BN_CLICKED(IDC_CK_ENABLE_SWITCH_PANEL, OnCkEnableSwitchPanel)
	ON_BN_CLICKED(IDC_CK_ENABLE_ICPCON_MODULE, OnCkEnableIcpconModule)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_COMMAND_RANGE(IDC_CK_BAUD_1200, IDC_CK_BAUD_115200, OnChkBaud)
	ON_CBN_SELCHANGE(IDC_IPC_COMBO_MINIMIZE, OnCbnSelchangeIpcComboMinimize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CICPCONConfigPage message handlers
BOOL CICPCONConfigPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

//	CDialog::OnInitDialog();
//	EnableToolTips(TRUE);

	m_cProgressPorts.SetRange(0, 255);
	m_cProgressPorts.SetPos(0);
	m_cProgressAddress.SetRange(0, 255);
	m_cProgressAddress.SetPos(0);

	CString sLeft, sTop, sRight, sBottom, sNothing;
	sNothing.LoadString(IDS_NOTHING);
	sLeft.LoadString(IDS_LEFT);
	sRight.LoadString(IDS_RIGHT);
	sTop.LoadString(IDS_TOP);
	sBottom.LoadString(IDS_BOTTOM);
	
	m_comboMinimize.AddString(sNothing);
	sNothing = _T(", ");
	m_comboMinimize.AddString(sLeft + sNothing + sTop);
	m_comboMinimize.AddString(sLeft + sNothing + sBottom);
	m_comboMinimize.AddString(sRight + sNothing + sTop);
	m_comboMinimize.AddString(sRight + sNothing + sBottom);

	m_cBtnConfigModules.EnableWindow(FALSE);
	m_cBtnConfigModules.GetWindowText(m_strBtnConfigModule);

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));
	m_bExpandModules = wkp.GetInt(SETTINGS_SECTION, _T("ExpandModules"), m_bExpandModules);
	m_nTimeOut       = wkp.GetInt(SETTINGS_SECTION, ICP_TIMEOUT    , m_nTimeOut);
	
	GetDlgItem(IDC_CK_POLL_STATE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CK_MULTIPLE_ACCESS)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
	GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);

	m_comboMinimize.EnableWindow(m_bSwitchPanel);

	Initialize();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnDestroy() 
{
	OnBtnCancel();
	if (ComPortStatus((char)m_nComPort) == ComportOpened)
	{
		Close_Com((char)m_nComPort);
	}

	HWND hwnd = ::FindWindow(NULL, WK_APP_NAME_ICPCON_UNIT);
	if (hwnd)
	{
		::PostMessage(hwnd, WK_WINTHREAD_STOP_THREAD, 0, 0);
	}

	CSVPage::OnDestroy();

	if (m_hAccelerator)
	{
		DestroyAcceleratorTable(m_hAccelerator);
		m_hAccelerator = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
unsigned int  CICPCONConfigPage::ThreadProc(void *pParam)
{
	CICPCONConfigPage*pThis = (CICPCONConfigPage*)pParam;
	if (POLL_PORT == pThis->m_nWhat)
	{
		pThis->OnPollPort();
	}
	else if (POLL_ALL_PORTS == pThis->m_nWhat)
	{
		pThis->OnScanAllPorts();
	}
	else if (INITIALIZE_MODULES == pThis->m_nWhat)
	{
		pThis->InitModules();
	}

	pThis->m_hThread   = INVALID_HANDLE_VALUE;
	pThis->m_nThreadID = 0;
	pThis->m_nWhat     = 0;
	pThis->m_bStopThread = FALSE;
	pThis->PostMessage(WM_USER, USER_MSG_SET_THREAD_CONTROL_BTN, FALSE);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBtnPollPort() 
{
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
		if (UpdateData())
		{
			m_nWhat   = POLL_PORT;
			SetThreadControlBtn(TRUE);
			m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
		}
   }
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBtnCancel() 
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		m_bStopThread = TRUE;
		WaitForSingleObject(m_hThread, 15000);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBtnDeleteTree() 
{
	m_cTreeOfModules.DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::SetThreadControlBtn(BOOL bThread)
{
	GetDlgItem(IDC_TREE_OF_MODULES)->EnableWindow(!bThread);

	if (!m_bIsLocal) bThread = TRUE;

	GetDlgItem(IDC_BTN_SCAN_ALL_PORTS)->EnableWindow(!bThread);
	GetDlgItem(IDC_BTN_POLL_PORT)->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_9600  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_1200  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_4800  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_19200 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_2400  )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_38400 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_57600 )->EnableWindow(!bThread);
	GetDlgItem(IDC_CK_BAUD_115200)->EnableWindow(!bThread);

	if (!m_bIsLocal) bThread = FALSE;

	((CEdit*)GetDlgItem(IDC_EDT_TIMEOUT             ))->SetReadOnly(bThread);
	((CEdit*)GetDlgItem(IDC_EDT_PORT                ))->SetReadOnly(bThread);
	((CEdit*)GetDlgItem(IDC_EDT_MODULE_ADDRESS_START))->SetReadOnly(bThread);
	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(bThread);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::OnPollPort() 
{
	CString	str, strPort, strModule, strIO, strLocation, strI, strO;
	WORD	wRetValue;
	WORD	wT = 0;
	int		i, nModuleAddress, nFirst = 1, nBaud, nInputGroupID, nOutputGroupID;
	BOOL	bCheckInput     = 0,
			bCheckOutput    = 0,
			bMultipleAccess = 0,
			bPollInState    = 1,
			bPollOutState   = 0,
			bModuleError    = FALSE;
	const int iLength = 80;
	char	szCmd[iLength], szResult[iLength];  // must be char[] for communication with i7000 driver
	CWK_String sResult;				  // for comparisson with szResult

	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));
	CWK_Profile wkpIP(CWK_Profile::WKP_ABSOLUT, HKEY_LOCAL_MACHINE, _T(""), _T(""));
	CICPI7000Module *pModule = NULL;
	HTREEITEM hTemp = NULL;
	HTREEITEM hPort = NULL, hModule = NULL;

	for (nBaud=0; nBaud<7; nBaud++)
	{
		if (ComPortStatus((char)m_nComPort) == ComportOpened)
		{
			Close_Com((char)m_nComPort);
		}

		if (m_bStopThread) break;

		if (nBaud==0)
		{
			if (m_bBaud2400) m_dwBaudrate = CBR_2400;
			else continue;
		}
		else if (nBaud==1)
		{
			if (m_bBaud4800) m_dwBaudrate = CBR_4800;
			else continue;
		}
		else if (nBaud==2)
		{
			if (m_bBaud9600) m_dwBaudrate = CBR_9600;
			else continue;
		}
		else if (nBaud==3)
		{
			if (m_bBaud19200) m_dwBaudrate = CBR_19200;
			else continue;
		}
		else if (nBaud==4)
		{
			if (m_bBaud38400) m_dwBaudrate = CBR_38400;
			else continue;
		}
		else if (nBaud==5)
		{
			if (m_bBaud57600) m_dwBaudrate = CBR_57600;
			else continue;
		}
		else if (nBaud==6)
		{
			if (m_bBaud115200) m_dwBaudrate = CBR_115200;
			else continue;
		}
		if (m_bIsLocal)
		{
			CTime t1 = CTime::GetCurrentTime();
			wRetValue = Open_Com((char)m_nComPort, m_dwBaudrate, 8, 0, 0);
			CTime t2 = CTime::GetCurrentTime();
			CTimeSpan ts = t2 - t1;
			if (ts.GetSeconds() > 1)
			{
				if (INITIALIZE_MODULES == m_nWhat)
				{
					TRACE(_T("Comport Error %d\n"), m_nComPort);
					PostMessage(WM_USER, USER_MSG_REQUEST_DELETE_PORT, m_nComPort);
				}
				return FALSE;
			}
		}
		else
		{
			wRetValue  = NoError;
			m_nTimeOut = 100;
		}
		PostMessage(WM_USER, USER_MSG_SET_PROGRESS_ADDRESS, 0);

		if (wRetValue == NoError)
		{
			strPort.Format(IDS_COM_PORT, m_nComPort);
			hPort = m_cTreeOfModules.GetRootItem();
			while (hPort)														// Portnummer schon vorhanden?
			{
				str = m_cTreeOfModules.GetItemText(hPort);
				if (str.Find(strPort) != -1) break;
				hPort = m_cTreeOfModules.GetNextItem(hPort, TVGN_NEXT);
			}
			if (hPort == NULL)												// Nein
			{																		// neuen einfügen
				CString strSection, strKey, strName;
				int nServer, nPort;
				strSection.Format(_T("SYSTEM\\CurrentControlSet\\Services\\AESPV1X\\Parameters\\AESPV1XP%d"), m_nComPort);
				DWORD dwIP = wkpIP.GetInt(strSection, _T("IP Address"), 0);// Servernamen und IP-Adresse ermitteln
				if (!m_bIsLocal) dwIP = 0;
				if (dwIP)
				{
					nServer = wkpIP.GetInt(strSection, _T("Model"), 0);
					strName.Format(_T("ADAM %x"), nServer);			// Advantech Module
					bMultipleAccess = TRUE;									// standardmaessig COM Port schliessen
				}
				else if (m_bIsLocal)
				{
					for (nServer=1; ; nServer++)							// ICP CON Module
					{
						strSection.Format(_T("SYSTEM\\CurrentControlSet\\Services\\Ynsernet\\Parameter\\Server%d"), nServer);
						strName = wkpIP.GetString(strSection, _T("ServerName"), NULL);
						if (strName.IsEmpty()) break;
						for (i=1; ;i++)
						{
							strKey.Format(_T("ComNo%d"), i);
							nPort = wkpIP.GetInt(strSection, strKey, -1);
							if (nPort == -1) break;
							else if ((BYTE)nPort == (BYTE)m_nComPort)
							{
								dwIP = wkpIP.GetInt(strSection, _T("IPAddress"), 0);
								bMultipleAccess = FALSE;					// standardmaessig COM Port offen lassen
								break;
							}
						}
						if (dwIP) break;
					}
				}
				if (dwIP)
				{
					BYTE *pcIP = (BYTE *)&dwIP;
					strKey.Format(_T("%s: %s (%d.%d.%d.%d)"), strPort, strName, pcIP[0], pcIP[1], pcIP[2], pcIP[3]);
				}
				else
				{
					strKey = strPort;
				}
				hPort = m_cTreeOfModules.InsertItem(strKey, ICP_IMAGE_PORT, 0);
				m_cTreeOfModules.SetItemState(hPort, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);
			}
			int iMa, nMa = m_wAddresses.GetSize();
			for (iMa=0; iMa<nMa; iMa++)
			{
				nModuleAddress = m_wAddresses[iMa];
				if (m_bStopThread) break;
				bModuleError = FALSE;
				PostMessage(WM_USER, USER_MSG_SET_PROGRESS_ADDRESS, nModuleAddress);
				strLocation.Empty();
				strModule.Format(_T("Ports\\%s\\%02x"), strPort, nModuleAddress);

				strI.Format(_T("%s\\%s"), strModule, ICP_INPUTS);		// InputGroupID vorhanden?
				nInputGroupID = wkp.GetInt(strI, ICP_GROUPID, 0);
				if (nInputGroupID)
				{
					strI.Format(_T("Modules\\Group%04x"), nInputGroupID);
					bCheckInput     = wkp.GetInt(strI, ICP_MODULE_ENABLED, 1);
					strLocation     = wkp.GetString(strI, ICP_MODULE_LOCATION, NULL);
					bMultipleAccess = wkp.GetInt(strI, ICP_MULTIPLE_ACCESS, 1);
					bPollInState    = wkp.GetInt(strI, ICP_POLL_STATE, 1);
				}
				else
				{
					bCheckInput     = FALSE;
					bPollInState    = TRUE;
				}

				strO.Format(_T("%s\\%s"), strModule, ICP_OUTPUTS);		// OutputGroupID vorhanden?
				nOutputGroupID     = wkp.GetInt(strO, ICP_GROUPID, 0);
				strO.Format(_T("Modules\\Group%04x"), nOutputGroupID);
				if (nOutputGroupID)
				{
					bPollOutState   = wkp.GetInt(strO, ICP_POLL_STATE, 0);
					bCheckOutput    = wkp.GetInt(strO, ICP_MODULE_ENABLED, 1);
					if (nInputGroupID==0)
					{
						bMultipleAccess = wkp.GetInt(strO, ICP_MULTIPLE_ACCESS, 1);
						strLocation     = wkp.GetString(strO, ICP_MODULE_LOCATION, NULL);
					}
				}
				else
				{
					bCheckOutput  = FALSE;
					bPollOutState = FALSE;
				}

				sprintf(szCmd, "$%02xM", nModuleAddress);		// Modulname
				wRetValue = Send_Receive_Cmd((char)m_nComPort, szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
				if ((wRetValue != NoError) || (szResult[0] != _T('!')))
				{
					if (nInputGroupID)										// Modul vorhanden aber nicht ansprechbar
					{
						sResult = wkp.GetString(strI, ICP_MODULEID, NULL);// Name aus der Registry nehmen

						strncpy(&szResult[3], (LPCSTR)sResult, iLength-3);
						bModuleError = TRUE;
					}
					else if (nOutputGroupID)
					{
						sResult = wkp.GetString(strO, ICP_MODULEID, NULL);
						strncpy(&szResult[3], (LPCSTR)sResult, iLength-3);
						bModuleError = TRUE;
					}
					else continue;
					szResult[0] = _T('!');
					szResult[1] = szCmd[1];
					szResult[2] = szCmd[2];
				}

				m_cTreeOfModules.SetItemImage(hPort, ICP_IMAGE_PORT, bMultipleAccess);
				if (INITIALIZE_MODULES != m_nWhat)						// beim Portscan
				{
					if (nInputGroupID || nOutputGroupID)				// vorhandene nicht noch einmal auflisten
						continue;
				}
				{
					int nImage, nAddress = 0;
					bool bFound =false;
					hModule = m_cTreeOfModules.GetNextItem(hPort, TVGN_CHILD);
					while (hModule)
					{
						m_cTreeOfModules.GetItemImage(hModule, nImage, nAddress);
						if (nAddress == nModuleAddress)
						{
							bFound = true;
							break;
						}
						hModule = m_cTreeOfModules.GetNextItem(hModule, TVGN_NEXT);
					}
					if (bFound)
					{
						continue;
					}
				}

				pModule = new CICPI7000Module;
				pModule->AddRef();

				sResult = szResult;
				if (!pModule->SetModuleID(sResult))
				{
					pModule->Release();
					continue;
				}
				pModule->SetComPort((WORD)m_nComPort);					// Communication Parameters
				pModule->SetModuleAddress((WORD)nModuleAddress);
				pModule->SetBaudrate(m_dwBaudrate);
																		// Modul einfügen
				CString sName(&szResult[3]);
				str.Format(IDS_PORT_MODULES, pModule->GetModuleAddress(), sName);
				if (bModuleError)
				{
					str += _T(" *");
					pModule->SetFailCount(1);
				}
				hModule = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_MODULE, nModuleAddress, hPort);
				m_cTreeOfModules.SetItemState(hModule, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);
				m_cTreeOfModules.SetItemData(hModule, (DWORD)pModule);// Pointer merken

				int nImage   = ICP_IMAGE_LOCATION;						// Einbauort bzw. Modulname
				if (strLocation.IsEmpty())
				{
					strLocation.LoadString(IDS_MODULE_LOCATION);
					nImage = ICP_IMAGE_MODULE;								// Image zur Unterscheidung ob
				}																	// der Name geändert wurde
				hTemp = m_cTreeOfModules.InsertItem(strLocation, ICP_IMAGE_INFO, nImage, hModule);
				m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0)|TVIS_BOLD, 0x0000f000|TVIS_BOLD);
																					// Moduleigenschaften
				hTemp = m_cTreeOfModules.InsertItem(pModule->GetDIOmode(), ICP_IMAGE_INFO, ICP_IMAGE_DIOMODE, hModule);
				m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);

				str.Format(IDS_BAUDRATE, m_dwBaudrate);				// Baudrate
				hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, ICP_IMAGE_BAUDRATE, hModule);
				m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);

				sprintf(szCmd, "$%02xF", nModuleAddress);			// Version des Moduls
				wRetValue = Send_Receive_Cmd((char)m_nComPort, szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
				if (wRetValue == NoError)
				{
					CString sVersion(&szResult[3]);
					str.Format(IDS_FIRMWARE_VERSION, sVersion);
					hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, ICP_IMAGE_VERSION, hModule);
					m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);
				}
				else if ((nInputGroupID == 0) && (nOutputGroupID == 0))
				{
					WK_TRACE(_T("Error:%d, Send_Receive_Cmd(%d, %s)\n"), wRetValue, m_nComPort, szCmd);
					m_cTreeOfModules.DeleteItem(hModule);
					continue;
				}

				sprintf(szCmd, "$%02x2", nModuleAddress);			// Configuration lesen
				wRetValue = Send_Receive_Cmd((char)m_nComPort, szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
				if (wRetValue == NoError)
				{
					if (!pModule->SetConfig(szResult))
					{
						m_cTreeOfModules.DeleteItem(hModule);
						continue;
					}
				}
				else if ((nInputGroupID == 0) && (nOutputGroupID == 0))
				{
					WK_TRACE(_T("Error:%d, Send_Receive_Cmd(%d, %s)\n"), wRetValue, m_nComPort, szCmd);
					m_cTreeOfModules.DeleteItem(hModule);
					continue;
				}
				
				if (theApp.m_bProfessional)
				{
					hTemp = m_cTreeOfModules.InsertItem(_T("Checksum"), ICP_IMAGE_INFO, ICP_IMAGE_CHECKSUM, hModule);
					m_cTreeOfModules.SetCheck(hTemp, pModule->GetChecksum());
				}

				if (INITIALIZE_MODULES != m_nWhat)						// beim Portscan
				{
					m_dwChanged |= CHANGE_SAVE_MODULES;
					SetModified(TRUE, FALSE);
				}
				if (pModule->HasInputs())									// Inputs
				{
					str.LoadString(IDS_DIGITAL_IN);
					HTREEITEM hInputs = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, nInputGroupID, hModule);
					m_cTreeOfModules.SetItemData(hInputs, bPollInState);
					m_cTreeOfModules.SetCheck(hInputs, bCheckInput);
					if (!bModuleError)
					{
						pModule->DigitalIn();
					}
					int nInputs = pModule->GetInputs();
					for (i=0; i<nInputs; i++)
					{
						str.Format(IDS_DIGITAL_IN_N, i+pModule->GetDIoffset());
						hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INPUTS, 0, hInputs);
						m_cTreeOfModules.SetItemData(hTemp, i);
						m_cTreeOfModules.SetCheck(hTemp, pModule->GetDIbit(i));
						if (!m_bIsLocal)
						{
							m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);
						}
					}
				}

				if (pModule->HasOutputs())									// Outputs
				{
					str.LoadString(IDS_DIGITAL_OUT);
					HTREEITEM hOutputs = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_INFO, nOutputGroupID, hModule);
					m_cTreeOfModules.SetItemData(hOutputs, bPollOutState);
					m_cTreeOfModules.SetCheck(hOutputs, bCheckOutput);
					if (!bModuleError)
					{
						pModule->DigitalOutReadBack();
					}
					int nOutputs = pModule->GetOutputs();
					for (i=0; i<nOutputs; i++)
					{
						str.Format(IDS_DIGITAL_OUT_N, i+pModule->GetDOoffset());
						hTemp = m_cTreeOfModules.InsertItem(str, ICP_IMAGE_OUTPUTS, 0, hOutputs);
						m_cTreeOfModules.SetItemData(hTemp, i);
						m_cTreeOfModules.SetCheck(hTemp, pModule->GetDObit(i));
						if (!m_bIsLocal)
						{
							m_cTreeOfModules.SetItemState(hTemp, INDEXTOSTATEIMAGEMASK(0), 0x0000f000);
						}
					}
				}

				if (hModule)
				{
					if (nFirst)
					{
						PostMessage(WM_USER, USER_MSG_EXPAND_TREE_ITEM, (LPARAM)hPort);
						nFirst = 0;
					}
					if (m_bExpandModules)
					{
						PostMessage(WM_USER, USER_MSG_EXPAND_TREE_ITEM, (LPARAM)hModule);
					}
				}
				else
				{
					m_cTreeOfModules.DeleteItem(hPort);
				}
				hModule = NULL;
			}
			Close_Com((char)m_nComPort);
			PostMessage(WM_USER, USER_MSG_DELETE_IF_NO_CHILDREN, (LPARAM)hPort);
			hPort = NULL;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBtnScanAllPorts() 
{
   if (m_hThread == INVALID_HANDLE_VALUE)
   {
		if (UpdateData())
		{
			m_cTreeOfModules.DeleteAllItems();
			m_nWhat   = POLL_ALL_PORTS;
			SetThreadControlBtn(TRUE);
			m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
		}
   }
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnScanAllPorts() 
{
	int i, n, nPort = m_nComPort;
	n = m_wComPorts.GetSize();
	for (i=0; i<n; i++)
	{
		m_nComPort = m_wComPorts[i];
		if (m_bStopThread) break;
		PostMessage(WM_USER, USER_MSG_SET_PROGRESS_PORT, m_nComPort);
		OnPollPort();
	}
	m_nComPort = nPort;
	PostMessage(WM_USER, USER_MSG_SET_PROGRESS_PORT, m_nComPort);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnDeleteitemTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	int nImage, nSelected;
	m_cTreeOfModules.GetItemImage(pNMTreeView->itemOld.hItem, nImage, nSelected);
	if (nImage == ICP_IMAGE_MODULE)
	{
		DWORD dwData = m_cTreeOfModules.GetItemData(pNMTreeView->itemOld.hItem);
		if (dwData)
		{
			CICPI7000Module *pModule = (CICPI7000Module*) dwData;
			pModule->Release();
		}
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBeginlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	int nImage, nSelected;
	*pResult = 1;
	if (m_cTreeOfModules.GetItemImage(pTVDispInfo->item.hItem, nImage, nSelected))
	{
		if (    (nImage == ICP_IMAGE_INFO) 
			 && ((nSelected == ICP_IMAGE_MODULE) || (nSelected == ICP_IMAGE_LOCATION)))
		{
			*pResult = 0;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::SaveChanges() 
{
	CString strKey, strValue, strModule, strLocation;
	BOOL    bMultipleAccess, bPollState;
	CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
	CWK_Profile wkpModules(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationModules, _T(""));
	CWK_Profile wkpPorts(  CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationPorts, _T(""));
	CWK_Profile wkpUnit(   CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));
	CInputList  *pInputList  = NULL;
	COutputList *pOutputList = NULL;

	if (m_dwChanged & CHANGE_SAVEPARAM)
	{
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_TIMEOUT , m_nTimeOut);
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_COMPORT , m_nComPort);
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_POLLTIME, m_nPollTime);

		DWORD dwBaudrates  = 0;
		if (m_bBaud1200  ) dwBaudrates|=0x00000001;
		if (m_bBaud2400  ) dwBaudrates|=0x00000002;
		if (m_bBaud4800  ) dwBaudrates|=0x00000004;
		if (m_bBaud9600  ) dwBaudrates|=0x00000008;
		if (m_bBaud19200 ) dwBaudrates|=0x00000010;
		if (m_bBaud38400 ) dwBaudrates|=0x00000020;
		if (m_bBaud57600 ) dwBaudrates|=0x00000040;
		if (m_bBaud115200) dwBaudrates|=0x00000080;
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_BAUDRATES, dwBaudrates);
	}
	
	wkpUnit.WriteInt(SETTINGS_SECTION, ICP_SWITCH_PANEL, m_bSwitchPanel);
	if (m_bSwitchPanel && m_bICPCONEnabled)
	{
		bool bNoList  = pInputList != NULL ? false:true;	// need to save?
		pInputList  = m_pParent->GetDocument()->GetInputs();
		CInputGroup * pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
		if (pInputGroup)
		{
			if (bNoList) pInputList = NULL;					// no saving nessesary
		}
		else
		{
			m_dwChanged |= CHANGE_SAVE_MODULES;
			CString strName;
			strName.LoadString(IDS_SWITCHES);
			pInputGroup = pInputList->AddInputGroup(strName, 32, SM_ICPCONUnitInput);
			strName.LoadString(IDS_SWITCH_PANEL);
			pInputGroup->SetName(strName);
		}
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DX, m_szButton.cx);
		wkpUnit.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DY, m_szButton.cy);
		CString sMinPos;
		switch(m_comboMinimize.GetCurSel())
		{
			case 1:  sMinPos = MINPOS_LEFTTOP;     break;
			case 2:  sMinPos = MINPOS_LEFTBOTTOM;  break;
			case 3:  sMinPos = MINPOS_RIGHTTOP;    break;
			case 4:  sMinPos = MINPOS_RIGHTBOTTOM; break;
			default: sMinPos = MINPOS_NONE;        break;
		}
		wkpUnit.WriteString(SETTINGS_SECTION, ICP_MINIMIZE_POSITION, sMinPos);
	}
	else
	{
		bool bNoList  = pInputList != NULL ? false:true;	// need to save?
		pInputList  = m_pParent->GetDocument()->GetInputs();
		CInputGroup * pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
		if (pInputGroup)
		{
			DeleteGroup(pInputGroup->GetID().GetGroupID());	// list is saved here
			m_dwChanged |= CHANGE_SAVE_MODULES;
			m_dwChanged |= 1<<IMAGE_INPUT;
		}
		if (bNoList) pInputList = NULL;						// no saving nessesary
	}

	if (!(m_dwChanged & CHANGE_SAVE_MODULES))
	{
		m_dwChanged = 0;
		return;
	}
		
	HTREEITEM hPort = m_cTreeOfModules.GetRootItem();
	while (hPort)
	{
		HTREEITEM hModule = m_cTreeOfModules.GetNextItem(hPort, TVGN_CHILD);
		int nImage, nAddress, nGroupID;
		m_cTreeOfModules.GetItemImage(hPort, nImage, bMultipleAccess);
		ASSERT(nImage == ICP_IMAGE_PORT);
		while (hModule)
		{
			m_cTreeOfModules.GetItemImage(hModule, nImage, nAddress);
			ASSERT(nImage == ICP_IMAGE_MODULE);
			CICPI7000Module *pModule = (CICPI7000Module*)m_cTreeOfModules.GetItemData(hModule);
			BOOL bInputOn = FALSE, bOutputOn = FALSE;
			if (pModule == NULL) break;
			strLocation.Empty();
			HTREEITEM hTemp;
			for (hTemp = m_cTreeOfModules.GetNextItem(hModule, TVGN_CHILD); 
			     hTemp != NULL;
				  hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_NEXT))
			{
				if (m_cTreeOfModules.ItemHasChildren(hTemp))
				{
					int nImageTemp, nImage, nSelected;
					nGroupID = 0;
					m_cTreeOfModules.GetItemImage(hTemp, nImageTemp, nGroupID);
					bPollState = m_cTreeOfModules.GetItemData(hTemp);

					HTREEITEM hDIO = m_cTreeOfModules.GetNextItem(hTemp, TVGN_CHILD);
					if (hDIO!=NULL)
					{
						if (!m_bICPCONEnabled)
						{
							m_cTreeOfModules.SetCheck(hTemp, FALSE);
						}
						BOOL bCheck = m_cTreeOfModules.GetCheck(hTemp);
						m_cTreeOfModules.GetItemImage(hDIO, nImage, nSelected);

						CString strName, strSMname, str, strNewName;
						if (!strLocation.IsEmpty()) strName = strLocation;
						else strName.Format(_T("%s COM%d Adr:%d "), pModule->GetModuleIDstring(), pModule->GetComPort(), pModule->GetModuleAddress());
						if (nImage == ICP_IMAGE_INPUTS)
						{
							bInputOn = bCheck;
							strSMname.Format(_T("%s%02x%02x"), SM_ICPCONUnitInput, pModule->GetComPort(), pModule->GetModuleAddress());
							pInputList  = m_pParent->GetDocument()->GetInputs();
							CInputGroup *pInputGroup = NULL;
							if (nGroupID)
							{
								CSecID id((WORD)nGroupID, 0);
								pInputGroup = pInputList->GetGroupByID(id);
								if (pInputGroup)
								{
									if (bCheck)
									{
										pInputGroup->SetName(strName);
									}
									else
									{
										DeleteGroup(nGroupID);
										pInputGroup = NULL;
										strModule.Format(ICP_GROUP_FMT, nGroupID);
										wkpModules.DeleteSection(strModule);
									}
								}
							}

							if (pInputGroup==NULL)
							{
								if (bCheck)
								{
									str.LoadString(IDS_ON);
									strNewName.Format(_T("ICP CON %s"), str);
									pInputGroup = pInputList->AddInputGroup(strNewName, pModule->GetInputs(), strSMname);
									nGroupID = pInputGroup->GetID().GetGroupID();
									pInputGroup->SetName(strName);
								}
								else nGroupID = 0;
							}
						}
						else
						{
							bOutputOn = bCheck;
							strSMname.Format(_T("%s%02x%02x"), SM_ICPCONUnitOutput, pModule->GetComPort(), pModule->GetModuleAddress());
							str.LoadString(IDS_RELAY);
							strName = str + _T(" ") + strName;
							pOutputList  = m_pParent->GetDocument()->GetOutputs();
							COutputGroup *pOutputGroup = NULL;
							if (nGroupID)
							{
								CSecID id((WORD)nGroupID, 0);
								pOutputGroup = pOutputList->GetGroupByID(id);
								if (pOutputGroup)
								{
									if (bCheck)
									{
										pOutputGroup->SetName(strName);
									}
									else
									{
										DeleteGroup(nGroupID);
										pOutputGroup = NULL;
										strModule.Format(ICP_GROUP_FMT, nGroupID);
										wkpModules.DeleteSection(strModule);
									}
								}
							}

							if (pOutputGroup==NULL)
							{
								if (bCheck)
								{
									strNewName.Format(_T("ICPCON%s"), str);
									pOutputGroup = pOutputList->AddOutputGroup(strNewName, pModule->GetOutputs(), strSMname);
									nGroupID = pOutputGroup->GetID().GetGroupID();
									pOutputGroup->SetName(strName);
								}
								else nGroupID = 0;
							}
						}
						if (nGroupID == 0)
						{
							nGroupID = MAKEWORD(pModule->GetModuleAddress(), pModule->GetComPort());
						}
						strModule.Format(ICP_GROUP_FMT, nGroupID);
						m_cTreeOfModules.SetItemImage(hTemp, nImageTemp, nGroupID);
						
						wkpModules.WriteInt(strModule, ICP_COMPORT         , pModule->GetComPort());
						wkpModules.WriteInt(strModule, ICP_ADDRESS         , pModule->GetModuleAddress());
						wkpModules.WriteInt(strModule, ICP_BAUDRATE        , pModule->GetBaudrate());
						wkpModules.WriteInt(strModule, ICP_MULTIPLE_ACCESS , bMultipleAccess);
						wkpModules.WriteInt(strModule, ICP_POLL_STATE      , bPollState);
						wkpModules.WriteInt(strModule, ICP_MODULE_ENABLED  , bCheck);
						wkpModules.WriteString(strModule, ICP_MODULEID, pModule->GetModuleIDstring());

						if (!strLocation.IsEmpty())
						{
							wkpModules.WriteString(strModule, ICP_MODULE_LOCATION, strLocation);
						}

						if (nImage == ICP_IMAGE_INPUTS)
						{
							strValue.Format(_T("COM%d\\%02x\\%s"), pModule->GetComPort(), pModule->GetModuleAddress(), ICP_INPUTS);
							wkpPorts.WriteInt(strValue, ICP_GROUPID, nGroupID);
						}
						else
						{
							strValue.Format(_T("COM%d\\%02x\\%s"), pModule->GetComPort(), pModule->GetModuleAddress(), ICP_OUTPUTS);
							wkpPorts.WriteInt(strValue, ICP_GROUPID, nGroupID);
						}
					}
				}
				else
				{
					int nImage, nSelected;
					m_cTreeOfModules.GetItemImage(hTemp, nImage, nSelected);
					if ((nImage == ICP_IMAGE_INFO) && (nSelected == ICP_IMAGE_LOCATION))
					{
						strLocation = m_cTreeOfModules.GetItemText(hTemp);
					}
				}
			}
			if (bInputOn && bOutputOn)
			{
				nGroupID = MAKEWORD(pModule->GetModuleAddress(), pModule->GetComPort());
				strModule.Format(ICP_GROUP_FMT, nGroupID);
				wkpModules.DeleteSection(strModule);
			}
			hModule = m_cTreeOfModules.GetNextItem(hModule, TVGN_NEXT);
		}
		hPort = m_cTreeOfModules.GetNextItem(hPort, TVGN_NEXT);
	}

	strModule.Format(_T("%s.exe"), WK_APP_NAME_ICPCON_UNIT);
	wkpBase.WriteString(theApp.GetModuleSection(), WK_APP_NAME_ICPCON_UNIT, (m_bICPCONEnabled || m_bSwitchPanel) ? strModule : _T(""));

	m_dwChanged = 0;
	if (pInputList)
	{
		pInputList->Save(wkpBase, FALSE);
		m_dwChanged |= 1<<IMAGE_INPUT;
	}
	if (pOutputList)
	{
		pOutputList->Save(wkpBase, FALSE);
		m_dwChanged |= 1<<IMAGE_RELAY;
	}

	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, m_dwChanged);
	m_dwChanged = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::IsDataValid()
{
	OnBtnCancel();	// stop the search thread before calling message boxes
	if (UpdateData(TRUE))
	{
		if (m_bICPCONEnabled && m_bSwitchPanel)
		{
			return TRUE;
		}
		int nActive = 0;
		HTREEITEM hPort, hModule, hTemp;
		for (hPort = m_cTreeOfModules.GetRootItem(); hPort != NULL; hPort = m_cTreeOfModules.GetNextItem(hPort, TVGN_NEXT))
		{
			for (hModule = m_cTreeOfModules.GetNextItem(hPort, TVGN_CHILD); hModule != NULL; hModule = m_cTreeOfModules.GetNextItem(hModule, TVGN_NEXT))
			{
				for (hTemp = m_cTreeOfModules.GetNextItem(hModule, TVGN_CHILD); hTemp != NULL; hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_NEXT))
				{
					if (m_cTreeOfModules.ItemHasChildren(hTemp))
					{
						BOOL bCheck = m_cTreeOfModules.GetCheck(hTemp);
						if (bCheck) nActive++;
					}
				}
			}
		}
			  
		if (m_bICPCONEnabled)
		{
			if (nActive==0)
			{
				AfxMessageBox(IDS_ERROR_NO_MODULE_ACTIVE, MB_OK|MB_ICONEXCLAMATION);
				m_bICPCONEnabled = FALSE;
				CDataExchange dx(this, FALSE);
				DDX_Check(&dx, IDC_CK_ENABLE_ICPCON_MODULE, m_bICPCONEnabled);
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::Initialize()
{
	m_nWhat   = INITIALIZE_MODULES;
	SetThreadControlBtn(TRUE);
	m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadProc, (void*)this, 0, &m_nThreadID);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::CancelChanges()
{
	if (IsModified())
	{
		if (m_dwChanged & CHANGE_SAVE_MODULES)
		{
			OnBtnDeleteTree();
			Initialize();
		}
		if (m_dwChanged & CHANGE_SAVEPARAM)
		{
			SendMessage(WM_USER, USER_MSG_INIT_DIALOG, 0);
		}
		m_dwChanged = 0;
		SetModified(FALSE, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnNew()
{
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnDelete()
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	if (hItem)
	{
		int         nImage, nSelected, nPort = -1;
		HTREEITEM   hPort;
		CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
		m_cTreeOfModules.GetItemImage(hItem, nImage, nSelected);
		if (nImage == ICP_IMAGE_MODULE)
		{
			hPort = m_cTreeOfModules.GetNextItem(hItem, TVGN_PARENT);
			nPort = DeleteModule(hItem);
			m_cTreeOfModules.DeleteItem(hItem);
			if (m_cTreeOfModules.ItemHasChildren(hPort))
			{
				nPort = -1;
			}
			else
			{
				m_cTreeOfModules.DeleteItem(hPort);
			}
		}
		else if (nImage == ICP_IMAGE_PORT)
		{
			hPort = hItem;
			hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_CHILD);
			while (hItem)
			{
				nPort = DeleteModule(hItem);
				hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_NEXT);
			}
			m_cTreeOfModules.DeleteItem(hPort);
		}
		if (nPort != -1)
		{
			CWK_Profile  wkpPorts(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationPorts, _T(""));
			CString strSection;
			strSection.Format(_T("COM%d"), nPort);
			wkpPorts.DeleteSection(strSection);
		}
		m_pParent->GetDocument()->GetInputs()->Save(wkpBase, FALSE);
		m_pParent->GetDocument()->GetOutputs()->Save(wkpBase, FALSE);
		m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, 1<<IMAGE_INPUT|1<<IMAGE_RELAY);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::CanDelete()
{
	if (m_bIsLocal)
	{
		HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
		if (hItem)
		{
			int nImage, nSelected;
			m_cTreeOfModules.GetItemImage(hItem, nImage, nSelected);
			if ((nImage == ICP_IMAGE_MODULE) || (nImage == ICP_IMAGE_PORT))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CICPCONConfigPage::DeleteModule(HTREEITEM hModule)
{
	int nImage, nSelected, nPort = -1;
	m_cTreeOfModules.GetItemImage(hModule, nImage, nSelected);
	if (nImage == ICP_IMAGE_MODULE)
	{
		CString          strSection, strModule;
		int              nGroupID, nModuleAddress;
		CWK_Profile      wkpPorts(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationPorts, _T(""));
		CICPI7000Module *pModule = (CICPI7000Module*) m_cTreeOfModules.GetItemData(hModule);
		nPort          = pModule->GetComPort();
		nModuleAddress = pModule->GetModuleAddress();
		strSection.Format(_T("COM%d\\%02x\\%s"), nPort, nModuleAddress, ICP_INPUTS);
		nGroupID = wkpPorts.GetInt(strSection, ICP_GROUPID, 0);
		DeleteGroup(nGroupID);
		strSection.Format(_T("COM%d\\%02x\\%s"), nPort, nModuleAddress, ICP_OUTPUTS);
		nGroupID = wkpPorts.GetInt(strSection, ICP_GROUPID, 0);
		DeleteGroup(nGroupID);
		strSection.Format(_T("COM%d\\%02x"), nPort, nModuleAddress);
		wkpPorts.DeleteSection(strSection);
	}
	return nPort;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::DeleteGroup(int nGroupID)
{
	if (nGroupID)
	{
		CString     strModule;
		CSecID      id((WORD)nGroupID, 0);
		CWK_Profile wkpModules(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationModules, _T(""));
		if ((nGroupID & 0x0000f000) == 0x00001000)
		{
			CInputList  *pInputList  = m_pParent->GetDocument()->GetInputs();
			CInputGroup *pInputGroup = pInputList->GetGroupByID(id);
			if (pInputGroup)
			{
				CheckActivationsWithInputGroup(id, SVP_CO_CHECK_SILENT);
				CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
				pInputList->DeleteGroup(pInputGroup->GetSMName());
				pInputList->Save(wkpBase, FALSE);
			}
		}
		else if ((nGroupID & 0x0000f000) == 0x00002000)
		{
			COutput*pOutput;
			COutputList  *pOutputList  = m_pParent->GetDocument()->GetOutputs();
			COutputGroup *pOutputGroup = pOutputList->GetGroupByID(id);
			if (pOutputGroup)
			{
				int i, nSize = pOutputGroup->GetSize();
				for (i=0; i<nSize; i++)
				{
					pOutput = pOutputGroup->GetOutput(i);
					CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_SILENT);
				}
				CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
				pOutputList->DeleteGroup(pOutputGroup->GetSMName());
				pOutputList->Save(wkpBase, FALSE);
			}
		}
		strModule.Format(ICP_GROUP_FMT, nGroupID);
		wkpModules.DeleteSection(strModule);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::InitModules()
{
	CWK_Profile wkpModules(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationModules, _T(""));
	CWK_Profile wkpUnit(   CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));
	const int nValueNameSize = 64;
	TCHAR  szValueName[nValueNameSize];
	LONG  lResult = ERROR_SUCCESS;
	int   nModule=0, nAddress, nComPort, nBaudrate, nGroupID;
	HKEY  hModuleKey = NULL;
	CString str;

	HWND hwnd = ::FindWindow(NULL, WK_APP_NAME_ICPCON_UNIT);
	if (hwnd)
	{
		DWORD dwResult;
		::SendMessageTimeout(hwnd, WK_WINTHREAD_STOP_THREAD, 1, 0, SMTO_BLOCK, 5000, &dwResult);
		DWORD dwError = GetLastError();
		if (dwError)
		{
			AfxMessageBox(IDS_ERROR_OPENING_ICPCON_UNIT, MB_OK|MB_ICONERROR);
			return;
		}
	}

	CPtrList PortList;
	hModuleKey = wkpUnit.GetSectionKey(REG_LOC_MODULE_UNIT);
	if (hModuleKey)
	{
		for (nModule=0; lResult==ERROR_SUCCESS; nModule++)									// Enum the Ports
		{
			if (m_bStopThread)
				break;
			lResult = RegEnumKey(hModuleKey, nModule, (LPTSTR)szValueName, nValueNameSize-1);
			if (lResult == ERROR_SUCCESS)
			{
				_stscanf(szValueName, ICP_GROUP_FMT, &nGroupID);

				nAddress  = wkpModules.GetInt(szValueName, ICP_ADDRESS , -1);
				nComPort  = wkpModules.GetInt(szValueName, ICP_COMPORT , -1);
				nBaudrate = wkpModules.GetInt(szValueName, ICP_BAUDRATE, -1);
				if ((nAddress == -1) || (nComPort == -1) || (nBaudrate == -1))
					continue;
				if (PortList.Find((void*)nComPort))
					continue;
				m_nComPort            = nComPort;
				m_wAddresses.RemoveAll();
				m_wAddresses.Add((WORD)nAddress);
				SetBaudCheck(nBaudrate);
				if (!OnPollPort())
				{
					PortList.AddHead((void*)nComPort);
				}
			}
		}
		::RegCloseKey(hModuleKey);
	}
	PostMessage(WM_USER, USER_MSG_INIT_DIALOG, 0);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnEndlabeleditTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if (pTVDispInfo->item.mask	& TVIF_TEXT)
	{
		int nImage, nSelected;
		m_cTreeOfModules.GetItemImage(pTVDispInfo->item.hItem, nImage, nSelected);
		m_cTreeOfModules.SetItemText( pTVDispInfo->item.hItem, pTVDispInfo->item.pszText);
		if (    (nImage    == ICP_IMAGE_INFO)
			 && ((nSelected == ICP_IMAGE_MODULE) || (nSelected == ICP_IMAGE_LOCATION)))
		{
			m_cTreeOfModules.SetItemImage(pTVDispInfo->item.hItem, nImage, ICP_IMAGE_LOCATION);
		}
		m_dwChanged |= CHANGE_SAVE_MODULES;
		SetModified(TRUE, FALSE);
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnClickTreeOfModules(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TVHITTESTINFO tvHTI;
	int nImage = -1, nSelected = -1;
	BOOL bConfigModule = FALSE;
	GetCursorPos(&tvHTI.pt);
	m_cTreeOfModules.ScreenToClient(&tvHTI.pt);
	m_cTreeOfModules.HitTest(&tvHTI);

	m_pModule       = NULL;

	if (tvHTI.hItem)
	{
		m_cTreeOfModules.GetItemImage(tvHTI.hItem, nImage, nSelected);
	}
	if (tvHTI.flags & TVHT_ONITEMSTATEICON)
	{
		if (    (nImage == ICP_IMAGE_PORT)
			  || (nImage == ICP_IMAGE_MODULE)
			  || (nImage == ICP_IMAGE_INPUTS)
			  || ((nImage == ICP_IMAGE_INFO)  && !m_cTreeOfModules.ItemHasChildren(tvHTI.hItem)))
		{
			PostMessage(WM_USER, USER_MSG_TOGGLE_CHECK_STATE, (LPARAM)tvHTI.hItem);
			if (theApp.m_bProfessional && nSelected == ICP_IMAGE_CHECKSUM)
			{
				m_dwConfigChanged = CONFIG_CHECKSUM;
			}
		}
		else if ((nImage == ICP_IMAGE_INFO)  && m_cTreeOfModules.ItemHasChildren(tvHTI.hItem))
		{
			PostMessage(WM_USER, USER_MSG_REQUEST_SWITCH_GROUP, (LPARAM)tvHTI.hItem);
		}
	}
	if (tvHTI.flags & TVHT_ONITEM)
	{
		int nParent, nTemp;
		HTREEITEM hTemp;
		CDataExchange dx(this, false);
		BOOL  bGroup  = m_cTreeOfModules.ItemHasChildren(tvHTI.hItem);
		if (m_cBtnConfigModules.IsWindowVisible() && (nImage != ICP_IMAGE_MODULE))
		{
			SendMessage(WM_USER, USER_MSG_INIT_DIALOG, 0);
		}
		if (  ((nImage == ICP_IMAGE_INFO) && bGroup)
			 || (nImage == ICP_IMAGE_INPUTS)
			 || (nImage == ICP_IMAGE_OUTPUTS))
		{
			WORD  wRet = NoError;
			hTemp = tvHTI.hItem;
			if ((tvHTI.flags & TVHT_ONITEMSTATEICON) && (nImage == ICP_IMAGE_INPUTS))
			{
				PostMessage(WM_USER, USER_MSG_SET_CHILD_CHECK_STATE, (LPARAM)hTemp);
				tvHTI.hItem = m_cTreeOfModules.GetNextItem(hTemp, TVGN_PARENT);
				bGroup = TRUE;
			}

			while(hTemp) 
			{
				m_cTreeOfModules.GetItemImage(hTemp, nParent, nTemp);
				if (nParent == ICP_IMAGE_MODULE)
				{
					m_pModule = (CICPI7000Module*)m_cTreeOfModules.GetItemData(hTemp);
					ASSERT(m_pModule != NULL);
					break;
				}
				hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_PARENT);
			};
			if (m_pModule->GetFailSeconds()==0)
			{
				m_pModule->SetTimeOut((WORD)m_nTimeOut);
				m_nActive = m_cTreeOfModules.GetItemData(tvHTI.hItem);
				m_pModule->OpenPort();
				if (bGroup)
				{
					int i;
					hTemp = m_cTreeOfModules.GetNextItem(tvHTI.hItem, TVGN_CHILD);
					if (hTemp) m_cTreeOfModules.GetItemImage(hTemp, nImage, nSelected);
					
					if ((nImage == ICP_IMAGE_OUTPUTS))
					{
						wRet = m_pModule->DigitalOutReadBack();
						if (wRet == NoError)
						{
							for (hTemp = m_cTreeOfModules.GetNextItem(tvHTI.hItem, TVGN_CHILD), i=0; hTemp!=NULL; hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_NEXT), i++)
							{
								m_cTreeOfModules.SetCheck(hTemp, m_pModule->GetDObit(i));
							}
						}
					}
					else if (m_pModule->HasInputs())
					{
						wRet = m_pModule->DigitalIn();
						if (wRet == NoError)
						{
							for (hTemp = m_cTreeOfModules.GetNextItem(tvHTI.hItem, TVGN_CHILD), i=0; hTemp!=NULL; hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_NEXT), i++)
							{
								m_cTreeOfModules.SetCheck(hTemp, m_pModule->GetDIbit(i));
							}
						}
					}
				}
				else if (tvHTI.flags & TVHT_ONITEMSTATEICON)
				{
					BOOL bCheck = !m_cTreeOfModules.GetCheck(tvHTI.hItem);
					if (nImage == ICP_IMAGE_OUTPUTS)
					{
						m_pModule->SetDObit(m_nActive, bCheck ? true:false);
						m_pModule->DigitalOut();
					}
				}
				Sleep(250);
				m_pModule->ClosePort();
			}
			else
			{
				m_pModule = NULL;
			}
			if (bGroup)
			{
				nSelected = m_cTreeOfModules.GetItemData(tvHTI.hItem);
				DDX_Check(&dx, IDC_CK_POLL_STATE, nSelected);
			}
			GetDlgItem(IDC_CK_POLL_STATE)->EnableWindow(bGroup);
			GetDlgItem(IDC_CK_MULTIPLE_ACCESS)->EnableWindow(FALSE);
		}
		else
		{
			hTemp = tvHTI.hItem;
			BOOL bEnableMA  = FALSE;
			BOOL bEnablePS  = FALSE;
			BOOL bEnablePSA = TRUE;
			BOOL bEnablePA  = TRUE;
			BOOL bEnableMSA = TRUE;
			BOOL bEnableMOA = FALSE;
			BOOL bEnableSAP = TRUE;
			BOOL bEnablePP  = TRUE;
			BOOL bEnableTPR = TRUE;
			BOOL bEnableTO  = TRUE;
			if (nImage == ICP_IMAGE_PORT)
			{
				DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, nSelected);
				bEnableMA = TRUE;
			}
			else if (nImage == ICP_IMAGE_MODULE && theApp.m_bProfessional)
			{
				m_pModule     = (CICPI7000Module*)m_cTreeOfModules.GetItemData(hTemp);
				ASSERT(m_pModule != NULL);
				if (m_pModule->GetFailSeconds() == 0)
				{
					m_nModuleAddress = nSelected;
					m_nComPort    = m_pModule->GetComPort();
					m_dwConfigChanged = 0;
					bConfigModule = TRUE;
					hTemp = m_cTreeOfModules.GetNextItem(hTemp, TVGN_CHILD);
					m_cBtnConfigModules.SetWindowText(m_strBtnConfigModule + _T(": ") + m_cTreeOfModules.GetItemText(hTemp));
					bEnablePSA    = FALSE;
					bEnableMSA    = FALSE;
					bEnablePA     = FALSE;
					bEnableMOA	  = TRUE;
					bEnableSAP    = FALSE;
					bEnablePP     = FALSE;
					bEnableTPR    = FALSE;
					bEnableTO     = FALSE;
					bConfigModule = TRUE;
					SetBaudCheck(m_pModule->GetBaudrate());
					UpdateData(false);
				}
				else
				{
					m_pModule = NULL;
				}
			}

			m_cBtnConfigModules.ShowWindow(bConfigModule ? SW_SHOW : SW_HIDE);
			GetDlgItem(IDC_EDT_PORT_START          )->EnableWindow(bEnablePSA);
			GetDlgItem(IDC_EDT_PORT                )->EnableWindow(bEnablePA);
			GetDlgItem(IDC_EDT_MODULE_ADDRESS_START)->EnableWindow(bEnableMSA);
			GetDlgItem(IDC_EDT_MODULE_ADDRESS      )->EnableWindow(bEnableMOA);
			GetDlgItem(IDC_CK_POLL_STATE           )->EnableWindow(bEnablePS);
			GetDlgItem(IDC_CK_MULTIPLE_ACCESS      )->EnableWindow(bEnableMA);
			GetDlgItem(IDC_BTN_SCAN_ALL_PORTS      )->EnableWindow(bEnableSAP);
			GetDlgItem(IDC_BTN_POLL_PORT           )->EnableWindow(bEnablePP);
			GetDlgItem(IDC_EDT_TIME_POLL_RELAIS    )->EnableWindow(bEnableTPR);
			GetDlgItem(IDC_EDT_TIMEOUT             )->EnableWindow(bEnableTO);
		}
	}

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnCkComStates() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	while (hItem)
	{
		int nImage, bState, nTemp;
		m_cTreeOfModules.GetItemImage(hItem, nImage, nTemp);
		if (nImage == ICP_IMAGE_PORT)
		{
			CDataExchange dx(this, true);
			DDX_Check(&dx, IDC_CK_MULTIPLE_ACCESS, bState);
			m_cTreeOfModules.SetItemImage(hItem, nImage, bState);
			SetModified(TRUE, FALSE);
			m_dwChanged |= CHANGE_SAVE_MODULES;
			break;
		}
		hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_PARENT);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnCkModuleStates() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	while (hItem)
	{
		int nImage, bState, nTemp;
		m_cTreeOfModules.GetItemImage(hItem, nImage, nTemp);
		if (nImage == ICP_IMAGE_INFO && m_cTreeOfModules.ItemHasChildren(hItem))
		{
			CDataExchange dx(this, true);
			DDX_Check(&dx, IDC_CK_POLL_STATE, bState);
			m_cTreeOfModules.SetItemData(hItem, bState);
			SetModified(TRUE, FALSE);
			m_dwChanged |= CHANGE_SAVE_MODULES;
			break;
		}
		hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_PARENT);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CICPCONConfigPage::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	switch (wParam)
	{
		case USER_MSG_SET_PARENT_CHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			if (bCheck)
			{
				while (hItem)
				{
					hItem = m_cTreeOfModules.GetParentItem(hItem);
					if (hItem)
					{
						m_cTreeOfModules.SetCheck(hItem, bCheck);
					}
				}
			}
		} break;
		case USER_MSG_SET_THREAD_CONTROL_BTN:
		{
			SetThreadControlBtn((BOOL)lParam);
		} break;
		case USER_MSG_EXPAND_TREE_ITEM:
		{
			m_cTreeOfModules.Expand((HTREEITEM)lParam, TVE_EXPAND);
			m_cTreeOfModules.InvalidateRect(NULL);
		} break;
		case USER_MSG_SET_PROGRESS_ADDRESS:
		{
			m_cProgressAddress.SetPos(lParam);
			SetDlgItemInt(IDC_EDT_MODULE_ADDRESS, lParam, FALSE);
		} break;
		case USER_MSG_SET_PROGRESS_PORT:
		{
			m_cProgressPorts.SetPos(m_nComPort);
			SetDlgItemInt(IDC_EDT_PORT, m_nComPort, FALSE);
		} break;
		case USER_MSG_DELETE_IF_NO_CHILDREN:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			if (!m_cTreeOfModules.ItemHasChildren(hItem))
			{
				m_cTreeOfModules.DeleteItem(hItem);
			}
		} break;
		case USER_MSG_SET_CHILD_CHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_CHILD);
			while (hItem)
			{
				m_cTreeOfModules.SetCheck(hItem, bCheck);
				hItem = m_cTreeOfModules.GetNextItem(hItem, TVGN_NEXT);
			}
		} break;
		case USER_MSG_TOGGLE_CHECK_STATE:
		{
			HTREEITEM hItem = (HTREEITEM) lParam;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			m_cTreeOfModules.SetCheck(hItem, !bCheck);
		} break;
		case USER_MSG_REQUEST_SWITCH_GROUP:
		{
			int nType, nTemp, nGroup;
			HTREEITEM hItem = (HTREEITEM) lParam;
			HTREEITEM hTemp;
			BOOL bCheck = m_cTreeOfModules.GetCheck(hItem);
			if (!bCheck)
			{
				m_cTreeOfModules.GetItemImage(hItem, nTemp, nGroup);
				hTemp = m_cTreeOfModules.GetNextItem(hItem, TVGN_CHILD);
				if (hTemp)
				{
					m_cTreeOfModules.GetItemImage(hTemp, nType, nTemp);
					if (nType == ICP_IMAGE_INPUTS)
					{
						BOOL bNoActivations = CheckActivationsWithInputGroup(CSecID((WORD)nGroup, 0));
						if (!bNoActivations)
						{
							m_cTreeOfModules.SetCheck(hItem, TRUE);
							return 0;
						}
					}
					else if (nType == ICP_IMAGE_OUTPUTS)
					{
						COutput*pOutput;
						COutputList  *pOutputList  = m_pParent->GetDocument()->GetOutputs();
						COutputGroup *pOutputGroup = pOutputList->GetGroupByID(CSecID((WORD)nGroup, 0));
						if (pOutputGroup)
						{
							int i, nSize = pOutputGroup->GetSize();
							CString sNames;
							for (i=0; i<nSize; i++)
							{
								pOutput = pOutputGroup->GetOutput(i);
								if (!CheckActivationsWithOutput(pOutput->GetID(), 0, SVP_CO_CHECK_ONLY))
								{
									sNames += _T(" * ") + pOutput->GetName() + _T("\n");
								}
							}
							if (!sNames.IsEmpty())
							{
								CString sMsg;
								sMsg.LoadString(IDP_RELAIS_WITH_ACTIVATION);
								sMsg += _T("\n\n") + sNames;
								if (IDNO == AfxMessageBox(sMsg, MB_YESNO))
								{
									m_cTreeOfModules.SetCheck(hItem, TRUE);
									return 0;
								}
							}
						}
					}
				}
			}
			// Ein bzw. Ausgänge an oder ausschalten
			m_dwChanged |= CHANGE_SAVE_MODULES;
			SetModified(TRUE, TRUE);
		}break;
		case USER_MSG_INIT_DIALOG:
		{
			CWK_Profile wkpBase(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sBaseDVRT, _T(""));
			CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));
			m_nComPort         = wkp.GetInt(SETTINGS_SECTION, ICP_COMPORT          , m_nComPort);
			m_nPollTime        = wkp.GetInt(SETTINGS_SECTION, ICP_POLLTIME         , m_nPollTime);
			m_nTimeOut         = wkp.GetInt(SETTINGS_SECTION, ICP_TIMEOUT          , m_nTimeOut);

			DWORD dwBaudrates  = wkp.GetInt(SETTINGS_SECTION, ICP_BAUDRATES, 0);

			SetBaudCheck(0);
			if (dwBaudrates)
			{
				if (dwBaudrates&0x00000001) m_bBaud1200   = TRUE;
				if (dwBaudrates&0x00000002) m_bBaud2400   = TRUE;
				if (dwBaudrates&0x00000004) m_bBaud4800   = TRUE;
				if (dwBaudrates&0x00000008) m_bBaud9600   = TRUE;
				if (dwBaudrates&0x00000010) m_bBaud19200  = TRUE;
				if (dwBaudrates&0x00000020) m_bBaud38400  = TRUE;
				if (dwBaudrates&0x00000040) m_bBaud57600  = TRUE;
				if (dwBaudrates&0x00000080) m_bBaud115200 = TRUE;
			}
			else
			{
				m_bBaud9600 = TRUE;
			}

			m_bSwitchPanel = wkp.GetInt(SETTINGS_SECTION, ICP_SWITCH_PANEL, 0);
			m_szButton.cx = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DX, m_szButton.cx);
			m_szButton.cy = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DY, m_szButton.cy);
			CString sMinPos = wkp.GetString(SETTINGS_SECTION, ICP_MINIMIZE_POSITION, NULL);
			if (sMinPos.IsEmpty())
			{
				m_comboMinimize.SetCurSel(0);
			}
			else
			{
				sMinPos.MakeLower();
				if      (sMinPos == MINPOS_LEFTTOP    ) m_comboMinimize.SetCurSel(1);
				else if (sMinPos == MINPOS_LEFTBOTTOM ) m_comboMinimize.SetCurSel(2);
				else if (sMinPos == MINPOS_RIGHTTOP   ) m_comboMinimize.SetCurSel(3);
				else if (sMinPos == MINPOS_RIGHTBOTTOM) m_comboMinimize.SetCurSel(4);
				else                                    m_comboMinimize.SetCurSel(0);
			}
			m_comboMinimize.EnableWindow(m_bSwitchPanel);
			GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
			GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);

			CString strModule;
			strModule.Format(_T("%s.exe"), WK_APP_NAME_ICPCON_UNIT);
			strModule = wkpBase.GetString(theApp.GetModuleSection(), WK_APP_NAME_ICPCON_UNIT, NULL);
			m_bICPCONEnabled = !strModule.IsEmpty();

			UpdateData(FALSE);
		} break;
		case USER_MSG_REQUEST_DELETE_PORT:
		{
			CString strFormat;
			strFormat.Format(IDS_REQUEST_DELETE_PORT, lParam);
			if (AfxMessageBox(strFormat, MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				CWK_Profile wkpBase;
				CWK_Profile wkpPorts(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationPorts, _T(""));
				CString   strSection, strSubSection;
				int       nAddress, nGroupID, nPort = lParam;
				const int nValueNameSize = 64;
				char      szValueName[nValueNameSize];
				LONG      lResult = ERROR_SUCCESS;

				strSection.Format(_T("COM%d"), nPort);
				HKEY hKey = wkpPorts.GetSectionKey(strSection);
				if (hKey)
				{
					for (nAddress=0; lResult==ERROR_SUCCESS; nAddress++)									// Enum the Ports
					{
						lResult = RegEnumKey(hKey, nAddress, (LPTSTR)szValueName, nValueNameSize-1);
						if (lResult == ERROR_SUCCESS)
						{
							strSubSection.Format(_T("COM%d\\%s\\%s"), nPort, szValueName, ICP_INPUTS);
							nGroupID = wkpPorts.GetInt(strSubSection, ICP_GROUPID, 0);
							DeleteGroup(nGroupID);
							strSubSection.Format(_T("COM%d\\%s\\%s"), nPort, szValueName, ICP_OUTPUTS);
							nGroupID = wkpPorts.GetInt(strSubSection, ICP_GROUPID, 0);
							DeleteGroup(nGroupID);
						}
					}
				}
				wkpPorts.DeleteSection(strSection);
				::RegCloseKey(hKey);
				m_pParent->GetDocument()->GetInputs()->Save(wkpBase, FALSE);
				m_pParent->GetDocument()->GetOutputs()->Save(wkpBase, FALSE);
				m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, 1<<IMAGE_INPUT|1<<IMAGE_RELAY);
			}
		}break;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnOK() 
{
	CSVPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnEditTreeLabel() 
{
	HTREEITEM hItem = m_cTreeOfModules.GetSelectedItem();
	if (hItem)
	{
		m_cTreeOfModules.EditLabel(hItem);
	}
}
/////////////////////////////////////////////////////////////////////////////
int CICPCONConfigPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_hAccelerator = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ICPCONPAGE)); 	
	
	if (CSVPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAccelerator)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg))
		{
			return TRUE;
		}
	}
	
	return CSVPage::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
long CICPCONConfigPage::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_ICPCON_CLIENT, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnChangeEdtModuleAddress() 
{
	if (m_cBtnConfigModules.IsWindowVisible())
	{
		m_dwConfigChanged |= CONFIG_ADDRESS;
		m_cBtnConfigModules.EnableWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnBtnConfigModule() 
{
	if (   m_cBtnConfigModules.IsWindowVisible()
		 && (m_dwConfigChanged!=0))
	{
		if (AfxMessageBox(IDS_CHANGE_MODULEPARAMETER, MB_OKCANCEL|MB_ICONINFORMATION) == IDCANCEL)
		{
			return;
		}
		int   nID    = IDS_MODULEPARAMETER_ERROR;
		DWORD dwFlag = MB_ICONERROR;
		if (m_pModule->OpenPort())
		{
			char szCmd[70], szResult[70];
			WORD  wT, wRetValue;
			int nModuleAddress = m_pModule->GetModuleAddress();
			sprintf(szCmd, "$%02x2", nModuleAddress);
			wRetValue = Send_Receive_Cmd(m_pModule->GetComPort(), szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
			if (wRetValue != NoError) return;

			if (m_dwConfigChanged & CONFIG_ADDRESS)
			{
				CDataExchange dx(this, true);
				DDX_Text(&dx, IDC_EDT_MODULE_ADDRESS, nModuleAddress);
			}
			
			sprintf(szCmd, "%%%02x%02x4003%02x", m_pModule->GetModuleAddress(), nModuleAddress, m_pModule->m_wConfig);
			switch (m_dwConfigChanged & CONFIG_BAUDRATE)
			{
				case 0x0001 : szCmd[8]= '3'; break;
				case 0x0002 : szCmd[8]= '4'; break;
				case 0x0004 : szCmd[8]= '5'; break;
				case 0x0008 : szCmd[8]= '6'; break;
				case 0x0010 : szCmd[8]= '7'; break;
				case 0x0020 : szCmd[8]= '8'; break;
				case 0x0040 : szCmd[8]= '9'; break;
				case 0x0080 : szCmd[8]= 'A'; break;
				default:      szCmd[8]= szResult[6]; break;
			}

			wRetValue = Send_Receive_Cmd(m_pModule->GetComPort(), szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
			sprintf(szCmd, "!%02x", nModuleAddress);
			if ((wRetValue == NoError) && (strcmp(szResult, szCmd) == 0))
			{
				sprintf(szCmd, "$%02x2", nModuleAddress);
				wRetValue = Send_Receive_Cmd(m_pModule->GetComPort(), szCmd, szResult, (WORD)m_nTimeOut, 0, &wT);
				m_pModule->SetConfig(szResult);
				int nOldAddress = m_pModule->GetModuleAddress();
				if (nOldAddress != nModuleAddress)
				{
					CWK_Profile wkpPorts(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocationPorts, _T(""));
					CString sSection;
					sSection.Format(_T("Com%d\\%02x"), m_pModule->GetComPort(), m_pModule->GetModuleAddress());
					wkpPorts.DeleteSection(sSection);
					m_pModule->SetModuleAddress((WORD)nModuleAddress);
					// delete in- and out- put groups
					// the shared memory name consists of comport and module address
					CString strSMname;
					strSMname.Format(_T("%s%02x%02x"), SM_ICPCONUnitInput, m_pModule->GetComPort(), nOldAddress);
					CInputGroup * pInputGroup = m_pParent->GetDocument()->GetInputs()->GetGroupBySMName(strSMname);
					if (pInputGroup)
					{
						DeleteGroup(pInputGroup->GetID().GetGroupID());	// list is saved here
						m_dwChanged |= CHANGE_SAVE_MODULES;
						m_dwChanged |= 1<<IMAGE_INPUT;
					}
					strSMname.Format(_T("%s%02x%02x"), SM_ICPCONUnitOutput, m_pModule->GetComPort(), nOldAddress);
					COutputGroup * pOutputGroup = m_pParent->GetDocument()->GetOutputs()->GetGroupBySMName(strSMname);
					if (pOutputGroup)
					{
						DeleteGroup(pOutputGroup->GetID().GetGroupID());	// list is saved here
						m_dwChanged |= CHANGE_SAVE_MODULES;
						m_dwChanged |= 1<<IMAGE_RELAY;
					}
				}
				m_dwConfigChanged = 0;
				m_cBtnConfigModules.EnableWindow(FALSE);
				SaveChanges();
				nID    = IDS_MODULEPARAMETER_CHANGED;
				dwFlag = MB_ICONINFORMATION;
			}
			AfxMessageBox(nID, MB_OK|dwFlag);
			m_pModule->ClosePort();
		}			
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnChkBaud(UINT nID)
{
	if (m_cBtnConfigModules.IsWindowVisible())
	{
		CDataExchange dx(this, false);
		m_dwConfigChanged &= ~CONFIG_BAUDRATE;
		m_dwConfigChanged |= (1 << (nID - IDC_CK_BAUD_1200));
		m_cBtnConfigModules.EnableWindow();
		UINT i, iID;
		BOOL bCheck;
		for (i=0,iID=IDC_CK_BAUD_1200; iID<=IDC_CK_BAUD_115200; i++, iID++)
		{
			bCheck = ((1 << i) & m_dwConfigChanged)?TRUE:FALSE;
			DDX_Check(&dx, iID, bCheck);
		}
	}
	else
	{
		SetModified(TRUE, FALSE);
		m_dwChanged |= CHANGE_SAVEPARAM;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnChangeEdtTimeout() 
{
	SetModified(TRUE, TRUE);
	m_dwChanged |= CHANGE_SAVEPARAM;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnChangeEdtTimePollRelais() 
{
	SetModified(TRUE, TRUE);
	m_dwChanged |= CHANGE_SAVEPARAM;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::SetBaudCheck(int nBaudrate)
{
	m_bBaud1200   = FALSE;
	m_bBaud2400   = FALSE;
	m_bBaud4800   = FALSE;
	m_bBaud9600   = FALSE;
	m_bBaud19200  = FALSE;
	m_bBaud38400  = FALSE;
	m_bBaud57600  = FALSE;
	m_bBaud115200 = FALSE;
	switch (nBaudrate)
	{
		case   CBR_1200: m_bBaud1200   = TRUE; break;
		case   CBR_2400: m_bBaud2400   = TRUE; break;
		case   CBR_4800: m_bBaud4800   = TRUE; break;
		case   CBR_9600: m_bBaud9600   = TRUE; break;
		case  CBR_19200: m_bBaud19200  = TRUE; break;
		case  CBR_38400: m_bBaud38400  = TRUE; break;
		case  CBR_57600: m_bBaud57600  = TRUE; break;
		case CBR_115200: m_bBaud115200 = TRUE; break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnCkEnableSwitchPanel() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
	if (!m_bSwitchPanel)
	{
		CInputList  *pInputList  = m_pParent->GetDocument()->GetInputs();
		CInputGroup *pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
		if (pInputGroup)
		{
			BOOL bNoActivations = CheckActivationsWithInputGroup(pInputGroup->GetID());
			if (!bNoActivations)
			{
				dx.m_bSaveAndValidate = FALSE;
				m_bSwitchPanel = TRUE;
				DDX_Check(&dx, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
				return;
			}
		}
	}

	SetModified(TRUE, FALSE);
	m_dwChanged |= CHANGE_SAVEPARAM;
	GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
	GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);
	m_comboMinimize.EnableWindow(m_bSwitchPanel);
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnCbnSelchangeIpcComboMinimize()
{
	SetModified(TRUE, FALSE);
	m_dwChanged |= CHANGE_SAVEPARAM;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnCkEnableIcpconModule() 
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ENABLE_ICPCON_MODULE, m_bICPCONEnabled);
	if (!m_bICPCONEnabled)
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bICPCONEnabled	= TRUE;
			dx.m_bSaveAndValidate = FALSE;
			DDX_Check(&dx, IDC_CK_ENABLE_ICPCON_MODULE, m_bICPCONEnabled);
		}
		else
		{
			SetModified(TRUE, TRUE);
		}
	}
	else
	{
		SetModified(TRUE, TRUE);
	}
	m_dwChanged |= CHANGE_SAVE_MODULES;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::StretchElements()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CICPCONConfigPage::GetToolTip(UINT nID, CString&sText)
{
	switch (nID)
	{
		case IDC_CK_BAUD_2400:
		case IDC_CK_BAUD_4800:
		case IDC_CK_BAUD_9600:
		case IDC_CK_BAUD_19200:
		case IDC_CK_BAUD_38400:
		case IDC_CK_BAUD_57600:
		case IDC_CK_BAUD_115200:
			return sText.LoadString(IDC_CK_BAUD_1200);
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CICPCONConfigPage::OnSize(UINT nType, int cx, int cy) 
{
	CSVPage::OnSize(nType, cx, cy);
	CRect rc;
	CWnd *pWnd = GetDlgItem(IDC_LINE_ONE);
	if (pWnd)
	{
		pWnd->GetClientRect(&rc);
		pWnd->SetWindowPos(NULL, 0, 0, rc.right, 2, SWP_NOMOVE|SWP_NOOWNERZORDER);
	}
	pWnd = GetDlgItem(IDC_LINE_TWO);
	if (pWnd)
	{
		pWnd->GetClientRect(&rc);
		pWnd->SetWindowPos(NULL, 0, 0, rc.right, 2, SWP_NOMOVE|SWP_NOOWNERZORDER);
	}
	pWnd = GetDlgItem(IDC_LINE_THREE);
	if (pWnd)
	{
		pWnd->GetClientRect(&rc);
		pWnd->SetWindowPos(NULL, 0, 0, rc.right, 2, SWP_NOMOVE|SWP_NOOWNERZORDER);
	}
	pWnd = GetDlgItem(IDC_LINE_FOUR);
	if (pWnd)
	{
		pWnd->GetClientRect(&rc);
		pWnd->SetWindowPos(NULL, 0, 0, 2, rc.bottom, SWP_NOMOVE|SWP_NOOWNERZORDER);
	}
}
