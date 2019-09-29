#pragma once


// CTreeCtrlEx

class CTreeCtrlEx : public CTreeCtrl
{
    friend class CAnyFileViewerTreeView;
    typedef CMap< UINT, UINT, CWnd*, CWnd* > mapItemToCB;

    class CTreeComboBox : public CComboBox
    {
    public:
        CTreeComboBox(UINT nID);
        UINT GetID() { return mID; }

    private:
        CString            strCurrText;
        UINT            mID;

        //{{AFX_MSG(CTreeComboBox)
        afx_msg void    OnCloseUp();
        afx_msg void    OnKillFocus();
        afx_msg void    OnEditUpdate();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    DECLARE_DYNAMIC(CTreeCtrlEx)

public:

    CTreeCtrlEx();
    virtual ~CTreeCtrlEx();

    // Public Interface
public:

    //
    // Set Dropped height for item's combo box
    //
    // Arguments:    iNewHeight - number of pixels of dropped combo box height
    //
    void            SetComboDroppedHeight(int iNewHeight);

    //
    // Set minimum and maximux width for combo box
    //
    // Arguments:    iNewMinWidth - minimum preferred number of pixels for combo box width
    //                                (width of combo box is always restricted by control's rectangle)
    //                iNewMaxWidth - maximum number of pixels for combo box width
    //
    void            SetComboMinMaxWidths(UINT iNewMinWidth = 150, UINT iNewMaxWidth = UINT_MAX);

    //
    // Insert item into control with attached dropdown list (combo box)
    //
    // Arguments:    lpszItem, hParent, hInsertAfter - as in InsertItem(...) function in CTreeCtrl
    //                bEditableCombo - determines the type of combo box. TRUE - editable, FALSE - drop down list
    //
    // Return value: Same as in InsertItem(...) function in CTreeCtrl
    //
    CComboBox&      InsertComboBox(UINT aID, bool bEditableCombo=false);

    //
    // Remove specified item's combo box
    //
    // Arguments:    htiRemoveFrom - tree item, from which combo box need to be removed
    //
    void            RemoveComboBox(UINT aID);
    //
    // Remove all combo boxes from control
    //
    // Arguments:    none
    //
    void            RemoveAllComboBoxes();


    //
    // Next two functions overriden from parent class to prevent deleting tree item 
    // without deleting attached combo box
    //
    // Arguments:    same as in same function in parent class
    //
    //
    // Return value: Same as in parent class.
    //
    BOOL            DeleteItem(HTREEITEM hItem);
    BOOL            DeleteAllItems();

private:
    UINT            m_iComboDroppedHeight;    // Height of dropped list of item's combo box
    UINT            m_iComboMinWidth;        // Minimum width of item's combo box
    UINT            m_iComboMaxWidth;        // Maximum width of item's combo box
    mapItemToCB        m_mapExControls;        // Table for attached combo boxes 

private:
    //
    // For internal use only
    // Show attached combo box if there is one.
    //
    // Arguments:    hti - descriptor of tree item in which dropped list will be shown
    //
    // Return value: true if success, false otherwise
    //
    bool            ShowComboBox(HTREEITEM hti, UINT aID);

    //{{AFX_VIRTUAL(CTreeCtrlEx)
protected:
//    virtual void    PreSubclassWindow();
    //}}AFX_VIRTUAL


protected:
    afx_msg void    OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void    OnLButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};


