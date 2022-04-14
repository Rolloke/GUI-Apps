#if !defined(AFX_SIMPAGE_H__EB347BA2_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_SIMPAGE_H__EB347BA2_17C0_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SimPage.h : header file
//

#include "SVPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
class CInputList;
class COutputList;
/////////////////////////////////////////////////////////////////////////////
// CSimPage dialog

class CSimPage : public CSVPage
{
// Construction
public:
	CSimPage(CSVView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimPage)
	enum { IDD = IDD_SIM };
	BOOL	m_bSimUnit;
	BOOL	m_bSwitchPanel;
	int m_nButtonWidth;
	int m_nButtonHeight;
	CComboBox m_comboMinimize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();
	//}}AFX_VIRTUAL

// Overrides
protected:

	// Implementation
protected:
	void GetDlgSimUnitData();

	// Generated message map functions
	//{{AFX_MSG(CSimPage)
	afx_msg void OnCheckSimunit();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCkEnableSwitchPanel();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CInputList*				m_pInputList;
	COutputList*			m_pOutputList;
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPAGE_H__EB347BA2_17C0_11D1_93E0_00C095ECA33E__INCLUDED_)
