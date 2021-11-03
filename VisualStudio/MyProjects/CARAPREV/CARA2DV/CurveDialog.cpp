// CurveDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CurveDialog.h"
#include "MultiMon.h"
#include "..\CaraFrameWnd.h"

#ifdef ETSDEBUG_REPORT
 #define  ETSDEBUG_INCLUDE 
#endif
#include "CEtsDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveDialog 

CCurveDialog::CCurveDialog(UINT nID, CWnd* pParent)
	: CDialog(nID, pParent)
{
	//{{AFX_DATA_INIT(CCurveDialog)
   m_strNewName = _T("");
	//}}AFX_DATA_INIT
   m_pCurves    = NULL;
   m_pCurve     = NULL;
   m_pNewCurve  = NULL;
   m_pThread    = NULL;
   m_pdMarker   = NULL;
   m_pOldCurvePointer = NULL;
   m_bNoSetCurve = false;
   m_bAvoidRecursion = false;
}

CCurveDialog::~CCurveDialog()
{
   OnEndThread(1,1);
   DeleteCurves();
   SAFE_DELETE(m_pdMarker);
}

void CCurveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveDialog)
	DDX_Text(pDX, IDC_NEW_NAME, m_strNewName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveDialog, CDialog)
	//{{AFX_MSG_MAP(CCurveDialog)
   ON_MESSAGE(WM_HELP, OnHelp)
   ON_MESSAGE(WM_ENDTHREAD, OnEndThread)
   ON_BN_CLICKED(IDC_HIDE, OnHide)
   ON_WM_NCLBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	ON_BN_CLICKED(IDC_STOP, OnStop)
   ON_WM_INITMENUPOPUP()
	//}}AFX_MSG_MAP
   ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CCurveDialog 
BOOL CCurveDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_NEW_NAME)->ShowWindow(SW_SHOW);

   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_RESTORE, MF_BYCOMMAND|MF_GRAYED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_SIZE, MF_BYCOMMAND|MF_GRAYED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_MAXIMIZE, MF_BYCOMMAND|MF_GRAYED);

   long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
   lStyle &= ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX);
   ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

   return true;
}
void CCurveDialog::OnCancel()
{
   OnEndThread(DELETE_TEMPCURVE|TERMINATE_CALCULATION, NEWCURVE_IS_NOTOK);
   CDialog::OnCancel();
   CWnd::DestroyWindow();
}

bool CCurveDialog::SetCurve(CCurveLabel *pcl)
{
   if (m_bAvoidRecursion)
      return false;
   m_bAvoidRecursion = true;
   if ((m_pCurve == NULL) || (pcl && (m_pOldCurvePointer != pcl)))
   {
      bool bFailed = false;
      if (pcl->IsInterupted())
      {
         AfxMessageBox(IDS_CANNOT_CALC_CURVE, MB_OK|MB_ICONEXCLAMATION);
         bFailed = true;
      }
      else if (pcl->GetLocus() != 0)
      {
//         AfxMessageBox(IDS_CANNOT_CALC_CURVE3, MB_OK|MB_ICONEXCLAMATION);
//         bFailed = true;
      }
      else if (!pcl->IsSorted())
      {
         if (pcl->IsSortable())
         {
            if (AfxMessageBox(IDS_CANNOT_CALC_CURVE4, MB_OKCANCEL|MB_ICONINFORMATION|MB_DEFBUTTON2)==IDOK)
            {
               pcl->SortXAsc();
            }
            else bFailed = true;
         }
         else
         {
            AfxMessageBox(IDS_CANNOT_CALC_CURVE2, MB_OK|MB_ICONEXCLAMATION);
            bFailed = true;
         }
      }
      if (bFailed && (m_hWnd))
      {
         CWnd *pWnd = GetDlgItem(IDC_CALCULATE);
         if (pWnd) pWnd->EnableWindow(false);
         SetFocus();
         m_bAvoidRecursion = false;
         return false;
      }
      DeleteCurves();
      m_pOldCurvePointer = pcl;
      SAFE_DELETE(m_pdMarker);
      CPlotLabel *ppl = pcl->GetTransformation();
      if (ppl)
      {
         m_PlotSettings.xDivision = ppl->GetXDivision();
         m_PlotSettings.yDivision = ppl->GetYDivision();
         m_PlotSettings.xNumMode  = ppl->GetXNumModeEx();
         m_PlotSettings.yNumMode  = ppl->GetYNumModeEx();
         double dM1, dM2;
         if (ppl->GetMarker(0, dM1) && ppl->GetMarker(1, dM2))
         {
            m_pdMarker = new double[2];
            m_pdMarker[0] = dM1;
            m_pdMarker[1] = dM2;
         }
      }
      else
      {
         m_PlotSettings.xDivision = pcl->GetXDivision();
         m_PlotSettings.yDivision = pcl->GetYDivision();
         m_PlotSettings.xNumMode  = pcl->GetXNumMode();
         m_PlotSettings.yNumMode  = pcl->GetYNumMode();
      }
      m_pCurve = new CCurveLabel(pcl);
      if (m_pCurve)
      {
         m_pCurve->CopyValues(pcl);
         m_pCurve->SetText(pcl->GetText());
         InitCurveParams();
      }
   }
   m_bAvoidRecursion = false;
   return true;
}

LRESULT CCurveDialog::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI  = (HELPINFO*) lParam;

   if (pHI->dwContextId == 0)
   {
      CDialog::OnHelp();
      return true;
   }

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

