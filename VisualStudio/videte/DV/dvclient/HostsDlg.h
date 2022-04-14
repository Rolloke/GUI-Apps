// HostsDlg.h : header file
//
#if !defined(AFX_HOSTSDLG_H__E4AAB653_8BE1_11D5_BB63_0050BF49CEBF__INCLUDED_)
#define AFX_HOSTSDLG_H__E4AAB653_8BE1_11D5_BB63_0050BF49CEBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
class CPanel;
class CHost;
class CKeyboardDlg;

/////////////////////////////////////////////////////////////////////////////
// CHostsDlg dialog
class CHostsDlg : public CTransparentDialog
{
// Construction
public:
	CHostsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHostsDlg();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHostsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHostsDlg)
	afx_msg void OnEditName();
	afx_msg void OnEditIP();
	afx_msg void OnItemchangedHostsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnDelete();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDown();
	afx_msg void OnDownEnd();
	afx_msg void OnUp();
	afx_msg void OnUpStart();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void	EnableUpDownButtons();
	BOOL	ValidateIP(const CString &sIP);
	void	WaitToReadErrorMessage();
	BOOL	EditName();
	BOOL	EditIP();

// Dialog Data
private:
	//{{AFX_DATA(CHostsDlg)
	enum { IDD = IDD_HOSTS };
	CDisplay	m_displayName;
	CDisplay	m_displayIP;
	CDisplay	m_displayToolTip;
	CFrame	m_frameIP;
	CFrame	m_frameName;
	CFrame	m_frameToolTip;
	CSkinListCtrl	m_listHosts;
	CSkinButton	m_btnUpStart;
	CSkinButton	m_btnUp;
	CSkinButton	m_btnDown;
	CSkinButton	m_btnDownEnd;
	CSkinButton	m_btnNew;
	CSkinButton	m_btnDelete;
	CSkinButton	m_btnOK;
	//}}AFX_DATA

	CSkinButtons	m_Buttons;
	CDisplays		m_Displays;
	CImageList		m_ilUpStart;
	CImageList		m_ilUp;
	CImageList		m_ilDown;
	CImageList		m_ilDownEnd;
	CImageList		m_ilNew;
	CImageList		m_ilDelete;
	CImageList		m_ilOK;

	HCURSOR			m_hCursor;
	CPanel*			m_pPanel;
	CHostArray		m_Hosts;
	CString			m_sHost;
	CString			m_sIP;
	int				m_iSelectedItem;
	CHost*			m_pSelectedHost;
	CKeyboardDlg*	m_pKeyboardDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOSTSDLG_H__E4AAB653_8BE1_11D5_BB63_0050BF49CEBF__INCLUDED_)
