// SendEmail.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "SendEmail.h"
#include "FindCustomer.h"
#include "FacturaDoc.h"

#include <mapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define COPY_FIELD       0
#define BLIND_COPY_FIELD 1
#define PHP_CONTENT_FILE 2

#define ADRESS_FROM  "AdrFrom"
#define NAME_FROM    "NameFrom"

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSendEmail 


CSendEmail::CSendEmail(CWnd* pParent /*=NULL*/)
	: CDialog(CSendEmail::IDD, pParent)
{
   CWinApp *pApp = AfxGetApp();
	//{{AFX_DATA_INIT(CSendEmail)
	m_strAdress          = _T("");
	m_strSubject         = _T("");
	m_strBody            = _T("");
	m_strAdressCopy      = _T("");
	m_strAdressBlindCopy = _T("");
	m_strName            = _T("");
	//}}AFX_DATA_INIT

   m_strNameFrom     = pApp->GetProfileString(SETTINGS, NAME_FROM, NULL);
	m_strAdressFrom   = pApp->GetProfileString(SETTINGS, ADRESS_FROM, NULL);
   m_nCurrAttachment = LB_ERR;
   m_nInsertTo       = 0;
}


void CSendEmail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendEmail)
	DDX_Control(pDX, IDC_COMBO_ATTTACHMENTS, m_cAttachments);
	DDX_Text(pDX, IDC_ADRESS_STRING, m_strAdress);
	DDX_Text(pDX, IDC_SUBJECT_STRING, m_strSubject);
	DDV_MaxChars(pDX, m_strSubject, 256);
	DDX_Text(pDX, IDC_BODY_STRING, m_strBody);
	DDX_Text(pDX, IDC_ADRESS_STRING2, m_strAdressCopy);
	DDX_Text(pDX, IDC_ADRESS_STRING3, m_strAdressBlindCopy);
	DDX_Text(pDX, IDC_NAME_STRING, m_strName);
	DDX_Text(pDX, IDC_NAME_STRING_FROM, m_strNameFrom);
	DDX_Text(pDX, IDC_ADRESS_STRING_FROM, m_strAdressFrom);
	//}}AFX_DATA_MAP
   if (pDX->m_bSaveAndValidate)
   {
      if (m_strAdress.IsEmpty())
      {
         AfxMessageBox(IDS_SENDMAIL_NO_ADRESS, MB_ICONSTOP);
         pDX->PrepareCtrl(IDC_ADRESS_STRING);
         pDX->Fail();
      }
      if (m_strSubject.IsEmpty())
      {
         if (AfxMessageBox(IDS_SENDMAIL_NO_SUBJECT, MB_YESNO|MB_ICONQUESTION)==IDNO)
         {
            pDX->PrepareCtrl(IDC_SUBJECT_STRING);
            pDX->Fail();
         }
      }
      if (m_strBody.IsEmpty())
      {
         AfxMessageBox(IDS_SENDMAIL_NO_BODY, MB_ICONSTOP);
         pDX->PrepareCtrl(IDC_BODY_STRING);
         pDX->Fail();
      }
   }
}


BEGIN_MESSAGE_MAP(CSendEmail, CDialog)
	//{{AFX_MSG_MAP(CSendEmail)
	ON_BN_CLICKED(IDC_INSERT_ADRESS, OnInsertAdress)
	ON_BN_CLICKED(IDC_SEND_DIRECT, OnSendDirect)
	ON_BN_CLICKED(IDC_BTN_DELETE_ATTACHMENT, OnBtnDeleteAttachment)
	ON_BN_CLICKED(IDC_BTN_INSERT_ATTACHMENT, OnBtnInsertAttachment)
	ON_CBN_SELCHANGE(IDC_COMBO_ATTTACHMENTS, OnSelchangeComboAtttachments)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_ADRESS_STRING2, OnChangeCopies)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_SEND_USER_DATA, OnEditUserData)
	ON_BN_CLICKED(IDC_SEND_SAVE_USER, OnSaveUser)
	ON_EN_CHANGE(IDC_ADRESS_STRING3, OnChangeCopies)
	ON_BN_CLICKED(IDC_INSERT_ADRESS2, OnInsertAdress2)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSendEmail 

BOOL CSendEmail::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   UpdateAttDeleteBtn();
	return TRUE;
}

