// Derive.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "Derive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDerive 


CDerive::CDerive(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CDerive::IDD, pParent)
{
   //{{AFX_DATA_INIT(CDerive)
	//}}AFX_DATA_INIT
}


void CDerive::DoDataExchange(CDataExchange* pDX)
{
   CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDerive)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDerive, CCurveDialog)
	//{{AFX_MSG_MAP(CDerive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDerive 
UINT CDerive::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CDerive *pDrv = (CDerive*) pParam;

      pDrv->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pDrv->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);
      pDrv->m_pCurve->SetProgressWnd(pProg->m_hWnd);

      pDrv->m_pNewCurve = new CCurveLabel(pDrv->m_pCurve);     // neue Kurve erzeugen
      if (pDrv->m_pNewCurve)
      {
         pDrv->m_pNewCurve->SetTargetFuncNo(0);
         pDrv->m_pNewCurve->SetText((char*)LPCTSTR(pDrv->m_strNewName));

         if (pDrv->m_pCurve->Derive_X(pDrv->m_pNewCurve))
         {
            pDrv->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pDrv->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pDrv->m_pThread = NULL;
   }
   return uResult;
}

BOOL CDerive::OnInitDialog() 
{
   CCurveDialog::OnInitDialog();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CDerive::OnOK() 
{
   if (m_pCurve && UpdateData(true))                           // Rechenparameter ermitteln
   {
      GetDlgItem(IDC_CALCULATE)->EnableWindow(false);
      m_pThread = AfxBeginThread(ThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
      if (m_pThread)
      {
         m_pThread->m_bAutoDelete = TRUE;
         m_pThread->ResumeThread();
      }
   }
   else UpdateData(false);
}

void CDerive::InitCurveParams()
{
   if (m_pCurve)
   {
      const char * pszText = m_pCurve->GetText();
      if (pszText) m_strNewName.Format(IDS_DERIVE_CURVE, pszText);
      UpdateData(false);
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }	
}

