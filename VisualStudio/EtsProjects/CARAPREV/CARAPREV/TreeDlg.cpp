// TreeDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "TreeDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTreeDlg 
long CTreeDlg::gm_lOldListWndProc = 0;


CTreeDlg::CTreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeDlg)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
   m_pCallBack    = NULL;
   m_pParam       = NULL;
   m_pHelp        = NULL;
   m_pParamHelp   = NULL;
   m_pNMHdr       = NULL;
   m_pDragImage   = NULL;
   m_szSize.cx = 0;
   m_szSize.cy = 0;
   m_bResize   = true;
   m_bOkButton = false;
   memset(&m_TvItem, 0, sizeof(TVITEM));
}


void CTreeDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeDlg)
	DDX_Control(pDX, IDC_TREE_TREECTRL, m_TreeCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTreeDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TREECTRL, OnDblclkTreectrl)
	ON_WM_SIZE()
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TREECTRL, OnRclickTreeTreectrl)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_TREECTRL, OnBegindragTreeTreectrl)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE_TREECTRL, OnBeginlabeleditTreeTreectrl)
	ON_NOTIFY(TVN_BEGINRDRAG, IDC_TREE_TREECTRL, OnBeginrdragTreeTreectrl)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_TREECTRL, OnDeleteitemTreeTreectrl)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_TREECTRL, OnEndlabeleditTreeTreectrl)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CTreeDlg 

void CTreeDlg::OnDblclkTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
   if (m_bOkButton)
   {
   	*pResult = m_pCallBack(this, ONDBLCLKLSTLIST);
   }
   else
   {
	   m_TvItem.hItem = m_TreeCtrl.GetSelectedItem();
      if (m_TvItem.hItem && !m_TreeCtrl.ItemHasChildren(m_TvItem.hItem))
      {
         m_TreeCtrl.GetItem(&m_TvItem);
         CDialog::OnOK();
//         SendMessage(WM_COMMAND, IDOK, 0);
   	   *pResult = 1;
      }
   }
   m_pNMHdr = NULL;
}

BOOL CTreeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
   HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
   SetIcon(hIcon, true);
   if (m_ImageList.m_hImageList)
      m_TreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

   if (!m_strWndTitle.IsEmpty()) SetWindowText(m_strWndTitle);

   CWnd *pWndOk = GetDlgItem(IDOK);
   if (m_bOkButton)
   {
      CRect rcDlg, rcItem;
      GetWindowRect(&rcDlg);
      MapWindowPoints(NULL, (LPPOINT) &rcDlg, 2);
      pWndOk->GetWindowRect(&rcItem);
      m_szButton = rcItem.Size();
      MapWindowPoints(NULL, (LPPOINT) &rcItem, 2);
      m_ptOk.x = rcItem.left  - rcDlg.left;
      m_ptOk.y = rcDlg.bottom - rcItem.top;
   }
   else pWndOk->ShowWindow(SW_HIDE);

   if ((m_szSize.cx > 0) && (m_szSize.cy > 0))
      SetWindowPos(NULL, 0, 0, m_szSize.cx, m_szSize.cy, SWP_NOZORDER|SWP_NOMOVE);

   if (!m_bResize)
   {
      ModifyStyle(WS_THICKFRAME|DS_3DLOOK, WS_POPUP|DS_MODALFRAME, 0);
   }

   if (gm_lOldListWndProc == 0)
      gm_lOldListWndProc = ::SetWindowLong(GetDlgItem(IDC_TREE_TREECTRL)->m_hWnd, GWL_WNDPROC, (long)ListSubClassProc);

   if (m_pCallBack)
   {
      return m_pCallBack(this, ONINITDIALOG);
   }

   if (m_pCallBack)
   {
      return m_pCallBack(this, ONINITDIALOG);
   }
	return false;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void CTreeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
   if (m_bOkButton)
   {
      GetDlgItem(IDOK)->MoveWindow(m_ptOk.x, cy - m_ptOk.y, m_szButton.cx, m_szButton.cy, true);
      m_TreeCtrl.MoveWindow(5, 5, cx - 10, cy - m_ptOk.y - 10, true);
   }
   else m_TreeCtrl.MoveWindow(5, 5, cx - 10, cy - 10, true);
}

