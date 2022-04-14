// SwitchPanel.cpp : implementation file
//

#include "stdafx.h"
#include "ICPCONClient.h"
#include "SwitchPanel.h"
#include "ICPCONClientDlg.h"
#include "MinimizedDlg.h"

#define COMPILE_MULTIMON_STUBS
#include "multimon.h"

#include "i7000.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POLL_TIMER     0x0815
#define MINIMIZE_TIMER 0x0816

/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel dialog
extern CIPCCONClient theApp;

CSwitchPanel::CSwitchPanel(CWnd* pParent /*=NULL*/)
	: CTransparentDialog(CSwitchPanel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSwitchPanel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hAccelerator     = NULL;
	m_pMiniDlg         = NULL;
	m_hIcon				 = NULL;

	m_nPollTimer       = 0;
	m_nPollTime        = 5000;
	m_nTimeout         = 100;
	m_nTimeoutEdge     = 500;
	m_bThreadsArePolling = false;

	m_nMinimizeTimer   = 0;
	m_nMinimizeTime    = 10;
	m_nMinimizeCounter = 0;
	
	m_nButtonShape     = 0;
	m_nSkinColor       = 0;
	m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);

}
/////////////////////////////////////////////////////////////////////////////
CSwitchPanel::~CSwitchPanel()
{
	WK_DELETE(m_pMiniDlg);
	CICPI7000Thread::DeletePerformanceMonitor();
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::DoDataExchange(CDataExchange* pDX)
{
	CTransparentDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSwitchPanel)
	DDX_Control(pDX, IDC_RELAIS, m_cFirstRelais);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSwitchPanel, CTransparentDialog)
	//{{AFX_MSG_MAP(CSwitchPanel)
	ON_BN_CLICKED(IDC_BTN_CONFIG, OnBtnConfig)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(IDC_BTN_POLL_PORT, OnBtnPollPort)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_USER, OnUserMessage)
	ON_MESSAGE(WM_CHECK_SKINBUTTON, OnCheckSkinBtn)
	ON_COMMAND_RANGE(IDC_RELAIS, IDC_RELAIS_LAST, OnCheck)
	ON_COMMAND(IDC_BTN_CANCEL, OnBtnCancel)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)	
	ON_MESSAGE(WM_EXITMENULOOP, OnExitMenuLoop)	
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSwitchPanel message handlers
BOOL CSwitchPanel::OnInitDialog() 
{
	CTransparentDialog::OnInitDialog();
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, "");
	m_nSkinColor       = wkp.GetInt(SETTINGS_SECTION, ICP_SKINCOLOR        , m_nSkinColor);
	m_nButtonShape     = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_SHAPE     , m_nButtonShape);
	int nCheckPerformance = wkp.GetInt(SETTINGS_SECTION, _T("CheckPerformance"), -1);
	if (nCheckPerformance == 1)
	{
		CICPI7000Thread::CreatePerformanceMonitor();
	}
	else if (nCheckPerformance == -1)
	{
		wkp.WriteInt(SETTINGS_SECTION, _T("CheckPerformance"), 0);
	}

	CRect rcButton;

	GetWindowRect(&m_rcDlg);
	m_cFirstRelais.GetWindowRect(&rcButton);
	m_nEdgeX = m_rcDlg.right  - rcButton.right;
	m_nEdgeY = m_rcDlg.bottom - rcButton.bottom;
	m_ilRelaisBtn.Create(IDB_RELAIS_BTN, 16, 0, SKIN_COLOR_KEY);

	SetWindowText(WK_APP_NAME_ICPCON_CLIENT);

	InitializeCriticalSection(&m_cs);
	CreateAllButtons();

	SetDefID(0);

	m_nMinimizeTimer = SetTimer(MINIMIZE_TIMER, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnBtnConfig() 
{
	int nMinimizeTimer = m_nMinimizeTimer;

	m_nMinimizeTimer   = 0;
	SAFE_KILL_TIMER(m_nPollTimer);
	CIPCCONClientDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
		wkp.DeleteEntry(SETTINGS_SECTION, ICP_WINDOWRECT);
		DestroyControlThreads();
		DeleteAllButtons();
		CreateAllButtons();
		InvalidateRect(NULL);
	}
	else
	{
		if (m_bPollState)
		{
			m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
		}
	}
	m_nMinimizeTimer = nMinimizeTimer;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnCheck(UINT nID) 
{
#ifdef _DEBUG
	CThreadDebug td(_T("OnCheck"));
#endif

	CWnd *pWnd = GetDlgItem(nID);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
	{
		CSkinButton *pSkinBtn = (CSkinButton*)pWnd;
		int nCount = m_ThreadList.GetCount();
		if (nCount)
		{
			POSITION pos = m_ThreadList.GetHeadPosition();
			CICPI7000Module *pModule = (CICPI7000Module*) ::GetWindowLong(pWnd->m_hWnd, GWL_USERDATA);
			while (pos)
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
				if (pThread->ContainsModule((void*)pModule))
				{
#ifdef _DEBUG
					if (pModule->HasOutputs())
					{
						Lock();
						pModule->SetDObit(nID - pModule->m_nStartID, pSkinBtn->GetCheck());
						pModule->SetDOstateChanged(true);
						Unlock();
						PostMessage(WM_TIMER, m_nPollTimer);
					}
#else
					pThread->PostThreadMessage(WM_CHECK_SKINBUTTON, nID, (LPARAM)pWnd);
#endif
					break;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::DestroyControlThreads()
{
	int nCount = m_ThreadList.GetCount();
	if (nCount)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			pThread->StopThread();
			delete pThread;
		}
		m_ThreadList.RemoveAll();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnDestroy() 
{
	CRect rc;
	GetWindowRect(&rc);
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	CString str;
	if (!rc.IsRectEmpty())
	{
		str.Format(_T("%d %d %d %d"), rc.left, rc.top, rc.right, rc.bottom);
		wkp.WriteString(SETTINGS_SECTION, ICP_WINDOWRECT, str);
	}
	else
	{
		wkp.DeleteEntry(SETTINGS_SECTION, ICP_WINDOWRECT);
	}
	DestroyControlThreads();
	DeleteCriticalSection(&m_cs);
	DeleteAllButtons();
	
	CTransparentDialog::OnDestroy();

	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		   = m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	if (m_hAccelerator)
	{
		DestroyAcceleratorTable(m_hAccelerator);
		m_hAccelerator = NULL;
	}

}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::DeleteAllButtons()
{
	int nID;
	HWND hwnd;
	SAFE_KILL_TIMER(m_nPollTimer);
	for (nID=IDC_RELAIS; nID<=IDC_RELAIS_LAST; nID++)
	{
		hwnd = ::GetDlgItem(m_hWnd, nID);
		if (hwnd)
		{
			CICPI7000Module*pModule = (CICPI7000Module*)::GetWindowLong(hwnd, GWL_USERDATA);
			if (pModule)
			{
				pModule->Release();
			}
			if (nID == IDC_RELAIS)
			{
				::SetWindowLong(hwnd, GWL_USERDATA, 0);
			}
			else
			{
				delete CWnd::FromHandle(hwnd);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
int CSwitchPanel::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	m_hAccelerator = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR)); 	
	if (CTransparentDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	ShowWindow(SW_SHOW);

	NOTIFYICONDATA tnd;

	CString sTip = theApp.m_pszAppName;

	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd   = m_hWnd;
	tnd.uID    = 1;
	tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon = m_hIcon;

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSwitchPanel::PreTranslateMessage(MSG* pMsg) 
{
	if (m_hAccelerator)
	{
		if (TranslateAccelerator(m_hWnd, m_hAccelerator, pMsg))
		{
			return TRUE;
		}
	}
		
	return CTransparentDialog::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnCancel()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnOK() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::CreateAllButtons()
{
	DWORD dwPort, dwAddress, dwIOChannel, dwType, dwCount, dwValue;
	WORD  wPort, wAddress;
	const int nValueNameSize = 64;
	char  szValueName[nValueNameSize];
	CString strText, strKey, strPorts, strAddress, strDIO, strInOut, strItemID;
	HKEY  hPortKey = NULL, hSecKey = NULL, hAdressKey = NULL, hDIOkey = NULL, hIOkey;
	LONG  lResult;
	DWORD dwStyle;
	CRect rcButton, rcDlg;
	UINT  nIDButton;
	CFont *pFont = NULL;
	CICPI7000Module *pModule=NULL;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	int nSizeX   = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DX, 0),
	    nSizeY   = wkp.GetInt(SETTINGS_SECTION, ICP_BUTTON_DY, 0),
	    nOutputs = wkp.GetInt(SETTINGS_SECTION, ICP_OUTPUTS, 0),
		 nInputs  = wkp.GetInt(SETTINGS_SECTION, ICP_INPUTS , 0),
       nX       = 0,
		 nRight   = 0,
		 nButtons, nMaxX, nLeft;

	m_nPollTime     = wkp.GetInt(SETTINGS_SECTION, ICP_POLLTIME     , m_nPollTime);
	m_nTimeout      = wkp.GetInt(SETTINGS_SECTION, ICP_TIMEOUT      , m_nTimeout);
	m_nTimeoutEdge  = wkp.GetInt(SETTINGS_SECTION, ICP_TIMEOUT_EDGE , m_nTimeoutEdge);
	m_nMinimizeTime = wkp.GetInt(SETTINGS_SECTION, ICP_MINIMIZE_TIME, m_nMinimizeTime);
	m_bPollState    = FALSE;

	nButtons = nOutputs + nInputs;
	if (nButtons == 0)
	{
		return;
	}
	if (nButtons > 48)
	{
		nMaxX = 9;
	}
	else if (nButtons > 35)
	{
		nMaxX = 8;
	}
	else if (nButtons > 24)
	{
		nMaxX = 7;
	}
	else if (nButtons > 15)
	{
		nMaxX = 6;
	}
	else if (nButtons > 12)
	{
		nMaxX = 5;
	}
	else
	{
		nMaxX = 4;
	}

	if (m_nSkinColor == 1)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREEN);
		m_cFirstRelais.SetBaseColor(SKIN_COLOR_GREEN_SHADOW);
	}
	else if (m_nSkinColor == 2)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_BLUE);
		m_cFirstRelais.SetBaseColor(SKIN_COLOR_BLUE_SHADOW);
	}
	else if (m_nSkinColor == 3)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREY_MEDIUM_DARK);
		m_cFirstRelais.SetBaseColor(SKIN_COLOR_GREY_DARK);
	}
	else if (m_nSkinColor == 4)
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GREY_LIGHT);
		m_cFirstRelais.SetBaseColor(SKIN_COLOR_GREY_MEDIUM_LIGHT);
	}
	else
	{
		CreateTransparent(StyleBackGroundColorChangeBrushed, SKIN_COLOR_GOLD_METALLIC_LIGHT);
		m_cFirstRelais.SetBaseColor(SKIN_COLOR_GOLD_METALLIC);
	}
	if (m_nButtonShape == 0) m_cFirstRelais.SetShape(ShapeRect);
	else                     m_cFirstRelais.SetShape(ShapeRound);
	
	m_cFirstRelais.SetStyle(StyleCheck);
	m_cFirstRelais.SetSurface(SurfaceColorChangeBrushed);
	m_cFirstRelais.SetImageList(&m_ilRelaisBtn);
	m_cFirstRelais.EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_TOP);

	m_cFirstRelais.SetTextColorHighlighted(SKIN_COLOR_DARK_RED);
	m_cFirstRelais.SetTextAllignment(DT_WORDBREAK|DT_CENTER|DT_VCENTER);
	
	GetWindowRect(&rcDlg);
	rcDlg.right  = rcDlg.left + m_rcDlg.Width();
	rcDlg.bottom = rcDlg.top  + m_rcDlg.Height();
	m_cFirstRelais.GetWindowRect(&rcButton);

	ScreenToClient(&rcButton);
	if (nSizeX)
	{
		rcButton.right  = rcButton.left + nSizeX;
		rcButton.bottom = rcButton.top  + nSizeY;
		m_cFirstRelais.MoveWindow(&rcButton);
	}
	else
	{
		nSizeX = rcButton.Width();
		wkp.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DX, nSizeX);
		nSizeY = rcButton.Height();
		wkp.WriteInt(SETTINGS_SECTION, ICP_BUTTON_DY, nSizeY);
	}
	nLeft = rcButton.left;
	pFont = m_cFirstRelais.GetFont();
	nIDButton = m_cFirstRelais.GetDlgCtrlID();
	dwStyle   = ::GetWindowLong(m_cFirstRelais.m_hWnd, GWL_STYLE);

	strKey.Format(_T("Software\\%s"), theApp.m_sRegistryLocationPorts);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		for (dwPort=0; lResult==ERROR_SUCCESS; dwPort++)									// Enum the Ports
		{
			lResult = RegEnumKey(hSecKey, dwPort, (LPTSTR)szValueName, nValueNameSize-1);
			if (lResult == ERROR_SUCCESS)
			{
				wPort = atoi(&szValueName[3]);
				strPorts = strKey + _T("\\") + szValueName;
				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strPorts, 0, KEY_READ, &hPortKey);
				if (lResult != ERROR_SUCCESS) continue;
				dwCount = sizeof(DWORD);
				lResult = RegQueryValueEx(hPortKey, _T(""), NULL, &dwType, (BYTE*)&dwValue, &dwCount);
				if ((lResult == ERROR_SUCCESS) && (dwType == REG_DWORD) && (dwValue == 1))
				{
					BOOL bMultipleAccess = TRUE;
					RegQueryValueEx(hPortKey, ICP_MULTIPLE_ACCESS, NULL, &dwType, (BYTE*)&bMultipleAccess, &dwCount);

					for (dwAddress=0; lResult==ERROR_SUCCESS; dwAddress++)			// Enum the Addresses
					{
						pModule = NULL;
						lResult = RegEnumKey(hPortKey, dwAddress, (LPTSTR)szValueName, nValueNameSize-1);
						if (lResult == ERROR_SUCCESS)
						{
							char *pStopString;
							wAddress = (WORD)strtoul(szValueName, &pStopString, 16);
							strAddress = strPorts + _T("\\") + szValueName;
							lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strAddress, 0, KEY_READ, &hAdressKey);
							if (lResult != ERROR_SUCCESS) continue;
							dwCount = sizeof(DWORD);
							lResult = RegQueryValueEx(hAdressKey, _T(""), NULL, &dwType, (BYTE*)&dwValue, &dwCount);
							
							if ((lResult == ERROR_SUCCESS) && (dwType == REG_DWORD) && (dwValue == 1))
							{
								int nSize = strAddress.GetLength();
								for (int nInOut=0; nInOut<2; nInOut++)
								{
									if (nInOut==0)
									{
										strInOut = _T("Outputs");
									}
									else
									{
										strInOut = _T("Inputs");
									}
									strAddress = strAddress.Left(nSize) + _T("\\") + strInOut;
									lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strAddress, 0, KEY_READ, &hIOkey);
									BOOL bPollState      = FALSE;
									RegQueryValueEx(hIOkey, ICP_POLL_STATE     , NULL, &dwType, (BYTE*)&bPollState     , &dwCount);
									if (bPollState) m_bPollState = TRUE;
									if (lResult == ERROR_SUCCESS)
									{
										for (dwIOChannel=0; lResult==ERROR_SUCCESS; dwIOChannel++)// Enum the IO channels
										{
											lResult = RegEnumKey(hIOkey, dwIOChannel, (LPTSTR)szValueName, nValueNameSize-1);
											if (lResult == ERROR_SUCCESS)
											{
												strDIO = strAddress + _T("\\") + szValueName;
												lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strDIO, 0, KEY_READ, &hDIOkey);
												if (lResult != ERROR_SUCCESS) continue;
												dwCount = sizeof(DWORD);
												lResult = RegQueryValueEx(hDIOkey, _T(""), NULL, &dwType, (BYTE*)&dwValue, &dwCount);
												if ((lResult == ERROR_SUCCESS) && (dwType == REG_DWORD) && (dwValue == 1))
												{
													CIPCCONClient::ReadRegistryString(hDIOkey, ICP_NAME, strText);
													if (pModule == NULL)
													{
														WORD  wRet=NoError;
														int   i=0;
														CIPCCONClient::ReadRegistryString(hAdressKey, ICP_ITEMID, strItemID);
														dwCount = sizeof(DWORD);
														pModule = new CICPI7000Module();
														pModule->SetComPort(wPort);
														pModule->SetModuleAddress(wAddress);
														pModule->SetModuleID(strItemID);
														pModule->SetTimeOut(m_nTimeout);
														if (bPollState)
														{
															if (nInOut == 0) pModule->SetPollDO(true);
															else             pModule->SetPollDI(true);
														}
														pModule->m_nStartID = nIDButton;
														SetPortThread(wPort, pModule, bMultipleAccess?true:false);
														lResult = RegQueryValueEx(hAdressKey, ICP_BAUDRATE, NULL, &dwType, (BYTE*)&dwValue, &dwCount);
														if (lResult == ERROR_SUCCESS) pModule->SetBaudrate(dwValue);
													}

													dwCount = sizeof(DWORD);
													lResult = RegQueryValueEx(hDIOkey, ICP_CONTROL, NULL, &dwType, (BYTE*)&dwValue, &dwCount);
													pModule->SetDOlevel(nIDButton - pModule->m_nStartID, dwValue);
													if (nIDButton == IDC_RELAIS)
													{
														long lOld = SetWindowLong(m_cFirstRelais.m_hWnd, GWL_USERDATA, (long)pModule);
														ASSERT(lOld == 0);
														pModule->AddRef();
														m_cFirstRelais.SetWindowText(strText);
													}
													else
													{
														CSkinButton *pButton = new CSkinButton;
														if (++nX >= nMaxX)
														{
															nRight = rcButton.right;
															rcButton.left  = nLeft;
															rcButton.right = nLeft + nSizeX;
															rcButton.OffsetRect(0, nSizeY+5);
															nX = 0;
														}
														else
														{
															rcButton.OffsetRect(nSizeX+5, 0);
														}
														pButton->Create(strText, dwStyle, rcButton, this, nIDButton);
														pButton->SetImageList(&m_ilRelaisBtn);
														pButton->EnableImageAndText(DT_IMAGE_AND_TEXT|DT_RIGHT|DT_TOP);
														long lOld = SetWindowLong(pButton->m_hWnd, GWL_USERDATA, (long)pModule);
														ASSERT(lOld == 0);
														pModule->AddRef();
														pButton->CopyProperties(m_cFirstRelais);
														pButton->SetWindowText(strText);
														pButton->SetFont(pFont);
													}
													nIDButton++;
												}
												REG_CLOSE_KEY(hDIOkey);
												lResult = ERROR_SUCCESS;
											}
										};												// End Enum IO channels
									}
									REG_CLOSE_KEY(hIOkey);
								}
							}
							REG_CLOSE_KEY(hAdressKey);
							lResult = ERROR_SUCCESS;
						}
					};																// End Enum Addresses
				}
				REG_CLOSE_KEY(hPortKey);
				lResult = ERROR_SUCCESS;
			}
		};																			// End Enum Ports
	}
	REG_CLOSE_KEY(hSecKey);

	if (!nRight)																// rechte Seite noch nicht ermittelt ?
	{
		nRight = rcButton.right;
	}
	if (pFont)																	// Font darf nicht gelöscht werden
	{
		pFont->Detach();
	}

	rcButton.right   = nRight + m_nEdgeX;								// Dialoggröße berechnen
	rcButton.bottom += m_nEdgeY;
	ClientToScreen(&rcButton);
	rcDlg.right  = rcButton.right;
	rcDlg.bottom = rcButton.bottom;

	CString str = wkp.GetString(SETTINGS_SECTION, ICP_WINDOWRECT, NULL);
	if (!str.IsEmpty())
	{
		sscanf(str, _T("%d %d %d %d"), &rcDlg.left, &rcDlg.top, &rcDlg.right, &rcDlg.bottom);
	}
	MoveWindow(&rcDlg);

	ASSERT(m_nPollTimer == 0);
	if (m_bPollState)
	{
//		SendMessage(WM_TIMER, m_nPollTimer, 0);
		m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
	}
	else
	{
		PostMessage(WM_TIMER, m_nPollTimer, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nPollTimer)
	{
#ifdef _DEBUG
		CThreadDebug td(_T("OnPollTimer"));
#endif
		int nCount = m_ThreadList.GetCount();
		if (nCount)
		{
			nCount = 0;
			bool bFirst = true;
			POSITION pos = m_ThreadList.GetHeadPosition();
			while (pos)															// Threads prüfen
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
				BOOL bPollState = pThread->IncrementPollState();	// Pollt er zu lange ?
				if (bPollState == 3)
				{
					POSITION posM = pThread->m_Modules.GetHeadPosition();
					CWnd *pWnd;
					CICPI7000Module * pModule;								// Module des Threads
					while (posM)
					{
						pModule = (CICPI7000Module*)pThread->m_Modules.GetNext(posM);
						for (int i=0; i<pModule->GetInputs(); i++)
						{
							pWnd = GetDlgItem(pModule->m_nStartID + i);
							if (pWnd)
							{
								pWnd->EnableWindow(FALSE);					// disablen
							}
						}
					}										// da dieser Thread die Nachricht nicht weiter gibt
					m_bThreadsArePolling = FALSE; // die nächsten Threads starten
				}
			}

			if (m_bThreadsArePolling)										// Kommt die Timernachricht zu oft
			{
				m_bThreadsArePolling++;
				if (m_bThreadsArePolling > 10)
				{
					m_bThreadsArePolling = FALSE;
				}
				WK_TRACE(_T("ThreadsArePolling %d\n"), m_bThreadsArePolling);
				return;															// nix tun
			}
			
			pos = m_ThreadList.GetHeadPosition();						// Pollvorgang für die Threads starten
			while (pos)
			{
				CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
				if (pThread->HasFailed() || pThread->IsPolling())	// Portfehler ?
				{
					continue;													// nächsten Thread nehmen
				}
				else if (m_nPollTimer == 0)								// TimerID = 0
				{
#ifdef _DEBUG
						WK_TRACE(_T("PostThreadMessage(%d)\n"), pThread->GetPort());
#endif
					m_bThreadsArePolling = TRUE;							// alle Threads pollen
					pThread->PostThreadMessage(WM_TIMER, nIDEvent, MAKELONG(nCount, 1));
					break;
				}
				else																// TimerID != 0
				{
					if (pThread->DoPoll())									// nur zu pollende Threads pollen
					{
#ifdef _DEBUG
						WK_TRACE(_T("PostThreadMessage(%d)\n"), pThread->GetPort());
#endif
						m_bThreadsArePolling = TRUE;
						pThread->PostThreadMessage(WM_TIMER, nIDEvent, MAKELONG(nCount, 0));
						break;
					}
				}
				nCount++;
			}
		}
	}
	else if (nIDEvent == m_nMinimizeTimer)
	{
		/*
		CPoint pt;
		CRect  rc;
		GetWindowRect(&rc);
		GetCursorPos(&pt);
		if (rc.PtInRect(pt))
		{
			m_nMinimizeCounter = 0;
		}
		else */
		if (!theApp.m_bIsInMenuLoop)
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		}
		if (m_nMinimizeCounter++ > m_nMinimizeTime)
		{
			PostMessage(WM_USER, USER_MSG_MINIMIZE);
		}
	}
	
	CTransparentDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnUserMessage(WPARAM wParam,  LPARAM lParam)
{
	if (wParam == USER_MSG_RESTORE)
	{
		if (m_pMiniDlg)
		{
			m_pMiniDlg->ShowWindow(SW_HIDE);
			ShowWindow(SW_SHOW);
			m_nMinimizeTimer   = SetTimer(MINIMIZE_TIMER, 1000, NULL);
			m_nMinimizeCounter = 0;

			OnTimer(m_nPollTimer);

			if (m_bPollState)
			{
				m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
			}
		}
	}
	else if (wParam == USER_MSG_MINIMIZE)
	{
		if (!m_pMiniDlg)
		{
			m_pMiniDlg = new CMinimizedDlg();
			m_pMiniDlg->Create(IDD_MINIMIZED_DLG, this);
		}
		if (!m_pMiniDlg->IsWindowVisible())
		{
			m_pMiniDlg->ShowWindow(SW_SHOW);
			ShowWindow(SW_HIDE);
			SAFE_KILL_TIMER(m_nMinimizeTimer);
			SAFE_KILL_TIMER(m_nPollTimer);
		}
	}
	else if (wParam == USER_MSG_KILLTIMER)
	{
		if ((UINT)lParam == m_nPollTimer)
		{
			SAFE_KILL_TIMER(m_nPollTimer);
		}
		else if ((UINT)lParam == m_nMinimizeTimer)
		{
			SAFE_KILL_TIMER(m_nMinimizeTimer);
		}
	}
	else if (wParam == USER_MSG_POLL_THREADS_READY)
	{
#ifdef _DEBUG
		WK_TRACE(_T("USER_MSG_POLL_THREADS_READY\n"));
#endif
		m_bThreadsArePolling = FALSE;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnBtnCancel() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CTransparentDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		SetFocus();
		CRect rc;
		DWORD dwFlags = SWP_NOSIZE | SWP_NOMOVE;
		GetWindowRect(&rc);
		if ((rc.left < 50) && (rc.top < 50))
		{
			HMONITOR hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			if (hMon)
			{
				MONITORINFO mi;
				mi.cbSize = sizeof(MONITORINFO);
				if (GetMonitorInfo(hMon, &mi))
				{
					rc.left = 150;
					rc.top  = 150;
					dwFlags &= ~SWP_NOMOVE;
				}
			}
		}
		::SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, 0, 0, dwFlags);
	}
	else
	{
		m_bThreadsArePolling = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnCheckSkinBtn(WPARAM wParam,  LPARAM lParam)
{
	CWnd *pWnd = GetDlgItem(wParam);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSkinButton)))
	{
		CSkinButton *pSkinBtn = (CSkinButton*)pWnd;
		if (lParam == -1)
		{
			pSkinBtn->EnableWindow(FALSE);
		}
		else
		{
			pSkinBtn->EnableWindow(TRUE);
			pSkinBtn->SetCheck(lParam);
		}
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
bool CSwitchPanel::SetPortThread(WORD wPort, void *pModule, bool bMultipleAccess)
{
	int nCount = m_ThreadList.GetCount();
	if (nCount)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		while (pos)
		{
			CICPI7000Thread *pThread = (CICPI7000Thread*) m_ThreadList.GetNext(pos);
			if (pThread->GetPort() == (int) wPort)
			{
				pThread->AddModule(pModule);
				return true;
			}
		}
	}
	CICPI7000Thread*pNewThread = new CICPI7000Thread((int) wPort, pModule, this, bMultipleAccess);
	m_ThreadList.AddHead(pNewThread);
	pNewThread->CreateThread();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::Lock()
{
	EnterCriticalSection(&m_cs);
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::Unlock()
{
	LeaveCriticalSection(&m_cs);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;
			int    nEnable = MF_BYCOMMAND;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			if (m_bPollState) nEnable |= MF_CHECKED;
			else              nEnable |= MF_UNCHECKED;

			pM->CheckMenuItem(IDC_BTN_POLL_PORT, nEnable);
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnBtnPollPort() 
{
	if (m_bPollState)
	{
		SAFE_KILL_TIMER(m_nPollTimer);
		m_bPollState = FALSE;
	}
	else
	{
		if (m_nPollTimer == 0)
		{
			m_nPollTimer = SetTimer(POLL_TIMER, m_nPollTime, NULL);
		}
		m_bPollState = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
long CSwitchPanel::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_ICPCON_CLIENT, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSwitchPanel::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
LRESULT CSwitchPanel::OnEnterMenuLoop(WPARAM, LPARAM)
{
	theApp.m_bIsInMenuLoop = TRUE;
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT  CSwitchPanel::OnExitMenuLoop(WPARAM, LPARAM)
{
	theApp.m_bIsInMenuLoop = FALSE;
	return 0;
}

