// TestCIPCPerformanceDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "TestCIPCPerformance.h"
#include "TestCIPCPerformanceDlg.h"

#include "ipctestperformance.h"
#include ".\testcipcperformancedlg.h"
#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



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


// CTestCIPCPerformanceDlg Dialogfeld



CTestCIPCPerformanceDlg::CTestCIPCPerformanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestCIPCPerformanceDlg::IDD, pParent),
	m_cs(_T("CTestCIPCPerformanceDlg"))
{
	CWK_Criticalsection::SetTraceLocks(TRUE, 500);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCIPC = NULL;
	m_pThread = NULL;
	m_nCountTimer = 0;
	m_bCheck = FALSE;
	m_bCalcChecksum = FALSE;
	m_bInitRandomNumbers = FALSE;
	m_nBubblesPerSecondRes = 0;

#ifdef TEST_DV_STORAGE
	m_nBubbleSize = 10000;
	m_pData = malloc(m_nBubbleSize);
	m_nBubblesPerSecond = 100;
#endif
#ifdef TEST_PERFORMANCE
	m_nBubbleSize = 50000;
	m_pData = malloc(m_nBubbleSize);
	m_nBubblesPerSecond = 10;
#endif
}
CTestCIPCPerformanceDlg::~CTestCIPCPerformanceDlg()
{
	WK_DELETE(m_pCIPC);
	free(m_pData);
}

void CTestCIPCPerformanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TXT_INSTANCE, theApp.m_nInstance);
	DDX_Text(pDX, IDC_EDT_BUBBLES_PER_SECOND, m_nBubblesPerSecond);
	DDV_MinMaxLong(pDX, m_nBubblesPerSecond, 10, 10000);
	DDX_Text(pDX, IDC_EDT_BUBBLE_SIZE, m_nBubbleSize);
	DDV_MinMaxLong(pDX, m_nBubbleSize, 10, 1000000);
	DDX_Check(pDX, IDC_BTN_TEST, m_bCheck);
	DDX_Check(pDX, IDC_CK_CALC_CHECK_SUM, m_bCalcChecksum);
	DDX_Check(pDX, IDC_CK_INIT_RANDOM, m_bInitRandomNumbers);
}

BEGIN_MESSAGE_MAP(CTestCIPCPerformanceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_TEST, OnBnClickedBtnTest)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CK_CALC_CHECK_SUM, OnBnClickedCkCalcCheckSum)
	ON_BN_CLICKED(IDC_CK_INIT_RANDOM, OnBnClickedCkInitRandom)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CLEAR_DEBUG_OUT, OnBnClickedBtnClearDebugOut)
END_MESSAGE_MAP()


// CTestCIPCPerformanceDlg Meldungshandler

BOOL CTestCIPCPerformanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDebuggerWindowHandle(::GetDlgItem(m_hWnd, IDC_EDT_DEBUG));
#ifdef TEST_DV_STORAGE
	m_pCIPC = new CIPCTestPerformance(_T("DataBaseToServer"), FALSE, this);
#endif
#ifdef TEST_PERFORMANCE
	m_pCIPC = new CIPCTestPerformance(_T("SMTestCIPCPerformanceDlg"), theApp.m_nInstance == 1 ? TRUE : FALSE, this);
#endif
	TraceSystemInfos();
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

	m_nCountTimer = SetTimer(101, 1000, NULL);
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CTestCIPCPerformanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestCIPCPerformanceDlg::OnPaint() 
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
HCURSOR CTestCIPCPerformanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestCIPCPerformanceDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == m_nCountTimer)
	{
		CDataExchange dx(this, FALSE);
		DDX_Text(&dx, IDC_EDT_BUBBLES_PER_SECOND_RES, m_nBubblesPerSecondRes);
		m_nBubblesPerSecondRes *= m_nBubbleSize;
		DDX_Text(&dx, IDC_EDT_BYTES_PER_SECOND_RES, m_nBubblesPerSecondRes);
		m_nBubblesPerSecondRes = 0;
		m_nBubblesCount = 0;
	}
	CDialog::OnTimer(nIDEvent);
}

UINT CTestCIPCPerformanceDlg::ThreadProc(LPVOID pParam)
{
	XTIB::SetThreadName(_T(__FUNCTION__));

	CTestCIPCPerformanceDlg*pDlg = (CTestCIPCPerformanceDlg*)pParam;
	return pDlg->ThreadProc();
}

