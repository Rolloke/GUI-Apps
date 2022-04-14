/////////////////////////////////////////////////////////////////////////////
// PROJECT:		StressIt
// FILE:		$Workfile: StressIt.h $
// ARCHIVE:		$Archive: /Project/Tools/StressIt/StressIt.h $
// CHECKIN:		$Date: 5.08.98 9:56 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:56 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRESSIT_H__AA88D715_ADBF_11D1_9C64_00A024D29804__INCLUDED_)
#define AFX_STRESSIT_H__AA88D715_ADBF_11D1_9C64_00A024D29804__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStressItApp:
// See StressIt.cpp for the implementation of this class
//

class CStressItApp : public CWinApp
{
public:
	CStressItApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStressItApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStressItApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRESSIT_H__AA88D715_ADBF_11D1_9C64_00A024D29804__INCLUDED_)
