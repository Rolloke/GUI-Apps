// TashaUnit.h : main header file for the TASHAUNIT application
//

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
class CInstanceCounter;
class CTashaUnitApp : public CWinApp
{
public:
	CTashaUnitApp();
	int GetCardRef(){return 1; /*m_pInstance->GetInstance();*/}

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
	CInstanceCounter	*m_pInstance;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASHAUNIT_H__F917201D_97C1_4133_A630_82F9D7134639__INCLUDED_)
