#if !defined(AFX_SETUPDIALOG_H__D540E533_0C49_11D4_A0D8_004005A19028__INCLUDED_)
#define AFX_SETUPDIALOG_H__D540E533_0C49_11D4_A0D8_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog

class CSetupDialog : public CDialog
{
// Construction
public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupDialog)
	enum { IDD = IDD_SETUP };
	CProgressCtrl	m_ctrlProgress;
	CButton	m_ctrlButtonOK;
	CButton	m_ctrlButtonCancel;
	CButton	m_ctrlButtonUpdate;
	CButton	m_ctrlButtonSend;
	CListBox	m_ctrlListConfirmed;
	CListBox	m_ctrlListSend;
	CString	m_sFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// operations
public:
	void OnError(const CString& sError);
	void OnConfirmFile(const CString& sFile);

// Implementation
protected:
	void SendCurrentFile();
	void NextFile();

	// Generated message map functions
	//{{AFX_MSG(CSetupDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CStringArray m_saSend;
	int			 m_iRetries;
	CString		 m_sCommand;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDIALOG_H__D540E533_0C49_11D4_A0D8_004005A19028__INCLUDED_)
