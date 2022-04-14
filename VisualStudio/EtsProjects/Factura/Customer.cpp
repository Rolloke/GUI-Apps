// Customer.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "Customer.h"
#include "FacturaDoc.h"
#include "FindCustomer.h"
#include "ListDlg.h"
#include "SendEmail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCustomer 


CCustomer::CCustomer(CWnd* pParent /*=NULL*/)
	: CDialog(CCustomer::IDD, pParent)
{
   InitCustomerData();
   m_pCustomers = NULL;
   CWnd *pWnd   = AfxGetMainWnd();
   if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
   {
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pWnd)->GetActiveDocument();
      if (pDoc) m_pCustomers = &pDoc->m_Customers;
   }
   m_nMaxLen = AfxGetApp()->GetProfileInt(LABEL_SETTINGS, LABEL_LINE_LENGTH, 31);
}

void CCustomer::InitCustomerData()
{
   //{{AFX_DATA_INIT(CCustomer)
	//}}AFX_DATA_INIT
   m_strCity.Empty();
   m_strEmail.Empty();
   m_strFax.Empty();
   m_strStreetNo.Empty();
   m_strName.Empty();
   m_strPhone.Empty();
   m_strPostCode.Empty();
   m_strFirstName.Empty();
   m_strState.Empty();
   m_strStreet.Empty();
   m_strTitle.Empty();
   m_strComment.Empty();
   m_strCompany.Empty();
   m_nNo            =  0;
   m_nCustomerSex   =  0;
   m_nIndex         = -1;
   m_nCustomerFlags =  0;
   m_bChanged       = false;
}

void CCustomer::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCustomer)
   DDX_Text(   pDX, IDC_CUSTOMER_NO  , m_nNo);
   DDX_Radio(  pDX, IDC_CUSTOMER_MALE, m_nCustomerSex);
   DDX_Control(pDX, IDC_CUSTOMER_SPIN, m_CustomerSpin);
	//}}AFX_DATA_MAP

   DDX_Text(pDX, IDC_CUSTOMER_CITY,     m_strCity);
//   DDV_MaxChars(pDX, m_strCity, CITY_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_EMAIL,    m_strEmail);
//   DDV_MaxChars(pDX, m_strEmail, EMAIL_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_FAX,      m_strFax);
//   DDV_MaxChars(pDX, m_strFax, FAX_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_HOUSE_NO, m_strStreetNo);
//   DDV_MaxChars(pDX, m_strStreetNo, STREETNO_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_NAME,     m_strName);
//   DDV_MaxChars(pDX, m_strName, NAME_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_PHONE,    m_strPhone);
//   DDV_MaxChars(pDX, m_strPhone, PHONE_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_PLZ,      m_strPostCode);
//   DDV_MaxChars(pDX, m_strPostCode, PC_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_FIRSTNAME,  m_strFirstName);
//   DDV_MaxChars(pDX, m_strFirstName, FIRSTNAME_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_STATE,    m_strState);
//   DDV_MaxChars(pDX, m_strState, STATE_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_STREET,   m_strStreet);
//   DDV_MaxChars(pDX, m_strStreet, STREET_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_TITLE,    m_strTitle);
//   DDV_MaxChars(pDX, m_strTitle, TITLE_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_COMMENT, m_strComment);
//   DDV_MaxChars(pDX, m_strComment, COMMENT_LENGTH);
   DDX_Text(pDX, IDC_CUSTOMER_COMPANY, m_strCompany);
//   DDV_MaxChars(pDX, m_strCompany, COMPANY_LENGTH);
}


