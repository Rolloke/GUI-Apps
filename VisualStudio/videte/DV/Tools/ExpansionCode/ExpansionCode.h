// ExpansionCode.h : main header file for the EXPANSIONCODE application
//

#if !defined(AFX_EXPANSIONCODE_H__967E1852_4648_4CDD_BA80_9D2F7D2BEB58__INCLUDED_)
#define AFX_EXPANSIONCODE_H__967E1852_4648_4CDD_BA80_9D2F7D2BEB58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


/////////////////////////////////////////////////////////////////////////////
// CExpansionCodeApp:
// See ExpansionCode.cpp for the implementation of this class
//
//DTS transmitter expansions
#define FLAG_ISDN		0x00000001
#define FLAG_LAN		0x00000002
#define FLAG_BACKUP		0x00000004
#define FLAG_DVD		0x00000008
#define FLAG_Q_CAM_8	0x00000010
#define FLAG_Q_CAM_16	0x00000020


//idip receiver expansions
#define FLAG_TIMER		0x00000010
#define FLAG_PTZ		0x00000020
#define FLAG_MONITOR	0x00000040
#define FLAG_MAPS		0x00000080
#define FLAG_HOST0		0x00000400
#define FLAG_HOST5		0x00000100
#define FLAG_HOST10		0x00000200
#define FLAG_HOST20		0x00000800
#define FLAG_HOST30		0x00001000
#define FLAG_HOST40		0x00002000
#define FLAG_HOST50		0x00004000
#define FLAG_HOST60		0x00008000
#define FLAG_RESET		0x00010000

//idip transmitter dongle
#define DONGLE_FLAG_BACKUP			0x00000001
#define DONGLE_FLAG_Q				0x00000002
#define DONGLE_FLAG_IP				0x00000004
#define DONGLE_FLAG_Q_CAM_4			0x00000008
#define DONGLE_FLAG_Q_CAM_8			0x00000010
#define DONGLE_FLAG_Q_CAM_16		0x00000020
#define DONGLE_FLAG_Q_CAM_24		0x00000040
#define DONGLE_FLAG_Q_CAM_32		0x00000080
#define DONGLE_FLAG_ISDN			0x00000100
#define DONGLE_FLAG_PTZ				0x00000200
#define DONGLE_FLAG_SERIAL_ALARM	0x00000400
#define DONGLE_FLAG_GAA				0x00000800
#define DONGLE_FLAG_ACCESS			0x00001000
#define DONGLE_FLAG_SCANNER			0x00002000
#define DONGLE_FLAG_PARK			0x00004000
#define DONGLE_FLAG_ICPCON			0x00008000
#define DONGLE_FLAG_HTTP			0x00010000
#define DONGLE_FLAG_IP_CAM_4		0x00020000
#define DONGLE_FLAG_IP_CAM_8		0x00040000
#define DONGLE_FLAG_IP_CAM_16		0x00080000
#define DONGLE_FLAG_IP_CAM_24		0x00100000
#define DONGLE_FLAG_IP_CAM_32		0x00200000


class CExpansionCodeApp : public CWinApp
{
public:
	CExpansionCodeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpansionCodeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CExpansionCodeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPANSIONCODE_H__967E1852_4648_4CDD_BA80_9D2F7D2BEB58__INCLUDED_)
