///////////////////////////////////////////////////////////////
// FILE   : $Workfile: Tester.cpp $ 
// ARCHIVE: $Archive: /Project/Tools/ISDN-Tester/Tester.cpp $
// DATE   :	$Date: 9.06.97 12:31 $ 
// VERSION: $Revision: 18 $
// LAST   : $Modtime: 23.05.97 10:07 $ 
// AUTHOR : $Author: Hajo $
//        : $Nokeywords:$

/**modulref ***************************************************
 * name    : Tester.cpp
 * class   : CTesterApp
 * derived : CWinApp
 * members :
 * public  : CTesterApp();
 * access  : inline BOOL	DoVerify() const;
 *         : inline CString	GetTestFileName() const;
 *         : inline int		GetDestination() const;
 *         : inline int		GetSizeofpData() const;
 *         : inline int		GetLoop() const;
 *         : inline int		GetDoDelete() const;
 * private :		int		DoTest(const CString&);
 *         :		void	SplitArgs(const CString&);
 *         :		CString	ParseOptions(CStringArray&);
 *         :		CString	FindHostNumberByName(
 *         :					const CString&)	const;
 * data    : CString		  m_szHost;			
 *         : CString		  m_szUserDummy;
 *         : CString		  m_sTestFileName;	
 *         : int			  m_iDestination;	
 *         : int			  m_iSizeofpData;	
 *         : int			  m_iLoop;			
 *         : CIPCInputTester* m_pInput;
 *         : CStringArray	  m_lpCmdLineArray;
 *         : BOOL			  m_bDoVerify;
 *         : BOOL			  m_bDoDelete;
 *         : 
 * purpose : test of the ISDN unit
 *         :
 * valid   : -host    : machine name or number
 * params  : -size    : size of file to transfere
 * of the  : -verify  : compares received data with orig.
 * command : -delete  : delete the remote file
 * line    : -filename: name of the file to transfere
 *         : -loop    : number of repetitions to get file 
 *         :
 * author  : Hajo Fierke / Hedu 1997 w+k
 * history : 19.03.1997
 *         : 25.03.97 hajo -loop implemented
 *         : 25.03.97 verify added HEDU
 *         : 26.03.97 hajo -delete implemeted
 *		   : 01.04.97 hajo -loop set to 20
 *		   : 07.04.97 hajo added hedu's cryptical remarks  
 *         : 18.04.1997	hajo collect string: corrigé la famme
 **************************************************************/
/*	@DOC
	@TOPIC{Tester}
	if there is hell below, we'r all gonne go			@LINE
									Curtis Mayfield
	@ALSO @LINK{Overview | CTesterApp, overview}	

	@TOPIC{CTesterApp Overview|CTesterApp, overview}	
	Test application for Security3.0 Internal Documentation
	@ALSO @LINK{CWinApp}
	@TOPIC{CTesterApp Members|CTesterApp, members}
	@LISTHEAD{Access:}

	@PAR 
data:
	CString			 m_szHost;			@LINE
	CString			 m_sTestFileName;	@LINE
	int				 m_iDestination;	@LINE
	int				 m_iSizeofpData;	@LINE
	int				 m_iLoop;			@LINE
	CIPCInputTester* m_pInput;			@LINE
	CStringArray	 m_lpCmdLineArray;	@LINE
	BOOL			 m_bDoVerify;		@LINE
	BOOL			 m_bDoDelete;		@LINE

	@TOPIC{CTesterApp Notes|CTesterApp, notes}
	test of ISDN connection:						@LINE

notes:		  valid parameters
	-host    : machine name or number				@LINE
	-size    : size of file to transfere			@LINE
	-verify  : compares received data with orig.	@LINE
	-delete  : delete the remote file				@LINE
	-filename: name of the file to transfere		@LINE
	-loop    : number of repetitions to get file 	@LINE	
	@PAR
uses:
	@LINK{Overview | CIPCInputTester, overview}		@LINE
	@PAR
	@HRULE											@LINE
author  : Hajo Fierke / Hedu 1997 w+k				@LINE
history : 19.03.97 self								@LINE
        : 01.04.97 hajo -loop set to 20				@LINE
        : 07.04.97 hedu's cryptical remarks added	@LINE
	@DOCEND
*/
/*
@MEMBER{inline BOOL CTesterApp::DoVerify() const}

  inline BOOL DoVerify() const
*/
/*
@MEMBER{inline CString CTesterApp::GetTestFileName() const}

	inline CString GetTestFileName() const;
*/
/*
@MEMBER{inline int CTesterApp::GetDestination() const}

	inline int GetDestination() const;
*/
/*
@MEMBER{inline int CTesterApp::GetSizeofpData() const}

	inline int GetSizeofpData() const;
*/
/*
@MEMBER{inline int CTesterApp::GetLoop() const}

	inline int GetLoop() const;
*/
/*
@MEMBER{inline int CTesterApp::GetDoDelete() const}

	inline int GetDoDelete() const;
*/

#include "stdafx.h"
#include "Tester.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "TesterDoc.h"
#include "TesterView.h"
#include "wk.h"
#include "CIPCInputTester.h"
#include "CIPCServerControlClientSide.h"
#include "User.h"
#include "host.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////
// CTesterApp