void CSendEmail::OnInsertAdress() 
{
   CFindCustomer dlg;
   dlg.m_nMultiSelect = 100;
   if (dlg.DoModal() == IDOK)
   {
      int        nCount = dlg.m_nMultiSelect;
      Customer **ppList = dlg.GetCustomerList();
      CFile      File;
      if (m_nInsertTo == PHP_CONTENT_FILE)
      {
         CFileDialog dlg(false);
         if (dlg.DoModal() == IDOK)
         {
            if (File.Open(dlg.GetPathName(), CFile::modeCreate|CFile::modeWrite, NULL) == 0) return;
         }
      }

      if (ppList)
      {
         int nInserted = 0;
         for (int i=0; i<nCount; i++)
         {
            if (ppList[i]->szEmail[0] != 0)
            {
               nInserted++;
               switch (m_nInsertTo)
               {
                  case COPY_FIELD:
                     if (!m_strAdressCopy.IsEmpty()) m_strAdressCopy += _T(";");
                     m_strAdressCopy += ppList[i]->szEmail;
                     break;
                  case BLIND_COPY_FIELD:
                     if (!m_strAdressBlindCopy.IsEmpty()) m_strAdressBlindCopy += _T(";");
                     m_strAdressBlindCopy += ppList[i]->szEmail;
                     break;
                  case PHP_CONTENT_FILE:
                  {
                     CString str;
                     str.Format("#%s#%s#%s#%s#%c#\r\n", ppList[i]->szEmail, ppList[i]->szFirstName, ppList[i]->szName, ppList[i]->szTitle, (ppList[i]->nFlags & CUSTOMER_FEMALE)? 'f' : 'm');
                     File.Write(LPCTSTR(str), str.GetLength());
                  }   break;
               }
            }
         }
         CString str;
         str.Format("Eingefügte Adressen : %d", nInserted);
         AfxMessageBox(str, MB_OK, 0);
         UpdateData(false);
      }
   }	
}

void CSendEmail::OnCancel() 
{
	CDialog::OnCancel();
}

void CSendEmail::OnOK() 
{
	CDialog::OnOK();
   if (AfxMessageBox(IDS_SEND_MAIL_NOW, MB_ICONQUESTION|MB_YESNO)==IDYES)
   {
      CString strFormat = _T("mailto:%s?subject=%s&body=%s");
      CString str;
      str.Format(strFormat, m_strAdress, m_strSubject, m_strBody);
      HINSTANCE nResult = ::ShellExecute(m_hWnd, "open", str, NULL, NULL, SW_SHOW);
   //   HINSTANCE nResult = ::ShellExecute(m_hWnd, "open", "mailto:rolf@cara.de?subject=Na Du&body=Dies ist der Körper", NULL, NULL, SW_SHOW);
   }
}

