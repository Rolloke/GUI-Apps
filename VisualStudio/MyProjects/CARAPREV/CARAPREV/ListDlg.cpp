// ListDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "ListDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


long CListDlg::gm_lOldListWndProc = 0;
long CListDlg::gm_lOldEditCtrlWndProc = 0;
/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CListDlg 


CListDlg::CListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListDlg)
	m_strSelected = _T("");
	//}}AFX_DATA_INIT
   m_pCallBack = NULL;
   m_pParam    = NULL;
   m_nCursel   = 0;
   m_szSize.cx = 0;
   m_szSize.cy = 0;
   m_bResize   = true;
   m_bOkButton = false;
   m_pNMHdr    = NULL;
   m_pDragImage= NULL;
   m_nSubItem  = 0;
}

CListDlg::~CListDlg()
{
   DeleteDragImage();
}

void CListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListDlg)
	DDX_Control(pDX, IDC_LST_LIST, m_List);
	DDX_LBString(pDX, IDC_LST_LIST, m_strSelected);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CListDlg, CDialog)
	//{{AFX_MSG_MAP(CListDlg)
	ON_WM_SIZE()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_NOTIFY(NM_DBLCLK, IDC_LST_LIST, OnDblclkLstList)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LST_LIST, OnRclickLstList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LST_LIST, OnEndlabeleditLstList)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LST_LIST, OnBeginlabeleditLstList)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LST_LIST, OnBegindragLstList)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONUP()
	ON_NOTIFY(LVN_BEGINRDRAG, IDC_LST_LIST, OnBeginrdragLstList)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CListDlg 

void CListDlg::OnDblclkLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
   if (m_bOkButton)
   {
      if (m_pCallBack)
         *pResult = m_pCallBack(this, ONDBLCLKLSTLIST);
   }
   else if (UpdateData(true))
   {
      if (m_pCallBack)
         m_pCallBack(this, ONDBLCLKLSTLIST);
      OnOK();
   	*pResult = 0;
   }

   m_pNMHdr = NULL;
}

BOOL CListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
   SetIcon(hIcon, true);
   if (!m_strSelected.IsEmpty()) SetWindowText(m_strSelected);

   CWnd *pWndOk = GetDlgItem(IDOK);
   if (pWndOk)
   {
      if (m_bOkButton)
      {
         CRect rcDlg, rcItem;
         WINDOWPLACEMENT wpl;
         wpl.length = sizeof(wpl);
         ::GetWindowPlacement(pWndOk->m_hWnd, &wpl);
         GetWindowRect(&rcDlg);
         MapWindowPoints(NULL, (LPPOINT) &rcDlg, 2);
         pWndOk->GetWindowRect(&rcItem);
         m_szButton = rcItem.Size();
         MapWindowPoints(NULL, (LPPOINT) &rcItem, 2);
         m_ptOk.x = rcItem.left  - rcDlg.left;
         m_ptOk.y = rcDlg.bottom - rcItem.top;
      }
      else pWndOk->ShowWindow(SW_HIDE);
   }

   if ((m_szSize.cx > 0) && (m_szSize.cy > 0))
   {
      SetWindowPos(NULL, 0, 0, m_szSize.cx, m_szSize.cy, SWP_NOZORDER|SWP_NOMOVE);
   }

   if (!m_bResize)
   {
      ModifyStyle(WS_THICKFRAME|DS_3DLOOK, WS_POPUP|DS_MODALFRAME, 0);
   }

   if (gm_lOldListWndProc == 0)
      gm_lOldListWndProc = ::SetWindowLong(GetDlgItem(IDC_LST_LIST)->m_hWnd, GWL_WNDPROC, (long)ListSubClassProc);

   if (m_pCallBack)
   {
      return m_pCallBack(this, ONINITDIALOG);
   }
	return false;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void CListDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
   if (m_bOkButton)
   {
      GetDlgItem(IDOK)->MoveWindow(m_ptOk.x, cy - m_ptOk.y, m_szButton.cx, m_szButton.cy, true);
      m_List.MoveWindow(5, 5, cx - 10, cy - m_ptOk.y - 10, true);
   }
   else m_List.MoveWindow(5, 5, cx - 10, cy - 10, true);
   if (m_bResize && m_pCallBack)
   {
      m_pCallBack(this, ONSIZE);
   }
}

