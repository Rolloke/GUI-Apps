// RegisterDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "RegisterDlg.h"

#ifdef _REGISTER_BUILD

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define REGISTER         "Reg"
#define REGISTER_NAME    "Name"
#define REGISTER_COMPANY "Company"
#define REGISTER_STREET  "Street"
#define REGISTER_CITY    "City"
#define REGISTER_RGNO    "RegNo"
#define REGISTER_BIN     "Bin"

#define REG_UNINSTALL    "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CRegisterDlg 


CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent)
{
   char szName[MAX_PATH];
   DWORD    dwSize = MAX_PATH;
   CWinApp *pApp   = AfxGetApp();
   BYTE    *pBin   = NULL;
   UINT  i, nBytes = 0;

	//{{AFX_DATA_INIT(CRegisterDlg)
	m_bRegister = FALSE;
	//}}AFX_DATA_INIT

   HKEY  hKey          = 0;
   if ((::RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_UNINSTALL, 0, KEY_QUERY_VALUE|KEY_READ|KEY_WRITE, &hKey) == ERROR_SUCCESS) &&
       (hKey != NULL))
   {
      DWORD dwType = REG_DWORD, dwLength = sizeof(DWORD);
      strcpy(szName, pApp->m_pszAppName);
      nBytes = strlen(szName);
      for (i=0; i<nBytes; i++)
      {
         szName[i] += 3;
      }

      if ((::RegQueryValueEx(hKey, szName, NULL, &dwType, (BYTE*)&m_FirstTime, &dwLength) == ERROR_SUCCESS) && 
          (dwType == REG_DWORD))
      {

      }
      else
      {
         dwType = REG_DWORD;
         m_FirstTime = CTime::GetCurrentTime();
         RegSetValueEx(hKey, szName, 0, REG_DWORD, (BYTE*)&m_FirstTime, sizeof(DWORD));
      }
   }

   if (hKey) ::RegCloseKey(hKey);

   m_strName    = pApp->GetProfileString(REGISTER, REGISTER_NAME);
	m_strCompany = pApp->GetProfileString(REGISTER, REGISTER_COMPANY);
	m_strStreet  = pApp->GetProfileString(REGISTER, REGISTER_STREET);
	m_strCity    = pApp->GetProfileString(REGISTER, REGISTER_CITY);

	m_strRegNo   = pApp->GetProfileString(REGISTER, REGISTER_RGNO);
   m_nNum       = 0x12345678;
   pApp->GetProfileBinary(REGISTER, REGISTER_BIN, &pBin, &nBytes); 
   if (pBin)
   {
      char *pStr = (char*)LPCTSTR(pBin);
      for (i=0; i<nBytes; i++)
      {
         if (i&1) m_nNum *= pStr[i];
         else     m_nNum += pStr[i];
         pStr[i] = (pStr[i] ^ 0xd7) - 1;
      }
      delete pBin;
   }

   m_hTitleFont  = NULL;
   m_hPleaseFont = NULL;

   GetComputerName(szName, &dwSize);
   m_strComputer = _T(szName);

   dwSize = MAX_PATH;
   GetUserName(szName, &dwSize);
   m_strUser = _T(szName);
}

CRegisterDlg::~CRegisterDlg()
{
   if (m_hTitleFont)  ::DeleteObject(m_hTitleFont);
   if (m_hPleaseFont) ::DeleteObject(m_hPleaseFont);
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Text(pDX, IDC_REG_NAME_EDT, m_strName);
	DDX_Text(pDX, IDC_REG_REG_NO_EDT, m_strRegNo);
	DDX_Text(pDX, IDC_REG_STREET_EDT, m_strStreet);
	DDX_Text(pDX, IDC_REG_COMPANY_EDT, m_strCompany);
	DDX_Text(pDX, IDC_REG_CITTY_EDT, m_strCity);
	//}}AFX_DATA_MAP
   if (pDX->m_bSaveAndValidate && m_bRegister)
   {
      if (m_strName.IsEmpty() || m_strStreet.IsEmpty() || m_strCity.IsEmpty())
      {
         AfxMessageBox(IDS_REG_ERROR);
         pDX->Fail();
      }
   }
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_REG_BTN_REGISTER, OnRegBtnRegister)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CRegisterDlg 

