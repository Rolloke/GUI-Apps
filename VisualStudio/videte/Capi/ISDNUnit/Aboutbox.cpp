/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: Aboutbox.cpp $
// ARCHIVE:		$Archive: /Project/Capi/ISDNUnit/Aboutbox.cpp $
// CHECKIN:		$Date: 12/07/05 3:53p $
// VERSION:		$Revision: 94 $
// LAST CHANGE:	$Modtime: 12/07/05 3:51p $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "resource.h"
#include "ISDNUnit.h"
#include "aboutbox.h"
#include "WK_Version.h"
#include "ISDNConnection.h"
#include "OemGui/OemGuiApi.h"
#include ".\aboutbox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutBox dialog
CAboutBox::CAboutBox(BOOL bModeless, CISDNConnection* pConnection, CWnd* pParent /*=NULL*/)
	: CDialog(CAboutBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutBox)
	m_sModuleName = _T("");
	m_sVersion = _T("");
	m_bMSNSupport = FALSE;
	m_sConnectState = _T("");
	//}}AFX_DATA_INIT
	m_pWndParent = NULL;
	m_bModeless = TRUE;
	m_pConnection = NULL;

	m_pWndParent = pParent;
	m_bModeless = bModeless;
	m_pConnection = pConnection;
	m_connectionState = CONNECT_ERROR;
	m_nTimer = 0;
}

/////////////////////////////////////////////////////////////////////////////
CAboutBox::~CAboutBox()
{
}

