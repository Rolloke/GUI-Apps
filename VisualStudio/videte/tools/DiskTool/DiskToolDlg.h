// DiskToolDlg.h : header file
//

#if !defined(AFX_DISKTOOLDLG_H__439F239A_D49B_42A4_9BB7_9DFC15147B0F__INCLUDED_)
#define AFX_DISKTOOLDLG_H__439F239A_D49B_42A4_9BB7_9DFC15147B0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDiskToolDlg dialog
class CWinAta;
class CDiskToolDlg : public CDialog
{
// Construction
public:
	BOOL UpdateDialog();
	CDiskToolDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDiskToolDlg();

// Dialog Data
	//{{AFX_DATA(CDiskToolDlg)
	enum { IDD = IDD_DISKTOOL_DIALOG };
	CButton	m_ctrlWriteCache;
	BOOL	m_bWriteCache;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiskToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDiskToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnWriteCache();
	afx_msg void OnSetUDMA0();
	afx_msg void OnSetUDMA1();
	afx_msg void OnSetUDMA2();
	afx_msg void OnSetUDMA3();
	afx_msg void OnSetUDMA4();
	afx_msg void OnSetUDMA5();
	afx_msg void OnSetUDMA6();
	afx_msg void OnRadioDrv0();
	afx_msg void OnRadioDrv1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWinAta*	m_pCWinAta;
	BOOL		m_bHasAta;
	int			m_nMaxUDMAMode;
	int			m_nUDMAMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISKTOOLDLG_H__439F239A_D49B_42A4_9BB7_9DFC15147B0F__INCLUDED_)