BOOL CRegisterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   HFONT hFont = (HFONT)SendMessage(WM_GETFONT);
   LOGFONT lf;

   ::GetObject(hFont, sizeof(LOGFONT), &lf);
   lf.lfWeight   = FW_BOLD;
   m_hPleaseFont = ::CreateFontIndirect(&lf);
   lf.lfHeight   = -30;
   m_hTitleFont  = ::CreateFontIndirect(&lf);

   GetDlgItem(IDC_REG_TITLE)->SendMessage(WM_SETFONT, (WPARAM)m_hTitleFont, 0);
   GetDlgItem(IDC_REG_PLEASE_REG)->SendMessage(WM_SETFONT, (WPARAM)m_hPleaseFont, 0);
   GetDlgItem(IDC_REG_REG_NO_TXT)->SendMessage(WM_SETFONT, (WPARAM)m_hPleaseFont, 0);

   GetDlgItem(IDOK)->EnableWindow(false);
   CTimeSpan tSpan = CTime::GetCurrentTime() - m_FirstTime;
   long nDays = tSpan.GetDays();
   if (nDays < 28)  SetTimer(100, 1000, NULL);
   else
   {
      nDays -= 27;
      if (nDays > 15) nDays = 15;
      nDays *= 1000;
      SetTimer(100, nDays, NULL);
   }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CRegisterDlg::OnTimer(UINT nIDEvent) 
{
   if (nIDEvent == 100)
   {
	   CWnd *pWndOk = GetDlgItem(IDOK);
      if (pWndOk)
      {
         pWndOk->EnableWindow(true);
         KillTimer(100);
      }
   }
	
	CDialog::OnTimer(nIDEvent);
}

void CRegisterDlg::OnCancel() 
{
//	CDialog::OnCancel();
}

void CRegisterDlg::OnRegBtnRegister() 
{
   CDataExchange	dx(this, true);
	DDX_Check(&dx, IDC_REG_BTN_REGISTER, m_bRegister);
   int i;
   CWnd *pWnd;
   for (i=IDC_REG_PLEASE_REG; i<=IDC_REG_DESCRIPTION5; i++)
   {
      pWnd = GetDlgItem(i);
      if (pWnd) pWnd->ShowWindow((m_bRegister != 0) ? SW_HIDE : SW_SHOW);
   }

   for (i=IDC_REG_NAME_TXT; i<=IDC_REG_CITTY_EDT; i++)
   {
      pWnd = GetDlgItem(i);
      if (pWnd) pWnd->ShowWindow((m_bRegister != 0) ? SW_SHOW : SW_HIDE);
   }
}

void CRegisterDlg::OnOK() 
{
   CWinApp *pApp  = AfxGetApp();
   bool bTest = false;
   if (pApp->GetProfileInt(REGISTER, "GetTheKey", 0) == 0x0815)
   {
      if (!m_bRegister) CDialog::OnOK();
      else
      {
         if (UpdateData(true)) bTest = true;
      }
   }
   else CDialog::OnOK();
   if (m_bRegister)
   {
      CWinApp *pApp  = AfxGetApp();
      CString strReg = m_FirstTime.Format("%d.%m.%y/%H:%M");
      CString strComma = _T(", ");
      strReg += strComma + m_strName + strComma + m_strStreet + strComma + m_strCompany + strComma + m_strCity + strComma + m_strComputer + strComma + m_strUser;

      int i, nLen = strReg.GetLength();                        // Verschlüsseln
      char *pStr = (char*)LPCTSTR(strReg);
      m_nNum      = 0x12345678;
      for (i=0; i<nLen; i++)
      {
         pStr[i] = (pStr[i] + 1) ^ 0xd7;
         if (i&1) m_nNum *= pStr[i];
         else     m_nNum += pStr[i];
      }
      if (m_strRegNo.IsEmpty())
      {                                                        // Absenden
         CString str = _T("mailto:info@cara.de?subject=Registrierung ETS2DV&body=Registrierdaten:");
         str += strReg;
         ::ShellExecute(m_hWnd, "open", str, NULL, NULL, SW_SHOW);
      }

      // Daten in die Registry schreiben
	   pApp->WriteProfileString(REGISTER, REGISTER_NAME   , m_strName);
	   pApp->WriteProfileString(REGISTER, REGISTER_COMPANY, m_strCompany);
	   pApp->WriteProfileString(REGISTER, REGISTER_STREET , m_strStreet);
	   pApp->WriteProfileString(REGISTER, REGISTER_CITY   , m_strCity);
	   pApp->WriteProfileBinary(REGISTER, REGISTER_BIN    , (BYTE*)pStr, nLen);

	   pApp->WriteProfileString(REGISTER, REGISTER_RGNO   , m_strRegNo);
      if (bTest)
      {
         m_strRegNo.Format("%0X", m_nNum);
         UpdateData(false);
      }
   }
}

bool CRegisterDlg::IsRegistered()
{
   if (!m_strRegNo.IsEmpty())
   {
      CString str;
      str.Format("%0X", m_nNum);
      return (m_strRegNo == str) ? true : false;
   }
   return false;
}
#endif // _REGISTER_BUILD
