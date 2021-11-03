// AverageDlg.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "CARA2DV.h"
#include "AverageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CAverageDlg 
extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);


CAverageDlg::CAverageDlg(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CAverageDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CAverageDlg)
   m_dAverageRange   = 1.;
   m_nNoOfSteps      = 1;
   m_nStepIndexFrq   = 1;
   m_nDecade_Divider = 1;
	m_nDivision = -1;
	//}}AFX_DATA_INIT
   m_strUnitx        = _T("[]");
}

void CAverageDlg::DoDataExchange(CDataExchange* pDX)
{
   double dAverageRange   = m_dAverageRange;
   int    nNoOfSteps      = m_nNoOfSteps;
   int    nDecade_Divider = m_nDecade_Divider;

	CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAverageDlg)
	DDX_Control(pDX, IDC_AVG_WHERE, m_CAverageDlgWhere);
	DDX_CBIndex(pDX, IDC_AVG_FREQU_STEP, m_nStepIndexFrq);
	DDX_Radio(pDX, IDC_AVG_R_LIN, m_nDivision);
	//}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      switch(m_nDivision)
      {
         case 0:  // linear
 	         DDX_TextVar(pDX, IDC_AVG_RANGE, DBL_DIG, dAverageRange);
	         DDV_MinMaxDouble(pDX, dAverageRange, 1.e-013, 10000000000000.);
            break;
         case 1:  // log.
	         DDX_Text(pDX, IDC_AVG_LOG_DIVIDER, nDecade_Divider);
	         DDV_MinMaxInt(pDX, nDecade_Divider, 1, 10);
            break;
         case 2:  // frq.
	         DDX_Text(pDX, IDC_AVG_FREQU_NO_OF_STEPS, nNoOfSteps);
	         DDV_MinMaxInt(pDX, nNoOfSteps, 1, 15);
            break;
      }
   }
   else
   {
 	   DDX_TextVar(pDX, IDC_AVG_RANGE, DBL_DIG, dAverageRange);
	   DDV_MinMaxDouble(pDX, dAverageRange, 1.e-013, 10000000000000.);
	   DDX_Text(pDX, IDC_AVG_FREQU_NO_OF_STEPS, nNoOfSteps);
	   DDV_MinMaxInt(pDX, nNoOfSteps, 1, 15);
	   DDX_Text(pDX, IDC_AVG_LOG_DIVIDER, nDecade_Divider);
	   DDV_MinMaxInt(pDX, nDecade_Divider, 1, 10);
	   DDX_Text(pDX, IDC_AVG_UNITX, m_strUnitx);
   }

   m_dAverageRange   = dAverageRange;
   m_nNoOfSteps      = nNoOfSteps;
   m_nDecade_Divider = nDecade_Divider;
}


