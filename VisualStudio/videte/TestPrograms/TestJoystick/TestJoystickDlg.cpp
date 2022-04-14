// TestJoystickDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "TestJoystick.h"
#include "TestJoystickDlg.h"


#include "winuser.h"
#include ".\testjoystickdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <cpl.h>

HMODULE	g_hJoyCpl =  NULL;
APPLET_PROC pCplApplet = NULL;
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestJoystickDlg Dialogfeld



CTestJoystickDlg::CTestJoystickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestJoystickDlg::IDD, pParent)
{
	m_pJoyStick  = NULL;
	m_pJoyState  = NULL;
	m_pJoyState2 = NULL;
	OnBnClickedBtnReset();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLearnMode = -1;
	g_hJoyCpl = ::LoadLibrary(_T("joy.cpl"));
	if (g_hJoyCpl)
	{
		pCplApplet = (APPLET_PROC)::GetProcAddress(g_hJoyCpl, "CPlApplet");
	}
}
CTestJoystickDlg::~CTestJoystickDlg()
{
	if (g_hJoyCpl)
	{
		FreeLibrary(g_hJoyCpl);
	}
}
void CTestJoystickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_Z_POS, m_SliderZ);

	DDX_Control(pDX, IDC_SLIDER_RZ_POS, m_SliderRZ);

	DDX_Control(pDX, IDC_SLIDER_1_POS, m_Slider1);
	DDX_Control(pDX, IDC_SLIDER_2_POS, m_Slider2);
	DDX_Control(pDX, IDC_EDT_DEBUGOUT, m_Output);
	DDX_Control(pDX, IDC_BTN_CHOOSE_JOYSTICK, m_btnChooseJoystick);
	DDX_Control(pDX, IDC_XY_CTRL, m_ctrlTarget);
	DDX_Control(pDX, IDC_XY_CTRLR, m_ctrlTargetR);

//	DDX_Control(pDX, IDC_XY_CTRLR, m_ctrlPOV);
}

BEGIN_MESSAGE_MAP(CTestJoystickDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CK_BTN_1, IDC_CK_BTN_8, OnCkBtn)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_RESET, OnBnClickedBtnReset)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CHOOSE_JOYSTICK, OnBnClickedBtnChooseJoystick)
END_MESSAGE_MAP()


// CTestJoystickDlg Meldungshandler

