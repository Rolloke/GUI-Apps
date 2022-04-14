/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: LogDlg.h $
// ARCHIVE:		$Archive: /Project/Security 3.xx/LogDlg.h $
// CHECKIN:		$Date: 30.05.96 13:06 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 30.05.96 12:36 $
// BY AUTHOR:	$Author: Georg $
// $Nokeywords:$

#ifndef LOGDLG_H
#define LOGDLG_H

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CLogDlg dialog

class CLogDlg : public CDialog
{
// Construction
public:
	CLogDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogDlg)
	enum { IDD = IDD_LOG };
	CString	m_csLogID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // LOGDLG_H
