// FFTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "FFTDlg.h"


// CFFTDlg dialog

IMPLEMENT_DYNAMIC(CFFTDlg, CCurveDialog)
CFFTDlg::CFFTDlg(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CFFTDlg::IDD, pParent)
   , m_bTime(FALSE)
{
}

CFFTDlg::~CFFTDlg()
{
}

void CFFTDlg::DoDataExchange(CDataExchange* pDX)
{
   CCurveDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_RD_TIME, m_bTime);
}


BEGIN_MESSAGE_MAP(CFFTDlg, CCurveDialog)
END_MESSAGE_MAP()


// CFFTDlg message handlers
void CFFTDlg::OnOK() 
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

UINT CFFTDlg::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CFFTDlg *pDlg = (CFFTDlg*) pParam;

      pDlg->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pDlg->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);
      pDlg->m_pCurve->SetProgressWnd(pProg->m_hWnd);

      pDlg->m_pNewCurve = new CCurveLabel(pDlg->m_pCurve);     // neue Kurve erzeugen
      pDlg->m_pNewCurve->SetXStep(1.0/pDlg->m_pNewCurve->GetXStep());
      if (pDlg->m_pNewCurve)
      {
         pDlg->m_pNewCurve->SetTargetFuncNo(0);
         pDlg->m_pNewCurve->SetText((char*)LPCTSTR(pDlg->m_strNewName));
         pDlg->m_pNewCurve->SetXUnit(pDlg->m_bTime ? "Hz" : "t");

         if (pDlg->m_pCurve->FFT(pDlg->m_bTime ? true : false, pDlg->m_pNewCurve))
         {
            pDlg->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pDlg->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pDlg->m_pThread = NULL;
   }
   return uResult;
}