BOOL CTestJoystickDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_pJoyStick = new CJoyStickDX;
	CPoint ptRange(-13, 13);
	m_pJoyStick->SetAxisRange(ptRange.x, ptRange.y);
	m_ctrlTarget.SetHorzRange(ptRange);
	m_ctrlTarget.SetVertRange(ptRange);
	m_ctrlTarget.SetZeroRange(3);

	m_ctrlTargetR.SetHorzRange(ptRange);
	m_ctrlTargetR.SetVertRange(ptRange);
	m_ctrlTargetR.SetZeroRange(3);

	if (SUCCEEDED(m_pJoyStick->Init(m_hWnd, DISCL_EXCLUSIVE|DISCL_FOREGROUND, INITF_USE_DIJOYSTATE2)))
	{
		m_pJoyState2 = new DIJOYSTATE2;
		ZeroMemory(m_pJoyState2, sizeof(DIJOYSTATE2));
		DWORD dwCaps = m_pJoyStick->GetCapabilities();
		m_ctrlTarget.ShowWindow((dwCaps & (JOY_CAPS_XAXIS|JOY_CAPS_YAXIS)) ? SW_SHOW: SW_HIDE);
		m_SliderZ.SetRange(m_pJoyStick->GetAxisRangeMin(), m_pJoyStick->GetAxisRangeMax());
		m_SliderZ.EnableWindow(dwCaps & JOY_CAPS_ZAXIS ? TRUE : FALSE);
		OutputDebug(_T("Axis:%s, %s, %s\n") ,dwCaps & JOY_CAPS_XAXIS ? _T("X") : _T("") ,dwCaps & JOY_CAPS_YAXIS ? _T("Y") : _T("") ,dwCaps & JOY_CAPS_ZAXIS ? _T("Z") : _T(""));

		if (dwCaps &(JOY_CAPS_RXAXIS|JOY_CAPS_RYAXIS))
		{
			m_ctrlTargetR.ShowWindow(SW_SHOW);
		}
		else if (m_pJoyStick->GetNoOfPOVs())
		{
			m_ctrlPOV.Attach(m_ctrlTargetR.Detach());
			m_ctrlPOV.ShowWindow(SW_SHOW);
		}

		m_SliderRZ.SetRange(m_pJoyStick->GetAxisRangeMin(), m_pJoyStick->GetAxisRangeMax());
		m_SliderRZ.EnableWindow(dwCaps & JOY_CAPS_RZAXIS ? TRUE : FALSE);
		OutputDebug(_T("(R)Axis:%s, %s, %s\n") ,dwCaps & JOY_CAPS_RXAXIS ? _T("X") : _T("") ,dwCaps & JOY_CAPS_RYAXIS ? _T("Y") : _T("") ,dwCaps & JOY_CAPS_RZAXIS ? _T("Z") : _T(""));

		int i, n;
		for (i=IDC_CK_BTN_1, n=0; i<=IDC_CK_BTN_8; i++, n++)
		{
			GetDlgItem(i)->ShowWindow(n < m_pJoyStick->GetNoOfButtons() ? TRUE : FALSE);
		}
		OutputDebug(_T("Buttons:%d,\n") , m_pJoyStick->GetNoOfButtons());

		for (i=0; i<m_pJoyStick->GetNoOfSliders(); i++)
		{
			(&m_Slider1)[i].EnableWindow(TRUE);
			(&m_Slider1)[i].SetRange(m_pJoyStick->GetAxisRangeMin(), m_pJoyStick->GetAxisRangeMax());
		}
		OutputDebug(_T("Sliders:%d,\n") , m_pJoyStick->GetNoOfSliders());
		OutputDebug(_T("Keys:%d,\n") , m_pJoyStick->GetNoOfKeys());
		OutputDebug(_T("POV:%d,\n") , m_pJoyStick->GetNoOfPOVs());

		SetTimer(100, 50, NULL);
	}
	else
	{
		delete m_pJoyStick;
	}
	if (pCplApplet)
	{
		CPLINFO cplinfo;
		if (pCplApplet(m_hWnd, CPL_INIT, 0, 0))
		{
			int nSubPrograms = pCplApplet(m_hWnd, CPL_GETCOUNT, 0, 0);
			ZeroMemory(&cplinfo, sizeof(cplinfo));
			pCplApplet(m_hWnd, CPL_INQUIRE, 0, (LPARAM)&cplinfo);
			CString str;
			str.LoadString(g_hJoyCpl, cplinfo.idName);
			GetDlgItem(IDC_TXT_NAME)->SetWindowText(str);
			str.LoadString(g_hJoyCpl, cplinfo.idInfo);
			GetDlgItem(IDC_TXT_DESCR)->SetWindowText(str);
			HICON hIcon = ::LoadIcon(g_hJoyCpl, MAKEINTRESOURCE(cplinfo.idIcon));
			if (hIcon)
			{
				m_btnChooseJoystick.ModifyStyle(0, BS_ICON, 0);
				m_btnChooseJoystick.SetIcon(hIcon);
			}
		}
	}
	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	RegisterHotKey(VK_VOLUME_DOWN);
	RegisterHotKey(VK_VOLUME_UP);
	RegisterHotKey(VK_VOLUME_MUTE);

	RegisterHotKey(VK_BROWSER_BACK);
	RegisterHotKey(VK_BROWSER_FORWARD);
	RegisterHotKey(VK_BROWSER_HOME);
	RegisterHotKey(VK_BROWSER_REFRESH);
	RegisterHotKey(VK_BROWSER_STOP);
	RegisterHotKey(VK_BROWSER_SEARCH);
	RegisterHotKey(VK_BROWSER_FAVORITES);

	RegisterHotKey(VK_MEDIA_STOP);
	RegisterHotKey(VK_MEDIA_NEXT_TRACK);
	RegisterHotKey(VK_MEDIA_PREV_TRACK);
	RegisterHotKey(VK_MEDIA_PLAY_PAUSE);

	RegisterHotKey(VK_LAUNCH_MAIL);
	RegisterHotKey(VK_LAUNCH_MEDIA_SELECT);
	RegisterHotKey(VK_LAUNCH_APP1);
	RegisterHotKey(VK_LAUNCH_APP2);

	RegisterHotKey(VK_SLEEP);

	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CTestJoystickDlg::RegisterHotKey(UINT uID)
{
	BOOL bRet;
	bRet = ::RegisterHotKey(m_hWnd, uID, 0, uID);
	if (!bRet)
	{
		OutputDebug(_T("RegisterHotKey(%d) Error %d\n"), uID, GetLastError());
	}
	else
	{
		SendMessage(WM_SETHOTKEY, uID, 0);
	}
}

void CTestJoystickDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	OutputDebug(_T("OnSysCommand(%d, %d)\n"), nID, lParam);
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.

void CTestJoystickDlg::OnPaint() 
{
	if (IsIconic())
	{
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

// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CTestJoystickDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
LRESULT CTestJoystickDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	OutputDebug(_T("OnHotKey(%x, %x)\n"), wParam, lParam);
	return 0;
}
LRESULT CTestJoystickDlg::OnUser(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CTestJoystickDlg::OnCancel()
{
	PostQuitMessage(0);
	DestroyWindow();
}

void CTestJoystickDlg::OnDestroy()
{
	if (pCplApplet)
	{
		pCplApplet(m_hWnd, CPL_STOP, 0, 0);
		pCplApplet(m_hWnd, CPL_EXIT, 0, 0);
	}
	KillTimer(100);
	if (m_pJoyStick)
	{
		delete m_pJoyStick;
		m_pJoyStick = NULL;
	}
	if (m_pJoyState)
	{
		delete m_pJoyState;
		m_pJoyState = NULL;
	}
	if (m_pJoyState2)
	{
		delete m_pJoyState2;
		m_pJoyState2 = NULL;
	}
	CDialog::OnDestroy();
}

void CTestJoystickDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	delete this;
}

void CTestJoystickDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 100 && m_pJoyStick && m_pJoyState2)
	{
		if (SUCCEEDED(m_pJoyStick->UpdateInputState(m_pJoyState, m_pJoyState2)))
		{
			DWORD dwCaps = m_pJoyStick->GetCapabilities();
			CPoint ptPos(0,0);
			if (dwCaps & JOY_CAPS_XAXIS)
			{
				ptPos.x = m_pJoyState2->lX;
			}
			if (dwCaps & JOY_CAPS_YAXIS)
			{
				ptPos.y = m_pJoyState2->lY;
			}
			if (dwCaps & (JOY_CAPS_XAXIS|JOY_CAPS_YAXIS))
			{
				m_ctrlTarget.SetPos(ptPos);
			}
			if (dwCaps & JOY_CAPS_ZAXIS)
			{
				m_SliderZ.SetPos(m_pJoyState2->lZ);
			}

			CPoint ptRPos(0,0);
			if (dwCaps & JOY_CAPS_RXAXIS)
			{
				ptRPos.x = m_pJoyState2->lRx;
			}
			if (dwCaps & JOY_CAPS_RYAXIS)
			{
				ptRPos.y = m_pJoyState2->lRy;
			}
			if (dwCaps & (JOY_CAPS_RXAXIS|JOY_CAPS_RYAXIS))
			{
				m_ctrlTargetR.SetPos(ptRPos);
			}

			if (dwCaps & JOY_CAPS_RZAXIS)
			{
				m_SliderRZ.SetPos(m_pJoyState2->lRz);
			}

			CDataExchange dx(this, FALSE);
			int i, n;
			if (m_bLearnMode != -1)
			{
				for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
				{
					if (m_pJoyState2->rgbButtons[i] != 0)
					{
						m_nButtonMap[i] = m_bLearnMode;
						if (m_nButtonMap[m_bLearnMode] == m_bLearnMode)
						{
							m_nButtonMap[m_bLearnMode] = i;
						}
						n = FALSE;
						DDX_Check(&dx, IDC_CK_BTN_1+m_bLearnMode, n);
						m_bLearnMode = -1;
						break;
					}
				}
			}
			else
			{
				for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
				{
					n = m_pJoyState2->rgbButtons[i] != 0 ? 1 : 0;
					DDX_Check(&dx, IDC_CK_BTN_1+m_nButtonMap[i], n);
				}
			}

			for (i=0; i<m_pJoyStick->GetNoOfSliders(); i++)
			{
				(&m_Slider1)[i].SetPos(m_pJoyState2->rglSlider[i]);
			}

			CString str;
			for (i=0; i<m_pJoyStick->GetNoOfPOVs(); i++)
			{
				if (i==0)
				{
					m_ctrlPOV.SetAngle(m_pJoyState2->rgdwPOV[i]);
				}
				str.Format(_T("%d"), m_pJoyState2->rgdwPOV[i]);
				GetDlgItem(IDC_TXT_POV1+i)->SetWindowText(str);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}
void CTestJoystickDlg::OnCkBtn(UINT nID)
{
	m_bLearnMode =  nID - IDC_CK_BTN_1;
	int i, n;
	for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
	{
		if (m_nButtonMap[i] != i)
		{
			if (m_nButtonMap[i] == m_bLearnMode)
			{
				m_nButtonMap[i] = i;
			}
		}
	}

	CDataExchange dx(this, FALSE);
	for (i=0; i<m_pJoyStick->GetNoOfButtons(); i++)
	{
		n = (IDC_CK_BTN_1+i) == nID;
		DDX_Check(&dx, IDC_CK_BTN_1+i, n);
	}
}

void CTestJoystickDlg::OnBnClickedBtnReset()
{
	int i;
	for (i=0; i<8; i++)
	{
		m_nButtonMap[i] = i;
	}
	if (m_hWnd && m_bLearnMode != -1)
	{
		CDataExchange dx(this, FALSE);
		i = FALSE;
		DDX_Check(&dx, IDC_CK_BTN_1+m_bLearnMode, i);
		m_bLearnMode = -1;
	}
}

void _cdecl CTestJoystickDlg::OutputDebug(LPCTSTR lpszFormat, ...)
{
   va_list args;
   va_start(args, lpszFormat);
   int nBuf;
   TCHAR szBuffer[1024];
   ZeroMemory(szBuffer, 1023);

   nBuf = _vstprintf(szBuffer, lpszFormat, args);

	_TCHAR *psz = szBuffer;
	while (psz)
	{
		psz = _tcsstr(psz, _T("\n"));
		if (psz)
		{
			int i, nLen = (int)_tcslen(psz);
			for (i=nLen; i>0; i--)
				psz[i] = psz[i-1];
			psz[0] = '\r';
			psz = &psz[2];
		}
	}
	
	if (m_Output.GetWindowTextLength() > 28000)
	{
		m_Output.SetWindowText(NULL);
	}
	m_Output.SetSel(-1, -1);
	m_Output.ReplaceSel(szBuffer);

	va_end(args);
}

void CTestJoystickDlg::OnBnClickedBtnChooseJoystick()
{
	if (pCplApplet)
	{
		pCplApplet(m_hWnd, CPL_DBLCLK, 0, 0);
	}
}
