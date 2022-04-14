// QUnitDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "User.h"
#include "Permission.h"
#include <Skins/Skins.h> // Skins before OemGui!
#include "oemgui\oemlogindialog.h"
#include "oemgui\oemguiapi.h"
#include "CipcInputQUnit.h"
#include "CipcInputQUnitMDAlarm.h"
#include "CipcOutputQUnit.h"

#include "QUnit.h"
#include "QUnitDlg.h"
#include "VideoInlayWindow.h"
#include "CMotionDetection.h"
#include "CUDP.h"
#include "CMDPoints.h"
#include ".\qunitdlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CQUnitApp theApp;

// CAboutDlg-Dialogfeld für Anwendungsbefehl 'Info'

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQUnitDlg Dialogfeld



CQUnitDlg::CQUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQUnitDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pUDP				= NULL;
	m_pVideoInlayWindow = NULL;
	m_nActiveCamera		= 0;
	m_sIniFile			= _T("QUnit");
	m_bStandalone		= TRUE;
	m_pInput			= NULL;
	m_pInputMDAlarm		= NULL;
	m_pInputMDAlarm		= NULL;
	m_pOutputCamera		= NULL;
	m_pOutputRelay		= NULL;
	m_bMulti			= FALSE;
	m_bManualFramerates = FALSE;
	m_nDeinterlace		= FALSE;

	//m_bStandalone = 0-> SaVicUnit arbeitet mit Security, 1-> SaVicUnit ist Standalone
	m_bStandalone = ReadConfigurationInt(_T("Debug"), _T("Mode"),  0, m_sIniFile);
	if (m_bStandalone)
		WK_TRACE(_T("Running as 'standalone'\n"));   
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQUnitDlg)
	DDX_Control(pDX, IDC_CHECK_RELAY0, m_ctrlRelay0);
	DDX_Control(pDX, IDC_CHECK_ALARM0, m_ctrlAlarm0);
	DDX_Control(pDX, IDC_CAMERA0, m_ctrlCamera0);
	DDX_Control(pDX, IDC_COMBO_FPS0, m_ctrlComboFps0);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_ctrlBrightness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_ctrlContrast);
	DDX_Control(pDX, IDC_SLIDER_SATURATION, m_ctrlSaturation);
	DDX_Control(pDX, IDC_SLIDER_HUE, m_ctrlHue);
	DDX_Radio(pDX, IDC_RD_DEINT0, m_nDeinterlace);
	//}}AFX_DATA_MAP
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CQUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CQUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
	ON_MESSAGE(WM_UPDATE_DIALOG,OnUpdateDialog)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_RELAY0, IDC_CHECK_RELAY31, OnRelay)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CAMERA0, IDC_CAMERA63, OnCamera)
	ON_CONTROL_RANGE(CBN_SELENDOK, IDC_COMBO_FPS0, IDC_COMBO_FPS63, OnSelectFps)
	ON_BN_CLICKED(IDC_PREVIEW, OnBnClickedPreview)
	ON_BN_CLICKED(IDC_DLG_MD_CONFIG, OnDlgMdConfig)
	ON_CBN_SELENDOK(IDC_COMBO_RESOLUTION, OnSelectResolution)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)
	ON_COMMAND(ID_FILE_ABOUT, OnFileAbout)
	ON_COMMAND(ID_FILE_SETTINGS, OnFileSettings)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_BN_CLICKED(IDC_CHECK_MULTI, OnBnClickedCheckMulti)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_FREE_FPS, OnBnClickedFreeFps)
	ON_BN_CLICKED(IDC_RD_DEINT0, OnBnClickedRdDeint)
	ON_BN_CLICKED(IDC_RD_DEINT1, OnBnClickedRdDeint)
	ON_BN_CLICKED(IDC_RD_DEINT2, OnBnClickedRdDeint)
	ON_BN_CLICKED(IDC_RD_DEINT3, OnBnClickedRdDeint)
	ON_BN_CLICKED(IDC_RD_DEINT4, OnBnClickedRdDeint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
long CQUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM /*lParam*/)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_QUNIT, 0);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
int CQUnitDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nResult = -1;
	if (CDialog::OnCreate(lpCreateStruct)==0)
	{
		m_pUDP = new CUDP(this, m_sIniFile, CMN5_VIDEO_FORMAT_PAL_B, 720, 288);
		if (m_pUDP)
		{
			if (m_pUDP->IsValid())
			{
				//Sleep(2000);
				if (!m_bStandalone)			
					CreateCIPCConnection();
				nResult = 0;
			}
		}
	}

	if (!m_bStandalone)
	{
		// Icon ins Systemtray
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;

		tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYCLICKED;
		tnd.hIcon		= m_hIcon;

		CString sTip = COemGuiApi::GetApplicationName(WAI_QUNIT);

		lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
		Shell_NotifyIcon(NIM_ADD, &tnd);
		ShowWindow(SW_HIDE);
	}

	return nResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::CreateCIPCConnection()
{
	BOOL bResult = FALSE;

	CString sSMName;

	// die CIPC-Schnittstellen aufbauen.
	m_pInput = new CIPCInputQUnit(this, m_pUDP, SM_Q_INPUT);
	if (!m_pInput)
		WK_TRACE_ERROR(_T("CQUnitDlg::Create\tCIPCInputQUnit-Objekt konnte nicht angelegt werden.\n"));

	m_pInputMDAlarm = new CIPCInputQUnitMDAlarm(this, m_pUDP, SM_Q_MD_INPUT);
	if (!m_pInputMDAlarm)
		WK_TRACE_ERROR(_T("CQUnitDlg::Create\tCIPCInputQUnitMDAlarm-Objekt konnte nicht angelegt werden.\n"));
	
	m_pOutputCamera = new CIPCOutputQUnit(this, m_pUDP, OUTPUT_TYPE_CAMERA, SM_Q_OUTPUT_CAMERA, m_sIniFile);
	if (!m_pOutputCamera)
		WK_TRACE_ERROR(_T("CQUnitDlg::Create\tCIPCOutputQUnit-QCamera-Objekt konnte nicht angelegt werden.\n"));
	
	m_pOutputRelay = new CIPCOutputQUnit(this, m_pUDP, OUTPUT_TYPE_RELAY, SM_Q_OUTPUT_RELAY, m_sIniFile);
	if (!m_pOutputRelay)
		WK_TRACE_ERROR(_T("CQUnitDlg::Create\tCIPCOutputQUnit-Relay-Objekt konnte nicht angelegt werden.\n"));	

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(AfxGetAppName());

	// Camerabuttons anlegen
	DWORD dwStyle = ::GetWindowLong(m_ctrlCamera0.m_hWnd, GWL_STYLE);
	CFont* pFont = m_ctrlCamera0.GetFont();
	CRect rect;
	m_ctrlCamera0.GetWindowRect(&rect);
	ScreenToClient(&rect);
	CRect rectBack(rect);

	__int64 i64CamMask = 0;
	if (m_pUDP)
		i64CamMask = m_pUDP->ScanForCameras();

	CString s;
	int nID = m_ctrlCamera0.GetDlgCtrlID();
	m_ctrlCamera0.EnableWindow(TSTBIT(i64CamMask, 0));

	for (int nI = 1; nI < m_pUDP->GetAvailableCameras(); nI++)
	{
		CButton*  pWnd = new CButton;
		rect.OffsetRect(rect.Width()+0, 0);
		if ((nI % 16) == 0)
		{
			rect = rectBack;
			rect.OffsetRect(0, (nI/16)*(rect.Height()+28));
		}

		s.Format(_T("%d"), nI+1);
		pWnd->Create(s, dwStyle, rect, this, nID+nI);
		pWnd->SetFont(pFont);
		pWnd->EnableWindow(TSTBIT(i64CamMask, nI));
	}	

	// Fps Comboboxen anlegen
	dwStyle = ::GetWindowLong(m_ctrlComboFps0.m_hWnd, GWL_STYLE);
	pFont = m_ctrlComboFps0.GetFont();
	m_ctrlComboFps0.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rectBack = rect;

	// Größe der ausgeklappten Listbox holen.
	CRect rcCtrl;
	m_ctrlComboFps0.GetDroppedControlRect(&rcCtrl);
	ScreenToClient(&rcCtrl);

	nID = m_ctrlComboFps0.GetDlgCtrlID();
	m_ctrlComboFps0.EnableWindow(TSTBIT(i64CamMask, 0));
	for (int nI = 1; nI < m_pUDP->GetAvailableCameras(); nI++)
	{
		CComboBox*  pWnd = new CComboBox;
		rect.OffsetRect(rect.Width()+0, 0);
		if ((nI % 16) == 0)
		{
			rect = rectBack;
			rect.OffsetRect(0, (nI/16)*(rect.Height()+24));
		}
		
		// Combobox anlegen
		pWnd->Create(dwStyle, rect, this, nID+nI);

		// Texteinträge kopieren
		CString sText;
		for (int nIndex = 0; nIndex < m_ctrlComboFps0.GetCount(); nIndex++)
		{
			m_ctrlComboFps0.GetLBText(nIndex, sText);
			pWnd->AddString(sText);
		}
		pWnd->SetFont(pFont);
		pWnd->EnableWindow(TSTBIT(i64CamMask, nI));

		// Listboxgröße anpassen
		COMBOBOXINFO cbi;
		ZeroMemory(&cbi, sizeof(COMBOBOXINFO));
		cbi.cbSize = sizeof(COMBOBOXINFO);
		pWnd->GetComboBoxInfo(&cbi);
		::SetWindowPos(cbi.hwndList, HWND_TOP, 0,0,0,rcCtrl.bottom, SWP_NOZORDER|SWP_NOMOVE);

		pWnd->EnableWindow(TSTBIT(i64CamMask, nI));
	}	

	// Relaybuttons anlegen
	dwStyle = ::GetWindowLong(m_ctrlRelay0.m_hWnd, GWL_STYLE);
	m_ctrlRelay0.GetWindowRect(&rect);
	ScreenToClient(&rect);
	nID = m_ctrlRelay0.GetDlgCtrlID();
	for (nI = 1; nI < m_pUDP->GetAvailableRelays(); nI++)
	{
		CButton*  pWnd = new CButton;
		rect.OffsetRect(rect.Width()+1, 0);
		pWnd->Create(_T(""), dwStyle, rect, this, nID+nI);
	}	

	// Alarmbuttons anlegen
	dwStyle = ::GetWindowLong(m_ctrlAlarm0.m_hWnd, GWL_STYLE);
	m_ctrlAlarm0.GetWindowRect(&rect);
	ScreenToClient(&rect);
	nID = m_ctrlAlarm0.GetDlgCtrlID();
	for (nI = 1; nI < m_pUDP->GetAvailableInputs(); nI++)
	{
		CButton*  pWnd = new CButton;
		rect.OffsetRect(rect.Width()+1, 0);
		pWnd->Create(_T(""), dwStyle, rect, this, nID+nI);
	}	
	

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
//	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
//	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	m_ctrlBrightness.SetRange(0, 255);
	m_ctrlContrast.SetRange(0, 255);
	m_ctrlSaturation.SetRange(0, 255);
	m_ctrlHue.SetRange(0, 255);

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden


	// Defaulteinträge der Kameraauswahlcombox setzen
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
	CString sName;
	sName.LoadString(IDS_COMBO_QCIF);
	pCombo->InsertString(0, sName);
	sName.LoadString(IDS_COMBO_CIF);
	pCombo->InsertString(1, sName);
	sName.LoadString(IDS_COMBO_2CIF);
	pCombo->InsertString(2, sName);
	sName.LoadString(IDS_COMBO_4CIF);
	pCombo->InsertString(3, sName);
	pCombo->SetCurSel(1);

	if (m_bStandalone)
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		OnCamera(IDC_CAMERA0);
		UpdateDialog();
	}
	else
	{
		CenterWindow();
		ShowWindow(SW_SHOW);
		ShowWindow(SW_MINIMIZE);
	}
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
		OnFileAbout();
	else if ((nID == SC_MINIMIZE) && (m_bStandalone == FALSE))
		ShowWindow(SW_HIDE);
	else
		CDialog::OnSysCommand(nID, lParam);
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnPaint() 
{
	if (IsIconic())
	{
		if (m_bStandalone == FALSE)
			ShowWindow(SW_HIDE);

		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CQUnitDlg::OnTrayClicked(WPARAM /*wParam*/, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MENU1);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}
	return 0;
}
// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
////////////////////////////////////////////////////////////////////////////////////////////////////
HCURSOR CQUnitDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnDestroy()
{
	m_csPreview.Lock();
	WK_DELETE(m_pVideoInlayWindow);
	m_csPreview.Unlock();

	WK_DELETE(m_pInput);
	WK_DELETE(m_pInputMDAlarm);
	WK_DELETE(m_pOutputCamera);
	WK_DELETE(m_pOutputRelay);

	// Relay-buttons zerstören
	if (m_ctrlRelay0.m_hWnd)
	{
		int nID = m_ctrlRelay0.GetDlgCtrlID();
		for (int nI = 1; nI < m_pUDP->GetAvailableRelays(); nI++)
		{
			CWnd* pWnd = GetDlgItem(nID+nI);
			if (pWnd)
				WK_DELETE(pWnd);
		}	
	}

	// Alarm-buttons zerstören
	if (m_ctrlAlarm0.m_hWnd)
	{
		int nID = m_ctrlAlarm0.GetDlgCtrlID();
		for (int nI = 1; nI < m_pUDP->GetAvailableInputs(); nI++)
		{
			CWnd* pWnd = GetDlgItem(nID+nI);
			if (pWnd)
				WK_DELETE(pWnd);
		}	
	}

	// Kamera-buttons zerstören
	if (m_ctrlCamera0.m_hWnd)
	{
		int nID = m_ctrlCamera0.GetDlgCtrlID();
		for (int nI = 1; nI < m_pUDP->GetAvailableCameras(); nI++)
		{
			CWnd* pWnd = GetDlgItem(nID+nI);
			if (pWnd)
				WK_DELETE(pWnd);
		}	
	}

	// Fps Combobox-buttons zerstören
	if (m_ctrlComboFps0.m_hWnd)
	{
		int nID = m_ctrlComboFps0.GetDlgCtrlID();
		for (int nI = 1; nI < m_pUDP->GetAvailableCameras(); nI++)
		{
			CWnd* pWnd = GetDlgItem(nID+nI);
			if (pWnd)
				WK_DELETE(pWnd);
		}	
	}
	WK_DELETE(m_pUDP);

	if (!m_bStandalone)
	{
		// Nur erforderlich, wenn Icon in der Systemtaskleiste
		NOTIFYICONDATA tnd;

		tnd.cbSize		= sizeof(NOTIFYICONDATA);
		tnd.hWnd		= m_hWnd;
		tnd.uID			= 1;
		tnd.uFlags		= 0;
		
		Shell_NotifyIcon(NIM_DELETE, &tnd);
	}

	CDialog::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::SetActiveCamera(int nActiveCamera)
{
	PostMessage(WM_COMMAND, MAKELONG(IDC_CAMERA0+nActiveCamera, BN_CLICKED), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnRelay(UINT nID)
{
	CButton* pButton = (CButton*)GetDlgItem(nID);
	if (pButton && m_pUDP)
	{
		BOOL bCheck = pButton->GetCheck();
		DWORD dwMask = 0;

		if (m_pUDP && m_pUDP->DoRequestGetRelay(dwMask))
		{
			if (bCheck)		
				dwMask = (DWORD)SETBIT(dwMask, nID-IDC_CHECK_RELAY0);
			else
				dwMask = (DWORD)CLRBIT(dwMask, nID-IDC_CHECK_RELAY0);
			
			m_pUDP->DoRequestSetRelay(dwMask);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnSelectFps(UINT nID)
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(nID);
	if (pComboBox)
	{
		int nCamera = nID - IDC_COMBO_FPS0;

		if (m_pUDP)
		{
			int nIndex = pComboBox->GetCurSel();
			if (nIndex != CB_ERR)
			{
				CString sText;
				pComboBox->GetLBText(nIndex, sText);

				int nFps = _wtoi(sText);
				if (m_bManualFramerates)
					m_pUDP->DoRequestSetFramerate(nCamera, nFps);
				else
					m_pUDP->UpdateFramerateCalculation(nCamera, nFps);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnCamera(UINT nID)
{
	CButton* pButton = (CButton*)GetDlgItem(nID);
	if (pButton)
	{
		m_nActiveCamera = nID - IDC_CAMERA0;

		if (m_pUDP)
		{
			m_pUDP->SetActiveCamera(m_nActiveCamera);
			if (m_bStandalone)
				m_pUDP->SetCameraToAnalogOut(m_nActiveCamera);
		}

		if (m_pVideoInlayWindow)
			m_pVideoInlayWindow->SetActiveCamera(m_nActiveCamera);

		UpdateDialog();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{ 
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;

	// Get the current position of scroll box.
	int curpos = pSlider->GetPos();

	if (nSBCode != SB_ENDSCROLL)
	{
		if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_BRIGHTNESS)
		{
			if (m_pUDP)
				m_pUDP->DoRequestSetBrightness(m_nActiveCamera, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_CONTRAST)
		{
			if (m_pUDP)
				m_pUDP->DoRequestSetContrast(m_nActiveCamera, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_SATURATION)
		{
			if (m_pUDP)
				m_pUDP->DoRequestSetSaturation(m_nActiveCamera, curpos);
		}
		else if (pScrollBar->GetDlgCtrlID() == IDC_SLIDER_HUE)
		{
			if (m_pUDP)
				m_pUDP->DoRequestSetHue(m_nActiveCamera, curpos);
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnBnClickedPreview()
{
	m_csPreview.Lock();
	if (!m_pVideoInlayWindow)
		m_pVideoInlayWindow = new CVideoInlayWindow(m_pUDP, this);
	else
	{
		delete m_pVideoInlayWindow;
		m_pVideoInlayWindow = NULL;
	}
	m_csPreview.Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnCapture(QIMAGE* pQImage)
{
	BOOL bResult = FALSE;

	static DWORD dwCounter  = 0;
	static DWORD dwLastTC	= GetTickCount();

	m_csPreview.Lock();
	if (m_pVideoInlayWindow)
		bResult = m_pVideoInlayWindow->DrawImage(pQImage, m_bMulti);
	m_csPreview.Unlock();

	dwCounter++;
	CWnd* pWnd = GetDlgItem(IDC_FRAME_COUNTER);
	if (pWnd && pWnd->IsWindowVisible())
	{
		CString sText;
		sText.Format(_T("%d"), dwCounter);
		pWnd->SetWindowText(sText);
	} 

	pWnd = GetDlgItem(IDC_FRAMERATE);
	if (pWnd && pWnd->IsWindowVisible())
	{
		CString sText;
		if ((dwCounter % 100) == 0)
		{
			double fFps = (1000.0*100.0) / (double)(GetTickCount() - dwLastTC);
			dwLastTC = GetTickCount();
			sText.Format(_T("%.01f"), fFps);
			pWnd->SetWindowText(sText);
		}
	}

	// Und ab damit zur CIPC
	if (m_pOutputCamera)
		m_pOutputCamera->IndicationDataReceived(pQImage);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnReceivedMotionAlarm(int nCamera, CMDPoints& Points)
{
	if (m_pInputMDAlarm)
		m_pInputMDAlarm->OnReceivedMotionAlarm(nCamera, Points, TRUE);
 	if (m_pInputMDAlarm)
		m_pInputMDAlarm->OnReceivedMotionAlarm(nCamera, Points, FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnVideoState(int nCamera, BOOL bPresent)
{
	BOOL bResult = FALSE;

	if (m_pOutputCamera)
		m_pOutputCamera->OnIndicationVideoStateChanged(nCamera, bPresent);

	WK_TRACE(_T("CQUnitDlg::OnVideoState (Camera=%d, bPresent=%d)\n"), nCamera, bPresent);

	CButton* pButton = (CButton*)GetDlgItem(IDC_CAMERA0+nCamera);
	if (pButton)
		pButton->EnableWindow(bPresent);

	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_FPS0+nCamera);
	if (pComboBox)
		pComboBox->EnableWindow(bPresent);

	if (m_bStandalone)
	{
		if (m_pUDP)
			m_pUDP->UpdateFramerateCalculation(m_nActiveCamera, 25);
	}

	bResult = TRUE;

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnSensor(DWORD dwSensorMask)
{
	BOOL bResult = FALSE;

	if (m_pInput)
		m_pInput->AlarmStateChanged(dwSensorMask);

	if (m_pUDP)
	{
		for (int nI = 0; nI < m_pUDP->GetAvailableInputs(); nI++)
		{
			CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_ALARM0+nI);
			if (pButton)
				pButton->SetCheck(TSTBIT(dwSensorMask, nI));
		}
		bResult = TRUE;
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
 BOOL CQUnitDlg::UpdateDialog()
{
	BOOL bResult = FALSE;

	if (IsWindowVisible())
		bResult = PostMessage(WM_UPDATE_DIALOG);

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
long CQUnitDlg::OnUpdateDialog(WPARAM wParam, LPARAM lParam)
{
	if (m_pUDP)
	{
		int nValue = 0;

		if (m_pUDP->DoRequestGetBrightness(m_nActiveCamera, nValue))
			m_ctrlBrightness.SetPos(nValue);
		if (m_pUDP->DoRequestGetContrast(m_nActiveCamera, nValue))
			m_ctrlContrast.SetPos(nValue);
		if (m_pUDP->DoRequestGetSaturation(m_nActiveCamera, nValue))
			m_ctrlSaturation.SetPos(nValue);
		if (m_pUDP->DoRequestGetHue(m_nActiveCamera, nValue))
			m_ctrlHue.SetPos(nValue);
		
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);
		CSize ImageSize = m_pUDP->DoRequestGetImageSize(m_nActiveCamera);
		if (ImageSize.cx == 180 && ImageSize.cy == 144)
			pCombo->SetCurSel(0);
		else if (ImageSize.cx == 360 && ImageSize.cy == 288)
			pCombo->SetCurSel(1);
		else if (ImageSize.cx == 704 && ImageSize.cy == 288)
			pCombo->SetCurSel(2);
		else if (ImageSize.cx == 704 && ImageSize.cy == 576)
			pCombo->SetCurSel(3);
		else
			WK_TRACE_WARNING(_T("OnUpdateDialog unknown Imagesize (%dx%d)\n"), ImageSize.cx, ImageSize.cy);
	}

	CheckRadioButton(IDC_CAMERA0, IDC_CAMERA63, IDC_CAMERA0+m_nActiveCamera);

	if (m_pUDP)
	{
		DWORD dwMask = 0;
		if (m_pUDP->DoRequestGetRelay(dwMask))
		{
			for (int nI = 0; nI < m_pUDP->GetAvailableRelays(); nI++)
			{
				CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK_RELAY0+nI);
				if (pButton)
					pButton->SetCheck(TSTBIT(dwMask, nI));
			}
		}
	}

	if (m_pUDP)
	{
		for (int nCamera = 0; nCamera < m_pUDP->GetAvailableCameras(); nCamera++)
		{
			int nFps = 0;
			if (m_pUDP->DoRequestGetFramerate(nCamera, nFps))
			{
				CComboBox* pComboBox = (CComboBox*)GetDlgItem(m_ctrlComboFps0.GetDlgCtrlID()+nCamera);
				if (pComboBox)
				{
					CString sText;
					sText.Format(_T("%d"), nFps);
					int nIndex = pComboBox->FindStringExact(0, sText);
					if (nIndex != CB_ERR)
						pComboBox->SetCurSel(nIndex);
				}
			}
			
		}
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnDlgMdConfig()
{
	if (m_pUDP)
		m_pUDP->OnDlgMdConfig(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnSelectResolution() 
{
	if (m_pUDP)
	{
		CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION);

		int nSel = pCombo->GetCurSel();	
		if (nSel != CB_ERR )
		{
			switch (nSel)
			{
				case 0:
					m_pUDP->DoRequestSetImageSize(m_nActiveCamera, CSize(180, 144));
					break;
				case 1:
					m_pUDP->DoRequestSetImageSize(m_nActiveCamera, CSize(360, 288));
					break;
				case 2:
					m_pUDP->DoRequestSetImageSize(m_nActiveCamera, CSize(704, 288));
					break;
				case 3:
					m_pUDP->DoRequestSetImageSize(m_nActiveCamera, CSize(704, 576));
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnBnClickedCheckMulti()
{
	m_bMulti = !m_bMulti;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnBnClickedFreeFps()
{
	m_bManualFramerates = !m_bManualFramerates;
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnFileAbout()
{
	COemGuiApi::AboutDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnFileSettings()
{

	if (m_bStandalone || Login())
	{
		ShowWindow(SW_NORMAL);
		UpdateDialog();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnFileExit()
{
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::Login()
{
	BOOL bRet = FALSE;			 
	CString sUser;
	CString sPassword;
	CSecID  idUser;
	CUserArray UserArray;
	CWK_Profile prof;

	COEMLoginDialog dlg(this);
										   
	if (IDOK==dlg.DoModal())	  		 
		bRet = TRUE;		 

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetFramerate(int nCamera, int nFps)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetFramerate(int nCamera, int nFps)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetBrightness(int nCamera, int nValue)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetBrightness(int nCamera, int nValue)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetContrast(int nCamera, int nValue)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetContrast(int nCamera, int nValue)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetSaturation(int nCamera, int nValue)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetSaturation(int nCamera, int nValue)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetHue(int nCamera, int nValue)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetHue(int nCamera, int nValue)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetRelay(DWORD dwMask)
{
	UpdateDialog();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetRelay(DWORD dwMask)
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetAlarmEdge(DWORD dwEdgeMask)
{
	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetAlarmEdge(DWORD dwEdgeMask)
{
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetVideoState(int nCamera, BOOL bPresent)
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmSetImageSize(int nCamera, CSize ImageSize)
{
	UpdateDialog();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CQUnitDlg::OnConfirmGetImageSize(int nCamera, CSize ImageSize)
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CQUnitDlg::OnBnClickedRdDeint()
{
	CDataExchange dx(this, TRUE);
	DDX_Radio(&dx, IDC_RD_DEINT0, m_nDeinterlace);
	m_pVideoInlayWindow->SetDeinterlace(m_nDeinterlace);
}
