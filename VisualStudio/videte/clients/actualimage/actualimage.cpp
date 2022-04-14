// actualimage.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "actualimage.h"
#include "ActualOutputImage.h"
#include "process.h"
#include "conio.h"
#include "wk_util.h"

UINT MyControllingFunction( LPVOID pParam );


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "wk_names.h"
#include "wk_trace.h"

/////////////////////////////////////////////////////////////////////////////
// CActualimageApp

BEGIN_MESSAGE_MAP(CActualimageApp, CWinApp)
	//{{AFX_MSG_MAP(CActualimageApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActualimageApp construction

CActualimageApp::CActualimageApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CActualimageApp object

CActualimageApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CActualimageApp initialization

BOOL CActualimageApp::InitInstance()
{
	if ( WK_ALONE(WK_APP_NAME_ACTUAL_IMAGE)==FALSE )
	{
		return FALSE;
	}


	InitDebugger("actualimage.log",WAI_INVALID);


	// split commandline string into permission and password, if possible
	CString sCommandline = m_lpCmdLine;
	int first = sCommandline.Find(' ');
	if(first == -1)			//no space found, that means , only permission
	{
		m_sPermission = m_lpCmdLine;
	}
	else					// space found, that means permission and password
	{
		m_sPermission = sCommandline.Left(first);
		m_sPassword = sCommandline.Right(sCommandline.GetLength()-(first+1));
	}

//	WK_TRACE("permission: %s  password: %s\n", m_sPermission, m_sPassword);

	int iNumTries = 5;			//5 possible turns to build up a server connection
	while (iNumTries>0)
	{
		if (RequestAndSaveImages())
		{
			break;
		}
		Sleep(1000);
		iNumTries--;
	}


	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CActualimageApp::RequestAndSaveImages()
{
	CConnectionRecord c;
	c.SetDestinationHost(LOCAL_LOOP_BACK);				//request Server Connection
	c.SetPermission(m_sPermission);
	c.SetPassword(m_sPassword);
	CIPCFetchResult fr = NewFetchOutputConnection(c);
	
	if (fr.GetError() == CIPC_ERROR_OKAY)
	{
		WK_TRACE("connect ok with %s\n",fr.GetShmName());//request shared memory area
		CActualOutputImage* pActualOutputImage = new CActualOutputImage(fr.GetShmName());

		WK_TRACE("waiting for images\n");
		
		int iSleep = 100;
		int iTimeOut = 65*IMAGETIMER/iSleep;					            //max time to get all images
		BOOL bOK = FALSE;

		while (iTimeOut>0)
		{
			if (pActualOutputImage->IsReady())
			{
				bOK = TRUE;
				break;
			}
			Sleep(iSleep);
			iTimeOut--;
		}
		if (bOK)
		{
			WK_TRACE("have all images\n");
		}
		else 
		{
			WK_TRACE_ERROR("don't have all images\n");
		}
		WK_DELETE(pActualOutputImage);

		return bOK;
	}
	else
	{
		WK_TRACE_ERROR("Connection not ok:%s",fr.GetErrorMessage());
		return FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////
int CActualimageApp::ExitInstance() 
{
	CloseDebugger();
	
	return 0;
}
