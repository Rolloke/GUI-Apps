// DBCheck.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DBCheck.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "DBCheckDoc.h"
#include "LeftView.h"
#include "IPCServerControlDBCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBCheckApp

BEGIN_MESSAGE_MAP(CDBCheckApp, CWinApp)
	//{{AFX_MSG_MAP(CDBCheckApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBCheckApp construction

CDBCheckApp::CDBCheckApp()
{
	m_pUser       = NULL;
	m_pPermission = NULL;
   m_pCIPCServerControlDBCheck = NULL;
}
CDBCheckApp::~CDBCheckApp()
{
	WK_DELETE(m_pUser);
	WK_DELETE(m_pPermission);
   WK_DELETE(m_pCIPCServerControlDBCheck);
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CDBCheckApp object

CDBCheckApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDBCheckApp initialization

BOOL CDBCheckApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


   InitDebugger("DBCheck.log",WAI_INVALID);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

   if (!Login())
	{
		return FALSE;
	}

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_DBCHECTYPE,
		RUNTIME_CLASS(CDBCheckDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

   m_pCIPCServerControlDBCheck = new CIPCServerControlDBCheck;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CSpinButtonCtrl	m_Spin;
	CStatic	m_StaticIcon;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
   
// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnChangeEdit1();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   CImageList m_SmallImage;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Control(pDX, IDC_STATIC_ICON, m_StaticIcon);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDBCheckApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDBCheckApp message handlers


BOOL CDBCheckApp::OnIdle(LONG lCount) 
{
	if (WK_IS_WINDOW(m_pMainWnd))
	{
		((CMainFrame*)m_pMainWnd)->OnIdle();
	}
	CErrorMessage* pErrorMessage = m_ErrorMessages.SafeGetAndRemoveHead();

	if (pErrorMessage)
	{
		CString s;
		s.Format(IDP_ERROR,pErrorMessage->GetServer(),
			pErrorMessage->GetMessage());
		COemGuiApi::MessageBox(s,10,MB_ICONINFORMATION|MB_OK);
		WK_DELETE(pErrorMessage);
	}
	return CWinApp::OnIdle(lCount);
}

CHostArray &CDBCheckApp::GetHosts()
{
	CWK_Profile wkp;
	m_Hosts.Load(wkp);
	return m_Hosts;
}

int CDBCheckApp::ExitInstance() 
{
   CloseDebugger();
	return 0;
}

BOOL CDBCheckApp::LoginAsSuperVisor()
{
	CWK_Profile wkp;
	WK_DELETE(m_pUser);
	CPermissionArray PermissionArray;
	CPermission* pPermission;

	PermissionArray.Load(wkp);
	pPermission = PermissionArray.GetSuperVisor();
	WK_DELETE(m_pPermission);
	m_pPermission = new CPermission(*pPermission);
	
	m_pUser = new CUser();
	m_pUser->SetName("SuperVisor");
	m_pUser->SetPermissionID(m_pPermission->GetID());
	return TRUE;
}
BOOL CDBCheckApp::Login()
{
#ifdef _DEBUG
//	return LoginAsSuperVisor();
#endif
/*
	if (m_bReadOnlyModus)
	{
		return LoginAsSuperVisor();
	}
*/
	CWK_Profile wkp;
	CPermissionArray pa;
	CPermission* pPermission = pa.GetSpecialReceiver();
	if (pPermission)
	{
		WK_DELETE(m_pUser);
		m_pUser = new CUser();
		m_pUser->SetName(pPermission->GetName());
		WK_DELETE(m_pPermission);
		m_pPermission = new CPermission(*pPermission);
		WK_TRACE_USER("%s hat sich eingeloggt.\n",m_pUser->GetName());
		return TRUE;
	}
	else
	{
		COEMLoginDialog dlg(AfxGetMainWnd());

		if (IDOK==dlg.DoModal())
		{
			if (m_pUser)
			{
				WK_TRACE_USER("%s hat sich ausgeloggt.\n",m_pUser->GetName());
			}
			WK_DELETE(m_pUser);
			m_pUser = new CUser(dlg.GetUser());
			WK_DELETE(m_pPermission);
			m_pPermission = new CPermission(dlg.GetPermission());
			if (m_pUser)
			{
				WK_TRACE_USER("%s hat sich eingeloggt.\n",m_pUser->GetName());
			}

			return (m_pUser && m_pUser->GetID()!=SECID_NO_ID) &&
				   (m_pPermission && m_pPermission->GetID()!=SECID_NO_ID);
		}
	}

	return FALSE;
}

WORD CDBCheckApp::GetNewServerID()
{
   POSITION pos    = m_pDocManager->GetFirstDocTemplatePosition();
   POSITION posD   = NULL;
   WORD     wNewID = 1;
   while (pos)
   {
      CDocTemplate*pDocT = m_pDocManager->GetNextDocTemplate(pos);
      posD = pDocT->GetFirstDocPosition();
      while (posD)
      {
         CDocument*pDoc = pDocT->GetNextDoc(posD);
         if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDBCheckDoc)))
         {
            if (wNewID == ((CDBCheckDoc*)pDoc)->GetServerID())
            {
               wNewID++;
               continue;
            }
         }
      }
   }
   return wNewID;
}

