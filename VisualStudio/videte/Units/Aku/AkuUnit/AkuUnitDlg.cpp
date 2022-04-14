/////////////////////////////////////////////////////////////////////////////
// PROJECT:		AkuUnit
// FILE:		$Workfile: AkuUnitDlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/Aku/AkuUnit/AkuUnitDlg.cpp $
// CHECKIN:		$Date: 27.02.04 9:53 $
// VERSION:		$Revision: 10 $
// LAST CHANGE:	$Modtime: 27.02.04 9:08 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "wk.h"
#include "wk_names.h"
#include "AkuUnit.h"
#include "AkuUnitDlg.h"
#include "CAku.h"
#include "CipcInputAkuUnit.h"
#include "CipcOutputAkuUnit.h"
#include "oemgui\oemguiapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CAkuUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitDlg dialog

CAkuUnitDlg::CAkuUnitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAkuUnitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAkuUnitDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_bOk				= FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_AKUUNIT);

	m_pAku				= NULL;
	m_pOutput			= NULL;
	m_pInput			= NULL;
}

void CAkuUnitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAkuUnitDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAkuUnitDlg, CDialog)
	//{{AFX_MSG_MAP(CAkuUnitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_QUERYOPEN()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_AKU_ALARM, OnWmAlarm)
	ON_COMMAND(ID_ABOUT,	OnAbout)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitDlg message handlers

BOOL CAkuUnitDlg::OnInitDialog()
{
 	TCHAR	szSMName[128];

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Aku-Objekt anlegen
	m_pAku = new CAku(m_hWnd, AKU_INI);
	
	if (!m_pAku)
	{
		 WK_TRACE( _T("ERROR:\tCAkuUnitDlg::OnInitDialog\tCAku-Objekt konnte nicht angelegt werden.\n"));
		return -1;
	}		

	if (!m_pAku->IsValid())
	{
		 WK_TRACE( _T("ERROR:\tCAkuUnitDlg::OnInitDialog\tCAku-Objekt konnte nicht korrekt initialisiert werden.\n"));
		WK_DELETE(m_pAku);
		return -1;
	}		
	
	// die CIPC-Schnittstellen aufbauen.
	wsprintf(szSMName, SM_AKU_INPUT);
	m_pInput = new CIPCInputAkuUnit(m_pAku, szSMName);
	if (!m_pInput)
		 WK_TRACE( _T("ERROR:\tCAkuUnitDlg::OnInitDialog\tCIPCInputAkuUnit-Objekt konnte nicht angelegt werden.\n"));

	wsprintf(szSMName, SM_AKU_OUTPUT);
	m_pOutput = new CIPCOutputAkuUnit(this, m_pAku, szSMName);
	if (!m_pOutput)
		 WK_TRACE( _T("ERROR:\tCAkuUnitDlg::OnInitDialog\tCIPCOutputAkuUnit-Relay-Objekt konnte nicht angelegt werden.\n"));	

	ShowWindow(SW_SHOWMINNOACTIVE);

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_AKUUNIT);
	SetWindowText(sTip);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_AKUUNIT);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);
	ShowWindow(SW_HIDE);

	m_bOk  = TRUE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAkuUnitDlg::OnQueryOpen()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////

void CAkuUnitDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAkuUnitDlg::OnPaint() 
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
HCURSOR CAkuUnitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAkuUnitDlg::OnDestroy() 
{
	 WK_TRACE( _T("CAkuUnitDlg::OnDestroy()\n"));

	CDialog::OnDestroy();
	
 	m_bOk  = FALSE;

	WK_DELETE(m_pInput);
	WK_DELETE(m_pOutput);

	WK_DELETE(m_pAku);

	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
}

/////////////////////////////////////////////////////////////////////////////
// CAkuUnitDlg::OnWmAlarm
long CAkuUnitDlg::OnWmAlarm(WPARAM wParam, LPARAM lParam)
{
	WORD wAkuCard	= (WORD)wParam;
	BYTE byAlarm	= (BYTE)lParam;

//	 WK_TRACE( _T("CAkuUnitDlg::OnAlarm\tAkuCard%u\tAlarm%u\n"), wAkuCard, byAlarm);

	m_pInput->AlarmStateChanged(wAkuCard, byAlarm);

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
long CAkuUnitDlg::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_AKUUNIT, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CAkuUnitDlg::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);
			
			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CAkuUnitDlg::OnAbout()
{
	COemGuiApi::AboutDialog(this);
}
