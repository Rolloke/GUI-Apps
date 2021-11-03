// PrintLabel.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PrintLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPrintLabel 


CPrintLabel::CPrintLabel(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintLabel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintLabel)
	m_nLabelCount = 1;
	//}}AFX_DATA_INIT
   CFacturaApp *pApp =(CFacturaApp*) AfxGetApp();
   m_nLines  = pApp->GetProfileInt(LABEL_SETTINGS, LABEL_LINES, 12),
   m_nLength = pApp->GetProfileInt(LABEL_SETTINGS, LABEL_LINE_LENGTH, 31);
}

CPrintLabel::~CPrintLabel()
{
   CFacturaApp *pApp =(CFacturaApp*) AfxGetApp();
}

void CPrintLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintLabel)
	DDX_Control(pDX, IDC_SPIN_COUNT, m_cSpinCount);
	DDX_Control(pDX, IDC_EDT_TEXT, m_cText);
	DDX_Text(pDX, IDC_EDT_COUNT, m_nLabelCount);
	DDX_Text(pDX, IDC_LENGTH, m_nLength);
	DDV_MinMaxInt(pDX, m_nLength, 1, 1024);
	DDX_Text(pDX, IDC_LINES, m_nLines);
	DDV_MinMaxInt(pDX, m_nLines, 1, 1024);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintLabel, CDialog)
	//{{AFX_MSG_MAP(CPrintLabel)
	ON_BN_CLICKED(IDC_SAVESETTINGS, OnSavesettings)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPrintLabel 

void CPrintLabel::OnOK() 
{
   if (UpdateData(true))
   {
      CFacturaApp *pApp =(CFacturaApp*) AfxGetApp();
      if (pApp->GetStandardPrinter(ADR_KL_PRINTER))
      {
         int i, j, nLen;
         if ((m_nLength > 0) && (m_nLines > 0))
         {
            char *pszText = (char*) malloc(m_nLength+1);
            if (pszText)
            {
               for (j=0; j<m_nLabelCount; j++)
               {
                  for (i=0; i<m_nLines; i++)
                  {
                     nLen = m_cText.GetLine(i, pszText, m_nLength);
                     pszText[nLen] = 0;
                     pApp->StandardPrint(0, pszText, 0, 1, m_nLength);
                  }
               }
               free(pszText);
            }
         }
         pApp->CloseStandardPrinter();
      }
   }
	CDialog::OnOK();
}

BOOL CPrintLabel::OnInitDialog() 
{
	CDialog::OnInitDialog();
   m_cSpinCount.SetRange32(1000, 1);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CPrintLabel::OnSavesettings() 
{
   if (UpdateData())
   {
      CFacturaApp *pApp =(CFacturaApp*) AfxGetApp();
      pApp->WriteProfileInt(LABEL_SETTINGS, LABEL_LINES, m_nLines),
      pApp->WriteProfileInt(LABEL_SETTINGS, LABEL_LINE_LENGTH, m_nLength);
   }
}

void CPrintLabel::OnCancel() 
{
	CDialog::OnCancel();
}

LRESULT CPrintLabel::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
