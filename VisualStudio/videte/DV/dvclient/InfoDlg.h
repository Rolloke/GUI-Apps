#if !defined(AFX_INFODLG_H__73916154_8739_4AE4_948F_64F40C4102FD__INCLUDED_)
#define AFX_INFODLG_H__73916154_8739_4AE4_948F_64F40C4102FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoDlg.h : header file
//

#include "resource.h"
class CDisplay;
class CSkinButton;
class CFrame;

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog

class CInfoDlg : public CTransparentDialog
{
// Construction
public:
	CInfoDlg(CString sInfo, CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();

// Dialog Data
	//{{AFX_DATA(CInfoDlg)
	enum { IDD = IDD_INFO_DLG };
	CDisplay	m_ctrlDisplayFeatureName;
	CSkinButton	m_ctrlOK;
	CFrame		m_ctrlBorder;
	CDisplay	m_ctrlDisplayFeature;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	void UpdateDisplayFeatures();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnInfoOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//members

	HCURSOR		m_hCursor;
	CWnd*		m_pPanel;
	CImageList	m_ilOK;
	COLORREF	m_BaseColor;
	CString		m_sInfoTxt;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFODLG_H__73916154_8739_4AE4_948F_64F40C4102FD__INCLUDED_)
