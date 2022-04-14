// IPBookDlg.h : header file
//

#if !defined(AFX_IPBOOKDLG_H__B5B72939_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
#define AFX_IPBOOKDLG_H__B5B72939_CC12_11D4_86C3_004005A26A3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include "LocalIPGang.h"
/////////////////////////////////////////////////////////////////////////////
// CIPBookDlg dialog

class CLocalIPGang;
class CIPBookDlg : public CDialog
{
// Construction
public:
	void SetBandwidth(CString sIP, int iThroughput);
	void SetLocalIP(CString sIP);
	void RemoveAddress(CString sIP,CString sHost);
	void AddAddress(CString sIP,CString sHost);
	CIPBookDlg(CWnd* pParent = NULL);	// standard constructor
	~CIPBookDlg();
// Dialog Data
	//{{AFX_DATA(CIPBookDlg)
	enum { IDD = IDD_IPBOOK_DIALOG };
	CStatic	m_CtrlGraph;
	CListCtrl	m_CtrlIPList;
	CString	m_sIPList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPBookDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnListening(CWinSocket* pSocket, UINT uPort);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CIPBookDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickIplist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CDC* m_pdcGraph;
//	CFile m_fileLog;

	CLocalIPGang *m_pSocket;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPBOOKDLG_H__B5B72939_CC12_11D4_86C3_004005A26A3B__INCLUDED_)
