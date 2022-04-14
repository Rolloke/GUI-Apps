#if !defined(AFX_QUERYFIELDTIMEDIALOG_H__4F824629_D0A4_11D2_B59E_004005A19028__INCLUDED_)
#define AFX_QUERYFIELDTIMEDIALOG_H__4F824629_D0A4_11D2_B59E_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryFieldTimeDialog.h : header file
//
#include "QueryFieldDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CQueryFieldTimeDialog dialog

class CQueryFieldTimeDialog : public CQueryFieldDialog
{
// Construction
public:
	CQueryFieldTimeDialog(const CString& sName, 
						  CIPCField* pField, 
					      CQueryDialog* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQueryFieldTimeDialog)
	enum { IDD = IDD_QUERY_FIELD_TIME };
	CDateTimeCtrl	m_ctrlTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryFieldTimeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryFieldTimeDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeTime(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYFIELDTIMEDIALOG_H__4F824629_D0A4_11D2_B59E_004005A19028__INCLUDED_)