BEGIN_MESSAGE_MAP(CCustomer, CDialog)
   //{{AFX_MSG_MAP(CCustomer)
   ON_BN_CLICKED(IDC_CUSTOMER_FIND, OnCustomerFind)
   ON_BN_CLICKED(IDC_CUSTOMER_NEW, OnCustomerNew)
   ON_EN_CHANGE(IDC_CUSTOMER_CITY, OnChangeCustomer)
   ON_BN_CLICKED(IDC_CUSTOMER_DELETE, OnCustomerDelete)
   ON_NOTIFY(UDN_DELTAPOS, IDC_CUSTOMER_SPIN, OnDeltaposCustomerSpin)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_CONTEXTMENU()
	ON_EN_KILLFOCUS(IDC_CUSTOMER_PLZ, OnKillfocusCustomerPlz)
	ON_BN_CLICKED(IDC_CUSTOMER_FLAGS, OnCustomerFlags)
	ON_BN_CLICKED(IDC_CUSTOMER_SENDEMAIL, OnCustomerSendemail)
	ON_BN_CLICKED(IDC_CUSTOMER_PRINT_ADR_KL, OnCustomerPrintAdrKl)
	ON_BN_CLICKED(IDC_CUSTOMER_IMPORT, OnCustomerImport)
   ON_EN_CHANGE(IDC_CUSTOMER_EMAIL     , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_FAX       , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_HOUSE_NO  , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_NAME      , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_NO        , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_PHONE     , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_PLZ       , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_FIRSTNAME , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_STATE     , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_STREET    , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_TITLE     , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_COMMENT   , OnChangeCustomer)
   ON_EN_CHANGE(IDC_CUSTOMER_COMPANY   , OnChangeCustomer)
   ON_BN_CLICKED(IDC_CUSTOMER_MALE     , OnChangeCustomer)
   ON_BN_CLICKED(IDC_CUSTOMER_FEMALE   , OnChangeCustomer)
	ON_WM_CTLCOLOR()
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCustomer 

void CCustomer::OnCustomerFind() 
{
   if (m_hWnd) UpdateData(true);
   CFindCustomer FindCustomer;
   FindCustomer.m_bExternalSearch = true;
   if (!m_strName.IsEmpty())
   {
      FindCustomer.m_strSearchFor    = m_strName;
      FindCustomer.m_nColumn         = 1;
   }
   else if (!m_strCity.IsEmpty())
   {
      FindCustomer.m_strSearchFor    = m_strCity;
      FindCustomer.m_nColumn         = 4;
   }
   else if (!m_strCompany.IsEmpty())
   {
      FindCustomer.m_strSearchFor    = m_strCompany;
      FindCustomer.m_nColumn         = 6;
   }
   else
   {
      Customer *pCustomer = GetCustomer();
      if (pCustomer) FindCustomer.m_nItem = pCustomer->nNo;
      FindCustomer.m_bExternalSearch = false;
   }
   SaveChangedData();

   FindCustomer.m_nSearchStrLen = FindCustomer.m_strSearchFor.GetLength();
   if (FindCustomer.DoModal() == IDOK)
   {
      m_nIndex = FindCustomer.m_nItem;
      if (GetCustomerData() && UpdateData(false))
      {
         m_bChanged = false;
         GetDlgItem(IDC_CUSTOMER_DELETE)->EnableWindow(true);
      }
   }
}

Customer * CCustomer::GetCustomer()
{
   if (m_nIndex == -1)  return false;
   if (!m_pCustomers)   return false;
   POSITION pos = m_pCustomers->FindIndex(m_nIndex);
   if (pos==NULL)       return false;
   return (Customer*) m_pCustomers->GetAt(pos);
}
bool CCustomer::GetCustomerData()
{
   Customer *pCustomer = GetCustomer();
   if (pCustomer==NULL) return false;
   m_strCity      = _T(pCustomer->szCity);
   m_strEmail     = _T(pCustomer->szEmail);
   m_strFax       = _T(pCustomer->szFaxNo);
   m_strStreetNo  = _T(pCustomer->szStreetNo);
   m_strName      = _T(pCustomer->szName);
   m_strPhone     = _T(pCustomer->szPhoneNo);
   m_strPostCode  = _T(pCustomer->szPostalCode);
   m_strFirstName = _T(pCustomer->szFirstName);
   m_strState     = _T(pCustomer->szState);
   m_strStreet    = _T(pCustomer->szStreet);
   m_strTitle     = _T(pCustomer->szTitle);
   m_strComment   = _T(pCustomer->szComment);
   m_strCompany   = _T(pCustomer->szCompany);
   m_nNo            = pCustomer->nNo;
   m_nCustomerFlags = pCustomer->nFlags;
   m_nCustomerSex     = (m_nCustomerFlags & CUSTOMER_FEMALE)       ? 1    : 0;
   return true;
}

