// MiCoApp.h : main header file for the MICOAPP application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMiCoUnitApp:
// See MiCoApp.cpp for the implementation of this class
//

class CMiCoUnitApp : public CWinApp
{
public:
	CMiCoUnitApp();
	~CMiCoUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiCoUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();
	virtual void Dump( CDumpContext &dc ) const;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMiCoUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	private:
//	CWinThread* m_pThread;
};


/////////////////////////////////////////////////////////////////////////////
