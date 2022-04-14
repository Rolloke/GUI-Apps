#if !defined(AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_)
#define AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FieldDialog.h : header file
//

class CImageWindow;
/////////////////////////////////////////////////////////////////////////////
// CFieldDialog dialog

class CFieldDialog : public CWK_Dialog
{
// Construction
public:
	CFieldDialog(CImageWindow* pImageWindow);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFieldDialog)
	enum { IDD = IDD_FIELD };
	CRichEditCtrl	m_RichEditData;
	CComboBox	m_comboCamera;
	CString	m_sCamera;
	CString	m_sResolution;
	CString	m_sDate;
	CString	m_sTime;
	CString	m_sCompression;
	//}}AFX_DATA

	// attributes
public:
	DWORD GetSelectedCamID() const;

	// operations
public:
	void CameraNames(int iNumFields,const CIPCField fields[]);
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnCameraNames();
	void SetTextEntry(const CString &sText, DWORD dwMask);
	void ClearRestOfEntries();

	// Generated message map functions
	//{{AFX_MSG(CFieldDialog)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCamera();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CImageWindow*	m_pImageWindow;
	CIPCFields		m_CameraFields;
	DWORD			m_dwSelectedCamera;
	long			m_lLastChar;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_)