UINT CTestCIPCPerformanceDlg::ThreadProc()
{
	WK_TRACE(_T("Thread started with %d bubbles / sec \n"), m_nBubblesPerSecond);
#ifdef TEST_DV_STORAGE
	DWORD dwJobTime = 0;
	CSystemTime TimeStamp;
	WORD wArchiveNr = 0;
	CIPCPictureRecord pict(m_pCIPC, (const BYTE*)m_pData, m_nBubbleSize, CSecID(SECID_GROUP_OUTPUT, 0x1001),RESOLUTION_HIGH, COMPRESSION_1, COMPRESSION_MPEG4, TimeStamp, dwJobTime);
	CIPCField field1(_T("DBD_MD_X"), _T("5"), 'N');
	CIPCField field2(_T("DBD_MD_Y"), _T("12"), 'N');
	CIPCFields fields;
//	fields.AddNoDuplicates(field1);
//	fields.AddNoDuplicates(field2);
#endif

	while (m_bCheck)
	{
		m_cs.Lock(_T(__FUNCTION__));
		if (m_nBubblesCount++ < m_nBubblesPerSecond)
		{
#ifdef TEST_DV_STORAGE
			if (m_pCIPC)
			{
				wArchiveNr = 0x2000 | (WORD)MulDiv(rand(), 7, RAND_MAX);
				m_pCIPC->DoRequestNewSavePicture(wArchiveNr, pict, fields);
			}
#endif
#ifdef TEST_PERFORMANCE
			if (m_bInitRandomNumbers)
			{
				int i, nSize = m_nBubbleSize / sizeof(int);
				for (i=0; i<nSize; i++)
				{
					((int*)m_pData)[i] = rand();
				}
			}
			if (m_pCIPC)
			{
				m_pCIPC->DoRequestTransmitData(m_bCalcChecksum, 2, 3, m_nBubbleSize, m_pData);
			}
#endif
			Sleep(0);
		}
		else
		{
			Sleep(10);
		}
		m_cs.Unlock();
	}
	m_pThread = NULL;
	WK_TRACE(_T("Thread stopped\n"));
	return 0;
}

void CTestCIPCPerformanceDlg::OnBnClickedBtnTest()
{
	if (UpdateData(TRUE))
	{
		if (m_bCheck)
		{
			m_pData = realloc(m_pData, m_nBubbleSize);
			int i, nSize = m_nBubbleSize / sizeof(int);
			for (i=0; i<nSize; i++)
			{
				((int*)m_pData)[i] = rand();
			}
			m_pThread = AfxBeginThread(ThreadProc, (void*)this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
			m_pThread->m_bAutoDelete = TRUE;
		}
	}
	else
	{
		m_bCheck = FALSE;
		CDataExchange dx(this, FALSE);
		DDX_Check(&dx, IDC_BTN_TEST, m_bCheck);
	}
}

void CTestCIPCPerformanceDlg::OnDestroy()
{
	if (m_pThread)
	{
		m_bCheck = FALSE;
		WaitForSingleObject(m_pThread->m_hThread, 5000);
	}
	if (m_nCountTimer)
	{
		KillTimer(m_nCountTimer);
	}
	CDialog::OnDestroy();
}

void CTestCIPCPerformanceDlg::OnBnClickedCkCalcCheckSum()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_CALC_CHECK_SUM, m_bCalcChecksum);

// RKE: TestCriticalSections
//	CWK_Criticalsection::TraceLockedSections();
//	exit(0);
}

void CTestCIPCPerformanceDlg::OnBnClickedCkInitRandom()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_INIT_RANDOM, m_bInitRandomNumbers);

// RKE: TestCriticalSections
/*
	if (m_bInitRandomNumbers)
	{
//		CAutoCritSecEx acs1(&m_cs, _T(__FUNCTION__));
//		CAutoCritSecEx acs2(&m_cs);
		m_cs.Lock(_T(__FUNCTION__));
	}
	else
	{
		m_cs.Unlock();
	}
*/
}

void CTestCIPCPerformanceDlg::OnBnClickedBtnClearDebugOut()
{
	SendDlgItemMessage(IDC_EDT_DEBUG, WM_SETTEXT, 0, 0);
}

void CTestCIPCPerformanceDlg::TraceSystemInfos(void)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	WK_TRACE(_T("Processors:%d\n"),si.dwNumberOfProcessors);
	WK_TRACE(_T("PageSize:%d\n"),si.dwPageSize);

	if (IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE))
	{
		WK_TRACE(_T("The 3D-Now instruction set is available\n"));
	}
	if (IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE_DOUBLE))
	{
		WK_TRACE(_T("The compare and exchange double operation is available\n"));
	}
	if (IsProcessorFeaturePresent(PF_FLOATING_POINT_EMULATED))
	{
		WK_TRACE(_T("Floating-point operations are emulated using a software emulator\n"));
	}
	if (IsProcessorFeaturePresent(PF_FLOATING_POINT_PRECISION_ERRATA))
	{
		WK_TRACE(_T(" In rare circumstances, a floating-point precision error can occur\n"));
	}
	if (IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE))
	{
		WK_TRACE(_T("The MMX instruction set is available.\n"));
	}
	if (IsProcessorFeaturePresent(PF_PAE_ENABLED))
	{
		WK_TRACE(_T("The processor is PAE-enabled (Physical Address Extension).\n"));
	}
	if (IsProcessorFeaturePresent(PF_RDTSC_INSTRUCTION_AVAILABLE))
	{
		WK_TRACE(_T("The RDTSC instruction is available\n"));
	}
	if (IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE))
	{
		WK_TRACE(_T("The SSE instruction set is available\n"));
	}
	if (IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE))
	{
		WK_TRACE(_T("The SSE2 instruction set is available.\n"));
	}
}
