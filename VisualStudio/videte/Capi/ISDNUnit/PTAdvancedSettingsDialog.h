#if !defined(AFX_PTADVANCEDSETTINGSDIALOG_H__5087BAE2_3E85_11D1_B8C8_0060977A76F1__INCLUDED_)
#define AFX_PTADVANCEDSETTINGSDIALOG_H__5087BAE2_3E85_11D1_B8C8_0060977A76F1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PTAdvancedSettingsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPTAdvancedSettingsDialog dialog

class CPTAdvancedSettingsDialog : public CDialog
{
// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CPTAdvancedSettingsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPTAdvancedSettingsDialog)
	enum { IDD = IDD_ADVANCED_PT };
	BOOL	m_bDoHexDumpReceive;
	BOOL	m_bDoHexDumpSend;
	BOOL	m_bDisableEEPWrite;
	BOOL	m_bDisableFrameGrabbing;
	BOOL	m_bLowAsMid;
	BOOL	m_bDoTraceFrameSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTAdvancedSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CPTAdvancedSettingsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTADVANCEDSETTINGSDIALOG_H__5087BAE2_3E85_11D1_B8C8_0060977A76F1__INCLUDED_)
