// CIPCInputSimUnit.h
// w+k Sample Application
// Author : Uwe Freiwald

// Definition of CIPCInputSimUnit
// 
// The CIPCInputSimUnit performs all input communication and connection to Security Unit.
// Input means input from the view of Security unit. For SimUnit it's more output to Security.

// To write your own CIPCInput's, derive your own object from CIPCInput class and overwrite
// several virtual methods to implement your input application.
// You should create one instance of CIPCInputSimUnit per application, see CSimUnitApp.
// It' necessary to define a unique(!) so called shared memory name for every instance
// of CIPCInputSimUnit instance, pass this string to the constructor.
// You'll find more explainations in the CIPCInputSimUnit.cpp file of CIPCInputSimUnit class.

#ifndef _CIPCInputSimUnit_H_
#define _CIPCInputSimUnit_H_

#include "CipcInput.h"

class CIPCInputSimUnit : public CIPCInput
{
	// construction / destruction
public:
	CIPCInputSimUnit(LPCTSTR shmName);
	~CIPCInputSimUnit();
	// Attributes
	WORD GetGroupID() { return m_wGroupID; };
	// member variables access
	void	IndicateAlarm(int nNr, BOOL bUndo = FALSE);

	// overridables
public:	
	// requests
	virtual void OnRequestConnection();

	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual void OnRequestSetGroupID(WORD wGroupID);
	virtual void OnRequestHardware(WORD wGroupID);
	virtual void OnRequestSetEdge(WORD wGroupID,DWORD edgeMask);	// 1 positive, 0 negative
	virtual void OnRequestSetFree(WORD wGroupID,DWORD openMask);	// 1 open, 0 closed
	virtual void OnRequestReset(WORD wGroupID);	

	virtual void OnRequestAlarmState(WORD wGroupID);

	virtual void OnRequestDisconnect();
	// private data
private:
	WORD		m_wGroupID;

	// alert status variables
	BOOL	m_bAnyAlarm;
	DWORD	m_dwAlarmMask;
	DWORD	m_dwEnableMask;
	DWORD	m_dwEdgeMask;
};

#endif	// _CIPCInputSimUnit_H_