// Integrate.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "CARA2DV.h"
#include "Integrate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CIntegrate 


CIntegrate::CIntegrate(CWnd* pParent)
	: CCurveDialog(CIntegrate::IDD, pParent)
{
   //{{AFX_DATA_INIT(CIntegrate)
	//}}AFX_DATA_INIT
}


void CIntegrate::DoDataExchange(CDataExchange* pDX)
{
	CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntegrate)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntegrate, CCurveDialog)
	//{{AFX_MSG_MAP(CIntegrate)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_BN_CLICKED(IDC_HIDE, OnHide)
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CIntegrate 
UINT CIntegrate::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CIntegrate *pInt = (CIntegrate*) pParam;

      pInt->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pInt->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);
      pInt->m_pCurve->SetProgressWnd(pProg->m_hWnd);

      pInt->m_pNewCurve = new CCurveLabel(pInt->m_pCurve);     // neue Kurve erzeugen
      if (pInt->m_pNewCurve)
      {
         pInt->m_pNewCurve->SetTargetFuncNo(0);
         pInt->m_pNewCurve->SetText((char*)LPCTSTR(pInt->m_strNewName));

         if (pInt->m_pCurve->Integrate_X(0, pInt->m_pNewCurve))
         {
            pInt->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pInt->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pInt->m_pThread = NULL;
   }
   return uResult;
}

void CIntegrate::OnOK() 
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

BOOL CIntegrate::OnInitDialog() 
{
   CCurveDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CIntegrate::InitCurveParams()
{
   if (m_pCurve)
   {
      const char * pszText = m_pCurve->GetText();
      if (pszText) m_strNewName.Format(IDS_INTEGRATE_CURVE, pszText);
      UpdateData(false);
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }	
}
