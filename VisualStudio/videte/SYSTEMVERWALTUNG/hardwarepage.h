/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: hardwarepage.h $
// ARCHIVE:		$Archive: /Project/SystemVerwaltung/hardwarepage.h $
// CHECKIN:		$Date: 19.08.97 11:18 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 19.08.97 11:15 $
// BY AUTHOR:	$Author: Uwe $
// $Nokeywords:$

#ifndef HardwarePage_H
#define HardwarePage_H

#include "SVPage.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CHardwarePage dialog
class CHardwarePage : public CSVPage
{
	DECLARE_DYNAMIC(CHardwarePage)

// Construction
public:
	CHardwarePage(CSVView* pParent);
	~CHardwarePage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHardwarePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Overrides
protected:
	virtual BOOL IsDataValid();
	virtual void SaveChanges();
	virtual void CancelChanges();

	// Implementation

	// Generated message map functions
	//{{AFX_MSG(CHardwarePage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_DATA(CHardwarePage)
	enum { IDD = IDD_HARDWARE };
	//}}AFX_DATA

};
#endif // HardwarePage_H

/////////////////////////////////////////////////////////////////////////////
// end of $Workfile: hardwarepage.h $
