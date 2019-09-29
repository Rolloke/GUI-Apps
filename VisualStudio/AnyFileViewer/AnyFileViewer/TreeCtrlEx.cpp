// TreeCtrlEx.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "TreeCtrlEx.h"


// CTreeCtrlEx

IMPLEMENT_DYNAMIC(CTreeCtrlEx, CTreeCtrl)

CTreeCtrlEx::CTreeCtrlEx() : m_iComboMinWidth(150),
m_iComboMaxWidth(UINT_MAX),
m_iComboDroppedHeight(200)
{

}

CTreeCtrlEx::~CTreeCtrlEx()
{
    RemoveAllComboBoxes();
}

BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeyDown)
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// Implementation of CTreeCtrlEx::CTreeComboBox class
BEGIN_MESSAGE_MAP(CTreeCtrlEx::CTreeComboBox, CComboBox)
    //{{AFX_MSG_MAP(CTreeComboBox)
    ON_CONTROL_REFLECT(CBN_CLOSEUP, CTreeComboBox::OnCloseUp)
    ON_CONTROL_REFLECT(CBN_KILLFOCUS, CTreeComboBox::OnKillFocus)
    ON_CONTROL_REFLECT(CBN_EDITUPDATE, CTreeComboBox::OnEditUpdate)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



// CTreeCtrlEx-Meldungshandler

inline void CTreeCtrlEx::SetComboDroppedHeight(int iNewHeight)
{
    m_iComboDroppedHeight = iNewHeight;
}

inline void CTreeCtrlEx::SetComboMinMaxWidths(UINT iNewMinWidth /*= 150*/, UINT iNewMaxWidth /*= UINT_MAX*/)
{
    m_iComboMinWidth = iNewMinWidth;
    if (iNewMaxWidth < iNewMinWidth)
        m_iComboMaxWidth = iNewMinWidth;
    else
        m_iComboMaxWidth = iNewMaxWidth;
}


CComboBox& CTreeCtrlEx::InsertComboBox(UINT aID, bool bEditableCombo)
{
    CTreeComboBox *pCB = NULL;

    if (!m_mapExControls.Lookup(aID, (CWnd*&)pCB))
    {
        pCB = new CTreeComboBox(aID);
        DWORD dwComboBoxStyle = WS_CHILD | WS_VSCROLL;
        if (bEditableCombo)
        {
            dwComboBoxStyle |= CBS_DROPDOWN;
        }
        else
        {
            dwComboBoxStyle |= CBS_DROPDOWNLIST;
        }
        if (pCB->Create(dwComboBoxStyle, CRect(), this, 0))
        {
            m_mapExControls[aID] = pCB;
        }
    }

    return *pCB;
}

void CTreeCtrlEx::RemoveComboBox(UINT aID)
{
    CTreeComboBox *pCB = NULL;
    if (m_mapExControls.Lookup(aID, (CWnd*&)pCB))
    {
        m_mapExControls.RemoveKey(aID);
        if (::IsWindow(pCB->m_hWnd))
        {
            pCB->DestroyWindow();
        }
        delete pCB;
    }
}

void CTreeCtrlEx::RemoveAllComboBoxes()
{
    UINT fID;
    CTreeComboBox *pCB;

    POSITION pos = m_mapExControls.GetStartPosition();
    while (pos)
    {
        m_mapExControls.GetNextAssoc(pos, fID, (CWnd*&)pCB);
        RemoveComboBox(fID);
    }
}

BOOL CTreeCtrlEx::DeleteAllItems()
{
    RemoveAllComboBoxes();
    return CTreeCtrl::DeleteAllItems();
}

