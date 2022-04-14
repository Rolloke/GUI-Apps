#include "afxwin.h"
#if !defined(AFX_MEMORYMANEGMENTDLG_H__3F68612C_6745_4428_ABFA_D55E4F7AA801__INCLUDED_)
#define AFX_MEMORYMANEGMENTDLG_H__3F68612C_6745_4428_ABFA_D55E4F7AA801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MemoryManegmentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMemoryManegmentDlg dialog

class CMemoryManegmentDlg : public CPropertyPage
{
// Construction
public:
	CMemoryManegmentDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMemoryManegmentDlg)
	enum { IDD = IDD_MEMORY_MANAGEMENT };
	BOOL	m_bDisableKernelPaging;
	BOOL	m_bUnloadDllAlways;
	int		m_n2ndLeveCacheSize;
	BOOL	m_bSystemCacheSettings;
	int		m_nFileCacheSize;
	int		m_nLanManCacheSize;
	CComboBox m_ctrlMem;
	//}}AFX_DATA
	bool m_bChanged;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoryManegmentDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMemoryManegmentDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnRestore();
	afx_msg void OnBtnSave();
	afx_msg void OnChange();
	afx_msg LRESULT OnHelp(WPARAM, LPARAM);
	afx_msg void OnBnClickedBtnOptimizeFileCacheSize();
	afx_msg void OnCbnSelchangeComboMem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMORYMANEGMENTDLG_H__3F68612C_6745_4428_ABFA_D55E4F7AA801__INCLUDED_)