BEGIN_MESSAGE_MAP(CTesterApp, CWinApp)
	//{{AFX_MSG_MAP(CTesterApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping
		//		  macros here.
		//  DO NOT EDIT what you see in these blocks of 
		//	generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////
// CTesterApp construction

/*
@MEMBER{CTesterApp::CTesterApp()}

  CTesterApp
*/
CTesterApp::CTesterApp()
{
	m_pInput		= NULL;
	m_bDoDelete		= FALSE;
	m_bDoVerify		= FALSE;
	m_iDestination	= 4;	// use full pathname
	m_iSizeofpData  = (1024*2);
	m_iLoop			= 20;
	m_szHost		= CString( "83" ); // Micotest
	m_szUserDummy	= CString( "Tester" );
	m_sTestFileName	= CString( "C:\\Log\\foo.dat" );
}

///////////////////////////////////////////////////////////////
// The one and only CTesterApp object

CTesterApp theApp;

///////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	// DDX/DDV support
	virtual void DoDataExchange(CDataExchange* pDX);    
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTesterApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/*
@MD
*/
int CTesterApp::ExitInstance() 
{ 
	int i=0;
	if ((i=m_lpCmdLineArray.GetSize())>0)
	{
		m_lpCmdLineArray.RemoveAt(0,i);
	} // end if

 WK_DELETE(m_pInput);
	
 CloseDebugger();
	
 return CWinApp::ExitInstance();
}

/*
@MD

	BOOL InitInstance()
	CTesterApp initialization
uses:
	InitDebugger("Tester.log");
@ALSO
@LINK{CStringArray} 
*/
BOOL CTesterApp::InitInstance()
{
// Standard initialization
// If you are not using these features and wish to reduce the
// size of your final executable, you should remove from the
// following the specific initialization routines 
// you do not need.

#ifdef _AFXDLL
	// Call this when using MFC in a shared DLL
	Enable3dControls();		 
#else
	// Call this when linking to MFC statically
	Enable3dControlsStatic();
#endif

	// Load standard INI file options (including MRU)
	LoadStdProfileSettings();  

	// Register the application's document templates.  
	// Document templates serve as the connection between
	// documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_TESTERTYPE,
		RUNTIME_CLASS(CTesterDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CTesterView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	InitDebugger("Tester.log");

	if (m_lpCmdLine[0] == '\0')
	 {
		WK_TRACE("Host= %s\n",(const char*)m_szHost);
	 }
	else
	 {
		SplitArgs(m_lpCmdLine);
		m_szHost = FindHostNumberByName(ParseOptions(m_lpCmdLineArray));
	 }

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);	// OOPS HEDU
	pMainFrame->UpdateWindow();
	
	if( DoTest(m_szHost) ) {
		return TRUE;
	}
//	else
	return FALSE;
}

/**funcref ***************************************************
 * class   : CTesterApp 
 * name    : FindHostNumberByName(
 * input   : const CString &sHostsname) const;	 
 *         : 
 * output  : CString: if   sHostsname matches TCP:
 *         :			   returns sHostsnamen unchanged 
 *         : CString: if   sHostsname matches with CHost Array: 
 *         :			   returns host number as CString: 
 *         :          else returns m_szHost	(number)
 *         :
 * purpose : I bother You and I fake You, by the living god
 *         :	that made You!		UFO 
 * uses    : CHost, CHostArray
 *         :
 * author  : Hajo Fierke  1997 w+k
 * history : 24.03.1997
 *         : 23.05.1997	self tcp: check added
 *************************************************************/
/*
@MD
*/
CString CTesterApp::FindHostNumberByName(
	const CString &sHostsname) const
{
 int iWhatNum=sHostsname.FindOneOf( "0123456789" );
 CString isTcpIp=sHostsname.Left(4); // TCP:
 
 if(isTcpIp.CompareNoCase( "TCP:" ) == 0) // equal
 {
	return sHostsname;
 }

 if(iWhatNum > -1 && iWhatNum < 2)
 {
	return sHostsname;
 }
 // else:
 CHostArray l_hosts;
 
 l_hosts.Load();

 for (int i=0; i < l_hosts.GetSize(); i++)
 {
	CHost *pHost = l_hosts[i];
	if (pHost->GetName()== sHostsname) 
	{
		return (CString)pHost->GetNumber(); //(const char*)
	}
 }
 return m_szHost; 
}

/**funcref ****************************************************
 * class   : CTesterApp
 * name    : SplitArgs(
 * input   : const CString &argsIn);	m_lpCmdLine 
 *         :							
 * purpose : splits the command line into a member string array
 *         :
 * uses    : CStringArray* m_lpCmdLineArray 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 24.03.1997	self
 *         : 18.04.1997	hajo collect string: 
 **************************************************************/
