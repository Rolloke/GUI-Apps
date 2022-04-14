// EdtSubItemListCtrl.cpp : implementation file
//

#include "stdafx.h"
//#include "systemverwaltung.h"
#include "TestEditListSubitem.h"
#include "EdtSubItemListCtrl.h"

#include <winuser.h>

/////////////////////////////////////////////////////////////////////////////
// CSubItemEdit
CSubItemEdit::CSubItemEdit (CEdtSubItemListCtrl* pCtrl)
{
	m_pListCtrl = pCtrl;
	m_isClosing = false;
	m_pListCtrl->m_bIsEditing = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSubItemEdit::~CSubItemEdit()
{
	m_pListCtrl->m_bIsEditing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSubItemEdit, CEdit)
    //{{AFX_MSG_MAP(CSubItemEdit)
    ON_WM_KILLFOCUS()
    ON_WM_CHAR()
    ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_STOP_EDITING, (ON_MESSAGE_FNC)StopEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubItemEdit message handlers
BOOL CSubItemEdit::PreTranslateMessage (MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_hWnd)
    {
		switch (pMsg->wParam)
		{
			case VK_RETURN:
			case VK_DELETE:
			case VK_ESCAPE:
				::TranslateMessage (pMsg);
				::DispatchMessage (pMsg);
				return TRUE;		    	// DO NOT process further
			// case VK_TAB:
			case VK_LEFT:
				{
					int nStart, nEnd;
					GetSel(nStart, nEnd);
					if (   nStart == nEnd
						&& nStart == 0
						&& m_pListCtrl->OnArrow((UINT)pMsg->wParam, TRUE))
					{
						return TRUE;
					}
				}break;
			case VK_RIGHT:
				{
					int nLen = GetWindowTextLength();
					int nStart, nEnd;
					GetSel(nStart, nEnd);
					if (   nStart == nEnd
						&& nStart == nLen
						&& m_pListCtrl->OnArrow((UINT)pMsg->wParam, TRUE))
					{
						return TRUE;
					}
				}break;
			case VK_UP:
			case VK_DOWN:
				if (m_pListCtrl->OnArrow((UINT)pMsg->wParam, TRUE))
				{
					return TRUE;
				}break;
		}
    }

    return CEdit::PreTranslateMessage (pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSubItemEdit::OnKillFocus (CWnd* pNewWnd) 
{
    CEdit::OnKillFocus(pNewWnd);

	//End edit?
	if(!m_isClosing)
	{
		if(m_pListCtrl->m_bEditEndOnLostFocus)
		{
			StopEdit(FALSE);
		}
		else
		{
			//Notify list parent
			m_pListCtrl->GetParent()->SendMessage(WM_EDITINGLOSTFOCUS, (WPARAM)m_pListCtrl, (LPARAM)pNewWnd);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSubItemEdit::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags) 
{
//    BOOL Shift = GetKeyState (VK_SHIFT) < 0;
    switch (nChar)
    {
		case VK_ESCAPE :
		{	
			StopEdit(TRUE, VK_ESCAPE);
			return;
		}
		case VK_RETURN :
		{
			StopEdit(FALSE, VK_RETURN);
			return;
		}
    }

    CEdit::OnChar (nChar, nRepCnt, nFlags);
 
}
/////////////////////////////////////////////////////////////////////////////
int CSubItemEdit::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
    if (CEdit::OnCreate (lpCreateStruct) == -1)
		return -1;

    // Set the proper font
    SetFont (m_pListCtrl->GetFont());

    SetFocus();
    SetSel (0, -1);
    return 0;
}
/*********************************************************************************************
 Class      : CSubItemEdit
 Function   : StopEdit
 Description: Stops editing and validates the value of the subitem. If the value is not valid
              the edit mode is not stopped.
			  May be sent with WM_STOP_EDITING (WPARAM bCancel, LPARAM endkey)

 Parameters:   
  bCancel: (E): cancel edit mode (no validation)  (BOOL)
  endkey : (E): virtual key code  (UINT)

 Result type: edit mode stopped (TRUE, FALSE) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE StopEdit>
*********************************************************************************************/
BOOL CSubItemEdit::StopEdit(BOOL bCancel, UINT endkey)
{
	m_isClosing = true;

	if (   !bCancel 
		&& !m_pListCtrl->UpdateData())
	{
		m_isClosing = false;
		return FALSE;
	}
	HWND hParent = ::GetParent(m_hWnd);
	DestroyWindow();
	switch (endkey)
	{
		case VK_RETURN: case VK_ESCAPE:
			::InvalidateRect(hParent, NULL, TRUE);
			break;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSubItemEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}
/////////////////////////////////////////////////////////////////////////////
// CSubItemCombo
CSubItemCombo::CSubItemCombo (CEdtSubItemListCtrl* pCtrl)
{
	m_pListCtrl = pCtrl;
	m_isClosing = false;
	m_pListCtrl->m_bIsEditing = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CSubItemCombo::~CSubItemCombo()
{
	m_pListCtrl->m_bIsEditing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSubItemCombo, CComboBox)
    //{{AFX_MSG_MAP(CSubItemCombo)
    ON_WM_KILLFOCUS()
    ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_STOP_EDITING, (ON_MESSAGE_FNC)StopEdit)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubItemCombo message handlers
BOOL CSubItemCombo::PreTranslateMessage (MSG* pMsg) 
{
    if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_hWnd)
    {
		switch (pMsg->wParam)
		{
			case VK_RETURN:
				if (GetCurSel() != CB_ERR)
				{
					return StopEdit(FALSE, (UINT)pMsg->wParam);
				}
				else
				{
					return StopEdit(TRUE, VK_ESCAPE);
				}
				break;
			case VK_ESCAPE:
				return StopEdit(TRUE, (UINT)pMsg->wParam);
			case VK_LEFT:
				if (m_pListCtrl->OnArrow((UINT)pMsg->wParam, TRUE))
				{
					return TRUE;
				} break;
			case VK_RIGHT:
				if (m_pListCtrl->OnArrow((UINT)pMsg->wParam, TRUE))
				{
					return TRUE;
				} break;
			case VK_UP:
//				pMsg->wParam = VK_PRIOR;
				break;
			case VK_DOWN:
//				pMsg->wParam = VK_NEXT;
				break;
		}
    }

	return CComboBox::PreTranslateMessage (pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSubItemCombo::OnKillFocus (CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);

	//End edit?
	if(!m_isClosing)
	{
		if(m_pListCtrl->m_bEditEndOnLostFocus)
		{
			StopEdit(FALSE);
		}
		else
		{
			//Notify list parent
			m_pListCtrl->GetParent()->SendMessage(WM_EDITINGLOSTFOCUS, (WPARAM)m_pListCtrl, (LPARAM)pNewWnd);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSubItemCombo::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return CComboBox::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
int CSubItemCombo::OnCreate (LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate (lpCreateStruct) == -1)
		return -1;

    // Set the proper font
    SetFont(m_pListCtrl->GetFont());

    SetFocus();
    return 0;
}
/*********************************************************************************************
 Class      : CSubItemCombo
 Function   : StopEdit
 Description: Stops editing and validates the value of the subitem. If the value is not valid
              the edit mode is not stopped.
			  May be sent with WM_STOP_EDITING (WPARAM bCancel, LPARAM endkey)

 Parameters:   
  bCancel: (E): cancel edit mode (no validation)  (BOOL)
  endkey : (E): virtual key code  (UINT)

 Result type: edit mode stopped (TRUE, FALSE) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE StopEdit>
*********************************************************************************************/
BOOL CSubItemCombo::StopEdit(BOOL bCancel, UINT endkey)
{
	m_isClosing = true;

	if (   !bCancel 
		&& !m_pListCtrl->UpdateData())
	{
		m_isClosing = false;
		return FALSE;
	}
	HWND hParent = ::GetParent(m_hWnd);
	DestroyWindow();
	switch (endkey)
	{
		case VK_RETURN: case VK_ESCAPE:
			::InvalidateRect(hParent, NULL, TRUE);
			break;
	}
	return TRUE;
}
/*********************************************************************************************
 Class      : CSubItemCombo
 Function   : SetDroppedHeight
 Description: Sets the height of the dropped list of the ComboBox

 Parameters:   
  nHeight: (E): desired height  (int)

 Result type: old height of the list (int)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE SetDroppedHeight>
*********************************************************************************************/
int CSubItemCombo::SetDroppedHeight(int nHeight)
{
	COMBOBOXINFO cbi;
	CRect	rc;
	int nOldHeight = -1;
	cbi.cbSize = sizeof(COMBOBOXINFO);
	GetComboBoxInfo(&cbi);
	::GetWindowRect(cbi.hwndList, &rc);
	nOldHeight = rc.Height();

	::SetWindowPos(cbi.hwndList, NULL, 0, 0, rc.Width(), nHeight, SWP_NOMOVE|SWP_NOZORDER);

	return nOldHeight;
}
/////////////////////////////////////////////////////////////////////////////
void CSubItemCombo::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	delete this;
}

/////////////////////////////////////////////////////////////////////////////
// CEdtSubItemListCtrl
IMPLEMENT_DYNAMIC(CEdtSubItemListCtrl, CListCtrl)
CEdtSubItemListCtrl::CEdtSubItemListCtrl()
{
	m_bEditEndOnLostFocus = TRUE;
	m_bIsEditing = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CEdtSubItemListCtrl::~CEdtSubItemListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CEdtSubItemListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(CEdtSubItemListCtrl)
	ON_NOTIFY_REFLECT(NM_RETURN, OnNMReturn)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// virtual functions
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : DoDataExchange
 Description: Overwrite to validate the data of the edited text. Call the function of the
              base class after validation unlike CDialog:DoDataExchange().

 Parameters:   
  pDX: (EA): Dataexchange control object  (CDataExchange*)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE DoDataExchange>
*********************************************************************************************/
void CEdtSubItemListCtrl::DoDataExchange(CDataExchange* pDX)
{
	CString sText;
	DDX_Text(pDX, GetSelectionID(), sText);
	SetItemText(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem, sText);
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : CanEditSubItem
 Description: Overwrite to control whether the subitem may be edited

 Parameters:   
  nItem   : (E): table row  (int)
  nSubItem: (): table column  (int)

 Result type: Edit subitem (TRUE, FALSE) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE CanEditSubItem>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::CanEditSubItem(int nItem, int nSubItem)
{
	return (GetDlgItem(GetSubItemID(nItem, nSubItem)) == NULL) ? TRUE : FALSE;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : GetEditType
 Description: Overwrite to select an edit type

 Parameters:   
  nItem   : (E): table row    (int)
  nSubItem: (E): table column  (int)

 Result type: Type (EDIT_FIELD, COMBO_BOX) (eEditType)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE GetEditType>
*********************************************************************************************/
eEditType CEdtSubItemListCtrl::GetEditType(int nItem, int nSubItem)
{
	return EDIT_FIELD;
}
/*********************************************************************************************
Class      : CEdtSubItemListCtrl
Function   : GetItemStyle
Description: Overwrite to select a window style

Parameters:   
nItem   : (E): table row    (int)
nSubItem: (E): table column  (int)
dwStyle: (E): current default window style (DWORD)

Result type: DWORD 
Author: Uwe Freiwald
created: December, 28 2005
<TITLE GetItemStyle>
*********************************************************************************************/
DWORD CEdtSubItemListCtrl::GetItemStyle(int nItem, int nSubItem, DWORD dwStyle)
{
	return dwStyle;
}

/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : DrawSubItem
 Description: Overwrite to draw the subitem in the table. The base class function draws the
			  text in a single line left aligned.

 Parameters: 
  pDC      : (E): Device context to draw (CDC*)
  nItem    : (E): table row  (int)
  nSubItem : (E): table column  (int)
  rcSubItem: (E): bounding rectangle (CRect&)
  sText    : (E): subitem text (CString)
  lParam   : (E): item data pointer  (LPARAM)

 Result type: subitem text (CString)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE GetSubItemText>
*********************************************************************************************/
void CEdtSubItemListCtrl::DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam)
{
	pDC->SetBkMode(TRANSPARENT);
	rcSubItem.DeflateRect(1, 0);
	pDC->DrawText(sText, rcSubItem, DT_SINGLELINE|DT_LEFT);
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : InitComboBox
 Description: Overwrite to initialise the ComboBox text an selection. Use member "m_Selected"
              to determine the selected subitem.

 Parameters:   
  Combo: (EA): ComboBox object to initialize  (CSubItemCombo*)
  sText: (E): current text of the subitem  (const CString&)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE InitComboBox>
*********************************************************************************************/
void CEdtSubItemListCtrl::InitComboBox(CSubItemCombo*pCombo, const CString& sText)
{
	// RKE: Sample code
//	pCombo->SetDroppedHeight(100);
//	pCombo->AddString(_T("yes"));
//	pCombo->AddString(_T("no"));
	
//	int nSel = pCombo->FindString(0, sText);
//	if (nSel != CB_ERR)
//	{
//		pCombo->SetCurSel(nSel);
//	}
}

/////////////////////////////////////////////////////////////////////////////
// CEdtSubItemListCtrl message handlers
void CEdtSubItemListCtrl::OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult)
{
	EditSubItem();
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CEdtSubItemListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        // This is the pre-paint stage for an item.  We need to make another
        // request to be notified during the post-paint stage.
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	
    else if ( (CDDS_ITEMPREPAINT| CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rcSubItem;
		int nItem = (int)pLVCD->nmcd.dwItemSpec;
		int nBrushColor = COLOR_WINDOW;
		if (   pLVCD->nmcd.uItemState & (LVIS_SELECTED|LVIS_FOCUSED)
			|| m_Selected.Cell.wItem == nItem)
		{
			nBrushColor = COLOR_3DLIGHT;
		}

		if (   m_Selected.Cell.wItem    == nItem 
			&& m_Selected.Cell.wSubItem == pLVCD->iSubItem)
		{
			nBrushColor = COLOR_3DSHADOW;
			if (GetDlgItem(GetSelectionID()))
			{
				return;
			}
		}
		else
		{
			if (GetDlgItem(GetSubItemID(nItem, pLVCD->iSubItem)))
			{
				return;
			}
		}
		
		CString sText = GetItemText(nItem, pLVCD->iSubItem);
		GetSubItemRect(nItem, pLVCD->iSubItem, rcSubItem);

		pDC->FillRect(rcSubItem, CBrush::FromHandle(GetSysColorBrush(nBrushColor)));
		DrawSubItem(pDC, nItem, pLVCD->iSubItem, rcSubItem, sText, pLVCD->nmcd.lItemlParam);

		*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
	}
}
/////////////////////////////////////////////////////////////////////////////
void CEdtSubItemListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (SelectSubItemFromCursorPos() == ALREADY_SELECTED)
	{
		EditSubItem();
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CEdtSubItemListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (SelectSubItemFromCursorPos())
	{
		EditSubItem();
	}
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CEdtSubItemListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == m_hWnd)
    {
		switch (pMsg->wParam)
		{
			case VK_RETURN:
				EditSubItem();
				return TRUE;
			case VK_LEFT: case VK_RIGHT:
			case VK_UP: case VK_DOWN:
			case VK_HOME: case VK_END:
			case VK_NEXT: case VK_PRIOR:
				OnArrow((UINT)pMsg->wParam, m_bIsEditing);
				return TRUE;
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : OnArrow
 Description: Moves to the neighbour subitem in the table with cursor keys and edits the 
              subitem, if allowed and wanted.

 Parameters:   
  nKey : (E): virtual key code  (UINT)
  bEdit: (E): edit after selection  (BOOL)

 Result type: TRUE (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE OnArrow>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::OnArrow(UINT nKey, BOOL bEdit)
{
	CWnd *pWnd = GetDlgItem(GetSelectionID());
	int nNextItem = m_Selected.Cell.wItem, nNextSubItem = m_Selected.Cell.wSubItem;
	if (pWnd)
	{
		if (!pWnd->SendMessage(WM_STOP_EDITING, FALSE, nKey)) 
		{
			return TRUE;
		}
	}

	switch (nKey)
	{
		case VK_UP:    nNextItem--;    break;
		case VK_DOWN:  nNextItem++;    break;
		case VK_LEFT:  nNextSubItem--; break;
		case VK_RIGHT: nNextSubItem++; break;
		case VK_HOME:
			nNextItem = 0;
			nNextSubItem = 0;
			nKey = VK_RETURN;
			break;
		case VK_END:
			nNextItem = GetItemCount()-1;
			nNextSubItem = GetSubItemCount()-1;
			nKey = VK_RETURN;
			break;
		case VK_PRIOR:
			nNextItem -= GetCountPerPage();
			if (nNextItem < 0)
			{
				nNextItem = 0;
			}
			nKey = VK_RETURN;
			break;
		case VK_NEXT:
			nNextItem += GetCountPerPage();
			if (nNextItem >= GetItemCount())
			{
				nNextItem = GetItemCount()-1;
			}
			nKey = VK_RETURN;
			break;
		case VK_RETURN: break;
	}

	if (SelectSubItem(nNextItem, nNextSubItem) == TRUE || nKey == VK_RETURN)
	{
		if (bEdit)
		{
			EditSubItem();
		}
		else if (nKey == VK_RETURN)
		{
			InvalidateRect(NULL);
		}
	}
	else
	{
		switch (nKey)
		{
			case VK_UP:    nKey = VK_LEFT;  break;
			case VK_DOWN:  nKey = VK_RIGHT; break;
			case VK_LEFT:  
				m_Selected.Cell.wSubItem = (WORD)GetSubItemCount()-1;
				if (m_Selected.Cell.wItem > 0)
				{
					nKey = VK_UP; 
				}
				else
				{
					nKey = VK_END;
				}
				break;
			case VK_RIGHT: 
				m_Selected.Cell.wSubItem = 0;
				if (m_Selected.Cell.wItem < GetItemCount()-1)
				{
					nKey = VK_DOWN; 
				}
				else
				{
					nKey = VK_HOME;
				}
				break;
		}
		return OnArrow(nKey, bEdit);
	}
	return TRUE;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : SelectSubItemFromCursorPos
 Description: Selects the subitem in the table and marks it

 Parameters:   
  pt: (E): Cursor position in client coordinates  (CPoint)

 Result type: (FALSE, TRUE, ALREADY_SELECTED) (BOOL)
 created: December, 14 2005
 <TITLE SelectSubItemFromCursorPos>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::SelectSubItemFromCursorPos(CPoint pt)
{
	LVHITTESTINFO lvhi;
	if (pt == CPoint(-1,-1))
	{
		GetCursorPos(&lvhi.pt);
		ScreenToClient(&lvhi.pt);
	}
	else
	{
		lvhi.pt = pt;
	}
	SubItemHitTest(&lvhi);

	return SelectSubItem(lvhi.iItem, lvhi.iSubItem);
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : SelectSubItem
 Description: Selects the subitem in the table and marks it

 Parameters:   
  nItem   : (E): table row  (int)
  nSubItem: (E): table column  (int)

 Result type: (FALSE, TRUE, ALREADY_SELECTED) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE SelectSubItem>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::SelectSubItem(int nItem, int nSubItem)
{
	if (m_bIsEditing)
	{
		return FALSE;
	}
	if (   m_Selected.Cell.wItem    != nItem
		|| m_Selected.Cell.wSubItem != nSubItem)
	{
		if (   IsBetween(nItem, 0, GetItemCount()-1)
			&& IsBetween(nSubItem, 0, GetSubItemCount()-1)
			)
		{
			BOOL bShift = GetKeyState (VK_SHIFT) < 0;
			if (!bShift)
			{
				SetItemState(m_Selected.Cell.wItem, 0, LVIS_SELECTED);
			}
			m_Selected.Cell.wItem    = (WORD)nItem;
			m_Selected.Cell.wSubItem = (WORD)nSubItem;
			SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
			SetSelectedColumn(nSubItem);
			EnsureVisible(nItem, FALSE);
			InvalidateRect(NULL);
			NMITEMACTIVATE nmia;
			ZERO_INIT(nmia);
			nmia.hdr.idFrom		= GetDlgCtrlID();
			nmia.hdr.hwndFrom	= m_hWnd;
			nmia.hdr.code		= LVN_ITEMACTIVATE;
			nmia.iItem			= nItem;
			nmia.iSubItem		= nSubItem;
			::SendMessage(GetParent()->GetSafeHwnd(),
				WM_NOTIFY, (WPARAM) nmia.hdr.idFrom, (LPARAM)&nmia);
			return TRUE;
		}
		return FALSE;
	}
	return ALREADY_SELECTED;
}
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : SetEditEndOnLostFocus
 Description: Defines whether edit mode stops on WM_KILLFOCUS. The default is TRUE.

 Parameters:   
  bEnd: (E): Stop edit on kill focus (TRUE, FALSE)  (BOOL)

 Result type:  (void)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE SetEditEndOnLostFocus>
*********************************************************************************************/
void CEdtSubItemListCtrl::SetEditEndOnLostFocus(BOOL bEnd)
{
	m_bEditEndOnLostFocus = bEnd;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : EditSubItem
 Description: Starts editing the subitem, if selected and allowed

 Parameters: None 

 Result type: Edit started (TRUE, FALSE) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 13 2005
 <TITLE EditSubItem>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::EditSubItem()
{
	if (   m_Selected.dwID != NO_ITEM_ID
		&& !m_bIsEditing
		&& CanEditSubItem(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem)
		&& EnsureVisible(m_Selected.Cell.wItem, FALSE))
	{
		CRect rcSubItem;
		if (GetSubItemRect(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem, rcSubItem))
		{
			CString sText = GetItemText(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem);
			eEditType editType = GetEditType(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem);
			if (editType == EDIT_FIELD)
			{
				CSubItemEdit*pEdit = new CSubItemEdit(this);
				DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER;
				dwStyle = GetItemStyle(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem,dwStyle);
				pEdit->Create(dwStyle, rcSubItem, this, GetSelectionID()); 
				pEdit->SetWindowText(sText);
			}
			else if (editType == COMBO_BOX)
			{
				CSubItemCombo *pCombo = new CSubItemCombo(this);
				DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL;
				dwStyle = GetItemStyle(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem,dwStyle);
				pCombo->Create(dwStyle, rcSubItem, this, GetSelectionID());
				InitComboBox(pCombo, sText);
				pCombo->ShowDropDown(TRUE);
			}
			return TRUE;
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CEdtSubItemListCtrl::GetSubItemCount()
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	if (pHeaderCtrl != NULL)
	{
		return pHeaderCtrl->GetItemCount();
	}
	return 1;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : ValidateAll
 Description: Validates the values of all subitems in the table and selects and edits the
              subitem, if the value is not correct and editing of this item is allowed.

 Parameters: None 

 Result type:  (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE ValidateAll>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::ValidateAll()
{
	int nCount = GetItemCount();
	if (nCount)
	{
		CSubItemEdit*pEdit = new CSubItemEdit(this);
		CRect rcSubItem(0,0,0,0);
		DWORD dwStyle = WS_CHILD|ES_AUTOHSCROLL|WS_BORDER;
		pEdit->Create(dwStyle, rcSubItem, this, 0); 
		CString sText;
		for (m_Selected.Cell.wItem=0; m_Selected.Cell.wItem<nCount; m_Selected.Cell.wItem++)
		{
			for (m_Selected.Cell.wSubItem=0; m_Selected.Cell.wSubItem<GetSubItemCount(); m_Selected.Cell.wSubItem++)
			{
				pEdit->SetDlgCtrlID(GetSelectionID());
				sText = GetItemText(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem);
				pEdit->SetWindowText(sText);
				pEdit->m_isClosing = true;
				if (!UpdateData())
				{
					pEdit->m_isClosing = false;
					GetSubItemRect(m_Selected.Cell.wItem, m_Selected.Cell.wSubItem, rcSubItem);
					pEdit->MoveWindow(rcSubItem);
					pEdit->ShowWindow(SW_SHOW);
					return FALSE;
				}
			}
		}
		pEdit->DestroyWindow();
	}
	return TRUE;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : GetSelection
 Description: retrieves the selected subitem in the table.

 Parameters:   
  nItem   : (A): table row  (int&)
  nSubItem: (A): table column  (int&)

 Result type: An item was selected (TRUE, FALSE) (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE GetSelection>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::GetSelection(int &nItem, int &nSubItem)
{
	if (m_Selected.dwID != NO_ITEM_ID)
	{
		nItem    = m_Selected.Cell.wItem;
		nSubItem = m_Selected.Cell.wSubItem;
		return TRUE;
	}
	return FALSE;
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : GetSelectionID
 Description: retrieves the dialog control ID of the selected subitem.

 Parameters: None 

 Result type: ID of the subitem (DWORD)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE GetSelectionID>
*********************************************************************************************/
DWORD CEdtSubItemListCtrl::GetSelectionID()
{
	return m_Selected.GetID();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CEdtSubItemListCtrl::GetSubItemID(int nItem, int nSubItem)
{
	return uListItem((WORD)nItem, (WORD)nSubItem).GetID();
}
/*********************************************************************************************
 Class      : CEdtSubItemListCtrl
 Function   : GetSubItemRect
 Description: retrieves the bounding rectangle of the subitem.

 Parameters:   
  nItem   : (E): table row  (int)
  nSubItem: (E): table column  (int)
  rc      : (A): bounding rectangle  (CRect&)

 Result type:  (BOOL)
 Author: Rolf Kary-Ehlers
 created: December, 14 2005
 <TITLE GetSubItemRect>
*********************************************************************************************/
BOOL CEdtSubItemListCtrl::GetSubItemRect(int nItem, int nSubItem, CRect &rc)
{
	BOOL bReturn = CListCtrl::GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rc);
	if (bReturn && nSubItem == 0 && GetSubItemCount() > 1)
	{
		CRect rc2;
		CListCtrl::GetSubItemRect(nItem, 1, LVIR_BOUNDS, rc2);
		rc.right = rc2.left;
	}
	return bReturn;
}
