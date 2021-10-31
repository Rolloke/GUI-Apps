// EditFormDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "EditFormDlg.h"
#include "FacturaView.h"
#include "FacturaDoc.h"

#include "TextLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CEditFormDlg 

CEditFormDlg::CEditFormDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditFormDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditFormDlg)
	m_strCombo = _T("");
	//}}AFX_DATA_INIT
   m_pParent      = NULL;
   m_pInvoices    = NULL;
   m_nPosition    = CB_ERR;
   m_bTextChanged = false;
   m_bNoNewForms  = false;
}


void CEditFormDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditFormDlg)
	//}}AFX_DATA_MAP

   if (m_bNoNewForms)
   {
	   DDX_Control(pDX, IDC_EF_COMBO_LIST, m_cCombo);
   }
   else
   {
	   DDX_Control(pDX, IDC_EF_COMBO, m_cCombo);
	   DDX_CBString(pDX, IDC_EF_COMBO, m_strCombo);
   }
   if (m_pParent)
   {
      GetDlgItem(IDOK)->EnableWindow(!m_strCombo.IsEmpty());
   }
}


BEGIN_MESSAGE_MAP(CEditFormDlg, CDialog)
	//{{AFX_MSG_MAP(CEditFormDlg)
	ON_BN_CLICKED(IDC_EF_NEW, OnEfNew)
	ON_CBN_SELCHANGE(IDC_EF_COMBO_LIST, OnSelchangeEfCombo)
	ON_CBN_SELCHANGE(IDC_EF_COMBO, OnSelchangeEfCombo)
	ON_CBN_KILLFOCUS(IDC_EF_COMBO, OnKillfocusEfCombo)
	ON_CBN_EDITCHANGE(IDC_EF_COMBO, OnEditchangeEfCombo)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CEditFormDlg 

BOOL CEditFormDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   if (!m_strCaption.IsEmpty())
      SetWindowText(m_strCaption);

   if (m_pParent)
   {
      CLabelContainer *plc = m_pParent->GetDocument()->GetLabelContainer();
      ProcessLabel prl = {NULL, 0, &m_cCombo, NULL};
      plc->ProcessWithLabels(NULL, &prl, CEditFormDlg::EnumFormLabel);
      int nCount = m_cCombo.GetCount();
      if (m_nPosition == -1) m_nPosition = nCount - 1;
      if ((nCount != CB_ERR) && (m_nPosition < nCount))
      {
         m_nPosition = m_cCombo.SetCurSel(m_nPosition);
      }
      GetDlgItem(IDOK)->EnableWindow(m_nPosition != CB_ERR);
   }
   else if (m_pInvoices)
   {
      POSITION pos = m_pInvoices->GetHeadPosition();
      Invoice *pInvoice;
      int nIndex;
      while (pos)
      {
         pInvoice = (Invoice*) m_pInvoices->GetNext(pos);
         if (pInvoice && (pInvoice->nInvNo == -1))
         {
            nIndex = m_cCombo.AddString(&pInvoice->szComment[pInvoice->nMonth]);
            m_cCombo.SetItemDataPtr(nIndex, pInvoice);
         }
      }
      int nCount = m_cCombo.GetCount();
      if ((nCount != CB_ERR) && (m_nPosition != CB_ERR) && (m_nPosition < nCount))
      {
         m_nPosition = m_cCombo.SetCurSel(m_nPosition);
      }
      GetDlgItem(IDOK)->EnableWindow(true);
   }

   if (m_bNoNewForms)
   {
      GetDlgItem(IDC_EF_COMBO_LIST)->ShowWindow(SW_SHOW);
   }
   else
   {
      GetDlgItem(IDC_EF_COMBO)->ShowWindow(SW_SHOW);
      GetDlgItem(IDC_EF_NEW)->ShowWindow(SW_SHOW);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CEditFormDlg::OnOK()
{
   CDialog::OnOK();

   if (m_nPosition == CB_ERR) m_nPosition = m_cCombo.GetCount();

   if (m_nPosition != CB_ERR)
   {
      if (m_pInvoices)
      {
         m_pInvoices = (CPtrList*)m_cCombo.GetItemDataPtr(m_nPosition);
      }
      else if (m_pParent)
      {
         m_strCombo.Insert(0, FORM_SIGN);
      }
   }
   else
   {
      m_pInvoices = NULL;
   }
}

void CEditFormDlg::OnCancel() 
{
   CDialog::OnCancel();
   m_pInvoices = NULL;
}

void CEditFormDlg::OnEfNew() 
{
   m_strCombo = _T("Neue Vorlage");
   m_nPosition = CB_ERR;
   GetDlgItem(IDC_EF_NEW)->EnableWindow(false);
   UpdateData(false);
}

void CEditFormDlg::OnSelchangeEfCombo() 
{
   if (!m_bNoNewForms && !GetDlgItem(IDC_EF_NEW)->IsWindowEnabled() && (m_nPosition == CB_ERR))
   {
      GetDlgItem(IDC_EF_NEW)->EnableWindow(true);
   }
   m_nPosition = m_cCombo.GetCurSel();
}

void CEditFormDlg::OnKillfocusEfCombo()
{
	if (m_bTextChanged && (m_nPosition != CB_ERR) && UpdateData(true))
   {
      m_cCombo.InsertString(m_nPosition, m_strCombo);
      m_cCombo.DeleteString(m_nPosition+1);
      m_nPosition = m_cCombo.SetCurSel(m_nPosition);
   }
}

void CEditFormDlg::OnEditchangeEfCombo() 
{
	m_bTextChanged = true;
}

int CEditFormDlg::EnumFormLabel(CLabel *pl, void *p)
{
   ASSERT(pl != NULL);
   ASSERT(p  != NULL);
   if (pl->IsKindOf(RUNTIME_CLASS(CLabelContainer)))
   {
      CLabel *pltemp = ((CLabelContainer*)pl)->GetLabel(0);
      if (pltemp->IsKindOf(RUNTIME_CLASS(CTextLabel)))
      {
         char *ptest = FORM_SIGN;
         const char *ptxt = ((CTextLabel*)pltemp)->GetText();

         if (ptxt && (ptxt[0] == ptest[0]))
         {
            ProcessLabel *prl = (ProcessLabel*)p;
            ASSERT(prl->pParam1 != NULL);
            ((CComboBox*)prl->pParam1)->InsertString(prl->nPos, &ptxt[1]);
            prl->nPos++;
         }
      }
   }
   return 0;
}
LRESULT CEditFormDlg::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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

/*
LRESULT (CALLBACK *CEditFormDlg::gm_pOldWF)(HWND, UINT, WPARAM, LPARAM) = NULL;

gm_pOldWF  = (LRESULT (CALLBACK *)(HWND, UINT, WPARAM, LPARAM)) SetWindowLong(m_cCombo.m_hWnd, GWL_WNDPROC, (LONG) CEditFormDlg::SubClass);

LRESULT CALLBACK CEditFormDlg::SubClass(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
   ASSERT(gm_pOldWF != NULL);
   TRACE("Msg:%x\n", nMsg);
   return CallWindowProc(gm_pOldWF, hWnd, nMsg, wParam, lParam);
}
*/