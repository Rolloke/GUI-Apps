/////////////////////////////////////////////////////////////////////////////
// PROJECT:		TashaUnit
// FILE:		$Workfile: TashaUnit.h $
// ARCHIVE:		$Archive: /Project/Units/Tasha/TashaUnit/TashaUnit.h $
// CHECKIN:		$Date: 2.09.04 9:38 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 2.09.04 9:22 $
// BY AUTHOR:	$Author: Martin.lueck $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASHAUNIT_H__F917201D_97C1_4133_A630_82F9D7134639__INCLUDED_)
#define AFX_TASHAUNIT_H__F917201D_97C1_4133_A630_82F9D7134639__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTashaUnitApp:
// See TashaUnit.cpp for the implementation of this class
//
class CDebug;
class CTashaUnitDlg;
class CInstanceCounter;
class CTashaException;
class CTashaUnitApp : public CWinApp
{
public:
	CTashaUnitApp();
	WK_ApplicationId GetApplicationId();
	int GetCardRef();
	BOOL CleanUp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTashaUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTashaUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CTashaUnitDlg*		m_pTashaUnitDlg;	
	CInstanceCounter*	m_pInstance;
	CTashaException*	m_pCTashaException;
	CDebug*				m_pDebug;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHAUNIT_H__F917201D_97C1_4133_A630_82F9D7134639__INCLUDED_)
