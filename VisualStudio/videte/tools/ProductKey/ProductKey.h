// ProductKey.h : main header file for the PRODUCTKEY application
//

#if !defined(AFX_PRODUCTKEY_H__95A55B85_0A83_11D2_B533_00C095EC9EFA__INCLUDED_)
#define AFX_PRODUCTKEY_H__95A55B85_0A83_11D2_B533_00C095EC9EFA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProductKeyApp:
// See ProductKey.cpp for the implementation of this class
//

class CProductKeyApp : public CWinApp
{
public:
	CProductKeyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProductKeyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProductKeyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRODUCTKEY_H__95A55B85_0A83_11D2_B533_00C095EC9EFA__INCLUDED_)
