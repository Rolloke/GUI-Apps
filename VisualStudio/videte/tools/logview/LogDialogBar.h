#pragma once


// CLogDialogBar

class CLogDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CLogDialogBar)

public:
	CLogDialogBar();
	virtual ~CLogDialogBar();

private:
	CString m_strTipText;

protected:
	//{{AFX_MSG(CLogDialogBar)
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg void OnEditSearchTimeValue();
	afx_msg BOOL OnToolTipNotify(UINT /*id*/, NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};


