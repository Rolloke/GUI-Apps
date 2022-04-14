#if !defined(AFX_CSTATISTICDLG_H__58F64C88_B3AE_11D4_8FAA_004005A11E32__INCLUDED_)
#define AFX_CSTATISTICDLG_H__58F64C88_B3AE_11D4_8FAA_004005A11E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CStatisticDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CStatisticRecord
/////////////////////////////////////////////////////////////////////////////
class CStatisticRecord
{
public:
	CString		sCamName;
	CSystemTime	StartTime;
	CSystemTime	Duration;
	__int64		i64Using;
};
/////////////////////////////////////////////////////////////////////////////
WK_PTR_ARRAY(CStatisticRecordArray, CStatisticRecord*);
/////////////////////////////////////////////////////////////////////////////
//***************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CStatisticDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CStatisticDlg : public CTransparentDialog
{
	// Construction
public:
	CStatisticDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatisticDlg();

// Dialog Data
	//{{AFX_DATA(CStatisticDlg)
	enum { IDD = IDD_STATISTIC_DLG };
	CSkinButton	m_ctrlOK;
	CDisplay	m_ctrlDisplay;
	CFrame		m_ctrlBorder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatisticDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL UpdateStatistics();
	void ClearStatistics();

	// Generated message map functions
	//{{AFX_MSG(CStatisticDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnStatisticOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageList	m_ilOK;
	HCURSOR		m_hCursor;
	CWnd*		m_pPanel;
	COLORREF	m_BaseColor;
	CStatisticRecordArray	m_Statistics;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSTATISTICDLG_H__58F64C88_B3AE_11D4_8FAA_004005A11E32__INCLUDED_)
