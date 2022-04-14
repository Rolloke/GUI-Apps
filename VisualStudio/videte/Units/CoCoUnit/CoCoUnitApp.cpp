/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: CoCoUnitApp.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/CoCoUnitApp.cpp $
// CHECKIN:		$Date: 14.11.02 13:33 $
// VERSION:		$Revision: 30 $
// LAST CHANGE:	$Modtime: 14.11.02 13:31 $
// BY AUTHOR:	$Author: Georg.feddern $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wk.h"
#include "wk_Names.h"
#include "wk_util.h"
#include "cpydata.h"
#include "ccoco.h"
#include "CoCoUnitApp.h"
#include "CoCoUnitDlg.h"
#include "cipc.h"
#include "WK_RuntimeError.h"
#include "WK_Dongle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitApp

BEGIN_MESSAGE_MAP(CCoCoUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CCoCoUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitApp construction
CCoCoUnitApp::CCoCoUnitApp()
{
	m_pMainWnd  = NULL;
}


CCoCoUnitApp::~CCoCoUnitApp()
{
	m_pMainWnd  = NULL;
}

void CCoCoUnitApp::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << "CCoCoUnitApp = ";
}	  

/////////////////////////////////////////////////////////////////////////////
// The one and only CCoCoUnitApp object

CCoCoUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitApp initialization
BOOL CCoCoUnitApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Unit nur einmal starten
	if (WK_ALONE(WK_APP_NAME_COCOUNIT)==FALSE)
		return FALSE;
	
//	WK_STAT_TICKCOUNT = GetTickCount();

	InitDebugger("CoCoUnit.log", WAI_COCOUNIT);

	WK_PRINT_VERSION(18, 8, 16, 37, "18/8/97,16:37\0", // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	// Dongle vorhanden
	CWK_Dongle dongle(WAI_COCOUNIT);
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}
	
	WK_STAT_LOG("Reset", 1, "ServerActive");

//	Enable3dControls();
	CCoCoUnitDlg* pDlg;

	pDlg = new CCoCoUnitDlg();

	if (pDlg && pDlg->Create())
	{
		m_pMainWnd = pDlg;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoUnitApp::ExitInstance
int	CCoCoUnitApp::ExitInstance()
{
	WK_STAT_LOG("Reset", 0, "ServerActive");
	CloseDebugger();
	return CWinApp::ExitInstance();
}