void CCustomer::OnCustomerDelete() 
{
   if ((m_nIndex != -1) && (m_pCustomers))
   {
      POSITION pos = m_pCustomers->FindIndex(m_nIndex);
      if (pos)
      {
         Customer *pCustomer =(Customer*) m_pCustomers->GetAt(pos);
         if (pCustomer)
         {
            m_pCustomers->RemoveAt(pos);
            delete pCustomer;
            SetDocChanged();
            OnCustomerNew();
         }
      }
   }
}

void CCustomer::SaveChangedData()
{
   if (m_bChanged && (m_hWnd != NULL) && UpdateData(true))
   {
      if (AfxMessageBox(IDS_SAVE_CUSTOMER, MB_YESNO) == IDYES)
      {
         if (SaveNewCustomer()) m_bChanged = false;
      }
   }
}

void CCustomer::OnCustomerNew() 
{
   SaveChangedData();
   InitCustomerData();
   if (m_pCustomers && m_pCustomers->GetCount())
   {
      Customer *pCustomer =(Customer*) m_pCustomers->GetTail();
      if (pCustomer) m_nNo = pCustomer->nNo+1;
      else ASSERT(FALSE);
   }
   else m_nNo = 1;

   if (m_hWnd) UpdateData(false);
}

void CCustomer::OnCancel() 
{
   CDialog::OnCancel();
}

void CCustomer::OnOK() 
{
   CWnd *pOK    = GetDlgItem(IDOK);
   CWnd *pFocus = GetFocus();
   if (pOK == pFocus)
   {
      if (UpdateData(true))
      {
         SaveNewCustomer();
         pOK->EnableWindow(false);	
      }
      CDialog::OnOK();
   }
   else if (pFocus)
   {
      pFocus = GetNextDlgTabItem(pFocus);
      if (pFocus) pFocus->SetFocus();
   }
}

bool CCustomer::SaveNewCustomer()
{
   if (m_strCity.IsEmpty() &&
       m_strEmail.IsEmpty() &&
       m_strFax.IsEmpty() &&
       m_strStreetNo.IsEmpty() &&
       m_strName.IsEmpty() &&
       m_strPhone.IsEmpty() &&
       m_strPostCode.IsEmpty() &&
       m_strFirstName.IsEmpty() &&
       m_strState.IsEmpty() &&
       m_strStreet.IsEmpty() &&
       m_strTitle.IsEmpty() &&
       m_strComment.IsEmpty() &&
       m_strCompany.IsEmpty())
   {
      return false;
   }
   if ((m_nIndex == -1) && (m_pCustomers != NULL))
   {
      Customer *pCustomer = new Customer;
      if (pCustomer)
      {
         POSITION pos = m_pCustomers->AddTail(pCustomer);
         if (pos == NULL)
         {
            if (pCustomer) delete pCustomer;
            pCustomer = NULL;
         }
         else
         {
            m_nIndex = m_pCustomers->GetCount()-1;
            SaveCustomer(pCustomer);
            return true;
         }
      }
      m_nIndex = -1;
   }
   else if (m_pCustomers)
   {
      POSITION pos = m_pCustomers->FindIndex(m_nIndex);
      if (pos==NULL)       return false;
      Customer *pCustomer =(Customer*) m_pCustomers->GetAt(pos);
      if (pCustomer)
      {
         SaveCustomer(pCustomer);
         return true;
      }
      else m_nIndex = -1;
   }
   return false;
}

