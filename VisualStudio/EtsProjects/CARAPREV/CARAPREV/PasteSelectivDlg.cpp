// PasteSelectivDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "PasteSelectivDlg.h"
#include "CaraFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPasteSelectivDlg 


CPasteSelectivDlg::CPasteSelectivDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasteSelectivDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPasteSelectivDlg)
   m_nSelect = -1;
   //}}AFX_DATA_INIT
}


void CPasteSelectivDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPasteSelectivDlg)
   DDX_Radio(pDX, IDC_PASTESEL_FORMAT0, m_nSelect);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasteSelectivDlg, CDialog)
	//{{AFX_MSG_MAP(CPasteSelectivDlg)
	ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPasteSelectivDlg 

BOOL CPasteSelectivDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   GetDlgItem(IDC_PASTESEL_FORMAT0)->EnableWindow(false);
   GetDlgItem(IDC_PASTESEL_FORMAT1)->EnableWindow(false);
   GetDlgItem(IDC_PASTESEL_FORMAT2)->EnableWindow(false);

   CWnd *pWnd = AfxGetApp()->m_pMainWnd;
   if (pWnd && (((CCaraFrameWnd*)pWnd)->m_nCF_LABELOBJECT != 0) && ::IsClipboardFormatAvailable(((CCaraFrameWnd*)pWnd)->m_nCF_LABELOBJECT))
   {
      m_nSelect = 0;
      GetDlgItem(IDC_PASTESEL_FORMAT0)->EnableWindow(true);
   }
   if (::IsClipboardFormatAvailable(CF_ENHMETAFILE))
   {
      if (m_nSelect==-1) m_nSelect = 1;
      GetDlgItem(IDC_PASTESEL_FORMAT1)->EnableWindow(true);
   }
   if (::IsClipboardFormatAvailable(CF_DIB))
   {
      if (m_nSelect==-1) m_nSelect = 2;
      GetDlgItem(IDC_PASTESEL_FORMAT2)->EnableWindow(true);
   }
   if (m_nSelect==-1) m_nSelect = 3;
   UpdateData(false);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

LRESULT CPasteSelectivDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
   if (pHI->dwContextId == 0) return true;

   if (pHI->iContextType == HELPINFO_WINDOW)                   // Hilfe für ein Control oder window
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_CONTEXTPOPUP);
   }
   else                                                        // Hilfe für einen Menüpunkt
   {
      CDialog::WinHelp(pHI->dwContextId, HELP_FINDER);
   }
   return true;
}

void CPasteSelectivDlg::OnOK() 
{
   UpdateData(true);
	CDialog::OnOK();
}
