// PrintAdr.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PrintAdr.h"
#include "FacturaDoc.h"
#include "FindCustomer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintAdr 


CPrintAdr::CPrintAdr(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintAdr::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPrintAdr)
   m_nFrom           = 0;
   m_nTo             = 0;
   m_bHiak           = TRUE;
   m_bCale           = TRUE;
   m_bServiceCard    = TRUE;
	m_bPrintNameOfAdress = FALSE;
	//}}AFX_DATA_INIT
   m_nMax            = 0;
}

CPrintAdr::~CPrintAdr()
{
}

void CPrintAdr::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPrintAdr)
   DDX_Check(pDX, IDC_PRINT_ADR_HIAK, m_bHiak);
   DDX_Check(pDX, IDC_PRINT_ADR_CALE, m_bCale);
   DDX_Check(pDX, IDC_PRINT_ADR_SERVICECARD, m_bServiceCard);
	DDX_Check(pDX, IDC_PRINT_NAME_OF_ADR, m_bPrintNameOfAdress);
	//}}AFX_DATA_MAP

   DDX_Text(pDX, IDC_PRINT_ADR_CUST_FROM, m_nFrom);
   DDX_Text(pDX, IDC_PRINT_ADR_CUST_TO, m_nTo);
   DDV_MinMaxInt(pDX, m_nFrom, 0, m_nTo);
   DDV_MinMaxInt(pDX, m_nTo, m_nFrom, m_nMax);
}


BEGIN_MESSAGE_MAP(CPrintAdr, CDialog)
   //{{AFX_MSG_MAP(CPrintAdr)
   ON_BN_CLICKED(IDC_PRINT_ADR_SELECT, OnPrintAdrSelect)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPrintAdr 

void CPrintAdr::OnPrintAdrSelect() 
{
   CFindCustomer dlg;
   dlg.m_nMultiSelect = m_nMax;
   if (dlg.DoModal() == IDOK)
   {
      CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
      int        nCount = dlg.m_nMultiSelect;
      Customer **ppList = dlg.GetCustomerList();
      if (ppList && pApp && pApp->GetStandardPrinter(ADR_KL_PRINTER))
      {
         for (int i=0; i<nCount; i++)
         {
            pApp->PrintAdrKl(ppList[i]);
         }
         pApp->CloseStandardPrinter();
      }
      return;
   }	
}

void CPrintAdr::OnCancel() 
{
   CDialog::OnCancel();
}

void CPrintAdr::OnOK()
{
   CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
   if (UpdateData(true) && pApp && pApp->GetStandardPrinter(ADR_KL_PRINTER))
   {
      pApp->m_bPrintNameOfAdress = (m_bPrintNameOfAdress != 0) ? true : false;
      bool bSelect = m_bHiak || m_bCale || m_bServiceCard;
      POSITION pos = m_pCustomers->GetHeadPosition();
      while ( pos!=NULL)
      {
         Customer *pCustomer = (Customer*) m_pCustomers->GetNext(pos);
         if (pCustomer)
         {
            if ((pCustomer->nNo >= m_nFrom) && (pCustomer->nNo <= m_nTo))
            {
               if (bSelect)
               {
                  if (m_bHiak && !(pCustomer->nFlags&CUSTOMER_HIAK)) continue;
                  if (m_bCale && !(pCustomer->nFlags&CUSTOMER_CALE)) continue;
                  if (m_bServiceCard && !(pCustomer->nFlags&CUSTOMER_SERVICE_CARD)) continue;
               }
               pApp->PrintAdrKl(pCustomer);
            }
         }
      }
      pApp->CloseStandardPrinter();
      CDialog::OnOK();
   }
}

BOOL CPrintAdr::OnInitDialog() 
{
   CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
   if (!pApp)             return false;
   if (!pApp->m_pMainWnd) return false;
   CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
   if (!pDoc)             return false;
   m_bPrintNameOfAdress = pApp->m_bPrintNameOfAdress;
   m_pCustomers = &pDoc->m_Customers;
   POSITION pos = m_pCustomers->GetTailPosition();
   if (pos)
   {
      Customer *pC = (Customer*) m_pCustomers->GetAt(pos);
      m_nMax = pC->nNo;
   }

   CDialog::OnInitDialog();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

LRESULT CPrintAdr::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
