#if !defined(AFX_CONVERSIONDIALOG_H__20EA8297_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
#define AFX_CONVERSIONDIALOG_H__20EA8297_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConversionDialog.h : header file
//
#include "ConvertThread.h"
/////////////////////////////////////////////////////////////////////////////
// CConversionDialog dialog

class CConversionDialog : public CDialog
{
// Construction
public:
	CConversionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConversionDialog)
	enum { IDD = IDD_CONVERSION };
	CButton	m_OK;
	CProgressCtrl	m_Progress;
	CString	m_sCurrentFile;
	CString	m_sTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConversionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Convert();

	// Generated message map functions
	//{{AFX_MSG(CConversionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	DWORD			m_dwCount;
	DWORD 			m_dwStart;
	CConvertThread  m_ConvertThread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVERSIONDIALOG_H__20EA8297_73B2_11D2_B4E3_00C095EC9EFA__INCLUDED_)
