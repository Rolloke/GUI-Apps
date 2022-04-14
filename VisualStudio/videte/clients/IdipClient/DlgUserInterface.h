#pragma once
#include "afxcmn.h"


// CDlgUserInterface dialog

class CDlgUserInterface : public CSkinDialog
{
	DECLARE_DYNAMIC(CDlgUserInterface)

public:
	CDlgUserInterface(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUserInterface();

// Dialog Data
	enum  { IDD = IDD_USER_INTERFACE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void SaveTreeData(HTREEITEM hParent, BOOL bTest);

private:
	CSkinTree	m_UI_Tree;
	int			m_nVisibility;
	HTREEITEM	m_hSelected;

protected:
	afx_msg void OnTvnSelchangedUiTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
protected:
	virtual void OnCancel();
public:
	int m_nDivisionMode;
	BOOL m_bAsList;
	BOOL m_bMinimized;
};
