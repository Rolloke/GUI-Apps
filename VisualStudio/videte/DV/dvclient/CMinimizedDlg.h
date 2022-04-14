/* GlobalReplace: CSkinsDlg --> CPanel */
#if !defined(AFX_CMINIMIZEDDLG_H__4CED19C3_B83A_11D3_8E0E_004005A11E32__INCLUDED_)
#define AFX_CMINIMIZEDDLG_H__4CED19C3_B83A_11D3_8E0E_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CMinimizedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg dialog
class CPanel;

class CMinimizedDlg : public CDialog
{
// Construction
public:
	BOOL MoveToCorner();
	CMinimizedDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create	();

// Dialog Data
	//{{AFX_DATA(CMinimizedDlg)
	enum { IDD = IDD_MINIMIZED };
	CStatic	m_ctrlBitmap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMinimizedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CMinimizedDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CPanel* m_pPanel;
	CBitmap m_Bitmap;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMINIMIZEDDLG_H__4CED19C3_B83A_11D3_8E0E_004005A11E32__INCLUDED_)
