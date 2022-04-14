#if !defined(AFX_INPUTDIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_INPUTDIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InputDialog.h : header file
//

class CInputPage;
class CInput;

#include "TreeList.h"
/////////////////////////////////////////////////////////////////////////////
// CInputDialog dialog

class CInputDialog : public CWK_Dialog
{
// Construction
public:
	CInputDialog(CInputPage* pInputPage, CInput* pInput);

// Dialog Data
	//{{AFX_DATA(CInputDialog)
	enum { IDD = IDD_INPUT };
	CStatic	m_StaticTypeInput;
	CSpinButtonCtrl	m_SpinTypeInput;
	CButton	m_btnTempDeactivate;
	CButton	m_btnInstruction;
	CButton	m_btnEdge;
	CEdit	m_editName;
	BOOL	m_bEdge;
	BOOL	m_bPushButton;
	CString	m_sName;
	CString	m_sNr;
	BOOL	m_bTempDeactivate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	virtual BOOL StretchElements();

// Implementation
protected:
	void SetInput(CInput* pInput);
	void InputToControl();
	void ControlToInput();
	void InitTypeTree();
	void ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CInputDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnChange();
	afx_msg void OnInstruction();
	afx_msg void OnChangeEditTypeInput();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:
	CInputPage*	m_pInputPage;
	CInput*		m_pInput;
	int			m_iTypePos;
/*
	HTREEITEM	m_hOff;
	HTREEITEM	m_hMelder;
	HTREEITEM	m_hBlock;
*/
   static int gm_nList[3];

	friend class CInputPage;
};
WK_PTR_ARRAY(CInputDialogArray,CInputDialog*);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDIALOG_H__3F82D902_1946_11D1_93E0_00C095ECA33E__INCLUDED_)
