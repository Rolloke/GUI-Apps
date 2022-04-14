/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: cocounitapp.h $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/cocounitapp.h $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 3 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitApp:
// See CoCoApp.cpp for the implementation of this class
//

class CCoCoUnitApp : public CWinApp
{
public:
	CCoCoUnitApp();
	~CCoCoUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCoCoUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();
	virtual void Dump( CDumpContext &dc ) const;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCoCoUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
//	CWinThread* m_pThread;
};


/////////////////////////////////////////////////////////////////////////////