LRESULT CCurveDialog::OnEndThread(WPARAM wParam, LPARAM lParam)
{
   //if (m_pThread)
   //{
   //   if (m_pThread->m_hThread != INVALID_HANDLE_VALUE)
   //   {
   //      int nCount = 0;
   //      DWORD nExitCode = 0;
   //      if ((lParam & TERMINATE_CALCULATION) && 
   //          (::GetExitCodeThread(m_pThread->m_hThread, &nExitCode) && (nExitCode==STILL_ACTIVE)))
   //      {
   //         GetDlgItem(IDC_PROGRESS)->ShowWindow(SW_HIDE);
   //      }
   //      while (::GetExitCodeThread(m_pThread->m_hThread, &nExitCode) && (nExitCode==STILL_ACTIVE))
   //      {
   //         if (nCount++ == 50)
   //         {
   //            ::TerminateThread(m_pThread->m_hThread, 1);
   //         }
   //         Sleep(0);
   //      }
   //   }
   //   delete m_pThread;

   if (lParam==NEWCURVE_IS_OK && IsWindow(m_hWnd))
   {
      CWnd *pWnd = GetParent();
      if (pWnd) pWnd->PostMessage(WM_NEWCURVE, (WPARAM) m_pNewCurve, 1);
      m_pNewCurve = NULL;
   }
   else if (m_pNewCurve)
   {
      delete m_pNewCurve;
      m_pNewCurve = NULL;
   }
   //   m_pThread = NULL;
   //}

   if (wParam & DELETE_TEMPCURVE)
   {
      if (m_pCurve)
      {
         delete m_pCurve;
         m_pCurve = NULL;
      }
   }

   if (wParam & DESTROY_WINDOW)
   {
      CWnd::DestroyWindow();
   }
   return 0;
}

LRESULT CCurveDialog::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
   if (wParam == SC_MAXIMIZE) 
      wParam = SC_RESTORE;

   if (wParam == SC_MINIMIZE)
   {
      WINDOWPLACEMENT wpD;
      MONITORINFOEX   mix;

      wpD.length = sizeof(WINDOWPLACEMENT);
      long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
      lStyle |= WS_MINIMIZEBOX;
      ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

      lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
      lStyle &= ~WS_EX_CONTEXTHELP;
      ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);

      ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), wParam, MF_BYCOMMAND|MF_GRAYED);
      ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_RESTORE, MF_BYCOMMAND|MF_ENABLED);

      ((CCaraFrameWnd*)AfxGetMainWnd())->GetMonitorInfo(mix, m_hWnd);
      GetWindowPlacement(&wpD);
      if (!::PtInRect(&mix.rcMonitor, wpD.ptMinPosition))
      {
         wpD.ptMinPosition.x = mix.rcMonitor.left+10;
         wpD.ptMinPosition.y = mix.rcMonitor.bottom-50;
      }
      wpD.flags   = WPF_SETMINPOSITION;
      wpD.showCmd = SW_SHOWMINIMIZED;
      SetWindowPlacement(&wpD);
      return 0;
   }
   else if (wParam == SC_RESTORE)
   {
      long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
      lStyle &= ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX);
      ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

      lStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
      lStyle |= WS_EX_CONTEXTHELP;
      ::SetWindowLong(m_hWnd, GWL_EXSTYLE, lStyle);

      ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), wParam, MF_BYCOMMAND|MF_GRAYED);
      ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_MINIMIZE, MF_BYCOMMAND|MF_ENABLED);
   }
   return DefWindowProc(WM_SYSCOMMAND, wParam, lParam);
}

void CCurveDialog::OnHide() 
{
   OnSysCommand(SC_MINIMIZE, 0);
}

void CCurveDialog::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
   CDialog::OnNcLButtonDblClk(nHitTest, point);
   OnHide();
}

void CCurveDialog::DeleteCurves()
{
   if (m_pCurve)
   {
      delete m_pCurve;
      m_pCurve = NULL;
      m_pOldCurvePointer = NULL;
   }
   if (m_pNewCurve)
   {
      delete m_pNewCurve;
      m_pNewCurve = NULL;
   }
}

CWnd* CCurveDialog::GetDlgItem(int nID)
{
   CWnd *pWnd = CWnd::GetDlgItem(nID);
   ASSERT(pWnd != NULL);
   return pWnd;
}

void CCurveDialog::OnCalculate() 
{
	OnOK();
}

void CCurveDialog::OnStop() 
{
	OnCancel();
}

bool CCurveDialog::IsActive()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return true;
   }
   return false;
}


LRESULT CCurveDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//   REPORT("Msg %x", message);
   if (message == WM_ACTIVATE)
   {
      WORD fActive = LOWORD(wParam);           // activation flag 
      WORD fMinimized = (BOOL) HIWORD(wParam); // minimized flag 
      HWND hwndPrevious = (HWND) lParam;       // window handle 
      if (WA_INACTIVE != fActive)
      {
         if (m_pThread || m_bNoSetCurve)
         {

         }
         else if (m_pCurves)
         {
            CLabel *pl = m_pCurves->GetLabel(0);
            if (pl && pl->IsKindOf(RUNTIME_CLASS(CCurveLabel)))
            {
               if (!SetCurve((CCurveLabel*) pl))
               {
                  ::SendMessage(::GetParent(m_hWnd), WM_RESETPICKSTATES, 0, 0);
               }
            }
         }
      }
   }
	return CDialog::WindowProc(message, wParam, lParam);
}

