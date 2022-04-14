#if !defined(AFX_RELAYDIALOG_H__AE643842_1D39_11D1_93E0_00C095ECA33E__INCLUDED_)
#define AFX_RELAYDIALOG_H__AE643842_1D39_11D1_93E0_00C095ECA33E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RelayDialog.h : header file
//

class CRelayPage;
class COutput;

#include "TreeList.h"

/////////////////////////////////////////////////////////////////////////////
// CRelayDialog dialog

class CRelayDialog : public CWK_Dialog
{
// Construction
public:
	CRelayDialog(CRelayPage* pRelayPage, COutput* pOutput);

// Dialog Data
	//{{AFX_DATA(CRelayDialog)
	enum { IDD = IDD_RELAY };
	CStatic	m_StaticTypeRelay;
	CSpinButtonCtrl	m_SpinTypeRelay;
	CButton	m_btnClientKeep;
	CButton	m_btnClientBreak;
	CComboBox	m_cbEdge;
	CEdit	m_editClose;
	CEdit	m_editName;
	CString	m_sNr;
	CString	m_sName;
	CString	m_sClose;
	BOOL	m_bClientBreak;
	BOOL	m_bClientKeep;
	int		m_iEdge;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelayDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL StretchElements();

	// Implementation
protected:
	void SetOutput(COutput* pOutput);
	void OutputToControl();
	void ControlToOutput();
	void InitControls();
	void ShowHide();

	// Generated message map functions
	//{{AFX_MSG(CRelayDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckClientBreak();
	afx_msg void OnCheckClientKeep();
	afx_msg void OnSelchangeComboEdge();
	afx_msg void OnChangeEditCloseRelay();
	afx_msg void OnChangeKommentar();
	afx_msg void OnChangeEditTypeRelay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRelayPage* m_pRelayPage;
	COutput*		m_pOutput;
	int					m_iTypePos;

   bool        m_bSetPos;

   static int gm_nList[2];

	friend class CRelayPage;
};
WK_PTR_ARRAY(CRelayDialogArray,CRelayDialog*);

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RELAYDIALOG_H__AE643842_1D39_11D1_93E0_00C095ECA33E__INCLUDED_)