CServer* CDBCheckApp::GetServerByIndex(int nID)
{
   POSITION pos    = m_pDocManager->GetFirstDocTemplatePosition();
   POSITION posD   = NULL;
   while (pos)
   {
      CDocTemplate*pDocT = m_pDocManager->GetNextDocTemplate(pos);
      posD = pDocT->GetFirstDocPosition();
      while (posD)
      {
         CDocument*pDoc = pDocT->GetNextDoc(posD);
         if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDBCheckDoc)) && (0 == nID--))
         {
            return ((CDBCheckDoc*)pDoc)->GetServer(0);
         }
      }
   }
   return NULL;
}

bool CDBCheckApp::IsLocked()
{
   POSITION pos    = m_pDocManager->GetFirstDocTemplatePosition();
   POSITION posD   = NULL;
   while (pos)
   {
      CDocTemplate*pDocT = m_pDocManager->GetNextDocTemplate(pos);
      posD = pDocT->GetFirstDocPosition();
      while (posD)
      {
         CDocument*pDoc = pDocT->GetNextDoc(posD);
         if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CDBCheckDoc)))
         {
            CIPCDatabaseDBCheck *pDB = ((CDBCheckDoc*)pDoc)->GetServer(0)->GetDatabase();
            if (pDB && pDB->IsLocked())
            { 
               return true;
            }
         }
      }
   }
   return false;
}
int nList[3] = {0, 5, 6};
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   int i = 0;
	m_SmallImage.Create(IDB_IMAGES1,27,1,RGB(0,255,255));
//   m_Spin.SetBuddy(&m_StaticIcon);
   m_Spin.SetRange32(0, 2);
//   m_StaticIcon.ModifyStyle(0, SS_ICON|SS_OWNERDRAW, 0);
   m_Spin.SetPos(i);
   HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(m_SmallImage), nList[i], ILD_TRANSPARENT);
   m_StaticIcon.SetIcon(hIcon);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	*pResult = 0;
}

void CAboutDlg::OnOK() 
{
	int nPos = m_Spin.GetPos();
	
	CDialog::OnOK();
}

void CAboutDlg::OnChangeEdit1() 
{
   if (m_Spin.m_hWnd == NULL) return;
   m_StaticIcon.InvalidateRect(NULL);
/*
   int nPos = m_Spin.GetPos();
   if (nPos>=0 && nPos < 3)
   {
      HICON hIcon = ::ImageList_GetIcon(HIMAGELIST(m_SmallImage), nList[nPos], ILD_TRANSPARENT);
      m_StaticIcon.SetIcon(hIcon);
   }
*/
}

void CAboutDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   if (IDC_STATIC_ICON == nIDCtl)
   {
      int nPos = m_Spin.GetPos();
      ::SaveDC(lpDrawItemStruct->hDC);
      ::FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, GetSysColorBrush(COLOR_BTNFACE));
      ::SetBkColor(lpDrawItemStruct->hDC, GetSysColor(COLOR_BTNFACE));
      ::ImageList_Draw( HIMAGELIST(m_SmallImage), nList[nPos], lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top, ILD_TRANSPARENT);
      CString str;
      str.Format("Text %d", nPos);
      lpDrawItemStruct->rcItem.left+=30;
      ::DrawText(lpDrawItemStruct->hDC, str, str.GetLength(), &lpDrawItemStruct->rcItem, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
      ::RestoreDC(lpDrawItemStruct->hDC, -1);
   }
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