bool CTreeCtrlEx::ShowComboBox(HTREEITEM aTI, UINT aID)
{
    CTreeComboBox *pCB;
    if (m_mapExControls.Lookup(aID, (CWnd*&)pCB))
    {
        // Set combo box dimentions and position 
        CRect itemRect;
        CRect clientRect;
        SCROLLINFO sInfo;
        int iVScrollWidth = 3; // Make the combo box fully visible

        // Select current item in combo box if it's found.
        CString strItemText = GetItemText(aTI);
        if (pCB->GetID() == TVHT_ONITEMICON)
        {
            int fImage;
            GetItemImage(aTI, fImage, fImage);
            int i, n = pCB->GetCount();
            for (i = 0; i < n; ++i)
            {
                if (pCB->GetItemData(i) == fImage)
                {
                    pCB->SetCurSel(i);
                    break;
                }
            }
            if (i == n)
            {
                return false;
            }
        }
        else
        {
            int iFIndex = pCB->FindString(-1, strItemText);
            if (iFIndex != CB_ERR)
            {
                pCB->SetCurSel(iFIndex);
            }
            else
            {
                pCB->SetWindowText(strItemText);
            }
        }

        GetItemRect(aTI, &itemRect, TRUE);
        GetWindowRect(&clientRect);

        // Determine if scrollbar is visible
        memset(&sInfo, 0, sizeof(SCROLLINFO));
        sInfo.cbSize = sizeof(SCROLLINFO);
        GetScrollInfo(SB_VERT, &sInfo);
        if (sInfo.nPage) // Determine if is there vertical scroll bar in tree ctrl.
        {
            iVScrollWidth += ::GetSystemMetrics(SM_CXVSCROLL);
        }
        int iComboLeft = itemRect.left;
        UINT iComboWidth;
        int iClientWidth = clientRect.Width();

        if (m_iComboMaxWidth < m_iComboMinWidth)
        {
            m_iComboMaxWidth = m_iComboMinWidth;
        }
        if ((m_iComboMaxWidth == UINT_MAX) || (m_iComboMaxWidth >= UINT(iClientWidth - itemRect.left - iVScrollWidth)))
        {
            iComboWidth = iClientWidth - itemRect.left - iVScrollWidth;
        }
        else
        {
            iComboWidth = m_iComboMaxWidth;
        }
        if ((iComboWidth < m_iComboMinWidth) && (int(iComboWidth) + iComboLeft >= iClientWidth - iVScrollWidth))
        {
            iComboLeft = iClientWidth - iVScrollWidth - m_iComboMinWidth;
            iComboWidth = m_iComboMinWidth;
        }

        if (m_iComboMinWidth > UINT(iClientWidth - iVScrollWidth))
        {
            iComboLeft = 0;
            iComboWidth = iClientWidth - iVScrollWidth;
        }

        pCB->SetWindowPos(NULL, iComboLeft, itemRect.top, iComboWidth, m_iComboDroppedHeight, SWP_NOZORDER);

        // Set the same font as CTreeCtrl has.
        pCB->SetFont(GetFont(), FALSE);

        pCB->ShowWindow(SW_SHOWNORMAL);
        pCB->ShowDropDown(TRUE);
        pCB->SetFocus();
    }
    return true;
}

void CTreeCtrlEx::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
    TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

    if (!(GetKeyState(VK_MENU) & 0x8000))
    {
        switch (pTVKeyDown->wVKey)
        {
        case VK_F2:
            EditLabel(GetSelectedItem());
            *pResult = 1;
            break;
        case VK_F4:
            if (ShowComboBox(GetSelectedItem(), TVHT_ONITEMICON))
            {
                *pResult = 1;
            }
            break;
        }
    }

    *pResult = 0;
}

void CTreeCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        CTreeCtrl::OnLButtonDblClk(nFlags, point);
        return;
    }

    // Determine where DblClick appears
    UINT flags;
    if (HitTest(point, &flags))
    {
        if ((flags & TVHT_ONITEMICON))
        {
            if (ShowComboBox(GetSelectedItem(), TVHT_ONITEMICON))
            {
                return;
            }
        }
    }
    CTreeCtrl::OnLButtonDblClk(nFlags, point);
}


CTreeCtrlEx::CTreeComboBox::CTreeComboBox(UINT nID) : mID(nID)
{
}

void CTreeCtrlEx::CTreeComboBox::OnCloseUp()
{
    CString strText;
    CTreeCtrl* pParent = (CTreeCtrl*)GetParent();
    ASSERT(pParent);

    int iCurSel = GetCurSel();
    if (iCurSel == CB_ERR)
    {
        strText = strCurrText;
    }
    else
    {
        GetLBText(iCurSel, strText);
    }

    switch (mID)
    {
        case TVHT_ONITEMICON:
        if (iCurSel != CB_ERR)
        {
            int fImage = static_cast<int>(GetItemData(iCurSel));
            pParent->SetItemImage(pParent->GetSelectedItem(), fImage, fImage);
        }   break;
        case TVHT_ONITEMLABEL:
        if (!strText.IsEmpty())
        {
            pParent->SetItemText(pParent->GetSelectedItem(), strText);
        }
        break;
    }
    pParent->SetFocus();
}

void CTreeCtrlEx::CTreeComboBox::OnKillFocus()
{
    ShowWindow(SW_HIDE);
}

void CTreeCtrlEx::CTreeComboBox::OnEditUpdate()
{
    // We need to hold last entered text to set it as an item description, 'cause
    // combo box sets matched text into it's edit part
    GetWindowText(strCurrText);
}


