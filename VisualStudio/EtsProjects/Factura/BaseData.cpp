// BaseData.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "BaseData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld BaseData 


BaseData::BaseData(CWnd* pParent /*=NULL*/)
	: CDialog(BaseData::IDD, pParent)
{
	//{{AFX_DATA_INIT(BaseData)
	m_fNNAmount = 0.0f;
	m_fTax = 0.0f;
	//}}AFX_DATA_INIT
}


void BaseData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BaseData)
	DDX_Text(pDX, IDC_BD_NN_AMOUNT, m_fNNAmount);
	DDX_Text(pDX, IDC_BD_TAX, m_fTax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BaseData, CDialog)
	//{{AFX_MSG_MAP(BaseData)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten BaseData 

void BaseData::OnOK() 
{
	CDialog::OnOK();
}

LRESULT BaseData::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
