// PrintFromTo.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PrintFromTo.h"
#include "ETS3DGLRegKeys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintFromTo 

#define PRINT_LINE_LENGTH "PrintLineLength"

CPrintFromTo::CPrintFromTo(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintFromTo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintFromTo)
	m_nFrom = 0;
	m_nTo = 0;
	m_bPrintHidden = FALSE;
	//}}AFX_DATA_INIT
	m_nLingeLength = AfxGetApp()->GetProfileInt(REGKEY_DEFAULT, PRINT_LINE_LENGTH, 100);
}


void CPrintFromTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintFromTo)
	DDX_Text(pDX, IDC_EDT_PFT_LINE_LENGTH, m_nLingeLength);
	DDV_MinMaxInt(pDX, m_nLingeLength, 1, 1024);
	DDX_Check(pDX, IDC_CK_PFT_PRINT_HIDDEN, m_bPrintHidden);
	//}}AFX_DATA_MAP

   DDX_Text(pDX, IDC_EDT_PROD_FROM, m_nFrom);
	DDV_MinMaxInt(pDX, m_nFrom, m_nRangeFrom, m_nRangeTo);
	DDX_Text(pDX, IDC_EDT_PROD_TO, m_nTo);
	DDV_MinMaxInt(pDX, m_nTo, m_nRangeFrom, m_nRangeTo);
   if (pDX->m_bSaveAndValidate)
   {
      if (m_nFrom > m_nTo)
      {
         swap(m_nFrom, m_nTo);
      }
   }
}


BEGIN_MESSAGE_MAP(CPrintFromTo, CDialog)
	//{{AFX_MSG_MAP(CPrintFromTo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPrintFromTo 

void CPrintFromTo::OnCancel() 
{
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	
	CDialog::OnCancel();
}

BOOL CPrintFromTo::OnInitDialog() 
{
   m_nFrom = m_nRangeFrom;
   m_nTo   = m_nRangeTo;
	CDialog::OnInitDialog();
	
	return TRUE;
}

void CPrintFromTo::OnOK() 
{
	CDialog::OnOK();
	AfxGetApp()->WriteProfileInt(REGKEY_DEFAULT, PRINT_LINE_LENGTH, m_nLingeLength);
}
