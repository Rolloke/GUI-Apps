#if !defined(AFX_SOCKETPAGE_H__EB347BA3_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_SOCKETPAGE_H__EB347BA3_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SocketPage.h : header file
//

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSocketPage dialog

class CSocketPage : public CSVPage
{
// Construction
public:
	CSocketPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSocketPage)
	enum { IDD = IDD_SOCKET };
	CButton	m_btnUseIPHostList;
	CRichEditCtrl	m_EditHostList;
	CButton	m_btnTestTCP;
	CStatic	m_ctlTestResult;
	CEdit	m_editSync;
	CButton	m_btn64;
	CButton	m_btn128;
	CButton	m_btn256;
	CButton	m_btn512;
	CButton	m_btn1MBit;
	CButton	m_btn2MBit;
	CButton	m_btn4MBit;
	CButton	m_btn8MBit;
	CButton	m_btnUseDNS;
	CButton	m_btnDoListen;
	CButton	m_btnDoCacheUnknownNames;
	CButton	m_btnDoCacheKnownNames;
	CButton	m_btnDoBeepOnIncoming;
	CButton	m_btnAllowOutgoingCalls;
	BOOL	m_bSocketUnit;
	BOOL	m_bUseDNS;
	BOOL	m_bDoCacheUnknownNames;
	BOOL	m_bDoCacheKnownNames;
	BOOL	m_bDoBeepOnIncoming;
	BOOL	m_bDoListen;
	BOOL	m_bAllowOutgoingCalls;
	UINT	m_dwSyncRetryTimeSeconds;
	int		m_iBitrate;
	BOOL	m_bUseHostList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSocketPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	void ShowHide();

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSocketPage)
	afx_msg void OnCheckSocket();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnTcpipTest();
	afx_msg void OnUseIPHostList();
	afx_msg void OnChangeEditIphostlist();
	afx_msg void OnCopyTcpToList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL LoadHostListFromRegistry();
	BOOL SaveHostListToRegistry();
	BOOL CheckIPList();
	BOOL CheckLine(int nLine);
	BOOL CheckIPSyntax(const CString &sHost);
	void SetSelection(int nLine, COLORREF colRef);
	BOOL IsHostInHostList(const CString &sHost);

private:
	BOOL m_bLoadingHostList;
	CFont m_fixedFont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKETPAGE_H__EB347BA3_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
