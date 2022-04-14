/////////////////////////////////////////////////////////////////
//
// FILE:	$Workfile: CIPCInputTester.cpp $ 
// ARCHIVE:	$Archive: /Project/Tools/Tester/CIPCInputTester.cpp $
// CHECKIN:	$Date: 17.04.97 18:17 $ 
// VERSION:	$Revision: 20 $
// LAST:	$Modtime: 17.04.97 16:02 $ 
// AUTHOR:	$Author: Hajo $
//			$Nokeywords:$

/**classref ***************************************************
 * name    : CIPCInputTester.cpp
 * class   : CIPCInputTester
 * derived : CIPCInput
 * uses    : CTickCounter;						   TickCount.h
 *         : WK_DELETE_ARRAY(m_pData); 
 * members : 
 * public  : virtual void OnReadChannelFound();
 *         : virtual void OnConfirmConnection();
 *         : virtual void OnRequestDisconnect();
 *         : virtual void OnRequestGetFile();
 *         : virtual void OnConfirmGetFile(.pipifax.);
 *         : virtual void OnConfirmFileUpdate(...);
 *         :		 void DoMyRequestFileUpdate();
 *         :		 void DoMyDeleteFile();
 *         :		 void Init(void);
 *         : 
 * protect : int		  m_iDestination;	
 *         : CString	  m_sTestFileName;
 *         : CTickCounter m_TickCounter[11];
 *         : BYTE*		  m_pData;
 *         : int		  m_iSizeofpData;
 *         : int		  m_iLoops;
 *         : 
 * purpose : test of ISDN connection
 *         : calculates the the time between the call of
 *         : DoRequestxxx and
 *         : OnConfirmxxx
 *         : 
 * author  : Hajo Fierke / Hedu 1997 w+k
 * history : 19.03.97 self
 *         : 25.03.97 verify added HEDU
 *         : 26.03.97 hajo add: DoMyDeleteFile on request
 *		   : 04.04.97 hedu's cryptical remarks added
 *		   : 07.04.97 hedu's cryptical remarks added
 **************************************************************/
/*	@DOC

	@TOPIC{CIPCInputTester Overview | CIPCInputTester, overview}
	@KEY{Overview, CIPCInputTester}
	ich weiss nicht, was soll es bedeuten?					@LINE
	@ALSO	@LINK{members | CIPCInputTester, members},
			@LINK{notes | CIPCInputTester, notes}

	@TOPIC{CIPCInputTester Notes | CIPCInputTester, notes}
	@KEY{Notes, CIPCInputTester}
	Test application for Security3.0 Internal Documentation	@LINE
	
	purpose: 												@LINE
	test of ISDN connection: 								@LINE
	calculates the the time between the call of				@LINE
	DoRequestxxx and OnConfirmxxx							@LINE

	@ALSO	@LINK{members | CIPCInputTester, members},
			@LINK{overview | CIPCInputTester, overview}

	@TOPIC{CIPCInputTester Members|CIPCInputTester, members}
	@KEY{Members, CIPCInputTester}
	@ALSO	@LINK{overview | CIPCInputTester, overview}
			@LINK{notes | CIPCInputTester, notes}

	@PAR 
protected members:
	int m_iDestination;							@LINE
	CString m_sTestFileName;					@LINE
	CTickCounter m_TickCounter[11];				@LINE
	BYTE* m_pData;								@LINE
	int m_iSizeofpData;							@LINE
	int m_iLoops;								@LINE
	@PAR
uses:
	@LINK{CTickCounter}									@LINE
	@LINK{WK_DELETE_ARRAY}
	@HRULE												@LINE
author  : Hajo Fierke / Hedu 1997 w+k					@LINE
history : 19.03.97 self									@LINE
        : 25.03.97 verify added HEDU					@LINE
        : 26.03.97 hajo add: DoMyDeleteFile on request	@LINE
        : 04.04.97 hedu's cryptical remarks added		@LINE
        : 07.04.97 hedu's cryptical remarks added		@LINE
	@DOCEND
*/
#include "stdafx.h"
#include "TickCount.h"
#include "CIPCInputTester.h"
#include "Tester.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CONNECT 0
#define GETFILE 1
#define FILEUPD 2

