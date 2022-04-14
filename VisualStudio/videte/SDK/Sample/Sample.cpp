/******************************************************************************
|* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
|* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
|* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
|* PARTICULAR PURPOSE.
|* 
|* Copyright 2005 videte IT ® AG. All Rights Reserved.
|*-----------------------------------------------------------------------------
|* videteSDK / Sample
|*
|* PROGRAM: Sample.cpp
|*
|* PURPOSE: Main file of the application; implementation file.
******************************************************************************/


#include "stdafx.h"
#include "Sample.h"
#include "SampleDlg.h"
#include ".\sample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSampleApp::CSampleApp()
{
}

//////////////////////////////////////////////////////////////////////
// message map
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSampleApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// The one and only CSampleApp object
CSampleApp theApp;


/*********************************************************************************************
Description   : Initialization of the application.

Parameters:   none

Return Value:  TRUE if initialization was ok,
			   FALSE if initialization was not ok. (BOOL)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
BOOL CSampleApp::InitInstance()
{
	CWinApp::InitInstance();

	//initialize the debugger. In folder C:\log\ a the file "SampleApp.log" receives all
	//traces when using "WK_TRACE(_T("hello world\n"))
	//this log-file also includes all traces from the enclosed dll's
	InitDebugger(_T("SampleApp.log"),WAI_DATABASE_CLIENT);
	WK_TRACE(_T("###### sample started ######\n"));

	CSampleDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/*********************************************************************************************
Description   : Exits the application.

Parameters:   none

Return Value:  (int)

Author: Tomas Krogh

created: July, 14 2005
*********************************************************************************************/
int CSampleApp::ExitInstance()
{
	//close the debugger
	CloseDebugger();
	CLivePicture::ReleaseMMTimer();
	return CWinApp::ExitInstance();
}