void CCustomer::SaveCustomer(Customer * pCustomer)
{
   pCustomer->nNo    = m_nNo;
   pCustomer->nFlags = m_nCustomerFlags;
   if (m_nCustomerSex == 1) pCustomer->nFlags |=  CUSTOMER_FEMALE;
   else                     pCustomer->nFlags &= ~CUSTOMER_FEMALE;

   pCustomer->FreeAllText();
   if (!m_strCity.IsEmpty())     pCustomer->szCity      = _tcsdup(m_strCity);
   if (!m_strEmail.IsEmpty())    pCustomer->szEmail     = _tcsdup(m_strEmail);
   if (!m_strFax.IsEmpty())      pCustomer->szFaxNo     = _tcsdup(m_strFax);
   if (!m_strStreetNo.IsEmpty()) pCustomer->szStreetNo  = _tcsdup(m_strStreetNo);
   if (!m_strName.IsEmpty())     pCustomer->szName      = _tcsdup(m_strName);
   if (!m_strPhone.IsEmpty())    pCustomer->szPhoneNo   = _tcsdup(m_strPhone);
   if (!m_strPostCode.IsEmpty()) pCustomer->szPostalCode= _tcsdup(m_strPostCode);
   if (!m_strFirstName.IsEmpty())pCustomer->szFirstName = _tcsdup(m_strFirstName);
   if (!m_strState.IsEmpty())    pCustomer->szState     = _tcsdup(m_strState);
   if (!m_strStreet.IsEmpty())   pCustomer->szStreet    = _tcsdup(m_strStreet);
   if (!m_strTitle.IsEmpty())    pCustomer->szTitle     = _tcsdup(m_strTitle);
   if (!m_strComment.IsEmpty())  pCustomer->szComment   = _tcsdup(m_strComment);
   if (!m_strCompany.IsEmpty())  pCustomer->szCompany   = _tcsdup(m_strCompany);

   SetDocChanged();
}
BOOL CCustomer::OnInitDialog() 
{
   CDialog::OnInitDialog();
	
   m_CustomerSpin.SetRange(0, m_pCustomers->GetCount()-1);

   if (GetCustomerData()) UpdateData(false);
   else OnCustomerNew();

   GetDlgItem(IDC_CUSTOMER_DELETE)->EnableWindow(m_nIndex != -1);

   if ((m_nIndex != -1) && GetCustomerData() && UpdateData(false))
   {
      GetDlgItem(IDC_CUSTOMER_DELETE)->EnableWindow(true);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CCustomer::OnChangeCustomer() 
{
   m_bChanged = true;
}

void CCustomer::SetDocChanged()
{
   CWinApp *pApp = AfxGetApp();
   if (pApp && pApp->m_pMainWnd)
   {
      CDocument* pDoc = ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (pDoc) pDoc->SetModifiedFlag(true);
   }
}

void CCustomer::OnDeltaposCustomerSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   SaveChangedData();
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   int nPos   = m_nIndex + pNMUpDown->iDelta;
   int nCount = m_pCustomers->GetCount();
   if ((nPos >= 0) && (nPos < nCount)) m_nIndex = nPos;
   else if (nPos >= nCount)            m_nIndex = 0;
   else                                m_nIndex = m_pCustomers->GetCount()-1;

   if (GetCustomerData() && UpdateData(false))
   {
      m_bChanged = false;
      GetDlgItem(IDC_CUSTOMER_DELETE)->EnableWindow(true);
   }

   *pResult = 0;
}


void CCustomer::OnEditPaste() 
{
   if (::OpenClipboard(this->m_hWnd))
   {
      char *pszText = (char*) ::GetClipboardData(CF_TEXT);
      if (pszText)
      {
         if (MessageBox(pszText, "Adresse einfügen ?", MB_YESNO) == IDYES)
         {
            int i = 0;  //      1 ,  2 ,  3 ,  4 ,  5 ,  6    7
            char *pszMask[] = {" ", ", ", " ", ", ", " ", ", ", " "};
            char *pszOld = pszText;
            bool bContinue = false;
            while ((pszText = strstr(pszOld, pszMask[i++])) != NULL)
            {
               if (((i&1) != 0) && (pszText[-1] == ','))
               {
                  bContinue   = true;
                  pszText[-1] = 0;
               }
               pszText[0] = 0;
               switch(i)
               {
                  case 1: m_strName      = _T(pszOld); break;
                  case 2: m_strFirstName = _T(pszOld); break;
                  case 3: m_strStreet    = _T(pszOld); break;
                  case 4: m_strStreetNo  = _T(pszOld); break;
                  case 5: m_strPostCode  = _T(pszOld); break;
                  case 6: m_strCity      = _T(pszOld); break;
               }
               if (i & 1) pszOld = &pszText[1];
               else       pszOld = &pszText[2];
               if (bContinue)
               {
                  bContinue = false;
                  i++;
               }
               if (i == 6)
               {
                  m_strPhone    = _T(pszOld);
                  break;
               }
            }
            UpdateData(false);
         }
      }
      ::CloseClipboard();
   }
}

void CCustomer::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
   CWnd *pPrintAdrKl = GetDlgItem(IDC_CUSTOMER_PRINT_ADR_KL);
   CRect rcWnd;
   if (pPrintAdrKl)
   {
      pPrintAdrKl->GetWindowRect(&rcWnd);
      CPoint pt = point;
//      ClientToScreen(&pt);
      if (rcWnd.PtInRect(pt))
      {
         CMenu Menu;
         Menu.LoadMenu(IDR_INV_PRT_ADR_KL);
         CMenu * pContext = Menu.GetSubMenu(0);
         if (pContext)
         {  
            CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
            pContext->CheckMenuItem(ID_PRT_NAME_OF_ADR , ((pApp->m_bPrintNameOfAdress)    ? MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
            pContext->CheckMenuItem(ID_PRT_ADR_W_INV_NO, ((pApp->m_bPrintAdressWithInvNo) ? MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
            pContext->EnableMenuItem(ID_PRT_ADR_W_INV_NO, MF_GRAYED|MF_BYCOMMAND);
            switch (pContext->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, this))
            {
               case ID_PRT_NAME_OF_ADR:
                  pApp->m_bPrintNameOfAdress = !pApp->m_bPrintNameOfAdress;
                  break;
               case ID_PRT_ADR_W_INV_NO:
                  pApp->m_bPrintAdressWithInvNo = !pApp->m_bPrintAdressWithInvNo;
                  break;
               case IDCANCEL: default:break;
            }
            return;
         }
      }
   }

   OnEditPaste();
   return;	
}


void CCustomer::OnKillfocusCustomerPlz() 
{
	if (UpdateData(true))
   {
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
      POSITION  pos;
      State *pState;
      pos = pDoc->m_States.GetHeadPosition();
      if (m_strState.IsEmpty())
      {
         while (pos)
         {
            pState = (State*) pDoc->m_States.GetNext(pos);
            if (pState)
            {
               const char *pPC = LPCTSTR(m_strPostCode);
               const char *ptr = strstr(pPC, LPCTSTR(pState->szStateSign));
               if (ptr == pPC)
               {
                  UINT i, nlen;
                  for (i=0; ptr[i] != 0; i++)
                     if (!isalpha(ptr[i])) break;
                  nlen = max(strlen(pState->szStateSign), i);
                  if (strncmp(ptr, pState->szStateSign, nlen) != 0) continue;
                  m_strState = _T(pState->szState);
                  CDataExchange dx(this, false);
                  DDX_Text(&dx, IDC_CUSTOMER_STATE,m_strState);
                  break;
               }
            }
         }
      }
      if (m_strCity.IsEmpty())
      {
         CPtrList *plist   =  &pDoc->m_Customers;
         const char *pszPC = LPCTSTR(m_strPostCode);
         POSITION  pos     = plist->GetHeadPosition();
         Customer *pc;
         while (pos)
         {
            pc = (Customer*) plist->GetNext(pos);
            if (strcmp(pszPC, pc->szPostalCode) == 0)
            {
               m_strCity = _T(pc->szCity);
               CDataExchange dx(this, false);
               DDX_Text(&dx, IDC_CUSTOMER_CITY, m_strCity);
               break;
            }
         }
      }
   }
}

void CCustomer::OnCustomerImport() 
{
   OnCustomerNew();
   const static char *strKeys[] = 
   {
      "Von: ",          //  0
      "An: ",           //  1
      "Gesendet: ",     //  2
      "Betreff: ",      //  3
      "Browser  = ",    //  4
      "Ident    = ",    //  5
      "Produkte = ",    //  6
      "Porto    = ",    //  7
      "Summe    = ",    //  8
      "Vorname  = ",    //  9
      "Nachname = ",    // 10
      "Strasse  = ",    // 11
      "PLZ      = ",    // 12
      "Ort      = ",    // 13
      "EMail    = ",    // 14
      "Telefon  = ",    // 15
      "Telefax  = ",    // 16
      "Firma    = ",    // 17
      "RgNr     = ",    // 18
      "KontoNr  = ",    // 19
      "Bank     = ",    // 20
      "BLZ      = ",    // 21
      NULL
   };
   CFileDialog fd(true);
   int i, j, nLen;
   fd.m_ofn.nFilterIndex   = 1;
   fd.m_ofn.nFileExtension = 1;
   CString string, strBank, strKto, strBLZ, strIdent;
   string.LoadString(IDS_CUSTOMER_TXT_FILE);
   fd.m_ofn.lpstrFilter    = LPCTSTR(string);
   for (i=string.GetLength()-1; i>=0; i--) if (fd.m_ofn.lpstrFilter[i] == '\n') ((char*)fd.m_ofn.lpstrFilter)[i] = 0;
   fd.m_ofn.nMaxCustFilter = 2;
   fd.m_ofn.lpstrDefExt    = &fd.m_ofn.lpstrFilter[strlen(fd.m_ofn.lpstrFilter)];

   CFile file;
   if ((fd.DoModal() == IDOK) && (fd.m_ofn.lpstrFileTitle != NULL) &&
        file.Open(fd.m_ofn.lpstrFileTitle, CFile::modeRead))
   {
      DWORD dwLength = file.GetLength();
      if ((dwLength > 0) && (dwLength < 65536))
      {
         char *ptr1, *ptr2, ctemp;
         char *pstr = new char[dwLength+1];
         if (pstr)
         {
            file.Read(pstr, dwLength);
            file.Close();      
            pstr[dwLength] = 0;
            for (i=0; strKeys[i] != NULL; i++)
            {
               ptr1  = strstr(pstr, strKeys[i]);
               if (ptr1)
               {
//                  if (strKeys[i+1]) ptr2 = strstr(ptr1, strKeys[i+1]);
//                  else              ptr2 = NULL;
                  ptr2 = strstr(ptr1, "\r\n");
                  ptr1 += strlen(strKeys[i]);
                  if (ptr2 == NULL) ptr2 = &pstr[dwLength];
                  nLen = ptr2 - ptr1;
                  for (j=-1; ptr2[j]!=0; j--)
                  {
                     if (-j > nLen)        break;
                     if (isalnum(ptr2[j])) break;
                  }
                  ctemp     = ptr2[j+1];
                  ptr2[j+1] = 0;

                  switch (i)
                  {
                     case  4:
                     {
                        CString str = ((CFacturaApp*)AfxGetApp())->m_strStartDir + _T("Browser.log");
                        FILE *fp = fopen(str, "a+t");
                        if (fp)
                        {
                           fprintf(fp, "%s\n", ptr1);
                           fclose(fp);
                        }
                     }break;
                     case  5: strIdent       = _T(ptr1); break;
                     case  9: m_strFirstName = _T(ptr1); break;
                     case 10: m_strName      = _T(ptr1); break;
                     case 11: m_strStreet    = _T(ptr1); break;
                     case 12: m_strPostCode  = _T(ptr1); break;
                     case 13: m_strCity      = _T(ptr1); break;
                     case 14: m_strEmail     = _T(ptr1); break;
                     case 15: m_strPhone     = _T(ptr1); break;
                     case 16: m_strFax       = _T(ptr1); break;
                     case 17: m_strCompany   = _T(ptr1); break;
                     case 19: strKto         = _T(ptr1); break;
                     case 20: strBank        = _T(ptr1); break;
                     case 21: strBLZ         = _T(ptr1); break;
                     case  8: m_strAmountOfImport = _T(ptr1); break;
                  }
                  ptr2[j+1] = ctemp;
               }
            }

            for (i=0; i<(int)dwLength; i++)
            {
               if (pstr[i] == '\n') pstr[i] = 10;
               if (pstr[i] == '\r') pstr[i] = 13;
            }

            m_strComment = _T(pstr);
            if (m_hWnd) UpdateData(false);
            if (AfxMessageBox(IDS_SAVE_IN_REPORT_FILE, MB_YESNO) == IDYES)
            {
               char szTempPath[_MAX_PATH];
               CString strFilename;
               GetTempPath(_MAX_PATH, szTempPath);
               if (strFilename.LoadString(IDS_LOG_FILE_NAME))
                  strcat(szTempPath, strFilename);
               BOOL bOpen = file.Open(szTempPath, CFile::modeWrite);
               if (!bOpen) bOpen = file.Open(szTempPath, CFile::modeCreate|CFile::modeWrite);
               if (bOpen)
               {
                  file.SeekToEnd();
                  file.Write(pstr, dwLength);
                  pstr[0] = 13;
                  pstr[1] = 10;
                  file.Write(pstr, 2);
                  file.Write("--------------------", 20);
                  file.Write(pstr, 2);
                  file.Close();
               }
            }
            if (AfxMessageBox(IDS_DELETE_CUSTOMER_FILE, MB_YESNO)== IDYES)
            {
               CFile::Remove(fd.m_ofn.lpstrFileTitle);
            }
            OnChangeCustomer();
            if (!strBank.IsEmpty() || !strKto.IsEmpty() || !strBLZ.IsEmpty())
            {
               m_strCommentOfImport.Format("%s\r\nKto: %s\r\nBLZ: %s\r\n", strBank, strKto, strBLZ);
            }
            if (strIdent.GetLength() > 3)
            {
               m_strCommentOfImport += _T("IDENT: ") + strIdent;
            }
            delete[] pstr;
         }
      }
   }
}

void CCustomer::OnCustomerFlags()
{
   CListDlg ListDlg;
   ListDlg.m_strSelected.LoadString(IDS_CUSTOMER_FLAGS_DLG);
   ListDlg.m_pCallBack = CustomerFlags;
   ListDlg.m_pParam    = this;
   ListDlg.m_bOkButton = true;
   ListDlg.m_szSize.cx = 250;
   ListDlg.m_szSize.cy = 250;

   ListDlg.DoModal();
}

int CCustomer::CustomerFlags(CListDlg *pDlg, int nReason)
{
   CCustomer *pThis = (CCustomer*)pDlg->m_pParam;
   if (nReason == ONINITDIALOG)
   {
      int i;
      CRect rc;
      CString str;
      pDlg->m_List.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES,
         LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
      pDlg->m_List.ModifyStyle(0, LVS_EDITLABELS|LVS_NOSORTHEADER, 0);
      pDlg->m_List.GetClientRect(&rc);
      pDlg->m_List.InsertColumn(0, "", LVCFMT_LEFT, rc.right-17, 0);
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();

      for (i=0; i<CUSTOMER_FLAG_CNT; i++)
         pDlg->m_List.InsertItem(i, pDoc->m_strCustomerFlags[i]);

      for (i=0; i<CUSTOMER_FLAG_CNT;i++)
      {
         if (pThis->m_nCustomerFlags & (1 << (i+1)))
            ListView_SetCheckState(pDlg->m_List.m_hWnd, i, 1);
      }
      return 1;
   }
   else if (nReason == ONOK)
   {
      int i;
      long lFlag;
      bool bChecked, bState, bChanged = false;
      for (i=0; i<CUSTOMER_FLAG_CNT; i++)
      {
         lFlag    = 1 << (i+1);
         bState   = (pThis->m_nCustomerFlags&lFlag) ? true : false;
         bChecked = ListView_GetCheckState(pDlg->m_List.m_hWnd, i) ? true : false;
         if (bState != bChecked)
         {
            bChanged = true;
            if (bChecked) pThis->m_nCustomerFlags |=  lFlag;
            else          pThis->m_nCustomerFlags &= ~lFlag;
         }
      }
      if (bChanged)
         pThis->OnChangeCustomer();
   }
   else if (nReason == ONENDLABELEDITLSTLIST)
   {
      LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pDlg->m_pNMHdr;
      if ((pDispInfo->item.mask & LVIF_TEXT) &&
          (pDispInfo->item.pszText != NULL) &&
          (pDispInfo->item.iItem >= 0) && 
          (pDispInfo->item.iItem <  CUSTOMER_FLAG_CNT))
      {
         CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();
         pDoc->m_strCustomerFlags[pDispInfo->item.iItem] = _T(pDispInfo->item.pszText);
         pDoc->SetModifiedFlag(true);
         return 1;
      }
   }
   return 0;
}

void CCustomer::OnCustomerSendemail() 
{
   if (UpdateData(true) && !m_strEmail.IsEmpty())
   {
      CSendEmail dlg;
      CString strSpace = _T(" ");
      dlg.m_strAdress = m_strEmail;
      dlg.m_strName   = m_strFirstName + strSpace + m_strName;
      dlg.DoModal();
   }
}

void CCustomer::OnCustomerPrintAdrKl() 
{
   if (UpdateData(true))
   {
      SaveChangedData();
      Customer *pCustomer = GetCustomer();
      if (pCustomer)
      {
         CFacturaApp *pApp = (CFacturaApp*) AfxGetApp();
         pApp->PrintAdrKl(pCustomer, NULL);
      }
   }
}

HBRUSH CCustomer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
   switch (pWnd->GetDlgCtrlID())
   {
      case IDC_CUSTOMER_COMPANY:
      case IDC_CUSTOMER_STREET:
      {
         CString str;
         bool bChangeColor = false;
         pWnd->GetWindowText(str);
         char *pstr = (char*)strstr(str, "\r\n");
         if (pstr)
         {
            char *pstrP = (char*)LPCTSTR(str);
            do
            {
               pstr[0] = 0;
               if (strlen(pstrP) > m_nMaxLen)
               {
                  bChangeColor = true;
                  break;
               }
               pstrP = &pstr[1];
               pstr = strstr(&pstr[1], "\n");
            }
            while (pstr);
            if (!bChangeColor && (strlen(pstrP) > m_nMaxLen))
            {
               bChangeColor = true;
            }
         }
         else if (str.GetLength() > (int)m_nMaxLen)
            bChangeColor = true;
         if (bChangeColor) pDC->SetTextColor(RGB(255,  0,  0));
      }break;
   }
	return hbr;
}

LRESULT CCustomer::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
