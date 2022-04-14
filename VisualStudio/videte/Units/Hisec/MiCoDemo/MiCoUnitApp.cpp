// MiCoApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "mmsystem.h"
#include "MiCoDefs.h"
#include "MiCoUnitApp.h"
#include "MiCoUnitDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiCoUnitApp

BEGIN_MESSAGE_MAP(CMiCoUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CMiCoUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
/*@TOPIC{CMicoUnitApp Overview|Overview,CMiCoUnitApp}
@RELATED @LINK{members|CMiCoUnitApp},

Allgemeiner Überblick.....
*/
/*
@MLIST @RELATED @LINK{Overview|Overview,CMiCoUnitApp}
*/
/*@MHEAD{constructor:}*/
/*@MD CMiCoUnitApp construction */
CMiCoUnitApp::CMiCoUnitApp()
{
	m_pMainWnd  = NULL;
}


/*@MHEAD{destructor:}*/
/*@MD NYD*/
CMiCoUnitApp::~CMiCoUnitApp()
{
	m_pMainWnd  = NULL;
}

void CMiCoUnitApp::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << "CMiCoUnitApp = ";
}	  

/////////////////////////////////////////////////////////////////////////////
// The one and only CMiCoUnitApp object

CMiCoUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMiCoUnitApp initialization
/*@MHEAD{Init/Exit}*/ 
/*@MD 
@RELATED @MLINK{CMiCoUnitApp::ExitInstance}

NYD
*/
BOOL CMiCoUnitApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CMiCoUnitDlg* pDlg;

	pDlg = new CMiCoUnitDlg();

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

/*@MD 
@RELATED @MLINK{CMiCoUnitApp::InitInstance}

  @ARGS

	@CW{dwCmd}, the used cmd
	@CW{szText}, may be NULL
NYD
*/
int CMiCoUnitApp::ExitInstance()
{
	return m_msgCur.wParam; // returns the value from PostQuitMessage
}