LRESULT CListDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
   if (pHI->dwContextId == 0) return true;

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

void CListDlg::OnDestroy() 
{
   CImageList*pIL = m_List.SetImageList(NULL, TVSIL_NORMAL);
   if (pIL && pIL->m_hImageList)
   {
      ImageList_Destroy(pIL->Detach());
   }
   HICON hIcon = GetIcon(true);
   if (hIcon) DestroyIcon(hIcon);
	CDialog::OnDestroy();
}

void CListDlg::OnOK() 
{
   m_nCursel = m_List.GetSelectionMark();
   if (m_pCallBack)
   {
      if (m_pCallBack(this, ONOK) != 0) CDialog::OnOK();
      else                              CDialog::OnCancel();
   }
   else CDialog::OnOK();
}

void CListDlg::OnRclickLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
   m_nCursel = m_List.GetSelectionMark();
   if (m_pCallBack) *pResult = m_pCallBack(this, ONRCLICKLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;
}

void CListDlg::OnBeginlabeleditLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
//	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   LVHITTESTINFO lvHTI;
   lvHTI.pt = m_ptOk;
   m_List.SendMessage(LVM_SUBITEMHITTEST, 0, (LPARAM)&lvHTI);
   if ((lvHTI.iItem != -1) && (lvHTI.flags & LVHT_ONITEM) && (lvHTI.iSubItem > 0))
   {
      CEdit* pEdit = m_List.GetEditControl();
      if (pEdit)
      {
         if (::GetWindowLong(m_List.m_hWnd, GWL_USERDATA)==0)
         {
            m_nCursel  = lvHTI.iItem;
            m_nSubItem = lvHTI.iSubItem;
            gm_lOldEditCtrlWndProc = ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, (long)EditCtrlWndProc);
            ::SetWindowLong(m_List.m_hWnd, GWL_USERDATA, (long)this);
            CString str = m_List.GetItemText(m_nCursel, m_nSubItem);
            pEdit->SetWindowText(str);
         }
      }
   }

   if (m_pCallBack) *pResult = *pResult = m_pCallBack(this, ONBEGINLABELEDITLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;
}

LRESULT CALLBACK CListDlg::EditCtrlWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (gm_lOldEditCtrlWndProc == 0) return 0;

   if (uMsg == WM_WINDOWPOSCHANGING)
   {
      WINDOWPOS *pwp = (WINDOWPOS*)lParam;
      HWND hwndP = ::GetParent(hwnd);
      CRect rc;
      CListDlg *pDlg = (CListDlg*) ::GetWindowLong(hwndP, GWL_USERDATA);
      if (pDlg)
      {
         ListView_GetSubItemRect(hwndP, pDlg->m_nCursel, pDlg->m_nSubItem, LVIR_BOUNDS, (RECT*)&rc);
         pwp->x = rc.left;
         pwp->y = rc.top;
         pwp->cx= rc.Width();
         pwp->cy= rc.Height();
      }
   }  
   return CallWindowProc((WNDPROC)gm_lOldEditCtrlWndProc, hwnd, uMsg, wParam, lParam);
}

void CListDlg::OnEndlabeleditLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
//	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
   if (m_pCallBack) *pResult = m_pCallBack(this, ONENDLABELEDITLSTLIST);
   else             *pResult = 0;

   CEdit* pEdit = m_List.GetEditControl();
   if (pEdit)
   {
      ::SetWindowLong(pEdit->m_hWnd, GWL_WNDPROC, gm_lOldEditCtrlWndProc);
   }
   m_nSubItem = 0;
   ::SetWindowLong(m_List.m_hWnd, GWL_USERDATA, (long)0);
   m_pNMHdr = NULL;
}

