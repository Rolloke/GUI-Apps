/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: statuslist.h $
// ARCHIVE:		$Archive: /DV/SystemVerwaltung/statuslist.h $
// CHECKIN:		$Date: 22.02.01 11:06 $
// VERSION:		$Revision: 1 $
// LAST CHANGE:	$Modtime: 3.08.00 12:27 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef StatusList_H
#define StatusList_H

/////////////////////////////////////////////////////////////////////////////
// CStatusList window
class CStatusList : public CListCtrl
{
// Construction
public:
	CStatusList();

// Attributes
public:

// Operations
public:
	BOOL DeleteAllItems();
	void SetStateInfo(CImageList* pImageList, int nStates, int nWidth); 
												// 0 = radio mode
												// 1 = one check 
												// 2... more checks
								// nWidth column width for every checkbox in state image list

	// for check mode, use multiple selection
	UINT GetItemStatus(int nIndex);
	void SetItemStatus(int nIndex,UINT uStatus);

	// for radio mode, use single selection
	void SetRadioItem(int nIndex);
	void SetRadioItemFromlParam(LPARAM lParam);
	int  GetRadioItem();
	LPARAM  GetRadioItemData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStatusList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusList)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	void OnSpace();

	DECLARE_MESSAGE_MAP()

protected:
	int m_nStates;
	int m_nWidth;
	int m_RadioIndex;
};

/////////////////////////////////////////////////////////////////////////////
#endif // StatusList_H