/*
@MD
*/
void CTesterApp::SplitArgs(const CString &argsIn)
{
 CString collect(argsIn);
 int num = argsIn.GetLength();
 collect += ' ';

 WK_TRACE("argsIn= %s\n",(const char*)argsIn);
 int i=0;
 int j=0;
 for(i=0; i<num; i++)
  {
   j=0;
   while(i < num && argsIn[i]==' ') 
	   i++; //remove leading spaces
   while(i < num && argsIn[i]!=' ') 
   {
    collect.SetAt(j, argsIn[i]);
    i++;
    j++;
   }
   collect.SetAt(j, 0);

   m_lpCmdLineArray.Add( (const char *)collect );
  }
}

/**funcref ****************************************************
 * class   : CTesterApp
 * name    : ParseOptions(
 * input   : CStringArray lpCmdLineArray);	m_lpCmdLineArray 
 *         :							
 * purpose : parses the CStringArray into allowed values:
 *         : -filename: m_sTestFileName							
 *         : -size	  : m_iSizeofpData						
 *         : -host	  : m_szHost 						
 *         : -verify  : m_bDoVerify
 *         : -loop 	  :	m_iLoop
 *         : -delete  :	m_bDoDelete	
 *         :
 * uses    : WK_TRACE_ERROR(...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 24.03.1997
 *         : 25.03.1997	hajo -loop implemented
 **************************************************************/
/*
@MD
*/
CString CTesterApp::ParseOptions(CStringArray &lpCmdLineArray)
{
 CString l_sHost;

// WK_TRACE("lpCmdLineArray[%d]= %s\n",0,(CString)lpCmdLineArray->GetAt(0));
// WK_TRACE("lpCmdLineArray[%d]= %s\n",1,(CString)lpCmdLineArray->GetAt(1));
 int i=0;

 while ((i=lpCmdLineArray.GetSize()))
 {

	if ((lpCmdLineArray.GetAt(0)=="-host") && (i>=2) )
	{
		l_sHost = (CString)lpCmdLineArray.GetAt(1);
		lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((lpCmdLineArray.GetAt(0)=="-filename") && (i>=2) )
	{
		m_sTestFileName = (CString)lpCmdLineArray.GetAt(1);
		lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((lpCmdLineArray.GetAt(0)=="-loop") && (i>=2) ) 
	{
		m_iLoop = atoi(lpCmdLineArray.GetAt(1));
		if(m_iLoop > 20)
		{	
			m_iLoop=20;
			WK_TRACE("m_iLoop is set to %d\n",m_iLoop);
		}
		lpCmdLineArray.RemoveAt(0,2);
	}
	else if ((lpCmdLineArray.GetAt(0)=="-size")&& (i>=2) ) 
	{
		m_iSizeofpData = atoi(lpCmdLineArray.GetAt(1));
		lpCmdLineArray.RemoveAt(0,2);
//			WK_TRACE("m_iSizeofpData= %d\n",m_iSizeofpData);
	}
	else if (lpCmdLineArray[0]=="-verify") 
	{
		lpCmdLineArray.RemoveAt(0);
		m_bDoVerify=TRUE;
//			WK_TRACE("-verify enabled\n");
	}
	else if (lpCmdLineArray[0]=="-delete") 
	{
		lpCmdLineArray.RemoveAt(0);
		m_bDoDelete=TRUE;
//			WK_TRACE("-delete enabled\n");
	}
	else // if there are still some left
	{
		WK_TRACE_ERROR("Unused arg %s\n",
				(const char*)lpCmdLineArray[0]);
		lpCmdLineArray.RemoveAt(0); // push it out!
	}
	   
 }	// end while 
 return l_sHost;
}

///////////////////////////////////////////////////////////////
// CTesterApp commands
/*
@MD
*/
int CTesterApp::DoTest(const CString &sHost)
{
	CUser userDummy;
	userDummy.SetName(m_szUserDummy);
	CConnectionRecord cRec(
		sHost,			// const CString &remoteHost,
		userDummy,		// CUser user
		"SuperVisor",	// sPermission,
		""				// sPassword,
						// DWORD dwTimeout=30000
		);
	CTickCounter m_TickCounter;
	m_TickCounter.StartCount();

	CIPCFetchResult result = NewFetchInputConnection( cRec );
	// double check on Okay AND a shared memory name
	if(result.IsOkay()==FALSE || result.GetShmName().GetLength()==0)	 
	{
		WK_TRACE_ERROR("gotta no FetchResult\n");
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return 0;	// <<< EXIT >>>
	}

	m_TickCounter.StopCount();
	WK_TRACE("NewFetchInputConnection nach %d ms\n",
		m_TickCounter.GetDeltaTicks()
		);

	
	CString       sShmName = result.GetShmName() ;
	if(sShmName.GetLength())
	{
//		WK_TRACE("sShmName=%s\n",(const char*)sShmName);	
		m_pInput = new CIPCInputTester(sShmName);
		m_pInput->StartThread();
	}
	else
	{
		WK_TRACE("hostname %s failed\n",(const char*)sHost);	
		WK_TRACE("\terror %s[%d]\n",
					NameOfEnum(result.GetError()),
					result.GetErrorCode()
					);	
		WK_TRACE("\tmsg %s\n",
					(const char *)result.GetErrorMessage()
					);	
	}
	return 42;
}

