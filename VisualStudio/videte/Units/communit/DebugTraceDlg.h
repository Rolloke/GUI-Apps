#pragma once


// CDebugTraceDlg dialog

class CDebugTraceDlg : public CWK_Dialog
{
	DECLARE_DYNAMIC(CDebugTraceDlg)

public:
	CDebugTraceDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugTraceDlg();

protected:
	virtual BOOL StretchElements() { return TRUE; };

// Dialog Data
public:
	enum { IDD = IDD_DEBUG_TRACE };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnClearOutput();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCkTopmost();
	afx_msg void OnBnClickedCkSettings();
	afx_msg void OnBnClickedBtnReset();
};
