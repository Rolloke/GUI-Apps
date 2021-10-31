// PayMode.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Factura.h"
#include "PayMode.h"
#include "FacturaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPayMode 


CPayMode::CPayMode(CWnd* pParent /*=NULL*/)
	: CDialog(CPayMode::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPayMode)
	m_strEdit = _T("Neu");
	m_nIndex = 0;
	//}}AFX_DATA_INIT
   m_nIndex   = -1;
   m_nPayMode = 1;
}


void CPayMode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPayMode)
	DDX_Text(pDX, IDC_PAYMODE_EDIT, m_strEdit);
	DDX_Text(pDX, IDC_PAYMODE_SPIN_EDIT, m_nIndex);
	//}}AFX_DATA_MAP
   GetDlgItem(IDC_PAYMODE_DELETE)->EnableWindow(m_nIndex != -1);
}

BEGIN_MESSAGE_MAP(CPayMode, CDialog)
	//{{AFX_MSG_MAP(CPayMode)
	ON_BN_CLICKED(IDC_PAYMODE_NEW, OnPaymodeNew)
	ON_BN_CLICKED(IDC_PAYMODE_DELETE, OnPaymodeDelete)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PAYMODE_SPIN, OnDeltaposPaymodeSpin)
	ON_EN_CHANGE(IDC_PAYMODE_EDIT, OnChangePaymodeEdit)
	ON_BN_CLICKED(IDC_PAYMODE_SAVE, OnPaymodeSave)
   ON_MESSAGE(WM_HELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CPayMode 

void CPayMode::OnPaymodeNew() 
{
   SaveChangedData();
   if (m_nIndex != -1)
   {
      m_nIndex   = -1;
      m_strEdit  =_T("Neu");;
      UpdateData(false);
   }
}

void CPayMode::OnPaymodeDelete() 
{
   if (m_nIndex != -1)
   {
      POSITION pos = m_pPayModes->FindIndex(m_nIndex);
      if (pos)
      {
         BYTE *pOld = (BYTE*) m_pPayModes->GetAt(pos);
         if (pOld)
         {
            delete[] pOld;
            m_pPayModes->RemoveAt(pos);
            m_nIndex = 0;
            InitPayModeData();
         }
      }
   }
}

void CPayMode::OnDeltaposPaymodeSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
   SaveChangedData();
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   m_nIndex -= pNMUpDown->iDelta;
   if      (m_nIndex <                        0) m_nIndex = m_pPayModes->GetCount()-1;
   else if (m_nIndex >= m_pPayModes->GetCount()) m_nIndex = 0;
   InitPayModeData();

   *pResult = 0;
}

BOOL CPayMode::OnInitDialog() 
{
	if (CDialog::OnInitDialog())
   {
      CWinApp *pApp = AfxGetApp();
      if (!pApp)             return false;
      if (!pApp->m_pMainWnd) return false;
      CFacturaDoc* pDoc = (CFacturaDoc*) ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
      if (!pDoc)             return false;
      m_pPayModes = &pDoc->m_PayModes;
      m_nIndex = m_pPayModes->GetCount()-1;
      InitPayModeData();
      GetDlgItem(IDC_PAYMODE_SAVE)->EnableWindow(false);
      return TRUE;
   }
   return false;
}

void CPayMode::SaveChangedData(bool bAsk)
{
   if (GetDlgItem(IDC_PAYMODE_SAVE)->IsWindowEnabled() && UpdateData(true))
   {  
      int nReturn = IDYES;
      if (bAsk) nReturn = AfxMessageBox(IDS_SAVE_PAYMODE, MB_YESNO);
      if (nReturn == IDYES)
      {
         int nSize = sizeof(PayMode)+m_strEdit.GetLength()+1;
	      PayMode *pPM = (PayMode*) new BYTE[nSize];
         if (pPM)
         {
            pPM->nMode = m_nPayMode;
            pPM->nSize = nSize;
            strcpy(pPM->szMode, LPCTSTR(m_strEdit));
            CWinApp *pApp = AfxGetApp();
            if (pApp && pApp->m_pMainWnd)
            {
               CDocument* pDoc = ((CFrameWnd*)pApp->m_pMainWnd)->GetActiveDocument();
               if (pDoc) pDoc->SetModifiedFlag(true);
            }
         }
         if (m_nIndex == -1)
         {
            int nMaxMode = 0;
            POSITION pos = m_pPayModes->GetHeadPosition();
            while (pos)
            {
               PayMode* pPayMode = (PayMode*) m_pPayModes->GetNext(pos);
               if (pPayMode && (pPayMode->nMode > nMaxMode)) nMaxMode = pPayMode->nMode;
            }
            pPM->nMode = nMaxMode+1;
            m_nPayMode = pPM->nMode;
            m_pPayModes->AddTail(pPM);
            m_nIndex = m_pPayModes->GetCount()-1;
            CDataExchange dx(this, false);
         	DDX_Text(&dx, IDC_PAYMODE_SPIN_EDIT, m_nIndex);
         }
         else
         {
            POSITION pos = m_pPayModes->FindIndex(m_nIndex);
            if (pos)
            {
               PayMode *pOld = (PayMode*) m_pPayModes->GetAt(pos);
               if (pOld) delete[] (BYTE*) pOld;
               m_pPayModes->SetAt(pos, pPM);
            }
         }
      }
      GetDlgItem(IDC_PAYMODE_SAVE)->EnableWindow(false);
   }
}

void CPayMode::OnChangePaymodeEdit() 
{
   GetDlgItem(IDC_PAYMODE_SAVE)->EnableWindow(true);
}

void CPayMode::InitPayModeData()
{
   if (m_nIndex != -1)
   {
      POSITION pos = m_pPayModes->FindIndex(m_nIndex);
      if (pos)
      {
         PayMode *pPM = (PayMode*) m_pPayModes->GetAt(pos);
         if (pPM)
         {
            m_strEdit = _T(pPM->szMode);
            m_nPayMode = pPM->nMode;
            UpdateData(false);
         }
      }
   }
}

void CPayMode::OnPaymodeSave() 
{
   SaveChangedData(false);
}

void CPayMode::OnCancel() 
{
   SaveChangedData();
	CDialog::OnCancel();
}

void CPayMode::OnOK()
{
   SaveChangedData();
   CDialog::OnOK();
}

LRESULT CPayMode::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
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
