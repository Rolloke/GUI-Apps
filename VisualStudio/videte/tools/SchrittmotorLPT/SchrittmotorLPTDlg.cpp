
// SchrittmotorLPTDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "SchrittmotorLPT.h"
#include "SchrittmotorLPTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"

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


// CSchrittmotorLPTDlg-Dialogfeld




CSchrittmotorLPTDlg::CSchrittmotorLPTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSchrittmotorLPTDlg::IDD, pParent)
    , mDelay_ms(1)
    , mTestSteps(500)
    , mDistanceX(1)
    , mDistanceY(1)
    , mDistanceZ(1)
    , mInterpreter(mPlotterDC)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSchrittmotorLPTDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDT_DELAY_MS, mDelay_ms);
    DDV_MinMaxInt(pDX, mDelay_ms, 1, 10000);
    DDX_Text(pDX, IDC_EDT_TEST_STEPS, mTestSteps);
    DDV_MinMaxInt(pDX, mTestSteps, 1, 10000);
    DDX_Text(pDX, IDC_EDT_DISTANCE_X, mDistanceX);
    DDV_MinMaxFloat(pDX, mDistanceX, 0.0001f, 1);
    DDX_Text(pDX, IDC_EDT_DISTANCE_Y, mDistanceY);
    DDV_MinMaxFloat(pDX, mDistanceY, 0.0001f, 1);
    DDX_Text(pDX, IDC_EDT_DISTANCE_Z, mDistanceZ);
    DDV_MinMaxFloat(pDX, mDistanceZ, 0.0001f, 1);
    DDX_Control(pDX, IDC_TEST_DRAW, mTestDraw);
}

BEGIN_MESSAGE_MAP(CSchrittmotorLPTDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_RIGHT, CSchrittmotorLPTDlg::OnBnClickedBtnRight)
    ON_BN_CLICKED(IDC_BTN_LEFT, CSchrittmotorLPTDlg::OnBnClickedBtnLeft)
    ON_BN_CLICKED(IDC_BTN_CALIBRATE, CSchrittmotorLPTDlg::OnBnClickedBtnCalibrate)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BTN_LOAD_PS_FILE, CSchrittmotorLPTDlg::OnBnClickedBtnLoadPsFile)
    ON_BN_CLICKED(IDC_BTN_PLOTT, CSchrittmotorLPTDlg::OnBnClickedBtnPlott)
END_MESSAGE_MAP()


// CSchrittmotorLPTDlg-Meldungshandler

BOOL CSchrittmotorLPTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

    mMotorDriver.CreateThread();
    BOOL bSet = mMotorDriver.SetThreadPriority(THREAD_PRIORITY_TIME_CRITICAL);

    mPlotterDC.Attach(::GetWindowDC(mTestDraw.m_hWnd));

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CSchrittmotorLPTDlg::OnDestroy()
{
    ::ReleaseDC(mTestDraw.m_hWnd, mPlotterDC.Detach());
    CDialog::OnDestroy();
}

void CSchrittmotorLPTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
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

void CSchrittmotorLPTDlg::OnPaint()
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
HCURSOR CSchrittmotorLPTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSchrittmotorLPTDlg::OnBnClickedBtnLeft()
{
    if (UpdateData())
    {
        mMotorDriver.setDelayIn_ms(mDelay_ms);
        mMotorDriver.setTestSteps(mTestSteps);
        mMotorDriver.PostThreadMessage(WM_TEST_RUN_LEFT, 0, 0);
    }
}

void CSchrittmotorLPTDlg::OnBnClickedBtnRight()
{
    if (UpdateData())
    {
        mMotorDriver.setDelayIn_ms(mDelay_ms);
        mMotorDriver.setTestSteps(mTestSteps);
        mMotorDriver.PostThreadMessage(WM_TEST_RUN_RIGHT, 0, 0);
    }
}

void CSchrittmotorLPTDlg::OnBnClickedBtnCalibrate()
{
    mMotorDriver.PostThreadMessage(WM_CALIBRATE_PLOTTER, 0, 0);
}


void CSchrittmotorLPTDlg::OnBnClickedBtnLoadPsFile()
{
    CFileDialog dlg(TRUE, ".ps", 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, "Postscript File|*.ps|All files|*.*||", this);
    if (dlg.DoModal() == IDOK)
    {
        mInterpreter.readPostscript(dlg.GetPathName());
    }
}

void CSchrittmotorLPTDlg::OnBnClickedBtnPlott()
{
    mMotorDriver.setPlotCommands(mPlotterDC.getDrawCommands());
    mMotorDriver.PostThreadMessage(WM_PLOT_COMMANDS, 0, 0);
}
