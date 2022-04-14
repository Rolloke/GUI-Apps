// CIPCInputSimUnit.cpp
// w+k Sample Application
// Author : Uwe Freiwald

// Implementation of CIPCInputSimUnit
// 
// The CIPCInputSimUnit performs all input communication and connection to Security Unit.
// Input means input from the view of Security unit. For SimUnit it's more output to Security.

// To write your own CIPCInput's, derive your own object from CIPCInput class and overwrite
// several virtual methods to implement your input application.
// You should create one or more instances of CIPCInputSimUnit per application, see CSimUnitApp.
//
// You can handle up to 32 (DWORD size) inputs per object instance with status 1 or 0, a switch with open
// or close status. For further input use another instance with other shared memory name.

/////////////////////////////////////////////////////////////////////////////
//
//						Communication Start
//
//			MASTER(Security)		SLAVE(Unit)
//
//		DoRequestConnection	->	OnRequestConnection
//							<-	DoConfirmConnection
//
//		DoRequestGroupID	->	OnRequestGroupID
//		OnConfirmSetGroupID	<-	DoConfirmGroupID		
//
//		DoRequestHardware	->	OnRequestHardware
//		OnConfirmHardware	<-	DoConfirmHardware		
//
//		If Hardware Error Code != 0 Connection stops hiere
//
//		DoRequestSetEdge	->	OnRequestSetEdge
//		OnConfirmSetEdge	<-	DoConfirmEdge
//
//		DoRequestSetFree	->	OnRequestSetFree
//		OnConfirmFree		<-	DoConfirmFree
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
//	  OnIndicateAlarmState  <- DoIndicateAlarmState
//
/////////////////////////////////////////////////////////////////////////////
//
//						Communication End
//
//			MASTER(Security)		SLAVE(Unit)
//
//		DoRequestDisconnect	->	OnRequestDisconnect
//		OnConfirmDisconnect	<-	DoConfirmDisconnect
//
//		DoRequestCloseModule	->	OnRequestCloseModule
//		OnConfirmCloseModule	<-	DoConfirmCloseModule
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SimUnit.h"
#include "SimUnitDlg.h"

#include "CIPCInputSimUnit.h"

//////////////////////////////////////////////////////////////////////////////////////
// Constructor
// 
// add one time construction and member variables initilizing code here
// call StartThread to start internal communication process with Security Unit
//
// the sample stores a pointer to App's instance variable
//
CIPCInputSimUnit::CIPCInputSimUnit(LPCTSTR shmName)
	: CIPCInput(shmName,FALSE)
{
	m_wGroupID = SECID_NO_GROUPID;
	m_bAnyAlarm = FALSE;
	m_dwAlarmMask = 0;
	m_dwEnableMask = 0;
	m_dwEdgeMask= 0xFFFFFFFF;
	StartThread(TRUE);
}
//////////////////////////////////////////////////////////////////////////////////////
// Destructor
//
// free all additional memory here
//
// default destructor calls StopThread too
//
CIPCInputSimUnit::~CIPCInputSimUnit()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Initializing and connection code
//////////////////////////////////////////////////////////////////////////////////////
// 1. called overridden method
// first establish connection to Security
//
void CIPCInputSimUnit::OnRequestConnection()
{
	MY_POST(ID_CONNECTION_INPUT,0);
	CIPC::OnRequestConnection();
}
//////////////////////////////////////////////////////////////////////////////////////
// 2. called overridden method
// store iGroupNr in member variable if necessary
void CIPCInputSimUnit::OnRequestSetGroupID(WORD wGroupID)
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
void CIPCInputSimUnit::OnRequestHardware(WORD iGroupNr)
{
	MY_POST(ID_HARDWARE_INPUT,0);

	DoConfirmHardware(m_wGroupID,0);
}
//////////////////////////////////////////////////////////////////////////////////////
// 4. called overridden method
//
void CIPCInputSimUnit::OnRequestSetEdge(WORD wGroupID,DWORD dwEdgeMask)	// 1 positive, 0 negative
{
	m_dwEdgeMask = ~dwEdgeMask;

	DWORD dwParam=0;
	dwParam = MAKELONG(m_dwEnableMask, m_dwEdgeMask);
	MY_POST(ID_EDGE_INPUT, dwParam);

	DoConfirmEdge(m_wGroupID, dwEdgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 5. called overridden method
//
void CIPCInputSimUnit::OnRequestSetFree(WORD wGroupID,DWORD dwEnableMask)	// 1 open, 0 closed
{
	m_dwEnableMask = dwEnableMask;

	DWORD dwParam=0;
	dwParam = MAKELONG(m_dwEnableMask, m_dwEdgeMask);
	MY_POST(ID_ENABLE_INPUT, dwParam);

	DoConfirmFree(m_wGroupID, dwEnableMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 6. called overridden method
// add reset code here, set all values to default
//
void CIPCInputSimUnit::OnRequestReset(WORD wGroupID)
{

	m_dwAlarmMask = m_dwEdgeMask & m_dwEnableMask;

	DWORD dwParam=0;
	dwParam = MAKELONG(m_dwEnableMask, m_dwAlarmMask);
	MY_POST(ID_RESET_INPUT, dwParam);

	DoConfirmReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
// Running Application code
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSimUnit::IndicateAlarm(int nNr, BOOL bUndo /*= FALSE*/)
{
	DWORD dwNewMask;

	ASSERT((nNr>0) && (nNr<17));
	
	dwNewMask = m_dwAlarmMask;

	if (bUndo)
		dwNewMask &= ~(1<<(nNr-1));
	else
		dwNewMask |= 1<<(nNr-1);
	
	dwNewMask = dwNewMask & m_dwEnableMask;

	// m_dwAlarmMask ist the state of the buttons
	DWORD dwInputMask = dwNewMask^m_dwEdgeMask;
	DWORD dwChangedMask = (dwNewMask^m_dwEdgeMask)^(m_dwAlarmMask^m_dwEdgeMask);
	DoIndicateAlarmState(m_wGroupID, dwInputMask, dwChangedMask);

	m_dwAlarmMask = dwNewMask;
	if (   m_bAnyAlarm
		&& (m_dwAlarmMask == 0)
		)
	{
		m_bAnyAlarm = FALSE;
		WK_TRACE(_T("Alarmmask %08x ; All should be quiet...\n"), m_dwAlarmMask);
	}
	else if (m_bAnyAlarm == FALSE)
	{
		m_bAnyAlarm = TRUE;
		WK_TRACE(_T("Alarmmask %08x ; Someone always disturbs...\n"), m_dwAlarmMask);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSimUnit::OnRequestAlarmState(WORD wGroupID)
{
	DoConfirmAlarmState(m_wGroupID, m_dwAlarmMask^m_dwEdgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// Disconnecting code
//////////////////////////////////////////////////////////////////////////////////////
// disconnecting method call initiated by Security, while Security shut's down
// add disconnecting code here
//
void CIPCInputSimUnit::OnRequestDisconnect()
{
	MY_POST(ID_DISCONNECT_INPUT,0);
}
//////////////////////////////////////////////////////////////////////////////////////
void CIPCInputSimUnit::OnRequestVersionInfo(WORD wGroupID)
{
	// 1 CIPC default
	// 2 supports VersionInfo
	// 3 supports EdgeMask (OpenMask)
	// 4 supports 16 alarms
	DoConfirmVersionInfo(wGroupID, 4);
}
