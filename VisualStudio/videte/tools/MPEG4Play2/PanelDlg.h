#pragma once


// CPanelDlg dialog

class CPanelDlg : public CDialog
{
	DECLARE_DYNAMIC(CPanelDlg)

public:
	CPanelDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelDlg();

// Attribute /////////////////////////////////////////////////////////////
private:
	CString m_strFilename;
	CToolTipCtrl m_ToolTips; 

// Dialog Data
	enum { IDD = IDD_DIALOG_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void SetFilename(CString strFilename);
	CString GetFilename();
	virtual void OnCancel(void);
	virtual void PostNcDestroy(void);
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonNextPic();
	afx_msg void OnBnClickedButtonPrevPic();
	virtual BOOL OnInitDialog(void);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonEnd();
	BOOL PreTranslateMessage(MSG* pMsg); 
};
