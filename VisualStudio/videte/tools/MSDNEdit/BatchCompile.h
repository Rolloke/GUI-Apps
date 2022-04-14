#if !defined(AFX_BATCHCOMPILE_H__D893DFB0_8697_4B44_8115_15AD23494662__INCLUDED_)
#define AFX_BATCHCOMPILE_H__D893DFB0_8697_4B44_8115_15AD23494662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchCompile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchCompile dialog

class CBatchCompile : public CDialog
{
// Construction
public:
	CBatchCompile(CWnd* pParent = NULL);   // standard constructor
   virtual ~CBatchCompile();
// Dialog Data
	//{{AFX_DATA(CBatchCompile)
	enum { IDD = IDD_COMPILE_BATCH };
	CListCtrl	m_cCompileList;
	CString	m_strPath;
	CString	m_strCommands;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchCompile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchCompile)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkCompileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBuildAll();
	virtual void OnOK();
	afx_msg void OnSelectCompiler();
	virtual void OnCancel();
	afx_msg void OnDeleteFile();
	afx_msg void OnClickCompileList(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg BOOL OnToolTipNotify(UINT, NMHDR *, LRESULT *);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void EnableDlgItems(bool);
   static unsigned __stdcall RenderThread(void *);
   HANDLE m_hRenderThread;
   volatile bool m_bRun;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHCOMPILE_H__D893DFB0_8697_4B44_8115_15AD23494662__INCLUDED_)
