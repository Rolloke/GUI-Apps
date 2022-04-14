// USBCamUnit.h : main header file for the USBCAMUNIT application
//

#if !defined(AFX_USBCAMUNIT_H__FE51E321_E7BF_493A_BF56_648D74DE8E23__INCLUDED_)
#define AFX_USBCAMUNIT_H__FE51E321_E7BF_493A_BF56_648D74DE8E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUSBCamUnitApp:
// See USBCamUnit.cpp for the implementation of this class
//
class CUSBCamUnitDlg;

class CUSBCamUnitApp : public CWinApp
{
public:
	CUSBCamUnitApp();
	virtual ~CUSBCamUnitApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUSBCamUnitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	CUSBCamUnitDlg* GetAudioUnitDlg(){return m_pUSBCamUnitDlg;}
	WK_ApplicationId GetApplicationId() {return m_AppID;}
	int              GetInstanceCount() {return m_nInstance;};

	//{{AFX_MSG(CUSBCamUnitApp)
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CUSBCamUnitDlg     *m_pUSBCamUnitDlg;
	WK_ApplicationId  m_AppID;
	int					m_nInstance;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USBCAMUNIT_H__FE51E321_E7BF_493A_BF56_648D74DE8E23__INCLUDED_)
