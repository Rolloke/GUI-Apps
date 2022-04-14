// ConfigureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Configure.h"
#include "ConfigureDlg.h"
#include "wk_profile.h"
#include "oemgui\oemguiapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigureDlg dialog

/////////////////////////////////////////////////////////////////////////////
CConfigureDlg::CConfigureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigureDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	CWK_Profile prof;

	m_nMiCoIOBase	= prof.GetInt("MiCoUnitPCI\\Device1\\System","MiCoIOBase",	  0x200);
	m_nCoVi1IOBase	= prof.GetInt("MiCoUnitPCI\\Device1\\System","ExtCard1IOBase", 0x220);
	m_nCoVi2IOBase  = prof.GetInt("MiCoUnitPCI\\Device1\\System","ExtCard2IOBase", 0x240);
	m_nCoVi3IOBase	= prof.GetInt("MiCoUnitPCI\\Device1\\System","ExtCard3IOBase", 0x320);
	m_nMiCoIRQ		= prof.GetInt("MiCoUnitPCI\\Device1\\System","IRQ", 11);
}

/////////////////////////////////////////////////////////////////////////////
CConfigureDlg::~CConfigureDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigureDlg)
	DDX_Control(pDX, IDC_COMBO_MICO_IRQ, m_comboMiCoIRQ);
	DDX_Control(pDX, IDC_COMBO_COVI3, m_comboCoVi3IOBase);
	DDX_Control(pDX, IDC_COMBO_COVI2, m_comboCoVi2IOBase);
	DDX_Control(pDX, IDC_COMBO_COVI1, m_comboCoVi1IOBase);
	DDX_Control(pDX, IDC_COMBO_MICO, m_comboMiCoIOBase);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConfigureDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigureDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELENDOK(IDC_COMBO_MICO, OnOkComboMico)
	ON_CBN_SELENDOK(IDC_COMBO_COVI1, OnOkComboCovi1)
	ON_CBN_SELENDOK(IDC_COMBO_COVI2, OnOkComboCovi2)
	ON_CBN_SELENDOK(IDC_COMBO_COVI3, OnOkComboCovi3)
	ON_CBN_SELENDOK(IDC_COMBO_MICO_IRQ, OnOkComboMicoIrq)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CConfigureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	CString sAddr;
	if (m_nMiCoIOBase == 0)
		sAddr = "Auto";
	else
		sAddr.Format("0x%x", m_nMiCoIOBase);
	
	int nIndex = m_comboMiCoIOBase.FindString(-1, sAddr);
	if (nIndex != CB_ERR)
		m_comboMiCoIOBase.SetCurSel(nIndex);

	sAddr.Format("%d", m_nMiCoIRQ);
	nIndex = m_comboMiCoIRQ.FindString(-1, sAddr);
	if (nIndex != CB_ERR)
		m_comboMiCoIRQ.SetCurSel(nIndex);

	sAddr.Format("0x%x", m_nCoVi1IOBase);
	nIndex = m_comboCoVi1IOBase.FindString(-1, sAddr);
	if (nIndex != CB_ERR)
		m_comboCoVi1IOBase.SetCurSel(nIndex);

	sAddr.Format("0x%x", m_nCoVi2IOBase);
	nIndex = m_comboCoVi2IOBase.FindString(-1, sAddr);
	if (nIndex != CB_ERR)
		m_comboCoVi2IOBase.SetCurSel(nIndex);
	
	sAddr.Format("0x%x", m_nCoVi3IOBase);
	nIndex = m_comboCoVi3IOBase.FindString(-1, sAddr);
	if (nIndex != CB_ERR)
		m_comboCoVi3IOBase.SetCurSel(nIndex);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		COemGuiApi::AboutDialog(this);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnPaint() 
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
/////////////////////////////////////////////////////////////////////////////
HCURSOR CConfigureDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOkComboMico() 
{
	int nCurSel = m_comboMiCoIOBase.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		CString sAddr;
		m_comboMiCoIOBase.GetLBText(nCurSel, sAddr);
		if (sAddr == "Auto")
			m_nMiCoIOBase = 0;
		sscanf(sAddr, "0x%03lx", &m_nMiCoIOBase);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOkComboCovi1() 
{
	int nCurSel = m_comboCoVi1IOBase.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		CString sAddr;
		m_comboCoVi1IOBase.GetLBText(nCurSel, sAddr);
		sscanf(sAddr, "0x%03lx", &m_nCoVi1IOBase);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOkComboCovi2() 
{
	int nCurSel = m_comboCoVi2IOBase.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		CString sAddr;
		m_comboCoVi2IOBase.GetLBText(nCurSel, sAddr);
		sscanf(sAddr, "0x%03lx", &m_nCoVi2IOBase);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOkComboCovi3() 
{
	int nCurSel = m_comboCoVi3IOBase.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		CString sAddr;
		m_comboCoVi3IOBase.GetLBText(nCurSel, sAddr);
		sscanf(sAddr, "0x%03lx", &m_nCoVi3IOBase);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOkComboMicoIrq() 
{
	int nCurSel = m_comboMiCoIRQ.GetCurSel();
	if (nCurSel != CB_ERR)
	{
		CString sAddr;
		m_comboMiCoIRQ.GetLBText(nCurSel, sAddr);
		sscanf(sAddr, "%d", &m_nMiCoIRQ);
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CConfigureDlg::OnOK() 
{
	CWK_Profile prof;
	CString		sErr	= "";
	
	if (m_nCoVi1IOBase == m_nCoVi2IOBase)
		sErr.LoadString(IDS_MSG_COVI1_COVI2_CONFLICT);
	if (m_nCoVi1IOBase == m_nCoVi3IOBase)
		sErr.LoadString(IDS_MSG_COVI1_COVI3_CONFLICT);
	if (m_nCoVi2IOBase == m_nCoVi3IOBase)
		sErr.LoadString(IDS_MSG_COVI2_COVI3_CONFLICT);

	if (!sErr.IsEmpty())
		MessageBox(sErr, "Fehler");
	else
	{
		prof.WriteInt("MiCoUnitPCI\\Device1\\System","MiCoIOBase",	   m_nMiCoIOBase);
		prof.WriteInt("MiCoUnitPCI\\Device1\\System","ExtCard1IOBase", m_nCoVi1IOBase);
		prof.WriteInt("MiCoUnitPCI\\Device1\\System","ExtCard2IOBase", m_nCoVi2IOBase);
		prof.WriteInt("MiCoUnitPCI\\Device1\\System","ExtCard3IOBase", m_nCoVi3IOBase);
		prof.WriteInt("MiCoUnitPCI\\Device1\\System","IRQ",			   m_nMiCoIRQ);
		prof.WriteInt("MiCoUnitPCI\\Device1\\General", "VideoRecheckTime", 10000);
		prof.WriteInt("RemoteControl","Init", 1);
		CDialog::OnOK();
	}
}

