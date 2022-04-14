#if !defined(AFX_MULTIMONITORPAGE_H__A5B6BC44_61D7_4983_8F8B_FEDE0CC17CDC__INCLUDED_)
#define AFX_MULTIMONITORPAGE_H__A5B6BC44_61D7_4983_8F8B_FEDE0CC17CDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// MultiMonitorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
#include "SVPage.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CMultiMonitorPage dialog

class CMultiMonitorPage : public CSVPage
{
// Construction
public:
	CMultiMonitorPage(CSVView* pParent);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiMonitorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMultiMonitorPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChanged();
	afx_msg void OnChangedSV();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int FlagsToRadioControl(DWORD dwFlags);
	DWORD RadioControlToFlags(int iRadioControl);

// Dialog Data
	//{{AFX_DATA(CMultiMonitorPage)
	enum { IDD = IDD_MULTI_MONITOR_PAGE };
	CButton m_radioVision_1;
	CButton m_radioVision_2;
	CButton m_radioVision_1_2;
	CButton m_radioRecherche_1;
	CButton m_radioRecherche_2;
	CButton m_radioSupervisor_1;
	CButton m_radioSupervisor_2;
	CButton	m_grpVision;
	CButton	m_grpSupervisor;
	CButton	m_grpRecherche;
	int	m_iVision;
	int	m_iRecherche;
	int	m_iSystVerw;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIMONITORPAGE_H__A5B6BC44_61D7_4983_8F8B_FEDE0CC17CDC__INCLUDED_)
