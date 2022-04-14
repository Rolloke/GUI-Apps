// acdc.h : main header file for the ACDC application
//

#if !defined(AFX_ACDC_H__C4621320_95EE_401D_A310_5381174E0E2C__INCLUDED_)
#define AFX_ACDC_H__C4621320_95EE_401D_A310_5381174E0E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "parameters.h"
#include "AcdcDlg.h"
#include "AcdcException.h"



// These are error strings that correspond to the error codes above.

static LPCTSTR s_cle[CLE_MAX] = {
	_T("Everything fine."),
	_T("Multiple commands were specified from the following set: listdrives, cdinfo, read, write, erase, eject, load. Only one command is allowed at one time."),
	_T("Missing drive name parameter after --drivename."),
	_T("Missing artist parameter after --artist."),
	_T("Missing title parameter after --title."),
	_T("Missing speed parameter after --speed."),
	_T("Supplied speed parameter is invalid."),
	_T("Missing volume name parameter after --iso."),
	_T("Invalid -- parameter."),
	_T("Multiple burn types were specified from the following set: iso/audio, videocd, svideocd, image. Only one burn type is allowed at one time."),
	_T("Missing file name after --XX parameter."),
	_T("Maximum number of 99 tracks has been reached."),
	_T("Parameter file was already processed. It cannot be done recursively!"),
	_T("Missing image filename parameter after --image."),
	_T("Missing error log filename parameter after --error_log."),
	_T("Missing import session parameter after --import."),
	_T("Supplied import session parameter is invalid."),
	_T("Iso/Audio and (S)VideoCD need at least one track!"),
	_T("Burn type was not specified! Use a proper combination of --audio, --videocd, --svideocd and --iso."),
};


class CIPCServerControlACDC;
/////////////////////////////////////////////////////////////////////////////
// CAcdcApp:
// See acdc.cpp for the implementation of this class
//
class CAcdcApp : public CWinApp
{
public:
	CAcdcApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcdcApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAcdcApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	afx_msg void OnAppExit();
	DECLARE_MESSAGE_MAP()

 //Functions
public: 

	CAcdcDlg*				GetAcdcDlg();
	CIPCServerControlACDC*	GetServerControl();

 //members
public:

	CIPCDrives m_Drives;
	CIPCDrive* m_pDrive;
//members
private:

	CAcdcDlg* m_pDlg;
	CIPCServerControlACDC*	m_pServerControl;
	CAcdcException*			m_pAcdcException;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
extern CAcdcApp theApp;
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACDC_H__C4621320_95EE_401D_A310_5381174E0E2C__INCLUDED_)
