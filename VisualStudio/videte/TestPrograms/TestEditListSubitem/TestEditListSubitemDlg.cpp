// TestEditListSubitemDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "TestEditListSubitem.h"
#include "TestEditListSubitemDlg.h"
#include ".\testeditlistsubitemdlg.h"

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
public:
	afx_msg void OnBnClickedBtnToTime();
	afx_msg void OnBnClickedBtnToValue();
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_TO_TIME, OnBnClickedBtnToTime)
	ON_BN_CLICKED(IDC_BTN_TO_VALUE, OnBnClickedBtnToValue)
END_MESSAGE_MAP()


// CTestEditListSubitemDlg Dialogfeld



CTestEditListSubitemDlg::CTestEditListSubitemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestEditListSubitemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestEditListSubitemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
}

BEGIN_MESSAGE_MAP(CTestEditListSubitemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_OPTIMIZE_COLUMN_WIDTH, OnBnClickedBtnOptimizeColumnWidth)
END_MESSAGE_MAP()


// CTestEditListSubitemDlg Meldungshandler

BOOL CTestEditListSubitemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

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

	ListView_SetExtendedListViewStyleEx(m_List.m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyleEx(m_List.m_hWnd, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);
	CRect rc;
	m_List.GetClientRect(&rc);
	int nWidth = rc.Width() / 3;
	m_List.InsertColumn(0, _T("Column 1"), LVCFMT_LEFT, nWidth);
	m_List.InsertColumn(1, _T("Column 2"), LVCFMT_LEFT, nWidth);
	m_List.InsertColumn(2, _T("Column 3"), LVCFMT_LEFT, nWidth);
	m_List.InsertItem(0, _T("Row 1"));
	m_List.InsertItem(1, _T("Row 2"));
	m_List.InsertItem(2, _T("Row 3"));
	m_List.InsertItem(3, _T("Row 4"));
	m_List.SetItemText(0, 1, _T("Row 1"));
	m_List.SetItemText(1, 1, _T("Row 2"));
	m_List.SetItemText(2, 1, _T("Row 3"));
	m_List.SetItemText(3, 1, _T("Row 4"));

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

void CTestEditListSubitemDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestEditListSubitemDlg::OnPaint() 
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
HCURSOR CTestEditListSubitemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestEditListSubitemDlg::OnBnClickedOk()
{
	if (m_List.ValidateAll())
	{
		OnOK();
	}
}

void CTestEditListSubitemDlg::OnBnClickedBtnOptimizeColumnWidth()
{
	CRect rc;
	CString sText;
	int i, j, nItems, nColumns, nWidth, nMinWidth, nTotal;
	TCHAR szColumn[64];
	LVCOLUMN lvc;
	ZERO_INIT(lvc);
	lvc.mask = LVCF_TEXT;
	lvc.pszText = szColumn;
	lvc.cchTextMax = 64;

	m_List.GetClientRect(&rc);
	nTotal = rc.Width();
	nItems = m_List.GetItemCount();
	nColumns = m_List.GetSubItemCount();

	for (j=0; j<nColumns-1; j++)
	{
		m_List.GetColumn(j, &lvc);
		nMinWidth = m_List.GetStringWidth(szColumn) + 15;
		for (i=0; i<nItems; i++)
		{
			sText = m_List.GetItemText(i, j);
			nWidth = m_List.GetStringWidth(sText) + 5;
			nMinWidth = max(nWidth, nMinWidth);
		}
		m_List.SetColumnWidth(j, nMinWidth);
		nTotal -= nMinWidth;
	}
	if (nTotal > 0)
	{
		m_List.SetColumnWidth(j, nTotal);
	}
}

void CAboutDlg::OnBnClickedBtnToTime()
{
	CString s;
	BOOL bTrans = FALSE;
	CTime time((time_t)GetDlgItemInt(IDC_EDT_TIME_AS_NUMER, &bTrans, FALSE));
	s = time.Format(_T("%d.%m.%Y"));
	SetDlgItemText(IDC_EDT_TIME, s);
}

void CAboutDlg::OnBnClickedBtnToValue()
{
	CString s;
	int nD=0, nM=0, nY=0;
	GetDlgItemText(IDC_EDT_TIME, s);
	_stscanf(s, _T("%d.%d.%d"), &nD, &nM, &nY);
	CTime time(nY, nM, nD, 0, 0, 0);
	SetDlgItemInt(IDC_EDT_TIME_AS_NUMER, (UINT)time.GetTime(), FALSE);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString s;
	CTime time;
	time = CTime::GetCurrentTime();
	s = time.Format(_T("%d.%m.%Y"));

	SetDlgItemText(IDC_EDT_TIME, s);
	SetDlgItemInt(IDC_EDT_TIME_AS_NUMER, (UINT)time.GetTime(), FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
