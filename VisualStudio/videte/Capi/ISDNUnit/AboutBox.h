// aboutbox.h : header file
//
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include "resource.h"

class CISDNConnection;

/////////////////////////////////////////////////////////////////////////////
// CBigIcon window
class CBigIcon : public CButton
{
// Attributes
public:

// Operations
public:
	void SizeToContent();

// Implementation
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//{{AFX_MSG(CBigIcon)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CAboutBox dialog
class CAboutBox : public CDialog
{
// Construction
public:
	CAboutBox(BOOL bModeless, CISDNConnection* pConnection, CWnd* pParent = NULL);    // standard constructor
	~CAboutBox();

	void DoUpdateConnectState(ConnectionState cs,
								  const CString sRemoteHostName,
								  const CString &sRemoteNumber,
								  BOOL bSupportsMSN
								  );

// Dialog Data
	//{{AFX_DATA(CAboutBox)
	enum { IDD = IDD_ABOUTBOX };
	CButton	m_ctlMSNSupport;
	CButton	m_cbHiddenQuit;
	CString	m_sModuleName;
	CString	m_sVersion;
	BOOL	m_bMSNSupport;
	CString	m_sConnectState;
	//}}AFX_DATA
	CWnd*				m_pWndParent;
	BOOL				m_bModeless;
	CISDNConnection*	m_pConnection;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CBigIcon m_icon; // self-draw button
	UINT m_nTimer;

	void ShowDlg();

	// Generated message map functions
	//{{AFX_MSG(CAboutBox)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnHiddenQuit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	ConnectionState m_connectionState;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
};

#endif	// ABOUTBOX_H
