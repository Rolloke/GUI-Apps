// LogOn.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "LogOn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CLogOn 
#define PASSWORD    "upw"
#define PROFILE     "upf"
#define NEW_SESSION "NewSes"

CLogOn::CLogOn(CWnd* pParent /*=NULL*/)
	: CDialog(CLogOn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogOn)
	m_bNewSession = FALSE;
	//}}AFX_DATA_INIT
   CWinApp *pApp = AfxGetApp();

	m_bSavePassword = FALSE;
	m_strPassWord = pApp->GetProfileString(SETTINGS, PASSWORD);
   int i, nLen = m_strPassWord.GetLength();
   char *str = (char*)LPCTSTR(m_strPassWord);
   for (i=0; i<nLen; i++) str[i] ^= 0xcc;
	m_strProfile  = pApp->GetProfileString(SETTINGS, PROFILE);
   m_bNewSession = pApp->GetProfileInt(SETTINGS, NEW_SESSION, 0);
}


void CLogOn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogOn)
	DDX_Check(pDX, IDC_CK_LOG_ON_SAVE_PASSWORD, m_bSavePassword);
	DDX_Text(pDX, IDC_EDT_LOG_ON_PASSWORD, m_strPassWord);
	DDX_Text(pDX, IDC_EDT_LOG_ON_PROFILE, m_strProfile);
	DDX_Check(pDX, IDC_CK_LOG_ON_ENTER_NEW_SESSION, m_bNewSession);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogOn, CDialog)
	//{{AFX_MSG_MAP(CLogOn)
	ON_BN_CLICKED(IDC_LOGON_SAVE, OnLogonSave)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLogOn 

void CLogOn::OnLogonSave() 
{
   if (UpdateData())
   {
      CWinApp *pApp = AfxGetApp();
      if (m_bSavePassword)
      {
         int i, nLen = m_strPassWord.GetLength();
         char *str = (char*)LPCTSTR(m_strPassWord);
         for (i=0; i<nLen; i++) str[i] ^= 0xcc;
         pApp->WriteProfileString(SETTINGS, PASSWORD, m_strPassWord);
         for (i=0; i<nLen; i++) str[i] ^= 0xcc;
      }
      pApp->WriteProfileString(SETTINGS, PROFILE, m_strProfile);
      pApp->WriteProfileInt(SETTINGS, NEW_SESSION, m_bNewSession);
   }	
}

LRESULT CLogOn::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      ((CFacturaApp*)AfxGetApp())->WinHelp((ULONG)pHI, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }
   return true;
}
