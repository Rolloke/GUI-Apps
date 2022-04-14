#if !defined(AFX_MSGFLAGSDLG_H__FD482B67_71BF_4616_B18F_B9FC2E525395__INCLUDED_)
#define AFX_MSGFLAGSDLG_H__FD482B67_71BF_4616_B18F_B9FC2E525395__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgFlagsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMsgFlagsDlg dialog

class CMsgFlagsDlg : public CDialog
{
// Construction
public:
	CMsgFlagsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMsgFlagsDlg)
	enum { IDD = IDD_FLAGS_MSG };
	CListCtrl	m_ListMsgFlags;
	//}}AFX_DATA
	DWORD m_dwFlags;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsgFlagsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMsgFlagsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSGFLAGSDLG_H__FD482B67_71BF_4616_B18F_B9FC2E525395__INCLUDED_)
