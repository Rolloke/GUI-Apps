/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: GenericUnitApp.h $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/GenericUnitApp.h $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 19.10.00 8:20 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "CInstanceCounter.h"

/////////////////////////////////////////////////////////////////////////////
// CGenericApp:
//
class CGenericApp : public CWinApp
{
public:
	WK_ApplicationId GetApplicationId();
	CGenericApp();
	~CGenericApp();

	BOOL CleanUp();

	CGenericUnitDlg* GetGenericUnitDlg(){return m_pGenericUnitDlg;}
	int GetCardRef(){return m_pInstance->GetInstance();}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenericApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CGenericApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CGenericUnitDlg		*m_pGenericUnitDlg;	
	CInstanceCounter	*m_pInstance;
};


/////////////////////////////////////////////////////////////////////////////
