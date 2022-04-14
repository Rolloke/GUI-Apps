/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ObjectViewTreeStuff.cpp $
// ARCHIVE:		$Archive: /Project/Clients/Recherche/ObjectViewTreeStuff.cpp $
// CHECKIN:		$Date: 19.07.02 10:45 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 19.07.02 10:17 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$

#include "stdafx.h"
#include "Recherche.h"

#include "RechercheDoc.h"
#include "ObjectView.h"

#include "images.h"
#include "BackupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectView
///////////////////////////////////////////////////////////////////////
void CObjectView::OnLButtonDown(UINT nFlags, CPoint point) 
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
BOOL CObjectView::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
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
HTREEITEM CObjectView::GetFirstSelectedItem()
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
HTREEITEM CObjectView::GetNextSelectedItem(HTREEITEM hItem)
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
HTREEITEM CObjectView::GetPrevSelectedItem(HTREEITEM hItem)
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
void CObjectView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
void CObjectView::ClearSelection()
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
		HTREEITEM hChild = ctrl.GetChildItem(hParent);
		while (hChild)
		{
			tvItem.hItem = hChild;
			ctrl.SetItem(&tvItem);
			SetCheckRecurse(ctrl,hChild,iState,bRecurse);
			hChild = ctrl.GetNextSiblingItem(hChild);
		}
	}

}
