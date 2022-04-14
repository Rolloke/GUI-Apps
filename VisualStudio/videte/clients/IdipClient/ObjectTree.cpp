/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ObjectTree.cpp $
// ARCHIVE:		$Archive: /Project/Clients/IdipClient/ObjectTree.cpp $
// CHECKIN:		$Date: 5.01.06 9:55 $
// VERSION:		$Revision: 46 $
// LAST CHANGE:	$Modtime: 4.01.06 16:07 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "IdipClient.h"

#include "MainFrm.h"

#include "IdipClientDoc.h"
#include "ObjectTree.h"
#include ".\objecttree.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CViewObjectTree, CTreeView)
/////////////////////////////////////////////////////////////////////////////
// CViewObjectTree
///////////////////////////////////////////////////////////////////////
CViewObjectTree::CViewObjectTree()
{
	m_hItemFirstSel = NULL;
	m_nHeightInPromille = 0;
	m_eVisibility = OTVS_InitiallyYes;
	m_bMinimized = FALSE;
}

void CViewObjectTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
	// Set focus to control if key strokes are needed.
	// Focus is not automatically given to control on lbuttondown

	if(nFlags & MK_CONTROL ) 
	{
		// Control key is down
		UINT flag;
		HTREEITEM hItem = theCtrl.HitTest( point, &flag );
		if( hItem )
		{
			// Toggle selection state
			UINT uNewSelState = 
				theCtrl.GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED ? 
							0 : TVIS_SELECTED;
            
			// Get old selected (focus) item and state
			HTREEITEM hItemOld = theCtrl.GetSelectedItem();
			UINT uOldSelState  = hItemOld ? 
					theCtrl.GetItemState(hItemOld, TVIS_SELECTED) : 0;
            
			// Select new item
			if( theCtrl.GetSelectedItem() == hItem )
				theCtrl.SelectItem( NULL );		// to prevent edit
			CTreeView::OnLButtonDown(nFlags, point);

			// Set proper selection (highlight) state for new item
			theCtrl.SetItemState(hItem, uNewSelState,  TVIS_SELECTED);

			// Restore state of old selected item
			if (hItemOld && hItemOld != hItem)
				theCtrl.SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);

			m_hItemFirstSel = NULL;

			return;
		}
	} 
	else if(nFlags & MK_SHIFT)
	{
		// Shift key is down
		UINT flag;
		HTREEITEM hItem = theCtrl.HitTest( point, &flag );

		// Initialize the reference item if this is the first shift selection
		if( !m_hItemFirstSel )
			m_hItemFirstSel = theCtrl.GetSelectedItem();

		// Select new item
		if( theCtrl.GetSelectedItem() == hItem )
			theCtrl.SelectItem( NULL );			// to prevent edit
		CTreeView::OnLButtonDown(nFlags, point);

		if( m_hItemFirstSel )
		{
			SelectItems( m_hItemFirstSel, hItem );
			return;
		}
	}
	else
	{
		// Normal - remove all selection and let default 
		// handler do the rest
		ClearSelection();
		m_hItemFirstSel = NULL;
	}

   CTreeView::OnLButtonDown(nFlags, point);
}
///////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	CTreeCtrl &theCtrl = GetTreeCtrl();
	HTREEITEM hItem = theCtrl.GetRootItem();

	// Clear selection upto the first item
	while ( hItem && hItem!=hItemFrom && hItem!=hItemTo )
	{
		hItem = theCtrl.GetNextVisibleItem( hItem );
		theCtrl.SetItemState( hItem, 0, TVIS_SELECTED );
	}

	if ( !hItem )
		return FALSE;	// Item is not visible

	theCtrl.SelectItem( hItemTo );

	// Rearrange hItemFrom and hItemTo so that hItemFirst is at top
	if( hItem == hItemTo )
	{
		hItemTo = hItemFrom;
		hItemFrom = hItem;
	}


	// Go through remaining visible items
	BOOL bSelect = TRUE;
	while ( hItem )
	{
		// Select or remove selection depending on whether item
		// is still within the range.
		theCtrl.SetItemState( hItem, bSelect ? TVIS_SELECTED : 0, TVIS_SELECTED );

		// Do we need to start removing items from selection
		if( hItem == hItemTo ) 
			bSelect = FALSE;

		hItem = theCtrl.GetNextVisibleItem( hItem );
	}

	return TRUE;
}
///////////////////////////////////////////////////////////////////////
HTREEITEM CViewObjectTree::GetFirstSelectedItem()
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
	for (HTREEITEM hItem = theCtrl.GetRootItem(); hItem!=NULL; hItem = theCtrl.GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
   {
		if ( theCtrl.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
      {
			return hItem;
      }
   }

	return NULL;
}
///////////////////////////////////////////////////////////////////////
HTREEITEM CViewObjectTree::GetNextSelectedItem(HTREEITEM hItem)
{
   CTreeCtrl &theCtrl = GetTreeCtrl();
	for (hItem = theCtrl.GetNextItem( hItem ,TVGN_NEXTVISIBLE); hItem!=NULL; hItem = theCtrl.GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
   {
		if ( theCtrl.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
      {
			return hItem;
      }
   }

	return NULL;
}
///////////////////////////////////////////////////////////////////////
HTREEITEM CViewObjectTree::GetPrevSelectedItem(HTREEITEM hItem)
{
   CTreeCtrl &theCtrl = GetTreeCtrl();

	for (hItem = theCtrl.GetNextItem( hItem,TVGN_PREVIOUSVISIBLE); hItem!=NULL; hItem = theCtrl.GetNextItem( hItem,TVGN_PREVIOUSVISIBLE) )
   {
		if ( theCtrl.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
      {
			return hItem;
      }
   }

	return NULL;
}
///////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   CTreeCtrl &theCtrl = GetTreeCtrl();

	if ( (nChar==VK_UP || nChar==VK_DOWN) && GetKeyState( VK_SHIFT )&0x8000)
	{
		// Initialize the reference item if this is the first shift selection
		if( !m_hItemFirstSel )
		{
			m_hItemFirstSel = theCtrl.GetSelectedItem();
			ClearSelection();
		}

		// Find which item is currently selected
		HTREEITEM hItemPrevSel = theCtrl.GetSelectedItem();

		HTREEITEM hItemNext;
		if ( nChar==VK_UP )
			hItemNext = theCtrl.GetPrevVisibleItem( hItemPrevSel );
		else
			hItemNext = theCtrl.GetNextVisibleItem( hItemPrevSel );

		if ( hItemNext )
		{
			// Determine if we need to reselect previously selected item
			BOOL bReselect = 
				!( theCtrl.GetItemState( hItemNext, TVIS_SELECTED ) & TVIS_SELECTED );

			// Select the next item - this will also deselect the previous item
			theCtrl.SelectItem( hItemNext );

			// Reselect the previously selected item
			if ( bReselect )
				theCtrl.SetItemState( hItemPrevSel, TVIS_SELECTED, TVIS_SELECTED );
		}
		return;
	}
	else if( nChar >= VK_SPACE )
	{
		m_hItemFirstSel = NULL;
		ClearSelection();
	}
	CTreeView::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::ClearSelection()
{
	// This can be time consuming for very large trees 
	// and is called every time the user does a normal selection
	// If performance is an issue, it may be better to maintain 
	// a list of selected items
	CTreeCtrl &theCtrl = GetTreeCtrl();
	for ( HTREEITEM hItem=theCtrl.GetRootItem(); hItem!=NULL; hItem=theCtrl.GetNextItem( hItem ,TVGN_NEXTVISIBLE) )
	{
		if ( theCtrl.GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )
		{
			theCtrl.SetItemState( hItem, 0, TVIS_SELECTED );
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::GetCheck(HTREEITEM hItem)
{
	return ::GetCheck(GetTreeCtrl(), hItem);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::SetCheck(HTREEITEM hItem,BOOL bFlag,BOOL bDisable/*=FALSE*/)
{
	return ::SetCheck(GetTreeCtrl(), hItem, bFlag, bDisable);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::SetCheckRecurse(HTREEITEM hParent, int iState, BOOL bRecurse/*=TRUE*/)
{
	::SetCheckRecurse(GetTreeCtrl(), hParent, iState, bRecurse);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CViewArchive diagnostics
#ifdef _DEBUG
void CViewObjectTree::AssertValid() const
{
	CTreeView::AssertValid();
}

void CViewObjectTree::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
CIdipClientDoc* CViewObjectTree::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIdipClientDoc)));
	return (CIdipClientDoc*)m_pDocument;
}
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////////
CPoint CViewObjectTree::GetWindowOffset(CWnd *pWnd)
{
	ASSERT(pWnd != NULL);
	CRect rc1, rc2;
	pWnd->GetWindowRect(&rc1);
	GetWindowRect(&rc2);
	return rc2.TopLeft() - rc1.TopLeft();
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewObjectTree, CTreeView)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_MOVE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
int CViewObjectTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!theApp.IsReadOnlyModus())
	{
		CRuntimeClass*pRTC = GetRuntimeClass();
		if (pRTC)
		{
			CWK_Profile wkp;
			CString sSection(&pRTC->m_lpszClassName[1]);
			DWORD dwVisibilty = wkp.GetInt(IDIP_CLIENT_VISIBILITY, sSection, m_eVisibility);
			m_eVisibility = (eVisibilityStates)LOWORD(dwVisibilty);
			m_bMinimized  = dwVisibilty & FLAG_MINIMIZED;
		}
	}
	
	m_nCaptionHeight = GetSystemMetrics(SM_CYSMCAPTION);
	m_nCaptionHeight += 2;

	CRect rcBtn;
	GetWindowRect(&rcBtn);
	rcBtn.OffsetRect(-rcBtn.left, rcBtn.top);
	rcBtn.bottom = m_nCaptionHeight;

	long lStyle = WS_CHILD;
	switch (m_eVisibility)
	{
		case OTVS_Always:
		case OTVS_InitiallyYes:
			lStyle |= WS_VISIBLE;
			break;
	}
	CWnd *pWnd = GetParent();
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
	{
		m_btnTitle.SetSurface(SurfaceColorChangeBrushed);
		m_btnTitle.Create(_T("Title"), lStyle, rcBtn, pWnd, IDC_TITLE_BTN);
		m_btnTitle.SetTextAllignment(TA_LEFT);

		m_btnMenu.SetSurface(SurfaceColorChangeBrushed);
		m_btnMenu.Create(_T("Menu"), lStyle, rcBtn, pWnd, IDC_MENU_BTN);
		m_btnMenu.SetImageList(CSkinDialog::GetDownButtonImages());

		m_btnClose.SetSurface(SurfaceColorChangeBrushed);
		m_btnClose.Create(_T("Close"), lStyle, rcBtn, pWnd, IDC_CLOSE_VIEW);
		m_btnClose.SetImageList(CSkinDialog::GetCloseButtonImages());

		m_btnMiniMize.SetSurface(SurfaceColorChangeBrushed);
		m_btnMiniMize.Create(_T("Minimize"), lStyle, rcBtn, pWnd, IDC_MINIMIZE_SPLITTER);
		m_btnMiniMize.SetImageList(theApp.m_bObjectViewsLeft ? CSkinDialog::GetLeftButtonImages() : CSkinDialog::GetRightButtonImages());

		m_btnMenu.SetWindowPos(&m_btnTitle, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		m_btnClose.SetWindowPos(&m_btnMenu, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		m_btnMiniMize.SetWindowPos(&m_btnClose, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		SetWindowPos(&m_btnMiniMize, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

		m_btnTitle.SetOwner(this);
		m_btnMenu.SetOwner(this);
		m_btnClose.SetOwner(this);
		m_btnMiniMize.SetOwner(this);
	}

	GetTreeCtrl().SetLineColor(CSkinTree::GetOEMLineColor());
	GetTreeCtrl().SetBkColor(CSkinTree::GetOEMBkColor());
	GetTreeCtrl().SetTextColor(CSkinTree::GetOEMTextColor());
	if (CSkinDialog::UseGlobalFonts())
	{
		GetTreeCtrl().SetFont(CSkinDialog::GetDialogItemGlobalFont());
		m_btnTitle.SetFont(CSkinDialog::GetVariableGlobalFont());
	}
	else
	{
		CFont *pFont;
		pFont = GetFont();
		if (pFont)
		{
			CFont BoldFont;
			LOGFONT lf;
			pFont->GetLogFont(&lf);
			lf.lfWeight = FW_BOLD;
			if (lf.lfHeight > 0) lf.lfHeight += 1;
			else				 lf.lfHeight -= 1;
			BoldFont.CreateFontIndirect(&lf);
			m_btnTitle.SetFont(&BoldFont);
			BoldFont.Detach();
		}
	}

	CDC *pDC = m_btnTitle.GetDC();
	if (pDC)
	{
		pDC->SaveDC();
		pDC->SelectObject(m_btnTitle.GetFont());
		CSize szText = pDC->GetTextExtent(_T("yÄ"), 2);
		m_nCaptionHeight = max(szText.cy + 4, m_nCaptionHeight);
		pDC->RestoreDC(-1);
		m_btnTitle.ReleaseDC(pDC);
	}

	EnableCloseButton(TRUE);

	ShowWindow((lStyle & WS_VISIBLE) ? SW_SHOW : SW_HIDE);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
// Global Tree functions
BOOL GetCheck(CTreeCtrl& ctrl, HTREEITEM hItem)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	ctrl.GetItem(&tvItem);

	int i = tvItem.state>>12;

	if (i==2)
	{
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL SetCheck(CTreeCtrl& ctrl, HTREEITEM hItem,BOOL bFlag,BOOL bDisable /* = FALSE */)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	/*
	Since state images are one-based, 1 in this macro turns the check off, and 
	2 turns it on. 
	If bDisable == TRUE, the check is turned on but not editable
	*/
	if(bDisable)
	{
		tvItem.state = INDEXTOSTATEIMAGEMASK(3);
	}
	else
	{
		tvItem.state = INDEXTOSTATEIMAGEMASK((bFlag ? 1 : 2));
	}


	return ctrl.SetItem(&tvItem);
}
/////////////////////////////////////////////////////////////////////////////
void SetCheckRecurse(CTreeCtrl& ctrl, HTREEITEM hParent, int iState, BOOL bRecurse/*=TRUE*/)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	tvItem.state = INDEXTOSTATEIMAGEMASK(iState);

	tvItem.hItem = hParent;
	ctrl.SetItem(&tvItem);

	if (bRecurse)
	{
		if (bRecurse < 0)
		{
			bRecurse++;
		}
		HTREEITEM hChild = ctrl.GetChildItem(hParent);
		while (hChild)
		{
			tvItem.hItem = hChild;
			ctrl.SetItem(&tvItem);
			::SetCheckRecurse(ctrl, hChild,iState,bRecurse);
			hChild = ctrl.GetNextSiblingItem(hChild);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
		case SC_CLOSE:
//		if (lParam == 0)
		{
			// KE: Close the whole splitter view
			theApp.GetMainFrame()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)GetParent());
			return;
		}break;
						 // OnNCClick
		case 0xf012: // Move Caption
		case 0xf001: // Size left
		case 0xf002: // Size right
		case 0xf003: // Size top
		case 0xf006: // Size bottom
			return;
		case SC_MINIMIZE:
		case SC_MOVE: case SC_SIZE:
			return;
		case SC_RESTORE:
			nID = SC_MAXIMIZE;
			break;
	}
	CTreeView::OnSysCommand(nID, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnSize(UINT nType, int cx, int cy)
{
//	m_bOnMinimizeBtn = FALSE;
	CTreeView::OnSize(nType, cx, cy);
	MoveTitleButtons();
}
void CViewObjectTree::OnMove(int x, int y)
{
	CTreeView::OnMove(x, y);
	MoveTitleButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::MoveTitleButtons()
{
	CRect rc;
	GetWindowRect(&rc);
	rc.bottom = rc.top + m_nCaptionHeight;
	GetParent()->ScreenToClient(&rc);

	CRect rcBtn(rc);
	rcBtn.left = rcBtn.right-rcBtn.Height();
	m_btnMiniMize.MoveWindow(rcBtn);
	rcBtn.OffsetRect(-rcBtn.Width(), 0);
	m_btnClose.MoveWindow(rcBtn);
	rcBtn.OffsetRect(-rcBtn.Width(), 0);
	m_btnMenu.MoveWindow(rcBtn);

	rc.right = rcBtn.left;
	m_btnTitle.MoveWindow(rc);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	if (bCalcValidRects)
	{
		CRect*pRc = (CRect*)&lpncsp->rgrc[0];
		if (!IsActive())
		{
			pRc->top = pRc->bottom;
		}
		else
		{
			pRc->top += m_nCaptionHeight;
		}
	}
	CTreeView::OnNcCalcSize(bCalcValidRects, lpncsp);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::IsActive()
{
	CWnd *pWndParent = GetParent();
	if (pWndParent)
	{
		ASSERT_KINDOF(CSplitterObjectView, pWndParent);
		CSplitterObjectView *pSOV = (CSplitterObjectView*)pWndParent;
		if (pSOV->m_nDivisionMode == OVDM_SHOW_ONE_VIEW)
		{
            return pSOV->m_nActiveID == GetDlgCtrlID() ? TRUE : FALSE;
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | 
			   TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::ModifyContextMenu(CMenu*pMenu)
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CViewObjectTree::OnShowObjectPane()
{
	switch(m_eVisibility)
	{
		case OTVS_Always:
			return FALSE;
		case OTVS_Never:
			if (theApp.IsReadOnlyModus())
			{
				return FALSE;
			}
			else
			{
				m_eVisibility = OTVS_InitiallyNo;
			}break;
	}
	int nCmdShow = IsWindowVisible() ? SW_HIDE : SW_SHOW;
	ShowWindow(nCmdShow);
	m_btnTitle.ShowWindow(nCmdShow);
	m_btnMenu.ShowWindow(nCmdShow);
	m_btnClose.ShowWindow(nCmdShow);
	m_btnMiniMize.ShowWindow(nCmdShow);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnUpdateCmdShowObjectPane(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!(m_eVisibility == OTVS_Always));
	pCmdUI->SetCheck(IsWindowVisible());
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnDestroy()
{
	GetParent()->SendMessage(WM_USER, WM_DESTROY, (LPARAM)this);
	if (!CSkinDialog::UseGlobalFonts())
	{
		CFont *pFont = m_btnTitle.GetFont();
		if (pFont)
		{
			pFont->DeleteObject();
		}
	}
	CTreeView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::CheckVisibility()
{
	if (m_eVisibility == OTVS_IfNotEmpty)
	{
		BOOL bEmpty = TRUE;
		HTREEITEM hItem = GetTreeCtrl().GetRootItem();
		if (hItem)
		{
			bEmpty = FALSE;
		}
		if (!(IsWindowVisible() ^ bEmpty))
		{
			ToggleVisibility();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::SetVisibility(eVisibilityStates eVisibility)
{
	BOOL bIsVisible  = IsWindowVisible();
	BOOL bSetVisible = bIsVisible;
	switch (eVisibility)
	{
		case OTVS_IfNotEmpty:
			m_eVisibility = eVisibility;
			CheckVisibility();
			break;
// to set the following states we have to fake "m_eVisibility" state
// member variable to use the available functions
		case OTVS_Always:		
			bSetVisible = TRUE; 
			m_eVisibility = OTVS_InitiallyNo;
			break;
		case OTVS_Never:
			bSetVisible = FALSE;
			m_eVisibility = OTVS_InitiallyYes;
			break;
	}

	EnableCloseButton(TRUE);

	if (bIsVisible != bSetVisible)
	{
		ToggleVisibility();
	}
	m_eVisibility = eVisibility;

	CRuntimeClass*pRTC = GetRuntimeClass();
	if (pRTC)
	{
		CWK_Profile wkp;
		CString sSection(&pRTC->m_lpszClassName[1]);
		DWORD dwVisibility = m_eVisibility;
		if (m_bMinimized) dwVisibility |= FLAG_MINIMIZED;
		wkp.WriteInt(IDIP_CLIENT_VISIBILITY, sSection, dwVisibility);
	}
}
/////////////////////////////////////////////////////////////////////////////
WORD CViewObjectTree::GetSelectedServer(WORD *pwHostId, HTREEITEM hSelected)
{
	HTREEITEM hItem = NULL;
	HTREEITEM hParent;
	if (hSelected)
	{
		hItem = hSelected;
	}
	else
	{
		hItem = GetTreeCtrl().GetSelectedItem();
	}
	while (hItem)
	{
		hParent = GetTreeCtrl().GetParentItem(hItem);
		if (hParent == NULL) break;
		hItem = hParent;
	}
	if (hItem==NULL)
	{
		hItem = GetTreeCtrl().GetRootItem();
	}
	return GetServerIDs(hItem, pwHostId);
}
//////////////////////////////////////////////////////////////////////////
WORD CViewObjectTree::GetServerIDs(HTREEITEM hServer, WORD *pwHost)
{
	DWORD dwID = SECID_NO_ID;
	if (hServer) dwID =  GetTreeCtrl().GetItemData(hServer);
	if (pwHost) *pwHost = HIWORD(dwID);
	return  LOWORD(dwID);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if (bActivate)
	{
		SetSelected(TRUE);
	}

	CTreeView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::EnableCloseButton(BOOL bEnable)
{
	m_btnClose.EnableWindow(m_eVisibility != OTVS_Always && bEnable ? TRUE : FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::SetSelected(BOOL bSelect)
{
	COLORREF color = CSkinButton::GetBaseColorG();
	if (bSelect)
	{
		CWnd *pParent = GetParent();
		if (pParent)
		{
			pParent->SendMessage(WM_USER, MAKELONG(WM_ACTIVATE, 1), (LPARAM)this);
			CMainFrame *pMF = theApp.GetMainFrame();
			if (pMF->GetActiveView() != this)
			{
				pMF->SetActiveView(this, TRUE);
			}
		}
		color = CSkinButton::ChangeBrightness(color, 40, TRUE);
	}
	m_btnTitle.SetBaseColor(color);
	m_btnMenu.SetBaseColor(color);
	m_btnClose.SetBaseColor(color);
	m_btnMiniMize.SetBaseColor(color);
}
/////////////////////////////////////////////////////////////////////////////
void CViewObjectTree::ToggleVisibility()
{
	CWnd *pWnd = GetParent();
	if (pWnd)
	{
		pWnd->SendMessage(WM_COMMAND, GetDlgCtrlID(), (LPARAM)m_btnTitle.m_hWnd);
	}
}
