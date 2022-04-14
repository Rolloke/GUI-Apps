// StatusList.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"

#include "StatusList.h"
#include "SVPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CStatusList::CStatusList()
{
	m_nStates = 1;
	m_nWidth  = 16;
	m_RadioIndex = -1;
}
/////////////////////////////////////////////////////////////////////////////
CStatusList::~CStatusList()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CStatusList, CListCtrl)
	//{{AFX_MSG_MAP(CStatusList)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CStatusList::DeleteAllItems()
{
	m_RadioIndex = -1;
	return CListCtrl::DeleteAllItems();
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::SetStateInfo(CImageList* pImageList, int nStates, int nWidth)
{
	SetImageList(pImageList,LVSIL_STATE);
	m_nStates = nStates;
	m_nWidth  = nWidth;
}
/////////////////////////////////////////////////////////////////////////////
UINT CStatusList::GetItemStatus(int nIndex)
{
	UINT uState;
	uState = GetItemState(nIndex,LVIS_STATEIMAGEMASK);
	uState = (uState >> 12) - 1;
	return uState;
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::SetItemStatus(int nIndex,UINT uStatus)
{
	UINT uState;
	uState = uStatus + 1;
	SetItemState(nIndex,INDEXTOSTATEIMAGEMASK(uState),LVIS_STATEIMAGEMASK);
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::SetRadioItem(int nIndex)
{
	if (nIndex==-1)
	{
		SetItemState(m_RadioIndex,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
		EnsureVisible(0, FALSE);
		m_RadioIndex = -1;
	}
	else
	{
		if (m_RadioIndex!=-1)
		{
			SetItemState(m_RadioIndex,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
		}
		SetItemState(nIndex,INDEXTOSTATEIMAGEMASK(2)|LVIS_FOCUSED,LVIS_STATEIMAGEMASK|LVIS_FOCUSED);
		SetItemState(nIndex,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
		EnsureVisible(nIndex, FALSE);
		m_RadioIndex = nIndex;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::SetRadioItemFromlParam(LPARAM lParam)
{
	int i,c;

	// try to set new radio
	c = GetItemCount();
	for (i=0;i<c;i++)
	{
		if (GetItemData(i)==(DWORD)lParam)
		{
			SetRadioItem(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
LPARAM CStatusList::GetRadioItemData()
{
	int index;

	index = GetRadioItem();
	if (index==-1)
		return (LPARAM)(-1);
	else
		return GetItemData(index);
}
/////////////////////////////////////////////////////////////////////////////
int CStatusList::GetRadioItem()
{
	return m_RadioIndex;
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::OnSpace() 
{
	int i,c;
	int nHitItem;
	UINT uItemState;
	UINT uState;

	c = GetItemCount();
	nHitItem = -1;

	for (i=0;i<c;i++)
	{
		if (GetItemState(i,LVIS_FOCUSED) & LVIS_FOCUSED)
		{
			nHitItem = i;
			break;
		}
	}

	if (nHitItem==-1)
		nHitItem = 0;

	if (m_nStates!=0)
	{
		// check mode
		uState = GetItemState(nHitItem,LVIS_STATEIMAGEMASK);
		uState = (uState >> 12) - 1;
		// modify state
		uState++;
		if (uState==((UINT)(1<<m_nStates)))
			uState = 0;

		for (i=0;i<c;i++)
		{
			uItemState = GetItemState(i,LVIS_STATEIMAGEMASK|LVIS_SELECTED);
			if (uItemState & LVIS_SELECTED)
			{
				uItemState = (uItemState & 0x0FFF) | ((uState + 1) << 12);
				SetItemState(i,uItemState,LVIS_STATEIMAGEMASK|LVIS_SELECTED);
			}
		}
	}
	else
	{
		SetRadioItem(nHitItem);
	}
	
	((CPropertyPage*)GetParent())->SetModified();
}
/////////////////////////////////////////////////////////////////////////////
// CStatusList message handlers
void CStatusList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT uFlags = 0;
	UINT uState;
	UINT uItemState;
	UINT uNum;
	int i,c;
	int nHitItem = HitTest(point,&uFlags);

	if (uFlags & LVHT_ONITEMSTATEICON)
	{
		uNum  = point.x / m_nWidth;
		uNum  = 1 << uNum;
		uState = GetItemState(nHitItem,LVIS_STATEIMAGEMASK);
		uState = (uState >> 12) - 1;
		// modify state
		uState = uState ^ uNum;

		if (m_nStates!=0)
		{
			// check mode
			if (GetItemState(nHitItem,LVIS_SELECTED) & LVIS_SELECTED)
			{
				c = GetItemCount();
				for (i=0;i<c;i++)
				{
					uItemState = GetItemState(i,LVIS_STATEIMAGEMASK|LVIS_SELECTED);
					if (uItemState & LVIS_SELECTED)
					{
						uItemState = (uItemState & 0x0FFF) | ((uState + 1) << 12);
						SetItemState(i,uItemState,LVIS_STATEIMAGEMASK|LVIS_SELECTED);
					}
				}
			}
			else
			{
				uState = uState + 1;
				SetItemState(nHitItem,INDEXTOSTATEIMAGEMASK(uState),LVIS_STATEIMAGEMASK);
			}
		}
		else
		{
			SetRadioItem(nHitItem);
		}
		
		((CPropertyPage*)GetParent())->SetModified();
	}
//	else
		CListCtrl::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::OnSetFocus(CWnd* pOldWnd) 
{
	CListCtrl::OnSetFocus(pOldWnd);

	if ( m_nStates == 0 ) {
		SetItemState(m_RadioIndex,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((nChar==VK_SPACE) && (m_nStates!=0))
		OnSpace();
	
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CStatusList::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if ((pNMListView->iItem!=m_RadioIndex) && (m_nStates==0))
	{
		SetRadioItem(pNMListView->iItem);
		((CSVPage*)GetParent())->OnItemChangedStatusList(GetDlgCtrlID(),pNMListView->iItem);
	}
	else if (m_nStates>0)
	{
		((CSVPage*)GetParent())->OnItemChangedStatusList(GetDlgCtrlID(),pNMListView->iItem);
	}

	*pResult = 0;
}
