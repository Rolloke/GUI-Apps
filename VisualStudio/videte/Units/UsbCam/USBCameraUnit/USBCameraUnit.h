// USBCameraUnit.h : main header file for the USBCAMERAUNIT application
//

#if !defined(AFX_USBCAMERAUNIT_H__9A57A8EA_B2EA_4E63_9F60_FC9B466CCC17__INCLUDED_)
#define AFX_USBCAMERAUNIT_H__9A57A8EA_B2EA_4E63_9F60_FC9B466CCC17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSBCameraUnitApp:
// See USBCameraUnit.cpp for the implementation of this class
//
class CUSBCameraUnitDlg;

class CUSBCameraUnitApp : public CWinApp
{
public:
	CUSBCameraUnitApp();
	virtual ~CUSBCameraUnitApp();

// Attributes
	DWORD GetMainTreadID()   {return m_dwThreadID;};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBCameraUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	CUSBCameraUnitDlg* GetUSBCameraUnitDlg(){return m_pDlg;}
	WK_ApplicationId GetApplicationId() {return m_AppID;}
	int              GetInstanceCount() {return m_nInstance;};

	//{{AFX_MSG(CUSBCameraUnitApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	WK_ApplicationId  m_AppID;
	int					m_nInstance;
	CUSBCameraUnitDlg*m_pDlg;
	DWORD             m_dwThreadID;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBCAMERAUNIT_H__9A57A8EA_B2EA_4E63_9F60_FC9B466CCC17__INCLUDED_)
