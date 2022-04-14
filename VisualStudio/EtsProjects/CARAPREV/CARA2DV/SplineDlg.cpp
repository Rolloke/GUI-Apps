// SplineDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "SplineDlg.h"
#include "..\Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CSplineDlg 
extern "C" __declspec(dllimport) BOOL DDX_TextVar(CDataExchange*, int, int, double&, bool bThrow=true);


CSplineDlg::CSplineDlg(CWnd* pParent /*=NULL*/)
	: CCurveDialog(CSplineDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CSplineDlg)
   m_nNumSteps   = 1;
   m_dRangeX1    = 0.0;
   m_dRangeX2    = 0.0;
   //}}AFX_DATA_INIT
   m_strUnitx = _T("");
   m_dMinX = -1.0;
   m_dMaxX =  1.0;
}

void CSplineDlg::DoDataExchange(CDataExchange* pDX)
{
	CCurveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplineDlg)
	DDX_Text(pDX, IDC_SPLINE_NUMSTEPS, m_nNumSteps);
	DDV_MinMaxInt(pDX, m_nNumSteps, 1, 20);
	//}}AFX_DATA_MAP

   DDX_TextVar(pDX, IDC_SPLINE_RANGEX1, DBL_DIG, m_dRangeX1);
	DDX_TextVar(pDX, IDC_SPLINE_RANGEX2, DBL_DIG, m_dRangeX2);

   if (pDX->m_bSaveAndValidate)
   {
      if (m_dRangeX1 > m_dRangeX2)
      {
         swap(m_dRangeX1, m_dRangeX2);
         pDX->m_bSaveAndValidate = false;
      }
      if (m_dRangeX1 < m_dMinX)
      {
         m_dRangeX1 = m_dMinX;
         pDX->m_bSaveAndValidate = false;
      }
      if (m_dRangeX2 > m_dMaxX)
      {
         m_dRangeX2 = m_dMaxX;
         pDX->m_bSaveAndValidate = false;
      }
      if (!pDX->m_bSaveAndValidate)
      {
      	DDX_Text(pDX, IDC_SPLINE_RANGEX1, m_dRangeX1);
         DDX_Text(pDX, IDC_SPLINE_RANGEX2, m_dRangeX2);
         pDX->m_bSaveAndValidate = true;
      }
   }
   else
   {
	   DDX_Text(pDX, IDC_SPLINE_UNITX, m_strUnitx);
   }
}


BEGIN_MESSAGE_MAP(CSplineDlg, CCurveDialog)
	//{{AFX_MSG_MAP(CSplineDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSplineDlg 
void CSplineDlg::OnOK() 
{
   if (m_pCurve && UpdateData(true))
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

UINT CSplineDlg::ThreadFunction(void *pParam)
{
   UINT uResult = 2;
   if (pParam)
   {
      CSplineDlg *pSpline = (CSplineDlg*) pParam;

      pSpline->GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_HIDE);

      CProgressCtrl *pProg = (CProgressCtrl*) pSpline->GetDlgItem(IDC_PROGRESS);
      pProg->SetRange(0, CURVE_PROGRESS_RANGE);                // Fortschrittanzeige
      pProg->ShowWindow(SW_SHOW);
      pSpline->m_pCurve->SetProgressWnd(pProg->m_hWnd);

      pSpline->m_pNewCurve = new CCurveLabel(pSpline->m_pCurve);
      if (pSpline->m_pNewCurve)
      {
         pSpline->m_pNewCurve->SetTargetFuncNo(0);
         pSpline->m_pNewCurve->SetColor(0xffffffff);
         pSpline->m_pNewCurve->SetText((char*)LPCTSTR(pSpline->m_strNewName));
         if (pSpline->m_dRangeX1  > pSpline->m_dRangeX2)  swap(pSpline->m_dRangeX1,  pSpline->m_dRangeX2);
         if (pSpline->m_pCurve->Spline(pSpline->m_nNumSteps, pSpline->m_pNewCurve, pSpline->m_dRangeX1, pSpline->m_dRangeX2))
         {
            pSpline->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_OK);
            uResult = 0;
         }
         else
         {
            pSpline->PostMessage(WM_ENDTHREAD, DELETE_TEMPCURVE|DESTROY_WINDOW, NEWCURVE_IS_NOTOK);
            uResult = 1;
         }
      }
      pSpline->m_pThread = NULL;
   }
   return uResult;
}

BOOL CSplineDlg::OnInitDialog() 
{
   CCurveDialog::OnInitDialog();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CSplineDlg::InitCurveParams()
{
   if (m_pCurve)
   {
      if (m_pCurve->GetLocus())
      {
         m_strUnitx = _T(m_pCurve->GetLocusUnit());
         m_dMinX    = m_pCurve->GetLocusValue(0);
         m_dMaxX    = m_pCurve->GetLocusValue(m_pCurve->GetNumValues()-1);
      }
      else
      {
         m_strUnitx = _T(m_pCurve->GetXUnit());
         m_dMinX    = m_pCurve->GetXMin();
         m_dMaxX    = m_pCurve->GetXMax();
      }
      if (m_pdMarker)
      {
         m_dRangeX1 = m_pdMarker[0];
         m_dRangeX2 = m_pdMarker[1];
      }
      else
      {
         m_dRangeX1 = m_dMinX;
         m_dRangeX2 = m_dMaxX;
      }
      m_dMinX   -= 1e-13;
      m_dMaxX   += 1e-13;
      const char * pszText = m_pCurve->GetText();
      if (pszText) m_strNewName.Format(IDS_SPLINE_CURVE, pszText);
      UpdateData(false);
      GetDlgItem(IDC_CALCULATE)->EnableWindow(true);
   }
}