void CListDlg::OnBeginrdragLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnBegindragLstList(pNMHDR, pResult);
}
void CListDlg::OnBegindragLstList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
   if (m_pCallBack) *pResult = m_pCallBack(this, ONBEGINDRAGLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;

   if (*pResult)                                               // jetzt Draggen ?
   {
      NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;
      MapWindowPoints(&m_List, &pNMListView->ptAction, 1);
      m_pDragImage = m_List.CreateDragImage(pNMListView->iItem, &pNMListView->ptAction);

      if (m_pDragImage)
      {
         POINT pt  = {0, 0};
         m_nCursel = pNMListView->iItem;
         m_pDragImage->BeginDrag(0, pt);
         m_pDragImage->DragEnter(&m_List, pNMListView->ptAction);
      }
   }
}

void CListDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
/*
   if (!(nFlags&MK_INLIST))
   {
      m_List.
   }
*/
   NM_LISTVIEW nmLV;
   MapWindowPoints(&m_List, &point, 1);
	if (m_pDragImage)
   {
      ZeroMemory(&nmLV, sizeof(NM_LISTVIEW));
      LVHITTESTINFO lvHti = {{point.x, point.y}, 0, 0, 0};
      nmLV.ptAction = point;
      nmLV.uChanged = nFlags;
      nmLV.iItem    = m_List.HitTest(&lvHti);
      nmLV.iSubItem = lvHti.iSubItem;
      nmLV.uNewState= lvHti.flags;
      m_pNMHdr = (NMHDR*)&nmLV;
      m_pDragImage->DragMove(point);
   }
   if (m_pCallBack) m_pCallBack(this, WM_MOUSEMOVE);
   m_pNMHdr = NULL;
	CDialog::OnMouseMove(nFlags, point);
}

void CListDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
   EndDrag(nFlags, point, WM_LBUTTONUP);
	CDialog::OnLButtonUp(nFlags, point);
}

void CListDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
   EndDrag(nFlags, point, WM_RBUTTONUP);
	CDialog::OnRButtonUp(nFlags, point);
}

void CListDlg::DeleteDragImage()
{
	if (m_pDragImage)
   {
      delete m_pDragImage;
      m_pDragImage = NULL;
   }
}

void CListDlg::DragLeave()
{
	if (m_pDragImage)
   {
      m_pDragImage->DragLeave(this);
   }
}

void CListDlg::EndDrag(UINT nFlags, CPoint point, UINT nMsg)
{
	if (m_pDragImage)
   {
      NMLISTVIEW nmLV;
      ZeroMemory(&nmLV, sizeof(NMLISTVIEW));
      LVHITTESTINFO lvHti = {{point.x, point.y}, 0, 0, 0};
      nmLV.ptAction = point;
      nmLV.uChanged = nFlags;
      nmLV.iItem    = m_List.HitTest(&lvHti);
      nmLV.iSubItem = lvHti.iSubItem;
      nmLV.uNewState= lvHti.flags;
      m_pNMHdr = (NMHDR*)&nmLV;
      m_pDragImage->EndDrag();
   }
   if (m_pCallBack) m_pCallBack(this, ONENDDRAGLSTLIST);
   if (m_pDragImage) m_nCursel = -1;
   m_pNMHdr  = NULL;
   DeleteDragImage();
}

LRESULT CALLBACK CListDlg::ListSubClassProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch (Msg)
   {
      case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_RBUTTONUP:case WM_LBUTTONDOWN:
      {
         HWND hWndParent = ::GetParent(hWnd);
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         ::MapWindowPoints(hWnd, hWndParent, &ptMouse, 1);
         LPARAM lParamP = MAKELONG(ptMouse.x, ptMouse.y);
         WPARAM wParamP = wParam|MK_INLIST;
         ::SendMessage(hWndParent, Msg, wParamP, lParamP);
      } break;
      case WM_DESTROY:
      {
         LRESULT lResult = CallWindowProc((WNDPROC)gm_lOldListWndProc, hWnd, Msg, wParam, lParam);
         SetWindowLong(hWnd, GWL_WNDPROC, gm_lOldListWndProc);
         gm_lOldListWndProc = 0;
         return lResult;
      }
   }
   return CallWindowProc((WNDPROC)gm_lOldListWndProc, hWnd, Msg, wParam, lParam);
}


void CListDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
   m_ptOk = point;
	CDialog::OnLButtonDown(nFlags, point);
}
