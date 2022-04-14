#pragma once

typedef LRESULT (CWnd::*ON_MESSAGE_FNC)(WPARAM,LPARAM);

#define WM_EDITINGLOSTFOCUS (WM_USER+1)
#define WM_STOP_EDITING		(WM_USER+2)

#define	NO_ITEM_ID			0xffffffff
#define ALREADY_SELECTED	2

enum eEditType
{
	EDIT_FIELD = 1,
	COMBO_BOX = 2
};
/////////////////////////////////////////////////////////////////////////////
// CSubItemEdit window
class CSubItemEdit : public CEdit
{
	friend class CEdtSubItemListCtrl;

public:
	CSubItemEdit (CEdtSubItemListCtrl* pCtrl);
    virtual ~CSubItemEdit();

    //{{AFX_VIRTUAL(CSubItemEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
    CEdtSubItemListCtrl*	m_pListCtrl;
	bool					m_isClosing;
    
    //{{AFX_MSG(CSubItemEdit)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg BOOL StopEdit(BOOL bCancel, UINT endkey=0);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSubItemCombo window
class CSubItemCombo: public CComboBox
{
	friend class CEdtSubItemListCtrl;

public:
	CSubItemCombo (CEdtSubItemListCtrl* pCtrl);
    virtual ~CSubItemCombo();

	// Operations
public:
	int SetDroppedHeight(int nHeight);

    //{{AFX_VIRTUAL(CSubItemCombo)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CSubItemCombo)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcDestroy();
	afx_msg BOOL StopEdit(BOOL bCancel, UINT endkey=0);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    CEdtSubItemListCtrl*	m_pListCtrl;
	bool					m_isClosing;
public:
};

/////////////////////////////////////////////////////////////////////////////
// CEdtSubItemListCtrl
class CEdtSubItemListCtrl : public CListCtrl
{
	friend class CSubItemEdit;
	friend class CSubItemCombo;
	DECLARE_DYNAMIC(CEdtSubItemListCtrl)

	struct sListItem
	{
		WORD wItem, wSubItem;
	};

public:
	CEdtSubItemListCtrl();
	virtual ~CEdtSubItemListCtrl();

	// Attributes
public:
	BOOL	GetSelection(int &nItem, int &nSubItem);
	int		GetSubItemCount();
	DWORD	GetSelectionID();
	BOOL	GetSubItemRect(int nItem, int nSubItem, CRect &rc);

	static DWORD GetSubItemID(int nItem, int nSubItem);

	// Operations
public:
	BOOL SelectSubItem(int nItem, int nSubItem);
	BOOL SelectSubItemFromCursorPos(CPoint pt=CPoint(-1,-1));
	BOOL EditSubItem();
	BOOL OnArrow(UINT nKey, BOOL bEdit);
	BOOL ValidateAll();
	void SetEditEndOnLostFocus(BOOL bEnd);

protected:
   //{{AFX_VIRTUAL(CEdtSubItemListCtrl)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL CanEditSubItem(int nItem, int nSubItem);
	virtual eEditType GetEditType(int nItem, int nSubItem);
	virtual DWORD GetItemStyle(int nItem, int nSubItem, DWORD dwStyle);
	virtual void InitComboBox(CSubItemCombo*pCombo, const CString& sText);
	virtual void DrawSubItem(CDC*pDC, int nItem, int nSubItem, CRect&rcSubItem, CString sText, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CEdtSubItemListCtrl)
	afx_msg void OnNMReturn(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


protected:
	union uListItem
	{
		uListItem()
		{
			dwID = NO_ITEM_ID;
		}
		uListItem(WORD wItem, WORD wSubItem) 
		{ 
			Cell.wItem = wItem; 
			Cell.wSubItem = wSubItem; 
		}
		DWORD GetID()
		{
			return dwID +1; 
		}

		sListItem	Cell;
		DWORD		dwID;
	} m_Selected;
	BOOL m_bEditEndOnLostFocus;
private:
	BOOL m_bIsEditing;
};