BEGIN_MESSAGE_MAP(CAverageDlg, CCurveDialog)
	//{{AFX_MSG_MAP(CAverageDlg)
	ON_BN_CLICKED(IDC_AVG_R_LIN, OnClickDivsion)
	ON_BN_CLICKED(IDC_AVG_R_LOG, OnClickDivsion)
	ON_BN_CLICKED(IDC_AVG_R_LOG_FRQ, OnClickDivsion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CAverageDlg 
void CAverageDlg::OnOK() 
{
   if (m_pCurve && UpdateData(true))                           // Rechenparameter ermitteln
   {
      GetDlgItem(IDC_CALCULATE)->EnableWindow(false);
      m_pThread = AfxBeginThread(ThreadFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
      if (m_pThread)
      {
         m_pThread->m_bAutoDelete = false;
         m_pThread->ResumeThread();
      }
   }
   else UpdateData(false);
}

UINT CAverageDlg::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CAverageDlg *pAvg = (CAverageDlg*) pParam;

      pAvg->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pAvg->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);
      pAvg->m_pCurve->SetProgressWnd(pProg->m_hWnd);

      double dWhere = 0.01 * (double) pAvg->m_CAverageDlgWhere.GetPos();
      pAvg->m_pNewCurve = new CCurveLabel(pAvg->m_pCurve);     // neue Kurve erzeugen
      if (   pAvg->m_pCurve->GetFormat() == CF_ARBITRARY
          || pAvg->m_pCurve->GetFormat() == CF_ARBITRARY_CONTR)
      {
         pAvg->m_pNewCurve->SetFormat(CF_SINGLE_FLOAT);
      }
      if (pAvg->m_pNewCurve)
      {
         pAvg->m_pNewCurve->SetTargetFuncNo(0);
         pAvg->m_pNewCurve->SetColor(0xffffffff);              // Parameter setzen
         pAvg->m_pNewCurve->SetText((char*)LPCTSTR(pAvg->m_strNewName));
         if (pAvg->m_nDivision == 2)
         {
            switch(pAvg->m_nStepIndexFrq)
            {
               case 0: pAvg->m_dAverageRange = pow( 2.0, (double)pAvg->m_nNoOfSteps/9.0); break;
               case 1: pAvg->m_dAverageRange = pow( 2.0, (double)pAvg->m_nNoOfSteps/3.0); break;
               case 2: pAvg->m_dAverageRange = pow( 2.0, (double)pAvg->m_nNoOfSteps);     break;
            }
         }
         else if (pAvg->m_nDivision == 1)
         {
            pAvg->m_dAverageRange = pow(10.0, 1./(double)pAvg->m_nDecade_Divider);
         }

         if (pAvg->m_pCurve->Average(pAvg->m_dAverageRange, dWhere, pAvg->m_pNewCurve))
         {
            pAvg->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pAvg->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pAvg->m_pThread = NULL;
   }
   return uResult;
}

BOOL CAverageDlg::OnInitDialog()
{
   CCurveDialog::OnInitDialog();
   m_CAverageDlgWhere.SetRange(0, 100, true);
   for (int i=10; i<=90; i+=10) m_CAverageDlgWhere.SetTic(i);
   m_CAverageDlgWhere.SetPos(50);
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CAverageDlg::InitCurveParams()
{
   if (m_pCurve)
   {
      bool bLin;
      int nCurveDiv = m_pCurve->GetXDivision();
      if (m_pCurve->GetLocus()) nCurveDiv = m_pCurve->GetLocus();

      if ((nCurveDiv == ETSDIV_LINEAR) || (nCurveDiv == ETSDIV_POLAR))
      {
         bLin = true;
         if (m_nDivision != 0) m_nDivision = 0;
      }
      else
      {
         if (m_nDivision <= 0) m_nDivision = (nCurveDiv == ETSDIV_LOGARITHMIC) ? 1 : 2;
         bLin = false;
      }
      GetDlgItem(IDC_AVG_R_LIN    )->EnableWindow(bLin);
      GetDlgItem(IDC_AVG_R_LOG    )->EnableWindow(!bLin);
      GetDlgItem(IDC_AVG_R_LOG_FRQ)->EnableWindow(!bLin);
      const char * pszText = m_pCurve->GetText();
      if (pszText) m_strNewName.Format(IDS_AVERAGE_CURVE, pszText);
      UpdateData(false);
      m_nDivision = -1;
      OnClickDivsion();
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }
}

void CAverageDlg::OnClickDivsion() 
{
   int nOldDiv = m_nDivision;
   CDataExchange dx(this, true);
	DDX_Radio(&dx, IDC_AVG_R_LIN, m_nDivision);
   if (nOldDiv != m_nDivision)
   {
      int nRange          = SW_HIDE,
          nLineText       = SW_HIDE,
          nLogText        = SW_HIDE,
          nLogText1       = SW_HIDE,
          nFrquText       = SW_HIDE,
          nLogDivider     = SW_HIDE,
          nFrequNoOfSteps = SW_HIDE,
          nFrequStep      = SW_HIDE,
          nAvgUnitx       = SW_HIDE;
      switch(m_nDivision)
      {
         case 0:  // linear
         { 
            char * szUnitx = m_pCurve->GetXUnit();
            if (szUnitx)
            {
               m_strUnitx.Format("[%s]", szUnitx);
               dx.m_bSaveAndValidate = false;
      	      DDX_Text(&dx, IDC_AVG_UNITX, m_strUnitx);
            }
            nRange = nLineText = nAvgUnitx = SW_SHOW;
         } break;
         case 1:  // logarithmisch
            nLogText = nLogText1 = nLogDivider = SW_SHOW;
         break;
         case 2:  // frequenz
            nFrequNoOfSteps = nFrequStep = nFrquText = SW_SHOW;
         break;
      }	
      GetDlgItem(IDC_AVG_RANGE)->ShowWindow(nRange);
      GetDlgItem(IDC_AVG_LINTEXT)->ShowWindow(nLineText);
      GetDlgItem(IDC_AVG_LOGTEXT)->ShowWindow(nLogText);
      GetDlgItem(IDC_AVG_LOGTEXT1)->ShowWindow(nLogText1);
      GetDlgItem(IDC_AVG_FRQUTEXT)->ShowWindow(nFrquText);
      GetDlgItem(IDC_AVG_LOG_DIVIDER)->ShowWindow(nLogDivider);
      GetDlgItem(IDC_AVG_FREQU_NO_OF_STEPS)->ShowWindow(nFrequNoOfSteps);
      GetDlgItem(IDC_AVG_FREQU_STEP)->ShowWindow(nFrequStep);
      GetDlgItem(IDC_AVG_UNITX)->ShowWindow(nAvgUnitx);
   }
}

BOOL CAverageDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
   NMHDR* pNMHDR = (NMHDR*)lParam;
   if (pNMHDR->code == TTN_GETDISPINFO)// && (pNMHDR->idFrom == IDC_AVG_WHERE))
   {
      NMTTDISPINFO*pnmtdi = (NMTTDISPINFO*)pNMHDR;
      int nValue = atoi(pnmtdi->szText);
      wsprintf(pnmtdi->szText, "%d.%02d", nValue/100, nValue%100);
   }
	return CCurveDialog::OnNotify(wParam, lParam, pResult);
}
