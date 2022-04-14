#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H


class COutputPage;
class COutput;

#include "TreeList.h"

/////////////////////////////////////////////////////////////////////////////
// COutputDialog dialog

class COutputDialog : public CWK_Dialog
{
// Construction
public:
	COutputDialog(COutputPage* pOutputPage, COutput* pOutput);

// Dialog Data
	//{{AFX_DATA(COutputDialog)
	enum { IDD = IDD_OUTPUT };
	CComboBox	m_cbAtDisconnect;
	CStatic	m_StaticTypeCamera;
	CSpinButtonCtrl	m_SpinTypeCamera;
	CButton	m_checkReference;
	CButton	m_checkTermination;
	CEdit	m_editName;
	CButton	m_checkSN;
	CEdit	m_editID;
	CComboBox	m_cbManu;
	CComboBox	m_cbCom;
	CString	m_sNr;
	CString	m_sName;
	BOOL	m_bSN;
	CString	m_sCom;
	BOOL	m_bReference;
	BOOL	m_bTermination;
	DWORD	m_dwSNID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputDialog)
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
	void	SetOutput(COutput* pOutput);
	void	OutputToControl();
	void	ControlToOutput();
	void	InitSN();
	void	InitCOM();
	void	ShowHide();
	void	InitCmdAtDisconnect();

	// Generated message map functions
	//{{AFX_MSG(COutputDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSn();
	afx_msg void OnSelchangeCom();
	afx_msg void OnChangeEditCloseRelay();
	afx_msg void OnChangeEditId();
	afx_msg void OnChangeKommentar();
	afx_msg void OnSelchangeManufacturer();
	afx_msg void OnCheckReference();
	afx_msg void OnCheckTermination();
	afx_msg void OnChangeEditTypeCamera();
	afx_msg void OnSelchangeSnAtDisconnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COutputPage*		m_pOutputPage;
	COutput*			m_pOutput;
	int					m_iTypePos;
	CameraControlType	m_eCameraType;
	CameraControlCmd	m_eCameraCmd;

	static int gm_nList[3];

	friend class COutputPage;
public:
	afx_msg void OnBnClickedBtnCamComParams();
	afx_msg void OnBnClickedBtnCamTypeSettings();
	afx_msg void OnEnSetfocusNameCamera();
};
WK_PTR_ARRAY(COutputDialogArray,COutputDialog*);
#endif
