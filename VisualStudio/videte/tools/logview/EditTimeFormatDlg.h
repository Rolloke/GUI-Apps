#pragma once
#include "afxcmn.h"


// EditTimeFormatDlg-Dialogfeld

class EditTimeFormatDlg : public CDialog
{
	DECLARE_DYNAMIC(EditTimeFormatDlg)

public:
	EditTimeFormatDlg(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~EditTimeFormatDlg();

// Dialogfelddaten
	enum { IDD = IDD_EDIT_TIME_FORMATS };
    CListCtrl m_List;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    virtual BOOL OnInitDialog();

    afx_msg void OnLvnGetdispinfoTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginlabeleditTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnEndlabeleditTimeFormatList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	DECLARE_MESSAGE_MAP()
    CString mToolTipText;
public:
};