/*
@MD

@ARGS 
@ITEM@CW{szShmName} is the name of the connection. 
@ITEM@CW{bAsMaster} used internally to assign the order of
channels. 
@BOLD Units have to set @CODE bAsMaster=FALSE @NORMAL.
@PAR
Calls @MLINK{CIPCInputTester::Init}
@ALSO
@LINK{Overview|CIPCInput}
*/
CIPCInputTester::CIPCInputTester(const char *szShmName)
	: CIPCInput(szShmName, FALSE) // bAsMaster=SLAVE
{						  //TRUE   = Master
	m_pData         = NULL;
	Init();
}

/*
@MHEAD{destructor:}
@MD
the destructor calls @MLINK{CIPC::StopThread}
			   and 	 @MLINK{WK_DELETE_ARRAY} to set memory free
@ALSO
@MLINK{CIPCInput}
*/
CIPCInputTester::~CIPCInputTester()
{
	StopThread();	// OOPS noch nicht erklärt
	WK_DELETE_ARRAY(m_pData); 
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : Init(
 * input   : );
 *         : 
 * purpose : initializes the members with the application's
 *         :	variables.
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 20.03.1997	self
 **************************************************************/
/*
@MHEAD{misc:}
@MD

  initializes the members with the application's variables.

@ALSO
@LINK{CTesterApp::MyGetApp} 
*/
void CIPCInputTester::Init(void)
{
	m_iDestination  = MyGetApp()->GetDestination();
	m_sTestFileName = MyGetApp()->GetTestFileName();
	m_iSizeofpData  = MyGetApp()->GetSizeofpData();
	m_iLoops        = MyGetApp()->GetLoop(); 	//<=10

	m_pData= new BYTE[m_iSizeofpData];
	for(int i=0; i < m_iSizeofpData; i++) 
	{
		m_pData[i]= (BYTE)(i & 255);
	}
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : OnReadChannelFound(
 * input   : );
 *         : 
 * purpose : starts tick counter and calls DoRequestConnection
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
/*
@MD

Overrides @MLINK{CIPC::OnReadChannelFound} 
Called if the read channel of a shared memory connection is found.
See @LINK{CIPCState} for details.

initialized m_TickCounter[xx],@LINK{CTickCounter} with one of
the following lables
@KEY{CONNECT}
@KEY{GETFILE}
@KEY{FILEUPD}

and calls @MLINK{CIPCInput::DoRequestConnection} 
@ALSO @MLINK{CIPC::DoRequestConnection}, 
	@MLINK{CIPC::OnRequestConnection}, 
	@MLINK{CIPC::OnRequestDisconnect}
*/
void CIPCInputTester::OnReadChannelFound()
{
//	WK_TRACE("OnReadChannelFound\n");

	m_TickCounter[CONNECT].StartCount();

	DoRequestConnection();
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : OnConfirmConnection(
 * input   : );
 *         :
 * purpose : is the answere to the Requested Connection	s.a. 
 *         : overrides virtual CIPCInput::OnConfirm...
 *         : holds the chain (request/confirm) alive.
 *         :
 * uses    : class CTickCounter
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
/*
@MD

called if the other side answered on a 
@LINK{CIPC::DoRequestConnection}.
Overrides @MLINK{CIPCInput::OnConfirmConnection} 
stops the TickCounter and prints out 
@MLINK{TickCounter::GetDeltaTicks}
and calls @MLINK{CIPCInputTester::DoMyRequestFileUpdate}
*/
void CIPCInputTester::OnConfirmConnection()
{
	m_TickCounter[CONNECT].StopCount();
	WK_TRACE("OnConfirmConnection nach %d ms\n",
		m_TickCounter[CONNECT].GetDeltaTicks()
		);
	DoMyRequestFileUpdate();
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : DoMyRequestFileUpdate(
 * input   :);
 *         : 
 * purpose : asks the server to create, open a file
 *         : for (over-) writing pData buffer.
 * uses    : DoRequestFileUpdate(
 *         : int iDestination,:	
 *         :     0 = AppDir, 
 *         :     1 = Windows, 
 *         :     2 = Windows/System,
 *         :     3 = Root,   
 *         :     4 = use the path\sFileName
 *         :     5, 6, 7 not implemented no doc do not use or die
 *         :   | 8 = DELETE	
 *         :   |10 = EXECUTE
 *         : m_sTestFileName...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 21.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::DoMyRequestFileUpdate()
{
//	WK_TRACE(" FileUpd\n");
	m_TickCounter[FILEUPD].StartCount();
	
	DoRequestFileUpdate(
		m_iDestination,
		m_sTestFileName,
		m_pData,				//const void *pData,
		(DWORD)m_iSizeofpData,	//DWORD dwDataLen,
		FALSE					//BOOL  bNeedsReboot
		);
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : DoMyDeleteFile(
 * input   :);
 *         : 
 * purpose : asks the server to create, open a file
 *         : for (over-) writing pData buffer.
 * uses    : DoRequestFileUpdate(
 *         : int iDestination or 8 = DELETE	
 *         : ...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 25.03.1997
 *         : 25.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::DoMyDeleteFile()
{
//	WK_TRACE(" DeleteFile\n");
	
	DoRequestFileUpdate(
		m_iDestination + 8,	// cave canem!
		m_sTestFileName,
		NULL,			// const void *pData,
		(DWORD)0,		// DWORD dwDataLen,
		FALSE			// BOOL bNeedsReboot
		);
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : OnConfirmFileUpdate(
 * input   : const CString &sFileName);	name to write out 
 *         :							or update
 * purpose : 
 * uses    : 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 21.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::OnConfirmFileUpdate(const CString &sFileName)
{
 m_TickCounter[FILEUPD].StopCount();
 
 double l_ticks= (double)m_TickCounter[FILEUPD].GetDeltaTicks();

 WK_TRACE("FileUpd = %d ms %d Baud\n",
		m_TickCounter[FILEUPD].GetDeltaTicks(),
		(int)( MyGetApp()->GetSizeofpData() * (8000. / l_ticks))
		);
 Sleep(777); // geb den launscher eine schanze
 DoMyRequestGetFile();
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : DoMyRequestGetFile(
 * input   : );
 *         : 
 * purpose : 
 * uses    : DoRequestGetFile(...);
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 21.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::DoMyRequestGetFile()
{
// WK_TRACE("OnRequestGetFile\n");
 m_TickCounter[GETFILE].StartCount();

 DoRequestGetFile(
		m_iDestination,	// int iDestination: 
						//  0 = AppDir, 1 = Windows, 
						//  2 = Windows/System,
						//  3 = Root,   4 = use path/sFileName
						//  5,6,7 implemented no doc do not use
						// | 8 = DELETE	
						// |10 = EXECUTE
		m_sTestFileName	// const CString &sFileName
		);
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : virtual void OnConfirmGetFile(
 * input   : int iDestination,
 *         : const CString &sFileName, name to write or update
 *         : const void *pData,
 *         :       DWORD dwDataLen
 *         :
 * purpose : prints out following results:
 *         : steps(20):Zeitschnitt=   1717 ms
 *         :           hoechstWert= 116295 [baud]
 *         : steps(20):Baudschnitt= 115499 [b/s]
 *         :           kleinstWert= 114219 [baud]
 *         : 
 * uses    : 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 26.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::OnConfirmGetFile(
	int iDestination,
	const CString &sFileName,
	const void *pData,
	DWORD dwDataLen
	)
{
 static double   dSchnitt = 0.0;
 static double      dBaud = 0.0;
        double    l_dBaud = 0.0;
 static double   dminBaud = 0.0;
 static double   dmaxBaud = 0.0;
 static int         iloop = m_iLoops-1; // default 10 loops
 static int iMerkDirEinen = 0;
		double    l_ticks = 0.0;
		int         iSize = (int)( MyGetApp()->GetSizeofpData() );

 m_TickCounter[GETFILE].StopCount();
 l_ticks = (double)m_TickCounter[GETFILE].GetDeltaTicks();

 if (dwDataLen == iSize) 
 {
	 if(l_ticks) 
	 {
		l_dBaud = (iSize * (8000. / l_ticks));
		/*
		WK_TRACE("received=%6d %6d ms %2d sec %6d baud\n",
			dwDataLen,
			(int)l_ticks,
			(int)(l_ticks/1000.),
			(int)(l_dBaud) 
			);
		*/
	 }
	 else
	 {
		 // OOPS what means no lticks ? error msg ?
	 }

	int iWrongCount=0; // do only print a limited number or messages
	if (MyGetApp()->DoVerify()) 
	{
		const BYTE *pByteData = (const BYTE *)pData;
		for (int i=0; i < iSize && iWrongCount<20; i++) 
		{
		 if (pByteData[i]!=m_pData[i]) 
		 {
			iWrongCount++;
			WK_TRACE_ERROR("Wrong data[%4d] got %d(0x%x) expected %d(0x%x)\n",
				 i, 
				 pByteData[i], pByteData[i],
				 m_pData[i], m_pData[i]
				 );
		 }
		}
	}	// end of verify

//	l_dBaud = (iSize * (8000. / l_ticks));
	dBaud += l_dBaud;

	if(dminBaud==0.0) // the first cut is the deepest
	{
		dminBaud = l_dBaud; 
	}
	else
	{
		dminBaud = min(l_dBaud,dminBaud); 
	}
	dmaxBaud = max(l_dBaud,dmaxBaud); 
	
	dSchnitt += l_ticks;
	iMerkDirEinen++;
 }	// end of correct file size
 else 
 {
	WK_TRACE_ERROR("Wrong filesize got %d expected %d\n",
					dwDataLen, iSize
					);
 } 

 if(iloop > 0)	 // see constructor
  { 
   DoMyRequestGetFile();
   iloop--;
  }
 else
  {
	if(iMerkDirEinen)
	{

		WK_TRACE("steps(%2d):Zeitschnitt= %6d ms\n",
			iMerkDirEinen,
			(int)(dSchnitt / iMerkDirEinen)
			);
		WK_TRACE("          hoechstWert= %d [baud]\n",
			(int)(dmaxBaud)
			);
		WK_TRACE("steps(%2d):Baudschnitt= %d [b/s]\n",
			iMerkDirEinen,
			(int)(dBaud / iMerkDirEinen)
			);
		WK_TRACE("          kleinstWert= %d [baud]\n",
			(int)(dminBaud)
			);
	}
	if( MyGetApp()->GetDoDelete() )
	{
//		WK_TRACE("Delete= %6d \n", MyGetApp()->GetDoDelete());
		DoMyDeleteFile();
	}
   Sleep(888); // wait before the deluge

   AfxGetMainWnd()->PostMessage(WM_CLOSE);
  }
}

/**funcref ****************************************************
 * class   : CIPCInputTester
 * name    : OnRequestDisconnect(
 * input   : );
 *         : 
 * purpose : 
 *         :
 * author  : Hajo Fierke 1997 w+k
 * history : 19.03.1997
 *         : 19.03.1997	self
 **************************************************************/
/*
@MD

*/
void CIPCInputTester::OnRequestDisconnect()
{
	WK_TRACE("OnRequestDisconnect\n");
}

