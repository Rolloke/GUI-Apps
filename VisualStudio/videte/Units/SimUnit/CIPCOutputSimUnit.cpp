// CIPCOutputSimUnit.cpp
// w+k Sample Application
// Author : Uwe Freiwald

// Implementation of CIPCOutputSimUnit
// 
// The CIPCOutputSimUnit performs all output communication and connection to Security Unit.
// Output means input from the view of Security unit. For SimUnit it's more input from Security.

// To write your own CIPCOutput's, derive your own object from CIPCOutput class and overwrite
// several virtual methods to implement your output application.
// You should create one or more instances of CIPCOutputSimUnit per application, see CSimUnitApp.
//
// You can handle up to 32 (DWORD size) outputs per object instance with status 1 or 0, a relay with open
// or close status. For further output use another instance with other shared memory name.

/////////////////////////////////////////////////////////////////////////////
//
//						Communication Start
//
//			MASTER(Security)		SLAVE(Unit)
//
//		DoRequestConnection	->	OnRequestConnection
//		OnConfirmConnection	<-	DoConfirmConnection
//
//		DoRequestSetGroupID	->	OnRequestSetGroupID
//		OnConfirmSetGroupID	<-	DoConfirmSetGroupID		
//
//		DoRequestHardware	->	OnRequestHardware
//		OnConfirmHardware	<-	DoConfirmHardware		
//
//		If Hardware Error Code != 0 Connection stops hiere
//
//		DoRequestReset		->	OnRequestReset
//		OnConfirmReset		<-	DoConfirmReset
//
//					Connection completed		
//
/////////////////////////////////////////////////////////////////////////////
//
//						Running Process
//
//			MASTER(Security)		SLAVE(Unit)
//
//		DoRequestCurrentState	->	OnRequestCurrentState
//		OnConfirmCurrentState	<-	DoConfirmCurrentState
//
//		DoRequestSelectRelay	->	OnRequestSelectRelay
//		OnConfirmSelect			<-	DoConfirmSelect
//					
/////////////////////////////////////////////////////////////////////////////
//
//						Communication End
//
//			MASTER(Security)		SLAVE(Unit)
//
//		DoRequestDisconnect		->	OnRequestDisconnect
//		OnConfirmDisconnect		<-	DoConfirmDisconnect
//
//		DoRequestCloseModule	->	OnRequestCloseModule
//		OnConfirmCloseModule	<-	DoConfirmCloseModule
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimUnit.h"
#include "SimUnitDlg.h"

#include "CIPCOutputSimUnit.h"

//////////////////////////////////////////////////////////////////////////////////////
// Constructor
// 
// add one time construction and member variables initilizing code here
// call StartThread to start internal communication process with Security Unit
//
// the sample stores a pointer to App's instance variable
//
CIPCOutputSimUnit::CIPCOutputSimUnit(CSimUnitDlg *pDlg, LPCTSTR shmName)
	: CIPCOutput(shmName,FALSE)
{
	m_wGroupID = SECID_NO_GROUPID;
	m_pDlg = pDlg;
	StartThread(TRUE);
}
//////////////////////////////////////////////////////////////////////////////////////
// Destructor
//
// free all additional memory here
//
// default destructor calls StopThread too
//
CIPCOutputSimUnit::~CIPCOutputSimUnit()
{
	StopThread();
}


//////////////////////////////////////////////////////////////////////////////////////
// Initializing and connection code
//////////////////////////////////////////////////////////////////////////////////////
// 1. called overridden method
// first establish connection to Security
//
void CIPCOutputSimUnit::OnRequestConnection()
{
	MY_POST(ID_CONNECTION_OUTPUT,0);
}
//////////////////////////////////////////////////////////////////////////////////////
// 2. called overridden method
// store iGroupNr in member variable if necessary
void CIPCOutputSimUnit::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID;
	DoConfirmSetGroupID(wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
// 3. called overridden method
// then initialize hardware (extension cards, devices connected on LPTx or COMx, ...)
// add hardware initializing code here
// don't forget to call DoConfirmHardware with error code as second parameter
//
void CIPCOutputSimUnit::OnRequestHardware(WORD wGroupID)
{
	MY_POST(ID_HARDWARE_OUTPUT,0);

	// SimUnit has not picture capabilities, so all is FALSE
	DoConfirmHardware(m_wGroupID, 0,
						FALSE,	// bCanSWCompress,
						FALSE,	// bCanSWDecompress,
						FALSE,	// CanColorCompress,
						FALSE,	// bCanColorDecompress,
						FALSE	// bCanOverlay
						);
}
//////////////////////////////////////////////////////////////////////////////////////
// 4. called overridden method
// add reset code here, set all values to default
//
void CIPCOutputSimUnit::OnRequestReset(WORD wGroupID)
{
	MY_POST(ID_RESET_OUTPUT,0);

	DoConfirmReset(wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
// Running Application code
//////////////////////////////////////////////////////////////////////////////////////
// Security ask's in specified time intervalls for current state
//
void CIPCOutputSimUnit::OnRequestCurrentState(WORD wGroupID)
{
	DoConfirmCurrentState(wGroupID, m_pDlg->GetRelayMask());
}
//////////////////////////////////////////////////////////////////////////////////////
// A process from security asks for closing or opening a relay
// iNr contains the relay number, zero based index
// bState = TRUE means relay have to be closed
//
void CIPCOutputSimUnit::OnRequestSetRelay(CSecID relayID, BOOL bClosed)
{
	WORD wGroupID = relayID.GetGroupID();
	
	if (m_wGroupID == wGroupID)
	{
		int	iNr = (int)relayID.GetSubID();
		if (bClosed) {
			MY_POST(ID_CLOSE_RELAY,iNr);
		} else {
			MY_POST(ID_OPEN_RELAY,iNr);
		}

		DoConfirmSetRelay(relayID, bClosed);
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestSetRelay Wrong group ID %04x != %04x\n"), wGroupID, m_wGroupID);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
// Disconnecting code
//////////////////////////////////////////////////////////////////////////////////////
// disconnecting method call initiated by Security, while Security shut's down
// add disconnecting code here
//
void CIPCOutputSimUnit::OnRequestDisconnect()
{
	MY_POST(ID_DISCONNECT_OUTPUT,0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCOutputSimUnit::OnRequestVersionInfo(WORD wGroupID)
{
	// 1 CIPC default
	// 2 supports VersionInfo
	DoConfirmVersionInfo(wGroupID,2);
}
