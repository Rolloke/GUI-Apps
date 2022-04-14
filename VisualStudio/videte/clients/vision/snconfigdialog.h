#if !defined(AFX_SNCONFIGDIALOG_H__F4DDC421_AC5F_11D1_8C1B_00C095EC9EFA__INCLUDED_)
#define AFX_SNCONFIGDIALOG_H__F4DDC421_AC5F_11D1_8C1B_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SNConfigDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSNConfigDialog dialog

class CSNConfigDialog : public CDialog
{
// Construction
public:
	CSNConfigDialog(CIPCInputVision* pInput,CIPCOutputVision* pOutput,
					CameraControlType ccType, DWORD dwPTZFunctions,
					CWnd* pParent = NULL);   // standard constructor
	~CSNConfigDialog();

// Dialog Data
	//{{AFX_DATA(CSNConfigDialog)
	enum { IDD = IDD_SN_CONFIG };
	CButton	m_btnUp;
	CButton	m_btnRight;
	CButton	m_btnLeft;
	CButton	m_btnDown;
	CButton	m_btnCenter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSNConfigDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSNConfigDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCenter();
	afx_msg void OnDown();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnUp();
	afx_msg void OnProg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDisplayWindow* m_pDisplayWindow;
	CIPCInputVision* m_pInput; 
	CIPCOutputVision* m_pOutput; 

	CSecID m_commID;
	CSecID m_camID;

	CameraControlType m_ccType;
	DWORD  m_dwPTZFunctions;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNCONFIGDIALOG_H__F4DDC421_AC5F_11D1_8C1B_00C095EC9EFA__INCLUDED_)