void CSendEmail::OnSendDirect() 
{
   if (UpdateData())
   {
      HANDLE         hMapi        = NULL;
      LPMAPILOGON    pfnLogOn     = NULL;
      LPMAPISENDMAIL pfnSendMail  = NULL;
      LPMAPILOGOFF   pfnLogOff    = NULL;
      LHANDLE        lhSession    = NULL;
      MapiFileDesc  *pAttachments = NULL;
      int            nAttachments = 0;
      try
      {
         ULONG   lResult      = 0;
         int     nReceiver    = 1;

         hMapi = LoadLibrary("MAPI32.DLL");                    // Dll laden
         if (hMapi < (HANDLE)32) throw (int) -1;               // Funktion laden
         pfnLogOn    = (LPMAPILOGON)GetProcAddress(   (HINSTANCE)hMapi, "MAPILogon");
         if (!pfnLogOn   ) throw (int) -2;
         pfnSendMail = (LPMAPISENDMAIL)GetProcAddress((HINSTANCE)hMapi, "MAPISendMail");
         if (!pfnSendMail) throw (int) -2;
         pfnLogOff   = (LPMAPILOGOFF)GetProcAddress(  (HINSTANCE)hMapi, "MAPILogoff");
         if (!pfnLogOff  ) throw (int) -2;
         MapiRecipDesc recipient[4] =                         // Empfängerstruktur
         {
            {                                                 // r 0
               0, MAPI_ORIG,                                  // MailTo:
               (char*)LPCTSTR(m_strNameFrom),                 // Name
               (char*)LPCTSTR(m_strAdressFrom),               // E-Mail Adresse
               0, NULL
            },
            {                                                 // r 1
               0, MAPI_TO,                                    // MailTo:
               (char*)LPCTSTR(m_strName),                     // Name
               (char*)LPCTSTR(m_strAdress),                   // E-Mail Adresse
               0, NULL
            },
            {                                                 // r 2
               0, MAPI_CC,                                    // Msg Copy
               (char*)"",                                     // Name
               (char*)LPCTSTR(m_strAdressCopy),               // E-Mail Adressen
               0, NULL
            },
            {                                                 // r 3
               0, MAPI_BCC,                                   // Msg Blind Copy
               (char*)"",                                     // Name
               (char*)LPCTSTR(m_strAdressBlindCopy),          // E-Mail Adressen
               0, NULL
            }
         };
         nAttachments = m_cAttachments.GetCount();
         if (nAttachments > 0)
         {
            pAttachments = new MapiFileDesc[nAttachments];
            for (int i=0; i<nAttachments; i++)
            {
               pAttachments[i].ulReserved = 0;
               pAttachments[i].flFlags    = 0;
               pAttachments[i].nPosition  = 0xFFFFFFFF;
               int nLen = m_cAttachments.GetLBTextLen(i);
               if (nLen>0)
               {
                  pAttachments[i].lpszPathName = (char*)malloc(nLen+1);
                  m_cAttachments.GetLBText(i, pAttachments[i].lpszPathName);
                  pAttachments[i].lpszPathName[nLen] = 0;
                  int j;
                  for (j=nLen-1; j>=0; j--)
                     if (pAttachments[i].lpszPathName[j]=='\\')break;
                  pAttachments[i].lpszFileName = &pAttachments[i].lpszPathName[j+1];
               }
               else
               {
                  pAttachments[i].lpszPathName = NULL;
                  pAttachments[i].lpszFileName = NULL;
               }
               pAttachments[i].lpFileType = NULL;
            }
         }
         else nAttachments = 0;
         if (!m_strAdressCopy.IsEmpty()) nReceiver++;
         if (!m_strAdressBlindCopy.IsEmpty())
         {
            if (m_strAdressCopy.IsEmpty()) recipient[2] = recipient[3];
            nReceiver++;
         }
         MapiMessage message =                                 // Messagestruktur
         {
            0,
            (char*)LPCTSTR(m_strSubject),                      // Betreff:
            (char*)LPCTSTR(m_strBody),                         // Text
            NULL,                                              // MessageTyp
            NULL,                                              // Date when Received
            NULL,                                              // ConversationID
            MAPI_RECEIPT_REQUESTED|MAPI_UNREAD,                // Flags
            recipient,                                         // Sender            
            nReceiver, &recipient[1],                          // Empfänger
            nAttachments, pAttachments                         // Attachments
         };
         if (!m_LogOn.m_strPassWord.IsEmpty() && !m_LogOn.m_strProfile.IsEmpty())
         {                                                     // Einlogen
            UINT nFlags = MAPI_LOGON_UI;
            if (m_LogOn.m_bNewSession) nFlags |= MAPI_NEW_SESSION;
            lResult = (*pfnLogOn)((ULONG)m_hWnd, (char*)LPCTSTR(m_LogOn.m_strProfile), (char*)LPCTSTR(m_LogOn.m_strPassWord), nFlags, 0, &lhSession);
         }
         else 
         {
            lResult = (*pfnLogOn)((ULONG)m_hWnd, NULL, NULL, 0, 0, &lhSession);
         }
         if (SUCCESS_SUCCESS != lResult)
         {
            lhSession = NULL;                                  // Fehler: Ende
            throw (int) lResult;
         }
                                                               // Senden
         lResult = (*pfnSendMail)(lhSession, (ULONG)m_hWnd, &message, 0, 0);
         if (SUCCESS_SUCCESS != lResult) throw (int) lResult;

         lResult = (*pfnLogOff)(lhSession, (ULONG)m_hWnd, 0, 0);// Ausloggen
         lhSession = NULL;
         if (SUCCESS_SUCCESS != lResult) throw (int) lResult;
      }
      catch(int nError)
      {
         CString str;
         str.Format("MAPI Error %d", nError);
         AfxMessageBox(str, MB_OK|MB_ICONEXCLAMATION);
      }
      if (lhSession)
      {
         (*pfnLogOff)(lhSession, 0, 0, 0);
      }
      if (hMapi) ::FreeLibrary((HMODULE)hMapi);
      if (pAttachments)
      {
         for (int i=0; i<nAttachments; i++)
         {
            if (pAttachments[i].lpszPathName) free(pAttachments[i].lpszPathName);
         }
         delete [] pAttachments;
      }
   }
}