LRESULT CTreeDlg::OnHelp(WPARAM wParam, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
   if (pHI->dwContextId == 0) return true;

   if (m_pHelp && pHI->iCtrlId == IDC_TREE_TREECTRL)
   {
      m_pHelp(this, pHI);
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

void CTreeDlg::OnDestroy() 
{
   HICON hIcon = GetIcon(true);
   if (hIcon) DestroyIcon(hIcon);
	CDialog::OnDestroy();
}

void CTreeDlg::OnOK() 
{
   m_TvItem.hItem = m_TreeCtrl.GetSelectedItem();
   m_TreeCtrl.GetItem(&m_TvItem);
   if (m_pCallBack)
   {
      if (m_pCallBack(this, ONOK) != 0) CDialog::OnOK();
      else                              CDialog::OnCancel();
   }
   else CDialog::OnOK();
}

void CTreeDlg::OnRclickTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   if (m_pCallBack)
   {
	   m_TvItem.hItem = m_TreeCtrl.GetSelectedItem();
      m_TreeCtrl.GetItem(&m_TvItem);
      m_pNMHdr = pNMHDR;
      if (m_pCallBack) *pResult = m_pCallBack(this, ONRCLICKLSTLIST);
      else             *pResult = 0;
      m_pNMHdr = NULL;
   }	
}

void CTreeDlg::OnBeginlabeleditTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
//	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
   if (m_pCallBack) *pResult = *pResult = m_pCallBack(this, ONBEGINLABELEDITLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;
}

void CTreeDlg::OnEndlabeleditTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
//	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
   if (m_pCallBack) *pResult = m_pCallBack(this, ONENDLABELEDITLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;
}

void CTreeDlg::OnBeginrdragTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnBegindragTreeTreectrl(pNMHDR, pResult);
}

void CTreeDlg::OnBegindragTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   m_pNMHdr = pNMHDR;
   if (m_pCallBack) *pResult = m_pCallBack(this, ONBEGINDRAGLSTLIST);
   else             *pResult = 0;
   m_pNMHdr = NULL;

   if (*pResult)                                               // jetzt Draggen ?
   {
   	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
      MapWindowPoints(&m_TreeCtrl, &pNMTreeView->ptDrag, 1);
      m_pDragImage = m_TreeCtrl.CreateDragImage(pNMTreeView->itemNew.hItem);
      if (m_pDragImage)
      {
         POINT pt  = {0, 0};
         m_TvItem = pNMTreeView->itemNew;
         m_pDragImage->BeginDrag(0, pt);
         m_pDragImage->DragEnter(&m_TreeCtrl, pNMTreeView->ptDrag);
      }
   }
}

void CTreeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
/*
   if (!(nFlags&MK_INLIST))
   {
      m_List.
   }
*/
   MapWindowPoints(&m_TreeCtrl, &point, 1);
   NM_TREEVIEW nmTV;
	if (m_pDragImage)
   {
      ZeroMemory(&nmTV, sizeof(NM_TREEVIEW));
      TVHITTESTINFO tvHti = {{point.x, point.y}, 0, NULL};
      nmTV.ptDrag        = point;
      nmTV.itemNew.hItem = m_TreeCtrl.HitTest(&tvHti);
      nmTV.itemNew.state = nFlags;
      nmTV.itemOld.state = tvHti.flags;
      m_pNMHdr = (NMHDR*)&nmTV;
      m_pDragImage->DragMove(point);
   }
   if (m_pCallBack) m_pCallBack(this, WM_MOUSEMOVE);
   m_pNMHdr = NULL;
	CDialog::OnMouseMove(nFlags, point);
}

void CTreeDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
   EndDrag(nFlags, point, WM_LBUTTONUP);
	CDialog::OnLButtonUp(nFlags, point);
}
 
void CTreeDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
   EndDrag(nFlags, point, WM_LBUTTONUP);
	CDialog::OnRButtonUp(nFlags, point);
}

void CTreeDlg::OnDeleteitemTreeTreectrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	*pResult = 0;
}

void CTreeDlg::DeleteDragImage()
{
	if (m_pDragImage)
   {
      delete m_pDragImage;
      m_pDragImage = NULL;
   }
}

void CTreeDlg::DragLeave()
{
	if (m_pDragImage)
   {
      m_pDragImage->DragLeave(this);
   }
}

void CTreeDlg::EndDrag(UINT nFlags, CPoint point, UINT nMsg)
{
   NM_TREEVIEW nmTV;
	if (m_pDragImage)
   {
      ZeroMemory(&nmTV, sizeof(NM_TREEVIEW));
      TVHITTESTINFO tvHti = {{point.x, point.y}, 0, NULL};
      nmTV.ptDrag        = point;
      nmTV.itemNew.hItem = m_TreeCtrl.HitTest(&tvHti);
      nmTV.itemNew.state = nFlags;
      nmTV.itemOld.state = tvHti.flags;
      m_pNMHdr = (NMHDR*)&nmTV;
      m_pDragImage->EndDrag();
   }
   if (m_pCallBack) m_pCallBack(this, nMsg);
   m_pNMHdr  = NULL;
   DeleteDragImage();
}

LRESULT CALLBACK CTreeDlg::ListSubClassProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   switch (Msg)
   {
      case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_RBUTTONUP:
      {
         HWND hWndParent = ::GetParent(hWnd);
         POINT ptMouse;
         POINTSTOPOINT(ptMouse, lParam);
         ::MapWindowPoints(hWnd, hWndParent, &ptMouse, 1);
         lParam = MAKELONG(ptMouse.x, ptMouse.y);
         wParam |= MK_INLIST;
         ::SendMessage(hWndParent, Msg, wParam, lParam);
      } return 0;
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

