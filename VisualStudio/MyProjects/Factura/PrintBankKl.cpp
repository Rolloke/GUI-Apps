// PrintBankKl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PrintBankKl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintBankKl 

#define BANK_SETTINGS   "Bank"
#define BANK_NAME       "Name"
#define BANK_BLZ        "Blz"
#define BANK_KTO_NO     "KtoNr"
#define BANK_CURRENCY   "Currency"
#define BANK_AMOUNT     "Amount"

CPrintBankKl::CPrintBankKl(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintBankKl::IDD, pParent)
{

	//{{AFX_DATA_INIT(CPrintBankKl)
	//}}AFX_DATA_INIT
	m_nCount       = 1;
}

CPrintBankKl::~CPrintBankKl()
{

}

void CPrintBankKl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintBankKl)
	DDX_Text(pDX, IDC_PBK_AMOUNT, m_strAmount);
	DDX_Text(pDX, IDC_PBK_BANKNAME, m_strBankName);
	DDX_Text(pDX, IDC_PBK_BLZ, m_strBlz);
	DDX_Text(pDX, IDC_PBK_CURRENCY, m_strCurrency);
	DDX_Text(pDX, IDC_PBK_KTO_NO, m_strKtoNo);
	DDX_Text(pDX, IDC_PBK_COUNT, m_nCount);
	DDV_MinMaxInt(pDX, m_nCount, 1, 200);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintBankKl, CDialog)
	//{{AFX_MSG_MAP(CPrintBankKl)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPrintBankKl 

void CPrintBankKl::OnOK() 
{
   CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
   if (UpdateData(true) && pApp)
   {
      PrintBankKl();
   }
	
	CDialog::OnOK();
}

void CPrintBankKl::PrintBankKl()
{
   CFacturaApp *pApp = (CFacturaApp*)AfxGetApp();
   if (pApp && pApp->GetStandardPrinter(ADR_KL_PRINTER))
   {
      CString str;
      int nMax = 30;
      int nOffset;
      for (int i=0; i<m_nCount; i++)
      {
         pApp->StandardPrint(0, "", 0, 1, nMax);
         nOffset = (nMax - m_strBankName.GetLength()) >> 1;
         pApp->StandardPrint(nOffset, (char*)LPCTSTR(m_strBankName), 0, 2, nMax);
         str = _T("Bankleitzahl ") + m_strBlz;
         nOffset = (nMax - str.GetLength()) >> 1;
         pApp->StandardPrint(nOffset, (char*)LPCTSTR(str), 0, 1, nMax);
         str = _T("Konto-Nr. ") + m_strKtoNo;
         nOffset = (nMax - str.GetLength()) >> 1;
         pApp->StandardPrint(nOffset, (char*)LPCTSTR(str), 0, 2, nMax);
         str = m_strCurrency + _T(" ") + m_strAmount;
         nOffset = (nMax - str.GetLength()) >> 1;
         pApp->StandardPrint(nOffset, (char*)LPCTSTR(str), 0, 6, nMax);
      }
      pApp->CloseStandardPrinter();
   }
}

LRESULT CPrintBankKl::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