void CAboutBox:: DoUpdateConnectState(ConnectionState cs,
									  const CString sRemoteHostName,
									  const CString &sRemoteNumber,
									  BOOL bSupportsMSN)
{
	BOOL bModified = FALSE;

	if (cs!=m_connectionState) 
	{
		LPCTSTR szText=NULL;
		switch (cs) {
			case CONNECT_ERROR:
				szText = _T("ERROR");
				break;
			case CONNECT_CLOSED:
				szText = _T("CLOSED");
				break;
			case CONNECT_REQUEST:
				szText = _T("REQUEST");
				break;
			case CONNECT_OPEN:
				szText = _T("OPEN");
				break;
			case CONNECT_CLOSING:
				szText = _T("CLOSING");
				break;
			default:
				szText = _T("UNKNOWN");
				break;
		}
		m_connectionState = cs;
		m_sConnectState = szText;
		bModified = TRUE;
	}

	if (bSupportsMSN!=m_bMSNSupport)
	{
		m_bMSNSupport= bSupportsMSN;
		bModified = TRUE;
	}
	// always set name ?
	CString sRemote;
	if (cs==CONNECT_OPEN)
	{	// OOPS or also request etc ?
		sRemote = sRemoteHostName;
		sRemote += _T(" [");
		sRemote += sRemoteNumber;
		sRemote += _T("]");
	}
	else
	{
		sRemote=_T("");
	}
	SetDlgItemText(IDC_REMOTE_STATION, sRemote);

	if (bModified)
	{
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAboutBox::ShowDlg()
{
	SetWindowPos( &wndTopMost, NULL, NULL, NULL, NULL, SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
}

/////////////////////////////////////////////////////////////////////////////
void CAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutBox)
	DDX_Control(pDX, IDC_MSN_SUPPORT, m_ctlMSNSupport);
	DDX_Control(pDX, IDC_HIDDEN_QUIT, m_cbHiddenQuit);
	DDX_Text(pDX, IDC_TXT_MODULE_NAME, m_sModuleName);
	DDX_Text(pDX, IDC_TXT_VERSION, m_sVersion);
	DDX_Check(pDX, IDC_MSN_SUPPORT, m_bMSNSupport);
	DDX_Text(pDX, IDC_CONNECT_STATE, m_sConnectState);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutBox, CDialog)
	//{{AFX_MSG_MAP(CAboutBox)
	ON_BN_CLICKED(IDC_HIDDEN_QUIT, OnHiddenQuit)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutBox message handlers
/*@MD
PRE: ISDNUnitApp has read its settings.
*/
BOOL CAboutBox::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifndef _DEBUG
	// disable hidden Quit button in release
	m_cbHiddenQuit.ShowWindow(SW_HIDE);
#endif

	m_ctlMSNSupport.EnableWindow(FALSE);

	m_sModuleName = COemGuiApi::GetApplicationName(theApp.GetApplicationId());

	CString sTitle(m_sModuleName);
	CString sTrailer;
	sTrailer.LoadString(IDS_STATE_TITLE_TRAILER);
	if (sTrailer.IsEmpty())
	{
		sTrailer = _T(" - Aktueller Zustand");
	}
	sTitle += sTrailer;
	SetWindowText(sTitle);

	m_sVersion = PLAIN_TEXT_VERSION(17, 3, 13, 36, _T("17/03/99,13:36\0"), // @AUTO_UPDATE
			0);

	CWnd *pHostWnd = GetDlgItem(IDC_STATIC_HOSTNAME);
	if (pHostWnd) 
	{
		pHostWnd->SetWindowText(MyGetApp()->GetOwnHostName());
	}
	else
	{
		// not found !?
	}

	CWnd *pOwnNumberWnd= GetDlgItem(IDC_STATIC_OWN_NUMBER);
	if (pOwnNumberWnd)
	{
		pOwnNumberWnd->SetWindowText(MyGetApp()->GetOwnNumber());
	}
	else
	{
		// not found !?
	}

	// initialize the big icon control
	m_icon.SubclassDlgItem(IDC_BIGICON, this);
	m_icon.SizeToContent();

	UpdateData(FALSE);
	ShowDlg();

	// Position des Dlg in der unteren rechten Ecke berechnen
	CRect rectDlg;
	GetWindowRect(&rectDlg);

	// NEW small offset of 50
	int iLeft	= GetSystemMetrics(SM_CXSCREEN)-rectDlg.Width()-50;
	int iTop	= GetSystemMetrics(SM_CYSCREEN)-rectDlg.Height()-50;

/*	// Falls Taskbar, entsprechend hoeher setzen
	APPBARDATA appBarData;
	SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData);
	iTop -= appBarData.rc.bottom - appBarData.rc.top;
*/


	SetWindowPos( &CWnd::wndTopMost,
				  iLeft,
				  iTop,
				  NULL,
				  NULL,
				  SWP_NOSIZE|SWP_SHOWWINDOW
				  );

	m_nTimer = SetTimer(0x0815, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CAboutBox::OnOK() 
{
	ShowWindow(SW_HIDE);
	// CAVEAT do not call CDialog::OnOK for NonModal
}

/////////////////////////////////////////////////////////////////////////////
void CAboutBox::OnCancel() 
{
	ShowWindow(SW_HIDE);
	// CAVEAT do not call CDialog::OnCancel for NonModal
}

/////////////////////////////////////////////////////////////////////////////
// CBigIcon
BEGIN_MESSAGE_MAP(CBigIcon, CButton)
	//{{AFX_MSG_MAP(CBigIcon)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBigIcon message handlers

#define CY_SHADOW   4
#define CX_SHADOW   4

void CBigIcon::SizeToContent()
{
	// get system icon size
	int cxIcon = ::GetSystemMetrics(SM_CXICON);
	int cyIcon = ::GetSystemMetrics(SM_CYICON);

	// a big icon should be twice the size of an icon + shadows
	SetWindowPos(NULL, 0, 0, cxIcon*2 + CX_SHADOW + 4, cyIcon*2 + CY_SHADOW + 4,
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
void CBigIcon::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT(pDC != NULL);

	CRect rect;
	GetClientRect(rect);
	int cxClient = rect.Width();
	int cyClient = rect.Height();

	// load icon
	HICON hIcon = NULL;
	if (theApp.RunAsPTUnit())
	{
		hIcon = theApp.LoadIcon(IDR_MAINFRAME_PT);	
	}
	else
	{
		hIcon = theApp.LoadIcon(IDR_MAINFRAME);
	}
	if (hIcon == NULL)
	{
		return;
	}

	// draw icon into off-screen bitmap
	int cxIcon = ::GetSystemMetrics(SM_CXICON);
	int cyIcon = ::GetSystemMetrics(SM_CYICON);

	CBitmap bitmap;
	if (!bitmap.CreateCompatibleBitmap(pDC, cxIcon, cyIcon))
		return;
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(pDC))
		return;
	CBitmap* pBitmapOld = dcMem.SelectObject(&bitmap);
	if (pBitmapOld == NULL)
		return;

	// blt the bits already on the window onto the off-screen bitmap
	dcMem.StretchBlt(0, 0, cxIcon, cyIcon, pDC,
		2, 2, cxClient-CX_SHADOW-4, cyClient-CY_SHADOW-4, SRCCOPY);

	// draw the icon on the background
	dcMem.DrawIcon(0, 0, hIcon);

	// draw border around icon
	CPen pen;
	pen.CreateStockObject(BLACK_PEN);
	CPen* pPenOld = pDC->SelectObject(&pen);
	pDC->Rectangle(0, 0, cxClient-CX_SHADOW, cyClient-CY_SHADOW);
	if (pPenOld)
		pDC->SelectObject(pPenOld);

	// draw shadows around icon
	CBrush br;
	br.CreateStockObject(DKGRAY_BRUSH);
	rect.SetRect(cxClient-CX_SHADOW, CY_SHADOW, cxClient, cyClient);
	pDC->FillRect(rect, &br);
	rect.SetRect(CX_SHADOW, cyClient-CY_SHADOW, cxClient, cyClient);
	pDC->FillRect(rect, &br);

	// draw the icon contents
	pDC->StretchBlt(1, 1, cxClient-CX_SHADOW-4, cyClient-CY_SHADOW-4,
		&dcMem, 0, 0, cxIcon, cyIcon, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CBigIcon::OnEraseBkgnd(CDC*)
{
	return TRUE;    // we don't do any erasing...
}



void CAboutBox::OnHiddenQuit() 
{
	ShowWindow(SW_HIDE);
	WK_TRACE(_T("QUIT clicked\n"));
	Sleep(100);
	// OOPS ::PostQuitMessage(456); still has windows left, mem leak
	// this works better :-)
	MyGetApp()->m_pMainWnd->DestroyWindow();	// OOPS
}


void CAboutBox::OnTimer(UINT nIDEvent)
{
	if (m_nTimer == nIDEvent)
	{
		theApp.UpdateAboutBox();
	}

	CDialog::OnTimer(nIDEvent);
}

void CAboutBox::OnDestroy()
{
	if (m_nTimer)
	{
		KillTimer(m_nTimer);
	}
	CDialog::OnDestroy();
}
