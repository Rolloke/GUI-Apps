// SelectInputDlg.h : header file
//

#include "InputList.h"
#include "InputGroup.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectInputDlg dialog

class CSelectInputDlg : public CWK_Dialog
{
// Construction
public:
	CSelectInputDlg(CWnd* pParent = NULL, CInputList* pInputList = NULL);   // standard constructor


	CInput*	GetSelectedInput();
	// Dialog Data
	//{{AFX_DATA(CSelectInputDlg)
	enum { IDD = IDD_INPUT_SELECT };
	CListCtrl	m_Inputs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectInputDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkInputList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CInputList* m_pInputList;
	CInput*		m_pSelectedInput;
};
