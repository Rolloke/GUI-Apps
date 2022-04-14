#if !defined(AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_)
#define AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FieldDialog.h : header file
//

class CWndImageRecherche;
/////////////////////////////////////////////////////////////////////////////
// CDlgField dialog
#define DATA_CONTROL_AS_LINE 1

class CDlgField : public CSkinDialog
{
// Construction
public:
	CDlgField(CWndImageRecherche* pWndImage);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgField)
	enum  { IDD = IDD_FIELD };
	CSkinComboBox	m_comboCamera;
	CString	m_sCamera;
	CString	m_sResolution;
	CString	m_sDate;
	CString	m_sTime;
	CString	m_sCompression;
	//}}AFX_DATA
#ifndef	DATA_CONTROL_AS_LINE
	CRichEditCtrl	m_RichEditData;
	CSkinStatic		m_HtmlData;
#endif
	// attributes
public:
	DWORD GetSelectedCamID() const;

	// operations
public:
	void CameraNames(int iNumFields,const CIPCField fields[]);
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgField)
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
#ifdef	DATA_CONTROL_AS_LINE
	void InitDataLineControls();
	BOOL CalculateHeight(CWnd*pWnd, const CString &sText, int nHeight, CRect& rc);
#else
	void ClearRestOfEntries();
#endif
	// Generated message map functions
	//{{AFX_MSG(CDlgField)
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCamera();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWndImageRecherche*	m_pWndImage;
	CIPCFields		m_CameraFields;
	DWORD			m_dwSelectedCamera;
#ifndef	DATA_CONTROL_AS_LINE
	long			m_lLastChar;
	CString			m_sStaticText;
#else
	CStringArray	m_DataLines;
	DWORD			m_dwBoldLines;
	DWORD			m_dwOldBoldLines;
	LOGFONT			m_Bold;
	CFont			m_fBold;
	CFont			m_fLight;
	int				m_nMaxCharsWithoutSpace;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIELDDIALOG_H__C8E76325_2524_11D2_B55F_00C095EC9EFA__INCLUDED_)
