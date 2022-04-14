// NonModalDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA3DV.h"
#include "NonModalDlg.h"
#include "resource.h"
#include "..\CaraFrameWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CNonModalDlg 
/*
extern void AFXAPI AfxTextFloatFormat(CDataExchange* pDX, int nIDC,
	void* pData, double value, int nSizeGcvt);
*/

CNonModalDlg::CNonModalDlg(UINT nID, CWnd* pParent)
	: CDialog(nID, pParent)
{
	//{{AFX_DATA_INIT(CNonModalDlg)
	//}}AFX_DATA_INIT
   m_dwChanges = 0;
}


void CNonModalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNonModalDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNonModalDlg, CDialog)
	//{{AFX_MSG_MAP(CNonModalDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CNonModalDlg 
void CNonModalDlg::OnCancel() 
{
   GetParent()->SendMessage(WM_APPLY_TO_VIEW, INVALIDATE_VIEW, NULL);
   CWnd::DestroyWindow();
}

bool CNonModalDlg::OnApplyNow()
{
   if (UpdateData(true))
   {
      GetParent()->SendMessage(WM_APPLY_TO_VIEW, 0, (LPARAM)this);
      SetChanged(0, 0xffffffff);
      return true;
   }
   else UpdateData(false);
   return false;
}

LRESULT CNonModalDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
   if (pHI->dwContextId ==          0) return true;
   if (pHI->dwContextId == IDC_STATIC) return true;

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

void CNonModalDlg::MiniMize()
{
   WINDOWPLACEMENT wndpl;
   MONITORINFOEX   mix;
   wndpl.length = sizeof(WINDOWPLACEMENT);

   long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
   lStyle |= WS_MINIMIZEBOX;
   ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

   ((CCaraFrameWnd*)AfxGetMainWnd())->GetMonitorInfo(mix, m_hWnd);
   GetWindowPlacement(&wndpl);
   if (!::PtInRect(&mix.rcMonitor, wndpl.ptMinPosition))
   {
      wndpl.ptMinPosition.x = mix.rcMonitor.left+10;
      wndpl.ptMinPosition.y = mix.rcMonitor.bottom-50;
   }
   GetWindowPlacement(&wndpl);
   wndpl.flags   = WPF_SETMINPOSITION;
   wndpl.showCmd = SW_SHOWMINIMIZED;
   wndpl.ptMinPosition.x = wndpl.rcNormalPosition.left;
   wndpl.ptMinPosition.y = wndpl.rcNormalPosition.top;
   SetWindowPlacement(&wndpl);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_RESTORE, MF_BYCOMMAND|MF_ENABLED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_MINIMIZE, MF_BYCOMMAND|MF_GRAYED);
}

void CNonModalDlg::Restore()
{
   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);

   long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
   lStyle &= ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX);
   ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);

   GetWindowPlacement(&wndpl);
   wndpl.flags   = WPF_SETMINPOSITION;
   wndpl.showCmd = SW_SHOWNORMAL;
   CSize size = ((CRect*)&wndpl.rcNormalPosition)->Size();
   wndpl.rcNormalPosition.left   = wndpl.ptMinPosition.x;
   wndpl.rcNormalPosition.right  = wndpl.rcNormalPosition.left + size.cx;
   wndpl.rcNormalPosition.top    = wndpl.ptMinPosition.y;
   wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top + size.cy;
   SetWindowPlacement(&wndpl);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_RESTORE, MF_BYCOMMAND|MF_GRAYED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_MINIMIZE, MF_BYCOMMAND|MF_ENABLED);
}

void CNonModalDlg::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
   CDialog::OnNcLButtonDblClk(nHitTest, point);

   WINDOWPLACEMENT wndpl;
   wndpl.length = sizeof(WINDOWPLACEMENT);
   GetWindowPlacement(&wndpl);
   if (wndpl.showCmd == SW_NORMAL)
      MiniMize();
   else
      Restore();

}

void CNonModalDlg::OnSysCommand(UINT nID, LPARAM lParam) 
{
   if (nID == SC_MAXIMIZE) 
      nID = SC_RESTORE;
   switch (nID)
   {
      case SC_MINIMIZE: MiniMize(); break;
      case SC_RESTORE:  Restore();
      default : CWnd::OnSysCommand(nID, lParam);
   }
}

BOOL CNonModalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   SetChanged(0, 0x0ffffffff);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_RESTORE, MF_BYCOMMAND|MF_GRAYED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_SIZE, MF_BYCOMMAND|MF_GRAYED);
   ::EnableMenuItem(::GetSystemMenu(m_hWnd, FALSE), SC_MAXIMIZE, MF_BYCOMMAND|MF_GRAYED);
   long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
   lStyle &= ~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX);
   ::SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}
/*
void CNonModalDlg::DDX_TextVar(CDataExchange *pDX, int nID, int nAcc, double &dVar)
{
   ::AfxTextFloatFormat(pDX, nID, &dVar, dVar, nAcc);
}
*/
void CNonModalDlg::SetChanged(DWORD dwChanged, DWORD dwUnchanged)
{
   m_dwChanges &= ~dwUnchanged;
   m_dwChanges |= dwChanged;
   CWnd *pWnd = GetDlgItem(ID_APPLY_NOW);
   if (pWnd)
   {
      pWnd->EnableWindow((m_dwChanges != 0) ? true : false);
   }
}

bool CNonModalDlg::IsActive()
{
   if (m_hWnd)
   {
      ASSERT(::IsWindow(m_hWnd));
      return true;
   }
   return false;
}

