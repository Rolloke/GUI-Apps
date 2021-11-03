// CurveFileDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "CurveFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveFileDlg

IMPLEMENT_DYNAMIC(CCurveFileDlg, CFileDialog)
long CCurveFileDlg::gm_lOldProc = 0;

CCurveFileDlg::CCurveFileDlg(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
   m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_READWAVEFILE);
   m_ofn.Flags |= OFN_ENABLETEMPLATE;
   m_lpszTemplateName = m_ofn.lpTemplateName;
   m_nPrecision = 0;
   m_bSigned    = true;
   m_nFileTypeSel = -1;
}


BEGIN_MESSAGE_MAP(CCurveFileDlg, CFileDialog)
	//{{AFX_MSG_MAP(CCurveFileDlg)
	ON_WM_CONTEXTMENU()
   ON_MESSAGE(WM_HELP, OnHelp)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CCurveFileDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	return;
}

BOOL CCurveFileDlg::OnInitDialog()
{
   BOOL bResult = CFileDialog::OnInitDialog();
   HWND hwndParent = ::GetParent(m_hWnd);
   HWND hwndCombo  = ::GetDlgItem(hwndParent, 0x0470);
   gm_lOldProc = ::SetWindowLong(hwndCombo, GWL_WNDPROC, (LONG)WindowProc);
   if (::GetWindowLong(hwndCombo, GWL_USERDATA) == 0)
   {
      ::SetWindowLong(hwndCombo, GWL_USERDATA, (LONG)this);
   }
   m_nFileTypeMax = ::SendMessage(hwndCombo, CB_GETCOUNT, 0, 0);
   return bResult;
}

BOOL CCurveFileDlg::OnFileNameOK( )
{
   if (UpdateData(true))
      return CFileDialog::OnFileNameOK();
   return false;
}

void CCurveFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateCurve)
   DDX_CBIndex(pDX, IDC_FILE_COMBO_FORMAT, m_nPrecision);
	DDX_Check(pDX, IDC_FILE_CK_SIGNED, m_bSigned);
	//}}AFX_DATA_MAP
}

LRESULT CCurveFileDlg::OnHelp(WPARAM wParam, LPARAM lParam)
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

void CCurveFileDlg::OnDestroy() 
{
   HWND hwndParent = ::GetParent(m_hWnd);
   HWND hwndCombo  = ::GetDlgItem(hwndParent, 0x0470);
   gm_lOldProc = ::SetWindowLong(hwndCombo, GWL_WNDPROC, (LONG)gm_lOldProc);
   gm_lOldProc = 0;
	CFileDialog::OnDestroy();
}

LRESULT CALLBACK CCurveFileDlg::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldProc == 0) return 0;
   if (uMsg == CB_GETCURSEL)
   {
      LRESULT lRes = CallWindowProc((WNDPROC) gm_lOldProc, hwnd, uMsg, wParam, lParam);
      CCurveFileDlg*pThis = (CCurveFileDlg*) ::GetWindowLong(hwnd, GWL_USERDATA);
      if (pThis) pThis->GetCurSelFileType(lRes);
      return lRes;
   }
   return CallWindowProc((WNDPROC) gm_lOldProc, hwnd, uMsg, wParam, lParam);
}

void CCurveFileDlg::GetCurSelFileType(long nSel)
{
   if (nSel != m_nFileTypeSel)
   {
      m_nFileTypeSel = nSel;
      bool bPrecision = false;
      bool bSigned    = false;
      if ((m_nFileTypeSel == m_nFileTypeMax-1) || // Alle Dateien
          (m_nFileTypeSel == m_nFileTypeMax-2))   // Wavedateien
      {
         bPrecision = bSigned = true;
      }
      else if (m_nFileTypeSel == 0)
      {
      }
      else bPrecision = true;
      GetDlgItem(IDC_FILE_COMBO_FORMAT)->EnableWindow(bPrecision);
      GetDlgItem(IDC_FILE_CK_SIGNED)->EnableWindow(bSigned);
   }
}

