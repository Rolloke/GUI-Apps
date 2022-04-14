/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: ComDlg.h $
// ARCHIVE:		$Archive: /Project/Tools/TstTools/SDITest/ComDlg.h $
// CHECKIN:		$Date: 27.04.98 8:18 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 27.04.98 8:10 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#if !defined(AFX_COMDLG_H__F063C725_A37F_11D1_9F29_0000C036AC0D__INCLUDED_)
#define AFX_COMDLG_H__F063C725_A37F_11D1_9F29_0000C036AC0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CComDlg dialog
class CComDlg : public CDialog
{
// Construction
public:
	CComDlg(int iCom, CWnd* pParent = NULL);   // standard constructor

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CComDlg)
	afx_msg void OnRadioComChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dialog Data
public:
	//{{AFX_DATA(CComDlg)
	enum { IDD = IDD_CONFIG_COM };
	CButton	m_btnCom1;
	int		m_iCom;
	//}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMDLG_H__F063C725_A37F_11D1_9F29_0000C036AC0D__INCLUDED_)