void CSendEmail::OnBtnDeleteAttachment() 
{
   if (m_nCurrAttachment != LB_ERR)
   {
      m_cAttachments.DeleteString(m_nCurrAttachment);
      m_nCurrAttachment = m_cAttachments.SetCurSel(0);
      UpdateAttDeleteBtn();
   }
}

void CSendEmail::OnBtnInsertAttachment() 
{
	CFileDialog dlg(true);
   if (dlg.DoModal() == IDOK)
   {
      m_cAttachments.AddString(dlg.GetPathName());
      m_nCurrAttachment = m_cAttachments.SetCurSel(m_cAttachments.GetCount()-1);
      UpdateAttDeleteBtn();
   }	
}

void CSendEmail::OnSelchangeComboAtttachments() 
{
   m_nCurrAttachment = m_cAttachments.GetCurSel();
   UpdateAttDeleteBtn();
}

HBRUSH CSendEmail::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
   if (GetWindowLong(pWnd->m_hWnd, GWL_ID)==IDC_BTN_DELETE_ATTACHMENT)
   {
      pDC->SetTextColor(RGB(255, 0,0));
      hbr = ::GetSysColorBrush(COLOR_INFOBK);
   }
	return hbr;
}

void CSendEmail::UpdateAttDeleteBtn()
{
   GetDlgItem(IDC_BTN_DELETE_ATTACHMENT)->EnableWindow((m_nCurrAttachment == LB_ERR) ? false : true);
}


void CSendEmail::OnChangeCopies() 
{
	UpdateOKBtn();
}

void CSendEmail::UpdateOKBtn()
{
   bool bOk = true;
   if (m_cAttachments.GetCount() > 0  ) bOk = false;
   if (!m_strAdressCopy.IsEmpty()     ) bOk = false;
   if (!m_strAdressBlindCopy.IsEmpty()) bOk = false;
   GetDlgItem(IDOK)->EnableWindow(bOk);
}


void CSendEmail::OnContextMenu(CWnd* pWnd, CPoint point) 
{
   CWnd *pInsertAdress = GetDlgItem(IDC_INSERT_ADRESS);
   CRect rcWnd;
   if (pInsertAdress)
   {
      pInsertAdress->GetWindowRect(&rcWnd);
      CPoint pt = point;
      if (rcWnd.PtInRect(pt))
      {
         CMenu Menu;
         Menu.LoadMenu(IDR_SENDM_INSERT_ADR);
         CMenu * pContext = Menu.GetSubMenu(0);
         if (pContext)
         {  
            if (!pWnd) pWnd = this;
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, pWnd))
            {
               case ID_INSERT_TO_BLIND_COPY:
               {
                  m_nInsertTo = BLIND_COPY_FIELD;
                  OnInsertAdress();
                  m_nInsertTo = 0;
               } break;
               case ID_INSERT_TO_PHP_CONTENT_FILE:
               {
                  m_nInsertTo = PHP_CONTENT_FILE;
                  OnInsertAdress();
                  m_nInsertTo = 0;
               } break;
               case IDCANCEL: default:break;
            }
         }
      }
   }
}


void CSendEmail::OnEditUserData() 
{
   m_LogOn.DoModal();	
}

void CSendEmail::OnSaveUser() 
{
   CWinApp *pApp = AfxGetApp();
   CDataExchange dx(this, true);
	DDX_Text(&dx, IDC_NAME_STRING_FROM, m_strNameFrom);
	DDX_Text(&dx, IDC_ADRESS_STRING_FROM, m_strAdressFrom);
   pApp->WriteProfileString(SETTINGS, ADRESS_FROM, m_strAdressFrom);
   pApp->WriteProfileString(SETTINGS, NAME_FROM  , m_strNameFrom);
}

void CSendEmail::OnInsertAdress2() 
{
   m_nInsertTo = BLIND_COPY_FIELD;
   OnInsertAdress();
   m_nInsertTo = 0;
}

LRESULT CSendEmail::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
