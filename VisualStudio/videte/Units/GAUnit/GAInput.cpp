// CGAInput.cpp
// Author : Uwe Freiwald

// Implementation of CGAInput
// 

#include "stdafx.h"
#include "GAUnit.h"
#include "GAInput.h"
//////////////////////////////////////////////////////////////////////////////////////
// Constructor
CGAInput::CGAInput(const char *shmName)
	: CIPCInput(shmName,FALSE)
{
	m_wGroupID = SECID_NO_GROUPID;
	m_OldMask = theApp.GetAlarmMask();
	StartThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Destructor
CGAInput::~CGAInput()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////////////////////
// Initializing and connection code
//////////////////////////////////////////////////////////////////////////////////////
// 1. called overridden method
//
//////////////////////////////////////////////////////////////////////////////////////
// 2. called overridden method
//
void CGAInput::OnRequestSetGroupID(WORD wGroupID)
{
	m_wGroupID = wGroupID; 
	DoConfirmSetGroupID(wGroupID);
};
//////////////////////////////////////////////////////////////////////////////////////
// 3. called overridden method
//
void CGAInput::OnRequestHardware(WORD wGroupNr)
{
	int eCode;
	eCode = theApp.OnConnect();
	DoConfirmHardware(m_wGroupID,eCode);
}
//////////////////////////////////////////////////////////////////////////////////////
// 4. called overridden method
//
void CGAInput::OnRequestSetEdge(WORD wGroupID,DWORD edgeMask)	// 1 positive, 0 negative
{
	DoConfirmEdge(m_wGroupID, edgeMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 5. called overridden method
//
void CGAInput::OnRequestSetFree(WORD wGroupID,DWORD openMask)	// 1 open, 0 closed
{
	DoConfirmFree(m_wGroupID, openMask);
}
//////////////////////////////////////////////////////////////////////////////////////
// 5. called overridden method
//
void CGAInput::OnRequestReset(WORD wGroupID)
{
	DoConfirmReset(m_wGroupID);
}
//////////////////////////////////////////////////////////////////////////////////////
// Running Application code
//////////////////////////////////////////////////////////////////////////////////////
void CGAInput::OnRequestAlarmState(WORD wGroupID)
{
	DoConfirmAlarmState(m_wGroupID,theApp.GetAlarmMask());
}
//////////////////////////////////////////////////////////////////////////////////////
// Disconnecting code
//
void CGAInput::OnRequestDisconnect()
{
	theApp.OnDisconnect();
}
//////////////////////////////////////////////////////////////////////////////////////
void CGAInput::IndicateAlarm(DWORD dwMask, LPCSTR lpszAlarmString)
{
	CIPCFields fields;
	fields.FromString(lpszAlarmString);
	CString sCurrency = theApp.GetCurrency();
	if (!sCurrency.IsEmpty())
	{
		fields.SafeAdd(new CIPCField(CIPCField::m_sfCurr,sCurrency,3,'C'));
	}
	DoIndicateAlarmFieldsState(m_wGroupID,dwMask,dwMask^m_OldMask,fields);
	m_OldMask = dwMask;
}
//////////////////////////////////////////////////////////////////////////////////////
void CGAInput::UpdateAlarm(WORD wSubID, LPCSTR lpszAlarmString)
{
	CSecID id(m_wGroupID,wSubID);
	CIPCFields fields;
	fields.FromString(lpszAlarmString);
	CString sCurrency = theApp.GetCurrency();
	if (!sCurrency.IsEmpty())
	{
		fields.SafeAdd(new CIPCField(CIPCField::m_sfCurr,sCurrency,3,'C'));
	}
	DoIndicateAlarmFieldsUpdate(id,fields);
}
