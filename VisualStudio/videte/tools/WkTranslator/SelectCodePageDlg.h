#if !defined(AFX_SELECTCODEPAGEDLG_H__4FA6CC3B_E7E1_4F18_9130_D2AD8E2A392B__INCLUDED_)
#define AFX_SELECTCODEPAGEDLG_H__4FA6CC3B_E7E1_4F18_9130_D2AD8E2A392B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectCodePageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectCodePageDlg dialog

class CSelectCodePageDlg : public CDialog
{
// Construction
public:
	CSelectCodePageDlg(CWnd* pParent = NULL);   // standard constructor
protected:
// Dialog Data
	//{{AFX_DATA(CSelectCodePageDlg)
	enum { IDD = IDD_SELECT_CODEPAGE };
	CComboBox	m_cCodePages;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectCodePageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	WORD m_wCodePage;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectCodePageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static BOOL CALLBACK EnumCodePagesProc(LPTSTR lpCodePageString);
	static HWND gm_hWndCombo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTCODEPAGEDLG_H__4FA6CC3B_E7E1_4F18_9130_D2AD8E2A392B__INCLUDED_)
