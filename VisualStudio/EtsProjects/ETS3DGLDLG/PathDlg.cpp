// PathDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ETS3DGLDLG.h"
#include "PathDlg.h"
#include "Ets3dGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPathDlg 


CPathDlg::CPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPathDlg)
	m_strDllPath = _T("");
	m_nPathSelection = 0;
	m_bUseDebugDll = FALSE;
	m_bUseLocalDll = FALSE;
	//}}AFX_DATA_INIT

   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   pApp->SetProfileName(NULL, ETSBIND_REGKEY);
   m_strDllPath = pApp->GetProfileString("", REGKEY_DLLPATH);
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME3);
   m_bUseDebugDll = pApp->GetProfileInt("", REGKEY_USEDEBUGDLL, 0);
   m_bUseLocalDll = pApp->GetProfileInt("", REGKEY_USELOCALDLL, 0);
   m_bPathChanged = false;
}

void CPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPathDlg)
	DDX_Text(pDX, IDC_DLL_PATH, m_strDllPath);
   DDV_MaxChars(pDX, m_strDllPath, _MAX_PATH);
	DDX_Radio(pDX, IDC_R_PATH0, m_nPathSelection);
	DDX_Check(pDX, IDC_CK_DEBUG, m_bUseDebugDll);
	DDX_Check(pDX, IDC_CK_NO_DLLPATH, m_bUseLocalDll);
	//}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CPathDlg, CDialog)
	//{{AFX_MSG_MAP(CPathDlg)
	ON_BN_CLICKED(IDC_SEARCH_PATH, OnSearchPath)
	ON_BN_CLICKED(IDC_R_PATH0, OnRPath)
	ON_EN_CHANGE(IDC_DLL_PATH, OnChangeDllPath)
	ON_BN_CLICKED(IDC_R_PATH1, OnRPath)
	ON_BN_CLICKED(IDC_R_PATH2, OnRPath)
	ON_BN_CLICKED(IDC_R_PATH3, OnRPath)
	ON_BN_CLICKED(IDC_CK_DEBUG, OnCkDebug)
	ON_BN_CLICKED(IDC_CK_NO_DLLPATH, OnCkNoDllpath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPathDlg 

void CPathDlg::OnOK() 
{
   if (UpdateData(true))
   {
      SavePathInRegistry();

   	CDialog::OnOK();
   }	
}

void CPathDlg::OnSearchPath() 
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_DLL_PATH, m_strDllPath);
   if (pApp->GetFolderPath(m_strDllPath, NULL, FOLDERPATH_CONCAT_SLASH))
   {
      dx.m_bSaveAndValidate = false;
   	DDX_Text(&dx, IDC_DLL_PATH, m_strDllPath);
      m_bPathChanged = true;
   }
}

CPathDlg::~CPathDlg()
{
	if (m_hIcon != INVALID_HANDLE_VALUE)
      ::DestroyIcon(m_hIcon);
}

BOOL CPathDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon(m_hIcon, TRUE);		// Kleines Symbol verwenden
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPathDlg::OnRPath() 
{
   int nOldPathSelection = m_nPathSelection;
   CDataExchange DX(this, true);

   if (m_bPathChanged && (AfxMessageBox(IDS_SAVE_CHANGED_PATH, MB_YESNO|MB_ICONQUESTION, 0) ==IDYES))
   {
   	DDX_Text(&DX, IDC_DLL_PATH, m_strDllPath);
      SavePathInRegistry();
   }
	DDX_Radio(&DX, IDC_R_PATH0, m_nPathSelection);
   if (nOldPathSelection != m_nPathSelection)
   {
      CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
      switch (m_nPathSelection)
      {
         case 0: m_strDllPath = pApp->GetProfileString("", REGKEY_DLLPATH); break;
         case 1: m_strDllPath = pApp->GetProfileString("", REGKEY_DLLPATH_DEBUG); break;
         case 2: m_strDllPath = pApp->GetProfileString("", REGKEY_DLLPATH_RELEASE); break;
         case 3: m_strDllPath = pApp->GetProfileString("", REGKEY_DLLPATH_RELEASE_E); break;
      }
      DX.m_bSaveAndValidate = false;
	   DDX_Text(&DX, IDC_DLL_PATH, m_strDllPath);
      DDV_MaxChars(&DX, m_strDllPath, _MAX_PATH);
      m_bPathChanged = false;
   }
}

void CPathDlg::SavePathInRegistry()
{
   CETS3DGLDLGApp *pApp = (CETS3DGLDLGApp*) AfxGetApp();
   pApp->SetProfileName(NULL, ETSBIND_REGKEY);
   switch (m_nPathSelection)
   {
      case 0: pApp->WriteProfileString("", REGKEY_DLLPATH, m_strDllPath);           break;
      case 1: pApp->WriteProfileString("", REGKEY_DLLPATH_DEBUG, m_strDllPath);     break;
      case 2: pApp->WriteProfileString("", REGKEY_DLLPATH_RELEASE, m_strDllPath);   break;
      case 3: pApp->WriteProfileString("", REGKEY_DLLPATH_RELEASE_E, m_strDllPath); break;
   }
   pApp->WriteProfileInt("", REGKEY_USEDEBUGDLL, m_bUseDebugDll);
   pApp->WriteProfileInt("", REGKEY_USELOCALDLL, m_bUseLocalDll);
}

void CPathDlg::OnChangeDllPath() 
{
   m_bPathChanged = true;
}

void CPathDlg::OnCkDebug() 
{
   CDataExchange dx(this, true);
	DDX_Check(&dx, IDC_CK_DEBUG, m_bUseDebugDll);
   if (m_bUseDebugDll && m_bUseLocalDll)
   {
      dx.m_bSaveAndValidate = false;
      m_bUseLocalDll = false;
	   DDX_Check(&dx, IDC_CK_NO_DLLPATH, m_bUseLocalDll);
   }	
}

void CPathDlg::OnCkNoDllpath() 
{
   CDataExchange dx(this, true);
   DDX_Check(&dx, IDC_CK_NO_DLLPATH, m_bUseLocalDll);
   if (m_bUseDebugDll && m_bUseLocalDll)
   {
      dx.m_bSaveAndValidate = false;
      m_bUseDebugDll = false;
      DDX_Check(&dx, IDC_CK_DEBUG, m_bUseDebugDll);
   }	
}
